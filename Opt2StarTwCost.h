//
// Created by Razieh Mousavi on 2024-04-04.
//
#include "Solution.h"
#ifndef VRPD3FEB_OPT2STARTWCOST_H
#define VRPD3FEB_OPT2STARTWCOST_H



class Opt2StarTwCost{
public:
    bool CheckTWforOpt(Sol& s , Driver * d1, Driver * d2,  Node * n1, Node* n2);
    bool CheckTW(Sol& s , Driver * d1);

private:
    static double GetCost(Sol& s);
    static double GetCostTW(Sol& s,Driver * d);


};




#endif //VRPD3FEB_OPT2STARTWCOST_H
