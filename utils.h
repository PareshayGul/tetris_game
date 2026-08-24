#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// ================================================================
//  WINDOW & BOARD DIMENSIONS
// ================================================================
const int WIN_W  = 900;       // window width  in pixels
const int WIN_H  = 700;       // window height in pixels

const int N      = 10;        // board columns
const int M      = 20;        // board rows
const int CELL   = 30;        // pixels per cell  (matches tiles.png block size)

const int BOARD_W = N * CELL;                      // 300 px
const int BOARD_H = M * CELL;                      // 600 px
const int BOARD_X = (WIN_W - BOARD_W) / 2;        // 300 – horizontally centered
const int BOARD_Y = (WIN_H - BOARD_H) / 2;        //  50 – vertically  centered

const std::string WINDOW_TITLE = "Pareshay's Tetris";

// ================================================================
//  SCREEN ENUM  –  which screen is currently showing
// ================================================================
enum class Screen { MENU, GAME, SETTINGS, GAMEOVER };

// ================================================================
//  GLOBAL GAME STATE
// ================================================================
inline int   gameGrid[M][N];        // -1 = empty, 1-7 = locked piece colour index
inline int   point_1[4][2];         // current falling piece  [cell][x/y]
inline int   point_2[4][2];         // backup used during collision checks

inline int   g_score  = 0;
inline int   g_level  = 1;
inline int   g_lines  = 0;
inline bool  g_paused = false;

inline Screen g_screen = Screen::MENU;

// Next-piece preview  (type index 0-6, colour index 1-7)
inline int   g_nextType  = 0;
inline int   g_nextColor = 1;

// ================================================================
//  KAWAII COLOUR PALETTE
// ================================================================
const sf::Color C_BG         { 255, 208, 232 };   // polka-dot background
const sf::Color C_BOARD      { 236, 216, 248 };   // lavender board
const sf::Color C_DEEP_PINK  { 255,  20, 147 };
const sf::Color C_HOT_PINK   { 255, 105, 180 };
const sf::Color C_BTN        { 255, 158, 200 };   // button fill
const sf::Color C_BTN_SHADOW { 204,  80, 144 };
const sf::Color C_WHITE_SOFT { 255, 245, 252 };
const sf::Color C_TEXT_PINK  { 192,  64, 160 };
const sf::Color C_LAVENDER   { 200, 168, 216 };
const sf::Color C_BORDER     {  58,  32,  64 };
const sf::Color C_GRID       { 170, 120, 210,  50 };

// ================================================================
//  HELPER  –  clear the board (fill every cell with -1 = empty)
// ================================================================
inline void clearBoard() {
    for (int r = 0; r < M; r++)
        for (int c = 0; c < N; c++)
            gameGrid[r][c] = -1;
}
