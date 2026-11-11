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

#ifndef MoveRVRP_H_
#define MoveRVRP_H_

#include "Node.h"
#include "Driver.h"


class MoveVrp
{
public:
	Node* n;
	Node* prev;
    Node* next;
	Driver* to;
	Driver* from;
	double DeltaCost;
	double DeltaDistance;
	bool IsFeasible;
    Shift Shift_toD;
};

#endif
