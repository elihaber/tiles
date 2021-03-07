/*
 * defs.h
 *
 *  Created on: Sep 11, 2017
 *      Author: eliha
 */

#ifndef INCLUDE_DEFS_H_
#define INCLUDE_DEFS_H_

#include <memory>
#include <sstream>
#include "enums.h"
#include "tile.h"

namespace Tiles
{

class Tile;

//typedef std::vector<std::shared_ptr<Move>> Solution;

struct GroupIdInfo
{
	GroupRedirectionType redirectionType;
	int redirectionMoveId;
	int groupId;

	GroupIdInfo(GroupRedirectionType redirType, int redirMoveId, int group) : redirectionType(redirType), redirectionMoveId(redirMoveId), groupId(group) { }

	std::string str()
	{
		std::stringstream sstr;

		if (redirectionType == GroupRedirectionType::CreatedFromTile)
		{
			sstr << "group created in move " << redirectionMoveId;
		}
		else if (redirectionType == GroupRedirectionType::CreatedFirstFromSplitSource)
		{
			sstr << "first group created by splitting source in move " << redirectionMoveId;
		}
		else if (redirectionType == GroupRedirectionType::CreatedSecondFromSplitSource)
		{
			sstr << "second group created by splitting source in move " << redirectionMoveId;
		}
		else if (redirectionType == GroupRedirectionType::CreatedFirstFromSplitTarget)
		{
			sstr << "first group created by splitting target in move " << redirectionMoveId;
		}
		else if (redirectionType == GroupRedirectionType::CreatedSecondFromSplitTarget)
		{
			sstr << "second group created by splitting target in move " << redirectionMoveId;
		}
		else
		{
			sstr << "illegal";
		}

		return sstr.str().c_str();
	}
};

struct MoveGroupSummary
{
	int groupCreatedFromTile;
	int originalGroupTarget;
	int groupCreatedFirstFromSplitTarget;
	int groupCreatedSecondFromSplitTarget;
	int originalGroupSource;
	int groupCreatedFirstFromSplitSource;
	int groupCreatedSecondFromSplitSource;

	MoveGroupSummary() :
		groupCreatedFromTile(-1),
		originalGroupTarget(-1), groupCreatedFirstFromSplitTarget(-1), groupCreatedSecondFromSplitTarget(-1),
		originalGroupSource(-1), groupCreatedFirstFromSplitSource(-1), groupCreatedSecondFromSplitSource(-1)
	{ }
};

struct Moove
{
	Location _source;
	Location _destination;
	int _sourceRackId;
	int _sourceRackPosition;
	int _sourceGroupId;
	int _sourcePositionInGroup;
	int _destinationGroupId;
	int _destinationPositionInGroup;
	MoveSourceType _moveSourceType;
	MoveDestinationType _moveDestinationType;
	MoveGroupSummary _moveGroupSummary;
	std::shared_ptr<Tile> _tile;

	Moove(Location source, Location destination, int sourceRackId, int sourceRackPosition,
		int sourceGroupId, int sourcePositionInGroup, int destinationGroupId, int destinationPositionInGroup,
		MoveSourceType moveSourceType, MoveDestinationType moveDestinationType,
		std::shared_ptr<Tile> tile)
		: _source(source), _destination(destination), _sourceRackId(sourceRackId), _sourceRackPosition(sourceRackPosition),
		  _sourceGroupId(sourceGroupId), _sourcePositionInGroup(sourcePositionInGroup), _destinationGroupId(destinationGroupId), _destinationPositionInGroup(destinationPositionInGroup),
		  _moveSourceType(moveSourceType), _moveDestinationType(moveDestinationType), _tile(tile)
	{

	}

	std::string str()
	{
		std::stringstream sstr;
		sstr << _tile->str();
		if (_source == Location::Rack)
		{
			sstr << " from rack " << _sourceRackId << " position " << _sourceRackPosition;
		}
		else
		{
			sstr << " from group " << _sourceGroupId;
			if (_moveSourceType == MoveSourceType::StartOfGroup)
			{
				sstr << " at start";
			}
			else if (_moveSourceType == MoveSourceType::EndOfGroup)
			{
				sstr << " at end";
			}
			else if (_moveSourceType == MoveSourceType::MiddleOfGroup)
			{
				sstr << " at position " << _sourcePositionInGroup;
			}
			else if (_moveSourceType == MoveSourceType::MiddleOfGroupAndSplit)
			{
				sstr << " at position " << _sourcePositionInGroup << " splitting the group into groups " << _moveGroupSummary.groupCreatedFirstFromSplitSource << " and " << _moveGroupSummary.groupCreatedSecondFromSplitSource;
			}
			else
			{
				throw std::runtime_error("Unknown source type");
			}
		}

		if (_moveDestinationType == MoveDestinationType::NewGroup)
		{
			sstr << " to new group wih id " << _moveGroupSummary.groupCreatedFromTile;
		}
		else
		{
			sstr << " to group " << _destinationGroupId;

			if (_moveDestinationType == MoveDestinationType::StartOfGroup)
			{
				sstr << " at start";
			}
			else if (_moveDestinationType == MoveDestinationType::EndOfGroup)
			{
				sstr << " at end";
			}
			else if (_moveDestinationType == MoveDestinationType::MiddleOfGroup)
			{
				sstr << " at position " << _destinationPositionInGroup;
			}
			else if (_moveDestinationType == MoveDestinationType::MiddleOfGroupAndSplit)
			{
				sstr << " at position " << _destinationPositionInGroup << " splitting the group into groups " << _moveGroupSummary.groupCreatedFirstFromSplitTarget << " and " << _moveGroupSummary.groupCreatedSecondFromSplitTarget;
			}
			else
			{
				throw std::runtime_error("Unknown destination type");
			}
		}

		return sstr.str().c_str();
	}
};

struct GroupSplitInfo
{
	int originalGroupId;
	int createdGroupIds[2];

	GroupSplitInfo()
	{
		originalGroupId = -1;
		createdGroupIds[0] = -1;
		createdGroupIds[1] = -1;
	}
};



} // namespace Tiles




#endif /* INCLUDE_DEFS_H_ */
