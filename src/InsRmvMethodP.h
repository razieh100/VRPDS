//
// Created by Razieh Mousavi on 2024-07-09.
//

#ifndef VRPD3FEB_INSRMVMETHODP_H
#define VRPD3FEB_INSRMVMETHODP_H




#include <vector>
#include "Move.h"
#include "Solution.h"



template <class NodeT, class DriverT, class MoveT>
class InsRmvMethodP {
public:
    InsRmvMethodP() : _noise(0) {}
    virtual ~InsRmvMethodP() {}

    // Calculate the cost of inserting n in the route d, the insertion position is
    // in m
    virtual void InsertCost(Sol &s, NodeT* n, DriverT* d,
                            Move<NodeT, DriverT, MoveT>& m) = 0;

    //virtual void InsertC(Sol &s, NodeT* n, DriverT* d,
    //                     Move<NodeT, DriverT, MoveT>& m, Node *nx, Node *pr) = 0;

    //virtual void InsertC(Sol &s, NodeT* n, DriverT* d,Move<NodeT, DriverT, MoveT>& m, Node *next, Node *prev) = 0;


//   Insert the node in the solution s
    virtual void ApplyInsertMove(Sol& s,
                                 Move<NodeT, DriverT, MoveT>& m) = 0;

    virtual void ApplyRemoveMove(Sol& s,
                                 NodeT* n) = 0;

    // Calculate the gain of removing the customer from the solution
//  virtual void RemoveCost(Sol& s, NodeT* n,
//    Move<NodeT, DriverT, MoveT>& m) = 0;

//  virtual void CancelMove(Sol& s,
//    Move<NodeT, DriverT, MoveT>& m) {}

    virtual void RemoveCost(Sol & s, NodeT *n,  Move<NodeT,DriverT,MoveT>& rmv){}//????

    // Fill a list with all the customers we have to insert in the solution
    virtual void FillCustomerList(Sol& s,std::vector<NodeT*>& list) {}
//  virtual void FillSatelliteList(Sol& s,std::vector<NodeT*>& list) {}
    void SetNoise(double noise) { _noise = noise; }
    double GetNoise() { return _noise; }

private:
    double _noise;
};


#endif //VRPD3FEB_INSRMVMETHODP_H
