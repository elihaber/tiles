/*
 * game_state.h
 *
 *  Created on: Sep 7, 2017
 *      Author: eliha
 */

#ifndef INCLUDE_GAME_STATE_H_
#define INCLUDE_GAME_STATE_H_

#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include "pool.h"
#include "board.h"

namespace Tiles
{

class Pool;
class Board;
class Rack;
class Player;

class GameState
{
private:
	Pool _pool;
	Board _board;
	std::vector<std::shared_ptr<Rack>> _racks;
	std::vector<std::shared_ptr<Player>> _players;
	int _numPlayers;
	int _firstPlayer;
	int _currentPlayer;
	bool _gameOver;
	int _lastPlayerToDrawFromPool;
	bool _checkForAllZeros;
	int _winner;
	bool _recording;
	bool _logging;
	std::ofstream _gameRecord;
	std::string _gameRecordName;
	std::ofstream _gameLog;
	std::string _gameLogName;

public:
	int getWinner() const { return _winner; }
	bool gameOver() const { return _gameOver; }
	bool currentPlayerIsHuman() const;

	void init(int numPlayers, bool withHuman = true);
	void playCurrentPlayer();
	void moveToNextPlayer() { if (++_currentPlayer >= _numPlayers) _currentPlayer = 0; }

	void startGameRecord();
	void addToGameRecord(std::string text);
	void startGameLog();
	void startAddToGameLog();
	void endAddToGameLog();

	void printStatus();
	void printPlayerAction();
};

} // namespace Tiles



#endif /* INCLUDE_GAME_STATE_H_ */
