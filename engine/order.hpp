#pragma once
#include "../game/movegen.hpp"
#include "../game/board.hpp"
#include <vector>
#include <algorithm>
static int pieceValue[7] = {
    0,     // EMPTY
    100,   // PAWN
    300,   // KNIGHT
    300,   // BISHOP
    500,   // ROOK
    900,   // QUEEN
    10000  // KING
};

// Stores history scores: piece x square
static int historyTable[13][64] = {{0}};

// Stores 2 killer moves per depth
static Move killerMoves[128][2];

// define for move equality


inline void scoreMove(const Board& board, Move& m, int depth) {
    if (m.flag == CAPTURE) {
        // MVV-LVA: Victim value * 1000 - attacker value
        int victim = pieceValue[m.captured];
        int attacker = pieceValue[m.piece];
        m.score = 1000000 + victim * 1000 - attacker;
        return;
    }

    // Killer moves
    if (killerMoves[depth][0] == m)
        { m.score = 900000; return; }
    if (killerMoves[depth][1] == m)
        { m.score = 800000; return; }

    // Quiet move: use history heuristic
    m.score = historyTable[m.piece][m.to];
}

inline void orderMoves(Board& board, std::vector<Move>& moves, int depth) {
    for (auto& m : moves)
        scoreMove(board, m, depth);

    std::sort(moves.begin(), moves.end(),
              [](const Move& a, const Move& b) {
                  return a.score > b.score;
              });
}

// Save killer moves
inline void addKiller(Move m, int depth) {
    if (!(killerMoves[depth][0] == m)) {
        killerMoves[depth][1] = killerMoves[depth][0];
        killerMoves[depth][0] = m;
    }
}

// Update history on quiet move causing beta cutoff
inline void updateHistory(Move m, int depth) {
    historyTable[m.piece][m.to] += depth * depth;
    if (historyTable[m.piece][m.to] > 100000000)
        historyTable[m.piece][m.to] /= 2;
}
