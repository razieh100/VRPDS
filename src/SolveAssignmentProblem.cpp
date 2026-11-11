//
// Created by Razieh Mousavi on 2024-04-11.
//

#include "SolveAssignmentProblem.h"


#include <algorithm>
#include <cstdio>
#include <iostream>
#include <limits>




namespace AssignmentProblemSolver {

    namespace {

        const bool verbose = false;


        Matrix normalizeInput(const Matrix &input, MODE mode) {
            const int org_rows = input.size(), org_cols = input[0].size();

            // is the number of cols not equal to number of rows?
            // if yes, expand with 0-cols / 0-cols
            const int mrank = std::max(org_rows, org_cols);

            Matrix output;
            output.resize(mrank, std::vector<int>(mrank, 0));

            int max_cost = 0;
            for (int i = 0; i < org_rows; i++) {
                for (int j = 0; j < org_cols; j++) {
                    output[i][j] = input[i][j];
                    max_cost = std::max(max_cost, output[i][j]);
                }
            }

            if (mode == MODE_MAXIMIZE_UTIL) {
                for (int i = 0; i < org_rows; i++) {
                    for (int j = 0; j < org_cols; j++) {
                        output[i][j] = max_cost - output[i][j];
                    }
                }
            }

            return output;
        }

    }  // namespace

    struct ShiftRouteInfo {
        Shift shift;
        std::vector<Node*> route;
    };


    void SolveAssignment(Sol &sol) {
       // sol.Show();

        int matrixSize = sol.GetDriverCount();// just for working drivers
        AssignmentProblemSolver::Matrix input(matrixSize, std::vector<int>(matrixSize, 0));

       // Define a vector to store both shift and route information
        std::vector<std::vector<ShiftRouteInfo>> shiftRouteMatrix(matrixSize, std::vector<ShiftRouteInfo>(matrixSize));

        for (int i = 0; i < sol.GetDriverCount(); ++i) {//driver

            Driver *d1 = sol.GetDriver(i);

            for (int j = 0; j < sol.GetDriverCount(); j++) {//route

                Driver *d2 = sol.GetDriver(j);
                if (sol.RoutesLength[d2->id] > 0.9)
                {// this is the route

                    Node *FirstNode = sol.GetNode(d2->StartNodeID);
                    Node *FirstCustomer = sol.DriverNext[FirstNode->id];

                    Node *LasttNode = sol.GetNode(d2->EndNodeID);
                    Node *LastCustomer = sol.DriverPrev[LasttNode->id];
                    if (d1->availability.first <= (FirstCustomer->due - sol.Distance(FirstNode, FirstCustomer, d2)) &&
                        d1->availability.second >= LasttNode->ready + sol.Distance(LastCustomer, LasttNode, d2)) {

                        //Shift newShiftd1 = sol.FindSmallestShiftForAssignment(d1, d2);/////// smallest shift of driver 1 that can do route of driver 2

                        //double newShiftCost = ((newShiftd1.end - newShiftd1.start) / 60) * d1->salary;
                        MoveDepot<class Node, class Driver, class MoveT> tmp;
                        DriversCostAndFeasibility(sol ,d1, d2, tmp);
                        if (tmp.IsFeasible == false)
                        {
                            input[i][j] = 90000000; //if shift not feasible 90000000
                        }else {

                            double newShiftCost = tmp.CostShift;
                            //int cost = sol.GetTravelCost(d2) + newShiftCost;
                            int cost =  newShiftCost;
                            // Store shift and route information
                            shiftRouteMatrix[i][j].shift = tmp.Shift_toD1;
                            shiftRouteMatrix[i][j].route = tmp.Route; //sol.FindNewRouteAfterInsertionDepot(sol.GetNode(d1->StartNodeID), sol.GetNode(d1->EndNodeID), sol.FindNewRouteAfterRemoveDepot(d2));

                            // std::cout << "driver "<< i<< " route "<<j<< "travel cost:  " << sol.GetTravelCost(d2)<< " shift cost: "<< newShiftCost<<" salary: "<<d1->salary << std::endl;

                            input[i][j] = cost;
                        }

                    } else {
                        input[i][j] = 90000000;//this route is not feasible for this driver
                    }
                } else
                {
                    input[i][j] = 0;// a driver can have no routes
                }
            }

            //std::cout << "FindSmallestShiftForAssignment " << d1->shift.start << ", " <<d1->shift.end << std::endl;
            //d1->shift = ShiftBefore;
            //std::cout << "Shift After" << d1->shift.start << ", " <<d1->shift.end << std::endl;
            //std::cout << "-----------------------" << std::endl;
        }
        //AssignmentProblemSolver::PrintMatrix(input);
        //std::cout << "---------------------------------" << std::endl;

        AssignmentProblemSolver::Result result = AssignmentProblemSolver::Solve(input, AssignmentProblemSolver::MODE_MINIMIZE_COST);

        //std::cout << "Assignment Matrix:" << std::endl;
        //AssignmentProblemSolver::PrintMatrix(result.assignment);

        // Print the total cost
        //std::cout << "Total Cost: " << result.totalCost << std::endl;// this is the cost of assignment including drivers without a route which is not the right cost
        int sum=0;
        for (int i = 0; i < sol.GetDriverCount(); i++) {
            for (int j = 0; j < sol.GetDriverCount(); j++) {
                if (result.assignment[i][j]>0.9){
                   // std::cout << "shift cost of " <<  "route " << sol.GetDriver(j)->id << " to driver "<< sol.GetDriver(i)->id<<   " is: "<< input[i][j] << std::endl;
                    sum= sum + input[i][j];// this is shift sum should be compared with shift cost
                }
            }
        }
        //std::cout << "new shift cost should be: " << sum << std::endl;
        // sol.Show();


         //from here change the solution
        if (sum < sol.GetShiftCost(1))
        {
            std::cout << "shift cost before: " << sol.GetShiftCost(1)<< std::endl;

            std::vector<Node *> _DriverNext (sol.GetData()->GetNodeCount(), nullptr);
            std::vector<Node *> _DriverPrev(sol.GetData()->GetNodeCount(), nullptr);

            for (int i = 0; i < sol.GetDriverCount(); i++) {
                for (int j = 0; j < sol.GetDriverCount(); j++) {
                    if (result.assignment[i][j] > 0.9) {
                       // if(i != j) {
                            if (i==0 ){
                               int a=1;
                            }
                            sol.AssignRouteToDriver(sol.GetDriver(i), shiftRouteMatrix[i][j].route, _DriverPrev, _DriverNext);
                       // }
                    }
                }
            }

            for (int i = 0; i < sol.GetDriverCount(); i++) {
                for (int j = 0; j < sol.GetDriverCount(); j++) {
                    if (result.assignment[i][j] > 0.9 & i !=j){
                        if (shiftRouteMatrix[i][j].route.size() ==0)
                        {
                            sol.RoutesLength[sol.GetDriver(i)->id]=0;
                        }else{
                            sol.RoutesLength[sol.GetDriver(i)->id] = shiftRouteMatrix[i][j].route.size()-2;
                        }
                    }
                }
            }


            sol.DriverNext = _DriverNext;
            sol.DriverPrev = _DriverPrev;
//update driverassigned to

            sol.DriverAssgnment();

            int sum2=0;
            for (int i = 0; i < sol.GetDriverCount(); i++) {
                for (int j = 0; j < sol.GetDriverCount(); j++) {
                    if (result.assignment[i][j] > 0.9) {

                        Driver *d = sol.GetDriver(i);
                        d->shift = shiftRouteMatrix[i][j].shift; // Update the driver's shift
                        d->Shift_Cost = ((d->shift.end - d->shift.start) / 60.0) * d->salary;
                        sum2 = sum2+ d->Shift_Cost;
                    }
                }
            }
            std::cout << " shift cost after update shift: " << sum2<< std::endl;


            //to here change the solution
            sol.Update();
            sol.Show(1);
            // shift drivers without route set shift to zero and others update their shift

        }

    }


    Result Solve(const Matrix &input, MODE mode) {
        Result result;
        result.success = true;
        result.cost = normalizeInput(input, mode);

        //const int INF = std::numeric_limits<int>::max();

        // mrank == rows == cols. it's a square matrix.
        const int mrank = result.cost.size();

        result.assignment.resize(mrank, std::vector<int>(mrank, NOT_ASSIGNED));

        int j, k, l, s, t, q, unmatched;

        int cost = 0;

        std::vector<int> col_mate = std::vector<int>(mrank, 0);
        std::vector<int> unchosen_row = std::vector<int>(mrank, 0);
        std::vector<int> row_dec = std::vector<int>(mrank, 0);
        std::vector<int> slack_row = std::vector<int>(mrank, 0);

        std::vector<int> row_mate = std::vector<int>(mrank, 0);
        std::vector<int> parent_row = std::vector<int>(mrank, 0);
        std::vector<int> col_inc = std::vector<int>(mrank, 0);
        std::vector<int> slack = std::vector<int>(mrank, 0);

        for (int i = 0; i < mrank; ++i) {
            for (int j = 0; j < mrank; ++j) {
                result.assignment[i][j] = NOT_ASSIGNED;
            }
        }

        // Begin subtract column minima in order to start with lots of zeroes 12
        if (verbose) fprintf(stderr, "Using heuristic\n");
        for (l = 0; l < mrank; l++) {
            s = result.cost[0][l];
            for (k = 1; k < mrank; k++) {
                if (result.cost[k][l] < s) {
                    s = result.cost[k][l];
                }
            }
            cost += s;
            if (s != 0) {
                for (k = 0; k < mrank; k++) {
                    result.cost[k][l] -= s;
                }
            }
        }
        // End subtract column minima in order to start with lots of zeroes 12

        // Begin initial state 16
        t = 0;
        for (l = 0; l < mrank; l++) {
            row_mate[l] = -1;
            parent_row[l] = -1;
            col_inc[l] = 0;
            slack[l] = INF;
        }
        for (k = 0; k < mrank; k++) {
            s = result.cost[k][0];
            for (l = 1; l < mrank; l++) {
                if (result.cost[k][l] < s) {
                    s = result.cost[k][l];
                }
            }
            row_dec[k] = s;
            for (l = 0; l < mrank; l++) {
                if (s == result.cost[k][l] && row_mate[l] < 0) {
                    col_mate[k] = l;
                    row_mate[l] = k;
                    if (verbose) fprintf(stderr, "matching col %d==row %d\n", l, k);
                    goto row_done;
                }
            }
            col_mate[k] = -1;
            if (verbose) fprintf(stderr, "node %d: unmatched row %d\n", t, k);
            unchosen_row[t++] = k;
            row_done:;
        }
        // End initial state 16

        // Begin Hungarian algorithm 18
        if (t == 0) goto done;
        unmatched = t;
        while (true) {
            if (verbose) fprintf(stderr, "Matched %d rows.\n", mrank - t);
            q = 0;
            while (true) {
                while (q < t) {
                    // Begin explore node q of the forest 19
                    {
                        k = unchosen_row[q];
                        s = row_dec[k];
                        for (l = 0; l < mrank; l++)
                            if (slack[l]) {
                                int del;
                                del = result.cost[k][l] - s + col_inc[l];
                                if (del < slack[l]) {
                                    if (del == 0) {
                                        if (row_mate[l] < 0) goto breakthru;
                                        slack[l] = 0;
                                        parent_row[l] = k;
                                        if (verbose)
                                            fprintf(stderr, "node %d: row %d==col %d--row %d\n", t,
                                                    row_mate[l], l, k);
                                        unchosen_row[t++] = row_mate[l];
                                    } else {
                                        slack[l] = del;
                                        slack_row[l] = k;
                                    }
                                }
                            }
                    }
                    // End explore node q of the forest 19
                    q++;
                }

                // Begin introduce a new zero into the matrix 21
                s = INF;
                for (l = 0; l < mrank; l++)
                    if (slack[l] && slack[l] < s) s = slack[l];
                for (q = 0; q < t; q++) {
                    row_dec[unchosen_row[q]] += s;
                }
                for (l = 0; l < mrank; l++)
                    if (slack[l]) {
                        slack[l] -= s;
                        if (slack[l] == 0) {
                            // Begin look at a new zero 22
                            k = slack_row[l];
                            if (verbose)
                                fprintf(stderr,
                                        "Decreasing uncovered elements by %d produces zero at "
                                        "[%d,%d]\n",
                                        s, k, l);
                            if (row_mate[l] < 0) {
                                for (int j = l + 1; j < mrank; j++) {
                                    if (slack[j] == 0) {
                                        col_inc[j] += s;
                                    }
                                }
                                goto breakthru;
                            } else {
                                parent_row[l] = k;
                                if (verbose)
                                    fprintf(stderr, "node %d: row %d==col %d--row %d\n", t,
                                            row_mate[l], l, k);
                                unchosen_row[t++] = row_mate[l];
                            }
                            // End look at a new zero 22
                        }
                    } else {
                        col_inc[l] += s;
                    }
                // End introduce a new zero into the matrix 21
            }
            breakthru:
            // Begin update the matching 20
            if (verbose) fprintf(stderr, "Breakthrough at node %d of %d!\n", q, t);
            while (true) {
                int j = col_mate[k];
                col_mate[k] = l;
                row_mate[l] = k;
                if (verbose) fprintf(stderr, "rematching col %d==row %d\n", l, k);
                if (j < 0) break;
                k = parent_row[j];
                l = j;
            }
            // End update the matching 20
            if (--unmatched == 0) goto done;
            // Begin get ready for another stage 17
            t = 0;
            for (l = 0; l < mrank; l++) {
                parent_row[l] = -1;
                slack[l] = INF;
            }
            for (k = 0; k < mrank; k++)
                if (col_mate[k] < 0) {
                    if (verbose) fprintf(stderr, "node %d: unmatched row %d\n", t, k);
                    unchosen_row[t++] = k;
                }
            // End get ready for another stage 17
        }
        done:

        // Begin doublecheck the solution 23
        for (k = 0; k < mrank; k++) {
            for (l = 0; l < mrank; l++) {
                if (result.cost[k][l] < row_dec[k] - col_inc[l]) return {};
            }
        }
        for (k = 0; k < mrank; k++) {
            l = col_mate[k];
            if (l < 0 || result.cost[k][l] != row_dec[k] - col_inc[l]) return {};
        }
        k = 0;
        for (l = 0; l < mrank; l++) {
            if (col_inc[l]) {
                k++;
            }
        }
        if (k > mrank) return {};
        // End doublecheck the solution 23
        // End Hungarian algorithm 18

        for (int i = 0; i < mrank; ++i) {
            result.assignment[i][col_mate[i]] = ASSIGNED;
            /*TRACE("%d - %d\n", i, col_mate[i]);*/
        }
        for (k = 0; k < mrank; ++k) {
            for (l = 0; l < mrank; ++l) {
                /*TRACE("%d ",result.cost[k][l]-row_dec[k]+col_inc[l]);*/
                result.cost[k][l] = result.cost[k][l] - row_dec[k] + col_inc[l];
            }
            /*TRACE("\n");*/
        }
        for (int i = 0; i < mrank; i++) {
            cost += row_dec[i];
        }
        for (int i = 0; i < mrank; i++) {
            cost -= col_inc[i];
        }
        if (verbose) fprintf(stderr, "Cost is %d\n", cost);

        result.totalCost = cost;
        return result;
    }

    void PrintMatrix(const Matrix &m) {
        const int rows = m.size(), cols = m[0].size();
        //fprintf(stderr, "\n");
        for (int i = 0; i < rows; i++) {
            //fprintf(stderr, " [");
            std::cout<<" [";
            for (int j = 0; j < cols; j++) {
                //fprintf(stderr, "%5d ", m[i][j];
                std::cout<<m[i][j]<<"\t";
            }
            //fprintf(stderr, "]\n");
            std::cout<< "]"<<std::endl;
        }
        //fprintf(stderr, "\n");
    }




    void DriversCostAndFeasibility(Sol &s,Driver *d1, Driver*d2, MoveDepot<class Node, class Driver, class MoveT> &tmp){

        tmp.IsFeasible = false;
        tmp.driver =d1;
        tmp.route =d2;


        //remove depot of route 2, d2
        std::vector<Node*> NewRoute22 = s.FindNewRouteAfterRemoveDepot( d2);
        //add depot of d1 to route 2 (d2)
        std::vector<Node*> NewRoute2 = s.FindNewRouteAfterInsertionDepot(s.GetNode(d1->StartNodeID), s.GetNode(d1->EndNodeID),  NewRoute22);
        //this a new route same as previous but with new depots. // d2 is the route

        //Shift shiftD1 = s.FindShortestShift(NewRoute1, d1);
        Shift shiftD2 = s.FindShortestShift(NewRoute2, d1);


        if (shiftD2.end == -1  ) {
            return; // Not feasible
        } else {

            double AfterInsertionShiftCostD1 = ((shiftD2.end - shiftD2.start) / 60.0) * d1->salary;// should be d1
            //double AfterInsertionShiftCostD1 = ((shiftD1.end - shiftD1.start) / 60.0) * d1->salary;
           // double deltaShift = AfterInsertionShiftCostD1  - d1->Shift_Cost ;

            tmp.CostShift = AfterInsertionShiftCostD1;

            tmp.Shift_toD1= shiftD2;
            tmp.IsFeasible = true;
            tmp.Route = NewRoute2;
        }
    }







}  // namespace Hungarian

