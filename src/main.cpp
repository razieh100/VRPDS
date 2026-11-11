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
#include <iostream>
#include <fstream>
#include <string>

#include "Data.h"
#include "Solver.h"
#include "LoadVrp.h"


//void generateTikzForInstance();

int main(int argc, char **argv) {
    std::string instanceName;

    Data data;
    if (argc > 1) {
        instanceName = argv[1];
        data.instance = instanceName;
        LoadVrp::LoadInstance(data);
        Solver::Run(data, data.instanceExactName);//changed
        exit(1);
    }



         //if no arguments
         std::string instanceListFile = "/Users/raziehmousavi/CLionProjects/vrpd3Feb/instances/small.txt";
         std::ifstream inFile(instanceListFile);
         if (!inFile) {
             std::cerr << "Unable to open file: " << instanceListFile << std::endl;
             return 1;
         }
         while (getline(inFile, instanceName)) {
             if (!instanceName.empty()) {
                 Data data;
                 data.instance = instanceName;
                 LoadVrp::LoadInstance(data);
                 Solver::Run(data, data.instanceExactName);
                  //generateTikzForInstance(data, instanceName);
             }

         }
         inFile.close();
         return 0;

}

















