/*
 * Copyright Razieh Mousavi
 *
 * The code may be used for academic, non-commercial purposes only.
 *
 * Please contact me at razieh.mousavi.1@ulaval.ca for questions
 *
 * If you have improvements, please contact me!
 *
*/
#include "Solver.h"
#include "SeqInsertionVrp.h"
#include "InsRmvMethodDistanceVrp.h"
#include "InsRmvMethodShift.h" //similar to vrp
#include "Intra.h"
#include "Relocate.h"
#include "SolveAssignmentProblem.h"
#include "RelocateChain.h"
#include "InsRmvMethodPerturb.h" // similar to vrp
#include "Perturb.h"
#include "Opt2StarTw.h"
#include "CrossExchangeTw.h"
#include <fstream>
#include <iomanip>
#include <filesystem>

void Solver::Run(Data &data, std::string instanceName) {
    Sol sol(&data);
    Sol Best(&data);

    sol.PutAllCustomersToUnassigned();
    InsRmvMethodDistance insRmv(data);
    SeqInsertion seqInsertion(data, insRmv);

    double TRelocate = 0, TIntra = 0, TRelocateC = 0, T2Opt = 0, TEx = 0, TAssign = 0;
    double NumberRelocate = 0, NumberIntra = 0, NumberRelocateC = 0, Number2Opt = 0, NumberEx = 0, NumberAssign = 0;

    double start = get_cpu_time();

    seqInsertion.Insert(sol);

    sol.AssignBestShift();
    Best = sol; // initial solution

    sol.Show(1);
    sol.Show(2);

//std::cin.get();
    sol.Show(1);
    double InitialTime = get_cpu_time() - start;// time needed to generate the initial solution
    sol.duration_time = InitialTime;

    double initialCost = sol.GetCost(1);
    double initialShiftCost = sol.GetShiftCost(1);
    double initialTravelCost = sol.GetTravelCost();
    double initial3PLCost = sol.GetTplCost();
    int initialNumCust3Pl = sol.GetUnassignedCustCount();

    int initialCusRouteAverage = 0;
    int initialMinNumCust = 10000;
    int initialMaxNumCust = 0;
    int UsedDriver = 0;
    double maxCapacityUsed = 0;
    double sumCapacity =0;
    int r =0;


    for (int i = 0; i < sol.GetDriverCount(); i++) {
        if (sol.RoutesLength[i] >= 1) {
            UsedDriver++;
            if (sol.RoutesLength[i] < initialMinNumCust)
                initialMinNumCust = sol.RoutesLength[i];
            if (sol.RoutesLength[i] > initialMaxNumCust)
                initialMaxNumCust = sol.RoutesLength[i];
        }
        initialCusRouteAverage = (sol.GetCustomerCount() - sol.GetUnassignedCustCount()) / UsedDriver;
    }

    int initialWaiting = sol.CalculateTotalAvailabilityWaitingTime();

    int Average = 0;



    for ( r = 1; r < 2500; r++) {
        double currentTime = get_cpu_time() - start;
        if (currentTime >= 3600) {
            std::cout << "Time limit reached. Stopping optimization." << std::endl;
            break;
        }

        if (r > 1.5) {//perturbation phase
            InsRmvMethodPerturb insRmvPrt(data);
            Perturb<Node, Driver, MoveVrp> RemoveInsert(&insRmvPrt);
          //00000
          RemoveInsert.Optimize(sol);
            sol.Show(1);
        }

        sol.Show(1);
        double prevBestCost = Best.GetCost(1);//for count improve

        double before0 = sol.GetCost(1);
        double RTT1 = get_cpu_time();

        Intra<Node, Driver> Intra;
       //111111
       Intra.Optimize(sol);

        if (sol.GetCost(1) < Best.GetCost(2)) {
            sol.AssignBestShift();
            Best = sol;
        }
        std::cout << " ---------------------------After Intra " << r << std::endl;

        double RTT2 = get_cpu_time();
        TIntra = TIntra + (RTT2 - RTT1);// this is the sum of time spent in this local search

        double next0 = sol.GetCost(1);
        if (next0< before0){NumberIntra++;}
        //PercentIntra = PercentIntra + ((-next11 + before) / before);

        double TIntraaa = get_cpu_time() - start;// this is to save the time in which the best solution is found

        if (sol.GetCost(1) < Best.GetCost(2)) {
            sol.duration_time = TIntraaa;
            sol.bestItr =r;
            sol.AssignBestShift();
            Best = sol;
        }

        sol.Show(1);
        std::cout << " End of Intra" << std::endl;

        InsRmvMethodShift insRmvShift(data);
        Relocate<Node, Driver, MoveVrp> relocateS(&insRmvShift);

        double before1 = sol.GetCost(1);
        double RT1 = get_cpu_time();

       //22222
       relocateS.Optimize(sol);

        std::cout << " ---------------------------After relocate " << r << std::endl;

        double RT2 = get_cpu_time();
        TRelocate = TRelocate + (RT2 - RT1);

        double next1 = sol.GetCost(1);
        if (next1< before1){NumberRelocate++;}
        //PercentRelocate = PercentRelocate + ((-next1 + before1) / before1);

        double RelocateNTime = get_cpu_time() - start;

        if (sol.GetCost(1) < Best.GetCost(2)) {
            sol.duration_time = RelocateNTime;
            sol.bestItr =r;
            sol.AssignBestShift();
            Best = sol;
        }

        sol.Show(1);
        std::cout << " End of Relocate" << std::endl;

       RelocateChain relocateChain;

        double before2 = sol.GetCost(1);
        double RTC1 = get_cpu_time();

      //33333
      relocateChain.Optimize(sol);

        double RTC2 = get_cpu_time();
        TRelocateC = TRelocateC + (RTC2 - RTC1);

        double next2 = sol.GetCost(1);
        if (next2< before2){NumberRelocateC++;}
        // PercentRelocateC = PercentRelocateC + ((-next2 + before2) / before2);

        double RelocateCTime = get_cpu_time() - start;
        if (sol.GetCost(1) < Best.GetCost(2)) {
            sol.duration_time = RelocateCTime;
            sol.bestItr =r;
            sol.AssignBestShift();
            Best = sol;
        }
        std::cout << " End of relocateChain" << std::endl;

        sol.Show(1);

        Opt2StarTw Opt2STw;

        double before3 = sol.GetCost(1);
        double OptTC1 = get_cpu_time();

        //44444444
        Opt2STw.Optimize(sol);

        double OptTC2 = get_cpu_time();
        T2Opt = T2Opt + (OptTC2 - OptTC1);

        double next3 = sol.GetCost(1);
        if (next3< before3){Number2Opt++;}
        // Percent2Opt = Percent2Opt + ((-next3 + before3) / before3);

        double OptTime = get_cpu_time() - start;
        if (sol.GetCost(1) < Best.GetCost(2)) {
            sol.duration_time = OptTime;
            sol.bestItr =r;
            sol.AssignBestShift();
            Best = sol;
        }
        std::cout << " End of 2Opt" << std::endl;

        sol.Show(1);

        CrossExchangeTw CrossEx;

        double before4 = sol.GetCost(1);
        double Ex1 = get_cpu_time();

        //5555555
        CrossEx.Optimize(sol);


        double Ex2 = get_cpu_time();
        TEx = TEx + (Ex2 - Ex1);

        double next4 = sol.GetCost(1);
        if (next4< before4){NumberEx++;}
        // PercentEx = PercentEx + ((-next4 + before4) / before4);

        double ExTime = get_cpu_time() - start;
        if (sol.GetCost(1) < Best.GetCost(2)) {
            sol.duration_time = ExTime;
            sol.bestItr =r;
            sol.AssignBestShift();
            Best = sol;
        }
        std::cout << " End of CrossExchange" << std::endl;





        double before5 = sol.GetCost(1);
        double As1 = get_cpu_time();
        sol.Show(1);
       //6666666
       AssignmentProblemSolver::SolveAssignment(sol);

        double As2 = get_cpu_time();
        TAssign = TAssign + (As2 - As1);

        double next5 = sol.GetCost(1);
        if (next5< before5){NumberAssign++;}
        // PercentAssign = PercentAssign + ((-next5 + before5) / before5);

        std::cout << " End of Assignment" << std::endl;
        double Assignime = get_cpu_time() - start;
        if (sol.GetCost(1) < Best.GetCost(2)) {
            sol.duration_time = Assignime;
            sol.bestItr =r;
            sol.AssignBestShift();
            Best = sol;
        }
        sol.Show(1);

        double x = sol.GetCost(1);
        Average = ((Average * (r - 1)) + x) / r;

// **🔹 Track No Improvement Here**
       // double prevBestCost = Best.GetCost(2);
        double currentCost = sol.GetCost(1);

        if (currentCost < prevBestCost) {
            Parameters::CountNoImprove = 0;  // ✅ Reset if cost improves
        } else if (std::abs(prevBestCost - currentCost) < EPS) {
            Parameters::CountNoImprove++;  // ✅ Increment if cost is unchanged
        } else {
            Parameters::CountNoImprove = 0;  // ✅ Reset if cost worsens
        }

        if (Best.GetCost(2) > sol.GetCost(1)) {
            sol.AssignBestShift();
            Best = sol;
        }
    }



    Best.Show(2);

    double BestCost = Best.GetCost(2);
    double BestShiftCost = Best.GetShiftCost(2);
    double BestTravelCost = Best.GetTravelCost();
    double Best3PLCost = Best.GetTplCost();

    int BestCusRouteAverage = 0;
    int BestlMinNumCust = 10000;
    int BestMaxNumCust = 0;
    int BestUsedDriver = 0;

    for (int i = 0; i < Best.GetDriverCount(); i++) {
        if (Best.RoutesLength[i] >= 1) {
            BestUsedDriver++;
            sumCapacity = sumCapacity+ Best.GetDriver(i)->load;
            if (Best.RoutesLength[i] < BestlMinNumCust)
                BestlMinNumCust = Best.RoutesLength[i];
            if (Best.RoutesLength[i] > BestMaxNumCust)
                BestMaxNumCust = Best.RoutesLength[i];
            if (Best.GetDriver(i)->load > maxCapacityUsed)
                maxCapacityUsed= Best.GetDriver(i)->load;
        }
        BestCusRouteAverage = (Best.GetCustomerCount() - Best.GetUnassignedCustCount()) / BestUsedDriver;
    }

    int BestWaiting = Best.CalculateTotalAvailabilityWaitingTime();
    double improvement = ((initialCost - BestCost) / initialCost) * 100.0;
    double Bestime = Best.duration_time;
    int bestIteration = Best.bestItr;
    double TotalTime = get_cpu_time() - start;
    int NumAssDrivers = Best.GetDriverCountAssigned();
    int NumAssCustomer = Best.GetCustomerCount() - Best.GetUnassignedCustCount();
    int NumCust3Pl = Best.GetUnassignedCustCount();

    int AveCapacityUsed = sumCapacity /BestUsedDriver;

    int SumShift = 0;
    int SumAvail = 0;
    double Percentutility = 0;
    for (int num = 0; num < Best.GetDriverCount(); num++) {
        if (Best.GetDriver(num)->sumDemand > 0.5) {
            int shift = Best.GetDriver(num)->shift.end - Best.GetDriver(num)->shift.start;
            int avail = Best.GetDriver(num)->availability.second - Best.GetDriver(num)->availability.first;
            SumShift = SumShift + shift;
            SumAvail = SumAvail + avail;
        }
    }
    Percentutility = 100 * (static_cast<double>(SumShift) / static_cast<double>(SumAvail));


   // Best.GenerateTikz(instanceName);

    std::ofstream csvFile;
    bool fileExists = std::filesystem::exists("results.csv");
    csvFile.open("results.csv", std::ios::app);

    if (!fileExists) {
        csvFile << "instanceName,initialCost,BestCost,initialShiftCost,BestShiftCost,initialTravelCost,BestTravelCost,"
                   "initial3PLCost,Best3PLCost,initialWaiting,BestWaiting,InitialTime,bestIteration,Bestime,Average,%ImprovementBestToIonitial,"
                   "IntraTime,RelocateTime,ReclocateChainTime,2OptTime,ExTime,AssignmentTime,#Intra,#Relocate,#RelocateC,#2Opt,"
                   "#Ex,#Assign,#AssDrivers,#AssCustomer,#3PlInitial,#3PlFinal,%Utility,initialCusRouteAverage,BestCusRouteAverage,"
                   "initialMinNumCust,BestlMinNumCust,initialMaxNumCust,BestMaxNumCust,UsedDriver,BestUsedDriver,sumCapacity,AveCapacityUsed,maxCapacityUsed,r,Total Time\n";
    }

    csvFile << std::fixed << std::setprecision(2)
            << instanceName << ","
            << initialCost << ","
            << BestCost << ","
            << initialShiftCost << ","
            << BestShiftCost << ","
            << initialTravelCost << ","
            << BestTravelCost << ","
            << initial3PLCost << ","
            << Best3PLCost << ","
            << initialWaiting << ","
            << BestWaiting << ","
            << InitialTime << ","
            << bestIteration << ","
            << Bestime << ","
            << Average << ","
            << improvement << ","
            << TIntra << ","
            << TRelocate << ","
            << TRelocateC << ","
            << T2Opt << ","
            << TEx << ","
            << TAssign << ","
            << NumberIntra << ","
            << NumberRelocate << ","
            << NumberRelocateC << ","
            << Number2Opt << ","
            << NumberEx << ","
            << NumberAssign << ","
            << NumAssDrivers << ","
            << NumAssCustomer << ","
            << initialNumCust3Pl << ","
            << NumCust3Pl << ","
            << Percentutility << ","
            << initialCusRouteAverage << ","
            << BestCusRouteAverage << ","
            << initialMinNumCust << ","
            << BestlMinNumCust << ","
            << initialMaxNumCust << ","
            << BestMaxNumCust << ","
            << UsedDriver << ","
            << BestUsedDriver << ","
            << sumCapacity << ","
            << AveCapacityUsed << ","
            << maxCapacityUsed << ","
            << r << ","
            << TotalTime << "\n";

    csvFile.close();
}

