#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include "game/bitboard.hpp"
#include "game/board.hpp"
#include "game/movegen.hpp"
#include "engine/search.hpp"

static Board board;
std::string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// Engine options
struct EngineOptions {
    int moveOverhead = 30; // ms
    int threads = 1;
    int hash = 128;        // MB
};

EngineOptions engineOptions;

void applyMoves(Board& board, const std::string& movesPart) {
    std::istringstream ss(movesPart);
    std::string moveStr;
    while (ss >> moveStr) {
        int fromFile = moveStr[0] - 'a';
        int fromRank = moveStr[1] - '1';
        int toFile   = moveStr[2] - 'a';
        int toRank   = moveStr[3] - '1';
        int from = fromRank * 8 + fromFile;
        int to = toRank * 8 + toFile;
        char promo = 0;
        if (moveStr.length() == 5) promo = moveStr[4];

        std::vector<Move> legalMoves = MoveGenerator::generateMoves(board);
        for (auto& move : legalMoves) {
            char movePromo = 0;
            switch(move.flag){
                case PROMOTION_QUEEN: movePromo = 'q'; break;
                case PROMOTION_ROOK: movePromo = 'r'; break;
                case PROMOTION_BISHOP: movePromo = 'b'; break;
                case PROMOTION_KNIGHT: movePromo = 'n'; break;
            }
            if (move.from == from && move.to == to && (promo == 0 || movePromo == promo)) {
                board.makeMove(move);
                break;
            }
        }
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string line;
    while (std::getline(std::cin, line)) {

        if (line == "uci") {
            std::cout << "id name MyEngine\n";
            std::cout << "id author MyName\n";

            // Declare options
            std::cout << "option name Move Overhead type spin default 30 min 0 max 5000\n";
            std::cout << "option name Threads type spin default 1 min 1 max 16\n";
            std::cout << "option name Hash type spin default 128 min 1 max 4096\n";

            std::cout << "uciok\n" << std::flush;
        }
        else if (line == "isready") {
            std::cout << "readyok\n" << std::flush;
        }
        else if (line.rfind("setoption name", 0) == 0) {
            // Parse UCI setoption commands
            std::istringstream iss(line);
            std::string token, name, value;
            iss >> token >> token; // skip "setoption name"
            iss >> name;

            std::string rest;
            std::getline(iss, rest);
            std::string valueStr = rest.substr(rest.find("value") + 6);

            if (name == "Move") { // Move Overhead
                engineOptions.moveOverhead = std::stoi(valueStr);
            } else if (name == "Threads") {
                engineOptions.threads = std::stoi(valueStr);
            } else if (name == "Hash") {
                engineOptions.hash = std::stoi(valueStr);
            }
        }
        else if (line == "ucinewgame") {
            board.loadFEN(startFEN);
        }
        else if (line.rfind("position", 0) == 0) {
            std::istringstream iss(line);
            std::string token;
            iss >> token; // 'position'

            iss >> token;
            if (token == "startpos") {
                board.loadFEN(startFEN);
            } else if (token == "fen") {
                std::string fen, tmp;
                int fields = 0;
                while (fields < 6 && iss >> tmp) {
                    fen += tmp + " ";
                    fields++;
                }
                board.loadFEN(fen);
            }

            if (iss >> token && token == "moves") {
                std::string movesPart;
                std::getline(iss, movesPart);
                applyMoves(board, movesPart);
            }
        }
        else if (line.rfind("go", 0) == 0) {
            int depth = 5;
            std::istringstream iss(line);
            std::string tok;
            while (iss >> tok) {
                if (tok == "depth") iss >> depth;
            }

            Search search(depth);
            SearchResult best = search.findBestMove(board);
            Move bestMove = best.bestMove;
            std::cout << "bestmove " << bestMove.toString() << "\n" << std::flush;
        }
        else if (line == "quit") {
            break;
        }
    }

    return 0;
}
