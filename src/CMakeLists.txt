set(HEADER_LIST
    "${PROJECT_SOURCE_DIR}/include/GraST/Graph.h"
    "${PROJECT_SOURCE_DIR}/include/GraST/StreamIn.h"
    "${PROJECT_SOURCE_DIR}/include/GraST/StreamMatch.h"
    "${PROJECT_SOURCE_DIR}/include/GraST/StreamkMatch.h"
    "${PROJECT_SOURCE_DIR}/include/GraST/StreamkDM.h"
    "${PROJECT_SOURCE_DIR}/include/GraST/Match.h"
    "${PROJECT_SOURCE_DIR}/include/GraST/MisraGries.h"
    "${PROJECT_SOURCE_DIR}/include/GraST/Utility.h"
    )

# Make an automatic library - will be static or dynamic based on user setting
add_library(gstream
  StreamIn.cc
  StreamMatch.cc
  StreamkMatch.cc
  StreamkDM.cc
  dpPath.cc
  LdStreamMatch.cc
  StreamEC.cc
  ldMatchStandard.cc
  ldMatchTwoPhase.cc
  ldMatchTwoPhasePar.cc
  greedyMatch.cc
  MisraGries.cc
  Utility.cc
  primalDualEC.cc
  ${HEADER_LIST}
  )

find_package(OpenMP)

if(OpenMP_CXX_FOUND)
  target_link_libraries(gstream PUBLIC OpenMP::OpenMP_CXX)
endif()

target_include_directories(gstream PUBLIC
  "${PROJECT_SOURCE_DIR}/include"
  )

# All users of this library will need at least C++17
target_compile_features(gstream PUBLIC cxx_std_17)
target_compile_options(gstream PRIVATE -O3)
