#include "bitboard.hpp"
#include <iostream> 


// Constructors

Bitboard::Bitboard():board(0ULL){} // create empty board (0 unsigned long long)
Bitboard::Bitboard(uint64_t b):board(b){} // creates bitboard represented by b

// Operations

void Bitboard::setBit(int pos){
    board |= (1ULL << pos);
}

void Bitboard::clearBit(int pos){
    board &= ~(1ULL << pos);
}

// Pop least significant bit (remove it)
void Bitboard::popLSB(){
    board &= (board - 1);
}


void Bitboard::flipBit(int pos){
    board ^= (1ULL << pos);
}

// Functions

bool Bitboard::getBit(int pos) const{
    return board & (1ULL << pos);
}

// Count how many bits are 1 (how many squares occupied)
int Bitboard::countBits() const{ 
    return __builtin_popcountll(board);
    /*
    int count = 0;
    uint64_t b = board;
    while (b) {
        b &= (b - 1);
        count++;
    }
    return count;
    */
}

// Get least significant bit index (lowest set bit)
int Bitboard::getLSB() const{
    return board ? __builtin_ctzll(board) : -1;
    /*
    if (!board) return -1;
    int index = 0;
    uint64_t b = board;
    while ((b & 1ULL) == 0ULL) {
        b >>= 1;
        index++;
    }
    return index;
    */
}


// Utilities

// convert bitboard to s8x8 matrix of '1' = filled and '.' = empty
std::string Bitboard::toString(uint64_t bb){
    std::string s;
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            int sq = rank * 8 + file;
            s += (bb & (1ULL << sq)) ? "1 " : ". ";
        }
        s += "\n";
    }
    return s;
}

// pritn board in terms of 8x8 matrix
void Bitboard::print() const{
    std::cout<<toString(board)<<std::endl;
}