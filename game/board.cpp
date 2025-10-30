#include "board.hpp"
#include <iostream>
#include <sstream>
#include <cctype>

// Constructor 
Board::Board(){
    clear();
}


void Board::clear(){
    // Clear all piece bitboards
    for (int i = 0; i < 12; i++)
        pieces[i] = Bitboard(0ULL);

    // Clear occupancy bitboards
    for (int i = 0; i < 3; i++)
        occupancy[i] = Bitboard(0ULL);

    turn = WHITE;
    enPassantSquare = NO_SQUARE; // enum value from bitboard.hpp
    castlingRights = 0;
}

// Set and Remove Pieces
void Board::setPiece(int square, Piece piece){
    if (square < A1 || square > H8) return; // ensure valid square
    pieces[piece].setBit(square);
}

void Board::removePiece(int square, Piece piece){
    if (square < A1 || square > H8) return; 
    pieces[piece].clearBit(square);
}

// Get Piece at Square 
Piece Board::getPiece(int square) const {
    if (square < A1 || square > H8) return NO_PIECE;
    for (int p = P; p <= k; p++) {
        if (pieces[p].getBit(square))
            return static_cast<Piece>(p);
    }
    return NO_PIECE;
}

// Update Occupancy
void Board::updateOccupancy(){
    occupancy[WHITE].board = 0ULL;
    occupancy[BLACK].board = 0ULL;

    // White pieces: P..K range [0...5] in pieces bitboard
    for (int x = P; x <= K; x++)
        occupancy[WHITE].board |= pieces[x].board;

    // Black pieces: p..k range [6...11] in pieces bitboard
    for (int x = p; x <= k; x++)
        occupancy[BLACK].board |= pieces[x].board;

    // Both sides
    occupancy[BOTH].board = occupancy[WHITE].board | occupancy[BLACK].board;
}

// Decrypt FEN
void Board::loadFEN(const std::string& fen) {
    clear();

    std::istringstream ss(fen);
    std::string boardPart, side, castling, enPassant;
    int halfmove = 0, fullmove = 1;

    ss >> boardPart >> side >> castling >> enPassant >> halfmove >> fullmove;

    int square = A8; // top-left corner
    for (char c : boardPart) {
        if (c == '/') {
            square -= 16; // move to next rank down
        } else if (isdigit(c)) {
            square += c - '0'; // skip empty squares
        } else {
            Piece piece = NO_PIECE;
            switch (c) {
                case 'P': piece = P; break;
                case 'N': piece = N; break;
                case 'B': piece = B; break;
                case 'R': piece = R; break;
                case 'Q': piece = Q; break;
                case 'K': piece = K; break;
                case 'p': piece = p; break;
                case 'n': piece = n; break;
                case 'b': piece = b; break;
                case 'r': piece = r; break;
                case 'q': piece = q; break;
                case 'k': piece = k; break;
            }
            if (piece != NO_PIECE)
                setPiece(square, piece);
            square++;
        }
    }

    // Side to move
    turn = (side == "w") ? WHITE : BLACK;

    // Castling rights
    castlingRights = 0;
    if (castling.find('K') != std::string::npos) castlingRights |= 1;
    if (castling.find('Q') != std::string::npos) castlingRights |= 2;
    if (castling.find('k') != std::string::npos) castlingRights |= 4;
    if (castling.find('q') != std::string::npos) castlingRights |= 8;

    // En passant
    if (enPassant != "-") {
        int file = enPassant[0] - 'a';
        int rank = enPassant[1] - '1';
        enPassantSquare = rank * 8 + file;
    } else {
        enPassantSquare = NO_SQUARE;
    }

    updateOccupancy();
}

// Print Board
void Board::printBoard() const {
    for (int rank = 7; rank >= 0; rank--) {
        std::cout<<rank+1<<" ";
        for (int file = 0; file < 8; file++) {
            int sq = rank * 8 + file;
            Piece x = getPiece(sq);
            char c = '.';
            switch (x) {
                case P: c = 'P'; break;
                case N: c = 'N'; break;
                case B: c = 'B'; break;
                case R: c = 'R'; break;
                case Q: c = 'Q'; break;
                case K: c = 'K'; break;
                case p: c = 'p'; break;
                case n: c = 'n'; break;
                case b: c = 'b'; break;
                case r: c = 'r'; break;
                case q: c = 'q'; break;
                case k: c = 'k'; break;
                default: break;
            }
            std::cout<<c<<" ";
        }
        std::cout<<"\n";
    }

    std::cout<<"  a b c d e f g h\n";
    std::cout<<(turn == WHITE ? "White" : "Black")<<" to move\n";
}
