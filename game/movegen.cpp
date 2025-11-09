#include "movegen.hpp"
#include <iostream>

// --- Check if positions a and b are in the same file (vertical) ---
inline bool sameFile(int a, int b){ 
    return a % 8 == b % 8;
}

// --- Check if positions a and b are in the same rank (horizontal) ---
inline bool sameRank(int a, int b){
    return a / 8 == b / 8;
}


// --- Main move generation entry ---
std::vector<Move> MoveGenerator::generateMoves(const Board& board){
    std::vector<Move> moves; // all moves physically possible (no king checks)

    // generate all moves and store them in `moves`
    generatePawnMoves(board, moves);
    generateKnightMoves(board, moves);
    generateBishopMoves(board, moves);
    generateRookMoves(board, moves);
    generateQueenMoves(board, moves);
    generateKingMoves(board, moves);


    std::vector<Move> legal; // all moves that are legal (check for king checks)

    // If our king is attacked after this move, it's illegal
    for(auto& move : moves){
        Board copy = board; // create a copy of board
        copy.makeMove(move); // make move 
        if (!copy.isKingInCheck(board.turn)){ // doesn't leave king in check
            legal.push_back(move); // move is legal
        }  
    }
    return legal;
}

// --- PAWN MOVES ---
void MoveGenerator::generatePawnMoves(const Board& board, std::vector<Move>& moves){
    Turn side = board.turn;
    // bitboard of all pawns (board.pieces[P] = bitboard, so we do bitboard.board to get the bitboard value) 
    uint64_t pawns = board.pieces[side == WHITE ? P : p].board;  

    // Pawn move offsets (depend on color) = {white offset, black offset}
    const int pawnPush[2] = {8, -8}; // go up 1 square: white moves +8, black -8
    const int pawnDoublePush[2] = {16, -16}; // go up 2 squares 

    int direction = (side == WHITE) ? 8 : -8; // sq + direction = next square that pawn moves to
    int startRank = (side == WHITE) ? 1 : 6; // start rank of pawn (0 indexed)
    int promotionRank = (side == WHITE) ? 6 : 1; // rank before promotion

    // go though all pawns
    while(pawns){ // after going through all pawns, pawns bitboard = 0
        int from = __builtin_ctzll(pawns); // get index of least significant bit (pawn square)
        pawns &= pawns - 1; // clear least significant b

        int to = from + direction; // move one to direction

        // move up 1 square
        if(!(board.occupancy[BOTH].board & (1ULL << to))){ // front square not occupied
            // Promotion
            if(from / 8 == promotionRank){ // if its at rank before promotion
                // Add four promotion options
                moves.emplace_back(from, to, PROMOTION_QUEEN, P);
                moves.emplace_back(from, to, PROMOTION_ROOK, P);
                moves.emplace_back(from, to, PROMOTION_BISHOP, P);
                moves.emplace_back(from, to, PROMOTION_KNIGHT, P);
            }
            else{ // doesn't promote
                moves.emplace_back(from, to, QUIET, (side == WHITE ? P : p));
            }

            // Double push
            if(from / 8 == startRank){ // its at starting rank, double pushed allowed
                int doubleTo = from + 2 * direction; // square after double pushing
                if(!(board.occupancy[BOTH].board & (1ULL << doubleTo))){ // front two squares not occupied 
                    moves.emplace_back(from, doubleTo, DOUBLE_PAWN_PUSH, (side == WHITE ? P : p));
                }
            }
        }

        // Captures
        for(int captureDir : {7, 9}){
            int toCap = (side == WHITE) ? from + captureDir : from - captureDir; // capture by side
            // wrap around error
            int fromFile = from % 8;
            int toFile = toCap % 8;

            if(toCap < 0 || toCap >= 64 || abs(toFile - fromFile) > 1) continue; // capture is invalid (goes out of board)
            if(board.occupancy[!side].board & (1ULL << toCap)){ // there exists piece for capture
                moves.emplace_back(from, toCap, CAPTURE, (side == WHITE ? P : p));
            }
        }


        // En passant
        if(board.enPassantSquare != NO_SQUARE){
            int epTarget = board.enPassantSquare;
            int leftCap = (side == WHITE) ? from + 7 : from - 9; // top left pawn square
            int rightCap = (side == WHITE) ? from + 9 : from - 7; // top right pawn square
            // wrap around error
            int fromFile = from % 8;
            int toFile = epTarget % 8;
            if(epTarget < 0 || epTarget >= 64 || abs(toFile - fromFile) > 1) continue;
            if(epTarget == leftCap || epTarget == rightCap){ // enPassant exists at square
                moves.emplace_back(from, epTarget, EN_PASSANT, P);
            }
        }

    }
}


// --- KNIGHT MOVES ---
void MoveGenerator::generateKnightMoves(const Board& board, std::vector<Move>& moves) {
    Turn side = board.turn;
    uint64_t knights = board.pieces[side == WHITE ? N : n].board;
    
    // Knight move offsets (in board index difference)
    int knightOffsets[8] = {17, 15, 10, 6, -6, -10, -15, -17};

    // go through all knights
    while(knights){
        // delete knight
        int from = __builtin_ctzll(knights);
        knights &= knights - 1;

        // go through each direction
        for(int offset : knightOffsets){
            int to = from + offset; // square it can go to

            // Skip if out of bounds (like wrapping from left to right)
            int fromFile = from % 8;
            int toFile = to % 8;
            if(to < 0 || to >= 64 || abs(toFile - fromFile) > 2)
                continue;

            // Cannot move into own piece
            if(board.occupancy[side].board & (1ULL << to)) continue;

            // Capture or quiet
            if(board.occupancy[!side].board & (1ULL << to))
                moves.emplace_back(from, to, CAPTURE, (side == WHITE ? N : n));
            else
                moves.emplace_back(from, to, QUIET, (side == WHITE ? N : n));
        }
    }
}



// --- BISHOP MOVES ---
void MoveGenerator::generateBishopMoves(const Board& board, std::vector<Move>& moves){
    int directions[4] = {9, 7, -9, -7};
    Turn side = board.turn;
    uint64_t bishops = board.pieces[side == WHITE ? B : b].board;

    // go through all bishops
    while(bishops){
        // delete bishop
        int from = __builtin_ctzll(bishops);
        bishops &= bishops - 1;

        //go that direction until end
        for(int dir : directions){
            int to = from;

            // go that direction until end
            while(true){
                int next = to + dir;

                // Break if out of board range
                if(next < 0 || next >= 64)
                    break;

                // Prevent wrapping across files
                int fromFile = to % 8;
                int toFile = next % 8;
                if(abs(toFile - fromFile) > 1)
                    break;

                if(board.occupancy[side].board & (1ULL << next)) // blocked by own piece
                    break; 

                if(board.occupancy[!side].board & (1ULL << next)){ // can capture piece
                    moves.emplace_back(from, next, CAPTURE, (side == WHITE ? B : b));
                    break; // stop after capture
                }

                moves.emplace_back(from, next, QUIET, (side == WHITE ? B : b)); // add move
                to = next; // go to next move
            }
        }
    }
}


// --- ROOK MOVES ---
void MoveGenerator::generateRookMoves(const Board& board, std::vector<Move>& moves){
    int directions[4] = {8, -8, 1, -1}; // only horizontal
    int side = board.turn;
    uint64_t rooks = board.pieces[side == WHITE ? R : r].board;

    // go through all rooks
    while(rooks){
        // delete rook
        int from = __builtin_ctzll(rooks);
        rooks &= rooks - 1;

        // go through all directions
        for(int dir : directions){
            int to = from;

            // go that direction until end
            while(true){
                int next = to + dir;

                // Out of board
                if(next < 0 || next >= 64)
                    break;

                // Prevent horizontal wrap
                if((dir == 1 || dir == -1) && (next / 8 != to / 8))
                    break;

                if(board.occupancy[side].board & (1ULL << next)) // blocked by own piece
                    break;

                if(board.occupancy[!side].board & (1ULL << next)) { // can capture piece
                    moves.emplace_back(from, next, CAPTURE, (side == WHITE ? R : r));
                    break;
                }

                moves.emplace_back(from, next, QUIET, (side == WHITE ? R : r)); // add move
                to = next; // go to next move
            }
        }
    }
}




// --- QUEEN MOVES ---
void MoveGenerator::generateQueenMoves(const Board& board, std::vector<Move>& moves){
    int directions[8] = {8, -8, 1, -1, 9, 7, -9, -7}; // all 8 directions
    int side = board.turn;
    uint64_t queens = board.pieces[side == WHITE ? Q : q].board;

    // go through all queens
    while(queens){ 
        // delete queen
        int from = __builtin_ctzll(queens);
        queens &= queens - 1;

        // go through all directions
        for(int dir : directions){
            int to = from;

            // go that direction until end
            while(true){
                int next = to + dir;

                if(next < 0 || next >= 64) // not on board
                    break;

                // Horizontal wrap check
                if((dir == 1 || dir == -1 || dir == 7 || dir == 9 || dir == -7 || dir == -9) &&
                    abs((next % 8) - (to % 8)) > 1)
                    break;

                if(board.occupancy[side].board & (1ULL << next)) // blocked by own piece
                    break;

                if(board.occupancy[!side].board & (1ULL << next)){ // capture opposite piece
                    moves.emplace_back(from, next, CAPTURE, (side == WHITE ? Q : q));
                    break; 
                }

                moves.emplace_back(from, next, QUIET, (side == WHITE ? Q : q)); // move to square
                to = next; // next square in that direction
            }
        }
    }
}


// --- KING MOVES ---
void MoveGenerator::generateKingMoves(const Board& board, std::vector<Move>& moves){
    int side = board.turn;
    uint64_t king = board.pieces[side == WHITE ? K : k].board;

    if (!king) return; // no king

    int from = __builtin_ctzll(king); // `from` square 
    int directions[8] = {8, -8, 1, -1, 9, 7, -9, -7}; // offsets

    // go through all 8 directions
    for(int dir : directions){
        int to = from + dir;

        if (to < 0 || to >= 64) continue;

        // Prevent wrapping horizontally
        int fromFile = from % 8;
        int toFile = to % 8;
        if (abs(toFile - fromFile) > 1) continue; // invalid move

        if (board.occupancy[side].board & (1ULL << to)) continue; // can't land on own piece

        if (board.occupancy[!side].board & (1ULL << to)) // can capture piece
            moves.emplace_back(from, to, CAPTURE, (side == WHITE ? K : k));
        else // moves there, no piece to capture
            moves.emplace_back(from, to, QUIET, (side == WHITE ? K : k));
    }



    // --- CASTLING ---
    uint64_t all = board.occupancy[BOTH].board;

    if(side == WHITE){
        // Kingside (K)
        if(board.castlingRights & 1){
            if(!(all & ((1ULL << F1) | (1ULL << G1))) &&
                (!board.isSquareAttacked(F1, !side) 
              && !board.isSquareAttacked(G1, !side))){ // castling path is empty and not attacked
                moves.emplace_back(E1, G1, KING_CASTLE, K);
            }
        }
        // Queenside (Q)
        if(board.castlingRights & 2){
            if(!(all & ((1ULL << D1) | (1ULL << C1) | (1ULL << B1))) &&
                (!board.isSquareAttacked(D1, !side) 
              && !board.isSquareAttacked(C1, !side))){ // castling path is empty and not attacked
                moves.emplace_back(E1, C1, QUEEN_CASTLE, K);
            }
        }
    } 
    else{
        // Kingside (k)
        if(board.castlingRights & 4){
            if(!(all & ((1ULL << F8) | (1ULL << G8)))&&
                (!board.isSquareAttacked(F8, !side) 
              && !board.isSquareAttacked(G8, !side))){ // castling path is empty and not attacked
                moves.emplace_back(E8, G8, KING_CASTLE, k);
            }
        }
        // Queenside (q)
        if(board.castlingRights & 8){
            if(!(all & ((1ULL << D8) | (1ULL << C8) | (1ULL << B8)))&&
                (!board.isSquareAttacked(D8, !side) 
              && !board.isSquareAttacked(C8, !side))){ // castling path is empty and not attacked
                moves.emplace_back(E8, C8, QUEEN_CASTLE, k);
            }
        }
    }

}
