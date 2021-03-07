/*
 * player.cpp
 *
 *  Created on: Sep 7, 2017
 *      Author: eliha
 */

#include <iostream>
#include <fstream>

#include "stdafx.h"
#include <memory>
#include <iostream>
#include "time.h"
#include "player.h"
#include "rack.h"
#include "board.h"
#include "pool.h"
#include "group.h"
#include "move.h"
#include "utils.h"
#include "solution.h"
#include "move_planner.h"
#include "move_receiver.h"

namespace Tiles
{

//static bool DEBUG_SOLUTION = false;
//static bool DEBUG_MOVES = false;
//static bool DEBUG_LOGIC = false;

Player::Player(int id, bool isHuman, int preMeldTurnTimeLimitMs, int postMeldTurnTimeLimitMs, std::shared_ptr<Rack> rack, Board& board, Pool& pool) :
		_id(id), _isHuman(isHuman), _name(""), _hasMelded(false), _isWinner(false),
		_preMeldTurnTimeLimitMs(preMeldTurnTimeLimitMs), _postMeldTurnTimeLimitMs(postMeldTurnTimeLimitMs),
		_didSomethingOnLastTurn(false), _meldedOnLastTurn(false), _drewTileOnLastTurn(false),
		_rack(rack), _board(board), _pool(pool),
		_maxLevel(1000), _printAlot(false), _debugLogic(false), _debugMoves(false)
{
}

void Player::applySolution(std::shared_ptr<Solution> solution)
{
	_rack->startTransaction();
	solution->applyAllMoves();
	_rack->commitTransaction();
	_board.trapJokers();
}

void Player::goComputer(bool drawTileIfNeeded)
{
	_didSomethingOnLastTurn = false;
	_meldedOnLastTurn = false;
	_drewTileOnLastTurn = false;
	_movePlanners.clear();

	int timeLimitMs = _postMeldTurnTimeLimitMs;

	if (!_hasMelded)
	{
		int timeLimitMs = _preMeldTurnTimeLimitMs;

	    std::shared_ptr<MovePlanner> movePlanner = std::make_shared<MovePlanner>(shared_from_this(), _rack, _board, _id, !_hasMelded, _maxLevel, -1, timeLimitMs, _debugMoves, _debugLogic);
	    _movePlanners.push_back(movePlanner);

	    movePlanner->planMove();

		if (movePlanner->getSolutionScore() >= 30)
		{
			applySolution(movePlanner->getBestSolution());

			_didSomethingOnLastTurn = true;
			_meldedOnLastTurn = true;
			_hasMelded = true;
		}
		else
		{
			drawTileFromPool(drawTileIfNeeded);
			return;
		}
	}

	// If here, the player has melded, either in this turn or an earlier turn.

    std::shared_ptr<MovePlanner> movePlanner = std::make_shared<MovePlanner>(shared_from_this(), _rack, _board, _id, !_hasMelded, _maxLevel, -1, timeLimitMs, _debugMoves, _debugLogic);
    _movePlanners.push_back(movePlanner);

    movePlanner->planMove();

    if (movePlanner->foundMoves())
    {
		applySolution(movePlanner->getBestSolution());
		_didSomethingOnLastTurn = true;
    }
	else if (!_meldedOnLastTurn)
	{
		drawTileFromPool(drawTileIfNeeded);
	}

}

void Player::goHuman()
{
	_didSomethingOnLastTurn = false;
	_meldedOnLastTurn = false;
	_drewTileOnLastTurn = false;
	_movePlanners.clear();

	if (!_hasMelded)
	{
	    std::shared_ptr<MoveReceiver> moveReceiver = std::make_shared<MoveReceiver>(shared_from_this(), _rack, _board, !_hasMelded);
	    _moveReceivers.push_back(moveReceiver);

	    moveReceiver->receiveSolution();

		if (moveReceiver->getSolutionScore() >= 30)
		{
			applySolution(moveReceiver->getSolution());

			_didSomethingOnLastTurn = true;
			_meldedOnLastTurn = true;
			_hasMelded = true;
		}
		else
		{
			drawTileFromPool(true);
			return;
		}
	}

	// If here, the player has melded, either in this turn or an earlier turn.

    std::shared_ptr<MoveReceiver> moveReceiver = std::make_shared<MoveReceiver>(shared_from_this(), _rack, _board, !_hasMelded);
    _moveReceivers.push_back(moveReceiver);

    moveReceiver->receiveSolution();

    if (moveReceiver->hasMoves())
    {
		applySolution(moveReceiver->getSolution());
		_didSomethingOnLastTurn = true;
    }
	else if (!_meldedOnLastTurn)
	{
		drawTileFromPool(true);
	}

}

void Player::drawTileFromPool(bool drawTileIfNeeded)
{
	if (_pool.numTilesRemaining() > 0 && drawTileIfNeeded)
	{
		_rack->addTile(_pool.takeRandomTile());
		_drewTileOnLastTurn = true;
	}
}

void Player::printStatus()
{
	std::cout << "Player " << _id << ": melded? " << (_hasMelded ? "yes" : "no") << " ";
	std::cout << "Player " << _id << ": numTiles: " << _rack->numTiles() << " ";
	_rack->printStatus();
}

void Player::printStatus(std::ofstream& stream) const
{
	stream << "Player " << _id << ": melded? " << (_hasMelded ? "yes" : "no") << " ";
	stream << "Player " << _id << ": numTiles: " << _rack->numTiles() << " ";
	_rack->printStatus(stream);
}

void Player::printLastMoveSummary()
{
	std::cout << "Player " << _id << " to move." << std::endl;
	int indexToPrint = 0;

	if (!didSomethingOnLastTurn())
	{
		std::cout << "No moves";
		if (drewTileOnLastTurn())
		{
			std::cout << " - drew from pool " << _rack->getTile(_rack->numTiles() - 1)->str() << std::endl;
		}
		else
		{
			std::cout << " and pool is empty." << std::endl;
		}
	}
	else
	{
		if (meldedOnLastTurn())
		{
			std::cout << "Melded with moves:" << std::endl;

			_movePlanners[0]->getBestSolution()->print();
			std::cout << "Stop reason: " << StopReasonStr[(int)_movePlanners[0]->stopReason()];
			std::cout << " Max level run: " << _movePlanners[0]->maxLevel();
			std::cout << " Max level reached: " << _movePlanners[0]->maxLevelReached();
			std::cout << " Time took: " << _movePlanners[0]->timeTookSec();
			std::cout << " Time to best solution: " << _movePlanners[0]->secsToBestSolution();
			std::cout << " Moves to best solution: " << _movePlanners[0]->numMovesToBestSolution();
			int totalMoves = 0;
			for (int i = 0; i < _movePlanners[0]->numLevelsWithMoves(); ++i)
			{
				//std::cout << "Num moves level " << i << ": " << _meldNumMovesPerLevel[i] << std::endl;
				totalMoves += _movePlanners[0]->numMovesPerLevel(i);
			}
			std::cout << " Total moves: " << totalMoves << std::endl;
			if (_movePlanners[1]->foundMoves())
			{
				std::cout << "Moves after meld:" << std::endl;
			}
			++indexToPrint;
		}
		else
		{
			std::cout << "Moves:" << std::endl;
		}
		_movePlanners[indexToPrint]->getBestSolution()->print();
	}
	std::cout << "Stop reason: " << StopReasonStr[(int)_movePlanners[indexToPrint]->stopReason()];
	std::cout << " Max level run: " << _movePlanners[indexToPrint]->maxLevel();
	std::cout << " Max level reached: " << _movePlanners[indexToPrint]->maxLevelReached();
	std::cout << " Time took: " << _movePlanners[indexToPrint]->timeTookSec();
	std::cout << " Time to best solution: " << _movePlanners[indexToPrint]->secsToBestSolution();
	std::cout << " Moves to best solution: " << _movePlanners[indexToPrint]->numMovesToBestSolution();
	int totalMoves = 0;
	for (int i = 0; i < _movePlanners[indexToPrint]->numLevelsWithMoves(); ++i)
	{
		//std::cout << "Num moves level " << i << ": " << _numMovesPerLevel[i] << std::endl;
		totalMoves += _movePlanners[indexToPrint]->numMovesPerLevel(i);
	}
	std::cout << " Total moves: " << totalMoves << std::endl;

	std::cout << std::endl;
}

void Player::printLastMoveSummary(std::ofstream& stream)
{
	stream << "Player " << _id << " to move." << std::endl;
	int indexToPrint = 0;

	if (!didSomethingOnLastTurn())
	{
		stream << "No moves";
		if (drewTileOnLastTurn())
		{
			stream << " - drew from pool " << _rack->getTile(_rack->numTiles() - 1)->str() << std::endl;
		}
		else
		{
			stream << " and pool is empty." << std::endl;
		}
	}
	else
	{
		if (meldedOnLastTurn())
		{
			stream << "Melded with moves:" << std::endl;

			_movePlanners[0]->getBestSolution()->print(stream);
			stream << "Stop reason: " << StopReasonStr[(int)_movePlanners[0]->stopReason()];
			stream << " Max level run: " << _movePlanners[0]->maxLevel();
			stream << " Max level reached: " << _movePlanners[0]->maxLevelReached();
			stream << " Time took: " << _movePlanners[0]->timeTookSec();
			stream << " Time to best solution: " << _movePlanners[0]->secsToBestSolution();
			stream << " Moves to best solution: " << _movePlanners[0]->numMovesToBestSolution();
			int totalMoves = 0;
			for (int i = 0; i < _movePlanners[0]->numLevelsWithMoves(); ++i)
			{
				//std::cout << "Num moves level " << i << ": " << _meldNumMovesPerLevel[i] << std::endl;
				totalMoves += _movePlanners[0]->numMovesPerLevel(i);
			}
			stream << " Total moves: " << totalMoves << std::endl;
			if (_movePlanners[1]->foundMoves())
			{
				stream << "Moves after meld:" << std::endl;
			}
			++indexToPrint;
		}
		else
		{
			stream << "Moves:" << std::endl;
		}
		_movePlanners[indexToPrint]->getBestSolution()->print(stream);
	}
	stream << "Stop reason: " << StopReasonStr[(int)_movePlanners[indexToPrint]->stopReason()];
	stream << " Max level run: " << _movePlanners[indexToPrint]->maxLevel();
	stream << " Max level reached: " << _movePlanners[indexToPrint]->maxLevelReached();
	stream << " Time took: " << _movePlanners[indexToPrint]->timeTookSec();
	stream << " Time to best solution: " << _movePlanners[indexToPrint]->secsToBestSolution();
	stream << " Moves to best solution: " << _movePlanners[indexToPrint]->numMovesToBestSolution();
	int totalMoves = 0;
	for (int i = 0; i < _movePlanners[indexToPrint]->numLevelsWithMoves(); ++i)
	{
		//std::cout << "Num moves level " << i << ": " << _numMovesPerLevel[i] << std::endl;
		totalMoves += _movePlanners[indexToPrint]->numMovesPerLevel(i);
	}
	stream << " Total moves: " << totalMoves << std::endl;

	stream << std::endl;
}

void Player::printLastMoveSummary(std::shared_ptr<Tile> tile)
{
	std::cout << "Player " << _id << " to move." << std::endl;
	int indexToPrint = 0;

	if (!didSomethingOnLastTurn())
	{
		std::cout << "No moves - drew from pool " << tile->str() << std::endl;
	}
	else
	{
		if (meldedOnLastTurn())
		{
			std::cout << "Melded with moves:" << std::endl;
			_movePlanners[0]->getBestSolution()->print();
			std::cout << "Stop reason: " << StopReasonStr[(int)_movePlanners[0]->stopReason()];
			std::cout << " Max level run: " << _movePlanners[0]->maxLevel();
			std::cout << " Max level reached: " << _movePlanners[0]->maxLevelReached();
			std::cout << " Time took: " << _movePlanners[0]->timeTookSec();
			std::cout << " Time to best solution: " << _movePlanners[0]->secsToBestSolution();
			std::cout << " Moves to best solution: " << _movePlanners[0]->numMovesToBestSolution();
			int totalMoves = 0;
			for (int i = 0; i < _movePlanners[0]->numLevelsWithMoves(); ++i)
			{
				//std::cout << "Num moves level " << i << ": " << _meldNumMovesPerLevel[i] << std::endl;
				totalMoves += _movePlanners[0]->numMovesPerLevel(i);
			}
			std::cout << " Total moves: " << totalMoves << std::endl;
			if (_movePlanners[1]->foundMoves())
			{
				std::cout << "Moves after meld:" << std::endl;
			}
			++indexToPrint;
		}
		else
		{
			std::cout << "Moves:" << std::endl;
		}
		_movePlanners[indexToPrint]->getBestSolution()->print();
	}
	std::cout << "Stop reason: " << StopReasonStr[(int)_movePlanners[indexToPrint]->stopReason()];
	std::cout << " Max level run: " << _movePlanners[indexToPrint]->maxLevel();
	std::cout << " Max level reached: " << _movePlanners[indexToPrint]->maxLevelReached();
	std::cout << " Time took: " << _movePlanners[indexToPrint]->timeTookSec();
	std::cout << " Time to best solution: " << _movePlanners[indexToPrint]->secsToBestSolution();
	std::cout << " Moves to best solution: " << _movePlanners[indexToPrint]->numMovesToBestSolution();
	int totalMoves = 0;
	for (int i = 0; i < _movePlanners[indexToPrint]->numLevelsWithMoves(); ++i)
	{
		//std::cout << "Num moves level " << i << ": " << _numMovesPerLevel[i] << std::endl;
		totalMoves += _movePlanners[indexToPrint]->numMovesPerLevel(i);
	}
	std::cout << " Total moves: " << totalMoves << std::endl;

	std::cout << std::endl;
}

} // namespace Tiles


