#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// ================================================================
//  GAME LOGIC   (implemented in functionality.cpp)
// ================================================================
bool anamoly();                              // collision check (matches original name)
void rotation(bool& rotate);                 // rotate current piece (SRS-lite wall kick)
void movement(int& delta_x);                 // move piece left / right
void checkLines(int& linesCleared);          // remove full rows, returns how many
void fallingPiece(float& timer,
                  float& delay,
                  int& colorNum,
                  bool& gameOver);           // auto-drop logic, spawns next piece
void spawnPiece(int type, int colorNum);     // place a new piece at the top
void hardDrop(int& colorNum, bool& gameOver);// slam piece to bottom instantly

// ================================================================
//  DRAWING HELPERS  (implemented in functionality.cpp)
// ================================================================
void drawPolkaBg   (sf::RenderWindow& win);
void drawCard      (sf::RenderWindow& win, float x, float y, float w, float h);
void drawButton    (sf::RenderWindow& win, sf::Font& font,
                    const std::string& label,
                    float x, float y, float w, float h);
void drawTextC     (sf::RenderWindow& win, sf::Font& font,
                    const std::string& str, unsigned size,
                    sf::Color col, float cx, float y);   // horizontally centered
void drawTextL     (sf::RenderWindow& win, sf::Font& font,
                    const std::string& str, unsigned size,
                    sf::Color col, float x, float y);    // left-aligned
bool mouseIn       (sf::Vector2i m, float x, float y, float w, float h);
void drawBoardFrame(sf::RenderWindow& win);
void drawGhost     (sf::RenderWindow& win, sf::Sprite& sprite);
void drawNextPiece (sf::RenderWindow& win, sf::Sprite& sprite,
                    float boxX, float boxY, float boxW, float boxH);
