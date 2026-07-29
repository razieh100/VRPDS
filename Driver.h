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
#ifndef VRPD16JAN_DRIVER_H
#define VRPD16JAN_DRIVER_H

#include <iostream>
#include "Shift.h"
#include <vector>

class Driver {
public:
    int id, no, StartNodeID, EndNodeID,
            distID;
    double  load, usage_cost{1}, curDistance{0},
            sumDemand{0},   load_cost{0}, capacity, salary{0}, Shift_Cost{0}, Travel_cost{0}, cost, BestShift_Cost{0};
    std::pair<int,int> availability;
    Shift shift;

    Shift Bestshift;

    std::vector<Shift> DriverShiftSet;
    int nbCustomers{0};

    Driver() : id(-1), no(-1), StartNodeID(0), EndNodeID(-1), distID(-1),

               load(0), usage_cost(0), availability(0, 0) {} //DriverShift(-1, -1)

    void Init() {
        load = 0;
        curDistance = 0;
        nbCustomers = 0;
        load_cost = 0;
        sumDemand = 0;
    }

    friend std::ostream &operator<<(std::ostream &os, const Driver &driver) {
        os << "Driver id: " << driver.id << " no: " << driver.no
           << " capacity: " << driver.capacity;
        return os;
    }

    const std::vector<Shift>& GetDriverShiftSet() const {
        return DriverShiftSet;
    }

};




#endif //VRPD16JAN_DRIVER_H
