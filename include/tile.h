/*
 * tile.h
 *
 *  Created on: Sep 4, 2017
 *      Author: eliha
 */

#ifndef INCLUDE_TILE_H_
#define INCLUDE_TILE_H_

#include <stdexcept>
#include <iostream>
#include <vector>
#include <memory>
#include "enums.h"

namespace Tiles
{

class Tile
{
public:
	Tile();
	Tile(int value, Color color);
	Tile(int id);
	Tile(Color color);

	int getValue() const { if (_value == -1) throw std::runtime_error("Attempt to get value of joker");  return _value; }
	Color getColor() const { return _color; }
	Color getJokerColor() const { if (_value != -1) throw std::runtime_error("Attempt to get joker color of non-joker"); return _jokerColor; }
	bool isJoker() const { return _value == -1; }
	bool isTrappedJoker() const { return (isJoker() && (numTrappedJokerValues() > 0)); }

	std::shared_ptr<Tile> getTrappedJokerValue(int i) { if (_value != -1) throw std::runtime_error("Attempt to get acting as tile of non-joker"); return _actingAsTiles[i]; }
	int numTrappedJokerValues() const { if (_value != -1) throw std::runtime_error("Attempt to get number of acting as tiles of non-joker"); return _actingAsTiles.size(); }
	void setJokerNotTrapped() { _actingAsTiles.clear(); }
	void trapJokerAs(int value, Color color) { _actingAsTiles.clear(); _actingAsTiles.push_back(std::make_shared<Tile>(value, color)); }
	void addTrappedJokerValue(int value, Color color) { _actingAsTiles.push_back(std::make_shared<Tile>(value, color)); }

	std::string str();
	std::string hashStr(int groupId, int position);

private:
	int _value;
	Color _color;
	Color _jokerColor;
	std::vector<std::shared_ptr<Tile>> _actingAsTiles;
};

} // namespace Tiles

#endif /* INCLUDE_TILE_H_ */
