set(DEPS boost_filesystem tbb 
             tbbmalloc tbbmalloc_proxy 
             mkl_intel_lp64 igl::core 
             mkl_sequential mkl_core  
             suitesparseconfig metis cholmod amd camd ccolamd colamd spqr 
             gmp mpfr)

file(GLOB HEADERS "include/*.h" "autodiff/*.h")
file(GLOB SOURCES "src/*.cpp" "autodiff/*.cpp")

add_executable(ParametericYarn ${HEADERS} ${SOURCES})
target_include_directories(ParametericYarn PUBLIC ../../Libs/spectra/include)
target_include_directories(ParametericYarn PUBLIC ../../Deps/libigl/include)
target_link_libraries(ParametericYarn ${DEPS} polyscope)