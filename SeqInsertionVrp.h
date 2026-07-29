#ifndef SEQUENTIAL_INSERTION_VRP_H
#define SEQUENTIAL_INSERTION_VRP_H

#include <iostream>

#include "mathfunc.h"
#include "InsRmvMethod.h"
#include "InsertOperator.h"
#include "Move.h"
#include "Solution.h"

#include "ListMoveRvrp.h"
#include "MoveVrp.h"

class SeqInsertion : public InsertOperator<Node, Driver> {
public:
    SeqInsertion(Data &dat, InsRmvMethod<Node, Driver, MoveVrp> &insrmv)
            : _data(dat),
              _insrmv(insrmv) {}

    void Insert(Sol &s) override;

private:

    void InsertNode(Sol &s, Node *n);

    void UpdateDriverShift(Sol &s);


    void FillMoves(Sol &s, Node *n, Driver *d, ListMoveRvrp &list);

    InsRmvMethod<Node, Driver, MoveVrp> &_insrmv;
    Data &_data;
};

#endif
