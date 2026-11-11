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

#ifndef VRPD16JAN_SOLUTION_H
#define VRPD16JAN_SOLUTION_H

#include "Data.h"
#include "Driver.h"
#include "Node.h"

#include <cassert>


//#include "InsRmvMethodFastVrp.h"

class Sol {

public:
    Sol();
    // Copy constructor
    Sol(const Sol& other);//me
    // Copy assignment operator
    Sol& operator=(const Sol& other);//me


    explicit Sol(Data *data);

    void AssignStationToDriver();

    void AssignStationToDriver(Driver *d);
    int GetCustomerCount() const { return _data->GetCustomerCount(); }

    Node *GetCustomer(int i) { return _data->GetCustomer(i); }
    Node *GetDepot(int i) { return _data->GetDepot(i); }

    Node *GetNode(int i) { return _data->GetNode(i); }

    void AddToUnassigneds(Node *n);

    void AddDriver();

    void AddNode();
    void SetDemand(Node *n, double dmd) { ListDemand[n->id] = dmd; }
    double GetDemand(Node *n) const { return ListDemand[n->id]; }

    //void Update() {
     //   UpdateSolution();
    //}
    void Update();
    void Update(Driver *d);

    void UpdateSolution(Driver *d);
    void UpdateSolution();
    void ChageDeptTWtoAvailability();

    void AssignBestShift();


    void UpdateNodeCumulativeDemand(std::vector<Node *> Driver_Prev);

    void RevertPath(Node * n1, Node * n2);
    void SwapTwoChains(Node* s1, Node* e1, Node* s2, Node* e2);
    Shift FindNewShiftAfterSwap( Driver *driver );

    std::vector<Node*> FindNewRouteForDriver(  Driver* d);

    void UpdateOneDriverShift(Driver *d);
    void UpdateDepartureBeginningWaitingTimes(Driver *d);
    void UpdateAvailabilityDepartureBeginningWaitingTimes(Driver *d);
    void UpdateAvailabilitySlackNew(Driver *d);
    void UpdateAvailabilitySlackNew(Driver *d, Node *i);



    void UpdateArrivalTimes(Driver *d);
    void UpdateAvailabilityArrivalTimes(Driver *d);
    void UpdateShiftSlack(Shift inputShift,Driver *d);
    void UpdateAvailabilitySlack(std::pair<int, int> available, Driver *d);
    void UpdateAvailabilitySumWaitingTimes(Driver *d);

    Shift UpdateToShortestShift (Driver *d);
                                                                                                                                                                                                                                                                                                                                                                                              Shift FindShortestShift (std::vector<Node*> NewRoute, Driver *d );
    //std::vector<double> FindArrivalofRouteInsertion(Driver* d, std::vector<Node*>& routeNodes );
    void FindTimesInsertion(Driver* d,  std::vector<Node*>& routeNodes, std::vector<double>& arrivalTimes, std::vector<double>& departureTimes, std::vector<double>& beginningTimes, std::vector<double>& waitingTimes, std::vector<double>& sumWaitingTimes);
    void UpdateSlackForAllNodesInsertion(Driver *d,  std::vector<Node*>& routeNodes,  std::vector<double>& beginningTime,  std::vector<double>& waitingTime ,std::vector<double>& slackTimes);
    double UpdateSlackNewInsertion(Driver *d, Node *i,  std::vector<double>& beginningTime,  std::vector<double>& waitingTime,  std::vector<Node*>& routeNodes);
    std::vector<Node*> FindNewRouteAfterInsertionChain(Node* prev, Node* s1, Node* e1, Driver* d);
   // void FindNewRouteAfterExchangeChain(Node* s1, Node* e1, Node* s2, Node* e2, Driver* d1, Driver* d2, std::vector<Node*> &routeNodesD1, std::vector<Node*> &routeNodesD2);
   std::vector<Node*> FindNewRouteAfterInsertionChainToVector(Node* prev, Node* s1, Node* e1, Driver* d, std::vector<Node*>& NewRoute1);
    std::vector<Node*> FindNewRouteAfterRemoveDepot( Driver* d);
    std::vector<Node*> FindNewRouteAfterInsertionDepot(Node* startDepot, Node* endDepot, std::vector<Node*>& NewRoute1);


    void UpdateNodeCumulativeDemand(Driver* d);



    void SetDriverShiftToLargest(Driver *d);
    Shift FindNewShiftAfterRemoven(Node *node, Driver *driver);
    Shift FindNewShiftAfterInsertion(Node *node, Node *prev);
    Shift FindRouteStartAndEnd (Driver *d);
    Shift FindSmallestShiftForAssignment(Driver *d1, Driver *d2);
    std::vector<Node*> FindNewRouteAfterInsertion(Node* prev, Node* next, Node* n, Driver* d);
    std::vector<Node*> FindNewRouteAfterRemoveNodeN( Node* n, Driver* d);
    std::vector<Node*> FindNewRouteAfterRemoveNodeChain(Node* s1, Node* e1, Driver* d);
    std::vector<Node*> FindNewRouteAfterRemoveAndInsertNode(Node* prev, Node* n, Node* next, Driver* d);

    void RelocateChainNew(Node *s1, Node *e1, Node *n);
    double smallestDivisibleBy30AfterOrEqual(double s);
    void PrintSwappedChains(Sol &s, Node* n1, Node* e1, std::vector<Node*> &chain);
    //void generateTikzForInstance();
    void ReplaceChainWithUnassigned(Node* s1, Node* e1, std::vector<Node*> &chain);
    std::vector<Node*> FindNewRouteAfterSwap(Node* s1, Node* e1, Node* prev, std::vector<Node*>& chain, Driver* d);



        void RemoveAndUnassignChain(Node* s1, Node* e1);
    int CountNodesInChain(Node* s1, Node* e1);

    double FindNewShiftEnd(Node *node);// not used
    double FindNewShiftCost( Driver driver ,double NewShiftEnd);
    void AssignNextDriverShift();
   // void AssignShiftsToDrivers(std::vector<std::pair<double, double>> shiftSet);
   void AssignShiftsToDrivers(std::vector<Shift> shiftSet);//not used anymore
    void AssignAvailabilityToDrivers();

    //void SortShifts(std::vector<std::pair<double, double>>& shiftSet);

    void SortShifts(std::vector<Shift>& shiftSet);
    void UpdateDriverShift();

    void GenerateTikz(std::string instanceName);
    void Update(Node *n);
    void Relocate(Node *next, Node *prev);
    void RelocateChain(Node *s, Node *e, Node *n);
    void Swap(Node *next, Node *prev);
    void AssignRouteToDriver( Driver *d, const std::vector<Node *> &vect, std::vector<Node *> &_DriverPrev, std::vector<Node *> &_DriverNext);
    void DriverAssgnment();
    void Show(int a);
   // void ShowCost();//me
    void Show(Driver *d);
    void ShowDrivers();

    void Remove(Node *n);
    void PutAllCustomersToUnassigned();
    void RemoveAndInsert(Driver *d, const std::vector<Node *> &vect);
    bool IsUnassigned(Node *n);
    void RemoveAndUnassignCustomers();
    void InsertAfter(Node *n, Node *prev);
    int GetDriverCount() { return _data->GetDriverCount(); }
    int GetDriverCountAssigned();
    Driver *GetDriverAssignedTo(Node *n) {
        return DriverAssignTo[n->id];
    }
    Driver *GetDriver(int i) { return _data->GetDriver(i); }


    void RemoveAndUnassign(Node *n);
    void RemoveFromUnassigneds(Node *n);


    void RemoveAndInsert(Node *n, Node *prev);
   // void Perturb();

    double TotalDistances() const { return _total_distances; }

    double Distance(Node *from, Node *to) const {
        return _data->Distance(from, to);
    }

    int ServiceT(int Id){//new
        return _data->serviceT(Id);
    }

    double Distance(Node *from, Node *to, Driver *d) const {
        return _data->Distance(from, to);
    }
    double Distance(int from, int to) const { return _data->Distance(from, to); }

    double Distance(int from, int to, Driver *d) const {
        return _data->Distance(from, to);
    }

    Node *GetUnassignedCust(int i) { return UnassignedCustVRP[i]; }
    int GetUnassignedCustCount() const { return UnassignedCustCount; }


    double GetCost(int b);//total
    double GetTplCost();
    double GetTravelCost();
    double GetShiftCost(int a);
    double GetTravelCost(Driver *d);
    double GetShiftCost(Driver *d, int a);

    double GetTplCostNode();

    Data *GetData() { return _data; }
    void SetTotalDistances(double dist) { _total_distances = dist; }


    std::vector<int> RoutesLength;
    std::vector<int> UnassignedCustIndex;
    std::vector<double> ListDemand;
    std::vector<double> LoadQuantity;
    std::vector<Node *> DriverNext;
    std::vector<Node *> DriverPrev;
    std::vector<double > DepartureTime;
   // std::vector<double > ArrivalTime;
    //std::vector<double > BeginningTime;
    //std::vector<double > WaitingTime;
    //std::vector<double > SumWaitingTime;
    std::vector<double > AvailabilitySumWaitingTime;
   // std::vector<double > AvailabilityVectorForSlack;
    //std::vector<double > SlackTime;
    std::vector<double > AvailabilityDepartureTime;
    std::vector<double > AvailabilityArrivalTime;
    std::vector<double > AvailabilityBeginningTime;
    std::vector<double > AvailabilityWaitingTime;
    std::vector<double > AvailabilitySlackTime;

    double CalculateTotalAvailabilityWaitingTime();

    double duration_time{0};
    int bestItr{0};
private:
    Data *_data{nullptr};

    double Cost{0}, TplCost{0}, ShiftCost{0}, TravelCost{0};
    bool computeLastCost{false};
    std::vector<Driver *> DriverAssignTo;
    std::vector<Node *> Unassigned;
    std::vector<Node *> UnassignedCustVRP;


    std::vector<double> TraveledDistance;
    int UnassignedCustCount{0};
    double _total_distances{0};

   // void UpdateDepartureTimes(Driver *d);
    void UpdateUnassignedDriverShift(Driver *d);

// Helper function to copy data from another Sol object
    void copyFrom(const Sol& other);//me

};

class Constraints {
public:


    static bool CheckCapacity(Sol &s, const std::vector<Node *> &insert, const std::vector<Node *> &remove, Driver *d);

    static bool
    CheckConstraints(Sol &s, const std::vector<Node *> &insert, const std::vector<Node *> &remove, Driver *d);

    static double ComputeLoad(Sol &s, const std::vector<Node *> &insert, const std::vector<Node *> &remove, Driver *d);

    static double ComputeLoad(const std::vector<Node *> &insert, const std::vector<Node *> &remove);


    static bool TimewindowsCheck(Sol &s, Node *prev, Node *n, Node *next, Driver *d);

private:

    static double ComputeDeterministicLoad(Sol &s, const std::vector<Node *> &insert, const std::vector<Node *> &
    remove, Driver *d);

    static double ComputeFactorLoad(Sol &s, const std::vector<Node *> &insert, const std::vector<Node *> &
    remove, Driver *d);

    static double ComputeBudgetLoad(Sol &s, const std::vector<Node *> &insert, const std::vector<Node *> &
    remove, Driver *d);

    static double ComputeDeterministicLoad(const std::vector<Node *> &insert, const std::vector<Node *> &
    remove);

    static double ComputeFactorLoad(const std::vector<Node *> &insert, const std::vector<Node *> &
    remove);

    static double ComputeBudgetLoad(const std::vector<Node *> &insert, const std::vector<Node *> &
    remove);

    //static bool CheckTime(Sol &s, const std::vector<Node *> &insert, const std::vector<Node *> &remove, Driver *d);


};


#endif //VRPD16JAN_SOLUTION_H
