#ifndef INCLUDE_MOVE_RECEIVER_H_
#define INCLUDE_MOVE_RECEIVER_H_

#include <memory>

namespace Tiles
{

class Board;
class Player;
class Rack;
class Solution;

class MoveReceiver
{
private:
	std::shared_ptr<Player> _player;
	std::shared_ptr<Rack> _rack;
	Board& _board;
	bool _isBeforeInitialMeld;
	std::shared_ptr<Solution> _solution;

	// Receives a single move from the input source.
	bool receiveMove();

	// Checks if all the moves received form a valid solution.
	bool validateSolution();

	// Removes all the moves from the solution.
	void emptySolution();

	// Receives confirmation or not from the input source for the solution.
	bool receiveConfirmation();

	// Create a move from the given text.
	bool createMoveFromText(const std::string& text, std::shared_ptr<Moove> move);

public:
	MoveReceiver(std::shared_ptr<Player> player, std::shared_ptr<Rack> rack, Board & board, bool isBeforeInitialMeld);

	void receiveSolution();
	int getSolutionScore() const;
	std::shared_ptr<Solution> getSolution() const;
	bool hasMoves() const;

};

}



#endif /* INCLUDE_MOVE_RECEIVER_H_ */
