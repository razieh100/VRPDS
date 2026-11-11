//
// Created by Razieh Mousavi on 2024-03-27.
//

#ifndef VRPD3FEB_INSRMVMETHODOPT_H
#define VRPD3FEB_INSRMVMETHODOPT_H


#include <algorithm>
#include <numeric>
#include <cassert>
#include "Driver.h"
#include "Node.h"
#include "InsRmvMethod.h"
#include "Parameters.h"
#include "Data.h"
#include "Solution.h"
#include "MoveVrp.h"

class InsRmvMethodOpt : public InsRmvMethod<Node, Driver, MoveVrp> {
public:
    InsRmvMethodOpt() : _added_noise(0) {}
    InsRmvMethodOpt(Data& prob);
    ~InsRmvMethodOpt() {}

    void InsertCost(Sol& s,
                    Node* n,
                    Driver* d,
                    Move<Node, Driver, MoveVrp>& m) override;
    void ApplyInsertMove(Sol& s, Move<Node, Driver, MoveVrp>& m)
    override;
    void FillCustomerList(Sol& s, std::vector<Node*>& list) override;

    void RemoveCost(Sol & s, Node *n,  Move<Node,Driver,MoveVrp>& rmv)override;


private:
    double _added_noise;
};







#endif //VRPD3FEB_INSRMVMETHODOPT_H
