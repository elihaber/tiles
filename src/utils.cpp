/*
 * utils.cpp
 *
 *  Created on: Sep 7, 2017
 *      Author: eliha
 */

//#ifdef WIN32
#include "stdafx.h"
//#endif

#include "utils.h"
#include "solution.h"

namespace Tiles
{

int Utils::calculateSolutionScore(const Solution& solution)
{
	int total = 0;
	for (int i = 0; i < solution.numMoves(); ++i)
	{
		const auto move = solution.getMove(i);
		if (move->_source != Location::Rack)
		{
			continue;
		}
		if (move->_tile->isJoker())
		{
			total += 30;
		}
		else
		{
			total += move->_tile->getValue();
		}
	}
	return total;
}

void Utils::getColorsNotInSet(const std::vector<Color>& existingColors, std::vector<Color>& missingColors)
{
	std::map<Color, bool> colorsFound;
	colorsFound.insert(std::make_pair(Color::Blue, false));
	colorsFound.insert(std::make_pair(Color::Black, false));
	colorsFound.insert(std::make_pair(Color::Yellow, false));
	colorsFound.insert(std::make_pair(Color::Red, false));
	for (auto color: existingColors)
	{
		colorsFound[color] = true;
	}
	for (auto iter = colorsFound.begin(); iter != colorsFound.end(); ++iter)
	{
		if (!iter->second)
		{
			missingColors.push_back(iter->first);
		}
	}
}

} // namespace Tiles
