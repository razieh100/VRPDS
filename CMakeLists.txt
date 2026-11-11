cmake_minimum_required(VERSION 3.24)
project(vrpds LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default to an optimized build
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# ---------- CPLEX location ----------
# Pass -DCPLEX_STUDIO_DIR=/path/to/CPLEX_Studio, or set the CPLEX_STUDIO_DIR
# environment variable. On Compute Canada clusters (e.g. Narval), the loaded
# CPLEX module's $EBROOTCPLEX is used automatically.
if(NOT CPLEX_STUDIO_DIR)
  if(DEFINED ENV{CPLEX_STUDIO_DIR})
    set(CPLEX_STUDIO_DIR $ENV{CPLEX_STUDIO_DIR})
  elseif(DEFINED ENV{EBROOTCPLEX})
    set(CPLEX_STUDIO_DIR $ENV{EBROOTCPLEX})
  else()
    message(FATAL_ERROR "CPLEX not found. Use -DCPLEX_STUDIO_DIR=/path/to/CPLEX_Studio")
  endif()
endif()

# ---------- Platform detection ----------
if(APPLE)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
    set(CPLEX_SYSTEM arm64_osx)
  else()
    set(CPLEX_SYSTEM x86-64_osx)
  endif()
elseif(UNIX)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
    set(CPLEX_SYSTEM arm64_linux)
  else()
    set(CPLEX_SYSTEM x86-64_linux)
  endif()
else()
  message(FATAL_ERROR "Unsupported platform (macOS and Linux only)")
endif()
message(STATUS "CPLEX: ${CPLEX_STUDIO_DIR} (${CPLEX_SYSTEM})")

# ---------- Executable ----------
add_executable(vrpds
  main.cpp Data.cpp Solution.cpp Solver.cpp Parameters.cpp
  Constraints.cpp LoadVrp.cpp ListMoveRvrp.cpp SeqInsertionVrp.cpp
  InsRmvMethodDistanceVrp.cpp InsRmvMethodShift.cpp InsRmvMethodOpt.cpp
  InsRmvMethodPerturb.cpp Opt2StarTwCost.cpp SolveAssignmentProblem.cpp
  waltime.c
)

target_include_directories(vrpds PRIVATE
  ${CPLEX_STUDIO_DIR}/concert/include
  ${CPLEX_STUDIO_DIR}/cplex/include
)

target_link_directories(vrpds PRIVATE
  ${CPLEX_STUDIO_DIR}/concert/lib/${CPLEX_SYSTEM}/static_pic
  ${CPLEX_STUDIO_DIR}/cplex/lib/${CPLEX_SYSTEM}/static_pic
)

target_compile_definitions(vrpds PRIVATE IL_STD)

find_package(Threads REQUIRED)
target_link_libraries(vrpds PRIVATE ilocplex concert cplex Threads::Threads m ${CMAKE_DL_LIBS})

if(APPLE)
  target_link_options(vrpds PRIVATE -Wl,-no_compact_unwind)
endif()
