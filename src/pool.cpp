/*
 * pool.cpp
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
#include "pool.h"
#include "utils.h"
#include "tile.h"

using namespace std;

namespace Tiles
{

Pool::Pool()
{
	reset();
}

void Pool::reset()
{
	_tilesRemaining.clear();
	_tilesTaken.clear();

	for (int i = 0; i < NumStartingTilesInPool; ++i)
	{
		_tilesRemaining.push_back(i);
	}
}

std::shared_ptr<Tile> Pool::takeRandomTile()
{
	int tileIndexToTake = Utils::getRand(_tilesRemaining.size());
	int tileIdToTake = _tilesRemaining.at(tileIndexToTake);
	_tilesTaken.push_back(tileIdToTake);
	_tilesRemaining.erase(_tilesRemaining.begin() + tileIndexToTake);
	return std::make_shared<Tile>(tileIdToTake);
}

void Pool::printStatus()
{
	cout << "Pool: Num tiles taken: " << numTilesTaken() << endl;
	cout << "Pool: Num tiles remaining: " << numTilesRemaining() << endl;
}

void Pool::printStatus(std::ofstream& stream)
{
	stream << "Pool: Num tiles taken: " << numTilesTaken() << endl;
	stream << "Pool: Num tiles remaining: " << numTilesRemaining() << endl;
}

} // namespace Tiles


