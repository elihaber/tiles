/*
 * solution.cpp
 *
 *  Created on: Sep 14, 2017
 *      Author: eliha
 */

//#ifdef WIN32
#include "stdafx.h"
//#endif

#include <iostream>
#include <fstream>

#include <map>
#include "solution.h"
#include "move.h"
#include "rack.h"
#include "board.h"
#include "group.h"
#include "mover.h"
#include "defs.h"

namespace Tiles
{

Solution::Solution(Rack& rack, Board& board, bool debug, bool debugMoves) : _debug(debug), _debugMoves(debugMoves), _board(board)
{
	_mover = std::make_shared<Mover>(rack, board, debugMoves);
}

Solution::Solution(Rack& rack, Board& board, std::vector<std::shared_ptr<Moove>>& moves) : _debug(false), _debugMoves(false), _board(board)
{
	for (unsigned int i = 0; i < moves.size(); ++i)
	{
		_moves.push_back(moves[i]);
	}
}

void Solution::applyMove(std::shared_ptr<Moove> move)
{
	_mover->doMove(*move);
}

void Solution::applyNewestMove()
{
	applyMove(_moves[_moves.size() - 1]);
}

void Solution::applyAllMoves()
{
	for (unsigned int i = 0; i < _moves.size(); ++i)
	{
		applyMove(_moves[i]);
	}
}

void Solution::unapplyAllMoves()
{
	for (unsigned int i = _moves.size() - 1; i >= 0; --i)
	{
		_mover->undoMove(*_moves[i]);
	}
}

void Solution::unapplyNewestMove()
{
	auto move = _moves[_moves.size() - 1];

	_mover->undoMove(*move);
}

void Solution::addMove(std::shared_ptr<Moove> move)
{
	if (_debug)
	{
		std::cout << "Solution";
		std::cout << ": Adding move: " << move->str() << std::endl;
	}
	_moves.push_back(move);
}

void Solution::removeNewestMove()
{
	if (_debug)
	{
		std::cout << "Solution";
		std::cout << ": Removing move: " << _moves[_moves.size() - 1]->str() << std::endl;
	}
	_moves.erase(_moves.end() - 1);
}

int Solution::score()
{
	int score = 0;
	for (unsigned int i = 0; i < _moves.size(); ++i)
	{
		if (_moves[i]->_source == Location::Rack)
		{
			auto tile = _moves[i]->_tile;
			score += (tile->isJoker() ? 30 : tile->getValue());
		}
	}
	return score;
}

void Solution::print() const
{
	for (unsigned int i = 0; i < _moves.size(); ++i)
	{
		std::cout << _moves[i]->str() << std::endl;
	}

}

void Solution::print(std::ofstream& stream) const
{
	for (unsigned int i = 0; i < _moves.size(); ++i)
	{
		stream << _moves[i]->str() << std::endl;
	}

}

void Solution::printWithName() const
{
	std::cout << "Solution";
	std::cout << ": moves:" << std::endl;

	print();
}



} // namespace Tiles



