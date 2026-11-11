//
// Created by Razieh Mousavi on 2024-03-01.
//

#include "InsRmvMethodShift.h"

#include <iostream>
#include <algorithm>
#include "mathfunc.h"


InsRmvMethodShift::InsRmvMethodShift(Data &dat) {
    _added_noise = dat.maxDistance;
    _added_noise *= Parameters::getRandomDoubleValue(0.05, 0.5);
//    SetNoise(0.05);
}

// This function also works but
void InsRmvMethodShift::InsertCost(Sol &s, Node *n, Driver *d, Move<Node, Driver, MoveVrp> &mo) {
    mo.IsFeasible = false;
    mo.DeltaCost = INFINI;
    mo.n = n;
    mo.to = d;



    Node *prev = s.GetNode(d->StartNodeID);
    while (prev->type != Parameters::TypeNode::END_LINK) {
        Node *next = s.DriverNext[prev->id];


        if (n->id == 10 && d->id == 2 && prev->id == 15) {
            std::cout << " node " << n->id << "driver: " << d->id << "after node: " << prev->id << std::endl;
        }


        if (!Constraints::CheckConstraints(s, {n}, {}, d)) {// check capacity
            // prev = next;///////
            break; //do we need to update current time?
        }
        double ArrivalTo_n = 0;
        ArrivalTo_n = s.AvailabilityDepartureTime[prev->id] + s.Distance(prev, n);
        if (ArrivalTo_n > n->due) {
            prev = next;
            break;
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

        // std::cout << "Insert cost-  Driver " << d->id<< "node: " << n->id<<"after: "<< prev->id<<std::endl;
        Shift shift = s.FindShortestShift(NewRoute, d);


        //find new shift
        // if (shift.start ==-2){
        //    break;
        //}//empty route
        if (shift.end == -1) { break; }//not feasible
        else {
            AfterInsertionShiftCost = ((shift.end - shift.start) / 60) * d->salary;
            deltaShift = AfterInsertionShiftCost - d->Shift_Cost;//////////////////////???????????????d->Shift_Cost
        }


        double deltaDist = s.Distance(prev, n) + s.Distance(n, next) - s.Distance(prev, next);
        double deltaTplCost = 0;// this part is different from initial solution, we dont insert in 3pl
        double newDeltacost = deltaDist + deltaTplCost + deltaShift;

        // counter=counter+1;////////////////
        //std::cout <<" line "<< counter << std::endl;/////////////

        if (newDeltacost <
            mo.DeltaCost) {//  mo.DeltaCost = INFINI; just to do the insert move, later will be checked with sum of remove and insert
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




/*void InsRmvMethodShift::InsertAndRemoveCostOneRoute(Sol &s, Node *n, Driver *d, Move<Node, Driver, MoveVrp> &mo) {
    mo.IsFeasible = false;
    mo.DeltaCost = INFINI;
    mo.n = n;
    mo.to = d;
    mo.from = d;
    mo.pre1 = s.DriverPrev[n->id];
    mo.nxt1 = s.DriverNext[n->id];



    Node *prev = s.GetNode(d->StartNodeID);
    while (prev->type != Parameters::TypeNode::END_LINK) {
        Node *next = s.DriverNext[prev->id];


       if(next->id == n->id ){
           prev = s.DriverNext[n->id];
           next = s.DriverNext[prev->id];
           if(next == NULL){
               break;
           }
       }

        double deltaShift = 0;
        double AfterInsertionShiftCost = 0;
        std::vector<Node*> routeNodes = s.FindNewRouteAfterRemoveAndInsertNode(prev,  n,  next,  d);
        Shift shiftN = s.FindShortestShift(routeNodes, d);

        if (shiftN.end == -1) { break; }//not feasible
        else {
            AfterInsertionShiftCost = ((shiftN.end - shiftN.start) / 60) * d->salary;
            deltaShift = AfterInsertionShiftCost - d->Shift_Cost;//////////////////////???????????????d->Shift_Cost
        }






        double deltaDist = -s.Distance(mo.pre1, n) - s.Distance(n, mo.nxt1) + s.Distance(mo.pre1, mo.nxt1)
                                                                              +s.Distance(prev, n) + s.Distance(n, next) - s.Distance(prev, next);
        double deltaTplCost = 0;// this part is different from initial solution, we dont insert in 3pl
        double newDeltacost = deltaDist + deltaTplCost + deltaShift;

        // counter=counter+1;////////////////
        //std::cout <<" line "<< counter << std::endl;/////////////

        if (newDeltacost <
            mo.DeltaCost) {//  mo.DeltaCost = INFINI; just to do the insert move, later will be checked with sum of remove and insert
            mo.DeltaDistance = deltaDist;
            mo.DeltaCost = newDeltacost;
            mo.IsFeasible = true;

            mo.Shift_to = shiftN;
            mo.Shift_from = d->shift;
            mo.move.prev = prev;// function works with this, I didnt change
            mo.pre2 = prev;
            mo.nxt2 = next;
        }
        prev = next;
    }

}
*/







// This function also works but
/*
void InsRmvMethodShift::InsertCost(Sol &s, Node *n, Driver *d, Move<Node, Driver, MoveVrp> &mo) {
    mo.IsFeasible = false;
    mo.DeltaCost = INFINI;
    mo.n = n;
    mo.to = d;

   // Shift SaveShift= s.GetDriver(d->id)->shift;
   //s.SetDriverShiftToLargest(d);

    Node *prev = s.GetNode(d->StartNodeID);
    double currentTime = d->shift.start; // Start time of driver's shift

    // int counter=0;////////////

    while (prev->type != Parameters::TypeNode::END_LINK) {
        Node *next = s.DriverNext[prev->id];

        if (!Constraints::CheckConstraints(s, {n}, {}, d)) {// check capacity
            prev = next;///////
            break;   //////  do we need to update current time?
        }

        currentTime = std::max(prev->ready, currentTime) + prev->serviceTime + s.Distance(prev, n);
        if (currentTime > std::min(n->due, d->shift.end) ) {
            prev = next;///////
            continue;//
        }
        currentTime = std::max(n->ready, currentTime) + n->serviceTime + s.Distance(n, next);
        Node *nn = next;
        bool violation = false;
        double temp = currentTime;

        if (temp > std::min(nn->due ,d->shift.end))
        {
            violation = true;
            break;//////
        }

        while (nn->type != Parameters::TypeNode::END_LINK) {
            if (temp > std::min(nn->due ,d->shift.end))
            {
                violation = true;
                break;//////
            }
            temp = std::max(nn->ready, temp) + nn->serviceTime + s.Distance(nn, s.DriverNext[nn->id]);
            nn = s.DriverNext[nn->id];
        }
        if(violation){
            prev = next;
            continue;
        }

//        if (nn->type = Parameters::TypeNode::END_LINK) {
        //feasible;

        //calculate insert cost
        double deltaDist = s.Distance(prev, n) + s.Distance(n, next) - s.Distance(prev, next);
       // double newcost = deltaDist;

        // counter=counter+1;////////////////
        //std::cout <<" line "<< counter << std::endl;/////////////
        mo.move.prev=prev;///////
        mo.Shift_to = s.FindNewShiftAfterInsertion(n, mo.move.prev);
        double deltaShiftCost;
        if (s.RoutesLength[d->id]<0.00001)
            deltaShiftCost= ((mo.Shift_to.end- mo.Shift_to.start)/60)* d->salary;
        else
            deltaShiftCost= (((mo.Shift_to.end- mo.Shift_to.start)-(SaveShift.end - SaveShift.start)) / 60) * d->salary;
            //deltaShiftCost= (((mo.Shift_to.end- mo.Shift_to.start)-(s.GetDriver(d->id)->shift.end - s.GetDriver(d->id)->shift.start)) / 60) * d->salary;
       // mo.DeltaCost= mo.DeltaCost+ deltaShiftCost;


        double newcost = deltaDist+ deltaShiftCost;

        if (newcost < mo.DeltaCost) {
            mo.DeltaDistance = deltaDist;
            mo.DeltaCost = newcost;
            mo.IsFeasible = true;
            mo.move.prev = prev;
            //   }
        }
//        }
//        currentTime = std::max(prev->ready, currentTime) + prev->serviceTime + s.Distance(prev, next);
        //next = prev; do we need to change currenttime here???????
        prev = next;
    }
    s.GetDriver(d->id)->shift = SaveShift;
}*/




void InsRmvMethodShift::FillCustomerList(Sol &s, std::vector<Node *> &list) {
    list.clear();
    for (int i = 0; i < s.GetUnassignedCustCount(); i++) {
        Node *n = s.GetUnassignedCust(i);
        if (Data::isCustomer(n))
            list.push_back(n);
    }
}


//void InsRmvMethodShift::ApplyRemoveInsertIntraRoute(Sol &s,
 //                                       Move<Node, Driver, MoveVrp> &m) {
 //       s.Remove(m.n);
 //   s.InsertAfter(m.n, m.move.prev);
//}

void InsRmvMethodShift::ApplyInsertMove(Sol &s,
                                        Move<Node, Driver, MoveVrp> &m) {
    if (m.from != nullptr) {
        s.Remove(m.n);
        m.from->sumDemand -= m.n->demand;
    } else if (s.IsUnassigned(m.n)) {
        s.RemoveFromUnassigneds(m.n);// update unassignedCust Vrp, unassignedcount and unassignedCustindex
    }
    if (m.move.prev != nullptr){
        s.InsertAfter(m.n, m.move.prev);
    }else {// assign to 3pl
        s.AddToUnassigneds(m.n);////////////new
    }
}

void InsRmvMethodShift::RemoveCost(Sol &s, Node *n, Move<Node, Driver, MoveVrp>& rmv){
    rmv.n= n;

    if (s.IsUnassigned(n)){// if n is 3PL
        rmv.DeltaCost= - s.GetData()->GetTplCost();
        rmv.from=NULL;
        rmv.Shift_from = NULL;
        rmv.Shift_to = NULL;
    }
    else{
        //find new shift after removing n
        std::vector<Node*> NewRoute = s.FindNewRouteAfterRemoveNodeN(n, s.GetDriverAssignedTo(n));
        Shift RemoveShift = s.FindShortestShift(NewRoute, s.GetDriverAssignedTo(n));

        double DeltaShiftCost;

        double driverNewShift;// = RemoveShift.end-RemoveShift.start;
        if(RemoveShift.start == -2)// if sift.first==-2
        {

            driverNewShift = 0;
        } else{
            driverNewShift = ((RemoveShift.end-RemoveShift.start)/60) * s.GetDriverAssignedTo(n)->salary;
        }

        double driverShiftOld = ((s.GetDriverAssignedTo(n)->shift.end - s.GetDriverAssignedTo(n)->shift.start)/60) * s.GetDriverAssignedTo(n)->salary;

        DeltaShiftCost = (driverNewShift -driverShiftOld);
        double DelatRoutCost = s.Distance(s.DriverPrev[n->id], s.DriverNext[n->id])- s.Distance(s.DriverPrev[n->id], n)- s.Distance(n,s.DriverNext[n->id]);

        rmv.DeltaCost= DelatRoutCost + DeltaShiftCost;
        rmv.from= s.GetDriverAssignedTo(n);
        rmv.Shift_from= s.GetDriverAssignedTo(n)->shift;// this is the best shift for this driver before remove

        if(RemoveShift.start == -2)// if sift.first==-2
        {
            rmv.Shift_to.start=0;
            rmv.Shift_to.end=0;

        }else{
            rmv.Shift_to = RemoveShift;// this is the best shift for this driver after remove
        }



        //  Driver= s.GetDriverAssignedTo(n);
        //rmv.move.from=  rmv.from= s.GetDriverAssignedTo(n);////

        // if 3pl, we only reduce one 3pl or else it is just routing cost of the node which changes
    }
}





