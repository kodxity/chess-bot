#include "../game/bitboard.hpp"
#include "../game/board.hpp"
#include "../game/movegen.hpp"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

// Reference: https://www.chessprogramming.org/Perft_Results

int val = 0;
uint64_t perft(Board board, int depth){
	std::vector<Move> legalMoves = MoveGenerator::generateMoves(board);
	
	if(depth==0){
		return 1;
	}
	uint64_t ans = 0;
	for(auto& move : legalMoves){
		// Board copy = board;
		board.makeMove(move);
		val = perft(board, depth-1);
		board.unmakeMove(move);
		// board = copy;
		ans += val;
		/*
		if(depth==3){
			std::cout<<move.toString()<<" "<<val<<" \n";
			val = 0;
		}
		if(board.turn != copy.turn){
			std::cout<<"turn\n";
		}
		if(board.enPassantSquare != copy.enPassantSquare){
			std::cout<<"en passant\n";
		}
		if(board.castlingRights != copy.castlingRights){
			std::cout<<"castle\n";
		}
		for(int j = 0;j<12;j++){
			if(board.pieces[j].board != copy.pieces[j].board){
				std::cout<<j<<" pieces"<<" "<<move.piece<<" "<<move.toString()<<" "<<move.flag<<" "<<board.pieces[j].board<<" "<<copy.pieces[j].board<<" "<<move.captured<<"\n";
			}
		}
		if(move.flag == EN_PASSANT){
			val++;
		}
		
		
		
		*/
		
	}
	return ans;
	
}

uint64_t runPerft(std::string startFEN, int depth){
	Board board;
	board.loadFEN(startFEN);
	val = 0;
	uint64_t ans = perft(board, depth);
	// std::cout<<depth<<" "<<val<<"\n";
	return ans;
}


TEST_CASE("perft") {
	
	
    CHECK(runPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 1) == 20);
	CHECK(runPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2) == 400);
	CHECK(runPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 3) == 8902);
	CHECK(runPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4) == 197281);
	CHECK(runPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5) == 4865609);
	CHECK(runPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 6) == 119060324);
	
    CHECK(runPerft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 1) == 48);
	CHECK(runPerft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 2) == 2039);
	CHECK(runPerft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 3) == 97862);
	CHECK(runPerft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 4) == 4085603);
	CHECK(runPerft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 5) == 193690690);
	// CHECK(runPerft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 6) == 8031647685);

	CHECK(runPerft("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 1) == 14);
	CHECK(runPerft("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 2) == 191);
	CHECK(runPerft("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 3) == 2812);
	CHECK(runPerft("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 4) == 43238);
	CHECK(runPerft("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 5) == 674624);
	CHECK(runPerft("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 6) == 11030083);

   	CHECK(runPerft("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 1) == 6);
	CHECK(runPerft("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 2) == 264);
	CHECK(runPerft("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 3) == 9467);
	CHECK(runPerft("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 4) == 422333);
	CHECK(runPerft("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 5) == 15833292);
	CHECK(runPerft("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 6) == 706045033);
	
	CHECK(runPerft("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 1) == 44);
	CHECK(runPerft("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 2) == 1486);
	CHECK(runPerft("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 3) == 62379);
	CHECK(runPerft("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 4) == 2103487);
	CHECK(runPerft("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 5) == 15833292);
	
	CHECK(runPerft("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 1) == 46);
	CHECK(runPerft("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 2) == 2079);
	CHECK(runPerft("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 3) == 89890);
	CHECK(runPerft("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 4) == 3894594);
	CHECK(runPerft("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 5) == 164075551);
	// CHECK(runPerft("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 6) == 6923051137);
	
}