/*
 * Tile.cpp
 *
 *  Created on: Sep 4, 2017
 *      Author: eliha
 */

//#ifdef WIN32
#include "stdafx.h"
//#endif

#include <sstream>
#include "tile.h"

namespace Tiles
{

Tile::Tile() : _value(-1), _color(Color::None), _jokerColor(Color::None)
{
}

Tile::Tile(int id)
{
	if (id == 104)
	{
		_jokerColor = Color::Red;
		_color = Color::None;
		_value = -1;
	}
	else if (id == 105)
	{
		_jokerColor = Color::Black;
		_color = Color::None;
		_value = -1;
	}
	else
	{
		_jokerColor = Color::None;
		_color = static_cast<Color>(id / 26 + 1);
		_value = id % 13 + 1;
	}
}

Tile::Tile(int value, Color color) : _value(value), _color(color), _jokerColor(Color::None)
{
}

Tile::Tile(Color color) : _value(-1), _color(Color::None), _jokerColor(color)
{
}

std::string Tile::str()
{
	std::stringstream sstr;
	if (_value != -1)
	{
		sstr << "(" << _value << "," << ColorStr[(int)_color] << ")";
	}
	else
	{
		sstr << "(" << ColorStr[(int)_jokerColor] << " Joker)";
	}

	return sstr.str().c_str();
}

std::string Tile::hashStr(int groupId, int position)
{
	return std::to_string(groupId) + "_" + std::to_string(position) + "_" + str();
	/*
	std::stringstream sstr;
	sstr << std::to_string(groupId) << "_" << std::to_string(position) << "_" << str();
	return sstr.str().c_str();
	*/
}

} // namespace Tiles
