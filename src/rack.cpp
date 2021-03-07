/*
 * rack.cpp
 *
 *  Created on: Sep 7, 2017
 *      Author: eliha
 */

//#ifdef WIN32
#include "stdafx.h"
//#endif

#include <iostream>
#include <fstream>

#include <iostream>
#include <sstream>
#include <algorithm>
#include "rack.h"
#include "group.h"

namespace Tiles
{

void Rack::addTile(std::shared_ptr<Tile> tile)
{
	_tiles.push_back(tile);
	_tileInUseFlags.push_back(false);
}

void Rack::addTile(std::shared_ptr<Tile> tile, int position)
{
	_tiles.insert(_tiles.begin() + position, tile);
	_tileInUseFlags.insert(_tileInUseFlags.begin() + position, false);
}

int Rack::numTiles() const
{
	return _tiles.size();
}

std::shared_ptr<Tile> Rack::getTile(int position) const
{
	return _tiles[position];
}

std::shared_ptr<Tile> Rack::takeTile(int position)
{
	if (_inTransaction)
	{
		_positionsToDelete.push_back(position);
	}

	if (_isSandboxState)
	{
		_tileInUseFlags[position] = true;
	}

	return _tiles[position];
}

void Rack::returnTile(int position)
{
	if (_inTransaction)
	{
		auto posToDelete = std::find(_positionsToDelete.begin(), _positionsToDelete.end(), position);
		if (posToDelete != _positionsToDelete.end())
		{
			_positionsToDelete.erase(posToDelete);
		}
	}

	if (_isSandboxState)
	{
		_tileInUseFlags[position] = false;
	}
}

void Rack::startTransaction()
{
_inTransaction = true;
_positionsToDelete.clear();
}

void Rack::startSandbox()
{
	_isSandboxState = true;
	for (unsigned int i = 0; i < _tileInUseFlags.size(); ++i)
	{
		_tileInUseFlags[i] = false;
	}
}

void Rack::commitTransaction()
{
	if (_positionsToDelete.size() > 0)
	{
		std::sort(_positionsToDelete.begin(), _positionsToDelete.end());

		for (int i = _positionsToDelete.size() - 1; i >= 0; --i)
		{
			_tiles.erase(_tiles.begin() + _positionsToDelete[i]);
			_tileInUseFlags.erase(_tileInUseFlags.begin() + _positionsToDelete[i]);
		}
	}

	_inTransaction = false;
}

void Rack::endSandbox()
{
	_isSandboxState = false;
}

bool Rack::isTileInUse(int position) const
{
	return _tileInUseFlags[position];
}

int Rack::numTilesInUse() const
{
	int total = 0;
	for (unsigned int i = 0; i < _tileInUseFlags.size(); ++i)
	{
		if (_tileInUseFlags[i])
		{
			++total;
		}
	}
	return total;
}

int Rack::numTilesNotInUse() const
{
	return numTiles() - numTilesInUse();
}

int Rack::firstTilePositionNotInUse(int position) const
{
	int pos = position;
	while ((unsigned)pos < _tileInUseFlags.size() && _tileInUseFlags[pos])
	{
		++pos;
	}

	return ((unsigned)pos >= _tileInUseFlags.size()) ? -1 : pos;
}

void Rack::getTilesInUse(std::vector<std::shared_ptr<Tile>>& tilesInUse) const
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

int Rack::findUnusedTile(Tile& targetTile) const
{
	for (unsigned int i = 0; i < _tiles.size(); ++i)
	{
		if (!_tileInUseFlags[i] &&
			((!targetTile.isJoker() && !_tiles[i]->isJoker() && _tiles[i]->getValue() == targetTile.getValue() && _tiles[i]->getColor() == targetTile.getColor()) ||
			(targetTile.isJoker() && _tiles[i]->isJoker() && _tiles[i]->getJokerColor() == targetTile.getJokerColor())))
		{
			return i;
		}
	}

	return -1;
}

int Rack::findUnusedTileExcludingColors(int value, std::vector<Color> colorsToExclude) const
{
	for (unsigned int i = 0; i < _tiles.size(); ++i)
	{
		if (_tiles[i]->isJoker())
		{
			continue;
		}
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

void Rack::setTileUnused(int position)
{
	_tileInUseFlags[position] = false;
}

void Rack::getAllUnusedTilePositions(std::vector<int>& unusedTilePositions) const
{
	for (unsigned int i = 0; i < _tileInUseFlags.size(); ++i)
	{
		if (!_tileInUseFlags[i])
		{
			unusedTilePositions.push_back(i);
		}
	}
}

void Rack::getAllUnusedJokerPositions(std::vector<int>& unusedJokerPositions) const
{
	for (unsigned int i = 0; i < _tileInUseFlags.size(); ++i)
	{
		if (_tiles[i]->isJoker() && !_tileInUseFlags[i])
		{
			unusedJokerPositions.push_back(i);
		}
	}
}

void Rack::getAllUnusedNonJokerPositions(std::vector<int>& unusedNonJokerPositions) const
{
	for (unsigned int i = 0; i < _tileInUseFlags.size(); ++i)
	{
		if (!_tiles[i]->isJoker() && !_tileInUseFlags[i])
		{
			unusedNonJokerPositions.push_back(i);
		}
	}
}

std::string Rack::getTilesUsedStr() const
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

void Rack::printStatus() const
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

void Rack::printStatus(std::ofstream& stream) const
{
	for (unsigned int i = 0; i < _tiles.size(); ++i)
	{
		stream << _tiles[i]->str();
		if (i < _tiles.size() - 1)
		{
			stream << " ";
		}
	}
	stream << std::endl;
}

} // namespace Tiles


