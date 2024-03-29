#include <sstream>
#include "board.h"

using namespace Constants;

Board::Board() : cells_{{false}}, currentScore_(0) {}

/*
 * Loop through each of the locations or "cells" on the board and fill
 * any cells populated by a piece with the appropriate color. Show
 * the score at the bottom of the window (rows filled).
 */
void Board::draw(SDL_Renderer *renderer, TTF_Font *font)
{
    displayScore(renderer, font);
    SDL_SetRenderDrawColor(
        renderer,
        /* Light Gray: */ 140, 140, 140, 255);
    for (int column = 0; column < NUM_COLUMNS; ++column)
    {
        for (int row = 0; row < NUM_ROWS; ++row)
        {
            if (cells_[column][row])
            {
                SDL_Rect rect{
                    column * Offset + 1,
                    row * Offset + 1,
                    Offset - 2,
                    Offset - 2};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

/*
 * Determine if the specified piece has collided with the bounding board.
 * Returns true if a collision was detected.
 */
bool Board::isCollision(const Piece &piece) const
{
    for (int column = 0; column < TETRINIMO_SIZE; ++column)
    {
        for (int row = 0; row < TETRINIMO_SIZE; ++row)
        {
            if (piece.isBlock(column, row))
            {
                int columnTarget = piece.getColumn() + column;
                int rowTarget = piece.getRow() + row;
                if (
                    columnTarget < 0 || columnTarget >= NUM_COLUMNS || rowTarget < 0 || rowTarget >= NUM_ROWS)
                {
                    return true;
                }
                if (cells_[columnTarget][rowTarget])
                    return true;
            }
        }
    }
    return false;
}

/*
 * Handles a piece collision on the board and recalculates the cells
 * to determine if any rows have been filled.
 */
void Board::unite(const Piece &piece)
{
    for (int column = 0; column < TETRINIMO_SIZE; ++column)
    {
        for (int row = 0; row < TETRINIMO_SIZE; ++row)
        {
            if (piece.isBlock(column, row))
            {
                int columnTarget = piece.getColumn() + column;
                int rowTarget = piece.getRow() + row;
                cells_[columnTarget][rowTarget] = true;
            }
        }
    }

    // Continuously loops through each of the rows until no full rows are
    // detected and ensures the full rows are collapsed and non-full rows
    // are shifted accordingly:
    while (areFullRowsPresent())
    {
        for (int row = NUM_ROWS - 1; row >= 0; --row)
        {
            if (isRowFull(row))
            {
                updateOffsetRow(row);
                currentScore_ += 1;
                for (int column = 0; column < NUM_COLUMNS; ++column)
                {
                    cells_[column][0] = false;
                }
            }
        }
    }
}

/*
 * Returns true if the specified row has all of the columns filled with
 * pieces.
 */
bool Board::isRowFull(int row)
{
    for (int column = 0; column < NUM_COLUMNS; ++column)
    {
        if (!cells_[column][row])
            return false;
    }
    return true;
}

/*
 * Returns true if there are full rows present on the board.
 */
bool Board::areFullRowsPresent()
{
    for (int row = NUM_ROWS - 1; row >= 0; --row)
    {
        if (isRowFull(row))
            return true;
    }
    return false;
}

/*
 * Updates the contents of the fullRow number with the contents of the
 * row above it (to collapse the filled row).
 */
void Board::updateOffsetRow(int fullRow)
{
    for (int column = 0; column < NUM_COLUMNS; ++column)
    {
        for (int rowOffset = fullRow - 1; rowOffset >= 0; --rowOffset)
        {
            cells_[column][rowOffset + 1] =
                cells_[column][rowOffset];
        }
    }
}

/*
 * Displays the current score at the bottom of the playing area.
 */
void Board::displayScore(SDL_Renderer *renderer, TTF_Font *font)
{
    std::stringstream message;
    message << "ROWS: " << currentScore_;
    SDL_Color white = {255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(
        font,
        message.str().c_str(),
        white);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(
        renderer,
        surface);
    SDL_Rect messageRect{20, GAME_HEIGHT + 15, surface->w, surface->h};
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, nullptr, &messageRect);
    SDL_DestroyTexture(texture);
}