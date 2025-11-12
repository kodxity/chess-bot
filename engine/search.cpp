#include "search.hpp"
#include "eval.hpp"   // your eval function header
#include "order.hpp"   // your eval function header
#include "../game/movegen.hpp"      // your move generator
#include "../game/board.hpp"      // your move generator
#include <algorithm>
#include <limits>

// -----------------------------------------
// Constructor
// -----------------------------------------
Search::Search(int maxDepth) : maxDepth(maxDepth) {}


// -----------------------------------------
// Main search entry: finds the best move
// -----------------------------------------
SearchResult Search::findBestMove(Board& board) {
    SearchResult result;
    result.score = -1e9;
    result.bestMove = Move(); // default no-move

    int alpha = -1e9;
    int beta  = 1e9;

    // Get legal moves from root
    std::vector<Move> moves = MoveGenerator::generateMoves(board);
    if (moves.empty()) { // checkmate or stalemate
        if (board.isKingInCheck(board.turn)){
            result.score = -1e9;
        }
        else{
            result.score = 0;
        }
    }

    int bestScore = -1e9;
    Move bestMove;

    // Simple depth search loop (no iterative deepening for now)
    for (const Move& mv : moves) {
        board.makeMove(mv);
        int score = -negamax(board, maxDepth - 1,  1, -beta, -alpha);
        board.unmakeMove(mv);
        std::cout<<mv.toString()<<" "<<score<<"\n";
        if (score > bestScore) {
            bestScore = score;
            bestMove = mv;
        }
    }

    result.bestMove = bestMove;
    result.score = bestScore;
    return result;
}


// -----------------------------------------
// Negamax with alpha-beta pruning
// -----------------------------------------

int Search::quiescence(Board& board, int alpha = -1e9, int beta = 1e9) {
	int standPat = pieceSumEval(board); // eval returns a white-relative score
    std::vector<Move> moves = MoveGenerator::generateMoves(board);
    if (moves.empty()) { // checkmate or stalemate
        if (board.isKingInCheck(board.turn)){
            return -1e9;
        }
        else{
            return 0;
        }
    }
	if (standPat >= beta) {
		return standPat; // Opponent won't let this happen
	}
	if (standPat > alpha) {
		alpha = standPat; // Update alpha if we found a better score
	}
    
	for (Move m : moves) {
		if (m.flag != QUIET) {
			board.makeMove(m);
			int score = -quiescence(board, -beta, -alpha);
			board.unmakeMove(m);

			if (score > standPat) {
				standPat = score; // Update the best score if we found a better one
				if (score > alpha) {
					alpha = score;
				}
			}
			if (score >= beta) {
				return score; // Beta cutoff
			}
		}
	}
	
	return standPat;
}

int Search::negamax(Board& board, int depth, int ply, int alpha, int beta) {
    if (depth <= 0) return quiescence(board, alpha, beta);

    std::vector<Move> moves = MoveGenerator::generateMoves(board);
    if (moves.empty()) { // checkmate or stalemate
        if (board.isKingInCheck(board.turn)){
            return -1e9;
        }
        else{
            return 0;
        }
    }

    orderMoves(board, moves, ply);
    int bestValue = -1e9;

    for (const Move& mv : moves) {
        board.makeMove(mv);
        int value = -negamax(board, depth - 1, ply+1, -beta, -alpha);
        /*
        if(depth==1 && mv.toString() == "d3e4"){
            std::cout<<1<<" "<<mv.toString()<<" "<<value<<" "<<pieceSumEval(board)<<" "<<board.pieces[P].countBits()<<" "<<board.pieces[p].countBits()<<" "<<board.pieces[n].countBits()<<" "<<board.pieces[N].countBits()<<board.pieces[b].countBits()<<" "<<board.pieces[B].countBits()<<" "<<board.pieces[r].countBits()<<" "<<board.pieces[R].countBits()<<"\n";
            board.printBoard();
        }
        */
        board.unmakeMove(mv);
        
        if (value > bestValue) {
            bestValue = value;
            if (value > alpha) {
				alpha = value; 
			}
        }

        if (value >= beta) {
            if (mv.flag == QUIET) {
                // QUIET move caused cutoff: record killer + history
                addKiller(mv, ply);
                updateHistory(mv, ply);
            }
            return bestValue; // alpha-beta cutoff
        }
    }

    return bestValue;
}

