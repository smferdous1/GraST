#executable app_streamio
add_executable(stio appStreamIO.cc)
target_compile_features(stio PRIVATE cxx_std_17)
target_compile_options(stio PRIVATE -O3 )
target_link_libraries(stio PRIVATE gstream)
target_include_directories(stio PRIVATE "${PROJECT_SOURCE_DIR}/include")

#executable app_graphproperties
add_executable(gp appGraphProperties.cc)
target_compile_features(gp PRIVATE cxx_std_17)
target_compile_options(gp PRIVATE -O3 )
target_link_libraries(gp PRIVATE gstream)
target_include_directories(gp PRIVATE "${PROJECT_SOURCE_DIR}/include")

#executable app_streammatch
add_executable(stmatch appStreamMatch.cc)
target_compile_features(stmatch PRIVATE cxx_std_17)
target_compile_options(stmatch PRIVATE -O3 )
target_link_libraries(stmatch PRIVATE gstream)
target_include_directories(stmatch PRIVATE "${PROJECT_SOURCE_DIR}/include")

#executable app_ldstreammatch
add_executable(ldstmatch appLdStreamMatch.cc)
target_compile_features(ldstmatch PRIVATE cxx_std_17)
target_compile_options(ldstmatch PRIVATE -O3 )
target_link_libraries(ldstmatch PRIVATE gstream)
target_include_directories(ldstmatch PRIVATE "${PROJECT_SOURCE_DIR}/include")

#executable app_grdymatch
add_executable(grdymatch appGrdyMatch.cc)
target_compile_features(grdymatch PRIVATE cxx_std_17)
target_compile_options(grdymatch PRIVATE -O3 )
target_link_libraries(grdymatch PRIVATE gstream)
target_include_directories(grdymatch PRIVATE "${PROJECT_SOURCE_DIR}/include")


#executable app_stec
add_executable(stec appStreamEC.cc)
target_compile_features(stec PRIVATE cxx_std_17)
target_compile_options(stec PRIVATE -O3 )
target_link_libraries(stec PRIVATE gstream)
target_include_directories(stec PRIVATE "${PROJECT_SOURCE_DIR}/include")

#executable app_nnec
add_executable(nnec appNN.cc)
target_compile_features(nnec PRIVATE cxx_std_17)
target_compile_options(nnec PRIVATE -O3 )
target_link_libraries(nnec PRIVATE gstream)
target_include_directories(nnec PRIVATE "${PROJECT_SOURCE_DIR}/include")

#executable app_twopassec
add_executable(twopassec appStreamTwoPassEC.cc)
target_compile_features(twopassec PRIVATE cxx_std_17)
target_compile_options(twopassec PRIVATE -O3 )
target_link_libraries(twopassec PRIVATE gstream)
target_include_directories(twopassec PRIVATE "${PROJECT_SOURCE_DIR}/include")

#executable app_ECPD
add_executable(ecpd appStreamECPD.cc)
target_compile_features(ecpd PRIVATE cxx_std_17)
target_compile_options(ecpd PRIVATE -O3 )
target_link_libraries(ecpd PRIVATE gstream)
target_include_directories(ecpd PRIVATE "${PROJECT_SOURCE_DIR}/include")

#executable app_pdthreehalf
add_executable(pdthreehalf appPDEdgecover.cc)
target_compile_features(pdthreehalf PRIVATE cxx_std_17)
target_compile_options(pdthreehalf PRIVATE -O3 )
target_link_libraries(pdthreehalf PRIVATE gstream)
target_include_directories(pdthreehalf PRIVATE "${PROJECT_SOURCE_DIR}/include")

#executable app_permute
add_executable(permin appPermuteInput.cc)
target_compile_features(permin PRIVATE cxx_std_17)
target_compile_options(permin PRIVATE -O3 )
target_link_libraries(permin PRIVATE gstream)
target_include_directories(permin PRIVATE "${PROJECT_SOURCE_DIR}/include")

#executable app_writeOmtx
add_executable(writeomtx appWriteOMtx.cc)
target_compile_features(writeomtx PRIVATE cxx_std_17)
target_compile_options(writeomtx PRIVATE -O3 )
target_link_libraries(writeomtx PRIVATE gstream)
target_include_directories(writeomtx PRIVATE "${PROJECT_SOURCE_DIR}/include")

#executable appPostProcessing
add_executable(postprocess appPostProcesing.cc)
target_compile_features(postprocess PRIVATE cxx_std_17)
target_compile_options(postprocess PRIVATE -O3 )
target_link_libraries(postprocess PRIVATE gstream)
target_include_directories(postprocess PRIVATE "${PROJECT_SOURCE_DIR}/include")

