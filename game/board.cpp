#include "board.hpp"
#include "../engine/nnue.hpp"
#include <iostream>
#include <sstream>
#include <cctype>



// Constructor 
Board::Board(){
    // initialize by clearing board
    clear();
}


void Board::clear(){
    // Clear all piece bitboards
    for(int i = 0; i < 12; i++)
        pieces[i] = Bitboard(0ULL);

    // Clear occupancy bitboards
    for(int i = 0; i < 3; i++)
        occupancy[i] = Bitboard(0ULL);

    turn = WHITE;
    enPassantSquare = NO_SQUARE; // enum value from bitboard.hpp
    castlingRights = 0;
}

int Board::calculate_index(int sq, int pt, bool side, bool perspective) {
	if (perspective) {
		side = 1-side;
		sq = (sq ^ 56);
	}
	return side * 64 * 6 + pt * 64 + sq;
}



void Board::build_accumulators(const Board& board, Accumulator& us, Accumulator& them) {
    init_accumulator(us, g_net);
    init_accumulator(them, g_net);

    for (int sq = 0; sq < 64; sq++) {

        Piece p = board.getPiece(sq);
        if (p == NO_PIECE) continue; 
   
        if (p < 6){  // white piece: P,N,B,R,Q,K = 0..5
            us.add_feature(g_net, calculate_index(sq, p, 0, 0));
            them.add_feature(g_net, calculate_index(sq, p, 0, 1));
            std::cout<<p<<" s "<<calculate_index(sq, p, 0, 0)<<" "<<calculate_index(sq, p, 0, 1)<<"\n";
        }
        else{         // black piece: p,n,b,r,q,k = 6..11
            us.add_feature(g_net, calculate_index(sq, p-6, 1, 0));
            them.add_feature(g_net, calculate_index(sq, p-6, 1, 1));
            std::cout<<p-6<<" "<<calculate_index(sq, p-6, 1, 0)<<" "<<calculate_index(sq, p-6, 1, 1)<<"\n";
        }
    }
    for (int i = 0; i < 16; i++){
        std::cout << "  [" << i << "] = " << us.vals[i] << "\n";
    }
    for (int i = 0; i < 16; i++){
        std::cout << "  [" << i << "] = " << them.vals[i] << "\n";
    }
}



// Set and Remove Pieces
void Board::setPiece(int square, Piece piece){
    if(square < A1 || square > H8) return; // ensure valid square
    pieces[piece].setBit(square);
    if(piece != NO_PIECE){
        if(piece < 6){
            us.add_feature(g_net, calculate_index(square, p, 0, 0));
            them.add_feature(g_net, calculate_index(square, p, 0, 1));
        }
        else{
            us.add_feature(g_net, calculate_index(square, p-6, 0, 0));
            them.add_feature(g_net, calculate_index(square, p-6, 0, 1));
        }
    }
}

void Board::removePiece(int square, Piece piece){
    if(square < A1 || square > H8) return; // ensure valid square
    pieces[piece].clearBit(square);
    if(piece != NO_PIECE){
        if(piece < 6){
            us.remove_feature(g_net, calculate_index(square, p, 0, 0));
            them.remove_feature(g_net, calculate_index(square, p, 0, 1));
        }
        else{
            us.remove_feature(g_net, calculate_index(square, p-6, 1, 0));
            them.remove_feature(g_net, calculate_index(square, p-6, 1, 1));
        }
    }
}

// Get Piece at Square 
Piece Board::getPiece(int square) const {
    if(square < A1 || square > H8) return NO_PIECE; // ensure valid square
    for(int p = P; p <= k; p++){
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

// check if given square is attacked by given side
bool Board::isSquareAttacked(int square, int bySide) const {
    uint64_t occ = occupancy[BOTH].board; // occupancy bitboard of all pieces

    // --- Pawns ---
    /*
    A pawn attacks the square to its top left or right. By the mapping
    of position to `square` = [0,...,63], a pawn will attack our current square
    if its to the lower left or right of our square. This corresponds to square-7
    and square-9 respectively for white. Thus, we check if a pawn exists there
    by ANDing bitboard of that square with the white pawn's bitboard. For black
    pawns, it's just the same thing but with different cooridnates (55,57,+9,+7)
    since it's on the opposite side.
    */
    // Also must check for wrap around error
    int file = square % 8;
    if(bySide == WHITE){
        if(file > 0 && square >= 9 && (pieces[P].board & (1ULL << (square - 9)))) return true;
        if(file < 7 && square >= 7 && (pieces[P].board & (1ULL << (square - 7)))) return true;
    }
    else{
        if(file < 7 && square <= 55 && (pieces[p].board & (1ULL << (square + 9)))) return true;
        if(file > 0 && square <= 57 && (pieces[p].board & (1ULL << (square + 7)))) return true;
    }



    // --- Knights ---
    uint64_t knights = pieces[bySide == WHITE ? N : n].board;
    
    // Offset = piece can move from square to square + Offset[i]
    static const int knightOffsets[8] = {17, 15, 10, 6, -17, -15, -10, -6}; 
    
    // go through each knightOffset and check if there is an opponent knight 
    // there that can move to our square 
    // check out of bounds and wrap around error
    for (int o : knightOffsets) {
        int to = square + o;
        if (to >= 0 && to < 64 && (knights & (1ULL << to)) && std::abs((to % 8) - (square % 8)) <= 2)
            return true;
    }

    // --- King (adjacent squares) ---
    uint64_t king = pieces[bySide == WHITE ? K : k].board;
    static const int kingOffsets[8] = {8, -8, 1, -1, 9, 7, -9, -7};
    
    // go through each kingOffset and check if the opponent king can 
    // move to our square 
    for (int o : kingOffsets) {
        int to = square + o;
        // check out of bounds and wrap around error
        if (to >= 0 && to < 64 && (king & (1ULL << to)) && std::abs((to % 8) - (square % 8)) <= 1)
            return true;
    }

    // --- Diagonal attacks (bishops/queens) ---
    // Bitboard of bishops + queens
    uint64_t diagAttackers = pieces[bySide == WHITE ? B : b].board |
                             pieces[bySide == WHITE ? Q : q].board;
    int diagDirs[4] = {9, 7, -9, -7}; // values to add or subtract to get to next square
    
    // Go through each direction (NE,NW,SE,SW)
    for (int dir : diagDirs) {
        // for each square that is on the board, that is a valid diagonal move
        for (int sq = square + dir;
            sq >= 0 && sq < 64 &&
            std::abs((sq % 8) - ((sq - dir) % 8)) == 1;
            sq += dir) {
            // check if there is a valid attacker (bishop or queen) there
            uint64_t mask = 1ULL << sq;
            if (occ & mask) { // a piece is there
                if (diagAttackers & mask){ // opponent piece
                    return true;
                }
                else{ // our own piece, so we don't have to continue anymore
                    break;
                }
                
            }
        }
    }

    // --- Straight attacks (rooks/queens) ---
    // Bitboard of rooks + queens
    uint64_t lineAttackers = pieces[bySide == WHITE ? R : r].board |
                             pieces[bySide == WHITE ? Q : q].board; 
    int lineDirs[4] = {8, -8, 1, -1}; // values to add or subtract to get to next square
    
    // Go through each direction (N,E,S,W)
    for(int dir : lineDirs){
        // for each square that is on the board, that is a valid straight move
        for (int sq = square + dir;
            sq >= 0 && sq < 64 &&
            std::abs((sq % 8) - ((sq - dir) % 8)) <= 1;
            sq += dir) {
            // check if there is a valid attacker (rook or queen) there

            if(occ & (1ULL << sq)){
                if(lineAttackers & ( 1ULL << sq)){ // opponent piece
                    return true;
                }
                else{ // our own piece, so we don't have to continue anymore
                    break;
                }
            }
        }
    }

    return false; // no attacker found
}

// Check if given side's king is in check
bool Board::isKingInCheck(int side) const{
    uint64_t kingBB = pieces[side == WHITE ? K : k].board;
    if(!kingBB) return false; // no king
    int kingSq = __builtin_ctzll(kingBB); // get bit of king's square
    return isSquareAttacked(kingSq, !side); // check if king's square is under attack
}


// Apply a move to the board
bool Board::makeMove(const Move& move){
    int from = move.from;
    int to = move.to;
    Piece piece = move.piece; // piece on current square
    int side = turn;
    
    // --- Reset en passant ---
    enPassantSquare = NO_SQUARE;

    // --- Handle captures ---
    if(move.flag == CAPTURE || move.flag == EN_PASSANT){
        int capSquare = to;

        // En passant capture happens right behind target (-8 or +8)
        if (move.flag == EN_PASSANT)
            capSquare += (side == WHITE ? -8 : 8);

        Piece capturedPiece = getPiece(capSquare); 
        if (capturedPiece != NO_PIECE)
            removePiece(capSquare, capturedPiece);
    }

    // --- Move the piece ---
    removePiece(from, piece);
    removePiece(to, move.captured);
    // --- Move logic ---
    switch (move.flag){
        case DOUBLE_PAWN_PUSH:
            enPassantSquare = (side == WHITE) ? (to - 8) : (to + 8);
            setPiece(to, piece);
            break;

        case KING_CASTLE:
            // move king
            setPiece(to, piece);

            // Move rook 
            if(to == G1){ // White kingside
                removePiece(H1, R); 
                setPiece(F1, R); 
            } 
            if(to == G8){ // Black kingside
                removePiece(H8, r); 
                setPiece(F8, r); 
            } 
            break;
        case QUEEN_CASTLE:
            // move king
            setPiece(to, piece);

            // Move rook
            if(to == C1){ // White queenside
                removePiece(A1, R); 
                setPiece(D1, R); 
            } 
            if(to == C8){ // Black queenside
                removePiece(A8, r); 
                setPiece(D8, r); 
            } 
            break;

        case PROMOTION_QUEEN:
        case PROMOTION_ROOK:
        case PROMOTION_BISHOP:
        case PROMOTION_KNIGHT:
            {
                // Replace pawn with new piece
                Piece promoPiece;
                switch(move.flag){
                    case PROMOTION_QUEEN: promoPiece = (side == WHITE ? Q : q); break;
                    case PROMOTION_ROOK: promoPiece = (side == WHITE ? R : r); break;
                    case PROMOTION_BISHOP: promoPiece = (side == WHITE ? B : b); break;
                    case PROMOTION_KNIGHT: promoPiece = (side == WHITE ? N : n); break;
                }
                setPiece(to, promoPiece);
            }
            break;

        default:
            // normal
            setPiece(to, piece);
            break;
    }

    // --- Update castling rights ---
    // King moved
    if (piece == K) castlingRights &= ~3;     // remove white K/Q rights
    if (piece == k) castlingRights &= ~12;    // remove black k/q rights

    // Rook moved or captured
    if (from == H1 || to == H1) castlingRights &= ~1; // remove white K
    if (from == A1 || to == A1) castlingRights &= ~2; // remove white Q
    if (from == H8 || to == H8) castlingRights &= ~4; // remove black k
    if (from == A8 || to == A8) castlingRights &= ~8; // remove black q

    // --- Switch side ---
    turn = (turn == WHITE ? BLACK : WHITE);

    updateOccupancy();
    return true;
}

// unmake a move to the board
bool Board::unmakeMove(const Move& move){
    int from = move.from;
    int to = move.to;
    Piece piece = move.piece; // piece on current square
    Piece capture = move.captured;
    // --- Switch side ---
    turn = (turn == WHITE ? BLACK : WHITE);
    int side = turn;
    
    // --- Reset en passant ---
    enPassantSquare = move.prevEnPassantSquare;
    // --- Update castling rights ---
    castlingRights = move.prevCastlingRights;

    

    

    // --- Move logic ---
    switch (move.flag){
        case DOUBLE_PAWN_PUSH:
            removePiece(to, piece);
            break;

        case KING_CASTLE:
            // move king
            removePiece(to, piece);

            // Move rook 
            if(to == G1){ // White kingside
                removePiece(F1, R);
                setPiece(H1, R); 
            } 
            if(to == G8){ // Black kingside
                removePiece(F8, r); 
                setPiece(H8, r); 
            } 
            break;
        case QUEEN_CASTLE:
            // move king
            removePiece(to, piece);

            // Move rook
            if(to == C1){ // White queenside
                removePiece(D1, R);
                setPiece(A1, R); 
            } 
            if(to == C8){ // Black queenside
                removePiece(D8, r); 
                setPiece(A8, r); 
            } 
            break;

        case PROMOTION_QUEEN:
        case PROMOTION_ROOK:
        case PROMOTION_BISHOP:
        case PROMOTION_KNIGHT:
            {
                // Replace pawn with new piece
                Piece promoPiece;
                switch(move.flag){
                    case PROMOTION_QUEEN: promoPiece = (side == WHITE ? Q : q); break;
                    case PROMOTION_ROOK: promoPiece = (side == WHITE ? R : r); break;
                    case PROMOTION_BISHOP: promoPiece = (side == WHITE ? B : b); break;
                    case PROMOTION_KNIGHT: promoPiece = (side == WHITE ? N : n); break;
                }
                removePiece(to, promoPiece);
            }
            break;

        default:
            // normal 
            removePiece(to, piece);
            break;
    }

    setPiece(from, piece);
    
    // --- Handle captures ---
    if(move.flag == CAPTURE || move.flag == EN_PASSANT){
        int capSquare = to;

        // En passant capture happens right behind target (-8 or +8)
        if (move.flag == EN_PASSANT)
            capSquare += (side == WHITE ? -8 : 8);

        if (capture != NO_PIECE)
            setPiece(capSquare, capture);
    }
    else{
        setPiece(to, capture);
    }

    

    updateOccupancy();
    return true;
}



// Decrypt FEN and load it onto the board
void Board::loadFEN(const std::string& fen){
    // Example: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

    clear(); // clear board
    std::istringstream ss(fen); // turn string into input string stream (so we can treat like cin)
    std::string boardPart, side, castling, enPassant;
    int halfmove = 0, fullmove = 1;

    // read from string (like cin, input separated by space)
    ss >> boardPart >> side >> castling >> enPassant >> halfmove >> fullmove; 

    int square = A8; // start from top-left corner
    for(char c : boardPart){
        if(c == '/'){
            square -= 16; // move to next rank down
        }
        else if (isdigit(c)){
            square += c - '0'; // skip empty squares
        }
        else{ // piece here (not empty)
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
            setPiece(square, piece); // set square to this piece
            square++; // move to next square
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
    if(enPassant != "-"){
        int file = enPassant[0] - 'a';
        int rank = enPassant[1] - '1';
        enPassantSquare = rank * 8 + file;
    } 
    else{
        enPassantSquare = NO_SQUARE;
    }

    updateOccupancy();
}

// Print Board
void Board::printBoard() const{
    for(int rank = 7; rank >= 0; rank--){
        std::cout<<rank+1<<" ";
        for(int file = 0; file < 8; file++){
            int sq = rank * 8 + file;
            Piece x = getPiece(sq);
            char c = '.';
            switch (x){
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
