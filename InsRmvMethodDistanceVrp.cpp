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
#include "InsRmvMethodDistanceVrp.h"
#include <iostream>
#include <algorithm>
#include "mathfunc.h"


InsRmvMethodDistance::InsRmvMethodDistance(Data &dat) {
    _added_noise = dat.maxDistance;
    _added_noise *= Parameters::getRandomDoubleValue(0.05, 0.5);
//    SetNoise(0.05);
}


// This function also works but
void InsRmvMethodDistance::InsertCost(Sol &s, Node *n, Driver *d, Move<Node, Driver, MoveVrp> &mo) {
    mo.IsFeasible = false;
    mo.DeltaCost = INFINI;//INFINI;
    mo.n = n;
    mo.to = d;

    Node *prev = s.GetNode(d->StartNodeID);
    while (prev->type != Parameters::TypeNode::END_LINK) {
        Node *next = s.DriverNext[prev->id];

        if (!Constraints::CheckConstraints(s, {n}, {}, d)) {// check capacity
            // prev = next;///////
            break; //do we need to update current time?
        }
        double ArrivalTo_n = 0;
        ArrivalTo_n = s.AvailabilityDepartureTime[prev->id] + s.Distance(prev, n);
        if ( ArrivalTo_n > n->due){
            prev = next;
            continue;
        }
        //double NewArrivalAtNext = std::max(ArrivalTo_n, n->ready) +n->serviceTime + s.Distance( n, next);
        double Beginning_n = std::max(ArrivalTo_n,n->ready )  ;
        double ArrivalAtNext= Beginning_n + n->serviceTime+ s.Distance(n, next);
        double DeltaBeginningAtNext = std::max(ArrivalAtNext, next->ready)-s.AvailabilityBeginningTime[next->id];//Beginning_n - s.DepartureTime[prev->id];//NewArrivalAtNext - s.ArrivalTime[next->id];

        if (DeltaBeginningAtNext > s.AvailabilitySlackTime[next->id]) {
            prev = next;
            continue;
        }
        // so far the insertion is feasible for the availability period but there might not be a shift with the required conditions
               double deltaShift = 0;
               double AfterInsertionShiftCost = 0;

               std::vector<Node*> NewRoute = s.FindNewRouteAfterInsertion(prev, next, n, d);
               Shift shift = s.FindShortestShift(NewRoute, d);


               //find new shift
               if (shift.end ==-1){break;}//not feasible
               else{
                   AfterInsertionShiftCost = ((shift.end - shift.start)/60)* d->salary;
                   deltaShift = AfterInsertionShiftCost - d->Shift_Cost;//////////////////////???????????????d->Shift_Cost
               }


            double deltaDist = s.Distance(prev, n) + s.Distance(n, next) - s.Distance(prev, next);
            //double deltaTplCost = 0 - s.GetData()->GetTplCost();
        double deltaTplCost = 0 - s.GetTplCostNode();

        double newDeltacost = deltaDist + deltaTplCost + deltaShift;

            // counter=counter+1;////////////////
            //std::cout <<" line "<< counter << std::endl;/////////////

            if (newDeltacost < mo.DeltaCost) {
                mo.DeltaDistance = deltaDist;
                mo.DeltaCost = newDeltacost;
                mo.IsFeasible = true;
                mo.move.prev = prev;
                mo.Shift_to = shift;
                mo.Shift_from = d->shift;
            }
        prev = next;
    }
}




void InsRmvMethodDistance::FillCustomerList(Sol &s, std::vector<Node *> &list) {
    list.clear();
    for (int i = 0; i < s.GetUnassignedCustCount(); i++) {
        Node *n = s.GetUnassignedCust(i);
        if (Data::isCustomer(n))
            list.push_back(n);
    }
}

void InsRmvMethodDistance::ApplyInsertMove(Sol &s,
                                           Move<Node, Driver, MoveVrp> &m) {
    if (m.from != nullptr) {
        s.Remove(m.n);
        m.from->sumDemand -= m.n->demand;
    } else if (s.IsUnassigned(m.n)) {
        s.RemoveFromUnassigneds(m.n);// update unassignedCust Vrp, unassignedcount and unassignedCustindex
    }
    s.InsertAfter(m.n, m.move.prev);
}

void InsRmvMethodDistance::RemoveCost(Sol &s, Node *n, Move<Node, Driver, MoveVrp>& rmv){

    if (s.IsUnassigned(n)){// if n is 3PL
        rmv.DeltaCost= - s.GetData()->GetTplCost();
        rmv.from=NULL;

    }
    //if not 3PL
    else{
        rmv.DeltaCost=s.Distance(s.DriverPrev[n->id], s.DriverNext[n->id])- s.Distance(s.DriverPrev[n->id], n)- s.Distance(n,s.DriverNext[n->id]);
        rmv.from= s.GetDriverAssignedTo(n);
     
    }
}




