#include "../game/board.hpp"

const int pawnValue = 1;
const int knightValue = 3;
const int bishopValue = 3;
const int rookValue = 5;
const int queenValue = 9;

int pieceSumEval(Board board){
    int evaluation = 0;
	evaluation += board.pieces['P'].countBits() * pawnValue;
	evaluation += board.pieces['K'].countBits() * pawnValue;
    evaluation += board.pieces['B'].countBits() * pawnValue;
    evaluation += board.pieces['R'].countBits() * pawnValue;
    evaluation += board.pieces['Q'].countBits() * pawnValue;
    evaluation -= board.pieces['p'].countBits() * pawnValue;
	evaluation -= board.pieces['k'].countBits() * pawnValue;
    evaluation -= board.pieces['b'].countBits() * pawnValue;
    evaluation -= board.pieces['r'].countBits() * pawnValue;
    evaluation -= board.pieces['q'].countBits() * pawnValue;
	return evaluation;
}

