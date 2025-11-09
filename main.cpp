#include <iostream>
#include <random>
#include "game/bitboard.hpp"
#include "game/board.hpp"
#include "game/movegen.hpp"
int main() {
    /*
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


    // movegen .hpp/.cpp example
    auto moves = MoveGenerator::generateMoves(board);

    for (auto& m : moves)
        std::cout << m.toString() << "\n";

    std::cout << "Total moves: " << moves.size() << "\n";
    */



    // ==================== CHESS GAME ====================
    
    
    Board board;
    // Standard starting FEN
    std::string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    // Load the FEN into the board
    board.loadFEN(startFEN);
    std::cout << "Welcome to your chess engine!\n";
    board.printBoard();

    std::mt19937 rng(std::random_device{}()); // for random engine moves

    while (true) {
        if (board.turn == WHITE) {
            // Player move
            std::string input;
            std::cout << "Enter your move: ";
            std::cin >> input;
            if (input == "quit") break;

            // Parse move
            if (input.size() != 4) {
                std::cout << "Invalid input format.\n";
                continue;
            }

            int fromFile = input[0] - 'a';
            int fromRank = input[1] - '1';
            int toFile   = input[2] - 'a';
            int toRank   = input[3] - '1';

            int from = fromRank * 8 + fromFile;
            int to   = toRank * 8 + toFile;

            std::vector<Move> legalMoves = MoveGenerator::generateMoves(board);
            if (legalMoves.empty()) {
                if (board.isKingInCheck(board.turn))
                    std::cout << "Checkmate! You Lose!\n";
                else
                    std::cout << "Stalemate!\n";
                break;
            }
            for (auto& m : legalMoves)
                std::cout << m.toString() << "\n";
            bool found = false;
            for (auto& move : legalMoves) {
                if (move.from == from && move.to == to) {
                    board.makeMove(move);
                    found = true;
                    break;
                }
            }

            if (!found) {
                std::cout << "Illegal move!\n";
                continue;
            }
        } 
        else {
            // Engine move
            std::vector<Move> legalMoves = MoveGenerator::generateMoves(board);
            for (auto& m : legalMoves)
                std::cout << m.toString() << "\n";
            if (legalMoves.empty()) {
                if (board.isKingInCheck(board.turn))
                    std::cout << "Checkmate! You win!\n";
                else
                    std::cout << "Stalemate!\n";
                break;
            }

            std::uniform_int_distribution<> dist(0, (int)legalMoves.size() - 1);
            Move bestMove = legalMoves[dist(rng)]; // random move for now
            board.makeMove(bestMove);
            std::cout << "Engine plays: " << bestMove.toString() << "\n";
        }

        board.printBoard();
    }

    std::cout << "Game over.\n";
}