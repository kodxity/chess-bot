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
		Board copy = board;
        copy.makeMove(move);
		val = perft(copy, depth-1);
		ans += val;
		/*
		if(move.flag == EN_PASSANT){
			val++;
		}
		if(depth==1){
			std::cout<<move.toString()<<val<<" \n";
			val = 0;
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
    CHECK(runPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 0) == 1);
    CHECK(runPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 1) == 20);
	CHECK(runPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2) == 400);
	CHECK(runPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 3) == 8902);
	CHECK(runPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4) == 197281);
	CHECK(runPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5) == 4865609);
	CHECK(runPerft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 6) == 119060324);

}