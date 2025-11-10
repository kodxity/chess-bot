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
    P, N, B, R, Q, K,   
    p, n, b, r, q, k,  
    NO_PIECE
};
// Each move can have a special type (normal, capture, promotion, etc.)
enum MoveFlag{
    QUIET = 0,          // normal move
    CAPTURE,            // a piece captures another
    DOUBLE_PAWN_PUSH,   // pawn moves two squares 
    KING_CASTLE,        // white king castles (O-O)
    QUEEN_CASTLE,       // white queen castles (O-O-O)
    EN_PASSANT,         // pawn captures en passant
    PROMOTION_QUEEN,
    PROMOTION_ROOK,
    PROMOTION_BISHOP,
    PROMOTION_KNIGHT
};




// Represents a chess move
struct Move {
    int from;           // square where the piece starts
    int to;             // square where it moves
    MoveFlag flag;      // special info (like CAPTURE or PROMOTION)
    Piece piece;          // which piece moved
    Piece captured;       // what was captured (-1 if none)
    int prevEnPassantSquare; // store previous en passant square
    int prevCastlingRights;
    // Constructor
    Move(int from_=0, int to_=0, MoveFlag flag_=QUIET, Piece piece_=NO_PIECE, Piece captured_=NO_PIECE, int prevEnPassantSquare_ = NO_SQUARE, int prevCastlingRights_ = 0)
        : from(from_), to(to_), flag(flag_), piece(piece_), captured(captured_), prevEnPassantSquare(prevEnPassantSquare_), prevCastlingRights(prevCastlingRights_){}
    // Convert move into string format
    std::string toString() const {
       char fromFile = 'a' + (from % 8);
        char fromRank = '1' + (from / 8);
        char toFile = 'a' + (to % 8);
        char toRank = '1' + (to / 8);
        return std::string() + fromFile + fromRank + toFile + toRank;
    }
};  

class Board{
public:

    // Bitboards
    Bitboard pieces[13]; // bitboards for all 12 pieces + empty piece
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


    // Moves
    bool makeMove(const Move& move); // make move `move`
    bool unmakeMove(const Move& move); //unmake move `move`
    // Game state
    bool isSquareAttacked(int square, int bySide) const; // check if given square is attacked by given side
    bool isKingInCheck(int side) const; // check if king is in check for given side

};