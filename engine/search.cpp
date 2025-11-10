#include "search.hpp"
#include "eval.hpp"   // your eval function header
#include "movegen.hpp"      // your move generator
#include "board.hpp"      // your move generator
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
    result.score = std::numeric_limits<int>::min();
    result.bestMove = Move(); // default no-move

    int alpha = std::numeric_limits<int>::min() + 1;
    int beta  = std::numeric_limits<int>::max() - 1;

    // Get legal moves from root
    std::vector<Move> moves = MoveGenerator::generateMoves(board);
    if (moves.empty()) {
        // no legal moves = checkmate or stalemate
        result.score = pieceSumEval(board);
        return result;
    }

    int bestScore = std::numeric_limits<int>::min();
    Move bestMove;

    // Simple depth search loop (no iterative deepening for now)
    for (const Move& mv : moves) {
        board.makeMove(mv);
        int score = -negamax(board, maxDepth - 1, -beta, -alpha);
        board.unmakeMove(mv);

        if (score > bestScore) {
            bestScore = score;
            bestMove = mv;
        }

        if (score > alpha) {
            alpha = score;
        }
    }

    result.bestMove = bestMove;
    result.score = bestScore;
    return result;
}


// -----------------------------------------
// Negamax with alpha-beta pruning
// -----------------------------------------
int Search::negamax(Board& board, int depth, int alpha, int beta) {
    if (depth == 0) {
        return pieceSumEval(board);
    }

    std::vector<Move> moves = MoveGenerator::generateMoves(board);
    if (moves.empty()) {
        // No legal moves → checkmate or stalemate
        return pieceSumEval(board);
    }

    int bestValue = std::numeric_limits<int>::min();

    for (const Move& mv : moves) {
        board.makeMove(mv);

        int value = -negamax(board, depth - 1, -beta, -alpha);

        board.unmakeMove(mv);

        if (value > bestValue) {
            bestValue = value;
        }

        alpha = std::max(alpha, value);
        if (alpha >= beta) {
            break; // alpha-beta cutoff
        }
    }

    return bestValue;
}

