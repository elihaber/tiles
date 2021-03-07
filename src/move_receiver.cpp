#include "move_receiver.h"
#include "solution.h"

namespace Tiles
{

MoveReceiver::MoveReceiver(std::shared_ptr<Player> player, std::shared_ptr<Rack> rack, Board & board, bool isBeforeInitialMeld) :
		_player(player), _rack(rack), _board(board), _isBeforeInitialMeld(isBeforeInitialMeld)
{

}

bool MoveReceiver::createMoveFromText(const std::string& text, std::shared_ptr<Moove> move) {

}

bool MoveReceiver::receiveMove() {
	bool receivedValidMoveOrNone = false;

	while (!receivedValidMoveOrNone) {
		std::cout << "Enter move or q for none: ";

		std::string userInput;
		std::cin >> userInput;

		if (userInput == "q" || userInput == "Q") {
			receivedValidMoveOrNone = true;
		}
		else {
			std::shared_ptr<Moove> move;
			if (createMoveFromText(userInput, move)) {
				receivedValidMoveOrNone = true;
			}
		}
	}
}

bool MoveReceiver::receiveConfirmation() {
	return false;
}

void MoveReceiver::receiveSolution() {

	bool confirmed = false;

	while (!confirmed) {
		while (receiveMove()) {
			;
		}

		if (validateSolution()) {
			emptySolution();
		}

		confirmed = receiveConfirmation();
	}
}

int MoveReceiver::getSolutionScore() const {
	return _solution->score();
}

std::shared_ptr<Solution> MoveReceiver::getSolution() const {
	return _solution;
}

bool MoveReceiver::hasMoves() const {
	return (_solution->numMoves() > 0);
}

void MoveReceiver::emptySolution() {
	_solution = std::make_shared<Solution>(*_rack, _board, false, false);
}

bool MoveReceiver::validateSolution() {
	true;
	_solution->applyAllMoves();
	bool result = _board.validateAllGroups();
	_solution->unapplyAllMoves();
	return result;
}

}


