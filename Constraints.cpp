
#include "Solution.h"
//#include "InsRmvMethodFastVrp.h"

using namespace std;


bool Constraints::CheckConstraints( Sol &s, const std::vector<Node *> &insert, const std::vector<Node *> &remove, Driver *d){

    bool CapacityCheck= CheckCapacity(s, insert,remove, d);
    return  CapacityCheck;
}

bool Constraints::TimewindowsCheck(Sol &s, Node *prev, Node *n, Node *next, Driver *d) {


        double currentTime = s.DepartureTime[prev->id] + s.Distance(prev, n);
        if (currentTime > std::min(n->due, d->shift.end)) {return false;}
        currentTime = std::max(n->ready, currentTime) + n->serviceTime + s.Distance(n, next);
        Node *nn = next;
        bool violation = false;
        double temp = currentTime;

        if (temp > std::min(nn->due, d->shift.end)) {return false;}
        while (nn->type != Parameters::TypeNode::END_LINK) {
            if (temp > std::min(nn->due, d->shift.end)) {
                return false;
            }
            temp = std::max(nn->ready, temp) + nn->serviceTime + s.Distance(nn, s.DriverNext[nn->id]);
            nn = s.DriverNext[nn->id];
        }
   // }
    return true;
}


bool
Constraints::CheckCapacity(Sol &s, const std::vector<Node *> &insert, const std::vector<Node *> &remove, Driver *d) {
    if (d->load + EPS >= d->capacity) {
        return false;
    }
    double new_worst_load = ComputeLoad(s, insert, remove, d);
    if (new_worst_load > d->capacity)
        return false;
    return true;
}

double Constraints::ComputeLoad(Sol &s, const std::vector<Node *> &insert, const std::vector<Node *> &
remove, Driver *d) {
    return ComputeDeterministicLoad(s, insert, remove, d);
}

double Constraints::ComputeDeterministicLoad(Sol &s, const std::vector<Node *> &insert, const std::vector<Node *> &
remove, Driver *d) {
    double load = d->load;
    for (auto n: insert) {
        load += s.GetDemand(n);
    }
    for (auto n: remove) {
        load -= s.GetDemand(n);
    }
    return load;
}

double Constraints::ComputeDeterministicLoad(const std::vector<Node *> &insert, const std::vector<Node *> &
remove) {
    double load = 0;
    for (auto n: insert) {
        load += n->demand;
    }
    for (auto n: remove) {
        load -= n->demand;
    }
    return load;
}
