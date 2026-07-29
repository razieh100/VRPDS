//
// Created by Razieh Mousavi on 2024-04-11.
//

#ifndef VRPD3FEB_SOLVEASSIGNMENTPROBLEM_H
#define VRPD3FEB_SOLVEASSIGNMENTPROBLEM_H



/********************************************************************
 ********************************************************************
 **
 ** libhungarian by Cyrill Stachniss, 2004
 ** http://www2.informatik.uni-freiburg.de/~stachnis/misc.html
 **
 ** Modified and adapted from C to C++ by Justin Buchanan
 **
 ** Solving the Minimum Assignment Problem using the
 ** Hungarian Method.
 **
 ** ** This file may be freely copied and distributed! **
 **
 ** Parts of the used code was originally provided by the
 ** "Stanford GraphGase", but I made changes to this code.
 ** As asked by  the copyright node of the "Stanford GraphGase",
 ** I hereby proclaim that this file are *NOT* part of the
 ** "Stanford GraphGase" distrubition!
 **
 ** This file is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied
 ** warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 ** PURPOSE.
 **
 ********************************************************************
 ********************************************************************/

//#pragma once
#include <vector>
#include "Solution.h"
#include "MoveDepot.h"

namespace AssignmentProblemSolver {

    typedef enum {
        MODE_MINIMIZE_COST,
        MODE_MAXIMIZE_UTIL,
    } MODE;

    typedef enum {
        NOT_ASSIGNED,
        ASSIGNED,
    } ASSIGN;

    using Matrix = std::vector<std::vector<int>>;

    struct Result {
        // True if the algorithm completed and found a solution.
        bool success = false;
        // The solution
        Matrix assignment;
        // A normalized form of the input cost matrix.
        Matrix cost;
        // The costs incurred by the assignment
        int totalCost = 0;
    };

/**
 * Runs the hungarian algorithm on the input cost matrix and returns a result
 * containing the normalized (square) cost matrix and a solution if one was
 * found.
 */
    Result Solve(const Matrix &input, MODE mode);

    void SolveAssignment (Sol & s);

    void PrintMatrix(const Matrix &m);
    void DriversCostAndFeasibility(Sol &s,Driver *d1, Driver*d2, MoveDepot<class Node, class Driver, class MoveT> &tmp);


    };  // namespace Hungarian





#endif //VRPD3FEB_SOLVEASSIGNMENTPROBLEM_H
