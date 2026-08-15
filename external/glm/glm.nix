{ stdenv, lib }:

stdenv.mkDerivation {
  pname = "glm";
  version = "1.0.1";

  src = lib.cleanSource ./.;

  # Header-only: install headers + a minimal CMake package config.
  dontConfigure = true;
  dontBuild = true;

  installPhase = ''
    runHook preInstall
    mkdir -p $out/include $out/lib/cmake/glm
    cp -a glm $out/include/
    cat > $out/lib/cmake/glm/glmConfig.cmake <<'EOF'
set(_glm_inc "''${CMAKE_CURRENT_LIST_DIR}/../../../include")
if(NOT TARGET glm::glm)
  add_library(glm::glm INTERFACE IMPORTED)
  set_target_properties(glm::glm PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "''${_glm_inc}")
endif()
set(glm_FOUND TRUE)
set(GLM_INCLUDE_DIRS "''${_glm_inc}")
EOF
    cat > $out/lib/cmake/glm/glmConfigVersion.cmake <<'EOF'
set(PACKAGE_VERSION "1.0.1")
set(PACKAGE_VERSION_COMPATIBLE TRUE)
set(PACKAGE_VERSION_EXACT TRUE)
EOF
    runHook postInstall
  '';

  meta = with lib; {
    description = "OpenGL Mathematics (header-only, vendored)";
    homepage = "https://github.com/g-truc/glm";
    license = licenses.mit;
    platforms = platforms.all;
  };
}
