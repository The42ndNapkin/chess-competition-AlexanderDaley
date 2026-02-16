#pragma once
#include <string>
#include "chess.hpp"

namespace ChessSimulator {
/**
 * @brief Move a piece on the board
 *
 * @param fen The board as FEN
 * @return std::string The move as UCI
 */
std::string Move(std::string fen);
float minimax(const chess::Board board, int depth, float alpha, float beta, bool isMaxingPlayer, chess::Movelist moves);
chess::Move findBestMove(const chess::Board board, int depth, chess::Movelist moves);
float eval(chess::Board board, chess::Movelist moves);
float materialScore(chess::Board board);
float mobilityScore(chess::Board board, chess::Movelist moves);
} // namespace ChessSimulator