#pragma once // prevent errors

#include <cstdint> // include uint64_t 
#include <iostream>
#include <string> // can use string
#include "bitboard.hpp"


// enums

enum Turn{
    WHITE, BLACK, BOTH
};

enum Piece{
    P, N, B, R, Q, K,   // White
    p, n, b, r, q, k,   // Black
    NO_PIECE
};


class Board{
public:

    // Bitboards
    Bitboard pieces[12]; // bitboards for all total 12 pieces
    Bitboard occupancy[3]; // Occupancy bitboards for WHITE, BLACK, BOTH

    // Game state
    Turn turn; 
    int enPassantSquare;
    int castlingRights; // 4 bits: KQkq = castle

    // Constructors
    Board();

    // Functions
    void clear(); // clear board
    void setPiece(int square, Piece piece); // put piece on square 
    void removePiece(int square, Piece piece); // remove piece on square
    Piece getPiece(int square) const; // get piece at that square

    void updateOccupancy(); // recalculates occupancy after these updates

    // Utility
    void loadFEN(const std::string& fen); // FEN handling
    void printBoard() const; // print visual board to console
};