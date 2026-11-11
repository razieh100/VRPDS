//
// Created by Razieh Mousavi on 2024-04-03.
//

#ifndef VRPD3FEB_OPT2STARTW_H
#define VRPD3FEB_OPT2STARTW_H

/*
 * Copyright Jean-Francois Cote 2015
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 *
 * 2-Opt* operator for vrptw problems
 */


#include "Solution.h"
#include "Opt2StarTwCost.h"
#include "Move2Chain.h"
//#include "waltime.h"



//template <class NodeT, class DriverT>
class Opt2StarTw
{

public:

    Opt2StarTw(){_found_imprv=false;}
    ~Opt2StarTw(){}

    void Optimize(Sol &s)
    {
        s.Update();
        _mv_count = 0;
        _nb_passes = 0;
        _found_imprv = false;
        double prevcost = s.GetCost(1);
        //printf("-->Opt2StarTw Optimize SolCost:%.3lf\n",prevcost);

        OptimizeAllPairs(s);

        double newcost = s.GetCost(1);
        printf("-->Opt2StarTw prev:%.3lf new:%.3lf ", prevcost,newcost);
        //printf("moves:%d passes:%llu ", _mv_count, _nb_passes);
        //if(prevcost > newcost) printf("************");
        //printf("\n");
        s.Update();
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

                        bool r = Optimize(s,s.GetDriver(i),s.GetDriver(j));
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

    bool Optimize(Sol &s,Driver * d1, Driver * d2)
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
            // double best_delta = 0.1;
            //NodeT * best_n1 = NULL;
            // NodeT * best_n2 = NULL;

            Node * n1 = s.DriverNext[ d1->StartNodeID ];
            while(n1->type != Parameters::TypeNode::END_LINK)
            {
                Node * n2 = s.DriverPrev[ d2->EndNodeID ];

                while (n2->type != Parameters::TypeNode::START_LINK)
                {
                    double dmdAfter_n1 = d1->sumDemand - s.DriverPrev[n1->id]->cumulative_dmd;//including n
                    double dmdAfter_n2 = d2->sumDemand - s.DriverPrev[n2->id]->cumulative_dmd;
                    _nb_passes++;
                    if(s.DriverPrev[n1->id]->cumulative_dmd + dmdAfter_n2 > d1->capacity) break;

                    if(s.DriverPrev[n2->id]->cumulative_dmd + dmdAfter_n1 <= d2->capacity)
                    {
                       // if(d1->id == 2 && d2->id == 19)
                       // {
                         //   if (n1->id == 88 && n2->id==78 ){
                          //      std::cout << " --------------" <<std::endl;
                          //  }
                       // }

                        Move2Chain tmp;
                        DriversCostAndFeasibility(s, n1, n2, d1, d2, tmp);

                        if(tmp.IsFeasible == false){
                            n2 = s.DriverPrev[n2->id];
                            continue;
                        }

                        if(tmp.DeltaCostTotal < BesMove.DeltaCostTotal)
                        {
                            BesMove.DeltaCostTotal = tmp.DeltaCostTotal;
                            BesMove.n1 = tmp.n1;
                            BesMove.n2 = tmp.n2;

                            BesMove.Shift_toD1 = tmp.Shift_toD1;
                            BesMove.Shift_toD2 = tmp.Shift_toD2;
                            BesMove.IsFeasible = true;

                            BesMove.from = tmp.from;
                            BesMove.to = tmp.to;

                          //  BesMove.Time = get_cpu_time() ;

                        }
                    }
                    n2 = s.DriverPrev[n2->id];
                }//end while
                n1 = s.DriverNext[n1->id];
            }//end while

            if(BesMove.DeltaCostTotal < -0.001)
            {

               // std::cout << " --------------" <<std::endl;
               // std::cout << " driver"<< d1->id << " and "  << d2->id<< std::endl;
                //std::cout << "Before Improvement in 2OptStar : total " << s.GetCost() <<"  travel "<< s.GetTravelCost() <<" 3pl "<< s.GetTplCost()<< "Shift: " << s.GetShiftCost()<<std::endl;

                  if(BesMove.DeltaCostTotal < 0){
                  // std::cout << " 2Pt cost "<< BesMove.DeltaCostTotal << std::endl;
                }
               // s.Show(d1);
               // s.Show(d2);

                s.SwapTwoChains(BesMove.n1,s.DriverPrev[d1->EndNodeID],BesMove.n2,s.DriverPrev[d2->EndNodeID]);
//Updates here
//shift updates should be before
                d1->shift = BesMove.Shift_toD1;
                d1->Shift_Cost = ((BesMove.Shift_toD1.end - BesMove.Shift_toD1.start)/ 60) *d1->salary;
                d2->shift = BesMove.Shift_toD2;
                d2->Shift_Cost = ((BesMove.Shift_toD2.end - BesMove.Shift_toD2.start)/ 60) *d2->salary;

                s.Update(d1);
                s.Update(d2);
                s.UpdateNodeCumulativeDemand(d1);
                s.UpdateNodeCumulativeDemand(d2);
                //s.Update();

                d1->shift = BesMove.Shift_toD1;
                d1->Shift_Cost = ((BesMove.Shift_toD1.end - BesMove.Shift_toD1.start)/ 60) *d1->salary;
                d2->shift = BesMove.Shift_toD2;
                d2->Shift_Cost = ((BesMove.Shift_toD2.end - BesMove.Shift_toD2.start)/ 60) *d2->salary;
//Updates here

                _mv_count++;
                _found_imprv = found_imprv = found = true;

                //std::cout << "Before Improvement in 2OptStar : total " << s.GetCost() <<"  travel "<< s.GetTravelCost() <<" 3pl "<< s.GetTplCost()<<"Shift: " << s.GetShiftCost()<<std::endl;

                //std::cout << "After Improvement in 2OptStar : node n1: " << BesMove.n1->id <<"  node n2: "<< BesMove.n2->id <<" - delta cost: "<< BesMove.DeltaCostTotal<< std::endl;

                //s.Show(d1);
                //s.Show(d2);
               // }
            }
        }//end while found impvr
        return found_imprv;
    }



    void DriversCostAndFeasibility(Sol &s,Node *n1, Node *n2, Driver *d1, Driver*d2, Move2Chain &tmp){

        tmp.IsFeasible = false;
        tmp.from =d1;
        tmp.to =d2;
        tmp.n1 = n1;
        tmp.n2= n2;

        // std::vector<Node*> NewRoute1;
        //std::vector<Node*> NewRoute2;
        std::vector<Node*> NewRoute11 = s.FindNewRouteAfterRemoveNodeChain(n1, s.DriverPrev[d1->EndNodeID], d1);
        std::vector<Node*> NewRoute22 = s.FindNewRouteAfterRemoveNodeChain(n2, s.DriverPrev[d2->EndNodeID], d2);

        std::vector<Node*> NewRoute1 = s.FindNewRouteAfterInsertionChainToVector(s.DriverPrev[n1->id], n2, s.DriverPrev[d2->EndNodeID], d1,  NewRoute11);
        std::vector<Node*> NewRoute2 = s.FindNewRouteAfterInsertionChainToVector(s.DriverPrev[n2->id], n1, s.DriverPrev[d1->EndNodeID], d2,  NewRoute22);



        //s.FindNewRouteAfterExchangeChain(n1, s.DriverPrev[d1->EndNodeID], n2, s.DriverPrev[d2->EndNodeID], d1, d2, NewRoute1, NewRoute2);
        // std::vector<Node*> NewRoute1 = s.FindNewRouteAfterExchangeChain(n2, n1,s.DriverPrev[d1->EndNodeID] , d1);
        //std::vector<Node*> NewRoute2 = s.FindNewRouteAfterExchangeChain(n1, n2,s.DriverPrev[d2->EndNodeID] , d2);

        Shift shiftD1 = s.FindShortestShift(NewRoute1, d1);
        Shift shiftD2 = s.FindShortestShift(NewRoute2, d2);


        if (shiftD1.end == -1 ||shiftD2.end == -1  ) {
            return; // Not feasible
        } else {

            double AfterInsertionShiftCostD2 = ((shiftD2.end - shiftD2.start) / 60.0) * d2->salary;
            double AfterInsertionShiftCostD1 = ((shiftD1.end - shiftD1.start) / 60.0) * d1->salary;
            double deltaShift = AfterInsertionShiftCostD2 + AfterInsertionShiftCostD1 - d2->Shift_Cost - d1->Shift_Cost;


            // Calculate the delta route cost
           // double deltaDist =  s.Distance(n1, s.DriverNext[n2->id]) + s.Distance(n2, s.DriverNext[n1->id])+ s.Distance(s.DriverPrev[d1->EndNodeID], s.GetNode(d2->EndNodeID)) +   s.Distance(s.DriverPrev[d2->EndNodeID], s.GetNode(d1->EndNodeID))-
                   // s.Distance(n1, s.DriverNext[n1->id]) - s.Distance(n2, s.DriverNext[n2->id]) - s.Distance(s.DriverPrev[d1->EndNodeID],s.GetNode(d1->EndNodeID) )- s.Distance(s.DriverPrev[d2->EndNodeID],s.GetNode(d2->EndNodeID) );
            Node *e1 = s.DriverPrev[d1->EndNodeID];
            Node *e2 = s.DriverPrev[d2->EndNodeID];
            double deltaDist =  - s.Distance(s.DriverPrev[n1->id], n1) - s.Distance(s.DriverPrev[n2->id], n2)
                    - s.Distance(e1, s.DriverNext[e1->id]) - s.Distance(e2, s.DriverNext[e2->id])
                    + s.Distance(e1, s.DriverNext[e2->id]) + s.Distance(e2, s.DriverNext[e1->id])
                    + s.Distance(s.DriverPrev[n1->id], n2) + s.Distance(s.DriverPrev[n2->id], n1);
           // std::cout << " deltaDist "<< deltaDist << std::endl;
            double deltaTplCost = 0; // No 3PL insertion cost in this case
            double newDeltaCost = deltaDist + deltaTplCost + deltaShift;

            tmp.DeltaCostTotal = newDeltaCost;//Total
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
    int _mv_count;
    unsigned long long _nb_passes;
    bool _found_imprv;

    Opt2StarTwCost opt;
};






#endif //VRPD3FEB_OPT2STARTW_H
