//
// Created by Razieh Mousavi on 2024-07-08.
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
//

#ifndef VRPD3FEB_PERTURB_H
#define VRPD3FEB_PERTURB_H



#include "Solution.h"
//#include "IAlgorithm.h"
#include "InsRmvMethodP.h"
//#include "../../lib/mathfunc.h"
#include <vector>





template <class NodeT, class DriverT, class MoveT>
class Perturb
{

    struct CustomerInfo {
        Node* customer;
        Driver* driver;
    };

public:
    Perturb(InsRmvMethodP<NodeT,DriverT,MoveT> * insrmv):_insrmv(insrmv){}

    void Optimize( Sol & s)
    {
        RemoveInsertK(s);
        //s.Show();
        std::cout <<" end optimize in relocate"<< std::endl;
    }


    void RemoveInsertK(Sol &s) {
        _insrmv->SetNoise(0);
        s.Update();

        // **Step 1: Remove k customers**
        double x = Parameters::getRandomDoubleValue(0, 40);
        double y = Parameters::getRandomDoubleValue(0, 40);

        for (int i = 0; i < s.GetCustomerCount(); i++) {
            Node* customer = s.GetCustomer(i);
            double distance = std::sqrt(std::pow(customer->x - x, 2) + std::pow(customer->y - y, 2));
            customer->perturbDis = distance;
        }

        std::vector<std::pair<Node*, Driver*>> customers;
        for (int i = 0; i < s.GetCustomerCount(); i++) {
            Node* customer = s.GetCustomer(i);
            Driver* drv = s.GetDriverAssignedTo(customer);
            customers.push_back({customer, drv});
        }

        std::sort(customers.begin(), customers.end(), [](const std::pair<Node*, Driver*>& a, const std::pair<Node*, Driver*>& b) {
            return a.first->perturbDis < b.first->perturbDis;
        });

       
        double baseRate = (s.GetCustomerCount() > 51) ? 0.05 : 0.20;

// **Increase rate by 5% for every failed iteration (no improvement)**
        double rate = baseRate + (Parameters::CountNoImprove * 0.1);

// **Cap rate at 80%**
        if (rate > 0.90) {
            rate = 0.50;
        }

        int k = static_cast<int>(customers.size() * rate);
        if (k < 1) {
            k = 1;  // Ensure at least 1 node is removed
        }

        int removedCount = 0, i = 0;
        while (removedCount < k && i < customers.size()) {
            Node* customer = customers[i].first;
            Driver* driver = customers[i].second;

            if (driver != NULL) {
                _insrmv->ApplyRemoveMove(s, customer);
                removedCount++;
            }
            i++;
        }

        s.Update();
        std::cout << "After removing nodes in perturbation: " << std::endl;
        s.Show(1);

        // **Step 2: Insert unassigned nodes iteratively using a greedy best approach**
        while (s.GetUnassignedCustCount() > 0) {
            Move<Node, Driver, MoveVrp> bestMove;
            Node* bestCustomer = nullptr;

            // **Find best move among all unassigned customers**
            for (int i = 0; i < s.GetUnassignedCustCount(); i++) {
                Node* customer = s.GetUnassignedCust(i);
                if (customer == nullptr) continue;

                for (int r = 0; r < s.GetDriverCount(); r++) {
                    Driver* d = s.GetDriver(r);
                    Move<Node, Driver, MoveVrp> m;
                    _insrmv->InsertCost(s, customer, d, m);

                    if (m.IsFeasible && m.DeltaCost < bestMove.DeltaCost) {
                        bestMove = m;
                        bestCustomer = customer;
                    }
                }
            }

            // **Apply the best move**
            if (bestMove.IsFeasible) {
                _insrmv->ApplyInsertMove(s, bestMove);
                bestMove.to->shift = bestMove.Shift_to;
                bestMove.to->Shift_Cost = ((bestMove.Shift_to.end - bestMove.Shift_to.start) / 60) * bestMove.to->salary;
                s.Update(bestMove.to);
                s.Show(1);
            } else {
                std::cout << "No feasible insertion found for unassigned customers." << std::endl;
                break;
            }
        }

        s.Update();
        std::cout << "After inserting unassigned nodes in perturbation: " << std::endl;
        s.Show(1);
    }



private:
    InsRmvMethodP<NodeT,DriverT,MoveT> * _insrmv;
    std::vector<NodeT*> nodes;
};

#endif //VRPD3FEB_PERTURB_H




