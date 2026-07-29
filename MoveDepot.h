//
// Created by Razieh Mousavi on 2024-06-27.
//

#ifndef VRPD3FEB_MOVEDEPOT_H
#define VRPD3FEB_MOVEDEPOT_H



#include "Node.h"
#include "Driver.h"
//#include "Shift.h"
#include <stdlib.h>
#include "mathfunc.h"

template<class NodeT, class DriverT, class MoveT>
class MoveDepot {
public:
    //Node* s;
    //Node* e;
    //Node* n;
    Driver* driver;
    Driver* route;
    double CostShift = -.0001;
    bool IsFeasible;
    Shift Shift_toD1;
   // Shift Shift_toD2;
   std::vector<Node*> Route;




    MoveDepot()
            : //n(nullptr),
              //s(nullptr),
             // e(nullptr),
              route(nullptr),
              driver(nullptr),
            // DeltaCost(INFINITE),
              IsFeasible(false) {}

};






#endif //VRPD3FEB_MOVEDEPOT_H
