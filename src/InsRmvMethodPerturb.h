//
// Created by Razieh Mousavi on 2024-07-09.
//

#ifndef VRPD3FEB_INSRMVMETHODPERTURB_H
#define VRPD3FEB_INSRMVMETHODPERTURB_H




#include <algorithm>
#include <numeric>
#include <cassert>
#include "Driver.h"
#include "Node.h"
#include "InsRmvMethodP.h"
#include "Parameters.h"
#include "Data.h"
#include "Solution.h"
#include "MoveVrp.h"

class InsRmvMethodPerturb : public InsRmvMethodP<Node, Driver, MoveVrp> {
public:
    InsRmvMethodPerturb() : _added_noise(0) {}
    InsRmvMethodPerturb(Data& prob);
    ~InsRmvMethodPerturb() {}

    void InsertCost(Sol& s,
                    Node* n,
                    Driver* d,
                    Move<Node, Driver, MoveVrp>& m) override;

    void ApplyInsertMove(Sol& s, Move<Node, Driver, MoveVrp>& m)
    override;
    void ApplyRemoveMove(Sol& s, Node* n)
    override;
    void FillCustomerList(Sol& s, std::vector<Node*>& list) override;

    void RemoveCost(Sol & s, Node *n,  Move<Node,Driver,MoveVrp>& rmv)override;


private:
    double _added_noise;
};








#endif //VRPD3FEB_INSRMVMETHODPERTURB_H
