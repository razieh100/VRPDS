/* Copyright Jean-Francois Cote 2017
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
*/

#ifndef LIST_MOVE_2Rvrp
#define LIST_MOVE_2Rvrp

#include "Node.h"
#include "Driver.h"
#include "Move.h"
#include "MoveVrp.h"
#include <vector>


struct ListMoveRvrpSorter
{
	bool operator() (Move<Node, Driver, MoveVrp>& n1, Move<Node, Driver, MoveVrp>& n2)
	{
		return (n1.DeltaCost < n2.DeltaCost);
	}
};

class ListMoveRvrp
{
public:
	void Clear();
	void Add(Move<Node, Driver, MoveVrp>& m);
	int Count();
	void Insert(int i, Move<Node, Driver, MoveVrp>& m);
	Move<Node, Driver, MoveVrp>& Get(int i);
	void Sort();
	void Resize(int n);
	void partial_Sort(int k);


	std::vector< Move<Node, Driver, MoveVrp> > _moves;
private:
	ListMoveRvrpSorter move_sorter;
};

#endif
