
#include "Data.h"

//void Data::LoadInstance(){
 //   }
void Data::GenerateAvailabilityAndShift() {
    Shift ShiftT;
    ShiftT.start = 0;
    ShiftT.end = GetHorizon();

    _shiftSet.clear();
    _shiftSet.push_back(ShiftT);

    for (int k = 0; k < GetDriverCount() ; k++){
        GetDriver(k)->DriverShiftSet.clear();
        GetDriver(k)->availability.first = ShiftT.start;
        GetDriver(k)->availability.second = ShiftT.end;
        GetDriver(k)->shift = ShiftT;

        GetDriver(k)->DriverShiftSet.push_back(ShiftT);  // Assign the matching shift to the driver
    }

}

