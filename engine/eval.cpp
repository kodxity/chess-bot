#include "../game/board.hpp"
#include "../game/movegen.hpp"
#include "eval.hpp"
const int pawnValue = 1;
const int knightValue = 3;
const int bishopValue = 3;
const int rookValue = 5;
const int queenValue = 9;


int pieceSumEval(Board &board){
    int evaluation = 0;
	evaluation += board.pieces[P].countBits() * pawnValue;
	evaluation += board.pieces[N].countBits() * knightValue;
    evaluation += board.pieces[B].countBits() * bishopValue;
    evaluation += board.pieces[R].countBits() * rookValue;
    evaluation += board.pieces[Q].countBits() * queenValue;
    evaluation -= board.pieces[p].countBits() * pawnValue;
	evaluation -= board.pieces[n].countBits() * knightValue;
    evaluation -= board.pieces[b].countBits() * bishopValue;
    evaluation -= board.pieces[r].countBits() * rookValue;
    evaluation -= board.pieces[q].countBits() * queenValue;
    /*
    std::vector<Move> moves = MoveGenerator::generateMoves(board);
    if (moves.empty()) { // checkmate or stalemate
        if (board.isKingInCheck(board.turn)){
            evaluation = -1e9;
        }
        else{
            evaluation = 0;
        }
    }
    */
	return (evaluation * (board.turn == WHITE ? 1 : -1));

}

#include "nnue.hpp"
#include <iostream>
#include <vector>

// ============================================================
// Global NNUE network instance
// ============================================================


// Call this once during engine init


// ============================================================
// Example: convert a board to NNUE accumulators
// ============================================================


// ============================================================
// Evaluate NNUE
// ============================================================

int32_t evaluate_board(const Board& b) {
    if (b.turn == WHITE)
        return evaluate(g_net, b.us, b.them);
    else
        return -evaluate(g_net, b.us, b.them);
}