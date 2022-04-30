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
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc, logmich, sexpcpp, priocpp }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
       in rec {
         packages = flake-utils.lib.flattenTree rec {
           construo = pkgs.stdenv.mkDerivation {
             name = "construo";
             src = nixpkgs.lib.cleanSource ./.;
             postFixup = ''
               wrapProgram $out/bin/construo.glut \
                  --prefix LIBGL_DRIVERS_PATH ":" "${pkgs.mesa.drivers}/lib/dri" \
                  --prefix LD_LIBRARY_PATH ":" "${pkgs.mesa.drivers}/lib"
            '';
             nativeBuildInputs = with pkgs; [
               cmake
               makeWrapper
               pkgconfig
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
             ];
           };
        };
        defaultPackage = packages.construo;
      });
}
