/*
 * move_search.cpp
 *
 *  Created on: Oct 1, 2017
 *      Author: eliha
 */

#include "stdafx.h"
#include "move_planner.h"
#include "board.h"
#include "group.h"
#include "rack.h"
#include "defs.h"

namespace Tiles
{

std::shared_ptr<Moove> MovePlanner::createBoardMove(int sourceGroupId, int sourceGroupPosition, int targetGroupId, int targetGroupPosition, MoveSourceType moveSourceType, MoveDestinationType moveDestinationType, std::shared_ptr<Tile> tile)
{
	return std::make_shared<Moove>(
		Location::Board, Location::Board, -1, -1,
		sourceGroupId, sourceGroupPosition,
		targetGroupId, targetGroupPosition,
		moveSourceType, moveDestinationType,
		tile);
}

std::shared_ptr<Moove> MovePlanner::createRackMove(int positionInRack, int targetGroupId, int positionInTargetGroup, MoveDestinationType moveDestinationType)
{
	return std::make_shared<Moove>(
		Location::Rack, Location::Board,
		_rack->id(), positionInRack, -1, -1,
		targetGroupId, positionInTargetGroup,
		MoveSourceType::Rack, moveDestinationType,
		_rack->getTile(positionInRack));
}

void MovePlanner::findSafeBoardTilesForTile(std::shared_ptr<Tile> tile, int targetGroupId, MoveDestinationType moveDestinationType, std::vector<std::shared_ptr<Moove>>& moves)
{
	std::vector<int> groupIds;
	_board.findGroupsThatCanSafelyLoseThisTile(tile, groupIds);

	for (unsigned int i = 0; i < groupIds.size(); ++i)
	{
		auto sourceGroup = _board.findGroupById(groupIds[i]);
		if (sourceGroup->isRun())
		{
			if (sourceGroup->isTileAtStartOfRun(tile))
			{
				auto move = createBoardMove(sourceGroup->id(), -1, targetGroupId, -1,
						MoveSourceType::StartOfGroup, moveDestinationType, sourceGroup->firstTile());
				moves.push_back(move);
			}
			else if (sourceGroup->isTileAtEndOfRun(tile))
			{
				auto move = createBoardMove(sourceGroup->id(), -1, targetGroupId, -1,
						MoveSourceType::EndOfGroup, moveDestinationType, sourceGroup->lastTile());
				moves.push_back(move);
			}
			else
			{
				int sourcePositionInRun = sourceGroup->getTilePosition(*tile);
				auto move = createBoardMove(sourceGroup->id(), sourcePositionInRun, targetGroupId, -1,
						MoveSourceType::MiddleOfGroupAndSplit, moveDestinationType, sourceGroup->getTile(sourcePositionInRun));
				moves.push_back(move);
			}
		}
		else
		{
			int sourcePositionInSet = sourceGroup->getTilePosition(*tile);
			auto move = createBoardMove(sourceGroup->id(), sourcePositionInSet, targetGroupId, -1,
					MoveSourceType::MiddleOfGroup, moveDestinationType, sourceGroup->getTile(sourcePositionInSet));
			moves.push_back(move);
		}
	}
}

void MovePlanner::findUnsafeBoardTilesForTile(std::shared_ptr<Tile> tile, int targetGroupId, MoveDestinationType moveDestinationType, std::vector<std::shared_ptr<Moove>>& moves)
{
	std::vector<int> groupIds;
	_board.findGroupsThatCanUnsafelyLoseThisTile(tile, groupIds);

	for (unsigned int i = 0; i < groupIds.size(); ++i)
	{
		auto sourceGroup = _board.findGroupById(groupIds[i]);
		if (sourceGroup->isRun())
		{
			if (sourceGroup->isTileAtStartOfRun(tile))
			{
				auto move = createBoardMove(sourceGroup->id(), -1, targetGroupId, -1,
						MoveSourceType::StartOfGroup, moveDestinationType, sourceGroup->firstTile());
				moves.push_back(move);
			}
			else if (sourceGroup->isTileAtEndOfRun(tile))
			{
				auto move = createBoardMove(sourceGroup->id(), -1, targetGroupId, -1,
						MoveSourceType::EndOfGroup, moveDestinationType, sourceGroup->lastTile());
				moves.push_back(move);
			}
			else
			{
				int sourcePositionInRun = sourceGroup->getTilePosition(*tile);
				auto move = createBoardMove(sourceGroup->id(), sourcePositionInRun, targetGroupId, -1,
						MoveSourceType::MiddleOfGroupAndSplit, moveDestinationType, sourceGroup->getTile(sourcePositionInRun));
				moves.push_back(move);
			}
		}
		else
		{
			int sourcePositionInSet = sourceGroup->getTilePosition(*tile);
			auto move = createBoardMove(sourceGroup->id(), sourcePositionInSet, targetGroupId, -1,
					MoveSourceType::MiddleOfGroup, moveDestinationType, sourceGroup->getTile(sourcePositionInSet));
			moves.push_back(move);
		}
	}
}

void MovePlanner::findSafePlacementForTile(std::shared_ptr<Tile> tile, int originalGroupId, MoveSourceType moveSourceType, int positionInInvalidGroup, MoveDestinationType moveDestinationType, GroupType destinationGroupType, std::vector<std::shared_ptr<Moove>>& moves)
{
	std::vector<int> groupIds;
	_board.findGroupsForTile(tile, originalGroupId, groupIds);

	for (unsigned int i = 0; i < groupIds.size(); ++i)
	{
		auto targetGroup = _board.findGroupById(groupIds[i]);

		if (targetGroup->isRun())
		{
			if (!tile->isJoker() && targetGroup->getRunStartValue() == tile->getValue() + 1)
			{
				if (moveDestinationType == MoveDestinationType::Any || moveDestinationType == MoveDestinationType::StartOfGroup)
				{
					auto move = createBoardMove(originalGroupId, positionInInvalidGroup, targetGroup->id(), -1,
							moveSourceType, MoveDestinationType::StartOfGroup, tile);
					moves.push_back(move);
				}
			}
			else if (!tile->isJoker() && targetGroup->getRunEndValue() == tile->getValue() - 1)
			{
				if (moveDestinationType == MoveDestinationType::Any || (moveDestinationType == MoveDestinationType::EndOfGroup && destinationGroupType == GroupType::Run))
				{
					auto move = createBoardMove(originalGroupId, positionInInvalidGroup, targetGroup->id(), -1,
							moveSourceType, MoveDestinationType::EndOfGroup, tile);
					moves.push_back(move);
				}
			}
			else
			{
				if (tile->isJoker())
				{
					if (moveDestinationType == MoveDestinationType::Any || moveDestinationType == MoveDestinationType::MiddleOfGroupAndSplit)
					{
						for (int targetPosition = 2; targetPosition <= targetGroup->numTiles() - 2; ++targetPosition)
						{
							auto move = createBoardMove(originalGroupId, positionInInvalidGroup, targetGroup->id(), targetPosition,
									moveSourceType, MoveDestinationType::MiddleOfGroupAndSplit, tile);
							moves.push_back(move);
						}
					}
				}
				else
				{
					if (moveDestinationType == MoveDestinationType::Any || moveDestinationType == MoveDestinationType::MiddleOfGroupAndSplit)
					{
						int targetPosition = tile->getValue() - targetGroup->getRunStartValue();
						auto move = createBoardMove(originalGroupId, positionInInvalidGroup, targetGroup->id(), targetPosition,
								moveSourceType, MoveDestinationType::MiddleOfGroupAndSplit, tile);
						moves.push_back(move);
					}
				}
			}
		}
		else
		{
			if (moveDestinationType == MoveDestinationType::Any || (moveDestinationType == MoveDestinationType::EndOfGroup && destinationGroupType == GroupType::Set))
			{
				auto move = createBoardMove(originalGroupId, positionInInvalidGroup, targetGroup->id(), -1,
						moveSourceType, MoveDestinationType::EndOfGroup, tile);
				moves.push_back(move);
			}
		}
	}
}

void MovePlanner::findUnsafePlacementForTile(std::shared_ptr<Tile> tile, int originalGroupId, MoveSourceType moveSourceType, int positionInOriginalGroup, MoveDestinationType moveDestinationType, GroupType destinationGroupType, std::vector<std::shared_ptr<Moove>>& moves)
{
	std::vector<int> groupIds;
	_board.findRunsForTileInMiddleWithInvalidSplit(tile, originalGroupId, groupIds);

	for (unsigned int i = 0; i < groupIds.size(); ++i)
	{
		auto targetGroup = _board.findGroupById(groupIds[i]);

		if (tile->isJoker())
		{
			if (targetGroup->numTiles() == 2)
			{
				if (moveDestinationType == MoveDestinationType::Any || moveDestinationType == MoveDestinationType::MiddleOfGroupAndSplit)
				{
					auto move = createBoardMove(originalGroupId, positionInOriginalGroup, targetGroup->id(), 1,
							moveSourceType, MoveDestinationType::MiddleOfGroupAndSplit, tile);
					moves.push_back(move);
				}
			}
			else if (targetGroup->numTiles() == 3 || targetGroup->numTiles() == 4)
			{
				if (moveDestinationType == MoveDestinationType::Any || moveDestinationType == MoveDestinationType::MiddleOfGroupAndSplit)
				{
					for (int position = 1; position < targetGroup->numTiles(); ++position)
					{
						auto move = createBoardMove(originalGroupId, positionInOriginalGroup, targetGroup->id(), position,
								moveSourceType, MoveDestinationType::MiddleOfGroupAndSplit, tile);
						moves.push_back(move);
					}
				}
			}
			else
			{
				if (moveDestinationType == MoveDestinationType::Any || moveDestinationType == MoveDestinationType::MiddleOfGroupAndSplit)
				{
					auto move1 = createBoardMove(originalGroupId, positionInOriginalGroup, targetGroup->id(), 1,
							moveSourceType, MoveDestinationType::MiddleOfGroupAndSplit, tile);
					moves.push_back(move1);

					auto move2 = createBoardMove(originalGroupId, positionInOriginalGroup, targetGroup->id(), targetGroup->numTiles() - 1,
							moveSourceType, MoveDestinationType::MiddleOfGroupAndSplit, tile);
					moves.push_back(move2);
				}
			}
		}
		else
		{
			if (moveDestinationType == MoveDestinationType::Any || moveDestinationType == MoveDestinationType::MiddleOfGroupAndSplit)
			{
				int targetPosition = tile->getValue() - targetGroup->getRunStartValue();
				auto move = createBoardMove(originalGroupId, positionInOriginalGroup, targetGroup->id(), targetPosition,
						moveSourceType, MoveDestinationType::MiddleOfGroupAndSplit, tile);
				moves.push_back(move);
			}
		}
	}
}

void MovePlanner::findRackTileForTile(std::shared_ptr<Tile> tile, int targetGroupId, MoveDestinationType moveDestinationType, std::vector<std::shared_ptr<Moove>>& moves)
{
	int tilePosition = _rack->findUnusedTile(*tile);

	if (tilePosition != -1)
	{
		auto move = createRackMove(tilePosition, targetGroupId, -1, moveDestinationType);
		moves.push_back(move);
	}
}

void MovePlanner::findAllRackTilesForGroup(int targetGroupId, MoveDestinationType moveDestinationType, std::vector<std::shared_ptr<Moove>>& moves)
{
	std::vector<int> unusedNonJokerPositions;
	_rack->getAllUnusedNonJokerPositions(unusedNonJokerPositions);
	for (int position : unusedNonJokerPositions)
	{
		auto move = createRackMove(position, targetGroupId, -1, moveDestinationType);
		moves.push_back(move);
	}
}

void MovePlanner::findRackJokerForGroup(int targetGroupId, MoveDestinationType moveDestinationType, std::vector<std::shared_ptr<Moove>>& moves)
{
	std::vector<int> rackJokerPositions;
	_rack->getAllUnusedJokerPositions(rackJokerPositions);
	if (rackJokerPositions.size() > 0)
	{
		auto move = createRackMove(rackJokerPositions[0], targetGroupId, -1, moveDestinationType);
		moves.push_back(move);
	}
}

void MovePlanner::findSafePlacementForRackTile(int rackPosition, MoveDestinationType moveDestinationType, GroupType destinationGroupType, std::vector<std::shared_ptr<Moove>>& moves)
{
	if (moveDestinationType == MoveDestinationType::StartOfGroup && destinationGroupType == GroupType::Run)
	{
		std::vector<int> runsToAddToStart;
		_board.findRunsForTileAtStart(_rack->getTile(rackPosition), -1, runsToAddToStart);

		for (int groupId : runsToAddToStart)
		{
			auto move = createRackMove(rackPosition, groupId, -1, MoveDestinationType::StartOfGroup);
			moves.push_back(move);
		}
	}
	if (moveDestinationType == MoveDestinationType::EndOfGroup && destinationGroupType == GroupType::Run)
	{
		std::vector<int> runsToAddToEnd;
		_board.findRunsForTileAtEnd(_rack->getTile(rackPosition), -1, runsToAddToEnd);

		for (int groupId : runsToAddToEnd)
		{
			auto move = createRackMove(rackPosition, groupId, -1, MoveDestinationType::EndOfGroup);
			moves.push_back(move);
		}
	}
	if (moveDestinationType == MoveDestinationType::EndOfGroup && destinationGroupType == GroupType::Set)
	{
		std::vector<int> setsToAddTo;
		_board.findSetsForTileWithoutSplit(_rack->getTile(rackPosition), -1, setsToAddTo);

		for (int groupId : setsToAddTo)
		{
			auto move = createRackMove(rackPosition, groupId, -1, MoveDestinationType::EndOfGroup);
			moves.push_back(move);
		}
	}
	if (moveDestinationType == MoveDestinationType::MiddleOfGroupAndSplit && destinationGroupType == GroupType::Run)
	{
		std::vector<int> runsToAddToMiddle;
		_board.findRunsForTileInMiddleWithValidSplit(_rack->getTile(rackPosition), -1, runsToAddToMiddle);

		for (int groupId : runsToAddToMiddle)
		{
			auto tile = _rack->getTile(rackPosition);

			if (tile->isJoker())
			{
				for (int targetPosition = 2; targetPosition <= _board.findGroupById(groupId)->numTiles() - 2; ++targetPosition)
				{
					auto move = createRackMove(rackPosition, groupId, targetPosition, MoveDestinationType::MiddleOfGroupAndSplit);
					moves.push_back(move);
				}
			}
			else
			{
				int targetPosition = _board.findGroupById(groupId)->getTargetPositionInRunForTile(_rack->getTile(rackPosition));

				auto move = createRackMove(rackPosition, groupId, targetPosition, MoveDestinationType::MiddleOfGroupAndSplit);
				moves.push_back(move);
			}
		}
	}
}

// See if we can add this tile to the middle of a run on the board, splitting the run into 2 runs, at least one of which is invalid.
void MovePlanner::findUnsafePlacementForRackTile(int rackPosition, MoveDestinationType moveDestinationType, GroupType destinationGroupType, std::vector<std::shared_ptr<Moove>>& moves)
{
	if (moveDestinationType == MoveDestinationType::MiddleOfGroupAndSplit && destinationGroupType == GroupType::Run)
	{
		std::vector<int> runsToAddToMiddle;
		auto tileToMove = _rack->getTile(rackPosition);
		_board.findRunsForTileInMiddleWithInvalidSplit(tileToMove, -1, runsToAddToMiddle);

		for (int groupId : runsToAddToMiddle)
		{
			auto targetGroup = _board.findGroupById(groupId);

			if (tileToMove->isJoker())
			{
				if (targetGroup->numTiles() == 2)
				{
					auto move = createRackMove(rackPosition, targetGroup->id(), 1, MoveDestinationType::MiddleOfGroupAndSplit);
					moves.push_back(move);
				}
				else if (targetGroup->numTiles() == 3 || targetGroup->numTiles() == 4)
				{
					for (int position = 1; position < targetGroup->numTiles(); ++position)
					{
						auto move = createRackMove(rackPosition, targetGroup->id(), position, MoveDestinationType::MiddleOfGroupAndSplit);
						moves.push_back(move);
					}
				}
				else
				{
					auto move1 = createRackMove(rackPosition, targetGroup->id(), 1, MoveDestinationType::MiddleOfGroupAndSplit);
					moves.push_back(move1);

					auto move2 = createRackMove(rackPosition, targetGroup->id(), targetGroup->numTiles() - 1, MoveDestinationType::MiddleOfGroupAndSplit);
					moves.push_back(move2);
				}
			}
			else
			{
				int targetPosition = _board.findGroupById(groupId)->getTargetPositionInRunForTile(tileToMove);

				auto move = createRackMove(rackPosition, groupId, targetPosition, MoveDestinationType::MiddleOfGroupAndSplit);
				moves.push_back(move);
			}
		}
	}
}

// Start a group on the board with this tile.
void MovePlanner::startNewGroupWithRackTile(int rackPosition, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto move = createRackMove(rackPosition, -1, -1, MoveDestinationType::NewGroup);
	moves.push_back(move);
}

// Find tiles from the rack that can be added to the start of an invalid run.
void MovePlanner::findRackTilesForInvalidRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	// Since we know that the target is an invalid run, it must have exactly 2 tiles, and none of them are jokers.

	auto invalidRun = _board.findGroupById(groupId);
	int runStartValue = invalidRun->getRunStartValue();
	Color runColor = invalidRun->getRunColor();

	if (runStartValue == 1)
	{
		return;
	}

	std::shared_ptr<Tile> tileToAddToStart = std::make_shared<Tile>(runStartValue - 1, runColor);
	findRackTileForTile(tileToAddToStart, groupId, MoveDestinationType::StartOfGroup, moves);

	if (!_isBeforeInitialMeld)
	{
		findRackJokerForGroup(groupId, MoveDestinationType::StartOfGroup, moves);
	}
}

// Find tiles from the rack that can be added to the end of an invalid run.
void MovePlanner::findRackTilesForInvalidRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	// Since we know that the target is an invalid run, it must have exactly 2 tiles, and none of them are jokers.

	auto invalidRun = _board.findGroupById(groupId);
	int runEndValue = invalidRun->getRunEndValue();
	Color runColor = invalidRun->getRunColor();

	if (runEndValue == 13)
	{
		return;
	}

	std::shared_ptr<Tile> tileToAddToEnd = std::make_shared<Tile>(runEndValue + 1, runColor);
	findRackTileForTile(tileToAddToEnd, groupId, MoveDestinationType::EndOfGroup, moves);

	if (!_isBeforeInitialMeld)
	{
		findRackJokerForGroup(groupId, MoveDestinationType::EndOfGroup, moves);
	}
}

// Find tiles from the rack that can be added to a group with no type.
void MovePlanner::findRackTilesForInvalidUntypedGroup(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	if (invalidGroup->numTiles() == 1)
	{
		// Case where the group is a single tile.
		auto groupTile = invalidGroup->firstTile();

		if (!groupTile->isJoker())
		{
			auto groupColor = groupTile->getColor();
			auto valueForSet = groupTile->getValue();
			int valueForStartOfRun = valueForSet - 1;
			int valueForEndOfRun = valueForSet + 1;
			std::vector<Color> colorsNotInSet;
			invalidGroup->getAllColorsNotInSet(colorsNotInSet);

			// Try to add a tile to the start, as a run.

			if (valueForStartOfRun >= 1)
			{
				std::shared_ptr<Tile> tileToAddToStart = std::make_shared<Tile>(valueForStartOfRun, groupColor);
				findRackTileForTile(tileToAddToStart, groupId, MoveDestinationType::StartOfGroup, moves);
			}

			// Try to add a tile to the end, as a run.

			if (valueForEndOfRun <= 13)
			{
				std::shared_ptr<Tile> tileToAddToEnd = std::make_shared<Tile>(valueForEndOfRun, groupColor);
				findRackTileForTile(tileToAddToEnd, groupId, MoveDestinationType::EndOfGroup, moves);
			}

			// Try to add a tile to the end, as a set.

			for (auto color : colorsNotInSet)
			{
				std::shared_ptr<Tile> tileToAddToSet = std::make_shared<Tile>(valueForSet, color);
				findRackTileForTile(tileToAddToSet, groupId, MoveDestinationType::EndOfGroup, moves);
			}

			// Add a joker.

			if (!_isBeforeInitialMeld)
			{
				findRackJokerForGroup(groupId, MoveDestinationType::StartOfGroup, moves);
				findRackJokerForGroup(groupId, MoveDestinationType::EndOfGroup, moves);
			}
		}
		else
		{
			// Group is a single joker.

			// Add any tile to group.
			findAllRackTilesForGroup(groupId, MoveDestinationType::StartOfGroup, moves);
			findAllRackTilesForGroup(groupId, MoveDestinationType::EndOfGroup, moves);

			// Add a joker.

			if (!_isBeforeInitialMeld)
			{
				findRackJokerForGroup(groupId, MoveDestinationType::EndOfGroup, moves);
			}
		}
	}
	else if (invalidGroup->numTiles() == 2)
	{
		// Group is 2 tiles, with at least one joker.

		if (invalidGroup->isOnlyTwoJokers())
		{
			// Both tiles in the group are jokers.

			findAllRackTilesForGroup(groupId, MoveDestinationType::StartOfGroup, moves);
			findAllRackTilesForGroup(groupId, MoveDestinationType::EndOfGroup, moves);
			findAllRackTilesForGroup(groupId, MoveDestinationType::MiddleOfGroup, moves);
		}
		else
		{
			Color tileColor = Color::None;
			int valueForStart = -1;
			int valueForEnd = -1;

			if (invalidGroup->firstTile()->isJoker())
			{
				tileColor = invalidGroup->lastTile()->getColor();
				int tileValue = invalidGroup->lastTile()->getValue();
				valueForStart = tileValue - 2;
				valueForEnd = tileValue + 1;
			}
			else
			{
				tileColor = invalidGroup->firstTile()->getColor();
				int tileValue = invalidGroup->firstTile()->getValue();
				valueForStart = tileValue - 1;
				valueForEnd = tileValue + 2;
			}

			findRackTileForTile(std::make_shared<Tile>(valueForStart, tileColor), groupId, MoveDestinationType::StartOfGroup, moves);
			findRackTileForTile(std::make_shared<Tile>(valueForEnd, tileColor), groupId, MoveDestinationType::EndOfGroup, moves);
			findRackTilesForInvalidSet(groupId, moves);
		}
	}

}

// Check if the board has a run or set with a tile that can be placed at the start of an invalid run, without invalidating any other groups.
void MovePlanner::findSafeBoardTilesForInvalidRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	int runStartValue = invalidGroup->getRunStartValue();
	Color runColor = invalidGroup->getRunColor();

	if (runStartValue > 1)
	{
		std::shared_ptr<Tile> tileForStart = std::make_shared<Tile>(runStartValue -1, runColor);
		findSafeBoardTilesForTile(tileForStart, groupId, MoveDestinationType::StartOfGroup, moves);
	}
}

// Check if the board has a run or set with a tile that can be placed at the end of an invalid run, without invalidating any other groups.
void MovePlanner::findSafeBoardTilesForInvalidRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	int runEndValue = invalidGroup->getRunEndValue();
	Color runColor = invalidGroup->getRunColor();

	if (runEndValue < 13)
	{
		std::shared_ptr<Tile> tileForEnd = std::make_shared<Tile>(runEndValue + 1, runColor);
		findSafeBoardTilesForTile(tileForEnd, groupId, MoveDestinationType::EndOfGroup, moves);
	}
}

// Check if the board has a run or set with a tile that can be placed at the start of an invalid run, thus invalidating the group that the tile came from.
void MovePlanner::findUnsafeBoardTilesForInvalidRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	int runStartValue = invalidGroup->getRunStartValue();
	Color runColor = invalidGroup->getRunColor();

	if (runStartValue > 1)
	{
		std::shared_ptr<Tile> tileForStart = std::make_shared<Tile>(runStartValue - 1, runColor);
		findUnsafeBoardTilesForTile(tileForStart, groupId, MoveDestinationType::StartOfGroup, moves);
	}
}

// Check if the board has a run or set with a tile that can be placed at the end of an invalid run, thus invalidating the group that the tile came from.
void MovePlanner::findUnsafeBoardTilesForInvalidRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	int runEndValue = invalidGroup->getRunEndValue();
	Color runColor = invalidGroup->getRunColor();

	if (runEndValue < 13)
	{
		std::shared_ptr<Tile> tileForEnd = std::make_shared<Tile>(runEndValue + 1, runColor);
		findUnsafeBoardTilesForTile(tileForEnd, groupId, MoveDestinationType::EndOfGroup, moves);
	}
}

void MovePlanner::findRackOrSafeTilesForInvalidRun(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	findRackTilesForInvalidRunStart(groupId, moves);
	findRackTilesForInvalidRunEnd(groupId, moves);
	findSafeBoardTilesForInvalidRunStart(groupId, moves);
	findSafeBoardTilesForInvalidRunEnd(groupId, moves);
}

// Check if the board has a run that can accept the first tile of this run as the last tile of itself.
void MovePlanner::findEndOfRunPlacementForInvalidRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	auto targetTile = invalidGroup->firstTile();

	if (targetTile->isJoker() || targetTile->getValue() > 3)
	{
		findSafePlacementForTile(targetTile, groupId, MoveSourceType::StartOfGroup, -1, MoveDestinationType::EndOfGroup, GroupType::Run, moves);
	}
}

// Check if the board has a run that can accept the last tile of this run as the first tile of itself.
void MovePlanner::findStartOfRunPlacementForInvalidRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	auto targetTile = invalidGroup->lastTile();

	if (targetTile->isJoker() || targetTile->getValue() < 11)
	{
		findSafePlacementForTile(targetTile, groupId, MoveSourceType::EndOfGroup, -1, MoveDestinationType::StartOfGroup, GroupType::Run, moves);
	}
}

// Check if the board has a run that can accept the first tile of this run as the first tile of itself.
void MovePlanner::findStartOfRunPlacementForInvalidRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	auto targetTile = invalidGroup->firstTile();

	findSafePlacementForTile(targetTile, groupId, MoveSourceType::StartOfGroup, -1, MoveDestinationType::StartOfGroup, GroupType::Run, moves);
}

// Check if the board has a run that can accept the last tile of this run as the last tile of itself.
void MovePlanner::findEndOfRunPlacementForInvalidRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	auto targetTile = invalidGroup->lastTile();

	findSafePlacementForTile(targetTile, groupId, MoveSourceType::EndOfGroup, -1, MoveDestinationType::EndOfGroup, GroupType::Run, moves);
}

// Check if the board has a set that can accept the first tile of this run without splitting the set.
void MovePlanner::findSetWithoutSplitPlacementForInvalidRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	auto targetTile = invalidGroup->firstTile();

	findSafePlacementForTile(targetTile, groupId, MoveSourceType::StartOfGroup, -1, MoveDestinationType::EndOfGroup, GroupType::Set, moves);
}

// Check if the board has a set that can accept the last tile of this run without splitting the set.
void MovePlanner::findSetWithoutSplitPlacementForInvalidRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	auto targetTile = invalidGroup->lastTile();

	findSafePlacementForTile(targetTile, groupId, MoveSourceType::EndOfGroup, -1, MoveDestinationType::EndOfGroup, GroupType::Set, moves);
}

// Check if the board has a run that can accept the first tile of this run in the middle of itself without creating invalid groups.
void MovePlanner::findSafeMiddleOfRunPlacementForInvalidRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	auto targetTile = invalidGroup->firstTile();

	findSafePlacementForTile(targetTile, groupId, MoveSourceType::StartOfGroup, -1, MoveDestinationType::MiddleOfGroupAndSplit, GroupType::Run, moves);
}

// Check if the board has a run that can accept the last tile of this run in the middle of itself without creating invalid groups.
void MovePlanner::findSafeMiddleOfRunPlacementForInvalidRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	auto targetTile = invalidGroup->lastTile();

	findSafePlacementForTile(targetTile, groupId, MoveSourceType::EndOfGroup, -1, MoveDestinationType::MiddleOfGroupAndSplit, GroupType::Run, moves);
}

// Check if the board has a run that can accept the first tile of this run in the middle of itself thus creating at least 1 invalid group.
void MovePlanner::findUnsafeMiddleOfRunPlacementForInvalidRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	auto targetTile = invalidGroup->firstTile();

	findUnsafePlacementForTile(targetTile, groupId, MoveSourceType::StartOfGroup, -1, MoveDestinationType::MiddleOfGroupAndSplit, GroupType::Run, moves);
}

// Check if the board has a run that can accept the first tile of this run in the middle of itself thus creating at least 1 invalid group.
void MovePlanner::findUnsafeMiddleOfRunPlacementForInvalidRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	auto targetTile = invalidGroup->lastTile();

	findUnsafePlacementForTile(targetTile, groupId, MoveSourceType::EndOfGroup, -1, MoveDestinationType::MiddleOfGroupAndSplit, GroupType::Run, moves);
}

void MovePlanner::findSafePlacementForInvalidRun(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto group = _board.findGroupById(groupId);

	if (!group->firstTile()->isJoker())
	{
		findEndOfRunPlacementForInvalidRunStart(groupId, moves);
	}
	if (!group->lastTile()->isJoker())
	{
		findStartOfRunPlacementForInvalidRunEnd(groupId, moves);
	}
	if (!group->firstTile()->isJoker())
	{
		findStartOfRunPlacementForInvalidRunStart(groupId, moves);
	}
	if (!group->lastTile()->isJoker())
	{
		findEndOfRunPlacementForInvalidRunEnd(groupId, moves);
	}
	if (!group->firstTile()->isJoker())
	{
		findSetWithoutSplitPlacementForInvalidRunStart(groupId, moves);
	}
	if (!group->lastTile()->isJoker())
	{
		findSetWithoutSplitPlacementForInvalidRunEnd(groupId, moves);
	}
	if (!group->firstTile()->isJoker())
	{
		findSafeMiddleOfRunPlacementForInvalidRunStart(groupId, moves);
	}
	if (!group->lastTile()->isJoker())
	{
		findSafeMiddleOfRunPlacementForInvalidRunEnd(groupId, moves);
	}
}

// Check if the rack has a tile that can be added to an invalid set.
void MovePlanner::findRackTilesForInvalidSet(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	// Since we know that the target is an invalid set, it must have exactly 2 tiles, and none of them are jokers.

	auto invalidSet = _board.findGroupById(groupId);
	int setValue = invalidSet->getSetValue();
	std::vector<Color> availableColors;
	invalidSet->getAllColorsNotInSet(availableColors);

	for (auto color : availableColors)
	{
		std::shared_ptr<Tile> tileToAddToSet = std::make_shared<Tile>(setValue, color);
		findRackTileForTile(tileToAddToSet, groupId, MoveDestinationType::EndOfGroup, moves);
	}

	if (!_isBeforeInitialMeld)
	{
		findRackJokerForGroup(groupId, MoveDestinationType::EndOfGroup, moves);
	}
}

// Check if the board has any groups that can safely lose a tile that can be added to an invalid set.
void MovePlanner::findSafeBoardTilesForInvalidSet(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	std::vector<Color> availableColors;
	invalidGroup->getAllColorsNotInSet(availableColors);
	int setValue = invalidGroup->getSetValue();

	for (auto color : availableColors)
	{
		auto targetTile = std::make_shared<Tile>(setValue, color);
		findSafeBoardTilesForTile(targetTile, groupId, MoveDestinationType::EndOfGroup, moves);
	}
}

// Check if the board has any groups that can unsafely lose a tile that can be added to an invalid set.
void MovePlanner::findUnsafeBoardTilesForInvalidSet(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	std::vector<Color> availableColors;
	invalidGroup->getAllColorsNotInSet(availableColors);
	int setValue = invalidGroup->getSetValue();

	for (auto color : availableColors)
	{
		auto targetTile = std::make_shared<Tile>(setValue, color);
		findUnsafeBoardTilesForTile(targetTile, groupId, MoveDestinationType::EndOfGroup, moves);
	}
}

void MovePlanner::findRackOrSafeTilesForInvalidSet(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	findRackTilesForInvalidSet(groupId, moves);
	findSafeBoardTilesForInvalidSet(groupId, moves);
}

// Find any sets or runs on the board that can accept either of the tiles of this group.
void MovePlanner::findSafePlacementForInvalidSet(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	for (int posInInvalidSet = 0; posInInvalidSet < invalidGroup->numTiles(); ++posInInvalidSet)
	{
		auto tileToMove = invalidGroup->getTile(posInInvalidSet);
		findSafePlacementForTile(tileToMove, groupId, MoveSourceType::MiddleOfGroup, posInInvalidSet, MoveDestinationType::Any, GroupType::None, moves);
	}
}

// Find any sets or runs on the board that can accept either of the tiles of this group, but will create an invalid group or two.
void MovePlanner::findUnsafePlacementForInvalidSet(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);

	for (int posInInvalidSet = 0; posInInvalidSet < invalidGroup->numTiles(); ++posInInvalidSet)
	{
		auto tileToMove = invalidGroup->getTile(posInInvalidSet);
		findUnsafePlacementForTile(tileToMove, groupId, MoveSourceType::MiddleOfGroup, posInInvalidSet, MoveDestinationType::MiddleOfGroupAndSplit, GroupType::Run, moves);
	}
}

void MovePlanner::findSafeBoardTilesForInvalidUntypedGroupAsRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);
	std::shared_ptr<Tile> targetTile;

	if (invalidGroup->isValid())
	{
		throw std::runtime_error("Tried to find tile as run start for invalid group but the group is valid");
	}
	if (invalidGroup->numTiles() == 0)
	{
		throw std::runtime_error("Tried to find tile as run start for empty group");
	}
	if (invalidGroup->isOnlyJoker() || invalidGroup->isOnlyTwoJokers())
	{
		throw std::runtime_error("Tried to find tile as run start for group that is only jokers");
	}

	// Calculate the tile needed.
	if (invalidGroup->firstTile()->isJoker())
	{
		if (invalidGroup->lastTile()->getValue() < 3)
		{
			return;
		}
		targetTile = std::make_shared<Tile>(invalidGroup->lastTile()->getValue() - 2, invalidGroup->lastTile()->getColor());
	}
	else
	{
		if (invalidGroup->firstTile()->getValue() < 2)
		{
			return;
		}
		targetTile = std::make_shared<Tile>(invalidGroup->firstTile()->getValue() - 1, invalidGroup->firstTile()->getColor());
	}

	findSafeBoardTilesForTile(targetTile, groupId, MoveDestinationType::StartOfGroup, moves);
}

void MovePlanner::findSafeBoardTilesForInvalidUntypedGroupAsRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);
	std::shared_ptr<Tile> targetTile;

	if (invalidGroup->isValid())
	{
		throw std::runtime_error("Tried to find tile as run end for invalid group but the group is valid");
	}
	if (invalidGroup->numTiles() == 0)
	{
		throw std::runtime_error("Tried to find tile as run end for empty group");
	}
	if (invalidGroup->isOnlyJoker() || invalidGroup->isOnlyTwoJokers())
	{
		throw std::runtime_error("Tried to find tile as run end for group that is only jokers");
	}

	// Calculate the tile needed.
	if (invalidGroup->lastTile()->isJoker())
	{
		if (invalidGroup->firstTile()->getValue() > 11)
		{
			return;
		}
		targetTile = std::make_shared<Tile>(invalidGroup->firstTile()->getValue() + 2, invalidGroup->firstTile()->getColor());
	}
	else
	{
		if (invalidGroup->lastTile()->getValue() > 12)
		{
			return;
		}
		targetTile = std::make_shared<Tile>(invalidGroup->lastTile()->getValue() + 1, invalidGroup->lastTile()->getColor());
	}

	findSafeBoardTilesForTile(targetTile, groupId, MoveDestinationType::EndOfGroup, moves);
}

void MovePlanner::findSafeBoardTilesForInvalidUntypedGroupAsSet(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);
	if (invalidGroup->isValid())
	{
		throw std::runtime_error("Tried to find tile as set for invalid group but the group is valid");
	}
	if (invalidGroup->numTiles() == 0)
	{
		throw std::runtime_error("Tried to find tile as set for empty group");
	}
	if (invalidGroup->isOnlyJoker() || invalidGroup->isOnlyTwoJokers())
	{
		throw std::runtime_error("Tried to find tile as set for group that is only jokers");
	}

	findSafeBoardTilesForInvalidSet(groupId, moves);
}

void MovePlanner::findUnsafeBoardTilesForInvalidUntypedGroupAsRunStart(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);
	std::shared_ptr<Tile> targetTile;

	if (invalidGroup->isValid())
	{
		throw std::runtime_error("Tried to find tile as run start for invalid group but the group is valid");
	}
	if (invalidGroup->numTiles() == 0)
	{
		throw std::runtime_error("Tried to find tile as run start for empty group");
	}
	if (invalidGroup->isOnlyJoker() || invalidGroup->isOnlyTwoJokers())
	{
		throw std::runtime_error("Tried to find tile as run start for group that is only jokers");
	}

	// Calculate the tile needed.
	if (invalidGroup->firstTile()->isJoker())
	{
		if (invalidGroup->lastTile()->getValue() < 3)
		{
			return;
		}
		targetTile = std::make_shared<Tile>(invalidGroup->lastTile()->getValue() - 2, invalidGroup->lastTile()->getColor());
	}
	else
	{
		if (invalidGroup->firstTile()->getValue() < 2)
		{
			return;
		}
		targetTile = std::make_shared<Tile>(invalidGroup->firstTile()->getValue() - 1, invalidGroup->firstTile()->getColor());
	}

	findUnsafeBoardTilesForTile(targetTile, groupId, MoveDestinationType::StartOfGroup, moves);
}

void MovePlanner::findUnsafeBoardTilesForInvalidUntypedGroupAsRunEnd(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);
	std::shared_ptr<Tile> targetTile;

	if (invalidGroup->isValid())
	{
		throw std::runtime_error("Tried to find tile as run end for invalid group but the group is valid");
	}
	if (invalidGroup->numTiles() == 0)
	{
		throw std::runtime_error("Tried to find tile as run end for empty group");
	}
	if (invalidGroup->isOnlyJoker() || invalidGroup->isOnlyTwoJokers())
	{
		throw std::runtime_error("Tried to find tile as run end for group that is only jokers");
	}

	// Calculate the tile needed.
	if (invalidGroup->lastTile()->isJoker())
	{
		if (invalidGroup->firstTile()->getValue() > 11)
		{
			return;
		}
		targetTile = std::make_shared<Tile>(invalidGroup->firstTile()->getValue() + 2, invalidGroup->firstTile()->getColor());
	}
	else
	{
		if (invalidGroup->lastTile()->getValue() > 12)
		{
			return;
		}
		targetTile = std::make_shared<Tile>(invalidGroup->lastTile()->getValue() + 1, invalidGroup->lastTile()->getColor());
	}

	findUnsafeBoardTilesForTile(targetTile, groupId, MoveDestinationType::EndOfGroup, moves);
}

void MovePlanner::findUnsafeBoardTilesForInvalidUntypedGroupAsSet(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto invalidGroup = _board.findGroupById(groupId);
	if (invalidGroup->isValid())
	{
		throw std::runtime_error("Tried to find tile as set for invalid group but the group is valid");
	}
	if (invalidGroup->numTiles() == 0)
	{
		throw std::runtime_error("Tried to find tile as set for empty group");
	}
	if (invalidGroup->isOnlyJoker() || invalidGroup->isOnlyTwoJokers())
	{
		throw std::runtime_error("Tried to find tile as set for group that is only jokers");
	}

	findUnsafeBoardTilesForInvalidSet(groupId, moves);
}

void MovePlanner::findUnsafeTilesForInvalidRun(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	findUnsafeBoardTilesForInvalidRunStart(groupId, moves);
	findUnsafeBoardTilesForInvalidRunEnd(groupId, moves);
}

void MovePlanner::findUnsafePlacementForInvalidRun(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	findUnsafeMiddleOfRunPlacementForInvalidRunStart(groupId, moves);
	findUnsafeMiddleOfRunPlacementForInvalidRunStart(groupId, moves);
}

void MovePlanner::getSafeMovesToFixInvalidGroup(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto group = _board.findGroupById(groupId);

	if (group->numTiles() == 2)
	{
		if (group->isRun())
		{
			if (group->tryMovingToOtherGroups())
			{
				findSafePlacementForInvalidRun(groupId, moves);
			}
			findRackOrSafeTilesForInvalidRun(groupId, moves);
		}
		else if (group->isSet())
		{
			if (group->tryMovingToOtherGroups())
			{
				findSafePlacementForInvalidSet(groupId, moves);
			}
			findRackOrSafeTilesForInvalidSet(groupId, moves);
		}
		else
		{
			if (group->tryMovingToOtherGroups())
			{
				if (!group->firstTile()->isJoker())
				{
					findSafePlacementForTile(group->firstTile(), groupId, MoveSourceType::StartOfGroup, -1, MoveDestinationType::Any, GroupType::None, moves);
				}
				if (!group->lastTile()->isJoker())
				{
					findSafePlacementForTile(group->lastTile(), groupId, MoveSourceType::EndOfGroup, -1, MoveDestinationType::Any, GroupType::None, moves);
				}
			}
			findRackTilesForInvalidUntypedGroup(groupId, moves);
			if (!group->firstTile()->isJoker() || !group->lastTile()->isJoker())
			{
				findSafeBoardTilesForInvalidUntypedGroupAsRunStart(groupId, moves);
				findSafeBoardTilesForInvalidUntypedGroupAsRunEnd(groupId, moves);
				findSafeBoardTilesForInvalidUntypedGroupAsSet(groupId, moves);
			}
		}
	}
	else
	{
		if (!group->firstTile()->isJoker())
		{
			if (group->tryMovingToOtherGroups())
			{
				findSafePlacementForInvalidSet(groupId, moves);
			}
		}
		findRackTilesForInvalidUntypedGroup(groupId, moves);
		if (!group->firstTile()->isJoker())
		{
			findSafeBoardTilesForInvalidUntypedGroupAsRunStart(groupId, moves);
			findSafeBoardTilesForInvalidUntypedGroupAsRunEnd(groupId, moves);
			findSafeBoardTilesForInvalidUntypedGroupAsSet(groupId, moves);
		}
	}
}

void MovePlanner::getUnsafeMovesToFixInvalidGroup(int groupId, std::vector<std::shared_ptr<Moove>>& moves)
{
	auto group = _board.findGroupById(groupId);

	if (group->numTiles() == 2)
	{
		if (group->isRun())
		{
			if (group->tryMovingToOtherGroups())
			{
				findUnsafePlacementForInvalidRun(groupId, moves);
			}
			findUnsafeTilesForInvalidRun(groupId, moves);
		}
		else if (group->isSet())
		{
			if (group->tryMovingToOtherGroups())
			{
				findUnsafePlacementForInvalidSet(groupId, moves);
			}
			findUnsafeBoardTilesForInvalidSet(groupId, moves);
		}
		else
		{
			if (group->tryMovingToOtherGroups())
			{
				if (!group->firstTile()->isJoker())
				{
					findUnsafePlacementForTile(group->firstTile(), groupId, MoveSourceType::StartOfGroup, -1, MoveDestinationType::Any, GroupType::None, moves);
				}
				if (!group->lastTile()->isJoker())
				{
					findUnsafePlacementForTile(group->lastTile(), groupId, MoveSourceType::EndOfGroup, -1, MoveDestinationType::Any, GroupType::None, moves);
				}
			}
			if (!group->firstTile()->isJoker() || !group->lastTile()->isJoker())
			{
				findUnsafeBoardTilesForInvalidUntypedGroupAsRunStart(groupId, moves);
				findUnsafeBoardTilesForInvalidUntypedGroupAsRunEnd(groupId, moves);
				findUnsafeBoardTilesForInvalidUntypedGroupAsSet(groupId, moves);
			}
		}
	}
	else
	{
		if (!group->firstTile()->isJoker())
		{
			if (group->tryMovingToOtherGroups())
			{
				findUnsafePlacementForInvalidSet(groupId, moves);
			}
		}
		if (!group->firstTile()->isJoker())
		{
			findUnsafeBoardTilesForInvalidUntypedGroupAsRunStart(groupId, moves);
			findUnsafeBoardTilesForInvalidUntypedGroupAsRunEnd(groupId, moves);
			findUnsafeBoardTilesForInvalidUntypedGroupAsSet(groupId, moves);
		}
	}
}

void MovePlanner::getSafeMovesFromRack(std::vector<std::shared_ptr<Moove>>& moves)
{
	int nextFreePosition = _rack->firstTilePositionNotInUse(0);

	while (nextFreePosition != -1)
	{
		findSafePlacementForRackTile(nextFreePosition, MoveDestinationType::StartOfGroup, GroupType::Run, moves);
		findSafePlacementForRackTile(nextFreePosition, MoveDestinationType::EndOfGroup, GroupType::Run, moves);
		findSafePlacementForRackTile(nextFreePosition, MoveDestinationType::EndOfGroup, GroupType::Set, moves);
		findSafePlacementForRackTile(nextFreePosition, MoveDestinationType::MiddleOfGroupAndSplit, GroupType::Run, moves);

		if (!_rack->getTile(nextFreePosition)->isJoker())
		{
			startNewGroupWithRackTile(nextFreePosition, moves);
		}

		nextFreePosition = _rack->firstTilePositionNotInUse(nextFreePosition + 1);
	}
}

void MovePlanner::getUnsafeMovesFromRack(std::vector<std::shared_ptr<Moove>>& moves)
{
	int nextFreePosition = _rack->firstTilePositionNotInUse(0);

	while (nextFreePosition != -1)
	{
		if (!_rack->getTile(nextFreePosition)->isJoker())
		{
			findUnsafePlacementForRackTile(nextFreePosition, MoveDestinationType::MiddleOfGroupAndSplit, GroupType::Run, moves);
		}

		nextFreePosition = _rack->firstTilePositionNotInUse(nextFreePosition + 1);
	}
}


} // namespace Tiles



