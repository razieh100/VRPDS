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
#ifndef ROBUST_TWO_ELRP_PARAMETERS_H
#define ROBUST_TWO_ELRP_PARAMETERS_H

#define EPS 10e-4

#include <string>

#include <iostream>
#include <chrono>
#include <random>
#include <numeric>

#define INFINI std::numeric_limits<int>::max()

class Parameters
{
public:

    enum TypeNode
    {
        NODE,
        DEPOT,
        CUSTOMER,
        START_LINK,
        END_LINK,
    };
    static std::mt19937 RANDOM_GEN;

    static double getRandomDoubleValue(double a, double b);
    static int getRandomIntValue(int a, int b);

    static int salary;
    static double TplSalary;
    static int ShiftInterval;
    static int CountNoImprove;



};

#endif // ROBUST_TWO_ELRP_PARAMETERS_H
