/*
 * move.cpp
 *
 *  Created on: Sep 11, 2017
 *      Author: eliha
 */

//#ifdef WIN32
#include "stdafx.h"
//#endif

#include <sstream>
#include "move.h"

namespace Tiles
{

std::string Move::str()
{
	std::stringstream sstr;
	sstr << _tile->str();
	if (_source == Location::Rack)
	{
		sstr << " from rack";
	}
	else
	{
		sstr << " from group " << _sourceId;
	}
	sstr << " to ";
	if (_destination == Location::Board)
	{
		if (_moveDestinationType == MoveDestinationType::NewGroup)
		{
			sstr << "new group " << _destinationId;
		}
		else if (_moveDestinationType == MoveDestinationType::StartOfGroup)
		{
			sstr << "start of group " << _destinationId;
		}
		else if (_moveDestinationType == MoveDestinationType::EndOfGroup)
		{
			sstr << "end of group " << _destinationId;
		}
		else if (_moveDestinationType == MoveDestinationType::SafeMiddleOfGroup)
		{
			if (_groupIdsCreated.size() == 2)
			{
				sstr << "middle of group " << _destinationId <<", creating 2 new groups: " << _groupIdsCreated[0] << " and " << _groupIdsCreated[1];
			}
			else
			{
				sstr << "middle of group " << _destinationId <<", creating 2 new groups whose ids we dont know yet";
			}
		}
		else
		{
			sstr << " illegal";
		}
	}
	else
	{
		sstr << "illegal";
	}

	return sstr.str().c_str();
}

std::string Move::strWithSourceInfo()
{
	std::stringstream sstr;
	sstr << _tile->str();
	sstr << " from ";
	if (_source == Location::Rack)
	{
		sstr << "rack " << _sourceId << " position " << _sourceExtraId;
	}
	else if (_source == Location::Board)
	{
		if (_moveSourceType == MoveSourceType::StartOfGroup)
		{
			sstr << "start of group " << _sourceId;
		}
		else if (_moveSourceType == MoveSourceType::EndOfGroup)
		{
			sstr << "end of group " << _sourceId;
		}
		else if (_moveSourceType == MoveSourceType::SafeMiddleOfGroup)
		{
			if (_groupIdsCreated.size() == 2)
			{
				sstr << "middle of group " << _sourceId << ", creating 2 new groups: " << _groupIdsCreated[0] << " and " << _groupIdsCreated[1];
			}
			else
			{
				sstr << "middle of group " << _sourceId << ", creating 2 new groups whose ids we dont know yet";
			}
		}
		else
		{
			sstr << " illegal";
		}

	}
	else
	{
		sstr << "illegal";
	}
	sstr << " to ";
	if (_destination == Location::Board)
	{
		if (_moveDestinationType == MoveDestinationType::NewGroup)
		{
			sstr << "new group " << _destinationId;
		}
		else if (_moveDestinationType == MoveDestinationType::StartOfGroup)
		{
			sstr << "start of group " << _destinationId;
		}
		else if (_moveDestinationType == MoveDestinationType::EndOfGroup)
		{
			sstr << "end of group " << _destinationId;
		}
		else if (_moveDestinationType == MoveDestinationType::SafeMiddleOfGroup)
		{
			if (_groupIdsCreated.size() == 2)
			{
				sstr << "middle of group " << _destinationId << ", creating 2 new groups: " << _groupIdsCreated[0] << " and " << _groupIdsCreated[1];
			}
			else
			{
				sstr << "middle of group " << _destinationId << ", creating 2 new groups whose ids we dont know yet";
			}
		}
		else
		{
			sstr << " illegal";
		}
	}
	else
	{
		sstr << "illegal";
	}

	return sstr.str().c_str();
}

} // namespace Tiles
