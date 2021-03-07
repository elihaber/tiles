/*
 * mover.cpp
 *
 *  Created on: Sep 19, 2017
 *      Author: eliha
 */
#include "stdafx.h"
#include "enums.h"
#include "defs.h"
#include "mover.h"
//#include "move.h"
#include "rack.h"
#include "board.h"

namespace Tiles
{

void Mover::debugDoMove(Moove& move)
{
	std::cout << "Doing move: " << move.str() << std::endl;
}

void Mover::debugUndoMove(Moove& move)
{
	std::cout << "Undoing move: " << move.str() << std::endl;
}

void Mover::doMove(Moove& move)
{
	if (move._source == Location::Rack)
	{
		if (move._moveDestinationType == MoveDestinationType::StartOfGroup)
		{
			auto tile = _rack.takeTile(move._sourceRackPosition);
			_board.addTileToStartOfGroup(tile, _board.findGroupById(move._destinationGroupId), false);
		}
		else if (move._moveDestinationType == MoveDestinationType::EndOfGroup)
		{
			auto tile = _rack.takeTile(move._sourceRackPosition);
			_board.addTileToEndOfGroup(tile, _board.findGroupById(move._destinationGroupId), false);
		}
		else if (move._moveDestinationType == MoveDestinationType::MiddleOfGroup)
		{
			auto tile = _rack.takeTile(move._sourceRackPosition);
			_board.addTileToMiddleOfGroup(tile, _board.findGroupById(move._destinationGroupId), move._destinationPositionInGroup, false);
		}
		else if (move._moveDestinationType == MoveDestinationType::MiddleOfGroupAndSplit)
		{
			GroupSplitInfo groupSplitInfo;
			groupSplitInfo.originalGroupId = move._destinationGroupId;

			auto tile = _rack.takeTile(move._sourceRackPosition);
			_board.addTileToMiddleOfGroupAndSplit(tile, _board.findGroupById(move._destinationGroupId), move._destinationPositionInGroup, groupSplitInfo, false);

			move._moveGroupSummary.originalGroupTarget = move._destinationGroupId;
			move._moveGroupSummary.groupCreatedFirstFromSplitTarget = groupSplitInfo.createdGroupIds[0];
			move._moveGroupSummary.groupCreatedSecondFromSplitTarget = groupSplitInfo.createdGroupIds[1];
		}
		else if (move._moveDestinationType == MoveDestinationType::NewGroup)
		{
			auto tile = _rack.takeTile(move._sourceRackPosition);
			int newGroupId = _board.createNewGroup(tile);
			move._moveGroupSummary.groupCreatedFromTile = newGroupId;
		}
		else
		{
			throw std::runtime_error("Unknown move destination type");
		}
	}
	else
	{ // Move source is the board
		if (move._moveDestinationType == MoveDestinationType::StartOfGroup)
		{
			if (move._moveSourceType == MoveSourceType::StartOfGroup)
			{
				_board.addTileToStartOfGroupFromStartOfGroup(move._sourceGroupId, move._destinationGroupId, false);
			}
			else if (move._moveSourceType == MoveSourceType::EndOfGroup)
			{
				_board.addTileToStartOfGroupFromEndOfGroup(move._sourceGroupId, move._destinationGroupId, false);
			}
			else if (move._moveSourceType == MoveSourceType::MiddleOfGroup)
			{
				_board.addTileToStartOfGroupFromMiddleOfGroup(move._sourceGroupId, move._sourcePositionInGroup, move._destinationGroupId, false);
			}
			else if (move._moveSourceType == MoveSourceType::MiddleOfGroupAndSplit)
			{
				GroupSplitInfo groupSplitInfo;
				groupSplitInfo.originalGroupId = move._sourceGroupId;

				_board.addTileToStartOfGroupFromMiddleOfGroupAndSplit(move._sourceGroupId, move._sourcePositionInGroup, move._destinationGroupId, groupSplitInfo, false);

				move._moveGroupSummary.originalGroupSource = move._sourceGroupId;
				move._moveGroupSummary.groupCreatedFirstFromSplitSource = groupSplitInfo.createdGroupIds[0];
				move._moveGroupSummary.groupCreatedSecondFromSplitSource = groupSplitInfo.createdGroupIds[1];
			}
			else
			{
				throw std::runtime_error("Move source type is not supported for board moves");
			}
		}
		else if (move._moveDestinationType == MoveDestinationType::EndOfGroup)
		{
			if (move._moveSourceType == MoveSourceType::StartOfGroup)
			{
				_board.addTileToEndOfGroupFromStartOfGroup(move._sourceGroupId, move._destinationGroupId, false);
			}
			else if (move._moveSourceType == MoveSourceType::EndOfGroup)
			{
				_board.addTileToEndOfGroupFromEndOfGroup(move._sourceGroupId, move._destinationGroupId, false);
			}
			else if (move._moveSourceType == MoveSourceType::MiddleOfGroup)
			{
				_board.addTileToEndOfGroupFromMiddleOfGroup(move._sourceGroupId, move._sourcePositionInGroup, move._destinationGroupId, false);
			}
			else if (move._moveSourceType == MoveSourceType::MiddleOfGroupAndSplit)
			{
				GroupSplitInfo groupSplitInfo;
				groupSplitInfo.originalGroupId = move._sourceGroupId;

				_board.addTileToEndOfGroupFromMiddleOfGroupAndSplit(move._sourceGroupId, move._sourcePositionInGroup, move._destinationGroupId, groupSplitInfo, false);

				move._moveGroupSummary.originalGroupSource = move._sourceGroupId;
				move._moveGroupSummary.groupCreatedFirstFromSplitSource = groupSplitInfo.createdGroupIds[0];
				move._moveGroupSummary.groupCreatedSecondFromSplitSource = groupSplitInfo.createdGroupIds[1];
			}
			else
			{
				throw std::runtime_error("Move source type is not supported for board moves");
			}
		}
		else if (move._moveDestinationType == MoveDestinationType::MiddleOfGroup)
		{
			if (move._moveSourceType == MoveSourceType::StartOfGroup)
			{
				_board.addTileToMiddleOfGroupFromStartOfGroup(move._sourceGroupId, move._destinationGroupId, move._destinationPositionInGroup, false);
			}
			else if (move._moveSourceType == MoveSourceType::EndOfGroup)
			{
				_board.addTileToMiddleOfGroupFromEndOfGroup(move._sourceGroupId, move._destinationGroupId, move._destinationPositionInGroup, false);
			}
			else if (move._moveSourceType == MoveSourceType::MiddleOfGroup)
			{
				_board.addTileToMiddleOfGroupFromMiddleOfGroup(move._sourceGroupId, move._sourcePositionInGroup, move._destinationGroupId, move._destinationPositionInGroup, false);
			}
			else if (move._moveSourceType == MoveSourceType::MiddleOfGroupAndSplit)
			{
				GroupSplitInfo groupSplitInfo;
				groupSplitInfo.originalGroupId = move._sourceGroupId;

				_board.addTileToMiddleOfGroupFromMiddleOfGroupAndSplit(move._sourceGroupId, move._sourcePositionInGroup, move._destinationGroupId, move._destinationPositionInGroup, groupSplitInfo, false);

				move._moveGroupSummary.originalGroupSource = move._sourceGroupId;
				move._moveGroupSummary.groupCreatedFirstFromSplitSource = groupSplitInfo.createdGroupIds[0];
				move._moveGroupSummary.groupCreatedSecondFromSplitSource = groupSplitInfo.createdGroupIds[1];
			}
			else
			{
				throw std::runtime_error("Move source type is not supported for board moves");
			}
		}
		else if (move._moveDestinationType == MoveDestinationType::MiddleOfGroupAndSplit)
		{
			if (move._moveSourceType == MoveSourceType::StartOfGroup)
			{
				GroupSplitInfo groupSplitInfo;
				groupSplitInfo.originalGroupId = move._destinationGroupId;

				_board.addTileToMiddleOfGroupAndSplitFromStartOfGroup(move._sourceGroupId, move._destinationGroupId, groupSplitInfo, move._destinationPositionInGroup, false);

				move._moveGroupSummary.originalGroupTarget = move._destinationGroupId;
				move._moveGroupSummary.groupCreatedFirstFromSplitTarget = groupSplitInfo.createdGroupIds[0];
				move._moveGroupSummary.groupCreatedSecondFromSplitTarget = groupSplitInfo.createdGroupIds[1];
			}
			else if (move._moveSourceType == MoveSourceType::EndOfGroup)
			{
				GroupSplitInfo groupSplitInfo;
				groupSplitInfo.originalGroupId = move._destinationGroupId;

				_board.addTileToMiddleOfGroupAndSplitFromEndOfGroup(move._sourceGroupId, move._destinationGroupId, groupSplitInfo, move._destinationPositionInGroup, false);

				move._moveGroupSummary.originalGroupTarget = move._destinationGroupId;
				move._moveGroupSummary.groupCreatedFirstFromSplitTarget = groupSplitInfo.createdGroupIds[0];
				move._moveGroupSummary.groupCreatedSecondFromSplitTarget = groupSplitInfo.createdGroupIds[1];
			}
			else if (move._moveSourceType == MoveSourceType::MiddleOfGroup)
			{
				GroupSplitInfo groupSplitInfo;
				groupSplitInfo.originalGroupId = move._destinationGroupId;

				_board.addTileToMiddleOfGroupAndSplitFromMiddleOfGroup(move._sourceGroupId, move._sourcePositionInGroup, move._destinationGroupId, move._destinationPositionInGroup, groupSplitInfo, false);

				move._moveGroupSummary.originalGroupTarget = move._destinationGroupId;
				move._moveGroupSummary.groupCreatedFirstFromSplitTarget = groupSplitInfo.createdGroupIds[0];
				move._moveGroupSummary.groupCreatedSecondFromSplitTarget = groupSplitInfo.createdGroupIds[1];
			}
			else if (move._moveSourceType == MoveSourceType::MiddleOfGroupAndSplit)
			{
				GroupSplitInfo groupSplitInfoSource;
				groupSplitInfoSource.originalGroupId = move._sourceGroupId;

				GroupSplitInfo groupSplitInfoDestination;
				groupSplitInfoSource.originalGroupId = move._destinationGroupId;

				_board.addTileToMiddleOfGroupAndSplitFromMiddleOfGroupAndSplit(move._sourceGroupId, move._sourcePositionInGroup, move._destinationGroupId, move._destinationPositionInGroup, groupSplitInfoSource, groupSplitInfoDestination, false);

				move._moveGroupSummary.originalGroupSource = move._sourceGroupId;
				move._moveGroupSummary.groupCreatedFirstFromSplitSource = groupSplitInfoSource.createdGroupIds[0];
				move._moveGroupSummary.groupCreatedSecondFromSplitSource = groupSplitInfoSource.createdGroupIds[1];

				move._moveGroupSummary.originalGroupTarget = move._destinationGroupId;
				move._moveGroupSummary.groupCreatedFirstFromSplitTarget = groupSplitInfoDestination.createdGroupIds[0];
				move._moveGroupSummary.groupCreatedSecondFromSplitTarget = groupSplitInfoDestination.createdGroupIds[1];
			}
			else
			{
				throw std::runtime_error("Move source type is not supported for board moves");
			}
		}
		else
		{
			throw std::runtime_error("Move destination type is not supported for board moves");
		}

	}

	if (_debug)
	{
		debugDoMove(move);
	}
}

void Mover::undoMove(Moove& move)
{
	if (move._source == Location::Rack)
	{
		if (move._moveDestinationType == MoveDestinationType::StartOfGroup)
		{
			_board.removeTileFromStartOfGroup(_board.findGroupById(move._destinationGroupId), true);
			_rack.returnTile(move._sourceRackPosition);
		}
		else if (move._moveDestinationType == MoveDestinationType::EndOfGroup)
		{
			_board.removeTileFromEndOfGroup(_board.findGroupById(move._destinationGroupId), true);
			_rack.returnTile(move._sourceRackPosition);
		}
		else if (move._moveDestinationType == MoveDestinationType::MiddleOfGroup)
		{
			_board.removeTileFromMiddleOfGroup(_board.findGroupById(move._destinationGroupId), move._destinationPositionInGroup, true);
			_rack.returnTile(move._sourceRackPosition);
		}
		else if (move._moveDestinationType == MoveDestinationType::MiddleOfGroupAndSplit)
		{
			GroupSplitInfo groupSplitInfo;
			groupSplitInfo.originalGroupId = move._moveGroupSummary.originalGroupTarget;
			groupSplitInfo.createdGroupIds[0] = move._moveGroupSummary.groupCreatedFirstFromSplitTarget;
			groupSplitInfo.createdGroupIds[1] = move._moveGroupSummary.groupCreatedSecondFromSplitTarget;

			_board.reconstituteSplitGroupWithoutLastTileFromFirstGroup(groupSplitInfo, true);
			_rack.returnTile(move._sourceRackPosition);
		}
		else if (move._moveDestinationType == MoveDestinationType::NewGroup)
		{
			_board.removeOnlyTileOfGroup(_board.findGroupById(move._moveGroupSummary.groupCreatedFromTile), true);
			_rack.returnTile(move._sourceRackPosition);
		}
		else
		{
			throw std::runtime_error("Move destination type is not supported for rack moves");
		}
	}
	else
	{ // Move source is the board
		if (move._moveDestinationType == MoveDestinationType::StartOfGroup)
		{
			if (move._moveSourceType == MoveSourceType::StartOfGroup)
			{
				_board.revertAddTileToStartOfGroupFromStartOfGroup(move._sourceGroupId, move._destinationGroupId);
			}
			else if (move._moveSourceType == MoveSourceType::EndOfGroup)
			{
				_board.revertAddTileToStartOfGroupFromEndOfGroup(move._sourceGroupId, move._destinationGroupId);
			}
			else if (move._moveSourceType == MoveSourceType::MiddleOfGroup)
			{
				_board.revertAddTileToStartOfGroupFromMiddleOfGroup(move._sourceGroupId, move._sourcePositionInGroup, move._destinationGroupId);
			}
			else if (move._moveSourceType == MoveSourceType::MiddleOfGroupAndSplit)
			{
				GroupSplitInfo groupSplitInfo;
				groupSplitInfo.originalGroupId = move._moveGroupSummary.originalGroupSource;
				groupSplitInfo.createdGroupIds[0] = move._moveGroupSummary.groupCreatedFirstFromSplitSource;
				groupSplitInfo.createdGroupIds[1] = move._moveGroupSummary.groupCreatedSecondFromSplitSource;

				_board.revertAddTileToStartOfGroupFromMiddleOfGroupAndSplit(move._destinationGroupId, groupSplitInfo);
			}
			else
			{
				throw std::runtime_error("Move source type is not supported for board moves");
			}
		}
		else if (move._moveDestinationType == MoveDestinationType::EndOfGroup)
		{
			if (move._moveSourceType == MoveSourceType::StartOfGroup)
			{
				_board.revertAddTileToEndOfGroupFromStartOfGroup(move._sourceGroupId, move._destinationGroupId);
			}
			else if (move._moveSourceType == MoveSourceType::EndOfGroup)
			{
				_board.revertAddTileToEndOfGroupFromEndOfGroup(move._sourceGroupId, move._destinationGroupId);
			}
			else if (move._moveSourceType == MoveSourceType::MiddleOfGroup)
			{
				_board.revertAddTileToEndOfGroupFromMiddleOfGroup(move._sourceGroupId, move._sourcePositionInGroup, move._destinationGroupId);
			}
			else if (move._moveSourceType == MoveSourceType::MiddleOfGroupAndSplit)
			{
				GroupSplitInfo groupSplitInfo;
				groupSplitInfo.originalGroupId = move._moveGroupSummary.originalGroupSource;
				groupSplitInfo.createdGroupIds[0] = move._moveGroupSummary.groupCreatedFirstFromSplitSource;
				groupSplitInfo.createdGroupIds[1] = move._moveGroupSummary.groupCreatedSecondFromSplitSource;

				_board.revertAddTileToEndOfGroupFromMiddleOfGroupAndSplit(move._destinationGroupId, groupSplitInfo);
			}
			else
			{
				throw std::runtime_error("Move source type is not supported for board moves");
			}
		}
		else if (move._moveDestinationType == MoveDestinationType::MiddleOfGroup)
		{
			if (move._moveSourceType == MoveSourceType::StartOfGroup)
			{
				_board.revertAddTileToMiddleOfGroupFromStartOfGroup(move._sourceGroupId, move._destinationGroupId, move._destinationPositionInGroup);
			}
			else if (move._moveSourceType == MoveSourceType::EndOfGroup)
			{
				_board.revertAddTileToMiddleOfGroupFromEndOfGroup(move._sourceGroupId, move._destinationGroupId, move._destinationPositionInGroup);
			}
			else if (move._moveSourceType == MoveSourceType::MiddleOfGroup)
			{
				_board.revertAddTileToMiddleOfGroupFromMiddleOfGroup(move._sourceGroupId, move._sourcePositionInGroup, move._destinationGroupId, move._destinationPositionInGroup);
			}
			else if (move._moveSourceType == MoveSourceType::MiddleOfGroupAndSplit)
			{
				GroupSplitInfo groupSplitInfo;
				groupSplitInfo.originalGroupId = move._moveGroupSummary.originalGroupSource;
				groupSplitInfo.createdGroupIds[0] = move._moveGroupSummary.groupCreatedFirstFromSplitSource;
				groupSplitInfo.createdGroupIds[1] = move._moveGroupSummary.groupCreatedSecondFromSplitSource;

				_board.revertAddTileToMiddleOfGroupFromMiddleOfGroupAndSplit(move._sourceGroupId, move._sourcePositionInGroup, move._destinationGroupId, move._destinationPositionInGroup, groupSplitInfo);
			}
			else
			{
				throw std::runtime_error("Move source type is not supported for board moves");
			}
		}
		else if (move._moveDestinationType == MoveDestinationType::MiddleOfGroupAndSplit)
		{
			if (move._moveSourceType == MoveSourceType::StartOfGroup)
			{
				GroupSplitInfo groupSplitInfo;
				groupSplitInfo.originalGroupId = move._moveGroupSummary.originalGroupTarget;
				groupSplitInfo.createdGroupIds[0] = move._moveGroupSummary.groupCreatedFirstFromSplitTarget;
				groupSplitInfo.createdGroupIds[1] = move._moveGroupSummary.groupCreatedSecondFromSplitTarget;

				_board.revertAddTileToMiddleOfGroupAndSplitFromStartOfGroup(move._sourceGroupId, move._destinationGroupId, groupSplitInfo);
			}
			else if (move._moveSourceType == MoveSourceType::EndOfGroup)
			{
				GroupSplitInfo groupSplitInfo;
				groupSplitInfo.originalGroupId = move._moveGroupSummary.originalGroupTarget;
				groupSplitInfo.createdGroupIds[0] = move._moveGroupSummary.groupCreatedFirstFromSplitTarget;
				groupSplitInfo.createdGroupIds[1] = move._moveGroupSummary.groupCreatedSecondFromSplitTarget;

				_board.revertAddTileToMiddleOfGroupAndSplitFromEndOfGroup(move._sourceGroupId, move._destinationGroupId, groupSplitInfo);
			}
			else if (move._moveSourceType == MoveSourceType::MiddleOfGroup)
			{
				GroupSplitInfo groupSplitInfo;
				groupSplitInfo.originalGroupId = move._moveGroupSummary.originalGroupTarget;
				groupSplitInfo.createdGroupIds[0] = move._moveGroupSummary.groupCreatedFirstFromSplitTarget;
				groupSplitInfo.createdGroupIds[1] = move._moveGroupSummary.groupCreatedSecondFromSplitTarget;

				_board.revertAddTileToMiddleOfGroupAndSplitFromMiddleOfGroup(move._sourceGroupId, move._sourcePositionInGroup, move._destinationGroupId, groupSplitInfo);
			}
			else if (move._moveSourceType == MoveSourceType::MiddleOfGroupAndSplit)
			{
				GroupSplitInfo groupSplitInfoTarget;
				groupSplitInfoTarget.originalGroupId = move._moveGroupSummary.originalGroupTarget;
				groupSplitInfoTarget.createdGroupIds[0] = move._moveGroupSummary.groupCreatedFirstFromSplitTarget;
				groupSplitInfoTarget.createdGroupIds[1] = move._moveGroupSummary.groupCreatedSecondFromSplitTarget;

				GroupSplitInfo groupSplitInfoSource;
				groupSplitInfoSource.originalGroupId = move._moveGroupSummary.originalGroupSource;
				groupSplitInfoSource.createdGroupIds[0] = move._moveGroupSummary.groupCreatedFirstFromSplitSource;
				groupSplitInfoSource.createdGroupIds[1] = move._moveGroupSummary.groupCreatedSecondFromSplitSource;

				_board.revertAddTileToMiddleOfGroupAndSplitFromMiddleOfGroupAndSplit(move._sourceGroupId, move._sourcePositionInGroup, move._destinationGroupId, groupSplitInfoSource, groupSplitInfoTarget);
			}
			else
			{
				throw std::runtime_error("Move source type is not supported for board moves");
			}
		}
		else
		{
			throw std::runtime_error("Move destination type is not supported for board moves");
		}

	}

	if (_debug)
	{
//		debugUndoMove(move);
	}
}

} // namespace Tiles




