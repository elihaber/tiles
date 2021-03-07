/*
 * group.h
 *
 *  Created on: Sep 7, 2017
 *      Author: eliha
 */

#ifndef INCLUDE_GROUP_H_
#define INCLUDE_GROUP_H_

#include <iostream>
#include <fstream>

#include <memory>
#include <vector>
#include "enums.h"
#include "tile.h"

namespace Tiles
{

class Group
{
private:
	int _id;
	std::vector<std::shared_ptr<Tile>> _tiles;
	GroupType _groupType;
	bool _isValid;
	size_t _hash;
	bool _tryMovingToOtherGroups;
	Color _runColor;
	int _runStartValue;
	int _runEndValue;
	int _setValue;
	std::vector<int> _jokerPositions;
	std::vector<int> _trappedJokerPositions;

	void calculateType();
	void calculateHash();
	void calculateSummary();

public:
	Group();
	Group(Group& group);
	Group(std::shared_ptr<Tile> tile);

	static std::shared_ptr<Group> createRunFromTwoRuns(std::shared_ptr<Group> group1, std::shared_ptr<Group> group2);
	static std::shared_ptr<Group> createRunFromTwoRunsAndTile(std::shared_ptr<Group> group1, std::shared_ptr<Group> group2, std::shared_ptr<Tile> tile);

	int       id() const                     { return _id; }
	GroupType groupType() const              { return _groupType; }
	bool      isSet() const                  { return (_groupType == GroupType::Set); }
	bool      isRun() const                  { return (_groupType == GroupType::Run); }
	bool      isNone() const                 { return (_groupType == GroupType::None); }
	size_t    getHash() const                { return _hash; }
	bool      tryMovingToOtherGroups() const { return _tryMovingToOtherGroups; }
	int       numTiles() const               { return _tiles.size(); }
	bool      isValid() const                { return (numTiles() >= 3 || numTiles() == 0); }
	int       numNonJokers() const           { return numTiles() - numJokers(); }
	int       numJokers() const              { return _jokerPositions.size(); }
	bool      hasJokers() const              { return (numJokers() > 0); }
	int       numTrappedJokers() const       { return _trappedJokerPositions.size(); }
	bool      hasTrappedJokers() const       { return (numTrappedJokers() > 0); }
	int       numNonTrappedJokers() const    { return numJokers() - numTrappedJokers(); }
	bool      isOnlyJoker() const            { return (numTiles() == 1 && numJokers() == 1); }
	bool      isOnlyTwoJokers() const        { return (numTiles() == 2 && numJokers() == 2); }
	bool      isOnlyJokers() const           { return (numTiles() > 0 && numTiles() == numJokers()); }

	std::shared_ptr<Tile> getTile(int position) const { return _tiles[position]; }
	std::shared_ptr<Tile> firstTile() const { return _tiles[0]; }
	std::shared_ptr<Tile> lastTile() const { return _tiles[_tiles.size() - 1]; }

	int getTilePosition(Tile& tile) const;
	void getAllColorsInSet(std::vector<Color>& colors) const;
	void getAllColorsNotInSet(std::vector<Color>& colors) const;
	bool setContainsColor(Color color) const;
	bool isTileAtStartOfRun(std::shared_ptr<Tile> tile) const;
	bool isTileAtEndOfRun(std::shared_ptr<Tile> tile) const;
	bool isTileInMiddleOfRun(std::shared_ptr<Tile> tile) const;
	bool containsTile(std::shared_ptr<Tile> tile) const;

	int getSetValue() const;
	Color getRunColor() const;
	int getRunStartValue() const;
	int getRunEndValue() const;

	// Finds the position in a run where a given tile can be added, splitting the run into two runs (no check here if they will be valid or not).
	int getTargetPositionInRunForTile(std::shared_ptr<Tile> tile) const;

	void setId(int id) { _id = id; }
	void setTryMovingToOtherGroups(bool val) { _tryMovingToOtherGroups = val; }
	void setGroupType(GroupType val) { _groupType = val; }

	void trapJokers();
	void untrapJokers();

	void addTileToStart(std::shared_ptr<Tile> tile);
	void addTileToEnd(std::shared_ptr<Tile> tile);
	void addTileToMiddle(std::shared_ptr<Tile> tile, int position);
	void addTileToMiddleAndSplit(std::shared_ptr<Tile> tile, int position, RunSplitType runSplitType, std::vector<std::shared_ptr<Group>>& groupsCreated);

	void removeFirstTile();
	void removeLastTile();
	void removeAllTiles();
	void removeOnlyTile();
	void removeTileFromMiddle(int position);
	void removeTileFromMiddle(std::shared_ptr<Tile> tile);
	void removeTileFromMiddleAndSplit(int position, std::vector<std::shared_ptr<Group>>& groupsCreated);
	void removeTileFromMiddleAndSplit(std::shared_ptr<Tile> tile, std::vector<std::shared_ptr<Group>>& groupsCreated);

	void validateData() const;
	bool validate() const;

	void printStatus() const;
	void printStatus(std::ofstream& stream) const;

};

} // namespace Tiles





#endif /* INCLUDE_GROUP_H_ */
