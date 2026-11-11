//
// Created by Razieh Mousavi on 2024-06-14.
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

#ifndef VRPD3FEB_MOVE2CHAIN_H
#define VRPD3FEB_MOVE2CHAIN_H


#include "Node.h"
#include "Driver.h"
//#include "Shift.h"
#include <stdlib.h>
#include "mathfunc.h"

//template<class NodeT, class DriverT, class MoveT>
class Move2Chain {
public:
    Node* e1;
    Node* e2;
    Node* n1;
    Node* n2;
    Driver* from;
    Driver* to;
    double DeltaCostRemove = -.0001;
    double DeltaCostInsert = -.0001;
    double DeltaShiftD1;
    double DeltaShiftD2;
    double DeltaCostTotal = -.0001;
    double ShiftTotal;
    double RoutetTotal;
    bool IsFeasible;
    Shift Shift_toD1;
    Shift Shift_toD2;
    double Time = 0;
    // Shift Shift_fromD1;
    //Shift Shift_fromD2;
    std::vector<Node*> chain;// for exchange where we exchange a chain from 3pl with a driver nodes



    Move2Chain()
            : n1(nullptr),
              n2(nullptr),
              e1(nullptr),
              e2(nullptr),
              to(nullptr),
              from(nullptr),

              IsFeasible(false) {}

};





#endif //VRPD3FEB_MOVE2CHAIN_H
