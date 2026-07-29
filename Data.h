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
#ifndef VRPD16JAN_DATA_H
#define VRPD16JAN_DATA_H

#include <vector>
#include <memory>
#include "Driver.h"
#include "Node.h"

class Data {
public:
    Data(){}

    void GenerateAvailabilityAndShift();
    //void LoadInstance();
    void AddDepot(const Node &n) {
        _depots.push_back(GetNodeCount());
        _nodes.push_back(std::make_shared<Node>(n));
    }
    void AddCustomer(const Node &n) {
        _customers.push_back(GetNodeCount());
        _nodes.push_back(std::make_shared<Node>(n));
    }
    void AddNode(const Node &n) {
        _nodes.push_back(std::make_shared<Node>(n));
    }
    void AddDriver(Driver &d) {
        _drivers.emplace_back(d);
    }
    int GetNodeCount() const { return int(_nodes.size()); }
    int GetDepotCount() const { return int(_depots.size()); }
    int GetCustomerCount() const { return int(_customers.size()); }
    Node *GetDepot(int i) {
        return _nodes[_depots[i]].get();
    }
    Node *GetCustomer(int i) const {
        return _nodes[_customers[i]].get();
    }
    static bool isCustomer(Node *n) {
        return (n->type == Parameters::TypeNode::CUSTOMER);
    }
    static bool isDepot(Node *n) {
        return (n->type == Parameters::TypeNode::DEPOT);
    }
    Node *GetNode(int i) { return _nodes[i].get(); }

    int GetDriverCount() { return (int) _drivers.size(); }

    int GetShiftsCount(){return (int) _shiftSet.size();}

    Driver *GetDriver(int id) {
        return &_drivers[id];
    }
    double Distance(int from, int to) const {
        return _distances[from][to];
    }
    double Distance(Node *from, Node *to) const {
        return Distance(from->distID, to->distID);
    }

    int serviceT(int n){//new
        return _nodes[n]->serviceTime;
    }



    void SetTimes(std::vector<std::vector<double>> &times) { _times = times; }
    double maxDistance{};

    void SetDistances(const std::vector<std::vector<double>>& distances) {
        _distances = distances;
    }//added

    void SetShifts(const std::vector<Shift>& shiftSet) {
        _shiftSet = shiftSet;
    }
   // void SetShifts( std::vector<std::pair<double, double>> shiftSet) {
    //    _shiftSet = shiftSet;
   // }//added

    void SetDimension(int dim) {
        _dimension = dim;
    }



    // Getter for depots
    const std::vector<int>& GetDepots() const {//me
        return _depots;
    }
    // Getter for customers
    const std::vector<int>& GetCustomers() const {//me
        return _customers;
    }

    const std::vector<std::vector<double>>& GetDistances() const {
        return _distances;
    }

    const std::vector<std::vector<double>>& GetTimes() const {
        return _times;
    }


    // Setters
    void SetCapacity(int cap) { capacity = cap; }
    void SetNumVehicles(int numVehicles) { Num_vehicles = numVehicles; }
    void SetMaxWorking(int maxWorking) { Max_working = maxWorking; }
    void SetMinWorking(int minWorking) { Min_working = minWorking; }
    void SetHorizon (int horizon){Horizon=horizon;}
    void SetTplCost (double tplCost ){TplCost=tplCost;}
    void SetInstance(std::string _instanceExactName){instanceExactName = _instanceExactName;}


    // Getters


    double GetTplCost() const {return TplCost;};
    int GetCapacity() const { return capacity; }
    int GetNumVehicles() const { return Num_vehicles; }
    int GetMaxWorking() const { return Max_working; }
    int GetMinWorking() const { return Min_working; }
    int GetHorizon() const { return Horizon; }

    const std::vector<Shift>& GetShifts() const {
        return _shiftSet;
    }
   // const std::vector<std::pair<double, double>>& GetShifts() const {
    //    return _shiftSet;
    //}
    std::string instance{};
    std::string instanceExactName{};
    private:
    double TplCost;
    int capacity;
    int Num_vehicles;
    int Max_working;
    int Min_working;
    int Horizon;
    int _dimension;//added
    std::vector<int> _depots;
    std::vector<int> _customers;
    std::vector<Driver> _drivers;
    std::vector<std::shared_ptr<Node>> _nodes;
    std::vector<std::vector<double>> _distances;
    std::vector<std::vector<double>> _times;
   // std::vector<std::pair<double, double>> _shiftSet;//R
    std::vector<Shift> _shiftSet;
};




#endif //VRPD16JAN_DATA_H
