/*
 * Rack.h
 *
 *  Created on: Sep 7, 2017
 *      Author: eliha
 */

#ifndef INCLUDE_RACK_H_
#define INCLUDE_RACK_H_

#include <iostream>
#include <fstream>

#include <memory>
#include <vector>
#include "tile.h"

namespace Tiles
{

class Rack
{
private:
	std::vector<int> _positionsToDelete;
	bool _inTransaction;

protected:
	int _id;
	std::vector<std::shared_ptr<Tile>> _tiles;

	// Sandbox members

	bool _isSandboxState;
	std::vector<bool> _tileInUseFlags;

public:
	Rack(int id) : _inTransaction(false), _id(id), _isSandboxState(false) { }
	virtual ~Rack() { }

	std::vector<std::shared_ptr<Tile>> tiles() { return _tiles; }

	virtual void addTile(std::shared_ptr<Tile> tile);
	virtual void addTile(std::shared_ptr<Tile> tile, int position);

	int id() const { return _id; }
	int numTiles() const;
	std::shared_ptr<Tile> getTile(int position) const;
	std::shared_ptr<Tile> takeTile(int position);
	void returnTile(int position);

	virtual void startTransaction();
	virtual void commitTransaction();
	void startSandbox();
	void endSandbox();

	std::string getTilesUsedStr() const;
	virtual void printStatus() const;
	virtual void printStatus(std::ofstream& stream) const;

	// Sandbox functions

	bool isTileInUse(int position) const;
	int numTilesInUse() const;
	int numTilesNotInUse() const;
	int firstTilePositionNotInUse(int position = -1) const;
	int findUnusedTile(Tile& targetTile) const;
	int findUnusedTileExcludingColors(int value, std::vector<Color> colorsToExclude) const;
	void getTilesInUse(std::vector<std::shared_ptr<Tile>>& tilesInUse) const;
	void setTileUnused(int position);
	void getAllUnusedTilePositions(std::vector<int>& unusedTilePositions) const;
	void getAllUnusedJokerPositions(std::vector<int>& unusedJokerPositions) const;
	void getAllUnusedNonJokerPositions(std::vector<int>& unusedNonJokerPositions) const;

};

} // namespace Tiles

#endif /* INCLUDE_RACK_H_ */
