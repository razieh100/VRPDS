//
// Created by Razieh Mousavi on 2024-03-27.
//

#include "InsRmvMethodOpt.h"

#include <iostream>
#include <algorithm>
#include "mathfunc.h"


InsRmvMethodOpt::InsRmvMethodOpt(Data &dat) {
    _added_noise = dat.maxDistance;
    _added_noise *= Parameters::getRandomDoubleValue(0.05, 0.5);
//    SetNoise(0.05);
}



// This function also works but
void InsRmvMethodOpt::InsertCost(Sol &s, Node *n, Driver *d, Move<Node, Driver, MoveVrp> &mo) {
    mo.IsFeasible = false;
    mo.DeltaCost = INFINI;
    mo.n = n;
    mo.to = d;

    Shift SaveShift= s.GetDriver(d->id)->shift;
    s.SetDriverShiftToLargest(d);

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


        //calculate insert cost
        double deltaDist = s.Distance(prev, n) + s.Distance(n, next) - s.Distance(prev, next);
       
        mo.move.prev=prev;///////
        mo.Shift_to = s.FindNewShiftAfterInsertion(n, mo.move.prev);
        double deltaShiftCost;
        if (s.RoutesLength[d->id]<0.00001)
            deltaShiftCost= ((mo.Shift_to.end- mo.Shift_to.start)/60)* d->salary;
        else
            deltaShiftCost= (((mo.Shift_to.end- mo.Shift_to.start)-(SaveShift.end - SaveShift.start)) / 60) * d->salary;
   


        double newcost = deltaDist+ deltaShiftCost;

        if (newcost < mo.DeltaCost) {
            mo.DeltaDistance = deltaDist;
            mo.DeltaCost = newcost;
            mo.IsFeasible = true;
            mo.move.prev = prev;
            //   }
        }
//        }

        prev = next;
    }
    s.GetDriver(d->id)->shift = SaveShift;
}




void InsRmvMethodOpt::FillCustomerList(Sol &s, std::vector<Node *> &list) {
    list.clear();
    for (int i = 0; i < s.GetUnassignedCustCount(); i++) {
        Node *n = s.GetUnassignedCust(i);
        if (Data::isCustomer(n))
            list.push_back(n);
    }
}

void InsRmvMethodOpt::ApplyInsertMove(Sol &s,
                                        Move<Node, Driver, MoveVrp> &m) {
    if (m.from != nullptr) {
        s.Remove(m.n);
        m.from->sumDemand -= m.n->demand;
    } else if (s.IsUnassigned(m.n)) {
        s.RemoveFromUnassigneds(m.n);// update unassignedCust Vrp, unassignedcount and unassignedCustindex
    }
    s.InsertAfter(m.n, m.move.prev);
}

void InsRmvMethodOpt::RemoveCost(Sol &s, Node *n, Move<Node, Driver, MoveVrp>& rmv){
    rmv.n= n;

    if (s.IsUnassigned(n)){// if n is 3PL
        rmv.DeltaCost= - s.GetData()->GetTplCost();
        rmv.from=NULL;
        rmv.Shift_from= NULL;
    }
    else{
        //find new shift after removing n
        Shift RemoveShift = s.FindNewShiftAfterRemoven(n, s.GetDriverAssignedTo(n));//rmv.from

        //double NewRouteEnd= s.FindNewShiftEnd(n);
        // find new shift
        double DeltaShiftCost;

        double driverNewShift;// = RemoveShift.end-RemoveShift.start;
        if(s.RoutesLength[s.GetDriverAssignedTo(n)->id] <= 1.1)
        {
            driverNewShift = 0;
        } else{
            driverNewShift = RemoveShift.end-RemoveShift.start;
        }

        double driverShiftDuration = s.GetDriverAssignedTo(n)->shift.end - s.GetDriverAssignedTo(n)->shift.start;

        DeltaShiftCost = (s.GetDriverAssignedTo(n)->salary)*(-driverShiftDuration+ driverNewShift)/60;//s.FindNewShiftCost(*s.GetDriverAssignedTo(n) , NewRouteEnd);


        double DelatRoutCost = s.Distance(s.DriverPrev[n->id], s.DriverNext[n->id])- s.Distance(s.DriverPrev[n->id], n)- s.Distance(n,s.DriverNext[n->id]);
        rmv.DeltaCost= DelatRoutCost + DeltaShiftCost;
        rmv.from= s.GetDriverAssignedTo(n);
        rmv.Shift_from= RemoveShift;


        
    }
}



