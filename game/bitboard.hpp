#pragma once // prevent errors

#include <cstdint> // include uint64_t 
#include <iostream>
#include <string> // can use string

class Bitboard{ // class represents one bitboard
public: // other files can use
    uint64_t board; // unsigned 64-bit integer representing the board

    // Constructors 
    Bitboard();
    Bitboard(uint64_t b);


    // Operations
    void setBit(int pos);
    void clearBit(int pos);
    void flipBit(int pos);
    void popLSB();

    // Functions
    bool getBit(int pos) const;
    int countBits() const;
    int getLSB() const;


    // Utilities
    static std::string toString(uint64_t bb); // convert bitboard into string of 1s and .
    void print() const; // print visual board to console
};


// assign each position in bitboard to a position on chess board
enum Square{
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    NO_SQUARE
};
