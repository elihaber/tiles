/*
 * player.h
 *
 *  Created on: Sep 7, 2017
 *      Author: eliha
 */

#ifndef INCLUDE_PLAYER_H_
#define INCLUDE_PLAYER_H_

#ifdef WIN32
#include <time.h>
#endif

#include <iostream>
#include <fstream>

#include <memory>
#include <vector>
#include <unordered_map>
#include <time.h>
#include "defs.h"
#include "solution.h"
#include "board.h"

namespace Tiles
{

class Rack;
class SandboxRack;
class Board;
class Pool;
class Tile;
class Move;
struct Moove;
class Solution;
class MovePlanner;
class MoveReceiver;
//struct GroupSplit;
//struct GroupReform;

class Player : public std::enable_shared_from_this<Player>
{
private:
	int _id;
	bool _isHuman;
	std::string _name;
	bool _hasMelded;
	bool _isWinner;
	int _preMeldTurnTimeLimitMs;
	int _postMeldTurnTimeLimitMs;
	bool _didSomethingOnLastTurn;
	bool _meldedOnLastTurn;
	bool _drewTileOnLastTurn;

	std::vector<std::shared_ptr<MovePlanner>> _movePlanners;
	std::vector<std::shared_ptr<MoveReceiver>> _moveReceivers;

	std::shared_ptr<Rack> _rack;
	Board& _board;
	Pool& _pool;

	int _maxLevel;
	bool _printAlot;
	bool _debugLogic;
	bool _debugMoves;

	void applySolution(std::shared_ptr<Solution> solution);
	void drawTileFromPool(bool drawTileIfNeeded);

public:
	Player(int id, bool isHuman, int preMeldTurnTimeLimitMs, int postMeldTurnTimeLimitMs, std::shared_ptr<Rack> rack, Board& board, Pool& pool);

	std::shared_ptr<Rack> rack() { return _rack; }

	int id() const { return _id; }
	bool isHuman() const { return _isHuman; }
	bool isWinner() const { return _isWinner; }
	bool didSomethingOnLastTurn() const { return _didSomethingOnLastTurn; }
	bool hasMelded() const { return _hasMelded; }
	bool meldedOnLastTurn() const { return _meldedOnLastTurn; }
	bool drewTileOnLastTurn() const { return _drewTileOnLastTurn; }
	void setHasMelded(bool val) { _hasMelded = val; }

	void goComputer(bool drawTileIfNeeded = true);
	void goHuman();

	void setDebugLogic(bool val) { _debugLogic = true; }
	void setDebugMoves(bool val) { _debugMoves = true; }

	void printStatus();
	void printStatus(std::ofstream& stream) const;
	void printLastMoveSummary();
	void printLastMoveSummary(std::ofstream& stream);
	void printLastMoveSummary(std::shared_ptr<Tile> tile);
};

} // namespace Tiles





#endif /* INCLUDE_PLAYER_H_ */
