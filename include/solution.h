/*
 * solution.h
 *
 *  Created on: Sep 14, 2017
 *      Author: eliha
 */

#ifndef INCLUDE_SOLUTION_H_
#define INCLUDE_SOLUTION_H_

#include <iostream>
#include <fstream>

#include <vector>
#include <memory>
#include <map>
#include "enums.h"
#include "defs.h"

namespace Tiles
{

class Move;
class Rack;
class Board;
class Mover;
struct Moove;

struct GroupSplit
{
	int originalGroup;
	int newGroup1;
	int newGroup2;
};

struct GroupReform
{
	int originalGroup1;
	int originalGroup2;
	int newGroup;
};

class Solution
{
private:
	bool _debug;
	bool _debugMoves;

	std::vector<std::shared_ptr<Moove>> _moves;
	std::vector<std::map<int, int>> _groupMappings;
	std::vector<GroupSplitInfo> _groupSplits;
	std::vector<MoveGroupSummary> _moveGroupSummaries;
	std::vector<int> _newGroupsCreated;

	std::shared_ptr<Mover> _mover;

	void applyMove(std::shared_ptr<Moove> move);

	Board& _board;
public:
	Solution(Rack& rack, Board& board, bool debug, bool debugMoves);
	Solution(Rack& rack, Board& board, std::vector<std::shared_ptr<Moove>>& moves);

	void addMove(std::shared_ptr<Moove> move);
	void applyNewestMove();
	void removeNewestMove();
	void unapplyNewestMove();
	void applyAllMoves();
	void unapplyAllMoves();

	std::shared_ptr<Moove> getMove(int i) const { return _moves[i]; }
	int numMoves() const { return _moves.size(); }
	void clear() { _moves.clear(); }
	std::vector<std::shared_ptr<Moove>>& moves() { return _moves; }
	int score();
	void print() const;
	void print(std::ofstream& stream) const;
	void printWithName() const;

};

} // namespace Tiles



#endif /* INCLUDE_SOLUTION_H_ */
