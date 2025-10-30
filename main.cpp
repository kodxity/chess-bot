#include <iostream>
#include "game/bitboard.hpp"
#include "game/board.hpp"

int main() {

    // bitboard .hpp/.cpp example
    Bitboard b;
    b.setBit(A2);
    b.setBit(H8);
    b.print();
    std::cout << "Bits set: " << b.countBits() << std::endl;


    // board .hpp/.cpp example
    Board board;
    // Standard starting FEN
    std::string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    // Load the FEN into the board
    board.loadFEN(startFEN);
    // Print the board
    board.printBoard();
}