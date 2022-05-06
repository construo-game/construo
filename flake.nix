{
  description = "Masses and springs simulation game";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-21.11";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "gitlab:grumbel/cmake-modules";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    logmich.url = "gitlab:logmich/logmich";
    logmich.inputs.nixpkgs.follows = "nixpkgs";
    logmich.inputs.flake-utils.follows = "flake-utils";
    logmich.inputs.tinycmmc.follows = "tinycmmc";

    sexpcpp.url = "gitlab:lispparser/sexp-cpp";
    sexpcpp.inputs.nixpkgs.follows = "nixpkgs";
    sexpcpp.inputs.flake-utils.follows = "flake-utils";
    sexpcpp.inputs.tinycmmc.follows = "tinycmmc";

    priocpp.url = "gitlab:grumbel/priocpp";
    priocpp.inputs.nixpkgs.follows = "nixpkgs";
    priocpp.inputs.flake-utils.follows = "flake-utils";
    priocpp.inputs.sexpcpp.follows = "sexpcpp";
    priocpp.inputs.tinycmmc.follows = "tinycmmc";
    priocpp.inputs.logmich.follows = "logmich";

    xdgcpp.url = "gitlab:grumbel/xdgcpp";
    xdgcpp.inputs.nixpkgs.follows = "nixpkgs";
    xdgcpp.inputs.flake-utils.follows = "flake-utils";
    xdgcpp.inputs.tinycmmc.follows = "tinycmmc";
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc, logmich, sexpcpp, priocpp, xdgcpp }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        version_file = pkgs.lib.fileContents ./VERSION;
        construo_has_version = ((builtins.substring 0 1) version_file) == "v";
        construo_version = if !construo_has_version
                           then ("0.2.3-${nixpkgs.lib.substring 0 8 self.lastModifiedDate}-${self.shortRev or "dirty"}")
                           else (builtins.substring 1 ((builtins.stringLength version_file) - 2) version_file);
       in rec {
         packages = flake-utils.lib.flattenTree rec {
           construo = pkgs.stdenv.mkDerivation rec {
             pname = "construo";
             version = construo_version;
             src = nixpkgs.lib.cleanSource ./.;
             postPatch = ''
                if ${if construo_has_version then "false" else "true"}; then
                  echo "${version}" > VERSION
                fi
                substituteInPlace CMakeLists.txt \
                  --replace "appstream-util" "appstream-util --nonet"
             '';
             cmakeFlags = [
               "-DWARNINGS=ON"
               # "-DWERROR=ON"
               "-DBUILD_TESTS=ON"
             ];
             doCheck = true;
             postFixup = ''
               wrapProgram $out/bin/construo.glut \
                  --prefix LIBGL_DRIVERS_PATH ":" "${pkgs.mesa.drivers}/lib/dri" \
                  --prefix LD_LIBRARY_PATH ":" "${pkgs.mesa.drivers}/lib"
               ln -s $out/bin/construo.x11 $out/bin/construo
             '';
             nativeBuildInputs = with pkgs; [
               cmake
               makeWrapper
               pkgconfig
             ];
             checkInputs = with pkgs; [
               appstream-glib
             ];
             buildInputs = with pkgs; [
               zlib
               xorg.libX11
               freeglut
               libGL
               libGLU
             ] ++ [
               tinycmmc.defaultPackage.${system}
               priocpp.defaultPackage.${system}
               xdgcpp.defaultPackage.${system}
             ];
           };
        };
        defaultPackage = packages.construo;
      });
}
