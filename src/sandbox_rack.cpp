/*
 * sandbox_rack.cpp
 *
 *  Created on: Sep 12, 2017
 *      Author: eliha
 */

//#ifdef WIN32
#include "stdafx.h"
//#endif

#include <iostream>
#include <sstream>
#include "rack.h"
#include "sandbox_rack.h"
#include "pool.h"
#include "group.h"
#include "board.h"

namespace Tiles
{

SandboxRack::SandboxRack(Rack& rack) : Rack(rack.id(), rack.pool(), rack.board())
{
	for (int i = 0; i < rack.numTiles(); ++i)
	{
		addTile(rack.getTile(i));
	}
}

SandboxRack::SandboxRack(Rack& rack, std::shared_ptr<Board> board) : Rack(rack.id(), rack.pool(), board)
{
	for (int i = 0; i < rack.numTiles(); ++i)
	{
		addTile(rack.getTile(i));
	}
}

void SandboxRack::addTile(std::shared_ptr<Tile> tile)
{
	_tiles.push_back(tile);
	_tileInUseFlags.push_back(false);
}

void SandboxRack::addTile(std::shared_ptr<Tile> tile, int position)
{
	_tiles.insert(_tiles.begin() + position, tile);
	_tileInUseFlags.insert(_tileInUseFlags.begin() + position, false);
}

bool SandboxRack::isTileInUse(int position) const
{
	return _tileInUseFlags[position];
}

int SandboxRack::numTilesInUse() const
{
	return _tileInUseFlags.size();
}

int SandboxRack::numTilesNotInUse() const
{
	return numTiles() - numTilesInUse();
}

int SandboxRack::firstTilePositionNotInUse(int position) const
{
	int pos = position;
	while ((unsigned)pos < _tileInUseFlags.size() && _tileInUseFlags[pos])
	{
		++pos;
	}

	return ((unsigned)pos == _tileInUseFlags.size()) ? -1 : pos;
}

void SandboxRack::getTilesInUse(std::vector<std::shared_ptr<Tile>>& tilesInUse) const
{
	tilesInUse.clear();
	for (unsigned int i = 0; i < _tileInUseFlags.size(); ++i)
	{
		if (_tileInUseFlags[i])
		{
			tilesInUse.push_back(getTile(i));
		}
	}
}

int SandboxRack::startNewGroupOnBoardWithTileInPosition(int position)
{
	_tileInUseFlags[position] = true;
	return _board->createNewGroup(getTile(position));
}

void SandboxRack::removeTileFromBoardGroupBackToPosition(int groupId, int position)
{
	_board->removeGroup(_board->findGroupById(groupId));
	_tileInUseFlags[position] = false;
}

int SandboxRack::findUnusedTile(Tile& targetTile) const
{
	for (unsigned int i = 0; i < _tiles.size(); ++i)
	{
		if (!_tileInUseFlags[i] && _tiles[i]->getValue() == targetTile.getValue() && _tiles[i]->getColor() == targetTile.getColor())
		{
			return i;
		}
	}

	return -1;
}

int SandboxRack::findUnusedTileExcludingColors(int value, std::vector<Color> colorsToExclude) const
{
	for (unsigned int i = 0; i < _tiles.size(); ++i)
	{
		if (_tiles[i]->getValue() != value || _tileInUseFlags[i])
		{
			continue;
		}

		// All this shit because find doesn't compile (see below)
		bool found = false;
		for (auto color : colorsToExclude)
		{
			if (color == _tiles[i]->getColor())
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			return i;
		}

		/* find doesn't compile for some reason
		if (std::find(colorsToExclude.begin(), colorsToExclude.end(), _tiles[i].getColor()) == colorsToExclude.end())
		{
			return i;
		}
		*/
	}

	return -1;

}

void SandboxRack::setTileUnused(int position)
{
	_tileInUseFlags[position] = false;
}

void SandboxRack::printStatus() const
{
	for (unsigned int i = 0; i < _tiles.size(); ++i)
	{
		std::cout << _tiles[i]->str();
		if (i < _tiles.size() - 1)
		{
			std::cout << " ";
		}
	}
	std::cout << std::endl;
}

std::string SandboxRack::getTilesUsedStr() const
{
	std::stringstream sstr;
	for (unsigned int i = 0; i < _tiles.size(); ++i)
	{
		if (_tileInUseFlags[i])
		{
			sstr << _tiles[i]->str() << " ";
		}
	}

	return sstr.str().c_str();
}

} // namespace Tiles


