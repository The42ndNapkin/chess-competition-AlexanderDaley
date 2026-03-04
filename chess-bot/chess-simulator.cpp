#include "chess-simulator.h"
#include "chess-simulator.h"
#include "chess-simulator.h"
#include "chess-simulator.h"
#include "chess-simulator.h"
#include "chess-simulator.h"
#include "chess-simulator.h"
#include "chess-simulator.h"
// disservin's lib. drop a star on his hard work!
// https://github.com/Disservin/chess-library
#include "chess.hpp"
#include <random>
#include <chrono>
using namespace ChessSimulator;

std::chrono::steady_clock::time_point searchStart;
int timeLimit;
bool timeUp = false;

const int PAWN_TABLE[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
                            50, 50, 50, 50, 50, 50, 50, 50,
                            10, 10, 20, 30, 30, 20, 10, 10,
                            5, 5, 10, 25, 25, 10, 5, 5,
                            0, 0, 0, 20, 20, 0, 0, 0,
                            5, -5, -10, 0, 0, -10, -5, 5,
                            5, 10, 10, -20, -20, 10, 10, 5,
                            0, 0, 0, 0, 0, 0, 0, 0 };

const int KNIGHT_TABLE[64] = { -50,-40,-30,-30,-30,-30,-40,-50,
                            -40,-20,  0,  0,  0,  0,-20,-40,
                            -30,  0, 10, 15, 15, 10,  0,-30,
                            -30,  5, 15, 20, 20, 15,  5,-30,
                            -30,  0, 15, 20, 20, 15,  0,-30,
                            -30,  5, 10, 15, 15, 10,  5,-30,
                            -40,-20,  0,  5,  5,  0,-20,-40,
                            -50,-40,-30,-30,-30,-30,-40,-50 };

const int BISHOP_TABLE[64] = { -20,-10,-10,-10,-10,-10,-10,-20,
                                -10,  0,  0,  0,  0,  0,  0,-10,
                                -10,  0,  5, 10, 10,  5,  0,-10,
                                -10,  5,  5, 10, 10,  5,  5,-10,
                                -10,  0, 10, 10, 10, 10,  0,-10,
                                -10, 10, 10, 10, 10, 10, 10,-10,
                                -10,  5,  0,  0,  0,  0,  5,-10,
                                -20,-10,-10,-10,-10,-10,-10,-20 };

const int ROOK_TABLE[64] = { 0,  0,  0,  0,  0,  0,  0,  0,
                             5, 10, 10, 10, 10, 10, 10,  5,
                             -5,  0,  0,  0,  0,  0,  0, -5,
                             -5,  0,  0,  0,  0,  0,  0, -5,
                             -5,  0,  0,  0,  0,  0,  0, -5,
                             -5,  0,  0,  0,  0,  0,  0, -5,
                             -5,  0,  0,  0,  0,  0,  0, -5,
                             0,  0,  0,  5,  5,  0,  0,  0 };

const int QUEEN_TABLE[64] = { -20,-10,-10, -5, -5,-10,-10,-20,
                                -10,  0,  0,  0,  0,  0,  0,-10,
                                -10,  0,  5,  5,  5,  5,  0,-10,
                                -5,  0,  5,  5,  5,  5,  0, -5,
                                 0,  0,  5,  5,  5,  5,  0, -5,
                                -10,  5,  5,  5,  5,  5,  0,-10,
                                -10,  0,  5,  0,  0,  0,  0,-10,
                                -20,-10,-10, -5, -5,-10,-10,-20 };

const int KING_TABLE[64] = { -30,-40,-40,-50,-50,-40,-40,-30,
                            -30,-40,-40,-50,-50,-40,-40,-30,
                            -30,-40,-40,-50,-50,-40,-40,-30,
                            -30,-40,-40,-50,-50,-40,-40,-30,
                            -20,-30,-30,-40,-40,-30,-30,-20,
                            -10,-20,-20,-20,-20,-20,-20,-10,
                            10, 10,  0,  0,  0,  0, 10, 10,
                            10, 20, 5,  0,  0, 5, 20, 10 };

std::string ChessSimulator::Move(std::string fen, int timeLimitMs) {
  // create your board based on the board string following the FEN notation
  // search for the best move using minimax / monte carlo tree search /
  // alpha-beta pruning / ... try to use nice heuristics to speed up the search
  // and have better results return the best move in UCI notation you will gain
  // extra points if you create your own board/move representation instead of
  // using the one provided by the library

  // here goes a random movement
  chess::Board board(fen);
  chess::Movelist moves;
  chess::movegen::legalmoves(moves, board);
  searchStart = std::chrono::steady_clock::now();
  timeLimit = timeLimitMs;
  
  if(moves.size() == 0)
    return "";
  
  chess::Move move;

  if (board.sideToMove() == chess::Color::BLACK)
  {
      // get random move
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<> dist(0, moves.size() - 1);
      move = moves[dist(gen)];
  }
  else
  {
      //calc best move using heuristics
      move = findBestMove(board, 100, moves);
  }
  
  return chess::uci::moveToUci(move);
}

//Min/max function with alpha/beta pruning
float ChessSimulator::minimax(const chess::Board board, int depth, float alpha, float beta, bool isMaxingPlayer, chess::Movelist moves)
{
    if (checkTime())
    {
        return eval(board, moves);
    }
    if (depth == 0)
    {
        return eval(board, moves);
    }

    float bestValue = isMaxingPlayer ? std::numeric_limits<float>::min() : std::numeric_limits<float>::max();


    for (const chess::Move& move : moves)
    {
        //update board state
        chess::Board newBoard = board;
        newBoard.makeMove(move);

        chess::Movelist newMoves;
        chess::movegen::legalmoves(newMoves, newBoard);

        //recursively call minimax
        float value = minimax(newBoard, depth - 1, alpha, beta, !isMaxingPlayer, newMoves);
        
        //update best value and alpha/beta
        if (isMaxingPlayer)
        {
            bestValue = std::max(bestValue, value);
            alpha = std::max(alpha, bestValue);
        }
        else
        {
            bestValue = std::min(bestValue, value);
            beta = std::min(beta, bestValue);
        }

        //Alpha/beta pruning
        if (beta <= alpha)
        {
            break;
        }
        
    }

    return bestValue;
}

//actual function to call for minmaxing the best possible move
chess::Move ChessSimulator::findBestMove(const chess::Board board, int depth, chess::Movelist moves)
{
    float heuristic = -INFINITY;
    chess::Move bestMove = chess::Move();

    for (const chess::Move& move : moves)
    {
        chess::Board newBoard = board;
        newBoard.makeMove(move);
        chess::Movelist newMoves;
        chess::movegen::legalmoves(newMoves, newBoard);

        float value = minimax(newBoard, depth, 0, 0, false, newMoves);
        if (checkTime())
        {
            return bestMove;
        }
        if (value > heuristic)
        {
            heuristic = value;
            bestMove = move;
        }
        
    }

    return bestMove;
}

//Heuristic evaluation function
float ChessSimulator::eval(chess::Board board, chess::Movelist moves)
{
    if (board.sideToMove() == chess::Color::WHITE)
    {
        return (materialScore(board) + mobilityScore(board, moves)) * 1;
    }
    else
    {
        return (materialScore(board) + mobilityScore(board, moves)) * -1;
    }
}



//Evaluates the material each player has left
float ChessSimulator::materialScore(chess::Board board)
{
    float temp = 0.0f;
    //value calcs - compare the number of pieces white has left to the number of pieces black has left
    temp += 2000 * (board.pieces(chess::PieceType::KING, chess::Color::WHITE).count() - board.pieces(chess::PieceType::KING, chess::Color::BLACK).count());
    temp += 9 * (board.pieces(chess::PieceType::QUEEN, chess::Color::WHITE).count() - board.pieces(chess::PieceType::QUEEN, chess::Color::BLACK).count());
    temp += 5 * (board.pieces(chess::PieceType::ROOK, chess::Color::WHITE).count() - board.pieces(chess::PieceType::ROOK, chess::Color::BLACK).count());
    temp += 3 * (board.pieces(chess::PieceType::BISHOP, chess::Color::WHITE).count() - board.pieces(chess::PieceType::BISHOP, chess::Color::BLACK).count());
    temp += 3 * (board.pieces(chess::PieceType::KNIGHT, chess::Color::WHITE).count() - board.pieces(chess::PieceType::KNIGHT, chess::Color::BLACK).count());
    temp += 1 * (board.pieces(chess::PieceType::PAWN, chess::Color::WHITE).count() - board.pieces(chess::PieceType::PAWN, chess::Color::BLACK).count());
    return temp;
}

//Evaluates the player's mobility options
float ChessSimulator::mobilityScore(chess::Board board, chess::Movelist moves)
{
    float temp = 0.0f;
    chess::Movelist tempMoves;
    
    if (moves.size() == 0)
    {
        return 0.0f;
    }

    //weight moves from the player's side
    //weight pawn moves
    chess::movegen::legalmoves(tempMoves, board, 1);
    temp += tempMoves.size();
    //weight knight moves
    chess::movegen::legalmoves(tempMoves, board, 2);
    temp += tempMoves.size() * 4;
    //weight bishop moves
    chess::movegen::legalmoves(tempMoves, board, 4);
    temp += tempMoves.size() * 4;
    //weight rook moves
    chess::movegen::legalmoves(tempMoves, board, 8);
    temp += tempMoves.size() * 2;
    //weight queen moves
    chess::movegen::legalmoves(tempMoves, board, 16);
    temp += tempMoves.size();
    //weight king moves
    chess::movegen::legalmoves(tempMoves, board, 32);
    temp += tempMoves.size();

    //weight moves from opponent's side
    chess::Board oppBoard = board;
    oppBoard.makeNullMove();
    //weight pawn moves
    chess::movegen::legalmoves(tempMoves, oppBoard, 1);
    temp -= tempMoves.size();
    //weight knight moves
    chess::movegen::legalmoves(tempMoves, oppBoard, 2);
    temp -= tempMoves.size() * 4;
    //weight bishop moves
    chess::movegen::legalmoves(tempMoves, oppBoard, 4);
    temp -= tempMoves.size() * 4;
    //weight rook moves
    chess::movegen::legalmoves(tempMoves, oppBoard, 8);
    temp -= tempMoves.size() * 2;
    //weight queen moves
    chess::movegen::legalmoves(tempMoves, oppBoard, 16);
    temp -= tempMoves.size();
    //weight king moves
    chess::movegen::legalmoves(tempMoves, oppBoard, 32);
    temp -= tempMoves.size();

    temp += checkPieceTables(board);
    temp += checkRookOpenFiles(board, moves);
    temp -= checkRookOpenFiles(oppBoard, moves);
    temp += checkCaptures(board, moves);

    return temp;
}

float ChessSimulator::checkPieceTables(chess::Board board)
{
    float total = 0.0f;

    for (int i = 0; i < 64; i++)
    {
        chess::Piece piece = board.at(chess::Square(i));

        if (piece == chess::Piece::NONE)
        {
            continue;
        }

        int add;

        switch (piece.type())
        {
        case (int)chess::PieceType::PAWN:
            add = PAWN_TABLE[i];
            break;
        case (int)chess::PieceType::KNIGHT:
            add = KNIGHT_TABLE[i];
            break;
        case (int)chess::PieceType::BISHOP:
            add = BISHOP_TABLE[i];
            break;
        case (int)chess::PieceType::ROOK:
            add = ROOK_TABLE[i];
            break;
        case (int)chess::PieceType::QUEEN:
            add = QUEEN_TABLE[i];
            break;
        case (int)chess::PieceType::KING:
            add = KING_TABLE[i];
            break;
        }
        
        if (piece.color() == board.sideToMove())
        {
            total += add;
        }
        else
        {
            total += add * -1;
        }
    }
    return total;
}

float ChessSimulator::checkRookOpenFiles(chess::Board board, chess::Movelist moves)
{
    chess::movegen::legalmoves(moves, board, 8);
    if (moves.size() >= board.pieces(chess::PieceType::ROOK, board.sideToMove()).count() * 10)
    {
        return moves.size();
    }
}

float ChessSimulator::checkCaptures(chess::Board board, chess::Movelist moves)
{
    float temp = 0.0f;
    for (chess::Move move : moves)
    {
        if (board.isCapture(move))
        {
            temp += 60.0f;
        }
    }
    board.makeNullMove();
    chess::movegen::legalmoves(moves, board);

    for (chess::Move move : moves)
    {
        if (board.isCapture(move))
        {
            temp -= 60.0f;
        }
    }

    return temp;
}


bool ChessSimulator::checkTime()
{
    auto now = std::chrono::steady_clock::now();

    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - searchStart).count();

    if (elapsed >= timeLimit)
    {
        //timeUp = true;
        return true;
    }

    return false;
}