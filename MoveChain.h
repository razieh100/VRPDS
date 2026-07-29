//
// Created by Razieh Mousavi on 2024-05-24.
//

#ifndef VRPD3FEB_MOVECHAIN_H
#define VRPD3FEB_MOVECHAIN_H


#include "Node.h"
#include "Driver.h"
//#include "Shift.h"
#include <stdlib.h>
#include "mathfunc.h"

//template<class NodeT, class DriverT, class MoveT>
class MoveChain {
public:
    Node* s;
    Node* e;
    Node* n;
    Driver* from;
    Driver* to;
    double DeltaCostRemove = -.0001;
    double DeltaDistanceRemove;
    double DeltaShiftremove;
    double DeltaCostInsert = -.0001;
    double DeltaDistanceInsert;
    double DeltaShiftInsert;
    double deltaCostReAndIns;
    bool IsFeasible;
    Shift Shift_toD1;
    Shift Shift_toD2;
    double Time;
   // Shift Shift_fromD1;
    //Shift Shift_fromD2;



    MoveChain()
            : n(nullptr),
              s(nullptr),
              e(nullptr),
              to(nullptr),
              from(nullptr),
             // DeltaCost(INFINITE),
              IsFeasible(false) {}

    //bool operator<(const Move<NodeT, DriverT, MoveT> &m) const {
     //   return (DeltaCost < m.DeltaCost);
   // }

    //bool operator<(const Move<NodeT, DriverT, MoveT> *m) const {
    //    return (DeltaCost < m->DeltaCost);
    //}
};





#endif //VRPD3FEB_MOVECHAIN_H
