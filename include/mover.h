/*
 * mover.h
 *
 *  Created on: Sep 19, 2017
 *      Author: eliha
 */

#ifndef INCLUDE_MOVER_H_
#define INCLUDE_MOVER_H_

#include <stack>
#include <vector>

namespace Tiles
{

struct Moove;
class Rack;
class Board;
struct MoveGroupSummary;
struct GroupIdInfo;

class Mover
{
private:
	Rack& _rack;
	Board& _board;
	bool _debug;

	void debugDoMove(Moove& move);
	void debugUndoMove(Moove& move);

public:
	Mover(Rack& rack, Board& board, bool debug) : _rack(rack), _board(board), _debug(debug) { }

	void doMove(Moove& move);
	void undoMove(Moove& move);

	void print() const;
	void printWithName() const;

};

} // namespace Tiles




#endif /* INCLUDE_MOVER_H_ */
