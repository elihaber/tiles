/*
 * move_planner.cpp
 *
 *  Created on: Nov 6, 2017
 *      Author: eliha
 */

#include "move_planner.h"
#include "solution.h"
#include "rack.h"
#include "defs.h"
#include "player.h"

namespace Tiles
{

static bool DEBUG_SOLUTION = false;

MovePlanner::MovePlanner(std::shared_ptr<Player> player, std::shared_ptr<Rack> rack, Board & board, int playerId, bool isBeforeInitialMeld, int maxLevel, int numSteps, int timeLimitMs, bool debugMoves, bool debugLogic) :
		_player(player), _rack(rack), _board(board), _playerId(playerId), _isBeforeInitialMeld(isBeforeInitialMeld), _maxLevel(maxLevel), _numSteps(numSteps), _timeLimitMs(timeLimitMs), _debugMoves(debugMoves), _debugLogic(debugLogic)
{

}

int MovePlanner::getSolutionScore()
{
	return Utils::calculateSolutionScore(*_bestSolution);
}

bool MovePlanner::foundMoves()
{
	return (_bestSolution->numMoves() > 0);
}

bool MovePlanner::hasDeadlinePassed()
{
	if (_deadlineHasPassed)
	{
		return true;
	}
	auto now = clock();
	if (now > _deadline)
	{
		_deadlineHasPassed = true;
		return true;
	}

	return false;
}

bool MovePlanner::isLevelTooHigh(int level)
{
	if (level > _maxLevelReached)
	{
		_maxLevelReached = level;
	}
	if (level > _maxLevelReachedThisPass)
	{
		_maxLevelReachedThisPass = level;
	}
	return (level > _currentMaxLevel);
}

bool MovePlanner::existInvalidGroupsOnBoard() const
{
	return _board.existInvalidGroups();
}

int MovePlanner::getFirstInvalidGroupOnBoard() const
{
	return _board.getFirstInvalidGroup();
}

void MovePlanner::evaluateCurrentSolution()
{
	int numTilesUsed = _rack->numTilesInUse();

	if (numTilesUsed > _bestNumTilesUsed || (numTilesUsed == _bestNumTilesUsed && _workingSolution->score() > _bestSolution->score()))
	{
		if (_debugLogic)
		{
			std::cout << "Found new best solution (" << numTilesUsed << " tiles, " << _workingSolution->numMoves() << " moves)" << std::endl;
		}

	    clock_t tend = clock();
	    auto clocksTook = tend - _startTime;
	    _secsToBestSolution = ((float)clocksTook)/CLOCKS_PER_SEC;

		_bestNumTilesUsed = numTilesUsed;

		_bestSolution->clear();
		for (int i = 0; i < _workingSolution->numMoves(); ++i)
		{
			auto move = _workingSolution->getMove(i);
			_bestSolution->addMove(_workingSolution->getMove(i));
			if (_debugLogic)
			{
				std::cout << i << ": " << move->str() << std::endl;
			}
		}

		_numMovesToBestSolution = 0;
		for (unsigned int i = 0; i < _numMovesPerLevel.size(); ++i)
		{
			_numMovesToBestSolution += _numMovesPerLevel[i];
		}
		_numMovesToBestSolution += _numMovesThisLevel;

		if (_rack->numTiles() == numTilesUsed)
		{
			_usedAllRackTiles = true;
		}
	}
}

void MovePlanner::executeMove(std::shared_ptr<Moove>& move)
{
	_workingSolution->addMove(move);
	_workingSolution->applyNewestMove();
}

void MovePlanner::undoMove()
{
	_workingSolution->unapplyNewestMove();
	_workingSolution->removeNewestMove();
}

StackStopReason MovePlanner::doNextMove(int level)
{
	if (hasDeadlinePassed())
	{
		_hitDeadlineInMiddle = true;
		return StackStopReason::TimedOut;
	}

	if (isLevelTooHigh(level))
	{
		_attemptedToPassMaxLevel = true;
		return StackStopReason::AttemptedToPassLevel;
	}

	auto analyzedBoard = _analyzedBoardsInThisStack.find(_board);
	if (analyzedBoard != _analyzedBoardsInThisStack.end())
	{
		if (_debugLogic)
		{
			std::cout << "This board has already been on the stack" << std::endl;
		}
		return StackStopReason::AlreadyAnalyzed;
	}
	else
	{
		if (_debugLogic)
		{
			std::cout << "This board has not been on the stack, and the stack has " << _analyzedBoardsInThisStack.size() << " boards" << std::endl;
		}
	}

	auto earlierAnalyzedBoard = _allAnalyzedBoards.find(_board);
	if (earlierAnalyzedBoard != _allAnalyzedBoards.end() && earlierAnalyzedBoard->second >= _currentMaxLevel - level)
	{
		return StackStopReason::AlreadyAnalyzed;
	}

	_analyzedBoardsInThisStack[_board] = _currentMaxLevel - level;

	std::vector<std::shared_ptr<Moove>> moves;
	bool isRackMoves = false;

	if (existInvalidGroupsOnBoard())
	{
		int groupId = getFirstInvalidGroupOnBoard();
		getSafeMovesToFixInvalidGroup(groupId, moves);
		if (!_isBeforeInitialMeld)
		{
			getUnsafeMovesToFixInvalidGroup(groupId, moves);
		}
	}
	else
	{
		isRackMoves = true;

		evaluateCurrentSolution();

		if (_usedAllRackTiles)
		{
			return StackStopReason::RackEmpty;
		}

		getSafeMovesFromRack(moves);
		if (!_isBeforeInitialMeld)
		{
			getUnsafeMovesFromRack(moves);
		}
	}

	for (unsigned int i = 0; i < moves.size(); ++i)
	{
		if (_debugLogic)
		{
			if (isRackMoves)
			{
				std::cout << "Level " << level << ": Doing rack move " << i << "/" << moves.size() << std::endl;
			}
			else
			{
				std::cout << "Level " << level << ": Doing invalid group solution " << i << "/" << moves.size() << std::endl;
			}
		}
		if (hasDeadlinePassed())
		{
			_hitDeadlineInMiddle = true;
			return StackStopReason::TimedOut;
		}

		if (_usedAllRackTiles)
		{
			return StackStopReason::RackEmpty;
		}

		++_numMovesThisLevel;

		executeMove(moves[i]);
		auto result = doNextMove(level + 1);
		undoMove();

		if (result == StackStopReason::TimedOut || result == StackStopReason::RackEmpty)
		{
			return result;
		}

		auto lastBoard = _analyzedBoardsInThisStack.find(_board);
		if (lastBoard != _analyzedBoardsInThisStack.end())
		{
			_analyzedBoardsInThisStack.erase(lastBoard);
		}

		_allAnalyzedBoards[_board] = (_currentMaxLevel - level);

	}

	return StackStopReason::None;
}



void MovePlanner::planMove()
{
	_startTime = clock();
    _deadline = static_cast<clock_t>(_startTime + (CLOCKS_PER_SEC * _timeLimitMs / 1000.0));

	_currentMaxLevel = 2;
	_stopReason = StopReason::None;
	_hitDeadlineInMiddle = false;
	_usedAllRackTiles = false;
	_bestNumTilesUsed = 0;
	_deadlineHasPassed = false;
	_maxLevelReached = 0;
	_allAnalyzedBoards.clear();
	_numMovesPerLevel.clear();
	_numMovesPerLevel.push_back(0);
	_numMovesPerLevel.push_back(0);

	if (_isBeforeInitialMeld)
	{
		_board.createGroupIgnoreList();
	}

	_bestSolution = std::make_shared<Solution>(*_rack, _board, DEBUG_SOLUTION, (_debugMoves));

	bool runAnotherLevel = true;
	while (runAnotherLevel)
	{
		if (_debugLogic)
		{
			std::cout << "Starting process with _currentMaxLevel=" << _currentMaxLevel << std::endl;
			_board.printStatus();
			_player->printStatus();
		}

		_rack->startSandbox();
		_workingSolution = std::make_shared<Solution>(*_rack, _board, DEBUG_SOLUTION, (_debugMoves));
		_attemptedToPassMaxLevel = false;
		_maxLevelReachedThisPass = 0;
		_analyzedBoardsInThisStack.clear();
		_numMovesThisLevel = 0;

		doNextMove(1);

		if (_usedAllRackTiles)
		{
			_stopReason = StopReason::RackEmpty;
			runAnotherLevel = false;
		}
		else if (!_attemptedToPassMaxLevel)
		{
			_stopReason = StopReason::NoAttemptToPassLevel;
			runAnotherLevel = false;
		}
		else if (_hitDeadlineInMiddle)
		{
			_stopReason = StopReason::TimeoutInMiddle;
			runAnotherLevel = false;
		}
		else if (hasDeadlinePassed())
		{
			_stopReason = StopReason::TimeoutAtEnd;
			runAnotherLevel = false;
		}
		else if (++_currentMaxLevel > _maxLevel)
		{
			_stopReason = StopReason::CompletedMaxLevel;
			runAnotherLevel = false;
		}

		_rack->endSandbox();
		_numMovesPerLevel.push_back(_numMovesThisLevel);
	}


	//ELIstd::cout << "Process ended with reason: : " << StopReasonStr[(int)_stopReason] << std::endl;

	if (_isBeforeInitialMeld)
	{
		_board.removeGroupIgnoreList();
	}

	clock_t tend = clock();
	auto clocksTook = tend - _startTime;
	_timeTookSec = ((float)clocksTook)/CLOCKS_PER_SEC;

}

}


