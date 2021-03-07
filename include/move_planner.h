/*
 * move_planner.h
 *
 *  Created on: Nov 6, 2017
 *      Author: eliha
 */

#ifndef INCLUDE_MOVE_PLANNER_H_
#define INCLUDE_MOVE_PLANNER_H_

#include <memory>
#include <vector>
#include <unordered_map>
#include "enums.h"
#include "board.h"
#include "utils.h"

namespace Tiles
{

class Player;
class Rack;
class Tile;
class Moove;
class Solution;

class MovePlanner
{
private:
	// Input items.

	std::shared_ptr<Player> _player;
	std::shared_ptr<Rack> _rack;
	Board& _board;
	int _playerId;
	bool _isBeforeInitialMeld;
	int _maxLevel;
	int _numSteps;
	int _timeLimitMs;
	bool _debugMoves;
	bool _debugLogic;

	// Output items.

	StopReason _stopReason;
	int _maxLevelReached;
	std::shared_ptr<Solution> _bestSolution;
	float _timeTookSec;

	// Items used during planning.

	int _currentMaxLevel;
	bool _hitDeadlineInMiddle;
	bool _usedAllRackTiles;
	int _bestNumTilesUsed;
	bool _deadlineHasPassed;
	std::unordered_map<Board, int> _allAnalyzedBoards;
	std::unordered_map<Board, int> _analyzedBoardsInThisStack;
	std::vector<int> _numMovesPerLevel;
	std::shared_ptr<Solution> _workingSolution;
	bool _attemptedToPassMaxLevel;
	int _maxLevelReachedThisPass;
	int _numMovesThisLevel;
	clock_t _deadline;
	clock_t _startTime;
	float _secsToBestSolution;
	int _numMovesToBestSolution;

	bool hasDeadlinePassed();
	bool isLevelTooHigh(int level);
	StackStopReason doNextMove(int level);
	bool existInvalidGroupsOnBoard() const;
	int getFirstInvalidGroupOnBoard() const;
	void evaluateCurrentSolution();
	void executeMove(std::shared_ptr<Moove>& move);
	void undoMove();

	void getSafeMovesToFixInvalidGroup(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void getUnsafeMovesToFixInvalidGroup(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void getSafeMovesFromRack(std::vector<std::shared_ptr<Moove>>& moves);
	void getUnsafeMovesFromRack(std::vector<std::shared_ptr<Moove>>& moves);

	std::shared_ptr<Moove> createBoardMove(int sourceGroupId, int sourceGroupPosition, int targetGroupId, int targetGroupPosition, MoveSourceType moveSourceType, MoveDestinationType moveDestinationType, std::shared_ptr<Tile> tile);
	std::shared_ptr<Moove> createRackMove(int positionInRack, int targetGroupId, int positionInTargetGroup, MoveDestinationType moveDestinationType);

	void findSafeBoardTilesForTile(std::shared_ptr<Tile> tile, int targetGroupId, MoveDestinationType moveDestinationType, std::vector<std::shared_ptr<Moove>>& moves);
	void findUnsafeBoardTilesForTile(std::shared_ptr<Tile> tile, int targetGroupId, MoveDestinationType moveDestinationType, std::vector<std::shared_ptr<Moove>>& moves);
	void findSafePlacementForTile(std::shared_ptr<Tile> tile, int originalGroupId, MoveSourceType moveSourceType, int positionInOriginalGroup, MoveDestinationType moveDestinationType, GroupType destinationGroupType, std::vector<std::shared_ptr<Moove>>& moves);
	void findUnsafePlacementForTile(std::shared_ptr<Tile> tile, int originalGroupId, MoveSourceType moveSourceType, int positionInOriginalGroup, MoveDestinationType moveDestinationType, GroupType destinationGroupType, std::vector<std::shared_ptr<Moove>>& moves);
	void findRackTileForTile(std::shared_ptr<Tile> tile, int targetGroupId, MoveDestinationType moveDestinationType, std::vector<std::shared_ptr<Moove>>& moves);
	void findRackJokerForGroup(int targetGroupId, MoveDestinationType moveDestinationType, std::vector<std::shared_ptr<Moove>>& moves);
	void findAllRackTilesForGroup(int targetGroupId, MoveDestinationType moveDestinationType, std::vector<std::shared_ptr<Moove>>& moves);
	void findSafePlacementForRackTile(int rackPosition, MoveDestinationType moveDestinationType, GroupType destinationGroupType, std::vector<std::shared_ptr<Moove>>& moves);
	void findUnsafePlacementForRackTile(int rackPosition, MoveDestinationType moveDestinationType, GroupType destinationGroupType, std::vector<std::shared_ptr<Moove>>& moves);
	void startNewGroupWithRackTile(int rackPosition, std::vector<std::shared_ptr<Moove>>& moves);

	void findRackTilesForInvalidRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findRackTilesForInvalidRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findSafeBoardTilesForInvalidRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findSafeBoardTilesForInvalidRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findRackOrSafeTilesForInvalidRun(int groupId, std::vector<std::shared_ptr<Moove>>& moves);

	void findEndOfRunPlacementForInvalidRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findStartOfRunPlacementForInvalidRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findStartOfRunPlacementForInvalidRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findEndOfRunPlacementForInvalidRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findSetWithoutSplitPlacementForInvalidRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findSetWithoutSplitPlacementForInvalidRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findSafeMiddleOfRunPlacementForInvalidRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findSafeMiddleOfRunPlacementForInvalidRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findSafePlacementForInvalidRun(int groupId, std::vector<std::shared_ptr<Moove>>& moves);

	void findRackTilesForInvalidSet(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findSafeBoardTilesForInvalidSet(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findUnsafeBoardTilesForInvalidSet(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findRackOrSafeTilesForInvalidSet(int groupId, std::vector<std::shared_ptr<Moove>>& moves);

	void findSafePlacementForInvalidSet(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findUnsafePlacementForInvalidSet(int groupId, std::vector<std::shared_ptr<Moove>>& moves);

	void findUnsafeBoardTilesForInvalidRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findUnsafeBoardTilesForInvalidRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findUnsafeTilesForInvalidRun(int groupId, std::vector<std::shared_ptr<Moove>>& moves);

	void findUnsafeMiddleOfRunPlacementForInvalidRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findUnsafeMiddleOfRunPlacementForInvalidRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findUnsafePlacementForInvalidRun(int groupId, std::vector<std::shared_ptr<Moove>>& moves);

	void findRackTilesForInvalidUntypedGroup(int groupId, std::vector<std::shared_ptr<Moove>>& moves);

	void findSafeBoardTilesForInvalidUntypedGroupAsRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findSafeBoardTilesForInvalidUntypedGroupAsRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findSafeBoardTilesForInvalidUntypedGroupAsSet(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findUnsafeBoardTilesForInvalidUntypedGroupAsRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findUnsafeBoardTilesForInvalidUntypedGroupAsRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves);
	void findUnsafeBoardTilesForInvalidUntypedGroupAsSet(int groupId, std::vector<std::shared_ptr<Moove>>& moves);

public:
	MovePlanner(std::shared_ptr<Player> player, std::shared_ptr<Rack> rack, Board & board, int playerId, bool isBeforeInitialMeld, int maxLevel, int numSteps, int timeLimitMs, bool debugMoves, bool debugLogic);

	void planMove();

	std::shared_ptr<Solution> getBestSolution() const { return _bestSolution; }
	int getSolutionScore();
	bool foundMoves();
	StopReason stopReason() const { return _stopReason; }
	int maxLevel() const { return _maxLevel; }
	int maxLevelReached() const { return _maxLevelReached; }
	float timeTookSec() const { return _timeTookSec; }
	float secsToBestSolution() const { return _secsToBestSolution; }
	int numMovesToBestSolution() const { return _numMovesToBestSolution; }
	int numLevelsWithMoves() const { return _numMovesPerLevel.size(); }
	int numMovesPerLevel(int level) const { return _numMovesPerLevel[level]; }


};

}



#endif /* INCLUDE_MOVE_PLANNER_H_ */
