{
  description = "Masses and springs simulation game";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";
    nix.inputs.nixpkgs.follows = "nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "gitlab:grumbel/cmake-modules";
    tinycmmc.inputs.nix.follows = "nix";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    priocpp.url = "gitlab:grumbel/priocpp";
    priocpp.inputs.nix.follows = "nix";
    priocpp.inputs.nixpkgs.follows = "nixpkgs";
    priocpp.inputs.flake-utils.follows = "flake-utils";
  };

  outputs = { self, nix, nixpkgs, flake-utils, tinycmmc, priocpp }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
       in rec {
         packages = flake-utils.lib.flattenTree rec {
           construo = pkgs.stdenv.mkDerivation {
             name = "construo";
             src = nixpkgs.lib.cleanSource ./.;
             cmakeFlags = [];
             postFixup = ''
               wrapProgram $out/bin/construo.glut \
                  --prefix LIBGL_DRIVERS_PATH ":" "${pkgs.mesa.drivers}/lib/dri" \
                  --prefix LD_LIBRARY_PATH ":" "${pkgs.mesa.drivers}/lib"
            '';
             nativeBuildInputs = [
               pkgs.cmake
               pkgs.gcc
               pkgs.git
               pkgs.makeWrapper
               pkgs.ninja
               pkgs.pkgconfig
             ];
             buildInputs = [
               priocpp.defaultPackage.${system}

               pkgs.zlib
               pkgs.xorg.libX11
               pkgs.freeglut
               pkgs.libGL
               pkgs.libGLU
             ];
           };
        };
        defaultPackage = packages.construo;
      });
}
