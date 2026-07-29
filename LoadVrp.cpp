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
#include <fstream>
#include <iostream>
#include <set>
#include <regex>
#include <functional>
#include <algorithm>

#include "LoadVrp.h"
#include "Parameters.h"

using namespace std;

void LoadVrp::LoadInstance(Data &dat) { //
    cout << "Started loading the instance... \n";
    std::string line;
    std::ifstream inputFile(dat.instance); // Replace with your actual file name
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open the file." << std::endl;
        exit(1);
    }

    // Read and ignore the name of the instance
    std::string instanceExactName{};
    inputFile>> instanceExactName;
    dat.SetInstance(instanceExactName);
    std::getline(inputFile, line);
    // Read and ignore the empty line
    std::getline(inputFile, line);
    // Read and ignore "CUSTOMER" and  lines
    std::getline(inputFile, line);
    int nbCustomer;
    inputFile >> nbCustomer;
    // Read and ignore "VEHICLE" and "NUMBER" lines
    std::getline(inputFile, line);
    std::getline(inputFile, line);
    std::getline(inputFile, line);
    // Read the number of vehicles and capacity
    int numVehicles;
    int capacity;
    inputFile >> numVehicles >> capacity;
    dat.SetNumVehicles(numVehicles);
    dat.SetCapacity(capacity);

    // Read and ignore the empty line
    std::getline(inputFile, line);
    std::getline(inputFile, line);
    std::getline(inputFile, line);
    std::getline(inputFile, line);

    // Read customer information

    // Read and process depot information
    Node depotNode;

    inputFile >> depotNode.id >> depotNode.x >> depotNode.y >> depotNode.demand >> depotNode.ready >> depotNode.due
              >> depotNode.serviceTime;
    depotNode.distID = depotNode.id;
    depotNode.no = depotNode.id + 1;
    depotNode.due = static_cast<int>(std::ceil(depotNode.due / Parameters::ShiftInterval)) * Parameters::ShiftInterval; // Round dueDate of the Depot to the closest number divisible by 30
    depotNode.type = Parameters::TypeNode::DEPOT;
    dat.AddDepot(depotNode);
    dat.SetHorizon(depotNode.due);

    for (int i = 0; i < nbCustomer; i++) {
        Node cust;
        inputFile >> cust.distID >> cust.x >> cust.y >> cust.demand >> cust.ready >> cust.due >> cust.serviceTime;
        cust.custID = i;
        cust.id = dat.GetNodeCount();
        cust.no = cust.id + 1;
        cust.type = Parameters::TypeNode::CUSTOMER;
        dat.AddCustomer(cust);
    }
    std::getline(inputFile, line);
    std::getline(inputFile, line);

    Node *dep = dat.GetDepot(0);

    int increase= 60;

    for (int i = 0; i < numVehicles; ++i) {
        Driver d;
        d.id = i;
        inputFile >> d.availability.first >> d.availability.second >> d.salary;

       // d.availability.first =1;//////////////
        //d.availability.second= 720;//////////900
        /*
        int start= d.availability.first;
        int end= d.availability.second;
        if (end + (increase/2) <= 720 && start - (increase/2) >= 0) {
            start = start - (increase/2); // Decrease start by increase/2
            end = end + (increase/2);     // Increase end by increase/2
        } else if (end + increase <= 720) {
            end = end + increase; // Only increase the end if start cannot decrease
        } else if (start - increase >= 0) {
            start = start - increase; // Only decrease the start if end cannot increase
        }*/


        /*


        if (d.availability.second + (increase/2) <= 900 && d.availability.first - (increase/2) >= 0) {
            d.availability.first = d.availability.first - (increase/2); // Decrease start by increase/2
            d.availability.second = d.availability.second + (increase/2);     // Increase end by increase/2
        } else if (d.availability.second + increase <= 900) {
            d.availability.second = d.availability.second + increase; // Only increase the end if start cannot decrease
        } else if (d.availability.first - increase >= 0) {
            d.availability.first = d.availability.first - increase; // Only decrease the start if end cannot increase
        }*/



        d.capacity = capacity;  // Setting the capacity for each driver
        //d.capacity = 400;
        //d.capacity = (int )(capacity * Parameters::getRandomDoubleValue(0.8,1));  // Setting the capacity for each driver
        //d.salary=Parameters::salary + Parameters::getRandomIntValue(0,5);
        //d.salary=Parameters::salary ;
        Node dep1;
        dep1.id = dat.GetNodeCount();
        dep1.no = dep->no;
        dep1.x = dep->x;
        dep1.y = dep->y;
        dep1.distID = dep->distID;
        dep1.ready=dep->ready;
        dep1.due=dep->due;

        Node dep2(dep1);
        dep2.id = dep1.id +1;
        dep1.type = Parameters::TypeNode::START_LINK;
        dep2.type = Parameters::TypeNode::END_LINK;

        d.StartNodeID = dep1.id;
        d.EndNodeID = dep2.id;
        d.distID = dep->distID;

        dat.AddNode(dep1);
        dat.AddNode(dep2);
        dat.AddDriver(d);

    }
    std::getline(inputFile, line);
    std::getline(inputFile, line);

    int minW, maxW;
    inputFile >> minW >> maxW;
   //minW= 1;///////////////////
    //maxW= 900;/////////////////
    dat.SetMinWorking(minW);
    dat.SetMaxWorking(maxW);
    inputFile.close();

    LoadVrp::ComputeMatrice(dat);

    LoadVrp::GenerateShifts(dat);// generate shifts
    LoadVrp::GenerateDriverShifts(dat);


}



void LoadVrp::ComputeMatrice(Data &dat) {
    int dim = dat.GetNodeCount();
    dat.SetDimension(dat.GetNodeCount());
    std::vector<std::vector<double>> distances(dim);
    std::vector<std::vector<double>> times(dim);
    for (int i = 0; i < dim; ++i) {
        times[i].resize(dim, 0);
        distances[i].resize(dim, 0);
    }
    double maxv = -INFINI;
    for (int i = 0; i < dat.GetNodeCount(); i++) {
        Node *ni = dat.GetNode(i);
        for (int j = 0; j < dat.GetNodeCount(); j++) {
            Node *nj = dat.GetNode(j);
            double dist = (int) round(sqrt((ni->x - nj->x) * (ni->x - nj->x) + (ni->y - nj->y) * (ni->y - nj->y)));
            distances[ni->distID][nj->distID] = dist;
            distances[nj->distID][ni->distID] = distances[ni->distID][nj->distID];
            times[ni->distID][nj->distID] = distances[ni->distID][nj->distID];
            times[nj->distID][ni->distID] = distances[nj->distID][ni->distID];

            if (dist > maxv && dist < INFINI)
                maxv = dist;
        }
    }
    dat.maxDistance = maxv;
    dat.SetTplCost(maxv* Parameters::TplSalary);//R



    dat.SetDistances(distances);
    dat.SetTimes(times);
}




void LoadVrp::GenerateShifts(Data &dat) {
    std::vector<Shift> shiftSet; // Use Shift instead of pairs
    for (int i = 0; i <= ((dat.GetHorizon() - dat.GetMinWorking()) / Parameters::ShiftInterval); ++i) {
        double start = (i * Parameters::ShiftInterval); // Start time of the shift
        for (int j = 0; j <= ((dat.GetMaxWorking() - dat.GetMinWorking()) / 60); ++j) {
            double end = start + dat.GetMinWorking() + j * 60; // End time of the shift
            if (end <= dat.GetHorizon()) {
                shiftSet.emplace_back(start, end); // Create a Shift object and add it to the vector
            }
        }
    }
    // If sorting is required
    std::sort(shiftSet.begin(), shiftSet.end(), [](const Shift &a, const Shift &b) {
        if (a.start != b.start) {
            return a.start < b.start;  // Sort by start time in increasing order
        }
        return (a.end - a.start) > (b.end - b.start);  // If start times are equal, sort by duration in decreasing order
    });

    // Assuming dat.SetShifts() can accept a vector of Shift objects
    dat.SetShifts(shiftSet);
}

void LoadVrp::GenerateDriverShifts(Data &dat) {
    const auto& shifts = dat.GetShifts();
    for (int k = 0; k < dat.GetDriverCount() ; k++){
       for (int j = 0; j < shifts.size(); j++) {
           if (shifts[j].start >= dat.GetDriver(k)->availability.first && shifts[j].end <= dat.GetDriver(k)->availability.second) {
               dat.GetDriver(k)->DriverShiftSet.push_back(shifts[j]);  // Assign the matching shift to the driver
          }
        }
   }
};
