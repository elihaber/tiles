/*
 * utils.h
 *
 *  Created on: Sep 7, 2017
 *      Author: eliha
 */

#ifndef INCLUDE_UTILS_H_
#define INCLUDE_UTILS_H_

#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "move.h" // need to include this before defs.h because defs.h uses Move as a forward declaration. TODO: Fix.
#include "defs.h"

namespace Tiles
{

class Solution;

class Utils
{
public:

#ifdef WIN32
	static void initRand() { srand(static_cast<unsigned int>(time(NULL))); }
#else
	static void initRand() { srand(time(NULL)); }
#endif

	static int getRand(int max) { return rand() % max; }
	static int calculateSolutionScore(const Solution& solution);
	static void getColorsNotInSet(const std::vector<Color>& existingColors, std::vector<Color>& missingColors);
};

} // namespace Tiles




#endif /* INCLUDE_UTILS_H_ */
