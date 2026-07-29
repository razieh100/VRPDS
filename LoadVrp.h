
#ifndef ROBUST_TWO_ELRP_LOADRVRP_H
#define ROBUST_TWO_ELRP_LOADRVRP_H

#include "Data.h" //"../Data.h"
class LoadVrp {
public:
    static void LoadInstance(Data & dat);
private:
    static void ComputeMatrice(Data &dat);
    static void GenerateShifts(Data &dat);
    static void GenerateDriverShifts(Data &dat);

    static void SetDepotTimewindow(Data &dat);
};


#endif //ROBUST_TWO_ELRP_LOADRVRP_H
