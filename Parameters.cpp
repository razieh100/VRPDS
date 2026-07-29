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
#include "Parameters.h"


//static std::mt19937 RANDOM_GEN;
//static const int seed_value = 40; // Choose any integer value as the seed
//std::mt19937 Parameters::RANDOM_GEN = std::mt19937(seed_value);


std::mt19937 Parameters::RANDOM_GEN =  std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());



double Parameters::getRandomDoubleValue(double a, double b) {
    std::uniform_real_distribution<> value = std::uniform_real_distribution<>(a, b);
    return value(Parameters::RANDOM_GEN);
}

int Parameters::getRandomIntValue(int a, int b) {
    std::uniform_int_distribution<> value = std::uniform_int_distribution<>(a, b);
    return value(Parameters::RANDOM_GEN);
}

int Parameters::salary = 10;    //R
double Parameters::TplSalary =1; //R
int Parameters::ShiftInterval = 30;
int Parameters::CountNoImprove = 0;


