
#ifndef VRPD16JAN_NODE_H
#define VRPD16JAN_NODE_H

#include <map>
#include <iostream>
#include <iostream>
#include "Parameters.h"

class Node {
public:
    int id, no, nodeID, custID{-1}, type, siteID, tempID{-1};
    double x, y, demand, ready, due, serviceTime, cumulative_dmd{0}, cumDistance{0}, fixedCost, perturbDis;
    int position{0};

    int distID, StartNodeID, EndNodeID;


    Node() : id(-1), no(-1), nodeID(-1), x(0), y(0), StartNodeID(-1), EndNodeID(-1),
             distID(0), demand(0), ready(0), due(0), serviceTime(0), type(-1), fixedCost(0) ,perturbDis(0){}

    virtual ~Node() = default;

    friend std::ostream &operator<<(std::ostream &os, const Node &node) {
        os << "id: " << node.id << " no: " << node.no << " nodeID: " << node.nodeID << "(" << node.x << "," << node.y
           << ") " << " distID: " << node.distID << " Demand " << node.demand << " ready " << node.ready << " due "
           << node.due << " serviceTime " << node.serviceTime;
        return os;
    }

    virtual void Show() const {
        std::cout << *this << std::endl;
    }
};

#endif //VRPD16JAN_NODE_H
