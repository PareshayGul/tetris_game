#pragma once

// ================================================================
//  TETROMINO DEFINITIONS
//  Each row = one piece type, 4 values = the 4 cells in a 2x4 grid.
//  Value encodes (col, row) as:  col = value % 2,  row = value / 2
//  This matches the original code exactly.
// ================================================================
inline int BLOCKS[7][4] = {
    { 1, 3, 5, 7 },   // 0  I  – vertical bar
    { 2, 4, 5, 7 },   // 1  Z
    { 3, 5, 4, 6 },   // 2  S
    { 3, 5, 4, 7 },   // 3  T
    { 2, 3, 5, 7 },   // 4  L
    { 3, 5, 7, 6 },   // 5  J
    { 2, 3, 4, 5 }    // 6  O  – square
};

// Colour index each piece type uses when freshly spawned
// (matches BLOCKS row 0-6 → colour strip column 1-7 in tiles.png)
inline int PIECE_COLOR[7] = { 1, 2, 3, 4, 5, 6, 7 };
