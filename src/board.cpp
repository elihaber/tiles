/*
 * board.cpp
 *
 *  Created on: Sep 10, 2017
 *      Author: eliha
 */

#//ifdef WIN32
#include "stdafx.h"
//#endif

#include <iostream>
#include <fstream>

#include <algorithm>
#include <cassert>

#include "board.h"
#include "group.h"
#include "mover.h"
#include "defs.h"

namespace Tiles
{

static int MAX_POSSIBLE_GROUPS = 100;
static int MIN_GROUP_ID = 1;

Board::Board() : _numTiles(0), _nextId(MIN_GROUP_ID)
{
	for (int i = 0; i < MAX_POSSIBLE_GROUPS; ++i)
	{
		_idsInUse.push_back(false);
	}
}

bool Board::operator==(const Board& other) const
{
	if (this->numGroups() != other.numGroups())
		return false;
	if (this->numTiles() != other.numTiles())
		return false;

	std::vector<int> idsAlreadyUsed;
	bool foundAllGroups = true;

	for (const auto & group : _groups)
	{
		bool foundMatchingGroup = false;
		for (int i = 0; i < other.numGroups(); ++i)
		{
			auto otherGroup = other.getGroupByPosition(i);

			// We will test if otherGroup is a match for group.

			if (std::find(idsAlreadyUsed.begin(), idsAlreadyUsed.end(), i) != idsAlreadyUsed.end())
			{
				// otherGroup has already been matched to a group, so skip it.
				continue;
			}
			if (group->numTiles() != otherGroup->numTiles())
			{
				// otherGroup does not have the same number of tiles as group, so skip it.
				continue;
			}
			if ((group->isSet() && !otherGroup->isSet()) || (group->isRun() && !otherGroup->isRun()) || (group->isNone() && !otherGroup->isNone()))
			{
				// otherGroup is a different group type than group, so skip it.
				continue;
			}
			if (group->isRun())
			{
				bool allTilesMatch = true;
				for (int i = 0; i < group->numTiles(); ++i)
				{
					if (group->getTile(i)->isJoker())
					{
						if (!otherGroup->getTile(i)->isJoker())
						{
							allTilesMatch = false;
							break;
						}
					}
					else
					{
						if (otherGroup->getTile(i)->isJoker())
						{
							allTilesMatch = false;
							break;
						}
						if (group->getTile(i)->getValue() != otherGroup->getTile(i)->getValue())
						{
							allTilesMatch = false;
							break;
						}
					}
				}

				if (allTilesMatch)
				{
					// otherGroup has the same starting tile as group, so it must be a match.
					foundMatchingGroup = true;
					idsAlreadyUsed.push_back(i);
					break;
				}

				// otherGroup does not have the same starting tile as group, so skip it.
				continue;
			}
			if (group->isSet())
			{
				int groupValue = group->getSetValue();
				int otherGroupValue = otherGroup->getSetValue();
				if (groupValue != otherGroupValue)
				{
					// otherGroup does not have the same value tiles as group, so skip it.
					continue;
				}

				bool allColorsFound = true;
				for (int j = 0; j < group->numTiles(); ++j)
				{
					if (group->getTile(j)->isJoker())
					{
						continue;
					}
					if (!otherGroup->setContainsColor(group->getTile(j)->getColor()))
					{
						allColorsFound = false;
						break;
					}
				}

				if (allColorsFound)
				{
					// otherGroup has all the colors that group has, so it must be a match.
					foundMatchingGroup = true;
					idsAlreadyUsed.push_back(i);
					break;
				}
				else
				{
					// otherGroup does not have one of the colors that group has, so skip it.
					continue;
				}
			}
			if (group->numTiles() == 0)
			{
				foundMatchingGroup = true;
				idsAlreadyUsed.push_back(i);
				break;
			}
			if (group->numTiles() == 1)
			{
				if ((group->getTile(0)->isJoker() && otherGroup->getTile(0)->isJoker()) || (!group->getTile(0)->isJoker() && !otherGroup->getTile(0)->isJoker() && group->getTile(0)->getColor() == otherGroup->getTile(0)->getColor() && group->getTile(0)->getValue() == otherGroup->getTile(0)->getValue()))
				{
					foundMatchingGroup = true;
					idsAlreadyUsed.push_back(i);
					break;
				}
				else
				{
					continue;
				}
			}
			if (group->numTiles() == 2)
			{
				if (group->numJokers() == 1)
				{
					if (otherGroup->numJokers() != 1)
					{
						continue;
					}
					int regularpos1 = 0;
					if (group->firstTile()->isJoker())
					{
						regularpos1 = 1;
					}
					int regularpos2 = 0;
					if (otherGroup->firstTile()->isJoker())
					{
						regularpos2= 1;
					}
					if (group->getTile(regularpos1)->getColor() == otherGroup->getTile(regularpos2)->getColor() && group->getTile(regularpos1)->getValue() == otherGroup->getTile(regularpos2)->getValue())
					{
						foundMatchingGroup = true;
						idsAlreadyUsed.push_back(i);
						break;
					}
					else
					{
						continue;
					}
				}
				if (group->numJokers() == 2)
				{
					if (otherGroup->numJokers() == 2)
					{
						foundMatchingGroup = true;
						idsAlreadyUsed.push_back(i);
						break;
					}
					else
					{
						continue;
					}
				}
			}
			// otherGroup is a single tile that does not match group, so skip it.
		}
		if (!foundMatchingGroup)
		{
			foundAllGroups = false;
			break;
		}
	}
	return foundAllGroups;
}

Board Board::clone() const
{
	Board board;
	return board;
}

//===============================================================================
//=============== Group and Tile Management: start
//===============================================================================

void Board::debug(std::string text) const
{
	/*
	std::cout << "========== " << text << std::endl;
	std::cout << "_groups.size() is " << _groups.size() << std::endl;

	for (unsigned int i = 0; i < _groups.size(); ++i)
	{
		try
		{
			std::cout << "i=" << i << std::endl;
			std::cout << "id=" << _groups[i]->id() << std::endl;
		}
		catch (std::runtime_error& e)
		{
			std::cout << "Exception: " << e.what() << std::endl;
		}
	}
	std::cout << "========== " << text << std::endl;
	*/
}

void Board::recalculateNextId()
{
	debug("recalculateNextId start");
	for (int i = MIN_GROUP_ID; i < MAX_POSSIBLE_GROUPS; ++i)
	{
		if (!_idsInUse[i])
		{
			_nextId = i;
			break;
		}
	}
	debug("recalculateNextId end");
}

void Board::addInvalidId(int id)
{
	debug("addInvalidId start");
	_invalidGroupIds.push_back(id);
	debug("addInvalidId end");
}

void Board::removeInvalidId(int id)
{
	debug("removeInvalidId start");
	auto pos = std::find(_invalidGroupIds.begin(), _invalidGroupIds.end(), id);

	if (pos != _invalidGroupIds.end())
	{
		_invalidGroupIds.erase(pos);
	}
	debug("removeInvalidId end");
}

std::shared_ptr<Group> Board::findGroupById(int id) const
{
	debug("findGroupById start");
	int pos = -1;
	try
	{
		pos = _groupIdsToPositions.at(id);
	}
	catch (std::out_of_range& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		std::cout << "id: " << id << std::endl;
		for (auto iter = _groupIdsToPositions.begin(); iter != _groupIdsToPositions.end(); ++iter)
		{
			std::cout << iter->first <<"->" << iter->second << std::endl;
		}
		std::cout << "that is all" << std::endl;
	}

	debug("findGroupById end");
	return _groups.at(pos);
}

int Board::findGroupPositionById(int id) const
{
	debug("findGroupPositionById start");
	int val = _groupIdsToPositions.at(id);
	debug("findGroupPositionById end");
	return val;
}

int Board::createNewGroup(std::shared_ptr<Tile> tile)
{
	debug("createNewGroup start");
	auto id = addGroup(std::make_shared<Group>(tile));
	findGroupById(id)->setTryMovingToOtherGroups(false);
	debug("createNewGroup end");

	assert(validateAllGroups());

	return id;
}

int Board::addGroup(std::shared_ptr<Group> group, int forcedGroupId)
{
	debug("addGroup start");
	int id = _nextId;
	if (forcedGroupId != -1)
	{
		id = forcedGroupId;
	}
	group->setId(id);

	if (!group->isValid())
	{
		addInvalidId(id);
	}

	_groups.push_back(group);
	_numTiles += group->numTiles();
	_groupIdsToPositions.emplace(id, (int)(_groups.size() - 1));
	_idsInUse[id] = true;

	recalculateNextId();

	debug("addGroup end");

	assert(checkInternalConsistency());

	assert(validateAllGroups());

	return id;
}

void Board::removeGroup(std::shared_ptr<Group> group, bool saveId)
{
	if (saveId)
	{
		_numTiles -= group->numTiles();
		group->removeAllTiles();
		removeInvalidId(group->id());
		return;
	}
	debug("removeGroup start");
	_numTiles -= group->numTiles();

	int position = findGroupPositionById(group->id());

	_groupIdsToPositions.erase(group->id());

	// All the groups placed after this group will have their positions reduced by 1.
	for (unsigned int i = position + 1; i < _groups.size(); ++i)
	{
		_groupIdsToPositions[_groups[i]->id()] = i - 1;
	}

	_idsInUse[group->id()] = false;

	removeInvalidId(group->id());

	_groups.erase(_groups.begin() + position);

	recalculateNextId();
	debug("removeGroup end");

	assert(checkInternalConsistency());

	assert(validateAllGroups());

}

bool Board::existInvalidGroups() const
{
	for (auto group : _groups)
	{
		if (!group->isValid())
		{
			return true;
		}
	}
	return false;
}

int Board::getFirstInvalidGroup() const
{
	for (auto group : _groups)
	{
		if (!group->isValid())
		{
			return group->id();
		}
	}
	throw std::runtime_error("Expected to find at least one invalid group");
}

void Board::createGroupIgnoreList()
{
	for (int i = 0; i < numGroups(); ++i)
	{
		_groupsToIgnore.push_back(_groups[i]->id());
	}
}

void Board::removeGroupIgnoreList()
{
	_groupsToIgnore.clear();
}

bool Board::ignoreGroup(int id) const
{
	return (std::find(_groupsToIgnore.begin(), _groupsToIgnore.end(), id) != _groupsToIgnore.end());
}

/*
void Board::validateNumTiles(std::string text) const
{
	int total = 0;
	for (auto group : _groups)
	{
		total += group->numTiles();
	}
	if (total != _numTiles)
	{
		std::cout << "NUMTILES FAILED TO VALIDATE: _numTiles=" << _numTiles << " total=" << total << " location: " << text << std::endl;
	}
}
*/

void Board::trapJokers()
{
	for (auto group : _groups)
	{
		group->trapJokers();
	}

	assert(validateAllGroups());

}

//===============================================================================
//=============== Group and Tile Management: end
//===============================================================================

//===============================================================================
//=============== Atomic Operations: start
//===============================================================================

void Board::addTileToStartOfGroup(std::shared_ptr<Tile> tile, std::shared_ptr<Group> group, bool isRevert)
{
	bool groupWasValid = group->isValid();

	group->addTileToStart(tile);
	++_numTiles;

	if (!groupWasValid && group->isValid())
	{
		removeInvalidId(group->id());
	}

	assert(validateAllGroups());

}

void Board::addTileToEndOfGroup(std::shared_ptr<Tile> tile, std::shared_ptr<Group> group, bool isRevert)
{
	bool groupWasValid = group->isValid();

	group->addTileToEnd(tile);
	++_numTiles;

	if (!groupWasValid && group->isValid())
	{
		removeInvalidId(group->id());
	}

	assert(validateAllGroups());

}

int Board::addTileToMiddleOfGroup(std::shared_ptr<Tile> tile, std::shared_ptr<Group> group, int position, bool isRevert)
{
	bool groupWasValid = group->isValid();

	group->addTileToMiddle(tile, position);
	++_numTiles;

	if (!groupWasValid && group->isValid())
	{
		removeInvalidId(group->id());
	}

	return position;

	assert(validateAllGroups());

}

void Board::addTileToMiddleOfGroupAndSplit(std::shared_ptr<Tile> tile, std::shared_ptr<Group> group, int position, GroupSplitInfo& groupSplitInfo, bool isRevert)
{
	std::vector<std::shared_ptr<Group>> groupsCreated;
	group->addTileToMiddleAndSplit(tile, position, RunSplitType::AddTileToEndOfFirstRun, groupsCreated);
	for (unsigned int i = 0; i < groupsCreated.size(); ++i)
	{
		groupSplitInfo.createdGroupIds[i] = addGroup(groupsCreated[i]);
	}
	removeGroup(group, !isRevert);

	assert(validateAllGroups());

}

void Board::removeTileFromStartOfGroup(std::shared_ptr<Group> group, bool isRevert)
{
	bool groupWasValid = group->isValid();

	group->removeFirstTile();
	--_numTiles;

	if (groupWasValid && !group->isValid())
	{
		addInvalidId(group->id());
	}

	if (group->numTiles() == 0)
	{
		removeGroup(group, !isRevert);
	}

	assert(validateAllGroups());

}

void Board::removeTileFromEndOfGroup(std::shared_ptr<Group> group, bool isRevert)
{
	bool groupWasValid = group->isValid();

	group->removeLastTile();
	--_numTiles;

	if (groupWasValid && !group->isValid())
	{
		addInvalidId(group->id());
	}

	if (group->numTiles() == 0)
	{
		removeGroup(group, !isRevert);
	}

	assert(validateAllGroups());

}

void Board::removeTileFromMiddleOfGroup(std::shared_ptr<Group> group, int position, bool isRevert)
{
	bool groupWasValid = group->isValid();

	group->removeTileFromMiddle(position);
	--_numTiles;

	if (groupWasValid && !group->isValid())
	{
		addInvalidId(group->id());
	}

	if (group->numTiles() == 0)
	{
		removeGroup(group, !isRevert);
	}

	assert(validateAllGroups());

}

void Board::removeTileFromMiddleOfGroupAndSplit(std::shared_ptr<Group> group, int position, GroupSplitInfo& groupSplitInfo, bool isRevert)
{
	std::vector<std::shared_ptr<Group>> groupsCreated;
	group->removeTileFromMiddleAndSplit(position, groupsCreated);
	removeGroup(group, !isRevert);
	for (unsigned int i = 0; i < groupsCreated.size(); ++i)
	{
		groupSplitInfo.createdGroupIds[i] = addGroup(groupsCreated[i]);
	}

	assert(validateAllGroups());

}

void Board::removeOnlyTileOfGroup(std::shared_ptr<Group> group, bool isRevert)
{
	group->removeOnlyTile();
	--_numTiles;
	removeGroup(group, !isRevert);

	assert(validateAllGroups());

}

void Board::reconstituteSplitGroupWithoutLastTileFromFirstGroup(const GroupSplitInfo& groupSplitInfo, bool isRevert)
{
	// This will combine 2 groups, removing the last tile from the first group.
	auto combinedGroup = Group::createRunFromTwoRuns(findGroupById(groupSplitInfo.createdGroupIds[0]), findGroupById(groupSplitInfo.createdGroupIds[1]));

	if (isRevert)
	{
		auto group = findGroupById(groupSplitInfo.originalGroupId);
		for (int i = 0; i < combinedGroup->numTiles(); ++i)
		{
			group->addTileToEnd(combinedGroup->getTile(i));
			++_numTiles;
		}
	}
	else
	{
		addGroup(combinedGroup);
	}

	removeGroup(findGroupById(groupSplitInfo.createdGroupIds[1]), !isRevert);
	removeGroup(findGroupById(groupSplitInfo.createdGroupIds[0]), !isRevert);

	assert(validateAllGroups());

}

void Board::reconstituteSplitGroupWithTileFromAnotherGroup(const GroupSplitInfo& groupSplitInfo, std::shared_ptr<Tile> tile, bool isRevert)
{
	// This will combine 2 groups, adding the provided tile in between them.
	auto combinedGroup = Group::createRunFromTwoRunsAndTile(findGroupById(groupSplitInfo.createdGroupIds[0]), findGroupById(groupSplitInfo.createdGroupIds[1]), tile);

	if (isRevert)
	{
		auto group = findGroupById(groupSplitInfo.originalGroupId);
		for (int i = 0; i < combinedGroup->numTiles(); ++i)
		{
			group->addTileToEnd(combinedGroup->getTile(i));
			++_numTiles;
		}
	}
	else
	{
		addGroup(combinedGroup);
	}

	removeGroup(findGroupById(groupSplitInfo.createdGroupIds[1]), !isRevert);
	removeGroup(findGroupById(groupSplitInfo.createdGroupIds[0]), !isRevert);

	assert(validateAllGroups());

}

//===============================================================================
//=============== Atomic Operations: end
//===============================================================================

//===============================================================================
//=============== Complex Operations: start
//===============================================================================

void Board::addTileToStartOfGroupFromStartOfGroup(int sourceGroupId, int destinationGroupId, bool isRevert)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = sourceGroup->firstTile();

	removeTileFromStartOfGroup(sourceGroup, isRevert);
	addTileToStartOfGroup(tile, destinationGroup, isRevert);
}

void Board::addTileToStartOfGroupFromEndOfGroup(int sourceGroupId, int destinationGroupId, bool isRevert)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = sourceGroup->lastTile();

	removeTileFromEndOfGroup(sourceGroup, isRevert);
	addTileToStartOfGroup(tile, destinationGroup, isRevert);
}

void Board::addTileToStartOfGroupFromMiddleOfGroup(int sourceGroupId, int sourcePosition, int destinationGroupId, bool isRevert)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = sourceGroup->getTile(sourcePosition);

	removeTileFromMiddleOfGroup(sourceGroup, sourcePosition, isRevert);
	addTileToStartOfGroup(tile, destinationGroup, isRevert);
}

void Board::addTileToStartOfGroupFromMiddleOfGroupAndSplit(int sourceGroupId, int sourcePosition, int destinationGroupId, GroupSplitInfo& groupSplitInfo, bool isRevert)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = sourceGroup->getTile(sourcePosition);

	removeTileFromMiddleOfGroupAndSplit(sourceGroup, sourcePosition, groupSplitInfo, isRevert);
	addTileToStartOfGroup(tile, destinationGroup, isRevert);
}

void Board::addTileToEndOfGroupFromStartOfGroup(int sourceGroupId, int destinationGroupId, bool isRevert)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = sourceGroup->firstTile();

	removeTileFromStartOfGroup(sourceGroup, isRevert);
	addTileToEndOfGroup(tile, destinationGroup, isRevert);
}

void Board::addTileToEndOfGroupFromEndOfGroup(int sourceGroupId, int destinationGroupId, bool isRevert)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = sourceGroup->lastTile();

	removeTileFromEndOfGroup(sourceGroup, isRevert);
	addTileToEndOfGroup(tile, destinationGroup, isRevert);
}

void Board::addTileToEndOfGroupFromMiddleOfGroup(int sourceGroupId, int sourcePosition, int destinationGroupId, bool isRevert)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = sourceGroup->getTile(sourcePosition);

	removeTileFromMiddleOfGroup(sourceGroup, sourcePosition, isRevert);
	addTileToEndOfGroup(tile, destinationGroup, isRevert);
}

void Board::addTileToEndOfGroupFromMiddleOfGroupAndSplit(int sourceGroupId, int sourcePosition, int destinationGroupId, GroupSplitInfo& groupSplitInfo, bool isRevert)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = sourceGroup->getTile(sourcePosition);

	removeTileFromMiddleOfGroupAndSplit(sourceGroup, sourcePosition, groupSplitInfo, isRevert);
	addTileToEndOfGroup(tile, destinationGroup, isRevert);
}

int Board::addTileToMiddleOfGroupFromStartOfGroup(int sourceGroupId, int destinationGroupId, int destinationPosition, bool isRevert)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = sourceGroup->firstTile();

	removeTileFromStartOfGroup(sourceGroup, isRevert);
	int pos = addTileToMiddleOfGroup(tile, destinationGroup, destinationPosition, isRevert);
	return pos;
}

int Board::addTileToMiddleOfGroupFromEndOfGroup(int sourceGroupId, int destinationGroupId, int destinationPosition, bool isRevert)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = sourceGroup->lastTile();

	removeTileFromEndOfGroup(sourceGroup, isRevert);
	int pos = addTileToMiddleOfGroup(tile, destinationGroup, destinationPosition, isRevert);
	return pos;
}

int Board::addTileToMiddleOfGroupFromMiddleOfGroupAndSplit(int sourceGroupId, int sourcePosition, int destinationGroupId, int destinationPosition, GroupSplitInfo& groupSplitInfo, bool isRevert)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = sourceGroup->getTile(sourcePosition);

	removeTileFromMiddleOfGroupAndSplit(sourceGroup, sourcePosition, groupSplitInfo, isRevert);
	int pos = addTileToMiddleOfGroup(tile, destinationGroup, destinationPosition, isRevert);
	return pos;
}

int Board::addTileToMiddleOfGroupFromMiddleOfGroup(int sourceGroupId, int sourcePosition, int destinationGroupId, int destinationPosition, bool isRevert)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = sourceGroup->getTile(sourcePosition);

	removeTileFromMiddleOfGroup(sourceGroup, sourcePosition, isRevert);
	int pos = addTileToMiddleOfGroup(tile, destinationGroup, destinationPosition, isRevert);
	return pos;
}

void Board::addTileToMiddleOfGroupAndSplitFromStartOfGroup(int sourceGroupId, int destinationGroupId, GroupSplitInfo& groupSplitInfo, int targetPosition, bool isRevert)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = sourceGroup->firstTile();

	removeTileFromStartOfGroup(sourceGroup, isRevert);
	addTileToMiddleOfGroupAndSplit(tile, destinationGroup, targetPosition, groupSplitInfo, isRevert);
}

void Board::addTileToMiddleOfGroupAndSplitFromEndOfGroup(int sourceGroupId, int destinationGroupId, GroupSplitInfo& groupSplitInfo, int targetPosition, bool isRevert)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = sourceGroup->lastTile();

	removeTileFromEndOfGroup(sourceGroup, isRevert);
	addTileToMiddleOfGroupAndSplit(tile, destinationGroup, targetPosition, groupSplitInfo, isRevert);
}

void Board::addTileToMiddleOfGroupAndSplitFromMiddleOfGroup(int sourceGroupId, int sourcePosition, int destinationGroupId, int targetPosition, GroupSplitInfo& groupSplitInfo, bool isRevert)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = sourceGroup->getTile(sourcePosition);

	removeTileFromMiddleOfGroup(sourceGroup, sourcePosition, isRevert);
	addTileToMiddleOfGroupAndSplit(tile, destinationGroup, targetPosition, groupSplitInfo, isRevert);
}

void Board::addTileToMiddleOfGroupAndSplitFromMiddleOfGroupAndSplit(int sourceGroupId, int sourcePosition, int destinationGroupId, int targetPosition, GroupSplitInfo& groupSplitInfoSource, GroupSplitInfo& groupSplitInfoDestination, bool isRevert)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = sourceGroup->getTile(sourcePosition);

	removeTileFromMiddleOfGroupAndSplit(sourceGroup, sourcePosition, groupSplitInfoSource, isRevert);
	addTileToMiddleOfGroupAndSplit(tile, destinationGroup, targetPosition, groupSplitInfoDestination, isRevert);
}

//===============================================================================
//=============== Complex Operations: end
//===============================================================================

//===============================================================================
//=============== Revert Complex Operations: start
//===============================================================================

void Board::revertAddTileToStartOfGroupFromStartOfGroup(int sourceGroupId, int destinationGroupId)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = destinationGroup->firstTile();

	addTileToStartOfGroup(tile, sourceGroup, true);
	removeTileFromStartOfGroup(destinationGroup, true);
}

void Board::revertAddTileToStartOfGroupFromEndOfGroup(int sourceGroupId, int destinationGroupId)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = destinationGroup->firstTile();

	addTileToEndOfGroup(tile, sourceGroup, true);
	removeTileFromStartOfGroup(destinationGroup, true);
}

void Board::revertAddTileToStartOfGroupFromMiddleOfGroup(int sourceGroupId, int sourcePosition, int destinationGroupId)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = destinationGroup->firstTile();

	addTileToMiddleOfGroup(tile, sourceGroup, sourcePosition, true);
	removeTileFromStartOfGroup(destinationGroup, true);
}

void Board::revertAddTileToStartOfGroupFromMiddleOfGroupAndSplit(int destinationGroupId, GroupSplitInfo& groupSplitInfo)
{
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = destinationGroup->firstTile();

	reconstituteSplitGroupWithTileFromAnotherGroup(groupSplitInfo, tile, true);
	removeTileFromStartOfGroup(destinationGroup, true);
}

void Board::revertAddTileToEndOfGroupFromStartOfGroup(int sourceGroupId, int destinationGroupId)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = destinationGroup->lastTile();

	addTileToStartOfGroup(tile, sourceGroup, true);
	removeTileFromEndOfGroup(destinationGroup, true);
}

void Board::revertAddTileToEndOfGroupFromEndOfGroup(int sourceGroupId, int destinationGroupId)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = destinationGroup->lastTile();

	addTileToEndOfGroup(tile, sourceGroup, true);
	removeTileFromEndOfGroup(destinationGroup, true);
}

void Board::revertAddTileToEndOfGroupFromMiddleOfGroup(int sourceGroupId, int sourcePosition, int destinationGroupId)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = destinationGroup->lastTile();

	addTileToMiddleOfGroup(tile, sourceGroup, sourcePosition, true);
	removeTileFromEndOfGroup(destinationGroup, true);
}

void Board::revertAddTileToEndOfGroupFromMiddleOfGroupAndSplit(int destinationGroupId, GroupSplitInfo& groupSplitInfo)
{
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = destinationGroup->lastTile();

	reconstituteSplitGroupWithTileFromAnotherGroup(groupSplitInfo, tile, true);
	removeTileFromEndOfGroup(destinationGroup, true);
}

void Board::revertAddTileToMiddleOfGroupFromStartOfGroup(int sourceGroupId, int destinationGroupId, int destinationPosition)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = destinationGroup->lastTile();

	addTileToStartOfGroup(tile, sourceGroup, true);
	removeTileFromMiddleOfGroup(destinationGroup, destinationGroup->numTiles() - 1, true);
}

void Board::revertAddTileToMiddleOfGroupFromEndOfGroup(int sourceGroupId, int destinationGroupId, int destinationPosition)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = destinationGroup->lastTile();

	addTileToEndOfGroup(tile, sourceGroup, true);
	removeTileFromMiddleOfGroup(destinationGroup, destinationGroup->numTiles() - 1, true);
}

void Board::revertAddTileToMiddleOfGroupFromMiddleOfGroup(int sourceGroupId, int sourcePosition, int destinationGroupId, int destinationPosition)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = destinationGroup->lastTile();

	addTileToMiddleOfGroup(tile, sourceGroup, sourcePosition, true);
	removeTileFromMiddleOfGroup(destinationGroup, destinationGroup->numTiles() - 1, true);
}

void Board::revertAddTileToMiddleOfGroupFromMiddleOfGroupAndSplit(int sourceGroupId, int sourcePosition, int destinationGroupId, int destinationPosition, GroupSplitInfo& groupSplitInfo)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto destinationGroup = findGroupById(destinationGroupId);
	auto tile = destinationGroup->lastTile();

	reconstituteSplitGroupWithTileFromAnotherGroup(groupSplitInfo, tile, true);
	removeTileFromMiddleOfGroup(destinationGroup, destinationGroup->numTiles() - 1, true);
}

void Board::revertAddTileToMiddleOfGroupAndSplitFromStartOfGroup(int sourceGroupId, int destinationGroupId, GroupSplitInfo& groupSplitInfo)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto firstDestinationGroup = findGroupById(groupSplitInfo.createdGroupIds[0]);
	auto tile = firstDestinationGroup->lastTile(); // The tile was always added to the end of the first split group.

	addTileToStartOfGroup(tile, sourceGroup, true);
	reconstituteSplitGroupWithoutLastTileFromFirstGroup(groupSplitInfo, true);
}

void Board::revertAddTileToMiddleOfGroupAndSplitFromEndOfGroup(int sourceGroupId, int destinationGroupId, GroupSplitInfo& groupSplitInfo)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto firstDestinationGroup = findGroupById(groupSplitInfo.createdGroupIds[0]);
	auto tile = firstDestinationGroup->lastTile(); // The tile was always added to the end of the first split group.

	addTileToEndOfGroup(tile, sourceGroup, true);
	reconstituteSplitGroupWithoutLastTileFromFirstGroup(groupSplitInfo, true);
}

void Board::revertAddTileToMiddleOfGroupAndSplitFromMiddleOfGroup(int sourceGroupId, int sourcePosition, int destinationGroupId, GroupSplitInfo& groupSplitInfo)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto firstDestinationGroup = findGroupById(groupSplitInfo.createdGroupIds[0]);
	auto tile = firstDestinationGroup->lastTile(); // The tile was always added to the end of the first split group.

	addTileToMiddleOfGroup(tile, sourceGroup, sourcePosition, true);
	reconstituteSplitGroupWithoutLastTileFromFirstGroup(groupSplitInfo, true);
}

void Board::revertAddTileToMiddleOfGroupAndSplitFromMiddleOfGroupAndSplit(int sourceGroupId, int sourcePosition, int destinationGroupId, GroupSplitInfo& groupSplitInfoSource, GroupSplitInfo& groupSplitInfoDestination)
{
	auto sourceGroup = findGroupById(sourceGroupId);
	auto firstDestinationGroup = findGroupById(groupSplitInfoDestination.createdGroupIds[0]);
	auto tile = firstDestinationGroup->lastTile(); // The tile was always added to the end of the first split group.

	reconstituteSplitGroupWithTileFromAnotherGroup(groupSplitInfoSource, tile, true);
	reconstituteSplitGroupWithoutLastTileFromFirstGroup(groupSplitInfoDestination, true);
}

//===============================================================================
//=============== Revert Complex Operations: end
//===============================================================================

//===============================================================================
//=============== Other Operations: start
//===============================================================================


// Find runs on the board that can take the given tile as their new start tile.
void Board::findRunsForTileAtStart(std::shared_ptr<Tile> tile, int originalGroupId, std::vector<int>& groups) const
{
	for (unsigned int i = 0; i < _groups.size(); ++i)
	{
		auto group = _groups[i];
		if (ignoreGroup(group->id()))
		{
			continue;
		}
		if (originalGroupId == group->id())
		{
			continue;
		}
		if (!group->isRun())
		{
			continue;
		}

		if ((tile->isJoker() && group->getRunStartValue() > 1) || (group->getRunColor() == tile->getColor() && group->getRunStartValue() == tile->getValue() + 1))
		{
			groups.push_back(group->id());
		}
	}
}

// Find runs on the board that can take the given tile as their new end tile.
void Board::findRunsForTileAtEnd(std::shared_ptr<Tile> tile, int originalGroupId, std::vector<int>& groups) const
{
	for (unsigned int i = 0; i < _groups.size(); ++i)
	{
		auto group = _groups[i];
		if (ignoreGroup(group->id()))
		{
			continue;
		}
		if (originalGroupId == group->id())
		{
			continue;
		}
		if (!group->isRun())
		{
			continue;
		}

		if ((tile->isJoker() && group->getRunEndValue() < 13) || (group->getRunColor() == tile->getColor() && group->getRunEndValue() == tile->getValue() - 1))
		{
			groups.push_back(group->id());
		}
	}
}

// Find runs on the board that can take the given tile in the middle of the run, and split into two valid runs.
void Board::findRunsForTileInMiddleWithValidSplit(std::shared_ptr<Tile> tile, int originalGroupId, std::vector<int>& groups) const
{
	for (unsigned int i = 0; i < _groups.size(); ++i)
	{
		auto group = _groups[i];
		if (ignoreGroup(group->id()))
		{
			continue;
		}
		if (originalGroupId == group->id())
		{
			continue;
		}
		if (!group->isRun())
		{
			continue;
		}
		if (group->numTiles() < 5)
		{
			continue;
		}

		if (tile->isJoker() || (group->getRunColor() == tile->getColor() && group->getRunStartValue() <= tile->getValue() - 2 && group->getRunEndValue() >= tile->getValue() + 2))
		{
			groups.push_back(group->id());
		}
	}
}

// Find runs on the board that can take the given tile in the middle of the run, and split into at least one invalid run.
void Board::findRunsForTileInMiddleWithInvalidSplit(std::shared_ptr<Tile> tile, int originalGroupId, std::vector<int>& groups) const
{
	for (unsigned int i = 0; i < _groups.size(); ++i)
	{
		auto group = _groups[i];
		if (ignoreGroup(group->id()))
		{
			continue;
		}
		if (originalGroupId == group->id())
		{
			continue;
		}
		if (!group->isRun())
		{
			continue;
		}

		if (tile->isJoker())
		{
			groups.push_back(group->id());
		}
		else
		{
			if (group->getRunStartValue() >= tile->getValue())
			{
				continue;
			}
			if (group->getRunEndValue() <= tile->getValue())
			{
				continue;
			}
			if (group->getRunColor() != tile->getColor())
			{
				continue;
			}
			if ((group->getRunStartValue() == tile->getValue() - 1 || group->getRunEndValue() == tile->getValue() + 1) && group->containsTile(tile))
			{
				groups.push_back(group->id());
			}
		}
	}
}

// Find sets on the board that can take the given tile and will not require splitting.
void Board::findSetsForTileWithoutSplit(std::shared_ptr<Tile> tile, int originalGroupId, std::vector<int>& groups) const
{
	for (unsigned int i = 0; i < _groups.size(); ++i)
	{
		auto group = _groups[i];
		if (ignoreGroup(group->id()))
		{
			continue;
		}
		if (originalGroupId == group->id())
		{
			continue;
		}
		if (!group->isSet())
		{
			continue;
		}
		if (group->numTiles() == 4)
		{
			continue;
		}

		if (tile->isJoker() || (group->getSetValue() == tile->getValue() && !group->setContainsColor(tile->getColor())))
		{
			groups.push_back(group->id());
		}
	}
}

// Find runs or sets on the board that can take the given tile and, if a set, will not require splitting.
void Board::findGroupsForTile(std::shared_ptr<Tile> tile, int originalGroupId, std::vector<int>& groups) const
{
	findRunsForTileAtStart(tile, originalGroupId, groups);
	findRunsForTileAtEnd(tile, originalGroupId, groups);
	findRunsForTileInMiddleWithValidSplit(tile, originalGroupId, groups);
	findSetsForTileWithoutSplit(tile, originalGroupId, groups);
}

// Find groups on the board that can lose this tile and will not cause anything to become invalid.
void Board::findGroupsThatCanSafelyLoseThisTile(std::shared_ptr<Tile> tile, std::vector<int>& groups) const
{
	for (const auto group : _groups)
	{
		if (ignoreGroup(group->id()))
		{
			continue;
		}
		if (group->numTiles() <= 3)
		{
			continue;
		}
		if (group->isSet())
		{
			if (group->getSetValue() == tile->getValue() && group->setContainsColor(tile->getColor()))
			{
				groups.push_back(group->id());
			}
			continue;
		}
		if (group->isRun())
		{
			// Exclude any groups of a wrong color
			if (group->getRunColor() != tile->getColor())
			{
				continue;
			}
			if (group->getRunStartValue() == tile->getValue() && !group->firstTile()->isJoker())
			{
				// The group is a run of the correct color with at least 4 tiles and its first tile matches the target tile.
				groups.push_back(group->id());
				continue;
			}
			if (group->getRunEndValue() == tile->getValue() && !group->lastTile()->isJoker())
			{
				// The group is a run of the correct color with at least 4 tiles and its last tile matches the target tile.
				groups.push_back(group->id());
				continue;
			}
			if (group->getRunStartValue() <= tile->getValue() - 3 && group->getRunEndValue() >= tile->getValue() + 3 && group->containsTile(tile))
			{
				// The group contains a match for the target tile, and taking it away will result in 2 valid groups.
				groups.push_back(group->id());
				continue;
			}
		}
	}
}

// Find groups on the board that can lose this tile and will cause them to become invalid.
void Board::findGroupsThatCanUnsafelyLoseThisTile(std::shared_ptr<Tile> tile, std::vector<int>& groups) const
{
	for (const auto group : _groups)
	{
		if (ignoreGroup(group->id()))
		{
			continue;
		}
		if (!group->containsTile(tile))
		{
			continue;
		}
		if (group->isSet())
		{
			if (group->numTiles() < 4)
			{
				groups.push_back(group->id());
			}
			continue;
		}
		if (group->isRun())
		{
			if (group->numTiles() < 4)
			{
				groups.push_back(group->id());
			}
			if (group->getRunStartValue() > tile->getValue() - 3 || group->getRunEndValue() < tile->getValue() + 3)
			{
				groups.push_back(group->id());
			}
		}
	}
}


/*
std::shared_ptr<Tile> Board::getTileFromGroup(int groupId, Tile& targetTile) const
{
	auto group = findGroupById(groupId);
	for (int i = 0; i < group->numTiles(); ++i)
	{
		auto tile = group->getTile(i);
		if (tile->getValue() == targetTile.getValue() && tile->getColor() == targetTile.getColor())
		{
			return tile;
		}
	}

	std::cout << "Searched for " << targetTile.str() << " in group " << group->id() << " found ";
	for (int i = 0; i < group->numTiles(); ++i)
	{
		auto tile = group->getTile(i);
		std::cout << tile->str();
		if (i < group->numTiles() - 1)
		{
			std::cout << " ";
		}
	}
	std::cout << std::endl;

	return nullptr; // TODO: Figure out a better way, maybe.
}
*/

void Board::printStatus()
{
	std::cout << "Board: numGroups=" << _groups.size() << " numTiles=" << _numTiles << std::endl;
	for (const auto & group : _groups)
	{
		group->printStatus();
	}
}

void Board::printStatus(std::ofstream& stream)
{
	stream << "Board: numGroups=" << _groups.size() << " numTiles=" << _numTiles << std::endl;
	for (const auto & group : _groups)
	{
		group->printStatus(stream);
	}
}

bool Board::checkInternalConsistency() const
{
	bool hasError = false;

	int numGroupIdMappings = _groupIdsToPositions.size();
	int numGroups = _groups.size();

	if (numGroupIdMappings != numGroups)
	{
		std::cout << "Number of groups is " << numGroups << " but number of group id mappings is " << numGroupIdMappings << std::endl;
		hasError = true;
	}

	for (auto iter = _groupIdsToPositions.begin(); iter != _groupIdsToPositions.end(); ++iter)
	{
		int groupId = iter->first;
		int position = iter->second;

		if (position > numGroups)
		{
			std::cout << "Board consistency check: _groupIdsToPositions[" << groupId << "]=" << position << ", which is more than the number of groups, " << numGroups << std::endl;
			hasError = true;
		}
		else if (position < 0)
		{
			std::cout << "Board consistency check: _groupIdsToPositions[" << groupId << "]=" << position << std::endl;
			hasError = true;
		}
		else
		{
			auto groupMappedTo = _groups[position];
			if (groupMappedTo->id() != groupId)
			{
				std::cout << "Board consistency check: _groupIdsToPositions[" << groupId << "]=" << position << ", but the group at that position actually has id=" << groupMappedTo->id() << std::endl;
				bool foundMappedGroup = false;
				for (unsigned int i = 0; i < _groups.size(); ++i)
				{
					if (_groups[i]->id() == groupId)
					{
						foundMappedGroup = true;
						std::cout << "Board consistency check: _groupIdsToPositions[" << groupId << "]=" << position << ", but group with id " << groupId << " is actually at position " << i << std::endl;
						hasError = true;
						break;
					}
				}
				if (!foundMappedGroup)
				{
					std::cout << "Board consistency check: _groupIdsToPositions[" << groupId << "]=" << position << ", but group with id " << groupId << " does not exist in the list of gorups" << std::endl;
					hasError = true;
				}
			}
		}
	}

	for (unsigned int i = 0; i < _groups.size(); ++i)
	{
		int groupId = _groups[i]->id();
		int position = i;

		auto iter = _groupIdsToPositions.find(groupId);
		if (iter == _groupIdsToPositions.end())
		{
			std::cout << "Board consistency check: Group at position " << position << " has id=" << groupId << " but that id does not exist in _groupIdsToPositions" << std::endl;
			hasError = true;
		}
		else
		{
			if (iter->second != position)
			{
				std::cout << "Board consistency check: Group at position " << position << " has id=" << groupId << " but _groupIdsToPositions[" << groupId << "]=" << iter->second << std::endl;
				hasError = true;
			}
		}

	}

	for (int i = 0; i < MAX_POSSIBLE_GROUPS; ++i)
	{
		if (_idsInUse[i])
		{
			auto iter = _groupIdsToPositions.find(i);
			if (iter == _groupIdsToPositions.end())
			{
				std::cout << "Board consistency check: _idsInUse is true for id = " << i << " but that id does not exist in _groupIdsToPositions" << std::endl;
				hasError = true;
			}
		}
	}

	for (auto iter = _groupIdsToPositions.begin(); iter != _groupIdsToPositions.end(); ++iter)
	{
		int groupId = iter->first;
		if (!_idsInUse[groupId])
		{
			std::cout << "Board consistency check: _idsInUse is false for id = " << groupId << " but that id exists in _groupIdsToPositions" << std::endl;
			hasError = true;
		}
	}

	return !hasError;
}

bool Board::validateAllGroups() const
{
	bool result = true;

	for (int i = 0; i < numGroups(); ++i)
	{
		auto group = _groups[i];

		if (!group->validate())
		{
			result = false;
		}
	}

	return result;
}

size_t Board::getHash() const
{
	size_t hash = 0;

	/*
	for (auto group : _groups)
	{
		hash = hash ^ group->getHash();
	}
	*/

	for (auto group : _groups)
	{
		for (int i = 0; i < group->numTiles(); ++i)
		{
			if (hash == 0)
			{
				hash = std::hash<std::string>()(group->getTile(i)->hashStr(group->id(), i));
			}
			else
			{
				hash = hash ^ std::hash<std::string>()(group->getTile(i)->hashStr(group->id(), i));
			}
		}
	}

	return hash;
}

} // namespace Tiles
