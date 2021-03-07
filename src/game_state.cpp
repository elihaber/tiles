/*
 * game_state.cpp
 *
 *  Created on: Sep 7, 2017
 *      Author: eliha
 */

#include <iostream>
#include <fstream>

//#ifdef WIN32
#include "stdafx.h"
//#endif

#include "game_state.h"
#include "pool.h"
#include "board.h"
#include "rack.h"
#include "player.h"
#include "utils.h"

namespace Tiles
{

void GameState::init(int numPlayers, bool withHuman)
{
	Utils::initRand();

	// TODO: Clear the pool and the board.

	_winner = -1;

	_racks.clear();

	_numPlayers = numPlayers;
	for (int i = 0; i < numPlayers; ++i)
	{
		auto rack = std::make_shared<Rack>(i);
		for (int j = 0; j < 14; ++j)
		{
			rack->addTile(_pool.takeRandomTile());
		}
		_racks.push_back(rack);

		_players.push_back(std::make_shared<Player>(i, (i == 0 && withHuman), 3000, 5000, rack, _board, _pool));
	}

	_firstPlayer = Utils::getRand(_numPlayers);
	_currentPlayer = _firstPlayer;
	_lastPlayerToDrawFromPool = -1;
	_checkForAllZeros = false;
	_gameOver = false;
	_recording = false;
	startGameLog();
}

bool GameState::currentPlayerIsHuman() const
{
	return _players[_currentPlayer]->isHuman();
}

void GameState::playCurrentPlayer()
{
	printStatus();

	_players[_currentPlayer]->goComputer();

	if (_recording)
	{
		addToGameRecord("pool.printStatus();");
		addToGameRecord("board.printStatus();");
		addToGameRecord("for (auto & player : players)");
		addToGameRecord("{");
		addToGameRecord("\tplayer.printStatus();");
		addToGameRecord("}");
		addToGameRecord("std::cout << std::endl;");
		addToGameRecord("players[" + std::to_string(_currentPlayer) + "].goComputer(false);");
	}

	if (_recording)
	{
		bool writeDrawTileToGameRecord = false;
		if (_pool.numTilesRemaining() > 0)
		{
			if (!_players[_currentPlayer]->didSomethingOnLastTurn())
			{
				writeDrawTileToGameRecord = true;
			}
		}
		else
		{
			if (_lastPlayerToDrawFromPool == -1)
			{
				writeDrawTileToGameRecord = true;
			}
		}
		if (writeDrawTileToGameRecord)
		{
			auto tile = _racks[_currentPlayer]->getTile(_racks[_currentPlayer]->numTiles() - 1);
			if (tile->isJoker())
			{
				addToGameRecord("racks[" + std::to_string(_currentPlayer) + "]->addTile(std::make_shared<Tile>(Color::" + ColorStr[(int)tile->getJokerColor()] + "));");
				addToGameRecord("players[" + std::to_string(_currentPlayer) + "].printLastMoveSummary(std::make_shared<Tile>(Color::" + ColorStr[(int)tile->getJokerColor()] + "));");
			}
			else
			{
				addToGameRecord("racks[" + std::to_string(_currentPlayer) + "]->addTile(std::make_shared<Tile>(" + std::to_string(tile->getValue()) + ", Color::" + ColorStr[(int)tile->getColor()] + "));");
				addToGameRecord("players[" + std::to_string(_currentPlayer) + "].printLastMoveSummary(std::make_shared<Tile>(" + std::to_string(tile->getValue()) + ", Color::" + ColorStr[(int)tile->getColor()] + "));");
			}
		}
		else
		{
			addToGameRecord("players[" + std::to_string(_currentPlayer) + "].printLastMoveSummary();");
		}

	}

	printPlayerAction();

	if (_racks[_currentPlayer]->numTiles() == 0)
	{
		_winner = _currentPlayer;
		_gameOver = true;
	}

	if (!_gameOver)
	{
		if (_pool.numTilesRemaining() == 0 && _lastPlayerToDrawFromPool == -1)
		{
			_lastPlayerToDrawFromPool = _currentPlayer;
		}
		if (_lastPlayerToDrawFromPool != -1 && _currentPlayer == _lastPlayerToDrawFromPool)
		{
			_checkForAllZeros = true;
		}
		if (_checkForAllZeros)
		{
			bool playerPutDownTiles = false;
			for (const auto player : _players)
			{
				if (player->didSomethingOnLastTurn())
				{
					playerPutDownTiles = true;
					break;
				}
			}
			if (!playerPutDownTiles)
			{
				_gameOver = true;
			}
		}
	}

	if (_gameOver)
	{
		printStatus();

		if (_recording)
		{
			addToGameRecord("pool.printStatus();");
			addToGameRecord("board.printStatus();");
			addToGameRecord("for (auto & player : players)");
			addToGameRecord("{");
			addToGameRecord("\tplayer.printStatus();");
			addToGameRecord("}");
			addToGameRecord("std::cout << \"Gamestate: Game has ended . Player " + std::to_string(_winner) + " is the winner.\" << std::endl;");
		}

	}
}

void GameState::startGameRecord()
{
	_recording = true;

	// Create file with timestamp
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	time (&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer,sizeof(buffer),"%Y%m%d%H%M%S",timeinfo);
	std::string strTime(buffer);
	_gameRecordName = "rec_" + strTime + ".txt";

	_gameRecord.open(_gameRecordName, std::ios::out | std::ios::app);

	_gameRecord << "Pool pool;" << std::endl;
	_gameRecord << "Board board;" << std::endl;
	_gameRecord << "std::vector<std::shared_ptr<Rack>> racks;" << std::endl;
	_gameRecord << "std::vector<Player> players;" << std::endl;
	_gameRecord << std::endl;
	_gameRecord << "for (int i = 0; i < " << _numPlayers << "; ++i)" << std::endl;
	_gameRecord << "{" << std::endl;
	_gameRecord << "\tauto rack = std::make_shared<Rack>(i);" << std::endl;
	_gameRecord << "\tPlayer player(i, false, 3000, 5000, rack, board, pool);" << std::endl;
	_gameRecord << "\tracks.push_back(rack);" << std::endl;
	_gameRecord << "\tplayers.push_back(player);" << std::endl;
	_gameRecord << "}" << std::endl;

	for (int i = 0; i < _numPlayers; ++i)
	{
		_gameRecord << std::endl;
		for (int j = 0; j < _racks[i]->numTiles(); ++j)
		{
			auto tile = _racks[i]->getTile(j);
			if (tile->isJoker())
			{
				_gameRecord << "racks[" << i << "]->addTile(std::make_shared<Tile>(Color::" << ColorStr[(int)tile->getJokerColor()] << "));" << std::endl;
			}
			else
			{
				_gameRecord << "racks[" << i << "]->addTile(std::make_shared<Tile>(" << tile->getValue() << ", Color::" << ColorStr[(int)tile->getColor()] << "));" << std::endl;
			}
		}

	}
	_gameRecord << std::endl;

	_gameRecord.close();
}

void GameState::addToGameRecord(std::string text)
{
	_gameRecord.open(_gameRecordName, std::ios::out | std::ios::app);


	_gameRecord << text << std::endl;

	_gameRecord.close();
}

void GameState::startGameLog()
{
	_logging = true;

	// Create file with timestamp
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S", timeinfo);
	std::string strTime(buffer);
	_gameLogName = "log_" + strTime + ".txt";

	_gameLog.open(_gameLogName, std::ios::out | std::ios::app);

	_gameLog.close();
}

void GameState::startAddToGameLog()
{
	_gameLog.open(_gameLogName, std::ios::out | std::ios::app);
}

void GameState::endAddToGameLog()
{
	_gameLog.close();
}

void GameState::printStatus()
{
	startAddToGameLog();

	_pool.printStatus();
	_board.printStatus();
	_pool.printStatus(_gameLog);
	_board.printStatus(_gameLog);

	for (auto const & player : _players)
	{
		player->printStatus();
		player->printStatus(_gameLog);
	}

	if (_gameOver)
	{
		std::cout << "Gamestate: Game has ended ";
		_gameLog << "Gamestate: Game has ended ";

		int winner = getWinner();
		if (winner != -1)
		{
			std::cout << ". Player " << winner << " is the winner." << std::endl;
			_gameLog << ". Player " << winner << " is the winner." << std::endl;
		}
		else
		{
			std::cout << "in a draw." << std::endl;
			_gameLog << "in a draw." << std::endl;
		}
	}
	else
	{
		std::cout << std::endl;
		_gameLog << std::endl;
	}

	endAddToGameLog();
}

void GameState::printPlayerAction()
{
	startAddToGameLog();

	_players[_currentPlayer]->printLastMoveSummary();
	_players[_currentPlayer]->printLastMoveSummary(_gameLog);

	endAddToGameLog();
}

} // namespace Tiles


