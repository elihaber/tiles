/*
 * board.h
 *
 *  Created on: Sep 7, 2017
 *      Author: eliha
 */

#ifndef INCLUDE_BOARD_H_
#define INCLUDE_BOARD_H_

#include <iostream>
#include <fstream>

#include <memory>
#include <vector>
#include <map>
#include "enums.h"

namespace Tiles
{

class Group;
class Tile;
struct GroupSplitInfo;

class Board
{
private:
	std::vector<std::shared_ptr<Group>> _groups;
	int _numTiles;
	int _nextId;
	std::map<int, int> _groupIdsToPositions;
	std::vector<bool> _idsInUse;
	std::vector<int> _invalidGroupIds;
	std::vector<int> _groupsToIgnore;

	void debug(std::string text) const;
	void recalculateNextId();
	void addInvalidId(int id);
	void removeInvalidId(int id);

public:
	Board();

	bool operator==(const Board& other) const;

	Board clone() const;

	int numTiles() const { return _numTiles; }
	int numGroups() const { return _groups.size(); }

	//===============================================================================
	//=============== Group and Tile Management
	//===============================================================================

	std::shared_ptr<Group> findGroupById(int id) const;
	int findGroupPositionById(int id) const;
	int createNewGroup(std::shared_ptr<Tile> tile);
	int addGroup(std::shared_ptr<Group> group, int forcedGroupId = -1);
	void removeGroup(std::shared_ptr<Group> group, bool saveId);
	bool existInvalidGroups() const;
	int getFirstInvalidGroup() const;
	void createGroupIgnoreList();
	void removeGroupIgnoreList();
	bool ignoreGroup(int id) const;
	std::shared_ptr<Group> getGroupByPosition(int position) const { return _groups[position]; }
	//void validateNumTiles(std::string text) const;
	void trapJokers();

	//===============================================================================
	//=============== Atomic Operations
	//===============================================================================

	void addTileToStartOfGroup(std::shared_ptr<Tile> tile, std::shared_ptr<Group> group, bool isRevert);
	void addTileToEndOfGroup(std::shared_ptr<Tile> tile, std::shared_ptr<Group> group, bool isRevert);
	int addTileToMiddleOfGroup(std::shared_ptr<Tile> tile, std::shared_ptr<Group> group, int position, bool isRevert);
	void addTileToMiddleOfGroupAndSplit(std::shared_ptr<Tile> tile, std::shared_ptr<Group> group, int position, GroupSplitInfo& groupSplitInfo, bool isRevert);
	void removeTileFromStartOfGroup(std::shared_ptr<Group> group, bool isRevert);
	void removeTileFromEndOfGroup(std::shared_ptr<Group> group, bool isRevert);
	void removeTileFromMiddleOfGroup(std::shared_ptr<Group> group, int position, bool isRevert);
	void removeTileFromMiddleOfGroupAndSplit(std::shared_ptr<Group> group, int position, GroupSplitInfo& groupSplitInfo, bool isRevert);
	void removeOnlyTileOfGroup(std::shared_ptr<Group> group, bool isRevert);

	void reconstituteSplitGroupWithoutLastTileFromFirstGroup(const GroupSplitInfo& groupSplitInfo, bool isRevert);
	void reconstituteSplitGroupWithTileFromAnotherGroup(const GroupSplitInfo& groupSplitInfo, std::shared_ptr<Tile> tile, bool isRevert);

	//===============================================================================
	//=============== Complex Operations
	//===============================================================================

	void addTileToStartOfGroupFromStartOfGroup(int sourceGroupId, int destinationGroupId, bool isRevert);
	void addTileToStartOfGroupFromEndOfGroup(int sourceGroupId, int destinationGroupId, bool isRevert);
	void addTileToStartOfGroupFromMiddleOfGroup(int sourceGroupId, int sourcePosition, int destinationGroupId, bool isRevert);
	void addTileToStartOfGroupFromMiddleOfGroupAndSplit(int sourceGroupId, int sourcePosition, int destinationGroupId, GroupSplitInfo& groupSplitInfo, bool isRevert);
	void addTileToEndOfGroupFromStartOfGroup(int sourceGroupId, int destinationGroupId, bool isRevert);
	void addTileToEndOfGroupFromEndOfGroup(int sourceGroupId, int destinationGroupId, bool isRevert);
	void addTileToEndOfGroupFromMiddleOfGroup(int sourceGroupId, int sourcePosition, int destinationGroupId, bool isRevert);
	void addTileToEndOfGroupFromMiddleOfGroupAndSplit(int sourceGroupId, int sourcePosition, int destinationGroupId, GroupSplitInfo& groupSplitInfo, bool isRevert);
	int addTileToMiddleOfGroupFromStartOfGroup(int sourceGroupId, int destinationGroupId, int destinationPosition, bool isRevert);
	int addTileToMiddleOfGroupFromEndOfGroup(int sourceGroupId, int destinationGroupId, int destinationPosition, bool isRevert);
	int addTileToMiddleOfGroupFromMiddleOfGroup(int sourceGroupId, int sourcePosition, int destinationGroupId, int destinationPosition, bool isRevert);
	int addTileToMiddleOfGroupFromMiddleOfGroupAndSplit(int sourceGroupId, int sourcePosition, int destinationGroupId, int destinationPosition, GroupSplitInfo& groupSplitInfo, bool isRevert);
	void addTileToMiddleOfGroupAndSplitFromStartOfGroup(int sourceGroupId, int destinationGroupId, GroupSplitInfo& groupSplitInfo, int targetPosition, bool isRevert);
	void addTileToMiddleOfGroupAndSplitFromEndOfGroup(int sourceGroupId, int destinationGroupId, GroupSplitInfo& groupSplitInfo, int targetPosition, bool isRevert);
	void addTileToMiddleOfGroupAndSplitFromMiddleOfGroup(int sourceGroupId, int sourcePosition, int destinationGroupId, int targetPosition, GroupSplitInfo& groupSplitInfo, bool isRevert);
	void addTileToMiddleOfGroupAndSplitFromMiddleOfGroupAndSplit(int sourceGroupId, int sourcePosition, int destinationGroupId, int targetPosition, GroupSplitInfo& groupSplitInfoSource, GroupSplitInfo& groupSplitInfoDestination, bool isRevert);

	//===============================================================================
	//=============== Revert Complex Operations
	//===============================================================================

	void revertAddTileToStartOfGroupFromStartOfGroup(int sourceGroupId, int destinationGroupId);
	void revertAddTileToStartOfGroupFromEndOfGroup(int sourceGroupId, int destinationGroupId);
	void revertAddTileToStartOfGroupFromMiddleOfGroup(int sourceGroupId, int sourcePosition, int destinationGroupId);
	void revertAddTileToStartOfGroupFromMiddleOfGroupAndSplit(int destinationGroupId, GroupSplitInfo& groupSplitInfo);
	void revertAddTileToEndOfGroupFromStartOfGroup(int sourceGroupId, int destinationGroupId);
	void revertAddTileToEndOfGroupFromEndOfGroup(int sourceGroupId, int destinationGroupId);
	void revertAddTileToEndOfGroupFromMiddleOfGroup(int sourceGroupId, int sourcePosition, int destinationGroupId);
	void revertAddTileToEndOfGroupFromMiddleOfGroupAndSplit(int destinationGroupId, GroupSplitInfo& groupSplitInfo);
	void revertAddTileToMiddleOfGroupAndSplitFromStartOfGroup(int sourceGroupId, int destinationGroupId, GroupSplitInfo& groupSplitInfo);
	void revertAddTileToMiddleOfGroupAndSplitFromEndOfGroup(int sourceGroupId, int destinationGroupId, GroupSplitInfo& groupSplitInfo);
	void revertAddTileToMiddleOfGroupAndSplitFromMiddleOfGroup(int sourceGroupId, int sourcePosition, int destinationGroupId, GroupSplitInfo& groupSplitInfo);
	void revertAddTileToMiddleOfGroupAndSplitFromMiddleOfGroupAndSplit(int sourceGroupId, int sourcePosition, int destinationGroupId, GroupSplitInfo& groupSplitInfoSource, GroupSplitInfo& groupSplitInfoDestination);
	void revertAddTileToMiddleOfGroupFromStartOfGroup(int sourceGroupId, int destinationGroupId, int destinationPosition);
	void revertAddTileToMiddleOfGroupFromEndOfGroup(int sourceGroupId, int destinationGroupId, int destinationPosition);
	void revertAddTileToMiddleOfGroupFromMiddleOfGroup(int sourceGroupId, int sourcePosition, int destinationGroupId, int destinationPosition);
	void revertAddTileToMiddleOfGroupFromMiddleOfGroupAndSplit(int sourceGroupId, int sourcePosition, int destinationGroupId, int destinationPosition, GroupSplitInfo& groupSplitInfo);

	//===============================================================================
	//=============== Other Operations
	//===============================================================================

	// Find runs on the board that can take the given tile as their new start tile.
	void findRunsForTileAtStart(std::shared_ptr<Tile> tile, int originalGroupId, std::vector<int>& groups) const;

	// Find runs on the board that can take the given tile as their new end tile.
	void findRunsForTileAtEnd(std::shared_ptr<Tile> tile, int originalGroupId, std::vector<int>& groups) const;

	// Find runs on the board that can take the given tile in the middle of the run, and split into two valid runs.
	void findRunsForTileInMiddleWithValidSplit(std::shared_ptr<Tile> tile, int originalGroupId, std::vector<int>& groups) const;

	// Find runs on the board that can take the given tile in the middle of the run, and split into two runs, where at least one of them is invalid.
	void findRunsForTileInMiddleWithInvalidSplit(std::shared_ptr<Tile> tile, int originalGroupId, std::vector<int>& groups) const;

	// Find sets on the board that can take the given tile and will not require splitting.
	void findSetsForTileWithoutSplit(std::shared_ptr<Tile> tile, int originalGroupId, std::vector<int>& groups) const;

	// Find runs or sets on the board that can take the given tile and, if a set, will not require splitting.
	void findGroupsForTile(std::shared_ptr<Tile> tile, int originalGroupId, std::vector<int>& groups) const;

	// Find groups on the board that can lose this tile and will not cause anything to become invalid.
	void findGroupsThatCanSafelyLoseThisTile(std::shared_ptr<Tile> tile, std::vector<int>& groups) const;

	// Find groups on the board that can lose this tile and will cause them to become invalid.
	void findGroupsThatCanUnsafelyLoseThisTile(std::shared_ptr<Tile> tile, std::vector<int>& groups) const;


//	std::shared_ptr<Tile> getTileFromGroup(int groupId, Tile& targetTile) const;

	void printStatus();
	void printStatus(std::ofstream& stream);

	bool checkInternalConsistency() const;
	bool validateAllGroups() const;

	size_t getHash() const;
};

} // namespace Tiles

namespace std
{

template <> class hash<Tiles::Board>
{
public :
	size_t operator()(const Tiles::Board& board ) const
	{
		return board.getHash();
	}
};

};



#endif /* INCLUDE_BOARD_H_ */
