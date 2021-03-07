/*
 * group.cpp
 *
 *  Created on: Sep 7, 2017
 *      Author: eliha
 */

#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include "group.h"
#include "utils.h"

namespace Tiles
{

Group::Group() : _id(-1), _groupType(GroupType::None), _isValid(true), _tryMovingToOtherGroups(true)
{
	calculateHash();
	calculateSummary();
}

Group::Group(Group& group) : _id(-1), _groupType(group.groupType()), _isValid(group.isValid()), _tryMovingToOtherGroups(true)
{
	for (int i = 0; i < group.numTiles(); ++i)
	{
		_tiles.push_back(group.getTile(i));
	}
	calculateHash();
	calculateSummary();
}

Group::Group(std::shared_ptr<Tile> tile) : _id(-1), _groupType(GroupType::None), _isValid(false), _tryMovingToOtherGroups(true)
{
	_tiles.push_back(tile);
	calculateHash();
	calculateSummary();
}

void Group::calculateHash()
{
	std::string str;
	if (_tiles.size() == 0)
	{
		str = "empty";
	}
	else if (_tiles.size() == 1)
	{
		if (_tiles[0]->isJoker())
		{
			str = "single_" + ColorStr[(int)_tiles[0]->getJokerColor()] + "Joker";
		}
		else
		{
			str = "single_" + ColorStr[(int)_tiles[0]->getColor()] + std::to_string(_tiles[0]->getValue());
		}
	}
	else
	{
		if (isSet())
		{
			str = "set_" + std::to_string(getSetValue());
			if (setContainsColor(Color::Black))
			{
				str += "Black";
			}
			if (setContainsColor(Color::Red))
			{
				str += "Red";
			}
			if (setContainsColor(Color::Blue))
			{
				str += "Blue";
			}
			if (setContainsColor(Color::Yellow))
			{
				str += "Yellow";
			}
			for (int i = 0; i < numJokers(); ++i)
			{
				str += "Joker";
			}
		}
		else if (isRun())
		{
			str = "run_" + ColorStr[(int)getRunColor()] + std::to_string(getRunStartValue()) + std::to_string(getRunEndValue());
			for (int i = 0; i < numJokers(); ++i)
			{
				str += "Joker";
			}
		}
		else
		{
			str = "none_";
			for (int i = 0; i < numTiles(); ++i)
			{
				str += _tiles[i]->str();
			}
		}
	}

	_hash = std::hash<std::string>()(str);
}

std::shared_ptr<Group> Group::createRunFromTwoRuns(std::shared_ptr<Group> group1, std::shared_ptr<Group> group2)
{
	auto group = std::make_shared<Group>();

	for (int i = 0; i < group1->numTiles() - 1; ++i)
	{
		group->addTileToEnd(group1->getTile(i));
	}
	for (int i = 0; i < group2->numTiles(); ++i)
	{
		group->addTileToEnd(group2->getTile(i));
	}
	group->setGroupType(GroupType::Run);

	return group;
}

std::shared_ptr<Group> Group::createRunFromTwoRunsAndTile(std::shared_ptr<Group> group1, std::shared_ptr<Group> group2, std::shared_ptr<Tile> tile)
{
	auto group = std::make_shared<Group>();

	for (int i = 0; i < group1->numTiles(); ++i)
	{
		group->addTileToEnd(group1->getTile(i));
	}
	group->addTileToEnd(tile);
	for (int i = 0; i < group2->numTiles(); ++i)
	{
		group->addTileToEnd(group2->getTile(i));
	}
	group->setGroupType(GroupType::Run);

	return group;
}

void Group::trapJokers()
{
	if (_tiles.size() < 3 && _tiles.size() != 0)
	{
		throw std::runtime_error("Tried to trap jokers for group with fewer than 3 tiles");
	}

	_trappedJokerPositions.clear();

	for (auto pos : _jokerPositions)
	{
		_trappedJokerPositions.push_back(pos);

		if (isRun())
		{
			_tiles[pos]->trapJokerAs(getRunStartValue() + pos, getRunColor());
		}
		else if (isSet())
		{
			std::vector<Color> colors;
			getAllColorsNotInSet(colors);

			if (colors.size() == 1)
			{
				_tiles[pos]->trapJokerAs(getSetValue(), colors[0]);
			}

			_tiles[pos]->setJokerNotTrapped();
			for (auto color : colors)
			{
				_tiles[pos]->addTrappedJokerValue(getSetValue(), color);
			}
		}
		else
		{
			throw std::runtime_error("Tried to trap joker in group that is not set and not run");
		}
	}

}

void Group::untrapJokers()
{
	for (auto pos : _trappedJokerPositions)
	{
		_tiles[pos]->setJokerNotTrapped();
	}
	_trappedJokerPositions.clear();
}

void Group::calculateType()
{
	int val1 = 0;
	int val2 = 0;
	int pos = 0;

	while (val1 == 0 && pos < numTiles())
	{
		if (!_tiles[pos]->isJoker())
		{
			val1 = _tiles[pos]->getValue();
		}
		else if (_tiles[pos]->isTrappedJoker())
		{
			val1 = _tiles[pos]->getTrappedJokerValue(0)->getValue();
		}
		++pos;
	}
	while (val2 == 0 && pos < numTiles())
	{
		if (!_tiles[pos]->isJoker())
		{
			val2 = _tiles[pos]->getValue();
		}
		else if (_tiles[pos]->isTrappedJoker())
		{
			val2 = _tiles[pos]->getTrappedJokerValue(0)->getValue();
		}
		++pos;
	}
	if (val1 == 0 || val2 == 0)
	{
		setGroupType(GroupType::None);
	}
	else if (val1 == val2)
	{
		setGroupType(GroupType::Set);
	}
	else
	{
		setGroupType(GroupType::Run);
	}
}

void Group::calculateSummary()
{
	if (_groupType == GroupType::Run)
	{
		_setValue = -1;
		for (unsigned int i = 0; i < _tiles.size(); ++i)
		{
			if (_tiles[i]->isJoker())
			{
				continue;
			}
			_runStartValue = _tiles[i]->getValue() - i;
			_runEndValue = _tiles.size() - i - 1 + _tiles[i]->getValue();
			_runColor = _tiles[i]->getColor();
			break;
		}
	}
	else if (_groupType == GroupType::Set)
	{
		_runStartValue = -1;
		_runEndValue = -1;
		_runColor = Color::None;
		for (unsigned int i = 0; i < _tiles.size(); ++i)
		{
			if (_tiles[i]->isJoker())
			{
				continue;
			}
			_setValue = _tiles[i]->getValue();
			break;
		}
	}
	else
	{
		if (isOnlyJoker() || isOnlyTwoJokers())
		{
			_runStartValue = -1;
			_runEndValue = -1;
			_runColor = Color::None;
			_setValue = -1;
		}
		else
		{
			for (unsigned int i = 0; i < _tiles.size(); ++i)
			{
				if (_tiles[i]->isJoker())
				{
					continue;
				}
				_runStartValue = _tiles[i]->getValue() - i;
				_runEndValue = _tiles.size() - i - 1 + _tiles[i]->getValue();
				_runColor = _tiles[i]->getColor();
				_setValue = _tiles[i]->getValue();
				break;
			}
		}
	}
}

int Group::getTilePosition(Tile& tile) const
{
	for (int i = 0; i < (int)_tiles.size(); ++i)
	{
		if ((!tile.isJoker() && !_tiles[i]->isJoker() && _tiles[i]->getValue() == tile.getValue() && _tiles[i]->getColor() == tile.getColor()) ||
			(tile.isJoker() && _tiles[i]->isJoker() && _tiles[i]->getJokerColor() == tile.getJokerColor()))
		{
			return i;
		}
	}

	std::cout << "Looking for " << tile.str() << " but the tiles in this group are: ";
	printStatus();
	throw std::runtime_error("Expected to find position of tile");
}

void Group::getAllColorsInSet(std::vector<Color>& colors) const
{
	if (isRun())
	{
		throw new std::runtime_error("Tried to get colors in set, but group is a run");
	}

	for (auto tile : _tiles)
	{
		if (tile->isJoker())
		{
			continue;
		}
		colors.push_back(tile->getColor());
	}
}

void Group::getAllColorsNotInSet(std::vector<Color>& colors) const
{
	if (isRun())
	{
		throw std::runtime_error("Tried to get colors not in set, but group is a run");
	}

	colors.push_back(Color::Black);
	colors.push_back(Color::Red);
	colors.push_back(Color::Blue);
	colors.push_back(Color::Yellow);

	for (auto tile : _tiles)
	{
		if (tile->isJoker())
		{
			continue;
		}
		// Complicated because std::find does not work
		int pos = -1;
		for (unsigned int i = 0; i < colors.size(); ++i)
		{
			if (colors[i] == tile->getColor())
			{
				pos = i;
				break;
			}
		}
		if (pos != -1)
		{
			colors.erase(colors.begin() + pos);
		}
	}
}

bool Group::setContainsColor(Color color) const
{
	if (isRun())
	{
		throw new std::runtime_error("Tried to get set colors, but group is a run");
	}

	for (const auto & tile : _tiles)
	{
		if (tile->isJoker())
		{
			continue;
		}

		if (tile->getColor() == color)
		{
			return true;
		}
	}
	return false;
}

bool Group::isTileAtStartOfRun(std::shared_ptr<Tile> tile) const
{
	if (!isRun())
	{
		throw new std::runtime_error("Tried to check start of run from group that is not a run");
	}

	return ((!tile->isJoker() && !firstTile()->isJoker() && firstTile()->getValue() == tile->getValue()) || (tile->isJoker() && firstTile()->isJoker() && tile->getJokerColor() == firstTile()->getJokerColor()));
}

bool Group::isTileAtEndOfRun(std::shared_ptr<Tile> tile) const
{
	if (!isRun())
	{
		throw new std::runtime_error("Tried to check end of run from group that is not a run");
	}

	return ((!tile->isJoker() && !lastTile()->isJoker() && lastTile()->getValue() == tile->getValue()) || (tile->isJoker() && lastTile()->isJoker() && tile->getJokerColor() == lastTile()->getJokerColor()));
}

bool Group::isTileInMiddleOfRun(std::shared_ptr<Tile> tile) const
{
	if (!isRun())
	{
		throw new std::runtime_error("Tried to check middle of run from group that is not a run");
	}

	return (!isTileAtStartOfRun(tile) && !isTileAtEndOfRun(tile)); // TODO: Note that this assumes the tile is somewhere in the run!
}

bool Group::containsTile(std::shared_ptr<Tile> tile) const
{
	for (auto groupTile : _tiles)
	{
		if ((!tile->isJoker() && !groupTile->isJoker() && groupTile->getValue() == tile->getValue() && groupTile->getColor() == tile->getColor()) || (tile->isJoker() && groupTile->isJoker() && tile->getJokerColor() == groupTile->getJokerColor()))
		{
			return true;
		}
	}

	return false;
}

int Group::getSetValue() const
{
	if (isRun()) throw std::runtime_error("Tried to get value of set, but group is a run");
	if (numTiles() != 0 && numTiles() == numJokers() && numTrappedJokers() == 0) throw std::runtime_error("Tried to get set value for non-trapped-joker-only set");
	return _setValue;
}

Color Group::getRunColor() const
{
	if (isSet()) throw std::runtime_error("Tried to get color of run, but group is a set");
	if (numTiles() != 0 && numTiles() == numJokers() && numTrappedJokers() == 0) throw std::runtime_error("Tried to get run color for non-trapped-joker-only run");
	return _runColor;
}

int Group::getRunStartValue() const
{
	if (isSet()) throw std::runtime_error("Tried to get start value of run, but group is a set");
	if (numTiles() != 0 && numTiles() == numJokers() && numTrappedJokers() == 0) throw std::runtime_error("Tried to get run start value for non-trapped-joker-only run");
	return _runStartValue;
}

int Group::getRunEndValue() const
{
	if (isSet()) throw std::runtime_error("Tried to get end value of run, but group is a set");
	if (numTiles() != 0 && numTiles() == numJokers() && numTrappedJokers() == 0) throw std::runtime_error("Tried to get run end value for non-trapped-joker-only run");
	return _runEndValue;
}

// Finds the position in a run where a given tile can be added, splitting the run into two runs (no check here if they will be valid or not).
int Group::getTargetPositionInRunForTile(std::shared_ptr<Tile> tile) const
{
	if (!isRun())
	{
		throw std::runtime_error("Tried to search for target position in run from group that is not a run");
	}
	if (tile->isJoker())
	{
		throw std::runtime_error("Tried to search for target position in run with a joker");
	}
	if (_runStartValue > tile->getValue() || _runEndValue < tile->getValue())
	{
		throw std::runtime_error("Tried to search for target position in run with a tile that is out of range");
	}

	return tile->getValue() - _runStartValue;
}

void Group::addTileToStart(std::shared_ptr<Tile> tile)
{
	_tiles.insert(_tiles.begin(), tile);

	for (int i = 0; i < numJokers(); ++i)
	{
		_jokerPositions[i]++;
	}
	for (int i = 0; i < numTrappedJokers(); ++i)
	{
		_trappedJokerPositions[i]++;
	}
	if (tile->isJoker())
	{
		_jokerPositions.push_back(0);
	}
	if (tile->isTrappedJoker())
	{
		_trappedJokerPositions.push_back(0);
	}

	if (_groupType == GroupType::None)
	{
		calculateType();
	}
	calculateHash();
	calculateSummary();
	validateData();
}

void Group::addTileToEnd(std::shared_ptr<Tile> tile)
{
	_tiles.push_back(tile);

	if (tile->isJoker())
	{
		_jokerPositions.push_back(numTiles() - 1);
	}
	if (tile->isTrappedJoker())
	{
		_trappedJokerPositions.push_back(numTiles() - 1);
	}

	if (_groupType == GroupType::None)
	{
		calculateType();
	}
	calculateHash();
	calculateSummary();
	validateData();
}

void Group::addTileToMiddle(std::shared_ptr<Tile> tile, int position)
{
	_tiles.insert(_tiles.begin() + position, tile);

	for (int i = 0; i < numJokers(); ++i)
	{
		if (_jokerPositions[i] >= position)
		{
			_jokerPositions[i]++;
		}
	}
	for (int i = 0; i < numTrappedJokers(); ++i)
	{
		if (_trappedJokerPositions[i] >= position)
		{
			_trappedJokerPositions[i]++;
		}
	}
	if (tile->isJoker())
	{
		_jokerPositions.push_back(position);
	}
	if (tile->isTrappedJoker())
	{
		_trappedJokerPositions.push_back(position);
	}

	if (_groupType == GroupType::None)
	{
		calculateType();
	}
	calculateHash();
	calculateSummary();
	validateData();
}

void Group::addTileToMiddleAndSplit(std::shared_ptr<Tile> tile, int position, RunSplitType runSplitType, std::vector<std::shared_ptr<Group>>& groupsCreated)
{
	auto firstGroup = std::make_shared<Group>();
	auto secondGroup = std::make_shared<Group>();
	for (int i = 0; i < position; ++i)
	{
		firstGroup->addTileToEnd(_tiles[i]);
	}
	if (runSplitType == RunSplitType::AddTileToEndOfFirstRun)
	{
		firstGroup->addTileToEnd(tile);
		secondGroup->addTileToEnd(_tiles[position]);
	}
	else
	{
		firstGroup->addTileToEnd(_tiles[position]);
		secondGroup->addTileToEnd(tile);
	}
	for (unsigned int i = position + 1; i < _tiles.size(); ++i)
	{
		secondGroup->addTileToEnd(_tiles[i]);
	}

	groupsCreated.push_back(firstGroup);
	groupsCreated.push_back(secondGroup);

	calculateHash();
	calculateSummary();
	validateData();
}

void Group::removeFirstTile()
{
	_tiles.erase(_tiles.begin());

	auto firstTileJoker = std::find(_jokerPositions.begin(), _jokerPositions.end(), 0);
	if (firstTileJoker != _jokerPositions.end())
	{
		_jokerPositions.erase(firstTileJoker);
	}
	auto firstTileTrappedJoker = std::find(_trappedJokerPositions.begin(), _trappedJokerPositions.end(), 0);
	if (firstTileTrappedJoker != _trappedJokerPositions.end())
	{
		_trappedJokerPositions.erase(firstTileTrappedJoker);
	}
	for (int i = 0; i < numJokers(); ++i)
	{
		_jokerPositions[i]--;
	}
	for (int i = 0; i < numTrappedJokers(); ++i)
	{
		_trappedJokerPositions[i]--;
	}
	if (numNonJokers() == 0)
	{
		untrapJokers();
	}
	if ((_tiles.size() == 3 && numNonTrappedJokers() == 2) || (_tiles.size() == 2 && numNonTrappedJokers() >= 1) || (_tiles.size() < 2))
	{
		_groupType = GroupType::None;
	}
	calculateHash();
	calculateSummary();
	validateData();
}

void Group::removeLastTile()
{
	_tiles.erase(_tiles.end() - 1);

	auto lastTileJoker = std::find(_jokerPositions.begin(), _jokerPositions.end(), _tiles.size());
	if (lastTileJoker != _jokerPositions.end())
	{
		_jokerPositions.erase(lastTileJoker);
	}
	auto lastTileTrappedJoker = std::find(_trappedJokerPositions.begin(), _trappedJokerPositions.end(), _tiles.size());
	if (lastTileTrappedJoker != _trappedJokerPositions.end())
	{
		_trappedJokerPositions.erase(lastTileTrappedJoker);
	}
	if (numNonJokers() == 0)
	{
		untrapJokers();
	}
	if ((_tiles.size() == 3 && numNonTrappedJokers() == 2) || (_tiles.size() == 2 && numNonTrappedJokers() >= 1) || (_tiles.size() < 2))
	{
		_groupType = GroupType::None;
	}
	calculateHash();
	calculateSummary();
	validateData();
}

void Group::removeAllTiles()
{
	_tiles.clear();
	_jokerPositions.clear();
	_trappedJokerPositions.clear();
	_groupType = GroupType::None;
	calculateHash();
	calculateSummary();
	validateData();
}

void Group::removeOnlyTile()
{
	_tiles.clear();
	_jokerPositions.clear();
	_trappedJokerPositions.clear();
	_groupType = GroupType::None;
	calculateHash();
	calculateSummary();
	validateData();
}

void Group::removeTileFromMiddle(int position)
{
	_tiles.erase(_tiles.begin() + position);

	auto relevantJoker = std::find(_jokerPositions.begin(), _jokerPositions.end(), position);
	if (relevantJoker != _jokerPositions.end())
	{
		_jokerPositions.erase(relevantJoker);
	}
	const int a = position;
	auto relevantTrappedJoker = std::find(_trappedJokerPositions.begin(), _trappedJokerPositions.end(), a);
	if (relevantTrappedJoker != _trappedJokerPositions.end())
	{
		_trappedJokerPositions.erase(relevantTrappedJoker);
	}
	for (int i = 0; i < numJokers(); ++i)
	{
		if (_jokerPositions[i] >= position)
		{
			_jokerPositions[i]--;
		}
	}
	for (int i = 0; i < numTrappedJokers(); ++i)
	{
		if (_trappedJokerPositions[i] >= position)
		{
			_trappedJokerPositions[i]--;
		}
	}
	if (numNonJokers() == 0)
	{
		untrapJokers();
	}

	if ((_tiles.size() == 3 && numNonTrappedJokers() == 2) || (_tiles.size() == 2 && numNonTrappedJokers() >= 1) || (_tiles.size() < 2))
	{
		_groupType = GroupType::None;
	}

	calculateHash();
	calculateSummary();
	validateData();
}

void Group::removeTileFromMiddleAndSplit(int position, std::vector<std::shared_ptr<Group>>& groupsCreated)
{
	if (!isRun())
	{
		throw new std::runtime_error("Tried to remove from middle of run from group that is not a run");
	}

	auto firstGroup = std::make_shared<Group>();
	int pos = 0;
	while (pos < position)
	{
		firstGroup->addTileToEnd(_tiles[pos]);
		++pos;
	}

	auto secondGroup = std::make_shared<Group>();
	for (unsigned int i = pos + 1; i < _tiles.size(); ++i)
	{
		secondGroup->addTileToEnd(_tiles[i]);
	}

	groupsCreated.push_back(firstGroup);
	groupsCreated.push_back(secondGroup);

	calculateHash();
	calculateSummary();
	validateData();
}

void Group::validateData() const
{
	// Check that the _jokerPositions vector agrees with the actual tiles.
	for (unsigned int i = 0; i < _jokerPositions.size(); ++i)
	{
		int testPosition = _jokerPositions[i];
		if ((int)_tiles.size() <= testPosition)
		{
			throw std::runtime_error("Joker position is less than number of tiles");
		}
		if (!_tiles[testPosition]->isJoker())
		{
			throw std::runtime_error("Joker position is not actually a joker");
		}
	}

	// Check that the _trappedJokerPositions vector agrees with the actual tiles.
	for (unsigned int i = 0; i < _trappedJokerPositions.size(); ++i)
	{
		int testPosition = _trappedJokerPositions[i];
		if ((int)_tiles.size() <= testPosition)
		{
			throw std::runtime_error("Trapped joker position is less than number of tiles");
		}
		if (!_tiles[testPosition]->isJoker())
		{
			throw std::runtime_error("Trapped joker position is not actually a joker");
		}
		if (!_tiles[testPosition]->isTrappedJoker())
		{
			throw std::runtime_error("Trapped joker position is a joker but not trapped");
		}
	}

	// Check that the tiles that are jokers are also in the list of jokers.
	for (unsigned int i = 0; i < _tiles.size(); ++i)
	{
		if (_tiles[i]->isJoker())
		{
			auto lookForJoker = std::find(_jokerPositions.begin(), _jokerPositions.end(), i);
			if (lookForJoker == _jokerPositions.end())
			{
				throw std::runtime_error("Tile is joker but not in list of joker positions");
			}
		}
		if (_tiles[i]->isTrappedJoker())
		{
			auto lookForJoker = std::find(_jokerPositions.begin(), _jokerPositions.end(), i);
			if (lookForJoker == _jokerPositions.end())
			{
				throw std::runtime_error("Tile is trapped joker but not in list of joker positions");
			}
			auto lookForTrappedJoker = std::find(_trappedJokerPositions.begin(), _trappedJokerPositions.end(), i);
			if (lookForTrappedJoker == _trappedJokerPositions.end())
			{
				throw std::runtime_error("Tile is trapped joker but not in list of trapped joker positions");
			}
		}
	}
}

bool Group::validate() const
{
	if (numTiles() == 0)
	{
		if (groupType() != GroupType::None)
		{
			std::cout << "Group " << id() << " has zero tiles but group type " << GroupTypeStr[(int)groupType()] << std::endl;
			return false;
		}
	}
	if (numTiles() == 1)
	{
		if (groupType() != GroupType::None)
		{
			std::cout << "Group " << id() << " has one tile but group type " << GroupTypeStr[(int)groupType()] << std::endl;
			return false;
		}
	}

	GroupType identifiedType = GroupType::None;
	int nextExpectedValue = 0;
	Color nextExpectedColor = Color::None;
	int prevValue = 0;
	Color prevColor = Color::None;
	for (int i = 0; i < numTiles(); ++i)
	{
		bool isJoker = _tiles[i]->isJoker();
		bool isTrappedJoker = _tiles[i]->isTrappedJoker();

		int currentValue = 0;
		Color currentColor = Color::None;
		if (!isJoker)
		{
			currentValue = _tiles[i]->getValue();
			currentColor = _tiles[i]->getColor();
		}
		else if (isTrappedJoker)
		{
			currentValue = _tiles[i]->getTrappedJokerValue(0)->getValue();
		}

		if (nextExpectedValue != 0 && !isJoker && currentValue != nextExpectedValue)
		{
			std::cout << "Group " << id() << " has tile of value " << currentValue << " in position " << i << " when expecting tile of value " << nextExpectedValue << std::endl;
			return false;
		}
		if (nextExpectedColor != Color::None && !isJoker && currentColor != nextExpectedColor)
		{
			std::cout << "Group " << id() << " has tile of Color " << ColorStr[(int)currentColor] << " in position " << i << " when expecting tile of color " << ColorStr[(int)nextExpectedColor] << std::endl;
			return false;
		}
		if (!isJoker || isTrappedJoker)
		{
			if (prevValue != 0)
			{
				if (currentValue == prevValue + 1)
				{
					if (prevColor != Color::None && currentColor != Color::None && currentColor != prevColor)
					{
						std::cout << "Group " << id() << " has tile with value " + currentValue << " and color " << ColorStr[(int)currentColor] << " in position " << i << " after tile with value " << prevValue << " and color " << ColorStr[(int)prevColor] << std::endl;
						return false;
					}
					identifiedType = GroupType::Run;
					nextExpectedValue = currentValue + 1;
					nextExpectedColor = currentColor;
				}
				else if (currentValue == prevValue)
				{
					if (prevColor != Color::None && currentColor != Color::None && currentColor == prevColor)
					{
						std::cout << "Group " << id() << " has tile with value " + currentValue << " and color " << ColorStr[(int)currentColor] << " in position " << i << " after tile with value " << prevValue << " and color " << ColorStr[(int)prevColor] << std::endl;
						return false;
					}
					identifiedType = GroupType::Set;
					nextExpectedValue = currentValue;
					nextExpectedColor = Color::None;
				}
				else
				{
					std::cout << "Group " << id() << " has tile with value " + currentValue << " and color " << ColorStr[(int)currentColor] << " in position " << i << " after tile with value " << prevValue << " and color " << ColorStr[(int)prevColor] << std::endl;
					return false;
				}
			}
		}
		else
		{
			if (identifiedType == GroupType::Run)
			{
				nextExpectedValue = prevValue + 2;
				nextExpectedColor = prevColor;
			}
			else if (identifiedType == GroupType::Set)
			{
				nextExpectedValue = prevValue;
				nextExpectedColor = Color::None;
			}
		}
	}

	return true;
}

void Group::printStatus() const
{
	std::cout << "Group " << _id << ": ";
	for (unsigned int i = 0; i < _tiles.size(); ++i)
	{
		std::cout << _tiles[i]->str();
		if (i < _tiles.size() - 1)
		{
			std::cout << " ";
		}
	}
	std::cout << " " << GroupTypeStr[(int)_groupType];
	if (!isValid())
	{
		std::cout << " INVALID";
	}
	std::cout << std::endl;
}

void Group::printStatus(std::ofstream& stream) const
{
	stream << "Group " << _id << ": ";
	for (unsigned int i = 0; i < _tiles.size(); ++i)
	{
		stream << _tiles[i]->str();
		if (i < _tiles.size() - 1)
		{
			stream << " ";
		}
	}
	stream << " " << GroupTypeStr[(int)_groupType];
	if (!isValid())
	{
		stream << " INVALID";
	}
	stream << std::endl;
}

} // namespace Tiles
