/* Copyright Jean-Francois Cote 2017
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
*/

#ifndef MOVE_RVRP_H_
#define MOVE_RVRP_H_
#include "Shift.h"
#include <stdlib.h>
#include "mathfunc.h"

template<class NodeT, class DriverT, class MoveT>
class Move {
public:
    NodeT *n;
    DriverT *to;
    DriverT *from;
    double DeltaCost;
    double DeltaDistance;
    bool IsFeasible;
    Shift Shift_to;
    Shift Shift_from;
    double time =0;
   // std::pair<int, int> Shift_to;
    //std::pair<int, int> Shift_from;


    MoveT move;

    Move()
            : n(nullptr),
              to(nullptr),
              from(nullptr),
              //DeltaCost(INFINITE),
              DeltaCost(-0.00001),
              IsFeasible(false) {}

    bool operator<(const Move<NodeT, DriverT, MoveT> &m) const {
        return (DeltaCost < m.DeltaCost);
    }

    bool operator<(const Move<NodeT, DriverT, MoveT> *m) const {
        return (DeltaCost < m->DeltaCost);
    }
};

#endif
