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
#include "SeqInsertionVrp.h"
#include <algorithm>


using namespace std;



void SeqInsertion::Insert(Sol &s) {
    printf("SeqInsertion::Insert\n");
    s.ChageDeptTWtoAvailability();
    s.Update();

    std::vector<Node *> nodes;
    _insrmv.FillCustomerList(s, nodes);//it adds all unassigned customers to the List

    std::mt19937 random_gen(62);
    shuffle(nodes.begin(), nodes.end(), random_gen);



    for (Node *n: nodes) {

        InsertNode(s, n);

    }

    s.Update();
   // s.UpdateDriverShift();

}

void SeqInsertion::InsertNode(Sol &s, Node *n) {
    Move<Node, Driver, MoveVrp> best;
    best.DeltaCost = INFINI;
    for (int k = 0; k < s.GetDriverCount(); k++) {
        Driver *d = s.GetDriver(k);
        Move<Node, Driver, MoveVrp> m;
        _insrmv.InsertCost(s, n, d, m);
         if (m.IsFeasible && m.DeltaCost < best.DeltaCost)
       // if (m.IsFeasible )
            best = m;
    }
    best.from = nullptr;
    if (best.IsFeasible) {
        _insrmv.ApplyInsertMove(s, best);
        //update shift???????????????????

        //update shift here
        best.to->shift = best.Shift_to;
        best.to->Shift_Cost = ((best.Shift_to.end - best.Shift_to.start)/60) * best.to->salary;

        s.Update(best.to);//update driver having the move best

       // std::cout << "Slack of node: " << n->id << " after insertion: " << s.AvailabilitySlackTime[n->id] << std::endl;


    }
    if (!best.IsFeasible){
        std::cout << "No feasible inssertion: " << n->id << std::endl;

    }
}

