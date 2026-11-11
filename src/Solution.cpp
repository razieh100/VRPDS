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
#include "Solution.h"
#include "Driver.h"
#include <fstream>

#include <algorithm>
#include <vector>


Sol::Sol() = default;//DepartureTime

Sol::Sol(Data *data) : _data(data),
                       RoutesLength(data->GetDriverCount(), 0),
                       LoadQuantity(data->GetDriverCount(), 0),
                       TraveledDistance(data->GetDriverCount(), 0),
                       DriverAssignTo(data->GetNodeCount(), nullptr),
                       Unassigned(data->GetNodeCount(), nullptr),
                       DriverNext(data->GetNodeCount(), nullptr),
                       DriverPrev(data->GetNodeCount(), nullptr),
                       ListDemand(data->GetNodeCount(), 0),
                       DepartureTime (data->GetNodeCount(), 0),
                      // ArrivalTime (data->GetNodeCount(), 0),
                       //BeginningTime (data->GetNodeCount(), 0),
                      // WaitingTime (data->GetNodeCount(), 0),
                       //SlackTime (data->GetNodeCount(), 90000),
                       AvailabilitySlackTime (data->GetNodeCount(), 90000),

                      // SumWaitingTime (data->GetNodeCount(), 0),
                       AvailabilitySumWaitingTime (data->GetNodeCount(), 0),
                      // AvailabilityVectorForSlack (data->GetNodeCount(), 0),


                       AvailabilityDepartureTime(data->GetNodeCount(), 0),
                       AvailabilityArrivalTime(data->GetNodeCount(), 0),
                       AvailabilityBeginningTime(data->GetNodeCount(), 0),
                       AvailabilityWaitingTime(data->GetNodeCount(), 0),

                       UnassignedCustIndex(data->GetCustomerCount(), -1),
                       UnassignedCustVRP(data->GetCustomerCount(), nullptr),
                       UnassignedCustCount(data->GetCustomerCount()) {
    AssignStationToDriver();
    for (int i = 0; i < GetCustomerCount(); i++) {
        Node *c = GetCustomer(i);
        ListDemand[c->id] = c->demand;
    }
}




// Helper function to copy data from another Sol object
void Sol::copyFrom(const Sol& other) {
    _data = other._data;
    RoutesLength = other.RoutesLength;
    UnassignedCustIndex = other.UnassignedCustIndex;
    ListDemand = other.ListDemand;
    LoadQuantity = other.LoadQuantity;
    DriverNext = other.DriverNext;
    DriverPrev = other.DriverPrev;
    DepartureTime = other.DepartureTime;
    AvailabilitySumWaitingTime = other.AvailabilitySumWaitingTime;
    AvailabilityDepartureTime = other.AvailabilityDepartureTime;
    AvailabilityArrivalTime = other.AvailabilityArrivalTime;
    AvailabilityBeginningTime = other.AvailabilityBeginningTime;
    AvailabilityWaitingTime = other.AvailabilityWaitingTime;
    AvailabilitySlackTime = other.AvailabilitySlackTime;
    duration_time = other.duration_time;
    bestItr = other.bestItr;
    Cost = other.Cost;
    TplCost = other.TplCost;
    ShiftCost = other.ShiftCost;
    TravelCost = other.TravelCost;
    computeLastCost = other.computeLastCost;
    DriverAssignTo = other.DriverAssignTo;
    Unassigned = other.Unassigned;
    UnassignedCustVRP = other.UnassignedCustVRP;
    TraveledDistance = other.TraveledDistance;
    UnassignedCustCount = other.UnassignedCustCount;
    _total_distances = other._total_distances;
}

// Copy constructor
Sol::Sol(const Sol& other) {
    copyFrom(other);
}

// Copy assignment operator
Sol& Sol::operator=(const Sol& other) {
    if (this != &other) {
        // If you're using dynamic memory, free existing resources here

        // Copy data from the other object
        copyFrom(other);
    }
    return *this;
}






void Sol:: Update() {
  // UpdateSolution();
   // ShiftCost= 0;
   // TravelCost= 0;

    TplCost =0;
    Cost =0;
    ShiftCost=0;
    TravelCost=0;

    computeLastCost = false;
    SetTotalDistances(0);
    double sumd = 0;
    for (int i = 0; i < GetDriverCount(); i++) {
        Driver *d = GetDriver(i);
        Update(d);

        sumd += TraveledDistance[d->id];// + (int) (d->nbCustomers > 0) * d->usage_cost;
        // std::cout << "driver "<<d->id << ":" <<sumd <<std::endl;
        ShiftCost += d->Shift_Cost;// Cost
        TravelCost += d->Travel_cost; // Cost
    }
    //UpdateNodeCumulativeDemand, just updated in DriverPrev
    std::vector<Node *> Driver_Prev = DriverPrev;
    UpdateNodeCumulativeDemand( Driver_Prev);
    //end of UpdateNodeCumulativeDemand

    SetTotalDistances(sumd); //TODO

    TplCost = GetTplCost(); // Cost
    Cost = GetCost(1); // Cost
}


void Sol:: Update(Driver *d) {
    computeLastCost = false;
    UpdateSolution(d);
}


void Sol::UpdateSolution(Driver *d) {
    computeLastCost = false;


    d->Init();
    LoadQuantity[d->id] = 0;
    double myReversalDistance = 0.;
    int position = 0;

    bool hasNodes = false;
    std::vector<double> list_demand;
    Node *prev = GetNode(d->StartNodeID);
    prev->position = position++;//?

    while (prev->type != Parameters::END_LINK) {
        Node *next = DriverNext[prev->id];
        d->curDistance += Distance(prev, next, d);
        next->position = position++;
        if (next->type == Parameters::END_LINK) {
            prev = next;
            continue;
        }
        d->sumDemand += ListDemand[next->id];
        list_demand.push_back(ListDemand[next->id]);
        prev = next;
        hasNodes = true;
    }


    UpdateAvailabilityArrivalTimes(d);
    UpdateAvailabilityDepartureBeginningWaitingTimes(d);
    UpdateAvailabilitySumWaitingTimes(d);
    UpdateAvailabilitySlackNew(d);
    //UpdateAvailabilitySlack(d->availability, d);


    if (!hasNodes)
        return;
    d->load = d->sumDemand;
    LoadQuantity[d->id] = d->sumDemand;
    TraveledDistance[d->id] = d->curDistance;

    d->nbCustomers = position - 1;


    //cost
    d->Travel_cost = GetTravelCost(d);////update cost
    d->Shift_Cost = GetShiftCost(d, 1);//// update cost
    d->cost = d->Travel_cost + d->Shift_Cost;/// update cost


    if (d->load > d->capacity + EPS) {
        printf("******************************************************\n");
        printf("We shouldn't insert in this route if worst_case_load %f > "
               "cap:%2.1lf \n",
               d->load, d->capacity);
        printf("*******************p***********************************\n");
        Show(d);
        for (auto dmd: list_demand)
            printf(" %lf ", dmd);
        printf("\n");

        exit(1);
    }
}




void Sol::ChageDeptTWtoAvailability(){
    for (int i = 0; i < GetDriverCount(); i++)
    {
        GetNode(GetDriver(i)->StartNodeID)->ready = GetDriver(i)->availability.first;
        GetNode(GetDriver(i)->StartNodeID)->due = GetDriver(i)->availability.second;

        GetNode(GetDriver(i)->EndNodeID)->ready = GetDriver(i)->availability.first;
        GetNode(GetDriver(i)->EndNodeID)->due = GetDriver(i)->availability.second;
    }
};
void Sol::UpdateNodeCumulativeDemand( std::vector<Node *> Driver_Prev){

    for(int i=0;i<Driver_Prev.size();i++){
        if(Driver_Prev[i]!= NULL) {
            Driver_Prev[i]->cumulative_dmd = 0;
            if (Driver_Prev[i]->type == Parameters::TypeNode::START_LINK) {
                Driver_Prev[i]->cumulative_dmd = 0;
            }
            if (Driver_Prev[i]->type == Parameters::TypeNode::END_LINK) {
                Driver_Prev[i]->cumulative_dmd = GetDriverAssignedTo(Driver_Prev[i])->sumDemand;
            }
            if (Driver_Prev[i]->type != Parameters::TypeNode::START_LINK && Driver_Prev[i]->type != Parameters::TypeNode::END_LINK){

                Node *prev = Driver_Prev[i];
                while(prev->type !=  Parameters::TypeNode::START_LINK)
                {
                    Driver_Prev[i]->cumulative_dmd += prev->demand;
                    prev = DriverPrev[prev->id];
                }
            }
        }
    }
}


void Sol::UpdateNodeCumulativeDemand(Driver* d) {
    // Start from the driver's start node
    Node* current = GetNode(d->StartNodeID);

    // Initialize cumulative demand
    double cumulative_dmd = 0;

    while (current != nullptr && current->type != Parameters::TypeNode::END_LINK) {
        if (current->type == Parameters::TypeNode::START_LINK) {
            current->cumulative_dmd = 0;
        } else {
            // Update cumulative demand for the current node
            cumulative_dmd += current->demand;
            current->cumulative_dmd = cumulative_dmd;
        }

        // Move to the next node in the driver's route
        current = DriverNext[current->id];
    }

    // Handle the end link node
    if (current != nullptr && current->type == Parameters::TypeNode::END_LINK) {
        current->cumulative_dmd = d->sumDemand;
    }
}

void Sol::RelocateChainNew(Node *s1, Node *e1, Node *n){
    Node* prev_s1 = DriverPrev[s1->id];
    Node* next_e1 = DriverNext[e1->id];

    Driver* d1 = DriverAssignTo[s1->id];

    if (n == nullptr) {
        // Handling 3PL assignment
        Node* nn = s1;
        int cnt1 = 0;

        // Remove and unassign the chain nodes using the existing function
        while(nn != next_e1)
        {
            RemoveAndUnassign(nn);
            nn = DriverNext[nn->id];
            cnt1++;
        }

        // Update the route connections for d1
        DriverNext[prev_s1->id] = next_e1;
        DriverPrev[next_e1->id] = prev_s1;

        RoutesLength[d1->id] -= cnt1;

    } else {
        // Standard relocation logic
        Node* prev_n = DriverPrev[n->id];
        Node* next_n = DriverNext[n->id];
        Driver* d2 = DriverAssignTo[n->id];

        DriverNext[prev_s1->id] = next_e1;
        DriverNext[n->id] = s1;

        DriverPrev[next_e1->id] = prev_s1;
        DriverPrev[next_n->id] = e1;

        DriverPrev[s1->id] = n;
        DriverNext[e1->id] = next_n;

        Node* nn = s1;
        int cnt1 = 0;
        while(nn != next_n)
        {
            DriverAssignTo[nn->id] = d2;
            nn = DriverNext[nn->id];
            cnt1++;
        }

        RoutesLength[d1->id] -= cnt1;
        RoutesLength[d2->id] += cnt1;
    }
}

double Sol::GetCost(int a) {
   // _last_cost.Init();


       double _lastCost=0;
       for (int i = 0; i < GetDriverCount(); i++)
       {
           _lastCost += GetTravelCost(GetDriver(i));

       }
    if(a==1) {
        for (int i = 0; i < GetDriverCount(); i++) {
            _lastCost += GetShiftCost(GetDriver(i), 1);
        }
    }
    if (a==2){
        for (int i = 0; i < GetDriverCount(); i++) {
            _lastCost += GetShiftCost(GetDriver(i), 2);
        }
    }
       _lastCost += GetTplCost();

       return _lastCost;



}

double Sol::GetTravelCost(){
    double Travel_Cost =0;
    for (int i = 0; i < GetDriverCount(); i++)
    {
        Travel_Cost += GetTravelCost(GetDriver(i));
    }
    return Travel_Cost;
}

double Sol::GetTravelCost(Driver *d) {
    double distance = 0;
    double worst_case_load = 0;
    int nb_node_in_route = 0;
    d->load_cost = 0;
    if (RoutesLength[d->id] < EPS) return 0;
    Node *prev = GetNode(d->StartNodeID);
    while (prev->type != Parameters::END_LINK) {
        Node *next = DriverNext[prev->id];
        if (next == nullptr)
            break;
        distance += Distance(prev, next, d);

        if (next->type == Parameters::END_LINK) {
            prev = next;
            continue;
        }
        nb_node_in_route += 1;
        // check route robustness for demand
        worst_case_load += ListDemand[next->id];
        prev = next;
    }
      //assert(TraveledDistance[d->id] == distance);
    (TraveledDistance[d->id] == distance);
    double Travel_Cost= distance;//defined in solution.h
    d->load_cost = worst_case_load;
    return Travel_Cost;
}

double Sol::GetShiftCost(int a) {
    double Shift_cost =0;// changed
    for (int i = 0; i < GetDriverCount(); i++)
    {
        Shift_cost += GetShiftCost(GetDriver(i), a);
    }
    return Shift_cost;
}

double Sol::GetShiftCost(Driver *d, int a) {
    if (a==1){
        double shift_Cost=0;

        if (RoutesLength[d->id]>0.0001){
            shift_Cost= ((d->shift.end - d->shift.start) / 60) * d->salary;
        } else{
            return shift_Cost;
        }
        return shift_Cost;
   }
    if (a==2){
        double shift_Cost=0;

        if (RoutesLength[d->id]>0.0001){
            shift_Cost= ((d->Bestshift.end - d->Bestshift.start) / 60) * d->salary;
        } else{
            return shift_Cost;
        }
        return shift_Cost;

    }

}

double Sol::GetTplCost() {
    double Tpl_Cost;
     Tpl_Cost = UnassignedCustCount* _data->GetTplCost();
    return Tpl_Cost;
}

double Sol::GetTplCostNode() {
    double Tpl_Cost;
    Tpl_Cost =  _data->GetTplCost();
    return Tpl_Cost;
}

//Revert the path of n1 to n2 inclusively
void Sol::RevertPath(Node * n1, Node * n2)
{
    Node * prev = DriverPrev[n1->id];
    Node * next = DriverNext[n2->id];

    Node * cur = n1;
    Node * last = next;
    Node * prevlast = n2;
    while(cur != n2)
    {
        Node * tmp = DriverNext[cur->id];
        DriverNext[cur->id] = last;
        DriverPrev[last->id] = cur;
        DriverPrev[cur->id] = prevlast;
        DriverNext[prevlast->id] = cur;


        last = cur;
        cur = tmp;
    }

    DriverNext[prev->id] = n2;
    DriverPrev[n2->id] = prev;
}
void Sol::RelocateChain(Node *s1, Node *e1, Node *n){
    Node* prev_s1 = DriverPrev[s1->id];
    Node* prev_n = DriverPrev[n->id];

    Node* next_e1 = DriverNext[e1->id];
    Node* next_n = DriverNext[n->id];

   // Driver* d1 = DriverAssignTo[s1->id];
    //Driver* d2 = DriverAssignTo[n->id];
     Driver* d1 = DriverAssignTo[s1->id];
    Driver* d2 = DriverAssignTo[n->id];

   // DriverNext[prev_s1->id] = next_n;
    DriverNext[prev_s1->id] = next_e1;
    DriverNext[n->id] = s1;

    DriverPrev[next_e1->id] = prev_s1;
    DriverPrev[next_n->id] = e1;

    DriverPrev[s1->id] = n;
    DriverPrev[next_e1->id] = prev_s1;

    DriverNext[e1->id] = next_n;
    DriverNext[prev_s1->id] = next_e1;


    Node* nn = s1;
    int cnt1 = 0;
    while(nn != next_n)
    {
        DriverAssignTo[nn->id] = d2;
        nn = DriverNext[nn->id];
        cnt1++;
    }

    //nn = s2;
    // int cnt2 = 0;
    //while(nn != next_e1)
    //{
    //   DriverAssignTo[nn->id] = d1;
    //   nn = DriverNext[nn->id];
    //  cnt2++;
    //}
    RoutesLength[d1->id] += (-cnt1);
    RoutesLength[d2->id] += (cnt1);
}

//Take the set of nodes from s1 to e1 and swap it with the set of nodes from s2 to e2
//e1 and e2 are included and s1 and s2

void Sol::SwapTwoChains(Node* s1, Node* e1, Node* s2, Node* e2)
{
    //printf("s1:%d e1:%d s2:%d e2:%d\n",s1->id,e1->id,s2->id,e2->id);
    Node* prev_s1 = DriverPrev[s1->id];
    Node* prev_s2 = DriverPrev[s2->id];

    Node* next_e1 = DriverNext[e1->id];
    Node* next_e2 = DriverNext[e2->id];

    Driver* d1 = DriverAssignTo[s1->id];
    Driver* d2 = DriverAssignTo[s2->id];

    DriverNext[prev_s1->id] = s2;
    DriverNext[prev_s2->id] = s1;

    DriverPrev[next_e1->id] = e2;
    DriverPrev[next_e2->id] = e1;

    DriverPrev[s1->id] = prev_s2;
    DriverPrev[s2->id] = prev_s1;

    DriverNext[e1->id] = next_e2;
    DriverNext[e2->id] = next_e1;

    Node* n = s1;
    int cnt1 = 0;
    while(n != next_e2)
    {
        DriverAssignTo[n->id] = d2;
        n = DriverNext[n->id];
        cnt1++;
    }

    n = s2;
    int cnt2 = 0;
    while(n != next_e1)
    {
        DriverAssignTo[n->id] = d1;
        n = DriverNext[n->id];
        cnt2++;
    }
    RoutesLength[d1->id] += (-cnt1+cnt2);
    RoutesLength[d2->id] += (-cnt2+cnt1);
}




void Sol::UpdateAvailabilityArrivalTimes(Driver *d){

    // Start from the beginning of the driver's shift
    if(RoutesLength[d->id] < 0.00000000009){
        AvailabilityArrivalTime[d->StartNodeID] = d->availability.first;
        AvailabilityArrivalTime[d->EndNodeID] = d->availability.first;
    }

    if(RoutesLength[d->id]>0.00000000009){

        double currentTime = d->availability.first;
        Node *current = GetNode(d->StartNodeID);

        while (current != nullptr && current->type != Parameters::END_LINK) {
            AvailabilityArrivalTime[current->id] = currentTime;

            Node *next = DriverNext[current->id];
            if (next == nullptr || next->type == Parameters::END_LINK) {
                // If next is the depot, update its arrival time
                if (next != nullptr && next->type == Parameters::END_LINK) {
                    double travelTimeToDepot = Distance(current, next, d);
                    AvailabilityArrivalTime[next->id] = std::max(AvailabilityArrivalTime[current->id], current->ready)+ current->serviceTime + travelTimeToDepot;
                }
                break;  // Reached the end of the route
            }

            // Calculate travel time to the next node
            double travelTime = Distance(current, next, d);
            double ArrivalTimeAtNext = std::max(AvailabilityArrivalTime[current->id], current->ready)+ current->serviceTime+ travelTime;

            // Update currentTime for the next iteration
            currentTime = ArrivalTimeAtNext;

            current = next;
        }
    }
}





void Sol::UpdateAvailabilityDepartureBeginningWaitingTimes(Driver *d) {

    if (RoutesLength[d->id] < 0.00000000009) {
        AvailabilityDepartureTime[d->StartNodeID] = AvailabilityArrivalTime[d->StartNodeID];
        AvailabilityBeginningTime[d->StartNodeID] = AvailabilityArrivalTime[d->StartNodeID];
        AvailabilityWaitingTime[d->StartNodeID] = 0;

        AvailabilityDepartureTime[d->EndNodeID] = AvailabilityArrivalTime[d->EndNodeID];
        AvailabilityBeginningTime[d->EndNodeID] = AvailabilityArrivalTime[d->EndNodeID];
        AvailabilityWaitingTime[d->EndNodeID] = 0;
    }


    // Start from the beginning of the driver's shift
    if (RoutesLength[d->id] > 0.00000000009) {
        double currentTime = d->availability.first;

        Node *current = GetNode(d->StartNodeID);

        while (current != nullptr && current->type != Parameters::END_LINK) {
            //BeginningTime[current->id] = std::max(ArrivalTime[current->id], current->ready);
            //DepartureTime[current->id] = BeginningTime[current->id] + current->serviceTime;
            AvailabilityDepartureTime[current->id] = currentTime;
            AvailabilityBeginningTime[current->id] = AvailabilityDepartureTime[current->id] - current->serviceTime; // Added
            AvailabilityWaitingTime[current->id] = AvailabilityBeginningTime[current->id] - AvailabilityArrivalTime[current->id]; // Added

            Node *next = DriverNext[current->id];
            if (next == nullptr || next->type == Parameters::END_LINK) {
                // If next is the depot, update its arrival time
                if (next != nullptr && next->type == Parameters::END_LINK) {
                    double travelTimeToDepot = Distance(current, next, d);
                    AvailabilityDepartureTime[next->id] = std::max((currentTime + travelTimeToDepot), next->ready)+ next->serviceTime;

                    AvailabilityBeginningTime[next->id] = AvailabilityDepartureTime[next->id] - next->serviceTime; // Added
                    AvailabilityWaitingTime[next->id] = AvailabilityBeginningTime[next->id] - AvailabilityArrivalTime[next->id]; // Added

                }
                break;  // Reached the end of the route
            }

            // Calculate travel time to the next node
            double travelTime = Distance(current, next, d);
            double startTimeAtNext = std::max(currentTime + travelTime, next->ready);

            // Update currentTime for the next iteration
            currentTime = startTimeAtNext + next->serviceTime;

            current = next;
        }
    }

}

// Calculate SumWaitingTime
void Sol::UpdateAvailabilitySumWaitingTimes(Driver *d) {

    if (RoutesLength[d->id] < 0.00000000009) {
                AvailabilitySumWaitingTime[d->StartNodeID] = AvailabilityWaitingTime[d->EndNodeID] ;
                AvailabilitySumWaitingTime[d->EndNodeID] = 0;
    }


    if (RoutesLength[d->id] > 0.00000000009) {

        Node *current = GetNode(d->StartNodeID);
        while (current != nullptr && current->type != Parameters::END_LINK) {
            double sum = 0;
            Node *next = DriverNext[current->id];
            while (next != nullptr && next->type != Parameters::END_LINK) {
                sum = sum + AvailabilityWaitingTime [next->id];
                next = DriverNext[next->id];
                if (next->type == Parameters::END_LINK) {
                    sum = sum + AvailabilityWaitingTime [next->id];
                    break;
                }
            }
            AvailabilitySumWaitingTime[current->id] = sum;
            current = DriverNext[current->id];
            if (current->type == Parameters::END_LINK) {
                AvailabilitySumWaitingTime[current->id] = 0;
                break;
            }
        }
    }
}

void Sol::UpdateAvailabilitySlackNew(Driver *d, Node *i)
{
    double min = 9000;
    double totalWaiting = 0;
    Node *current = DriverNext[i->id];

    while (current != nullptr && current->type != Parameters::END_LINK) {
        totalWaiting += AvailabilityWaitingTime[current->id];
        double currentValue = totalWaiting + (current->due - AvailabilityBeginningTime[current->id]);
        if (currentValue < min) {
            min = currentValue;
        }
        current = DriverNext[current->id];
    }

    // Handle the END_LINK node
    if (current != nullptr && current->type == Parameters::END_LINK) {
        totalWaiting += AvailabilityWaitingTime[current->id];
        double currentV = totalWaiting + (current->due - AvailabilityBeginningTime[current->id]);
        if (currentV < min) {
            min = currentV;
        }
    }

    // Set the AvailabilitySlackTime for node i
    AvailabilitySlackTime[i->id] = std::min(min, (i->due - AvailabilityBeginningTime[i->id])); //min;
}

void Sol::UpdateAvailabilitySlackNew(Driver *d) {

    if (RoutesLength[d->id] < 0.00000000009) {

        AvailabilitySlackTime[d->StartNodeID] = (GetNode(d->StartNodeID)->due - AvailabilityBeginningTime[GetNode(d->StartNodeID)->id]);;

        AvailabilitySlackTime[d->EndNodeID] = (GetNode(d->EndNodeID)->due - AvailabilityBeginningTime[GetNode(d->EndNodeID)->id]);
    }
    if (RoutesLength[d->id] > 0.00000000009) {

        Node *current = GetNode(d->StartNodeID);
        while (current != nullptr && current->type != Parameters::END_LINK) {
            UpdateAvailabilitySlackNew(d, current);
            current = DriverNext[current->id];
            if (current->type == Parameters::END_LINK) {
                AvailabilitySlackTime[current->id] = current->due - AvailabilityBeginningTime[current->id];
                break;
            }
        }
    }

}


void Sol::AssignBestShift(){

    for (int i = 0; i < GetDriverCount(); i++){
        Driver *driver = GetDriver(i);
        driver->Bestshift  = driver->shift;
        driver->BestShift_Cost = driver->Shift_Cost;
    }
}
void Sol::RemoveAndUnassignChain(Node* s1, Node* e1) {
    Node* current = s1;

    // Traverse and remove each node in the chain
    while (current != nullptr && current != e1) {
        RemoveAndUnassign(current);
        current = DriverNext[current->id];  // Move to the next node in the chain
    }

    // Remove the last node in the chain
    RemoveAndUnassign(e1);
}
int Sol::CountNodesInChain(Node* s1, Node* e1) {
    int count = 0;
    Node* current = s1;

    // Traverse from s1 to e1 and count the nodes
    while (current != nullptr) {
        count++;
        if (current == e1) break;  // Stop when reaching e1
        current = DriverNext[current->id];  // Move to the next node
    }

    return count;
}




void Sol::Show(int a) {

    std::cout << "Tpl customers: ";
    for (const auto& customer : UnassignedCustVRP) {
        if (customer != nullptr) {
            std::cout << customer->id << ", ";
        }
    }
    std::cout << std::endl; // Move to the next line after all IDs are printed

    if(a==1){
        std::cout <<  "TotalCost: " << GetCost(1) << " ,TplCost: " << GetTplCost() << " ,TravelCost: "<< GetTravelCost()<<" ,ShiftCost: " << GetShiftCost(1) << std::endl;// here it shows the cost of drivers, 3pl and route
    }
    if (a==2){
        std::cout <<  "TotalCost: " << GetCost(2) << " ,TplCost: " << GetTplCost() << " ,TravelCost: "<< GetTravelCost()<<" ,ShiftCost: " << GetShiftCost(2) << std::endl;// here it shows the cost of drivers, 3pl and route
    }
    ShowDrivers();// this shows the routes
}

void Sol::ShowDrivers() {

    for (int i = 0; i < GetDriverCount(); i++)
        Show(GetDriver(i));
}


void Sol::Show(Driver *d) {
    if (RoutesLength[d->id] < 1)
        return;
    // Cost curCost;
    //GetVrpCost(d, curCost);
    printf("R#%2d: Load: %4.2lf cap: %2.1lf ShiftC:%.2lf BShiftC:%.2lf TravelC:%.2lf cost:%.2lf ", d->id, d->load_cost,
           d->capacity, d->Shift_Cost ,d->BestShift_Cost , d->Travel_cost , d->cost);
    printf(" len:%d: ", (int) RoutesLength[d->id]);
    printf(" Shift: [%.2lf, %.2lf] Salary: %.2lf ", d->Bestshift.start, d->Bestshift.end, d->salary);  // Print the shift start and end times

    Node *cur = GetNode(d->StartNodeID);
    while (cur != nullptr) {
        if (Data::isCustomer(cur))
        printf("%3d-", cur->custID+1);

        else
            printf("%d-", cur->distID);

        cur = DriverNext[cur->id];
    }
    printf("\n");
    if (d->load_cost != LoadQuantity[d->id]) {
        printf("worst_case_load = %lf != d->worst_case_load %lf\n", d->load_cost,
               LoadQuantity[d->id]);
        exit(1);
    }
}

int Sol::GetDriverCountAssigned(){
    int DriverCountAssigned=0;
    for (int i = 0; i < _data->GetDriverCount(); i++) {
        if (RoutesLength[i] > 0.9 ) {
            DriverCountAssigned++;
        }
    }
    return DriverCountAssigned;
}

void Sol::AssignStationToDriver() {
    for (int i = 0; i < _data->GetDriverCount(); i++) {
        RoutesLength[i] = 0;
        Driver *d = _data->GetDriver(i);
        AssignStationToDriver(d);
    }
}

void Sol::AssignStationToDriver(Driver *d) {
    Node *n1 = _data->GetNode(d->StartNodeID);
    Node *n2 = _data->GetNode(d->EndNodeID);
    DriverNext[n1->id] = n2;
    DriverPrev[n2->id] = n1;
    DriverAssignTo[n1->id] = d;
    DriverAssignTo[n2->id] = d;
}
void Sol::AddDriver() {
    RoutesLength.push_back(0);
    LoadQuantity.push_back(0);
    RoutesLength.push_back(0);
}
void Sol::AddNode() {
    Unassigned.push_back(nullptr);
    DriverNext.push_back(nullptr);
    DriverPrev.push_back(nullptr);
    DriverAssignTo.push_back(nullptr);
    ListDemand.push_back(0);
}

void Sol::RemoveAndInsert(Node *n, Node *prev) {
    // Remove the node from its current position
    if (DriverNext[n->id] != nullptr) {
        DriverPrev[DriverNext[n->id]->id] = DriverPrev[n->id];
    }
    if (DriverPrev[n->id] != nullptr) {
        DriverNext[DriverPrev[n->id]->id] = DriverNext[n->id];
    }

    // Clear the node's pointers
    DriverNext[n->id] = nullptr;
    DriverPrev[n->id] = nullptr;

    // Insert the node after the specified previous node
    DriverNext[n->id] = DriverNext[prev->id];
    DriverPrev[n->id] = prev;

    if (DriverNext[prev->id] != nullptr) {
        DriverPrev[DriverNext[prev->id]->id] = n;
    }
    DriverNext[prev->id] = n;

    assert(DriverNext[n->id] != n);
}




void Sol::InsertAfter(Node *n, Node *prev) {
    Driver *d = DriverAssignTo[prev->id];
    RoutesLength[d->id]++;
    DriverAssignTo[n->id] = d;
    DriverNext[n->id] = DriverNext[prev->id];
    DriverPrev[n->id] = prev;
    if (DriverNext[prev->id] != nullptr)
        DriverPrev[DriverNext[prev->id]->id] = n;
    DriverNext[prev->id] = n;
    assert(DriverNext[n->id] != n);
}
void Sol::Relocate(Node *next, Node *prev) {
    Remove(next);
    InsertAfter(next, prev);
}

void Sol::Swap(Node *U, Node *V) {
    Node *UDriverPrev = DriverPrev[U->id];
    Node *VDriverPrev = DriverPrev[V->id];
    Remove(U);
    Remove(V);
    InsertAfter(U, VDriverPrev);
    InsertAfter(V, UDriverPrev);
}
void Sol::PutAllCustomersToUnassigned() {
    UnassignedCustCount = 0;
    for (int i = 0; i < GetCustomerCount(); i++) {
        AddToUnassigneds(GetCustomer(i));
    }
}
void Sol::AddToUnassigneds(Node *n) {
    UnassignedCustIndex[n->custID] = UnassignedCustCount;
    UnassignedCustVRP[UnassignedCustCount] = n;
    UnassignedCustCount++;
}


void Sol::AssignRouteToDriver( Driver *d, const std::vector<Node *> &vect, std::vector<Node *> &_DriverPrev, std::vector<Node *> &_DriverNext){
   // Node *n1 = DriverNext[d->StartNodeID];//GetNode(d->StartNodeID);
    //Node *n2 = DriverPrev[d->EndNodeID];




    if (vect.empty()){//no route assigned to the driver

           _DriverNext[d->StartNodeID] = GetNode(d->EndNodeID);
           _DriverPrev[d->EndNodeID] = GetNode(d->StartNodeID);
        return;

    }


        // RoutesLength[d->id] = 0;

         Node* current = vect[0];
         for(int i = 0;  i < vect.size()-1 ; i++)
         {
             _DriverNext[current->id] = vect[i+1];
             current= vect[i+1];
         }
          Node* currentt = vect[vect.size()-1];
          for(int j = vect.size()-1;  j>0 ; j--)
          {
              _DriverPrev[currentt->id] = vect[j-1];
              currentt = vect[j-1];
          }



    // RoutesLength[d->id] = 0;




}


void Sol::DriverAssgnment(){

    for (int i = 0; i < GetDriverCount(); i++) {
        if (RoutesLength[GetDriver(i)->id]> 0.1)
        {
            Node *prev = GetNode(GetDriver(i)->StartNodeID);
            DriverAssignTo[prev->id]= GetDriver(i);
            while (prev->type != Parameters::END_LINK) {
                Node *next = DriverNext[prev->id];
                if (next->type == Parameters::END_LINK) {
                   // prev = next;
                    DriverAssignTo[next->id]= GetDriver(i);
                    break;
                }
                DriverAssignTo[next->id]= GetDriver(i);
                prev = next;
            }
        }
    }
}

std::vector<Node*> Sol::FindNewRouteAfterSwap(Node* s1, Node* e1, Node* prev, std::vector<Node*>& chain, Driver* d) {
    std::vector<Node*> routeNodes;

    // Start from the driver's start node
    Node* current = GetNode(d->StartNodeID);
    bool inserted = false;

    while (current != nullptr) {
        // **Step 1: Skip (s1 → e1), effectively removing it**
        if (current == s1) {
            while (current != nullptr && current != e1) {
                current = DriverNext[current->id];  // Skip nodes in (s1 → e1)
            }
            if (current == e1) {
                current = DriverNext[current->id];  // Skip `e1` as well
            }
        }

        // **Step 2: Insert the entire `chain` after `prev`**
        if (current == prev && !inserted) {
            routeNodes.push_back(current); // Add `prev`

            // Insert all nodes in the given `chain` vector (instead of DriverNext)
            for (Node* node : chain) {
                routeNodes.push_back(node);
            }

            inserted = true;
        } else if (current != nullptr) {
            // **Step 3: Add remaining nodes in the original order**
            routeNodes.push_back(current);
        }

        current = (current != nullptr) ? DriverNext[current->id] : nullptr;

        if (current == nullptr || current->type == Parameters::END_LINK) {
            break;
        }
    }

    // Add the end link node to the route
    if (current != nullptr && current->type == Parameters::END_LINK) {
        routeNodes.push_back(current);
    }

    return routeNodes;
}
void Sol::PrintSwappedChains(Sol &s, Node* n1, Node* e1, std::vector<Node*> &chain) {


    // **Print the replaced chain (was in driver, now moved to 3PL)**
    std::cout << " n1 -> e1): ";
    Node* current = n1;
    while (current != nullptr && current != e1) {
        std::cout << current->id << " -> ";
        current = s.DriverNext[current->id];
    }
    std::cout << e1->id << std::endl;

    // **Print the inserted unassigned chain (now in the route)**
    std::cout << "Chain  ";
    for (Node* node : chain) {
        std::cout << node->id << " -> ";
    }
}

void Sol::ReplaceChainWithUnassigned(Node* s1, Node* e1, std::vector<Node*> &chain) {
    Driver* d1 = DriverAssignTo[s1->id];
    Node* prev_s1 = DriverPrev[s1->id];
    Node* next_e1 = DriverNext[e1->id];

    // **Step 1: Assign (s1 → e1) to 3PL**
    RemoveAndUnassignChain(s1, e1);  // ✅ Use helper function

    // **Step 2: Remove (s1 → e1) from the route**
    DriverNext[prev_s1->id] = next_e1;
    DriverPrev[next_e1->id] = prev_s1;

    // If there is no replacement chain, stop here after removal
    if (chain.empty()) {
        RoutesLength[d1->id] -= CountNodesInChain(s1, e1);
        return;
    }

    // **Step 3: Insert the new chain after prev_s1**
    Node* lastInserted = prev_s1;
    for (Node* node : chain) {
        DriverNext[lastInserted->id] = node;
        DriverPrev[node->id] = lastInserted;
        lastInserted = node;
        DriverAssignTo[node->id] = d1;
    }

    // **Step 4: Connect last new node to next_e1**
    DriverNext[lastInserted->id] = next_e1;
    DriverPrev[next_e1->id] = lastInserted;

    // **Step 5: Update route length**
    RoutesLength[d1->id] += (chain.size() - CountNodesInChain(s1, e1));
    // **Step 6: Remove the chain from unassigned**
    for (Node* node : chain) {
        RemoveFromUnassigneds(node);
    }
}

void Sol::Remove(Node *n) {
    if (DriverAssignTo[n->id] != nullptr)
        RoutesLength[DriverAssignTo[n->id]->id]--;
    if (DriverNext[n->id] != nullptr)
        DriverPrev[DriverNext[n->id]->id] = DriverPrev[n->id];
    if (DriverPrev[n->id] != nullptr)
        DriverNext[DriverPrev[n->id]->id] = DriverNext[n->id];
    DriverNext[n->id] = nullptr;
    DriverPrev[n->id] = nullptr;
    DriverAssignTo[n->id] = nullptr;
}
bool Sol::IsUnassigned(Node *n) {
    return UnassignedCustIndex[n->custID] != -1;

}
void Sol::RemoveAndUnassign(Node *n) {
    Remove(n);
    AddToUnassigneds(n);
}
void Sol::RemoveAndUnassignCustomers() {
    UnassignedCustCount = 0;
    for (int i = 0; i < GetCustomerCount(); i++) {
        Node *n = GetCustomer(i);
        RemoveAndUnassign(n);
    }
}
void Sol::RemoveFromUnassigneds(Node *c) {
    int ind = UnassignedCustIndex[c->custID];
    if (ind == -1)
        return;
    if (UnassignedCustCount < 1)
        return;
    Node *rep = UnassignedCustVRP[UnassignedCustCount - 1];
    UnassignedCustVRP[ind] = rep;
    UnassignedCustIndex[rep->custID] = ind;

    UnassignedCustVRP[UnassignedCustCount - 1] = nullptr;
    UnassignedCustCount--;
    UnassignedCustIndex[c->custID] = -1;
}
void Sol::RemoveAndInsert(Driver *d, const std::vector<Node *> &vect) {

    Node *prev = GetNode(d->StartNodeID);
    std::vector<Node *> temp_vec;
//    printf("Remove %d", d->id);
    while (prev->type != Parameters::END_LINK) {

        Node *next = DriverNext[prev->id];
        if (next->type == Parameters::CUSTOMER) {
            temp_vec.push_back(next);
//            printf(" %d", next->no);
        }
        prev = next;
    }
//    printf("\n");


//    Update(d);
//    Show(d);
}

// Sol::GetAssignedTo(Node *node){
//}


void Sol::SortShifts(std::vector<Shift>& shiftSet) {
    // Sort shifts first based on starting time (increasing) and then duration of each shift (decreasing)
    std::sort(shiftSet.begin(), shiftSet.end(), [](const Shift &a, const Shift &b) {
        if (a.start != b.start) {
            return a.start < b.start;  // Sort by start time in increasing order
        }
        return (a.end - a.start) > (b.end - b.start);  // If start times are equal, sort by duration in decreasing order
    });
}






// not used anymore, it was used to assign shifts to drivers in the start,
void Sol::AssignShiftsToDrivers(std::vector<Shift> shiftSet) {
    for (int i = 0; i < GetDriverCount(); ++i) {
        for (int j = 0; j < shiftSet.size(); ++j) {
            // Assuming availability is still stored as a std::pair<int, int> or std::pair<double, double>
            if (GetDriver(i)->availability.first <= shiftSet[j].start && GetDriver(i)->availability.second >= shiftSet[j].end) {
                GetDriver(i)->shift.start = shiftSet[j].start;
                GetDriver(i)->shift.end = shiftSet[j].end;
                break;
            }
        }
    }
}

void Sol::AssignAvailabilityToDrivers(){// we should change depot TW when ever we change the shift
    for (int i = 0; i < GetDriverCount(); ++i) {
        GetDriver(i)->shift.start = GetDriver(i)->availability.first;
        GetDriver(i)->shift.end = GetDriver(i)->availability.second;

        GetNode(GetDriver(i)->StartNodeID)->ready = GetDriver(i)->availability.first;
        GetNode(GetDriver(i)->StartNodeID)->due = GetDriver(i)->availability.second;
        GetNode(GetDriver(i)->EndNodeID)->ready = GetDriver(i)->availability.first;
        GetNode(GetDriver(i)->EndNodeID)->due = GetDriver(i)->availability.second;

    }

}


void Sol:: AssignNextDriverShift(){
    const auto& shifts = GetData()->GetShifts();
}




void Sol::SetDriverShiftToLargest(Driver *d) {
  //  const auto& shifts = GetData()->GetShifts(); // Assume this returns a std::vector<Shift>
    const auto& shifts = d->GetDriverShiftSet();
    for (int j = 0; j < shifts.size(); j++) {
        // Compare start and end times of shifts to find the largest shift that fits the criteria
        if (shifts[j].start <= d->shift.start && shifts[j].end >= d->shift.end) {
            d->shift = shifts[j]; // Assign the matching shift to the driver
        }
    }
}




void Sol:: UpdateDriverShift(){

    for (int k = 0; k < GetDriverCount() ; k++){
        Driver *driver = GetDriver(k);
        Shift newRouteTime = driver->shift;

        Node *FirstNode = GetNode(driver->StartNodeID);
        Node *FirstCustomer = DriverNext[FirstNode->id];

        Node *LasttNode = GetNode(driver->EndNodeID);
        Node *LastCustomer = DriverPrev[LasttNode->id];



        //  const auto& shifts = GetData()->GetShifts(); // Assume this returns a std::vector<Shift>
        const auto& shifts = driver->GetDriverShiftSet();
        for (int j = 0; j < shifts.size(); j++) {
            // Compare start and end times of shifts to find the largest shift that fits the criteria
            if (shifts[j].start <= (FirstCustomer->due - Distance(FirstNode, FirstCustomer, driver))  && shifts[j].end >= LasttNode->ready+ Distance(LastCustomer, LasttNode, driver)) {
                if (shifts[j].end- shifts[j].start < newRouteTime.end-newRouteTime.start){
                    driver->shift = shifts[j]; // Assign the matching shift to the driver
                }
            }
        }


    }

}






Shift Sol::UpdateToShortestShift (Driver *d){// we already know there is a feasible shift

    Shift ShortestShift = d->shift;
    if(TraveledDistance[d->id] < 0.00000000009){
        return ShortestShift;
    }
    if(TraveledDistance[d->id] > 0.00000000009){
        double F0 = AvailabilitySlackTime[d->StartNodeID];
        double W0 = AvailabilitySumWaitingTime[d->StartNodeID];

        double F0_time = d->availability.first + F0;
        double W0_time =  d->availability.first + W0;

        double s =  (F0_time) - std::fmod((F0_time), Parameters::ShiftInterval);
        double shortestPathlength = DepartureTime[GetNode(d->EndNodeID)->id] - std::min(F0, W0);
        double end;
        if(s< W0_time)
        {
            end = AvailabilityDepartureTime[d->EndNodeID];
        }
        if (s > W0_time)
        {
            end = shortestPathlength + s;
        }


        if ((end-s)/60 <= (_data->GetMaxWorking()/60) ){
            if ( (end-s)/60 <=2){
                int AddTimeToShift = 2- ((end-s)/60);
                double ShiftEndAfterShiftExtention = end + (AddTimeToShift*60);
                if (ShiftEndAfterShiftExtention <= d->availability.second)// avilability or depot deadline???
                {
                    ShortestShift.end = ShiftEndAfterShiftExtention;
                    ShortestShift.start = s;
                    return ShortestShift;
                }
            }
            else{
                ShortestShift.end = end;
                ShortestShift.start = s;
                return ShortestShift;
            }
        }


        return ShortestShift;
    }
}

std::vector<Node*> Sol::FindNewRouteAfterRemoveDepot(Driver* d) {
    std::vector<Node*> routeNodes;
    Node* startNode = GetNode(d->StartNodeID);
    Node* endNode = GetNode(d->EndNodeID);

    // Start from the driver's start node
    Node* current = GetNode(d->StartNodeID);

    // Follow the DriverNext links until the end link
    while (current != nullptr) {
        // Skip the startNode and endNode
        if (current != startNode && current != endNode) {
            routeNodes.push_back(current); // Add nodes other than startNode and endNode
        }

        current = DriverNext[current->id];

        // Break if the current node is null or if it is the end link
        if (current == nullptr || current->type == Parameters::END_LINK) {
            break;
        }
    }

    // Add the new endNode to the route
   // routeNodes.push_back(endNode);

    return routeNodes;
}


std::vector<Node*> Sol::FindNewRouteAfterRemoveNodeN( Node* n, Driver* d) {
    std::vector<Node*> routeNodes;

    // Start from the driver's start node
    Node* current = GetNode(d->StartNodeID);

    // Follow the DriverNext links until the end link
    while (current != nullptr) {
        // Skip node n
        if (current != n) {
            routeNodes.push_back(current); // Add nodes other than n
        }

        current = DriverNext[current->id];

        if (current == nullptr || current->type == Parameters::END_LINK) {
           // routeNodes.push_back(current);
            break;
        }
    }

    // Add the end link node to the route
    if (current != nullptr && current->type == Parameters::END_LINK) {
        routeNodes.push_back(current);
    }

    return routeNodes;

}


std::vector<Node*> Sol::FindNewRouteAfterRemoveAndInsertNode(Node* prev, Node* n, Node* next, Driver* d) {
    std::vector<Node*> routeNodes;

    // Start from the driver's start node
    Node* current = GetNode(d->StartNodeID);

    // Follow the DriverNext links until the end link
    while (current != nullptr) {
        // Skip node n
        if (current != n) {
            routeNodes.push_back(current); // Add nodes other than n
        }

        current = DriverNext[current->id];

        if (current == nullptr || current->type == Parameters::END_LINK) {
            break;
        }
    }

    // Add the end link node to the route
    if (current != nullptr && current->type == Parameters::END_LINK) {
        routeNodes.push_back(current);
    }

    // Insert node n between prev and next
    std::vector<Node*> newRoute;
    for (auto node : routeNodes) {
        newRoute.push_back(node);
        if (node == prev) {
            newRoute.push_back(n); // Insert node n after prev
        }
    }

    return newRoute;
}




std::vector<Node*> Sol::FindNewRouteForDriver(  Driver* d) {
    std::vector<Node*> routeNodes;

    // Start from the driver's start node
    Node* current = GetNode(d->StartNodeID);

    // Follow the DriverNext links until the end link
    while (current != nullptr) {

            routeNodes.push_back(current); // Add nodes other than n

        current = DriverNext[current->id];

        if (current == nullptr || current->type == Parameters::END_LINK) {
            // routeNodes.push_back(current);
            break;
        }
    }

    // Add the end link node to the route
    if (current != nullptr && current->type == Parameters::END_LINK) {
        routeNodes.push_back(current);
    }

    return routeNodes;

}



std::vector<Node*> Sol::FindNewRouteAfterRemoveNodeChain(Node* s1, Node* e1, Driver* d) {
    std::vector<Node*> routeNodes;

    // Start from the driver's start node
    Node* current = GetNode(d->StartNodeID);

    // Follow the DriverNext links until the end link
    while (current != nullptr) {
        // Skip nodes from s1 to e1
        if (current == s1) {
            while (current != nullptr && current != e1) {
                current = DriverNext[current->id];
            }
            // Skip e1 as well
            if (current == e1) {
                current = DriverNext[current->id];
                continue;
            }
        }

        // Add nodes other than the ones being removed
        if (current != nullptr) {
            routeNodes.push_back(current);
            current = DriverNext[current->id];
        }

        if (current == nullptr || current->type == Parameters::END_LINK) {
            break;
        }
    }

    // Add the end link node to the route
    if (current != nullptr && current->type == Parameters::END_LINK) {
        routeNodes.push_back(current);
    }

    return routeNodes;
}

std::vector<Node*> Sol::FindNewRouteAfterInsertionChain(Node* prev, Node* s1, Node* e1, Driver* d) {
    std::vector<Node*> routeNodes;

    // Start from the driver's start node
    Node* current = GetNode(d->StartNodeID);

    // Follow the DriverNext links until the end link
    while (current != nullptr) {
        // Insert the chain after prev and before the next node
        if (current == prev) {
            routeNodes.push_back(current); // Add prev
            // Add chain from s1 to e1
            Node* chainCurrent = s1;
            while (chainCurrent != nullptr && chainCurrent != e1) {
                routeNodes.push_back(chainCurrent);
                chainCurrent = DriverNext[chainCurrent->id];
            }
            routeNodes.push_back(e1); // Add e1
        } else {
            routeNodes.push_back(current); // Add other nodes
        }

        current = DriverNext[current->id];

        if (current == nullptr || current->type == Parameters::END_LINK) {
            break;
        }
    }

    // Add the end link node to the route
    if (current != nullptr && current->type == Parameters::END_LINK) {
        routeNodes.push_back(current);
    }

    return routeNodes;
}


std::vector<Node*> Sol::FindNewRouteAfterInsertionChainToVector(Node* prev, Node* s1, Node* e1, Driver* d, std::vector<Node*>& NewRoute1) {
    std::vector<Node*> routeNodes;

    // Iterate over the nodes in NewRoute1 and insert the chain
    for (auto current : NewRoute1) {
        // Insert the chain after prev
        if (current == prev) {
            routeNodes.push_back(current); // Add prev
            // Add chain from s1 to e1
            Node* chainCurrent = s1;
            while (chainCurrent != nullptr && chainCurrent != e1) {
                routeNodes.push_back(chainCurrent);
                chainCurrent = DriverNext[chainCurrent->id];
            }
            routeNodes.push_back(e1); // Add e1
        } else {
            routeNodes.push_back(current); // Add other nodes
        }
    }

    // Ensure the end link node is added to the route
    if (!routeNodes.empty() && routeNodes.back()->type != Parameters::END_LINK) {
        Node* endLink = GetNode(d->EndNodeID);
        routeNodes.push_back(endLink);
    }

    return routeNodes;
}



std::vector<Node*> Sol::FindNewRouteAfterInsertionDepot(Node* startDepot, Node* endDepot, std::vector<Node*>& NewRoute1) {
    std::vector<Node*> routeNodes;

    // Add the start depot to the beginning of the route
    routeNodes.push_back(startDepot);

    // Iterate over the nodes in NewRoute1 and insert them into routeNodes
    for (auto current : NewRoute1) {
        routeNodes.push_back(current); // Add each node in NewRoute1
    }

    // Add the end depot to the end of the route
    routeNodes.push_back(endDepot);

    // The new end link is explicitly the endDepot node, so no need to add any additional end link nodes

    return routeNodes;
}



std::vector<Node*> Sol::FindNewRouteAfterInsertion(Node* prev, Node* next, Node* n, Driver* d) {
    std::vector<Node*> routeNodes;

    // Start from the driver's start node
    Node* current = GetNode(d->StartNodeID);

    // Follow the DriverNext links until the end link
    while (current != nullptr) {
        // Insert node n after prev and before next
        if (current == prev) {
            routeNodes.push_back(current); // Add prev
            routeNodes.push_back(n); // Insert n
        } else if (current == next) {
            routeNodes.push_back(current); // Add next
        } else {
            routeNodes.push_back(current); // Add other nodes
        }

        current = DriverNext[current->id];

        if (current == nullptr || current->type == Parameters::END_LINK) {

            break;
        }
    }

    // Add the end link node to the route
    if (current != nullptr && current->type == Parameters::END_LINK) {
        routeNodes.push_back(current);
    }

    return routeNodes;
}





Shift Sol::FindShortestShift (std::vector<Node*> NewRoute , Driver *d){
    Shift shift;
    shift.start = -1;// if the route is empty so the shift cost is zero, set to -2
    shift.end = -1; // if infeasible, always check this

   // if (RoutesLength[d->id]< 0.001){ // for example if we want to find new shift after removing then we end up in an empty route
     if (NewRoute.size() <= 2.001){ // for example if we want to find new shift after removing then we end up in an empty route
     shift.start = -2;
         return shift;
     }

    std::vector<double> arrivalTimes ;//= FindArrivalofRouteInsertion(d, NewRoute);
    std::vector<double> departureTimes;
    std::vector<double> beginningTimes;
    std::vector<double> waitingTimes;
    std::vector<double> sumWaitingTimes;
    std::vector<double> slackTimes;


    // break;

   // FindArrivalofRouteInsertion(d, NewRoute);
    FindTimesInsertion(d, NewRoute, arrivalTimes, departureTimes, beginningTimes, waitingTimes, sumWaitingTimes);
    UpdateSlackForAllNodesInsertion(d, NewRoute, beginningTimes, waitingTimes, slackTimes);

    // Check if the minimum value in slackTimes is negative
    double minSlack = *std::min_element(slackTimes.begin(), slackTimes.end());
    if (minSlack < -0.000001) { // NewRoute is infeasible
        return shift; // It will return -1 for shift start and end
    }
    double F0 = slackTimes[0];
    double W0 = sumWaitingTimes[0];

    double F0_time = d->availability.first +F0;
    double W0_time = d->availability.first +W0;

    double s =   (F0_time) - std::fmod((F0_time), 30.0);// largest number before F0 dividable by 30

// this works too
    if (s <= W0_time){
    //if (s <= W0){

       // double newShiftEnd = smallestDivisibleBy30AfterOrEqual(departureTimes[d->EndNodeID]);

        double newShiftEnd = smallestDivisibleBy30AfterOrEqual(departureTimes[NewRoute.size()-1]);
        if (((newShiftEnd-s)/60) <= (_data->GetMaxWorking()/60) )
        {
            if ( (newShiftEnd-s)/60 < _data->GetMinWorking()/60)// less than 2
            {
                int AddTimeToShift =  _data->GetMinWorking() - ((newShiftEnd-s));
                double ShiftEndAfterShiftExtention = newShiftEnd + (AddTimeToShift);
                if (ShiftEndAfterShiftExtention <= d->availability.second){
                    shift.end = ShiftEndAfterShiftExtention;
                    shift.start = s;
                    return shift;
                } else{
                    double ShiftStartAfterShiftExtention = s - AddTimeToShift;
                    if (ShiftStartAfterShiftExtention >= d->availability.first){
                        shift.start = ShiftStartAfterShiftExtention;
                        shift.end = newShiftEnd;
                    }

                }
            }
            else{
                shift.start = s;
                shift.end = newShiftEnd;
            }
        }

    } else{ //end will change

        double deltaEndIncrease = s - W0_time;
        double newShiftEnd = departureTimes[NewRoute.size()-1] + deltaEndIncrease;
        newShiftEnd = smallestDivisibleBy30AfterOrEqual(newShiftEnd);
        if (((newShiftEnd-s)/60) <= (_data->GetMaxWorking()/60) ){
            if ( (newShiftEnd-s)/60 < _data->GetMinWorking()/60)// less than 2
            {
                int AddTimeToShift = _data->GetMinWorking() - ((newShiftEnd-s));//120
                double ShiftEndAfterShiftExtention = newShiftEnd + (AddTimeToShift);
                if (ShiftEndAfterShiftExtention <= d->availability.second){
                    shift.end = ShiftEndAfterShiftExtention;
                    shift.start = s;
                    return shift;
                }
                else{
                    double ShiftStartAfterShiftExtention = s - AddTimeToShift;
                    if (ShiftStartAfterShiftExtention >= d->availability.first){
                        shift.start = ShiftStartAfterShiftExtention;
                        shift.end = newShiftEnd;
                    }

                }
            }else{
                shift.start = s;
                shift.end = newShiftEnd;
            }
        }

    }
        return shift;
}

double Sol::UpdateSlackNewInsertion(Driver *d, Node *i, std::vector<double>& beginningTime, std::vector<double>& waitingTime, std::vector<Node*>& routeNodes) {
    double min = 9000; // Initialize with a large number
    double totalWaiting = 0;
    bool startCalculation = false;

    for (size_t idx = 0; idx < routeNodes.size(); ++idx) {
        Node* current = routeNodes[idx];

        if (current == i) {
            startCalculation = true;
            continue;
        }

        if (startCalculation) {
            totalWaiting += waitingTime[idx];
            double currentValue = totalWaiting + (current->due - beginningTime[idx]);
            if (currentValue < min) {
                min = currentValue;
            }
        }
    }

    // Return the calculated slack time for node i
    size_t i_idx = std::distance(routeNodes.begin(), std::find(routeNodes.begin(), routeNodes.end(), i));
    return std::min(min, (i->due - beginningTime[i_idx]));
}



void Sol::UpdateSlackForAllNodesInsertion(Driver *d, std::vector<Node*>& routeNodes, std::vector<double>& beginningTime, std::vector<double>& waitingTime, std::vector<double>& slackTimes) {
    slackTimes.assign(routeNodes.size(), 9000); // Initialize with a large number

    for (size_t idx = 0; idx < routeNodes.size(); ++idx) {
        Node* current = routeNodes[idx];
        if (current->type != Parameters::END_LINK) {
            slackTimes[idx] = UpdateSlackNewInsertion(d, current, beginningTime, waitingTime, routeNodes);
        }
    }

    size_t endIdx = routeNodes.size() - 1;
    slackTimes[endIdx] = routeNodes.back()->due - beginningTime[endIdx];
}


double Sol::CalculateTotalAvailabilityWaitingTime() {
    double totalAvailabilityWaitingTime = 0.0;

    // Iterate over all nodes and sum their AvailabilityWaitingTime
    for (int i = 0; i < AvailabilityWaitingTime.size(); ++i) {
        totalAvailabilityWaitingTime += AvailabilityWaitingTime[i];
    }

    return totalAvailabilityWaitingTime;
}



void Sol::FindTimesInsertion(Driver* d,  std::vector<Node*>& routeNodes, std::vector<double>& arrivalTimes, std::vector<double>& departureTimes, std::vector<double>& beginningTimes, std::vector<double>& waitingTimes, std::vector<double>& sumWaitingTimes) {


    size_t numNodes = routeNodes.size();

    if (numNodes == 0) {
        std::cerr << "Error: routeNodes is empty!" << std::endl;
        return;
    }


        arrivalTimes.resize(numNodes, 0.0);
        departureTimes.resize(numNodes, 0.0);
        beginningTimes.resize(numNodes, 0.0);
        waitingTimes.resize(numNodes, 0.0);
        sumWaitingTimes.resize(numNodes, 0.0);


   // arrivalTimes.resize(routeNodes.size(), 0.0);
   // departureTimes.resize(routeNodes.size(), 0.0);
   // beginningTimes.resize(routeNodes.size(), 0.0);
   // waitingTimes.resize(routeNodes.size(), 0.0);
    //sumWaitingTimes.resize(routeNodes.size(), 0.0);

    double currentTime = d->availability.first;

    for (size_t i = 0; i < routeNodes.size(); ++i) {
        Node* current = routeNodes[i];

        arrivalTimes[i] = currentTime;
        beginningTimes[i] = std::max(arrivalTimes[i], current->ready);
        waitingTimes[i] = beginningTimes[i] - arrivalTimes[i];
        departureTimes[i] = beginningTimes[i] + current->serviceTime;

        Node* next = (i + 1 < routeNodes.size()) ? routeNodes[i + 1] : nullptr;
        if (next == nullptr || next->type == Parameters::END_LINK) {
            double travelTimeToDepot = Distance(current, next, d);
            arrivalTimes.back() = departureTimes[i] + travelTimeToDepot;
            beginningTimes.back() =std::max(arrivalTimes.back() , next->ready);//  arrivalTimes.back();
            waitingTimes.back() = beginningTimes.back() - arrivalTimes.back();
            departureTimes.back() = beginningTimes.back() + next->serviceTime;
            break;  // Reached the end of the route
        }

        double travelTime = Distance(current, next, d);
        currentTime = departureTimes[i] + travelTime;
    }

    // Calculate sum waiting times
    for (size_t i = 0; i < routeNodes.size(); ++i) {
        double sum = 0;
        for (size_t j = i + 1; j < routeNodes.size(); ++j) {
            sum += waitingTimes[j];
        }
        sumWaitingTimes[i] = sum;
    }
}

struct CustomerInfo {
    Node* customer;
    Node* next;
    Node* prev;
};






Shift Sol::FindSmallestShiftForAssignment(Driver *d1, Driver *d2){// smallest shift of driver 1 that can do route of driver 2

    Shift oldRouteTime = d1->shift;

    Node *FirstNode = GetNode(d2->StartNodeID);
    Node *FirstCustomer = DriverNext[FirstNode->id];

    Node *LasttNode = GetNode(d2->EndNodeID);
    Node *LastCustomer = DriverPrev[LasttNode->id];



    //  const auto& shifts = GetData()->GetShifts(); // Assume this returns a std::vector<Shift>
    const auto& shifts = d1->GetDriverShiftSet();
    for (int j = 0; j < shifts.size(); j++) {
        // Compare start and end times of shifts to find the largest shift that fits the criteria
        if (shifts[j].start <= (FirstCustomer->due - Distance(FirstNode, FirstCustomer, d2))  && shifts[j].end >= LasttNode->ready+ Distance(LastCustomer, LasttNode, d2)) {
            if (shifts[j].end- shifts[j].start < oldRouteTime.end-oldRouteTime.start){
                d1->shift = shifts[j]; // Assign the matching shift to the driver
            }
        }
    }

    return d1->shift;

}



//Shift Sol::FindNewShiftAfterRemoven
Shift Sol::FindNewShiftAfterRemoven(Node *node, Driver *driver){// we have a route without node n and we need to find the shift based on it

    Shift newShift = driver->shift;
    //std::pair<int, int> newShift= driver->DriverShift;//initialize
    Shift newRouteTime;
   // std::pair<int, int> NewRoueTime;

    std::vector<Node *> DriverNextRemove= DriverNext;
    std::vector<Node *> DriverPrevRemove= DriverPrev;

    if (DriverNextRemove[node->id] != nullptr)
        DriverPrevRemove[DriverNextRemove[node->id]->id] = DriverPrevRemove[node->id];
    if (DriverPrev[node->id] != nullptr)
        DriverNextRemove[DriverPrevRemove[node->id]->id] = DriverNextRemove[node->id];

    DriverNextRemove[node->id] = nullptr;
    DriverPrevRemove[node->id] = nullptr;

   // newRouteTime.start= DriverNextRemove[driver->StartNodeID]->due- DriverNextRemove[driver->StartNodeID]->serviceTime- Distance(
         //   GetNode(driver->StartNodeID), GetNode(DriverNextRemove[driver->StartNodeID]->id));
     newRouteTime.start= DriverNextRemove[driver->StartNodeID]->due - Distance(
       GetNode(driver->StartNodeID), GetNode(DriverNextRemove[driver->StartNodeID]->id));
    double currentTime= newRouteTime.start;

    Node *current = GetNode(driver->StartNodeID);
    while (current != nullptr && current->type != Parameters::END_LINK) {

        Node *next = DriverNextRemove[current->id];

        if (next == nullptr || next->type == Parameters::END_LINK) {
            // If next is the depot, update its arrival time
            if (next != nullptr && next->type == Parameters::END_LINK) {
                double travelTimeToDepot = Distance(current, next, driver);
                currentTime = currentTime + travelTimeToDepot;
            }
            break;  // Reached the end of the route
        }


        // Calculate travel time to the next node
        double travelTime = Distance(current, next, driver);
        double startTimeAtNext = std::max(currentTime + travelTime, next->ready);

        // Update currentTime for the next iteration
        currentTime = startTimeAtNext + next->serviceTime;

        current = next;


    }

    newRouteTime.end= currentTime;

    const auto& shifts = driver->GetDriverShiftSet();//GetData()->GetShifts();
    //const auto& shifts = GetData()->GetShifts();
    for (int j = 0; j < shifts.size(); j++){

        if ((shifts[j].start) <= newRouteTime.start && shifts[j].end >= newRouteTime.end ) {
            if(shifts[j].end-shifts[j].start< newShift.end- newShift.start ){
                newShift.start= shifts[j].start;
                newShift.end= shifts[j].end;
            }
        }
    }

    return newShift;
}


Shift Sol:: FindNewShiftAfterInsertion(Node *node, Node *prev){

    Driver *d = DriverAssignTo[prev->id];

    std::vector<Node *> DriverNextInsert= DriverNext;
    std::vector<Node *> DriverPrevInsert= DriverPrev;

    DriverNextInsert[node->id] = DriverNextInsert[prev->id];// heere we have insert after prev function
    DriverPrevInsert[node->id] = prev;
    if (DriverNextInsert[prev->id] != nullptr)
        DriverPrevInsert[DriverNextInsert[prev->id]->id] = node;
    DriverNextInsert[prev->id] = node;
    // assert(DriverNextInsert[node->id] != node);

    Shift NewRouteTime; // Use Shift instead of std::pair<int, int>
    Shift newShift = d->shift;
   // std::pair<int, int> NewRoueTime;
    //std::pair<int, int> newShift= d->Shift;//initialize


    //NewRouteTime.start= DriverNextInsert[d->StartNodeID]->due- DriverNextInsert[d->StartNodeID]->serviceTime- Distance(
          //  GetNode(d->StartNodeID), GetNode(DriverNextInsert[d->StartNodeID]->id));
    NewRouteTime.start= DriverNextInsert[d->StartNodeID]->due - Distance(
      GetNode(d->StartNodeID), GetNode(DriverNextInsert[d->StartNodeID]->id));
    double currentTime= NewRouteTime.start;

    Node *current = GetNode(d->StartNodeID);
    while (current != nullptr && current->type != Parameters::END_LINK) {

        Node *next = DriverNextInsert[current->id];

        if (next == nullptr || next->type == Parameters::END_LINK) {
            // If next is the depot, update its arrival time
            if (next != nullptr && next->type == Parameters::END_LINK) {
                double travelTimeToDepot = Distance(current, next, d);
                currentTime = currentTime + travelTimeToDepot;
            }
            break;  // Reached the end of the route
        }


        // Calculate travel time to the next node
        double travelTime = Distance(current, next, d);
        double startTimeAtNext = std::max(currentTime + travelTime, next->ready);

        // Update currentTime for the next iteration
        currentTime = startTimeAtNext + next->serviceTime;

        current = next;

    }

    NewRouteTime.end= currentTime;

    //const auto& shifts = GetData()->GetShifts();

    const auto& shifts = d->GetDriverShiftSet();
    for (int j = 0; j < shifts.size(); j++){

        if ((shifts[j].start) <= NewRouteTime.start && shifts[j].end >= NewRouteTime.end ) {
            if(shifts[j].end-shifts[j].start< newShift.end- newShift.start ){
                newShift.start= shifts[j].start;
                newShift.end= shifts[j].end;
            }
        }
    }
    return newShift;

}






double Sol::smallestDivisibleBy30AfterOrEqual(double s) {
    double remainder = std::fmod(s, 30.0);
    if (remainder == 0.0) {
        return s; // s is already divisible by 30
    } else {
        return s + (30.0 - remainder); // Find the next multiple of 30
    }
}

Shift Sol::FindNewShiftAfterSwap( Driver *driver ){

    Shift NewRouteTime;
    Shift newShift;
    newShift.start= 0;
    newShift.end= 99999999999999;

    Shift invalidShift(0, 0);




    NewRouteTime.start= DriverNext[driver->StartNodeID]->due - Distance(
            GetNode(driver->StartNodeID), GetNode(DriverNext[driver->StartNodeID]->id), driver);

    NewRouteTime.end= DriverPrev[driver->EndNodeID]->ready + Distance(
            GetNode(DriverPrev[driver->EndNodeID]->id), GetNode(driver->EndNodeID),driver);


    bool foundnewShift = false;


    const auto& shifts = driver->GetDriverShiftSet();
    for (int j = 0; j < shifts.size(); j++){

        if ((shifts[j].start) <= NewRouteTime.start && shifts[j].end >= NewRouteTime.end ) {
            if(shifts[j].end-shifts[j].start< newShift.end- newShift.start ) {
                foundnewShift = true;
                newShift.start = shifts[j].start;
                newShift.end = shifts[j].end;
            }
        }
    }
    if (foundnewShift ){
        return newShift;
    }
    else{
        return invalidShift;
    }
};






void Sol::GenerateTikz(std::string fullPath) {

    // Extract base name from fullPath
    size_t lastSlashPos = fullPath.find_last_of("/\\");
    std::string baseName = fullPath.substr(lastSlashPos + 1); // +1 to skip the slash itself

    // Remove the file extension if present
    size_t lastDotPos = baseName.find_last_of('.');
    if (lastDotPos != std::string::npos) {
        baseName = baseName.substr(0, lastDotPos);
    }

    // Filename based on instance name to avoid overwriting diagrams of different instances
    std::string tikzFilename = "solution_" + baseName + ".tex";
    std::ofstream outFile(tikzFilename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open " << tikzFilename << " for writing.\n";
        return;
    }

    // Begin the TikZ document
    outFile << "\\documentclass[tikz,border=10pt]{standalone}\n";
    outFile << "\\usepackage{tikz}\n";
    outFile << "\\begin{document}\n";
    outFile << "\\begin{tikzpicture}[>=stealth, node distance=2cm]\n";

    // Define colors for up to 5 drivers
    std::vector<std::string> colors = {"red", "blue", "green", "orange", "purple"};

    Node* depot = _data->GetDepot(0); // Assume there is one depot
    // Draw Depot
    outFile << "\\node[draw, circle, fill=lightgray] (depot) at (" << depot->x << "," << depot->y << ") {" << depot->id << "};\n";

    // Draw Customers
    for (auto customerId : _data->GetCustomers()) {
        Node* customer = _data->GetNode(customerId);
        outFile << "\\node[draw, circle, fill=white] (n" << customer->id << ") at (" << customer->x << "," << customer->y << ") {" << customer->id << "};\n";
    }

    // Draw Paths with different colors for each driver
    for (int i = 0; i < GetDriverCount(); ++i) {
        Driver *d = GetDriver(i);
        Node *current = GetNode(d->StartNodeID);
        Node *next = DriverNext[current->id];
        std::string color = colors[i % colors.size()]; // Cycle through colors

        // Draw path from depot to the first node
        if (current->type != Parameters::TypeNode::DEPOT && next != nullptr && d->load > 0) {
            outFile << "\\draw[" << color << ",->] (depot) -- (n" << next->id << ");\n";
        }

        // Draw path for the rest of the nodes
        while (next != nullptr && DriverNext[next->id] != nullptr) {
            Node* afterNext = DriverNext[next->id];
            if (afterNext->type != Parameters::TypeNode::END_LINK) {
                outFile << "\\draw[" << color << ",->] (n" << next->id << ") -- (n" << afterNext->id << ");\n";
            } else {
                outFile << "\\draw[" << color << ",->] (n" << next->id << ") -- (depot);\n";
                break;
            }
            next = afterNext;
        }
    }

    // End the TikZ document
    outFile << "\\end{tikzpicture}\n";
    outFile << "\\end{document}\n";
    outFile.close();

    std::cout << "TikZ diagram for drivers' routes generated as " << tikzFilename << std::endl;

    // Compile the .tex file to PDF using pdflatex
    std::string command = "pdflatex " + tikzFilename;
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Failed to compile TikZ file to PDF." << std::endl;
    } else {
        std::cout << "Compiled TikZ file to PDF successfully." << std::endl;
    }
}
