/*
 * Pool.h
 *
 *  Created on: Sep 7, 2017
 *      Author: eliha
 */

#ifndef INCLUDE_POOL_H_
#define INCLUDE_POOL_H_

#include <iostream>
#include <fstream>

#include <memory>
#include <vector>

namespace Tiles
{

class Tile;

class Pool
{
private:
	std::vector<int> _tilesTaken;
	std::vector<int> _tilesRemaining;

public:
	Pool();
	void reset();
	int numTilesRemaining() { return _tilesRemaining.size(); }
	int numTilesTaken() { return _tilesTaken.size(); }
	std::shared_ptr<Tile> takeRandomTile();
	void printStatus();
	void printStatus(std::ofstream& stream);
	static const int NumStartingTilesInPool = 106;
};

} // namespace Tiles



#endif /* INCLUDE_POOL_H_ */
