#include <algorithm>

#include "ListMoveRvrp.h"


void ListMoveRvrp::Clear()
{
	_moves.clear();
}
void ListMoveRvrp::Add(Move<Node,Driver,MoveVrp> & m)
{
	_moves.push_back(m);
}
Move<Node, Driver, MoveVrp> & ListMoveRvrp::Get(int i)
{
	return _moves[i];
}

int ListMoveRvrp::Count()
{
	return (int)_moves.size();
}

void ListMoveRvrp::Sort()
{
	sort(_moves.begin(), _moves.end());
}
void ListMoveRvrp::Insert(int i, Move<Node, Driver, MoveVrp>& m)
{
	_moves.at(i) = m;
}
void ListMoveRvrp::Resize(int n)
{
	_moves.resize(n);
}
	void ListMoveRvrp::partial_Sort(int k)
{
	partial_sort(_moves.begin(), _moves.begin() + k, _moves.end(),move_sorter);
	
}
