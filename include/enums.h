/*
 * enums.h
 *
 *  Created on: Sep 4, 2017
 *      Author: eliha
 */

#ifndef INCLUDE_ENUMS_H_
#define INCLUDE_ENUMS_H_

#include <string>

namespace Tiles
{

enum class Color { None, Black, Red, Blue, Yellow };
static std::string ColorStr[] = { "None", "Black", "Red", "Blue", "Yellow" };

enum class GroupType { None, Run, Set };
static std::string GroupTypeStr[] = { "None", "Run", "Set" };

enum class Location { None, Rack, Board };
static std::string LocationStr[] = { "None", "Rack", "Board" };

enum class MoveDestinationType { None, StartOfGroup, EndOfGroup, MiddleOfGroup, MiddleOfGroupAndSplit, NewGroup, Any };
static std::string MoveDestinationTypeStr[] = { "None", "StartOfGroup", "EndOfGroup", "MiddleOfGroup", "MiddleOfGroupAndSplit", "NewGroup", "Any" };

enum class MoveSourceType { None, Rack, StartOfGroup, EndOfGroup, MiddleOfGroup, MiddleOfGroupAndSplit };
static std::string MoveSourceTypeStr[] = { "None", "Rack", "StartOfGroup", "EndOfGroup", "MiddleOfGroup", "MiddleOfGroupAndSplit" };

enum class SolutionType { None, TEMPORARY, ACTUAL };
static std::string SolutionTypeStr[] = { "None", "Temporary", "Actual" };

enum class GroupRedirectionType { None, CreatedFromTile, CreatedFirstFromSplitSource, CreatedSecondFromSplitSource, CreatedFirstFromSplitTarget, CreatedSecondFromSplitTarget };
static std::string GroupRedirectionTypeStr[] = { "None", "CreatedFromTile", "CreatedFirstFromSplitSource", "CreatedSecondFromSplitSource", "CreatedFirstFromSplitTarget", "CreatedSecondFromSplitTarget" };

enum class StopReason { None, NoAttemptToPassLevel, RackEmpty, TimeoutInMiddle, TimeoutAtEnd, CompletedMaxLevel };
static std::string StopReasonStr[] = { "None", "NoAttemptToPassLevel", "RackEmpty", "TimeoutInMiddle", "TimeoutAtEnd", "CompletedMaxLevel" };

enum class StackStopReason { None, AttemptedToPassLevel, TimedOut, RackEmpty, AlreadyAnalyzed };
static std::string StackStopReasonStr[] = { "None", "AttemptedToPassLevel", "TimedOut", "RackEmpty", "AlreadyAnalyzed" };

enum class RunSplitType { None, AddTileToEndOfFirstRun, AddTileToStartOfSecondRun };
static std::string RunSplitTypeStr[] = { "None", "AddTileToEndOfFirstRun", "AddTileToStartOfSecondRun" };

enum class AceFreeingMethod { None, OnlyWithReplacementCard, AnytimeSetRemainsValid };
static std::string AceFreeingMethodStr[] = { "None", "OnlyWithReplacementCard", "AnytimeSetRemainsValid" };

} // namespace Tiles

#endif /* INCLUDE_ENUMS_H_ */
