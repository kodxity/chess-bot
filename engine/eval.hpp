#pragma once

#include "../game/board.hpp"
#include "nnue.hpp"
#include <cstdint>

// ============================================================
// NNUE initialization
// ============================================================


// ============================================================
// Board evaluation functions
// ============================================================

// Evaluate the board using piece values only
int pieceSumEval(Board &board);

// Evaluate the board using the NNUE network
int32_t evaluate_board(const Board& board);
