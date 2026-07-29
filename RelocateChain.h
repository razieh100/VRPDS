//
// Created by Razieh Mousavi on 2024-05-10.
//

#ifndef VRPD3FEB_RELOCATECHAIN_H
#define VRPD3FEB_RELOCATECHAIN_H

#include "Solution.h"
#include "MoveChain.h"
#include "waltime.h"

//#include "IAlgorithm.h"
//#include "../../lib/mathfunc.h"
//#include "Relatedness.h"


//template <class Node, class Driver>
class RelocateChain
{

public:

    // RelocateChain(Relatedness<Node,Driver> * rel){_rel = rel;}
    RelocateChain(){}
    ~RelocateChain(){}

    void Optimize(Sol & s)
    {
        _nbiter = _nbitergl = _mv_count = 0;
        s.Update();
        //double prevcost = s.GetLastCalculatedCost();
        double prevcost = s.GetCost(1);
        printf("-->Relocate Chains:%.3lf\n", s.GetCost(1));

        // if(s.GetCustomerCount() < 50)
        OptimizeAllPairs(s);
        // else
        //   OptimizeRandomPairs(s);

        s.Update();
        s.Show(1);

        double newcost = s.GetCost(1);
        printf("-->Relocate Chains: prev:%.3lf new:%.3lf ", prevcost,newcost);
        printf("nbiter:%d glb:%d ", _nbiter,_nbitergl);
        if(prevcost > newcost) printf("************");
        printf("\n");
    }

    void OptimizeRandomPairs(Sol & s)
    {
        std::vector<Driver*> drivers;//excluding empty routes
        for(int i=0;i<s.GetDriverCount();i++)
            if(s.RoutesLength[i] >= 1)
                drivers.push_back(s.GetDriver(i));

        for(int i=0;i<100;i++)
        {
            int ind1 = Parameters::getRandomIntValue(0, (int)drivers.size());
            int ind2 = Parameters::getRandomIntValue(0, (int)drivers.size());
            //int ind1 = mat_func_get_rand_int(0, (int)drivers.size());
            //int ind2 = mat_func_get_rand_int(0, (int)drivers.size());
            if(ind1 == ind2) continue;
            Optimize(s,s.GetDriver(ind1),s.GetDriver(ind2));
        }
    }

    void OptimizeAllPairs(Sol & s)
    {
        bool found_imprv = true;
        std::vector<int> drivers(s.GetDriverCount(), 0);
        int c = 0;
        while(found_imprv)
        {
            c++;
            found_imprv = false;
            for(int i=0;i<s.GetDriverCount();i++)
                if (s.RoutesLength[i] >= 1)
                    for (int j = i + 1; j < s.GetDriverCount(); j++)
                    {
                        bool r = Optimize(s, s.GetDriver(i), s.GetDriver(j));
                        if (r) {
                            drivers[i] = _nbitergl + 1;
                            drivers[j] = _nbitergl + 1;
                            found_imprv = true;
                            //std::cout <<  "----------------------------------------- " << std::endl;
                            //s.Show(1);
                        }
                        _nbiter++;
                    }
            _nbitergl++;
        }
    }

//#include <algorithm>

    bool Optimize(Sol &s, Driver *d1, Driver *d2) {
        bool found_imprv = false;
        bool found = true;

        while (found) {
            found = false;
            MoveChain bestMove;

            // 1. Evaluate Chains from Assigned Routes (Relocate from d1 to d2)
            EvaluateAssignedChains(s, d1, d2, bestMove);

            // 2. Evaluate Chains from Unassigned Nodes (Insert unassigned into d2)
            EvaluateUnassignedChains(s, d2, bestMove);

            // 3. Evaluate Moving a Chain from d1 to 3PL
            EvaluateMoveTo3PL(s, d1, bestMove);

            // 4. Apply the Best Move Found
            if ((bestMove.DeltaCostInsert + bestMove.DeltaCostRemove) < -0.001) {
                if (bestMove.to == nullptr) {
                    // Move the chain to 3PL
                    s.RemoveAndUnassignChain(bestMove.s, bestMove.e);
                    std::cout << "Moved chain [" << bestMove.s->id << " → " << bestMove.e->id << "] to 3PL\n";
                } else {
                    // Move the chain between two routes
                    s.RelocateChainNew(bestMove.s, bestMove.e, bestMove.n);
                    s.Update(d1);
                    s.Update(d2);
                }

                // Update shift costs
                d1->shift = bestMove.Shift_toD1;
                d1->Shift_Cost = ((bestMove.Shift_toD1.end - bestMove.Shift_toD1.start) / 60) * d1->salary;

                if (bestMove.to != nullptr) {
                    d2->shift = bestMove.Shift_toD2;
                    d2->Shift_Cost = ((bestMove.Shift_toD2.end - bestMove.Shift_toD2.start) / 60) * d2->salary;
                }

                found_imprv = found = true;
                _mv_count++;
            }
        }
        return found_imprv;
    }

    void EvaluateMoveTo3PL(Sol &s, Driver *d1, MoveChain &bestMove) {
        Node *s1 = nullptr, *e1 = nullptr;
        double bestDeltaCost = 0.0;
        bool found = false;

        // **Step 1: Iterate through d1's route to find the best chain to move**
        Node *start = s.DriverNext[d1->StartNodeID];
        while (start->type != Parameters::TypeNode::END_LINK) {
            Node *end = start;
            double sum_dmd = 0, sum_time = 0;

            // **Extend the chain**
            while (end->type != Parameters::TypeNode::END_LINK) {
                sum_dmd += end->demand;
                sum_time += end->serviceTime + s.Distance(s.DriverPrev[end->id], end);

                MoveChain tmp;
                RemoveChainCost(s, start, end, d1, tmp);  // Compute removal cost

                // **Evaluate 3PL cost**
                double tplCost = s.GetData()->GetTplCost() * (s.CountNodesInChain(start, end));

                // **Compute total cost change**
                double totalDelta = tmp.DeltaCostRemove + tplCost;

                // **Find the best move**
                if (totalDelta < bestDeltaCost) {
                    bestDeltaCost = totalDelta;
                    s1 = start;
                    e1 = end;
                    bestMove = tmp;
                    bestMove.DeltaCostInsert = tplCost;
                    bestMove.to = nullptr;  // Indicate move to 3PL
                    found = true;
                }

                end = s.DriverNext[end->id];
            }
            start = s.DriverNext[start->id];
        }

        // **Step 2: Store the best move if found**
        if (found) {
            bestMove.s = s1;
            bestMove.e = e1;
        }
    }



    void EvaluateUnassignedChains(Sol &s, Driver *d2, MoveChain &bestMove) {
        int unassignedCount = s.GetUnassignedCustCount();
        std::vector<Node*> unassignedNodes;

        // Collect unassigned nodes
        for (int i = 0; i < unassignedCount; i++) {
            Node *node = s.GetUnassignedCust(i);
            if (node != nullptr) {
                unassignedNodes.push_back(node);
            }
        }

        // **Directly evaluate all chains of length 1 to 3**
        for (int chainLength = 1; chainLength <= 3; chainLength++) {
            for (size_t i = 0; i < unassignedNodes.size(); i++) {
                std::vector<Node*> chain;
                for (size_t j = i; j < i + chainLength && j < unassignedNodes.size(); j++) {
                    chain.push_back(unassignedNodes[j]);
                }

                if (chain.size() == chainLength) {
                    // **Step 1: Check capacity**
                    double sum_dmd_chain = 0, sum_time = 0;
                    for (Node *node : chain) {
                        sum_dmd_chain += node->demand;
                        sum_time += node->serviceTime;
                    }
                    if (d2->sumDemand + sum_dmd_chain > d2->capacity) continue; // Skip if exceeds capacity

                    MoveChain tmp;
                    Node *s1 = chain.front();
                    Node *e1 = chain.back();
                    Node *prev_d2 = s.GetNode(d2->StartNodeID);

                    while (prev_d2->type != Parameters::TypeNode::END_LINK) {
                        // **Step 2: Check time windows feasibility**
                        double arr_s1 = s.AvailabilityDepartureTime[prev_d2->id] + s.Distance(prev_d2, s1);
                        if (arr_s1 > s1->due) break;

                        Node *next = s.DriverNext[prev_d2->id];
                        double arr_next = std::max(arr_s1, s1->ready) + s1->serviceTime + sum_time + s.Distance(e1, next);
                        if (arr_next > next->due || arr_next - s.AvailabilityArrivalTime[next->id] > s.AvailabilitySlackTime[next->id]) {
                            prev_d2 = s.DriverNext[prev_d2->id];
                            continue;
                        }

                        // **Step 3: Find the new route and update shifts**
                        std::vector<Node*> NewRoute = s.FindNewRouteAfterInsertionChain(prev_d2, s1, e1, d2);
                        Shift shift = s.FindShortestShift(NewRoute, d2);

                        if (shift.end == -1) {
                            prev_d2 = s.DriverNext[prev_d2->id];
                            continue; // Skip if not feasible
                        }

                        // **Step 4: Compute cost changes**
                        double driverNewShift = ((shift.end - shift.start) / 60.0) * d2->salary;
                        double driverShiftOld = ((d2->shift.end - d2->shift.start) / 60.0) * d2->salary;
                        double deltaShift = driverNewShift - driverShiftOld;

                        // **Step 5: Calculate route cost change**
                        double deltaDist = s.Distance(prev_d2, s1) + s.Distance(e1, s.DriverNext[prev_d2->id]) - s.Distance(prev_d2, s.DriverNext[prev_d2->id]);
                        double deltaTplCost = 0;  // No 3PL cost in this case
                        double newDeltaCost = deltaDist + deltaTplCost + deltaShift;

                        // **Step 6: Store the best move if it's an improvement**
                        tmp.DeltaCostInsert = newDeltaCost;
                        tmp.Shift_toD2 = shift;
                        tmp.IsFeasible = true;

                        double deltaTmp = tmp.DeltaCostInsert;
                        double deltaBest = bestMove.DeltaCostInsert;

                        if (deltaTmp < deltaBest) {
                            bestMove = tmp;
                        }

                        prev_d2 = s.DriverNext[prev_d2->id];
                    }
                }
            }
        }
    }

    /*
    void EvaluateUnassignedChains(Sol &s, Driver *d2, MoveChain &bestMove) {
        int unassignedCount = s.GetUnassignedCustCount();
        std::vector<Node*> unassignedNodes;

        // Collect unassigned nodes
        for (int i = 0; i < unassignedCount; i++) {
            Node *node = s.GetUnassignedCust(i);
            if (node != nullptr) {
                unassignedNodes.push_back(node);
            }
        }

        // **Directly evaluate all chains of length 1 to 3**
        for (int chainLength = 1; chainLength <= 3; chainLength++) {
            for (size_t i = 0; i < unassignedNodes.size(); i++) {
                std::vector<Node*> chain;
                for (size_t j = i; j < i + chainLength && j < unassignedNodes.size(); j++) {
                    chain.push_back(unassignedNodes[j]);
                }

                if (chain.size() == chainLength) {
                    // Evaluate insertion of the selected chain
                    double sum_dmd_chain = 0, sum_time = 0;
                    for (Node *node : chain) {
                        sum_dmd_chain += node->demand;
                        sum_time += node->serviceTime;
                    }

                    if (d2->sumDemand + sum_dmd_chain > d2->capacity) continue; // Skip if exceeds capacity

                    MoveChain tmp;
                    Node *s1 = chain.front();
                    Node *e1 = chain.back();
                    Node *prev_d2 = s.GetNode(d2->StartNodeID);

                    while (prev_d2->type != Parameters::TypeNode::END_LINK) {
                        double arr_s1 = s.AvailabilityDepartureTime[prev_d2->id] + s.Distance(prev_d2, s1);
                        if (arr_s1 > s1->due) break;

                        Node *next = s.DriverNext[prev_d2->id];
                        double arr_next = std::max(arr_s1, s1->ready) + s1->serviceTime + sum_time + s.Distance(e1, next);
                        if (arr_next > next->due || arr_next - s.AvailabilityArrivalTime[next->id] > s.AvailabilitySlackTime[next->id]) {
                            prev_d2 = s.DriverNext[prev_d2->id];
                            continue;
                        }

                        InsertChainCost(s, s1, e1, d2, prev_d2, tmp);

                        if (!tmp.IsFeasible) {
                            prev_d2 = s.DriverNext[prev_d2->id];
                            continue;
                        }

                        double deltaTmp = tmp.DeltaCostRemove + tmp.DeltaCostInsert;
                        double deltaBest = bestMove.DeltaCostRemove + bestMove.DeltaCostInsert;

                        if (deltaTmp < deltaBest) {
                            bestMove = tmp;
                        }

                        prev_d2 = s.DriverNext[prev_d2->id];
                    }
                }
            }
        }
    }
*/


    void EvaluateAssignedChains(Sol &s, Driver *d1, Driver *d2, MoveChain &bestMove) {
        Node *s1 = s.DriverNext[d1->StartNodeID];
        while (s1->type != Parameters::TypeNode::END_LINK) {
            Node *e1 = s1;
            double sum_dmd_s1_e1 = 0;
            double sum_time = 0;

            while (e1->type != Parameters::TypeNode::END_LINK) {
                sum_dmd_s1_e1 += e1->demand;
                sum_time += e1->serviceTime + s.Distance(s.DriverPrev[e1->id], e1);

                MoveChain tmp;
                RemoveChainCost(s, s1, e1, d1, tmp);

                if (d2->sumDemand + sum_dmd_s1_e1 <= d2->capacity) {
                    Node *prev_d2 = s.GetNode(d2->StartNodeID);
                    while (prev_d2->type != Parameters::TypeNode::END_LINK) {
                        double arr_s1 = s.AvailabilityDepartureTime[prev_d2->id] + s.Distance(prev_d2, s1);
                        if (arr_s1 > s1->due) break;

                        Node *next = s.DriverNext[prev_d2->id];
                        double arr_next = std::max(arr_s1, s1->ready) + s1->serviceTime + sum_time + s.Distance(e1, next);

                        if (arr_next > next->due) {
                            prev_d2 = s.DriverNext[prev_d2->id];
                            continue;
                        }

                        if (arr_next - s.AvailabilityArrivalTime[next->id] > s.AvailabilitySlackTime[next->id]) {
                            prev_d2 = s.DriverNext[prev_d2->id];
                            continue;
                        }

                        InsertChainCost(s, s1, e1, d2, prev_d2, tmp);

                        if (!tmp.IsFeasible) {
                            prev_d2 = s.DriverNext[prev_d2->id];
                            continue;
                        }

                        double deltaTmp = tmp.DeltaCostRemove + tmp.DeltaCostInsert;
                        double deltaBest = bestMove.DeltaCostRemove + bestMove.DeltaCostInsert;

                        if (deltaTmp < deltaBest) {
                            bestMove = tmp;
                        }
                        prev_d2 = s.DriverNext[prev_d2->id];
                    }
                }
                e1 = s.DriverNext[e1->id];
            }
            s1 = s.DriverNext[s1->id];
        }
    }


    void InsertChainCost(Sol &s, Node *s1, Node *e1, Driver *d2, Node *prev_d2, MoveChain  &tmp) {
        tmp.IsFeasible = false;// d1, s and e alredy determined in remove function
        //tmp.s = s1;
        //tmp.e = e1;
        //tmp.from = d1;
        tmp.n = prev_d2;
        tmp.to = d2;


        // Find the new route after inserting the chain
        std::vector<Node*> NewRoute = s.FindNewRouteAfterInsertionChain(prev_d2, s1, e1, d2);
        // Find the best shift after inserting the chain
        Shift shift = s.FindShortestShift(NewRoute, d2);

        if (shift.end == -1) {
            return; // Not feasible
        } else {
            double AfterInsertionShiftCost = ((shift.end - shift.start) / 60.0) * d2->salary;
            double deltaShift = AfterInsertionShiftCost - d2->Shift_Cost;

            // Calculate the delta route cost
            double deltaDist = s.Distance(prev_d2, s1) + s.Distance(e1, s.DriverNext[prev_d2->id]) - s.Distance(prev_d2, s.DriverNext[prev_d2->id]);

            double deltaTplCost = 0; // No 3PL insertion cost in this case
            double newDeltaCost = deltaDist + deltaTplCost + deltaShift;


            tmp.DeltaDistanceInsert = deltaDist;
            tmp.DeltaCostInsert = newDeltaCost;//Total
            tmp.DeltaShiftInsert = deltaShift;
            tmp.Shift_toD2 = shift;
            tmp.IsFeasible = true;
        }
    }


    void RemoveChainCost(Sol &s, Node *s1, Node *e1, Driver *d1, MoveChain &rmv) {
        rmv.s = s1;
        rmv.e = e1;
        rmv.from = d1;

        // Find the new route after removing nodes from s1 to e1
        std::vector<Node*> NewRoute = s.FindNewRouteAfterRemoveNodeChain(s1, e1, d1);

        // Find the best shift after removing the chain
        Shift RemoveShift = s.FindShortestShift(NewRoute, d1);

        double DeltaShiftCost;

        double driverNewShift;
        if (RemoveShift.start == -2) {
            driverNewShift = 0;
        } else {
            driverNewShift = ((RemoveShift.end - RemoveShift.start) / 60.0) * d1->salary;
        }

        double driverShiftOld = ((d1->shift.end - d1->shift.start) / 60.0) * d1->salary;

        DeltaShiftCost = (driverNewShift - driverShiftOld);

        // Calculate the DeltaRouteCost, no need to calculate between s and e
        double DeltaRouteCost = s.Distance(s.DriverPrev[s1->id], s.DriverNext[e1->id]) - s.Distance(s.DriverPrev[s1->id], s1) - s.Distance(e1, s.DriverNext[e1->id]);

        rmv.DeltaCostRemove = DeltaRouteCost + DeltaShiftCost;// Total
        rmv.DeltaDistanceRemove = DeltaRouteCost;
        rmv.DeltaShiftremove = DeltaShiftCost;
        rmv.Shift_toD1 = RemoveShift;
    }



private:
    int _mv_count;
    int _nbiter;
    int _nbitergl;
    // Relatedness<Node,Driver> * _rel;
};

#endif //VRPD3FEB_RELOCATECHAIN_H
