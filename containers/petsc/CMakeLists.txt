cmake_minimum_required(VERSION 3.16)

find_package(PkgConfig REQUIRED)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

project(detect_compilers LANGUAGES)

set(petsc_root "$ENV{PETSC_DIR}")
if(NOT "$ENV{PETSC_ARCH}" STREQUAL "")
  string(APPEND petsc_root "/$ENV{PETSC_ARCH}")
endif()
set(ENV{PKG_CONFIG_PATH} "${petsc_root}/lib/pkgconfig")

pkg_get_variable(petsc_cxx PETSc cxxcompiler)
if(NOT petsc_cxx)
  pkg_get_variable(petsc_cxx petsc cxxcompiler)
endif()
if(petsc_cxx)
  set(CMAKE_CXX_COMPILER "${petsc_cxx}")
endif()

project(wavein_petsc_smoke LANGUAGES CXX)
pkg_search_module(PETSC REQUIRED IMPORTED_TARGET PETSc petsc)

add_executable(wavein-petsc-smoke smoke.cpp)
target_compile_features(wavein-petsc-smoke PRIVATE cxx_std_17)
target_link_libraries(wavein-petsc-smoke PRIVATE PkgConfig::PETSC)
install(TARGETS wavein-petsc-smoke RUNTIME DESTINATION bin)
