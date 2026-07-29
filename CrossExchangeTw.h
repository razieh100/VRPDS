//
// Created by Razieh Mousavi on 2024-05-15.
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
//

#ifndef VRPD3FEB_CROSSEXCHANGETW_H
#define VRPD3FEB_CROSSEXCHANGETW_H

#define CROSS_EXCHANGE_MAX_LEN 100

#include <algorithm>
#include "Solution.h"
#include "Move2Chain.h"
//#include "IAlgorithm.h"
//#include "../../lib/mathfunc.h"
#include <map>


//template <class NodeT, class DriverT>
class CrossExchangeTw
{

public:

    CrossExchangeTw() : _found_imprv(false){}
    ~CrossExchangeTw(){}

    void Optimize(Sol & sol)
    {
        s = &sol;
        // p = s->GetProb();
        s->Update();
        _mv_count = 0;
        _nb_passes = 0;
        _found_imprv = false;
        double prevcost = s->GetCost(1);

        OptimizeAllPairs(*s);

        s->Update();
        double newcost = s->GetCost(1);
    }

    void OptimizeAllPairs(Sol &s)
    {
        bool found_imprv = true;
        std::vector<int> drivers(s.GetDriverCount(), 0);
        int nbiter = 0, nbitergl=0;
        while(found_imprv)
        {
            found_imprv = false;
            for(int i=0;i<s.GetDriverCount();i++)
                if(s.RoutesLength[i] >= 1)
                    for(int j=i+1;j<s.GetDriverCount();j++)
                    {
                        if(s.RoutesLength[j] == 0) continue;
                        if(drivers[i] < nbitergl && drivers[j] < nbitergl) continue;

                        bool r = Optimize(s, s.GetDriver(i),s.GetDriver(j));
                        if(r)
                        {
                            drivers[i] = nbitergl+1;
                            drivers[j] = nbitergl+1;
                            found_imprv = true;
                        }
                        nbiter++;
                    }
            nbitergl++;
        }
    }

    bool Optimize(Sol &s, Driver * d1, Driver * d2)
    {
        //printf("Optimize 2OptStar d1:%d d2:%d\n",d1->id,d2->id);
        bool found_imprv = false;
        bool found = true;

        Move2Chain BesMove;
        while(found)
        {
            found = false;
            BesMove.DeltaCostTotal = 0.1;
            BesMove.n1 = NULL;
            BesMove.n2 = NULL;
            BesMove.e1 = NULL;
            BesMove.e2 = NULL;
            //double best_delta = -0.0001;

            // int move_type = -1;

            Move2Chain tmp;
            tmp.n1 =  s.DriverNext[ d1->StartNodeID ];
            while(tmp.n1 ->type != Parameters::TypeNode::END_LINK)
            {
                Node * prev_n1 = s.DriverPrev[tmp.n1 ->id];

                tmp.n2 =  s.DriverNext[ d2->StartNodeID];

                while(tmp.n2->type != Parameters::TypeNode::END_LINK)
                {
                    Node * prev_n2 = s.DriverPrev[tmp.n2->id];

                    // time window check of first nodes in each routes if exchanged
                    double visit_time_n1 = s.AvailabilityDepartureTime[prev_n2->id] + s.Distance(prev_n2,tmp.n1 );
                    double visit_time_n2 = s.AvailabilityDepartureTime[prev_n1->id] + s.Distance(prev_n1,tmp.n2);
                    if (visit_time_n1 > tmp.n1 ->due || visit_time_n2 > tmp.n2->due)
                        
                    {
                        tmp.n2 = s.DriverNext[ tmp.n2->id ];
                        continue;
                        
                    }


                    int cum_dmd1 = 0, len1 = 0, sum_srv1 = 0;
                    tmp.e1 = tmp.n1 ;
                    double Arrival_prev_e1 = s.AvailabilityArrivalTime[prev_n1->id];
                    while(tmp.e1->type != Parameters::TypeNode::END_LINK && len1 <= CROSS_EXCHANGE_MAX_LEN)
                    {
                        cum_dmd1 += tmp.e1->demand;
                        Node * prev_e1 = s.DriverPrev[tmp.e1->id];

                        double Arrival_e1 = 0;
                        if(tmp.e1 == tmp.n1 )
                            Arrival_e1 = visit_time_n1;
                        else
                            Arrival_e1 = std::max(Arrival_prev_e1, prev_e1->ready) +
                                         prev_e1->serviceTime +
                                         s.Distance(prev_e1,tmp.e1);
                        if (Arrival_e1 > tmp.e1->due)
                            break;



                        int cum_dmd2 = 0, len2 = 0, sum_srv2 = 0;
                        tmp.e2 = tmp.n2;
                        double Arrival_prev_e2 = s.AvailabilityArrivalTime[prev_n2->id];
                        while(tmp.e2->type != Parameters::TypeNode::END_LINK && len2 <= CROSS_EXCHANGE_MAX_LEN)
                        {
                            cum_dmd2 += tmp.e2->demand;
                            Node * prev_e2 = s.DriverPrev[tmp.e2->id];
                            Node * next_e1 = s.DriverNext[tmp.e1->id];
                            Node * next_e2 = s.DriverNext[tmp.e2->id];

                            double Arrival_e2 = 0;
                            if(tmp.e2 == tmp.n2)
                                Arrival_e2 = visit_time_n2;
                            else
                                Arrival_e2 =   std::max(Arrival_prev_e2, prev_e2->ready) +
                                               prev_e2->serviceTime +
                                               s.Distance(prev_e2,tmp.e2);
                            if (Arrival_e2 > tmp.e2->due)
                                break;


                            double visit_time_next_e2 = std::max(Arrival_e1, tmp.e1->ready) +
                                                        tmp.e1->serviceTime +
                                                        s.Distance(tmp.e1,next_e2);

                            double visit_time_next_e1 = std::max(Arrival_e2, tmp.e2->ready) +
                                                        tmp.e2->serviceTime +
                                                        s.Distance(tmp.e2,next_e1);
                            //   s.Distance(tmp.e2,next_e1);

                            if(d1->sumDemand - cum_dmd1 + cum_dmd2 > d1->capacity) break;// it exeeds the capacity so we cannot increase e2 also, so go to next e1
                            //here changedddddddddd
                            if(d2->sumDemand - cum_dmd2 + cum_dmd1 <= d2->capacity &&
                               visit_time_next_e1 - s.AvailabilityArrivalTime[next_e1->id] <=  s.AvailabilitySlackTime[next_e1->id]&&
                               visit_time_next_e2 - s.AvailabilityArrivalTime[next_e2->id] <=  s.AvailabilitySlackTime[next_e2->id])//tw of 2 routes and capacity of second
                            {

                                Move2Chain tmpNew;
                                DriversCostAndFeasibility(s, tmp.n1 , tmp.n2, tmp.e1, tmp.e2, d1, d2, tmpNew);

                                //	printf("cum1:%d cum2:%d\n", cum_dmd1, cum_dmd2);
                                if(tmpNew.DeltaCostTotal < BesMove.DeltaCostTotal && tmpNew.IsFeasible == true)
                                {
                                    BesMove.DeltaCostTotal = tmpNew.DeltaCostTotal;
                                    BesMove.ShiftTotal = tmpNew.ShiftTotal;
                                    BesMove.RoutetTotal = tmpNew.RoutetTotal;
                                    BesMove.n1 = tmpNew.n1; BesMove.e1 = tmpNew.e1;
                                    BesMove.n2 = tmpNew.n2; BesMove.e2 = tmpNew.e2;
                                    // move_type = 1;

                                    BesMove.Shift_toD1 = tmpNew.Shift_toD1;
                                    BesMove.Shift_toD2 = tmpNew.Shift_toD2;
                                    BesMove.IsFeasible = true;

                                    BesMove.from = tmpNew.from;
                                    BesMove.to = tmpNew.to;


                                    // BesMove.Time = get_cpu_time();

                                }
                            }
                            //   }

                            len2++;
                            tmp.e2 = s.DriverNext[ tmp.e2->id ];
                            Arrival_prev_e2 = Arrival_e2;
                        }

                        len1++;
                        tmp.e1 = s.DriverNext[ tmp.e1->id ];
                        Arrival_prev_e1 = Arrival_e1;
                    }

                    tmp.n2 = s.DriverNext[ tmp.n2->id ];
                }

                tmp.n1  = s.DriverNext[ tmp.n1 ->id ];
            }

            if(BesMove.DeltaCostTotal < -0.001)
            {

                //std::cout << " --------------" <<std::endl;
                //std::cout << " driver "<< d1->id << " and "  << d2->id<<" n1: " << BesMove.n1->id<< " n2: " << BesMove.n2->id <<" e1: "<< BesMove.e1->id << " e2: "<< BesMove.e2->id <<std::endl;
                //std::cout << "Before Improvement in CrossExchange : total " << s.GetCost() <<"  travel "<< s.GetTravelCost() <<" 3pl "<< s.GetTplCost()<< "Shift: " << s.GetShiftCost()<<std::endl;

                if(BesMove.DeltaCostTotal < 0){
                    //std::cout << " CrossExchange cost "<< BesMove.DeltaCostTotal << std::endl;
                    //std::cout << " CrossExchange Shiftcost "<< BesMove.ShiftTotal << std::endl;
                    //std::cout << " CrossExchange Routecost "<< BesMove.RoutetTotal << std::endl;
                    // std::cout << "Before  Improvement in CrossExchange : total " << s.GetCost() <<"  travel "<< s.GetTravelCost() <<" 3pl "<< s.GetTplCost()<<"Shift: " << s.GetShiftCost()<<std::endl;



                }


                s.SwapTwoChains(BesMove.n1,BesMove.e1,BesMove.n2,BesMove.e2);
//update shift should be before update because of calculating shift cost and total cost and ...
                d1->shift = BesMove.Shift_toD1;
                d1->Shift_Cost = ((BesMove.Shift_toD1.end - BesMove.Shift_toD1.start)/ 60) *d1->salary;
                d2->shift = BesMove.Shift_toD2;
                d2->Shift_Cost = ((BesMove.Shift_toD2.end - BesMove.Shift_toD2.start)/ 60) *d2->salary;

                s.Update(d1);
                s.Update(d2);
                s.UpdateNodeCumulativeDemand(d1);
                s.UpdateNodeCumulativeDemand(d2);

                d1->shift = BesMove.Shift_toD1;
                d1->Shift_Cost = ((BesMove.Shift_toD1.end - BesMove.Shift_toD1.start)/ 60) *d1->salary;
                d2->shift = BesMove.Shift_toD2;
                d2->Shift_Cost = ((BesMove.Shift_toD2.end - BesMove.Shift_toD2.start)/ 60) *d2->salary;


              

                _mv_count++;
                _found_imprv = found_imprv = found = true;
            }
        }
        return found_imprv;
    }


    void DriversCostAndFeasibility(Sol &s, Node *n1, Node *n2, Node *e1, Node *e2,Driver *d1, Driver*d2, Move2Chain &tmp){

        tmp.IsFeasible = false;
        tmp.from =d1;
        tmp.to =d2;
        tmp.n1 = n1;
        tmp.n2= n2;
        tmp.e1 = e1;
        tmp.e2= e2;

      
        std::vector<Node*> NewRoute11 = s.FindNewRouteAfterRemoveNodeChain(n1, e1, d1);
        std::vector<Node*> NewRoute22 = s.FindNewRouteAfterRemoveNodeChain(n2,e2, d2);

        std::vector<Node*> NewRoute1 = s.FindNewRouteAfterInsertionChainToVector(s.DriverPrev[n1->id], n2,e2, d1,  NewRoute11);
        std::vector<Node*> NewRoute2 = s.FindNewRouteAfterInsertionChainToVector(s.DriverPrev[n2->id], n1, e1, d2,  NewRoute22);


        //check capacity of d2


        Shift shiftD1 = s.FindShortestShift(NewRoute1, d1);
        Shift shiftD2 = s.FindShortestShift(NewRoute2, d2);


        if (shiftD1.end == -1 ||shiftD2.end == -1  ) {
            return; // Not feasible
        } else {

            double AfterInsertionShiftCostD2 = ((shiftD2.end - shiftD2.start) / 60.0) * d2->salary;
            double AfterInsertionShiftCostD1 = ((shiftD1.end - shiftD1.start) / 60.0) * d1->salary;
            double deltaShift = AfterInsertionShiftCostD2 + AfterInsertionShiftCostD1 - d2->Shift_Cost - d1->Shift_Cost;


            double deltaDist =  - s.Distance(s.DriverPrev[n1->id], n1) - s.Distance(s.DriverPrev[n2->id], n2)
                                - s.Distance(e1, s.DriverNext[e1->id]) - s.Distance(e2, s.DriverNext[e2->id])
                                + s.Distance(e1, s.DriverNext[e2->id]) + s.Distance(e2, s.DriverNext[e1->id])
                                + s.Distance(s.DriverPrev[n1->id], n2) + s.Distance(s.DriverPrev[n2->id], n1);
            // std::cout << " deltaDist "<< deltaDist << std::endl;
            double deltaTplCost = 0; // No 3PL insertion cost in this case
            double newDeltaCost = deltaDist + deltaTplCost + deltaShift;

            tmp.DeltaCostTotal = newDeltaCost;//Total
            tmp.ShiftTotal = deltaShift;
            tmp.RoutetTotal = deltaDist;
            // if(tmp.DeltaCostTotal < 0){
            //    std::cout << " 2Pt cost "<< tmp.DeltaCostTotal << std::endl;

            //}
            tmp.DeltaShiftD1 =  AfterInsertionShiftCostD1 -  d1->Shift_Cost;
            tmp.DeltaShiftD2 = AfterInsertionShiftCostD2  - d2->Shift_Cost;
            // tmp.DeltaDistanceInsert = deltaDist;
            //tmp.DeltaShiftInsert = deltaShift;
            tmp.Shift_toD2 = shiftD2;
            tmp.Shift_toD1= shiftD1;
            tmp.IsFeasible = true;
        }
    }



    bool FoundImprovement(){return _found_imprv;}
private:
    //Prob<NodeT, DriverT> * p;
    Sol * s;
    double ** dist;
    double ** time;
    int _mv_count;
    unsigned long long _nb_passes;
    bool _found_imprv;
};


#endif //VRPD3FEB_CROSSEXCHANGETW_H

