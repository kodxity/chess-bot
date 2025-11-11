#ifndef SEARCH_HPP
#define SEARCH_HPP

#include <vector>
#include <cstdint>
#include "../game/movegen.hpp"
#include "../game/board.hpp"

// Result container for the search output
struct SearchResult {
    Move bestMove;
    int score;
};

class Search {
public:
    Search(int maxDepth);

    // Main entry point: finds the best move from the root position
    SearchResult findBestMove(Board& board);

private:
    int maxDepth;

    // Negamax search with alpha-beta pruning
    int negamax(Board& board, int depth, int ply = 0, int alpha = 1e9, int beta = -1e9);
    int quiescence(Board& board, int alpha, int beta);
};

#endif // SEARCH_HPP
