cmake_minimum_required(VERSION 3.24)
project(vrpd3Feb)

set(CMAKE_CXX_STANDARD 17)



set(SYSTEM arm64_osx)
set(CONCERT_INCLUDE_DIRS ${CPLEX_STUDIO_DIR}/concert/include)
set(CONCERT_LIB_DIRS ${CPLEX_STUDIO_DIR}/concert/lib/${SYSTEM}/static_pic)
set(CPLEX_INCLUDE_DIRS ${CPLEX_STUDIO_DIR}/cplex/include)
set(CPLEX_LIB_DIRS ${CPLEX_STUDIO_DIR}/cplex/lib/${SYSTEM}/static_pic)
include_directories(${CONCERT_INCLUDE_DIRS} ${CPLEX_INCLUDE_DIRS} )
link_directories(${CONCERT_LIB_DIRS} ${CPLEX_LIB_DIRS} )
#END CPLEX CONFIG

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++17 -stdlib=libc++ -DIL_STD -Wl,-no_compact_unwind -lm -lpthread")


add_executable(vrpd3Feb main.cpp Data.cpp Data.h Node.h Driver.h Solution.cpp Solution.h Solver.cpp Solver.h Parameters.cpp Parameters.h InsertOperator.h InsRmvMethod.h InsRmvMethodDistanceVrp.cpp SeqInsertionVrp.h SeqInsertionVrp.cpp Constraints.cpp
         LoadVrp.cpp ListMoveRvrp.cpp Relocate.h  InsRmvMethodShift.cpp InsRmvMethodShift.h Shift.h  InsRmvMethodOpt.h InsRmvMethodOpt.cpp  Opt2StarTw.h   Opt2StarTwCost.cpp Opt2StarTwCost.h SolveAssignmentProblem.cpp SolveAssignmentProblem.h RelocateChain.h CrossExchangeTw.h MoveChain.h Move2Chain.h MoveDepot.h waltime.c waltime.h Perturb.h InsRmvMethodPerturb.cpp InsRmvMethodPerturb.h InsRmvMethodP.h  Intra.h )

