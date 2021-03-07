/*
 * tiles_console.cpp
 *
 *  Created on: Sep 4, 2017
 *      Author: eliha
 */

//#ifdef WIN32
#include "stdafx.h"
//#endif

#include <memory>
#include "game_state.h"
#include "player.h"
#include "rack.h"
#include "board.h"
#include "pool.h"
#include "tile.h"
#include "group.h"

#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

using namespace Tiles;

static bool RUN_DEBUG = false;
static bool RUN_RECORDED_GAME = false;
static bool RUN_UNIT_TEST = false;

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

int main()
{
	signal(SIGSEGV, handler);   // install our handler
/*
	Tiles::Board board;

	std::shared_ptr<Tiles::Tile> tile1 = std::make_shared<Tiles::Tile>(4, Tiles::Color::Yellow);
	std::shared_ptr<Tiles::Group> group1 = std::make_shared<Tiles::Group>(tile1);

	std::shared_ptr<Tiles::Tile> tile2 = std::make_shared<Tiles::Tile>(5, Tiles::Color::Blue);
	std::shared_ptr<Tiles::Group> group2 = std::make_shared<Tiles::Group>(tile2);

	int id1 = board.createNewGroup(tile1);
	auto gr1 = board.findGroupById(id1);
	board.removeOnlyTileOfGroup(gr1);
	int id2 = board.createNewGroup(tile2);
	auto gr2 = board.findGroupById(id2);

	std::cout << "All ok" << std::endl;
	//int numTurns = 0;
*/

	if (RUN_UNIT_TEST)
	{
		Board board_a, board_b;

		auto groupa1 = std::make_shared<Group>();
		groupa1->addTileToEnd(std::make_shared<Tile>(12, Color::Black));
		groupa1->addTileToEnd(std::make_shared<Tile>(12, Color::Red));
		groupa1->addTileToEnd(std::make_shared<Tile>(12, Color::Blue));
		board_a.addGroup(groupa1);
		auto groupa2 = std::make_shared<Group>();
		board_a.addGroup(groupa2);
		auto groupa3 = std::make_shared<Group>();
		groupa3->addTileToEnd(std::make_shared<Tile>(10, Color::Yellow));
		groupa3->addTileToEnd(std::make_shared<Tile>(11, Color::Yellow));
		groupa3->addTileToEnd(std::make_shared<Tile>(12, Color::Yellow));
		board_a.addGroup(groupa3);
		auto groupa4 = std::make_shared<Group>();
		groupa4->addTileToEnd(std::make_shared<Tile>(10, Color::Black));
		groupa4->addTileToEnd(std::make_shared<Tile>(11, Color::Black));
		board_a.addGroup(groupa4);
		auto groupa5 = std::make_shared<Group>();
		groupa5->addTileToEnd(std::make_shared<Tile>(105));
		board_a.addGroup(groupa5);
		auto groupa6 = std::make_shared<Group>();
		groupa6->addTileToEnd(std::make_shared<Tile>(11, Color::Black));
		groupa6->addTileToEnd(std::make_shared<Tile>(12, Color::Black));
		groupa6->addTileToEnd(std::make_shared<Tile>(13, Color::Black));
		board_a.addGroup(groupa6);

		auto groupb1 = std::make_shared<Group>();
		groupb1->addTileToEnd(std::make_shared<Tile>(12, Color::Red));
		groupb1->addTileToEnd(std::make_shared<Tile>(12, Color::Blue));
		groupb1->addTileToEnd(std::make_shared<Tile>(12, Color::Black));
		board_b.addGroup(groupb1);
		auto groupb2 = std::make_shared<Group>();
		board_b.addGroup(groupb2);
		auto groupb3 = std::make_shared<Group>();
		groupb3->addTileToEnd(std::make_shared<Tile>(10, Color::Yellow));
		groupb3->addTileToEnd(std::make_shared<Tile>(11, Color::Yellow));
		groupb3->addTileToEnd(std::make_shared<Tile>(12, Color::Yellow));
		board_b.addGroup(groupb3);
		auto groupb4 = std::make_shared<Group>();
		groupb4->addTileToEnd(std::make_shared<Tile>(10, Color::Black));
		groupb4->addTileToEnd(std::make_shared<Tile>(11, Color::Black));
		board_b.addGroup(groupb4);
		auto groupb5 = std::make_shared<Group>();
		groupb5->addTileToEnd(std::make_shared<Tile>(105));
		board_b.addGroup(groupb5);
		auto groupb6 = std::make_shared<Group>();
		groupb6->addTileToEnd(std::make_shared<Tile>(11, Color::Black));
		groupb6->addTileToEnd(std::make_shared<Tile>(12, Color::Black));
		groupb6->addTileToEnd(std::make_shared<Tile>(13, Color::Black));
		board_b.addGroup(groupb6);

		if (board_b == board_a)
		{
			std::cout << "The two boards are the same" << std::endl;
		}
		else
		{
			std::cout << "The two boards are different" << std::endl;
		}
		return 0;
	}

	if (!RUN_DEBUG && !RUN_RECORDED_GAME)
	{
		Tiles::GameState gamestate;
		gamestate.init(4, false);
		gamestate.startGameRecord();
		while (!gamestate.gameOver())
		{
			if (!gamestate.currentPlayerIsHuman())
			{
				gamestate.playCurrentPlayer();
			}
			gamestate.moveToNextPlayer();
		}
	}

	if (RUN_RECORDED_GAME)
	{
		Pool pool;
		Board board;
		std::vector<std::shared_ptr<Rack>> racks;
		std::vector<Player> players;

		for (int i = 0; i < 4; ++i)
		{
			auto rack = std::make_shared<Rack>(i);
			Player player(i, false, 3000, 5000, rack, board, pool);
			racks.push_back(rack);
			players.push_back(player);
		}

		racks[0]->addTile(std::make_shared<Tile>(1, Color::Blue));
		racks[0]->addTile(std::make_shared<Tile>(5, Color::Blue));
		racks[0]->addTile(std::make_shared<Tile>(6, Color::Red));
		racks[0]->addTile(std::make_shared<Tile>(8, Color::Blue));
		racks[0]->addTile(std::make_shared<Tile>(9, Color::Yellow));
		racks[0]->addTile(std::make_shared<Tile>(9, Color::Red));
		racks[0]->addTile(std::make_shared<Tile>(4, Color::Yellow));
		racks[0]->addTile(std::make_shared<Tile>(9, Color::Black));
		racks[0]->addTile(std::make_shared<Tile>(2, Color::Red));
		racks[0]->addTile(std::make_shared<Tile>(3, Color::Red));
		racks[0]->addTile(std::make_shared<Tile>(6, Color::Red));
		racks[0]->addTile(std::make_shared<Tile>(13, Color::Red));
		racks[0]->addTile(std::make_shared<Tile>(7, Color::Black));
		racks[0]->addTile(std::make_shared<Tile>(2, Color::Black));

		racks[1]->addTile(std::make_shared<Tile>(13, Color::Blue));
		racks[1]->addTile(std::make_shared<Tile>(7, Color::Blue));
		racks[1]->addTile(std::make_shared<Tile>(7, Color::Black));
		racks[1]->addTile(std::make_shared<Tile>(1, Color::Red));
		racks[1]->addTile(std::make_shared<Tile>(3, Color::Black));
		racks[1]->addTile(std::make_shared<Tile>(3, Color::Yellow));
		racks[1]->addTile(std::make_shared<Tile>(12, Color::Black));
		racks[1]->addTile(std::make_shared<Tile>(10, Color::Blue));
		racks[1]->addTile(std::make_shared<Tile>(1, Color::Yellow));
		racks[1]->addTile(std::make_shared<Tile>(10, Color::Red));
		racks[1]->addTile(std::make_shared<Tile>(11, Color::Yellow));
		racks[1]->addTile(std::make_shared<Tile>(3, Color::Yellow));
		racks[1]->addTile(std::make_shared<Tile>(10, Color::Yellow));
		racks[1]->addTile(std::make_shared<Tile>(10, Color::Black));

		racks[2]->addTile(std::make_shared<Tile>(1, Color::Blue));
		racks[2]->addTile(std::make_shared<Tile>(Color::Red));
		racks[2]->addTile(std::make_shared<Tile>(5, Color::Red));
		racks[2]->addTile(std::make_shared<Tile>(Color::Black));
		racks[2]->addTile(std::make_shared<Tile>(12, Color::Yellow));
		racks[2]->addTile(std::make_shared<Tile>(4, Color::Blue));
		racks[2]->addTile(std::make_shared<Tile>(4, Color::Red));
		racks[2]->addTile(std::make_shared<Tile>(3, Color::Red));
		racks[2]->addTile(std::make_shared<Tile>(2, Color::Blue));
		racks[2]->addTile(std::make_shared<Tile>(2, Color::Yellow));
		racks[2]->addTile(std::make_shared<Tile>(3, Color::Blue));
		racks[2]->addTile(std::make_shared<Tile>(11, Color::Blue));
		racks[2]->addTile(std::make_shared<Tile>(13, Color::Blue));
		racks[2]->addTile(std::make_shared<Tile>(4, Color::Black));

		racks[3]->addTile(std::make_shared<Tile>(2, Color::Yellow));
		racks[3]->addTile(std::make_shared<Tile>(6, Color::Yellow));
		racks[3]->addTile(std::make_shared<Tile>(9, Color::Blue));
		racks[3]->addTile(std::make_shared<Tile>(5, Color::Black));
		racks[3]->addTile(std::make_shared<Tile>(11, Color::Blue));
		racks[3]->addTile(std::make_shared<Tile>(9, Color::Black));
		racks[3]->addTile(std::make_shared<Tile>(8, Color::Yellow));
		racks[3]->addTile(std::make_shared<Tile>(8, Color::Blue));
		racks[3]->addTile(std::make_shared<Tile>(8, Color::Yellow));
		racks[3]->addTile(std::make_shared<Tile>(5, Color::Blue));
		racks[3]->addTile(std::make_shared<Tile>(12, Color::Black));
		racks[3]->addTile(std::make_shared<Tile>(1, Color::Yellow));
		racks[3]->addTile(std::make_shared<Tile>(8, Color::Black));
		racks[3]->addTile(std::make_shared<Tile>(8, Color::Black));

		pool.printStatus();
		board.printStatus();
		for (auto & player : players)
		{
			player.printStatus();
		}
		std::cout << std::endl;
		players[3].goComputer(false);
		racks[3]->addTile(std::make_shared<Tile>(13, Color::Black));
		players[3].printLastMoveSummary(std::make_shared<Tile>(13, Color::Black));
		pool.printStatus();
		board.printStatus();
		for (auto & player : players)
		{
			player.printStatus();
		}
		std::cout << std::endl;
		players[0].goComputer(false);
		racks[0]->addTile(std::make_shared<Tile>(11, Color::Yellow));
		players[0].printLastMoveSummary(std::make_shared<Tile>(11, Color::Yellow));
		pool.printStatus();
		board.printStatus();
		for (auto & player : players)
		{
			player.printStatus();
		}
		std::cout << std::endl;
		players[1].goComputer(false);
		players[1].printLastMoveSummary();
		pool.printStatus();
		board.printStatus();
		for (auto & player : players)
		{
			player.printStatus();
		}
		std::cout << std::endl;
		players[2].setDebugLogic(true);
		players[2].setDebugMoves(true);
		players[2].goComputer(false);
	}

	if (RUN_DEBUG)
	{
	}

//	std::cout << "Enter q to quit" << std::endl;
//	std::string userInput;
//	std::cin >> userInput;
	return 0;
}
