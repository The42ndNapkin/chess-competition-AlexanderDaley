#include "chess-simulator.h"
#include "chess-simulator.h"
#include "chess-simulator.h"
#include "chess-simulator.h"
// disservin's lib. drop a star on his hard work!
// https://github.com/Disservin/chess-library
#include "chess.hpp"
#include <random>
using namespace ChessSimulator;

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
      move = findBestMove(board, 5, moves);
  }
  
  return chess::uci::moveToUci(move);
}

//Min/max function with alpha/beta pruning
float ChessSimulator::minimax(const chess::Board board, int depth, float alpha, float beta, bool isMaxingPlayer, chess::Movelist moves)
{
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

        if (value > heuristic)
        {
            heuristic = value;
            bestMove = move;
        }
    }

    return bestMove;
}


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
    temp += 200 * (board.pieces(chess::PieceType::KING, chess::Color::WHITE).count() - board.pieces(chess::PieceType::KING, chess::Color::BLACK).count());
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

    return temp;
}