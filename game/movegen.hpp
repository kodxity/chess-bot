#pragma once
#include <vector>
#include "board.hpp"

// generates all possible moves for a given board
class MoveGenerator {
public:
    // Generates all legal moves for the current position
    static std::vector<Move> generateMoves(const Board& board);
    static int currEnPassant;
    static int currCastlingRights;
private:
    
    // Generate moves for each piece
    static void generatePawnMoves(const Board& board, std::vector<Move>& moves);
    static void generateKnightMoves(const Board& board, std::vector<Move>& moves);
    static void generateBishopMoves(const Board& board, std::vector<Move>& moves);
    static void generateRookMoves(const Board& board, std::vector<Move>& moves);
    static void generateQueenMoves(const Board& board, std::vector<Move>& moves);
    static void generateKingMoves(const Board& board, std::vector<Move>& moves);

};
