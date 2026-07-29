//
// Created by Razieh Mousavi on 2024-07-17.
//

#ifndef VRPD3FEB_INTRA_H
#define VRPD3FEB_INTRA_H


#include "Solution.h"
#include "MoveVrp.h"
#include "waltime.h"

//#include "IAlgorithm.h"
//#include "../../lib/mathfunc.h"
//#include "Relatedness.h"


template <class NodeT, class DriverT>
class Intra
{

public:

    Intra(){}
    ~Intra(){}

    void Optimize(Sol & s)
    {
        s.Update();
        bool found = true;
        while(found) {
            found = false;

            // Iterate through each driver
            for (int i = 0; i < s.GetDriverCount(); i++) {
                DriverT *d = s.GetDriver(i);

                // Check if the driver has more than one customer
                if (s.RoutesLength[d->id] < 1.5) {
                    continue;
                }
                MoveVrp BestMove;
                BestMove.DeltaCost = INFINI;
                // Iterate through each customer in the driver's route
                NodeT *s1 = s.DriverNext[d->StartNodeID];
                while (s1->type != Parameters::TypeNode::END_LINK) {
                    NodeT *customer = s1;
                    NodeT *prev = s.GetNode(d->StartNodeID);

                    while (prev->type != Parameters::TypeNode::END_LINK) {
                        NodeT *next = s.DriverNext[prev->id];
                        if (next == customer) {
                            prev = s.DriverNext[customer->id];
                            next = s.DriverNext[prev->id];
                            if (next == nullptr) {
                                break;
                            }
                        }


                        if (customer->id ==19 && prev->id == 23){
                            int a;
                        }
                        MoveVrp tmpMove;
                        // RemoveInsert(s, customer, prev, next, d, tmpMove);
                        {
                            tmpMove.n = customer; // remove n and insert it between prev and next
                            tmpMove.prev = prev;
                            tmpMove.next = next;

                            std::vector<NodeT*> NewRoute = s.FindNewRouteAfterRemoveAndInsertNode(prev, customer, next, d);
                            Shift shift = s.FindShortestShift(NewRoute, d);

                            if (shift.end == -1) {
                                prev = s.DriverNext[prev->id];
                                continue; // Not feasible, skip to next iteration
                            } else {
                                double AfterInsertionShiftCost = ((shift.end - shift.start) / 60.0) * d->salary;
                                double deltaShift = AfterInsertionShiftCost - d->Shift_Cost;

                                double a1= s.Distance(s.DriverPrev[customer->id], customer);
                                double a2 = s.Distance(customer, s.DriverNext[customer->id]);
                                double a3 = s.Distance(s.DriverPrev[customer->id], s.DriverNext[customer->id]);
                                double a4 = s.Distance(prev, next);
                                double a5= s.Distance(prev, customer);
                                double a6 = s.Distance(customer, next);
                                // Calculate the delta route cost
                                double deltaDist = -s.Distance(s.DriverPrev[customer->id], customer)
                                                   - s.Distance(customer, s.DriverNext[customer->id])
                                                   + s.Distance(s.DriverPrev[customer->id], s.DriverNext[customer->id])
                                                   - s.Distance(prev, next)
                                                   + s.Distance(prev, customer)
                                                   + s.Distance(customer, next);

                                double deltaTplCost = 0; // No 3PL insertion cost in this case
                                double newDeltaCost = deltaDist + deltaTplCost + deltaShift;

                                tmpMove.Shift_toD = shift;
                                tmpMove.IsFeasible = true;
                                tmpMove.DeltaCost = newDeltaCost;
                            }
                        }

                        if (tmpMove.IsFeasible && tmpMove.DeltaCost < BestMove.DeltaCost) {
                            BestMove = tmpMove;
                        }
                        prev = next;
                    }
                    s1 = s.DriverNext[s1->id];
                }

                if (BestMove.IsFeasible && BestMove.DeltaCost < -0.001) {
                    // Apply the move
                    s.RemoveAndInsert(BestMove.n, BestMove.prev);

                    // Update shifts and costs
                    d->shift = BestMove.Shift_toD;
                    d->Shift_Cost = ((BestMove.Shift_toD.end - BestMove.Shift_toD.start) / 60) * d->salary;

                    s.Update(d);

                    found = true;
                    s.Show(1);
                }
            }
        }
        s.Update();

    }

private:
    int _mv_count;
    int _nbiter;
    int _nbitergl;
};


#endif //VRPD3FEB_INTRA_H
