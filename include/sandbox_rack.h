/*
 * sandbox_rack.h
 *
 *  Created on: Sep 12, 2017
 *      Author: eliha
 */

#ifndef INCLUDE_SANDBOX_RACK_H_
#define INCLUDE_SANDBOX_RACK_H_

#include <memory>
#include <vector>
#include "rack.h"

namespace Tiles
{

class Pool;
class Board;

class SandboxRack : public Rack
{
private:
	std::vector<bool> _tileInUseFlags;


public:
	SandboxRack(Rack& rack);
	SandboxRack(Rack& rack, std::shared_ptr<Board> board);
	virtual ~SandboxRack() { }

	virtual void addTile(std::shared_ptr<Tile> tile);
	virtual void addTile(std::shared_ptr<Tile> tile, int position);

	bool isTileInUse(int position) const;
	int numTilesInUse() const;
	int numTilesNotInUse() const;
	int firstTilePositionNotInUse(int position = -1) const;
	int findUnusedTile(Tile& targetTile) const;
	int findUnusedTileExcludingColors(int value, std::vector<Color> colorsToExclude) const;
	void getTilesInUse(std::vector<std::shared_ptr<Tile>>& tilesInUse) const;

	virtual int startNewGroupOnBoardWithTileInPosition(int position);
	virtual void removeTileFromBoardGroupBackToPosition(int groupId, int position);
	virtual void setTileUnused(int position);

	virtual void startTransaction() { }
	virtual void commitTransaction() { }

	virtual void printStatus() const;
	std::string getTilesUsedStr() const;

};

} // namespace Tiles

#endif /* INCLUDE_SANDBOX_RACK_H_ */
