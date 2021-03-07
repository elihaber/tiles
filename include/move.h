/*
 * move.h
 *
 *  Created on: Sep 10, 2017
 *      Author: eliha
 */

#ifndef INCLUDE_MOVE_H_
#define INCLUDE_MOVE_H_

#include <memory>
#include <vector>
#include "tile.h"

namespace Tiles
{

class Pool;
class Board;

class Move
{
private:
	Location _source;
	Location _destination;
	int _sourceId;
	int _sourceExtraId;
	int _destinationId;
	int _destinationExtraId;
	MoveSourceType _moveSourceType;
	MoveDestinationType _moveDestinationType;
	std::vector<int> _groupIdsCreated;
	std::shared_ptr<Tile> _tile;

public:
	Move(Location source, Location destination,
			int sourceId, int sourceExtraId,
			int destinationId, int destinationExtraId,
			MoveSourceType moveSourceType,
			MoveDestinationType moveDestinationType,
			std::shared_ptr<Tile> tile) :
				_source(source), _destination(destination), _sourceId(sourceId), _sourceExtraId(sourceExtraId),
				_destinationId(destinationId), _destinationExtraId(destinationExtraId),
				_moveSourceType(moveSourceType), _moveDestinationType(moveDestinationType), _tile(tile)
	{

	}

	std::shared_ptr<Tile> tile() const { return _tile; }
	MoveSourceType moveSourceType() const { return _moveSourceType; }
	MoveDestinationType moveDestinationType() const { return _moveDestinationType; }
	int sourceId() const { return _sourceId; }
	int sourceExtraId() const { return _sourceExtraId; }
	int destinationId() const { return _destinationId; }
	int destinationExtraId() const { return _destinationExtraId; }
	std::vector<int> groupIdsCreated() { return _groupIdsCreated; }
	Location source() const { return _source; }
	void setDestinationId(int val) { _destinationId = val; }
	void addGroupIdCreated(int id) { _groupIdsCreated.push_back(id); }

	std::string str();
	std::string strWithSourceInfo();

};

} // namespace Tiles





#endif /* INCLUDE_MOVE_H_ */
