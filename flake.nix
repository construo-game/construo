{
  description = "Masses and springs simulation game";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "github:grumbel/tinycmmc";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    logmich.url = "github:logmich/logmich";
    logmich.inputs.nixpkgs.follows = "nixpkgs";
    logmich.inputs.tinycmmc.follows = "tinycmmc";

    sexpcpp.url = "github:lispparser/sexp-cpp";
    sexpcpp.inputs.nixpkgs.follows = "nixpkgs";
    sexpcpp.inputs.flake-utils.follows = "flake-utils";
    sexpcpp.inputs.tinycmmc.follows = "tinycmmc";

    priocpp.url = "github:grumbel/priocpp";
    priocpp.inputs.nixpkgs.follows = "nixpkgs";
    priocpp.inputs.flake-utils.follows = "flake-utils";
    priocpp.inputs.sexpcpp.follows = "sexpcpp";
    priocpp.inputs.tinycmmc.follows = "tinycmmc";
    priocpp.inputs.logmich.follows = "logmich";

    geomcpp.url = "github:grumbel/geomcpp";
    geomcpp.inputs.nixpkgs.follows = "nixpkgs";
    geomcpp.inputs.tinycmmc.follows = "tinycmmc";

    xdgcpp.url = "github:grumbel/xdgcpp";
    xdgcpp.inputs.nixpkgs.follows = "nixpkgs";
    xdgcpp.inputs.flake-utils.follows = "flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc, logmich, sexpcpp, priocpp, geomcpp, xdgcpp }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        version_file = pkgs.lib.fileContents ./VERSION;
        construo_has_version = ((builtins.substring 0 1) version_file) == "v";
        construo_version = if !construo_has_version
                           then ("0.2.3-${nixpkgs.lib.substring 0 8 self.lastModifiedDate}-${self.shortRev or "dirty"}")
                           else (builtins.substring 1 ((builtins.stringLength version_file) - 2) version_file);
       in {
         packages = rec {
           default = construo;

           construo = pkgs.stdenv.mkDerivation rec {
             pname = "construo";
             version = construo_version;

             src = ./.;

             postPatch = ''
                if ${if construo_has_version then "false" else "true"}; then
                  echo "${version}" > VERSION
                fi
                substituteInPlace CMakeLists.txt \
                  --replace "appstream-util" "appstream-util --nonet"
             '';

             cmakeFlags = [
               "-DWARNINGS=ON"
               "-DWERROR=ON"
               "-DBUILD_TESTS=ON"
             ];

             doCheck = true;

             postFixup = ''
               ln -s $out/bin/construo.x11 $out/bin/construo
             '';

             nativeBuildInputs = with pkgs; [
               cmake
               pkg-config
             ];

             buildInputs = with pkgs; [
               fmt_8
               freeglut
               glm
               gtest
               libGL
               libGLU
               xorg.libX11
               zlib
               libsigcxx30
             ] ++ [
               geomcpp.packages.${system}.default
               logmich.packages.${system}.default
               priocpp.packages.${system}.default
               tinycmmc.packages.${system}.default
               xdgcpp.packages.${system}.default
             ];
           };
        };
       }
    );
}
