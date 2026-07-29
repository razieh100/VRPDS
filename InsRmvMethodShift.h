//
// Created by Razieh Mousavi on 2024-03-01.
//

#ifndef VRPD2FEB_INSRMVMETHODSHIFT_H
#define VRPD2FEB_INSRMVMETHODSHIFT_H



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

class InsRmvMethodShift : public InsRmvMethod<Node, Driver, MoveVrp> {
public:
    InsRmvMethodShift() : _added_noise(0) {}
    InsRmvMethodShift(Data& prob);
    ~InsRmvMethodShift() {}

    void InsertCost(Sol& s,
                    Node* n,
                    Driver* d,
                    Move<Node, Driver, MoveVrp>& m) override;
    void ApplyInsertMove(Sol& s, Move<Node, Driver, MoveVrp>& m)
    override;
    void FillCustomerList(Sol& s, std::vector<Node*>& list) override;




    void RemoveCost(Sol & s, Node *n,  Move<Node,Driver,MoveVrp>& rmv)override;

   // void ApplyRemoveInsertIntraRoute(Sol& s, Move<Node, Driver, MoveVrp>& m)
   // override;
    //void InsertAndRemoveCostOneRoute(Sol &s, Node *n, Driver *d, Move<Node, Driver, MoveVrp> &mo) override;


private:
    double _added_noise;
};











#endif //VRPD2FEB_INSRMVMETHODSHIFT_H
