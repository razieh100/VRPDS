//
// Created by Razieh Mousavi on 2024-02-26.
//

#ifndef VRPD2FEB_RELOCATE_H
#define VRPD2FEB_RELOCATE_H


#include "Solution.h"
//#include "IAlgorithm.h"
#include "InsRmvMethod.h"
//#include "../../lib/mathfunc.h"
#include <vector>


template <class NodeT, class DriverT, class MoveT>
class Relocate
{


public:
    Relocate(InsRmvMethod<NodeT,DriverT,MoveT> * insrmv):_insrmv(insrmv){}

    void Optimize( Sol & s)
    {
        RelocateS(s);
        // RelocateIntra(s);
        s.Update();
        s.Show(1);
        std::cout <<" end optimize in relocate"<< std::endl;
    }

    void RelocateS(Sol & s)
    {
        _insrmv->SetNoise(0);
        s.Update();
        
        bool found = true;
        while(found)
        {//r++;
            //std::cout <<" oupsssssssssssssssssssssssss "<< std::endl;
            found = false;
            nodes.clear();
            for(int i = 0 ; i < s.GetCustomerCount() ; i++)
                nodes.push_back( s.GetCustomer(i) );
            //std::shuffle (vect.begin(), vect.end(), std::default_random_engine());

            std::cout <<" start iterating over nodes"<< std::endl;
            for(size_t i=0;i<nodes.size();i++)
            {

                //std::cout <<" node "<< i<<std::endl;

                Move<NodeT,DriverT,MoveT> rmv;

                _insrmv->RemoveCost(s, nodes[i],rmv);////// 3pl or in a route

                Move<NodeT,DriverT,MoveT> best;
                best.DeltaCost = INFINI;
                for(int j=0;j<s.GetDriverCount();j++)
                {

                    //std::cout <<" driver "<< j<<std::endl;
                    DriverT * d = s.GetDriver(j);
                    DriverT * d_firsr= s.GetDriverAssignedTo(nodes[i]);

                    if(d == d_firsr){ continue;}

                    Move<NodeT,DriverT,MoveT> tmp;
                    _insrmv->InsertCost(s,nodes[i],d,tmp); checked too

                    if (tmp.IsFeasible == false) {
                        continue;
                    }
                    

                    if(tmp.DeltaCost + 0.0001 < best.DeltaCost)
                        best = tmp;
                    
                }

                if (rmv.from != nullptr){

                    double TplInsertion = s.GetData()->GetTplCost();//insert cost
                    if(TplInsertion <  best.DeltaCost ){
                        best.n = nodes[i];
                        best.DeltaCost = TplInsertion;
                        best.IsFeasible = true;
                        best.move.prev = nullptr;
                        best.Shift_to.start = 0;
                        best.Shift_to.end = 0;
                        best.Shift_from = s.GetDriverAssignedTo(nodes[i])->shift;
                        best.to = nullptr;
                    }

                }


                if( best.IsFeasible == true && rmv.DeltaCost + best.DeltaCost < -0.0001)
                {
                    std::cout << "------------------------------------------------------------------------------------------------" << std::endl;

                    std::cout << "Remove Cost: " << rmv.DeltaCost  <<std::endl;
                    std::cout << "Insert Cost: " << best.DeltaCost <<std::endl;
                    std::cout << "Insert+Remove: " << best.DeltaCost+rmv.DeltaCost <<std::endl;

                    if (rmv.from!=NULL){// best dosnt have from, it has to. rmv has from
                        s.Show(rmv.from);
                    }
                    else{
                        std::cout << "node "<<  best.n <<  " ,from Tpl: " <<std::endl;}


                    if (best.to == nullptr){
                        std::cout << "To 3pl:  " << std::endl;
                    }else{
                        if (s.RoutesLength[best.to->id] < 0.00000000009 ){
                            std::cout << "Route-to has been empty before: route:  " << best.to->id<< std::endl;
                        } else{s.Show(best.to);
                        }
                    }

                    std::cout << "Old Cost: Totalcost: " << s.GetCost(1) << " Tpl cost: "<< s.GetTplCost()<< " Travel cost: "<<s.GetTravelCost() << " Shift cost: "<< s.GetShiftCost(1) << std::endl;
                    if(rmv.from!=NULL){//????????????
                        std::cout << "Shift from before: [" << s.GetDriver(rmv.from->id)->shift.start << ", " << s.GetDriver(rmv.from->id)->shift.end << " ]"<< std::endl;
                    }
                    if (best.to == nullptr) {
                        std::cout << "To 3pl: shift 0 " << std::endl;
                    }else{
                        std::cout << "Shift to before: [" << s.GetDriver(best.to->id)->shift.start << ", " << s.GetDriver(best.to->id)->shift.end << " ]"<< std::endl;
                    }
                    best.from = rmv.from;
                    _insrmv->ApplyInsertMove(s, best);


                    if (rmv.Shift_to.start==0 && rmv.from!= NULL){// when we remove node and route becomes empty
                        Driver* dr= s.GetDriver(rmv.from->id);
                        s.GetDriver(rmv.from->id)->shift.start = 0;
                        s.GetDriver(rmv.from->id)->shift.end = 0;
                        s.GetDriver(rmv.from->id)->Shift_Cost =0;
                    } else{
                        if(rmv.Shift_to.end>0.01 &&rmv.from != NULL)
                        {
                            s.GetDriver(rmv.from->id)->shift =  rmv.Shift_to;
                            s.GetDriver(rmv.from->id)->Shift_Cost = ((s.GetDriver(rmv.from->id)->shift.end-s.GetDriver(rmv.from->id)->shift.start)/60) * s.GetDriver(rmv.from->id)->salary;
                        }

                    }
                    if (best.to!= nullptr) {
                        s.GetDriver(best.to->id)->shift = best.Shift_to;
                        s.GetDriver(best.to->id)->Shift_Cost =
                                ((s.GetDriver(best.to->id)->shift.end - s.GetDriver(best.to->id)->shift.start) / 60) *
                                s.GetDriver(best.to->id)->salary;
                    }


                    if(rmv.from != NULL){
                        s.Update(rmv.from);
                    }
                    if (best.to != nullptr) {
                        s.Update(best.to);
                    }

                    // s.Update();
                    found = true;

                    if (rmv.from!=NULL){
                        s.Show(rmv.from);
                    }
                    else{std::cout << "node "<<  best.n->id <<  " ,from Tpl: " <<std::endl;}
                    if (best.to != nullptr) {
                        s.Show(best.to);
                    }
                    std::cout << "New Cost: Totalcost: " << s.GetCost(1) << " Tpl cost: "<< s.GetTplCost()<< " Travel cost: "<<s.GetTravelCost() << " Shift cost: "<< s.GetShiftCost(1) << std::endl;
                    if(rmv.from!=NULL){
                        std::cout << "Shift from after: [" << s.GetDriver(rmv.from->id)->shift.start << ", " << s.GetDriver(rmv.from->id)->shift.end << " ]"<< std::endl;
                    }
                    if (best.to != nullptr) {
                        std::cout << "Shift to after: [" << s.GetDriver(best.to->id)->shift.start << ", "
                                  << s.GetDriver(best.to->id)->shift.end << " ]" << std::endl;
                    }
                }
            }
        }

    }







private:
    InsRmvMethod<NodeT,DriverT,MoveT> * _insrmv;
    std::vector<NodeT*> nodes;
};




#endif //VRPD2FEB_RELOCATE_H



