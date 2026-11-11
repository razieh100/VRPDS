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

#ifndef INSRMVFAST_2ELRP_H
#define INSRMVFAST_2ELRP_H

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

class InsRmvMethodDistance : public InsRmvMethod<Node, Driver, MoveVrp> {
public:
  InsRmvMethodDistance() : _added_noise(0) {}
  InsRmvMethodDistance(Data& prob);
  ~InsRmvMethodDistance() {}

  void InsertCost(Sol& s,
    Node* n,
    Driver* d,
    Move<Node, Driver, MoveVrp>& m) override;
  void ApplyInsertMove(Sol& s, Move<Node, Driver, MoveVrp>& m)
  override;
  void FillCustomerList(Sol& s, std::vector<Node*>& list) override;

  void RemoveCost(Sol & s, Node *n,  Move<Node,Driver,MoveVrp>& rmv)override;//????


private:
  double _added_noise;
};

#endif
