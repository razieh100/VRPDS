//
// Created by Razieh Mousavi on 2024-07-09.
//

#include "InsRmvMethodPerturb.h"



#include <iostream>
#include <algorithm>
#include "mathfunc.h"


InsRmvMethodPerturb::InsRmvMethodPerturb(Data &dat) {
    _added_noise = dat.maxDistance;
    _added_noise *= Parameters::getRandomDoubleValue(0.05, 0.5);
//    SetNoise(0.05);
}

// This function also works but
void InsRmvMethodPerturb::InsertCost(Sol &s, Node *n, Driver *d, Move<Node, Driver, MoveVrp> &mo) {
    mo.IsFeasible = false;
    mo.DeltaCost = 0;//
    mo.n = n;
    mo.to = d;

    Node *prev = s.GetNode(d->StartNodeID);
    while (prev->type != Parameters::TypeNode::END_LINK) {
        Node *next = s.DriverNext[prev->id];

       // if (prev != pr && next != nx)
       // {

        if (!Constraints::CheckConstraints(s, {n}, {}, d)) {// check capacity
            // prev = next;///////
            break; //do we need to update current time?
        }
        double ArrivalTo_n = 0;
        ArrivalTo_n = s.AvailabilityDepartureTime[prev->id] + s.Distance(prev, n);
        if (ArrivalTo_n > n->due) {
            prev = next;
            continue;//break;
        }
        //double NewArrivalAtNext = std::max(ArrivalTo_n, n->ready) +n->serviceTime + s.Distance( n, next);
        double Beginning_n = std::max(ArrivalTo_n, n->ready);
        double ArrivalAtNext = Beginning_n + n->serviceTime + s.Distance(n, next);
        double DeltaBeginningAtNext = std::max(ArrivalAtNext, next->ready) -
                                      s.AvailabilityBeginningTime[next->id];//Beginning_n - s.DepartureTime[prev->id];//NewArrivalAtNext - s.ArrivalTime[next->id];

        if (DeltaBeginningAtNext > s.AvailabilitySlackTime[next->id]) {
            prev = next;
            continue;
        }
        // so far the insertion is feasible for the availability period but there might not be a shift with the required conditions
        double deltaShift = 0;
        double AfterInsertionShiftCost = 0;

        std::vector<Node *> NewRoute = s.FindNewRouteAfterInsertion(prev, next, n, d);
        Shift shift = s.FindShortestShift(NewRoute, d);


        //find new shift
        if (shift.end == -1) { break; }//not feasible
        else {
            AfterInsertionShiftCost = ((shift.end - shift.start) / 60) * d->salary;
            deltaShift = AfterInsertionShiftCost - d->Shift_Cost;//////////////////////???????????????d->Shift_Cost
        }


        double deltaDist = s.Distance(prev, n) + s.Distance(n, next) - s.Distance(prev, next);
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
       // } else{
        //    prev = next;
        //}
    }
}




void InsRmvMethodPerturb::FillCustomerList(Sol &s, std::vector<Node *> &list) {
    list.clear();
    for (int i = 0; i < s.GetUnassignedCustCount(); i++) {
        Node *n = s.GetUnassignedCust(i);
        if (Data::isCustomer(n))
            list.push_back(n);
    }
}

void InsRmvMethodPerturb::ApplyRemoveMove(Sol &s, Node* customer) {
    Driver* d = s.GetDriverAssignedTo(customer);

    if (d != nullptr) {
        s.RemoveAndUnassign(customer);
        //s.Remove(customer);
        d->sumDemand -= customer->demand;
    } else if (s.IsUnassigned(customer)) {
        return;//already to unassigned or 3pl, so keep it
       // s.RemoveFromUnassigneds(customer);// update unassignedCust Vrp, unassignedcount and unassignedCustindex
    }
    //s.InsertAfter(m.n, m.move.prev);
}






void InsRmvMethodPerturb::ApplyInsertMove(Sol &s,Move<Node, Driver, MoveVrp> &m) {
   // if (m.from != nullptr) {
    //    s.Remove(m.n);
    //    m.from->sumDemand -= m.n->demand;
   // } else if (s.IsUnassigned(m.n)) {
        s.RemoveFromUnassigneds(m.n);// update unassignedCust Vrp, unassignedcount and unassignedCustindex
   // }
    s.InsertAfter(m.n, m.move.prev);
}

void InsRmvMethodPerturb::RemoveCost(Sol &s, Node *n, Move<Node, Driver, MoveVrp>& rmv){
   // rmv.n= n;

    if (s.IsUnassigned(n)){
        return;
       // if n is 3PL
    //    rmv.DeltaCost= - s.GetData()->GetTplCost();
    //    rmv.from=NULL;
    //    rmv.Shift_from= NULL;
     //   rmv.Shift_to = NULL;
   }
    else{
        //find new shift after removing n
       // std::vector<Node*> NewRoute = s.FindNewRouteAfterRemoveNodeN(n, s.GetDriverAssignedTo(n));
       // Shift RemoveShift = s.FindShortestShift(NewRoute, s.GetDriverAssignedTo(n));
       // double DeltaShiftCost;

       // double driverNewShift;// = RemoveShift.end-RemoveShift.start;
       // if(RemoveShift.start == -2)// if sift.first==-2
       // {
       //     driverNewShift = 0;
       // } else{
        //    driverNewShift = ((RemoveShift.end-RemoveShift.start)/60) * s.GetDriverAssignedTo(n)->salary;
       // }

       // double driverShiftOld = ((s.GetDriverAssignedTo(n)->shift.end - s.GetDriverAssignedTo(n)->shift.start)/60) * s.GetDriverAssignedTo(n)->salary;

       // DeltaShiftCost = (driverNewShift -driverShiftOld);
       // double DelatRoutCost = s.Distance(s.DriverPrev[n->id], s.DriverNext[n->id])- s.Distance(s.DriverPrev[n->id], n)- s.Distance(n,s.DriverNext[n->id]);

      //  rmv.DeltaCost= DelatRoutCost + DeltaShiftCost;
      //  rmv.from= s.GetDriverAssignedTo(n);
       // rmv.Shift_from= s.GetDriverAssignedTo(n)->shift;// this is the best shift for this driver before remove
      //  rmv.Shift_to = RemoveShift;// this is the best shift for this driver after remove

//_insrmv->ApplyInsertMove(s, best);// remove from unassined is done here, apply remove
/////////////////////////// Update shift here
       // if(rmv.from != NULL)//Update Shifttttttttttttttttttttttttt
       // {
        //    s.GetDriver(rmv.from->id)->shift =  rmv.Shift_to;
      //  }

        //////////////Update
       // if(rmv.from != NULL){
        //    s.Update(rmv.from);
       // }

    }
}

