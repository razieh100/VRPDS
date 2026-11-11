/*
 * Copyright Jean-Francois Cote 2012
 *
 * The code may be used for academic, non-commercial purposes only.
 *
 * Please contact me at cotejean@iro.umontreal.ca for questions
 *
 * If you have improvements, please contact me!
 *
 */
#ifndef VRP_INSRMV_METHOD
#define VRP_INSRMV_METHOD

#include <vector>
#include "Move.h"
#include "Solution.h"



template <class NodeT, class DriverT, class MoveT>
class InsRmvMethod {
public:
    InsRmvMethod() : _noise(0) {}
    virtual ~InsRmvMethod() {}

    // Calculate the cost of inserting n in the route d, the insertion position is
    // in m
    virtual void InsertCost(Sol &s, NodeT* n, DriverT* d,
                            Move<NodeT, DriverT, MoveT>& m) = 0;





//   Insert the node in the solution s
    virtual void ApplyInsertMove(Sol& s,
                                 Move<NodeT, DriverT, MoveT>& m) = 0;



    virtual void RemoveCost(Sol & s, NodeT *n,  Move<NodeT,DriverT,MoveT>& rmv){}//????

    // Fill a list with all the customers we have to insert in the solution
    virtual void FillCustomerList(Sol& s,std::vector<NodeT*>& list) {}
//  virtual void FillSatelliteList(Sol& s,std::vector<NodeT*>& list) {}
    void SetNoise(double noise) { _noise = noise; }
    double GetNoise() { return _noise; }

private:
    double _noise;
};

#endif
