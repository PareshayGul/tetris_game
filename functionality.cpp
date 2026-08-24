

#include "functionality.h"
#include "utils.h"
#include "pieces.h"
#include <string>

// Simple replacement for std::min(a, b) for two ints, so we don't need <algorithm>
static int minInt(int a, int b) {
    return (a < b) ? a : b;
}
bool anamoly() {
    for (int i = 0; i < 4; i++) {
        int x = point_1[i][0];
        int y = point_1[i][1];
        if (x < 0 || x >= N) return false;
        // Floor
        if (y >= M)           return false;
        if (y >= 0 && gameGrid[y][x] != -1) return false;
    }
    return true;
}

void rotation(bool& rotate) {
    if (!rotate) return;

    for (int i = 0; i < 4; i++) {
        point_2[i][0] = point_1[i][0];
        point_2[i][1] = point_1[i][1];
    }

    int cx = point_1[1][0];
    int cy = point_1[1][1];
    for (int i = 0; i < 4; i++) {
        int dx = point_1[i][0] - cx;
        int dy = point_1[i][1] - cy;
        point_1[i][0] = cx + dy;
        point_1[i][1] = cy - dx;
    }

    int kicks[] = { 0, -1, 1, -2, 2 };
    bool ok = false;
    for (int k : kicks) {
        for (int i = 0; i < 4; i++) point_1[i][0] += k;
        if (anamoly()) { ok = true; break; }
        for (int i = 0; i < 4; i++) point_1[i][0] -= k;
    }

    // Also try shifting down by 1 if still invalid
    if (!ok) {
        for (int i = 0; i < 4; i++) point_1[i][1] += 1;
        if (!anamoly()) {
            // Restore on total failure
            for (int i = 0; i < 4; i++) point_1[i][1] -= 1;
            for (int i = 0; i < 4; i++) {
                point_1[i][0] = point_2[i][0];
                point_1[i][1] = point_2[i][1];
            }
        }
    }

    rotate = false;
}

void movement(int& delta_x) {
    if (delta_x == 0) return;

    for (int i = 0; i < 4; i++) {
        point_2[i][0] = point_1[i][0];
        point_2[i][1] = point_1[i][1];
        point_1[i][0] += delta_x;
    }
    if (!anamoly())
        for (int i = 0; i < 4; i++) {
            point_1[i][0] = point_2[i][0];
            point_1[i][1] = point_2[i][1];
        }

    delta_x = 0;
}

void checkLines(int& linesCleared) {
    linesCleared = 0;
    int k = M - 1;  // write pointer
    for (int i = M - 1; i >= 0; i--) {
        int count = 0;
        for (int j = 0; j < N; j++)
            if (gameGrid[i][j] != -1) count++;

        if (count == N) {
            linesCleared++;     // this row is full  →  skip copying it
        } else {
            // Copy row i into row k (may be same row if no lines were cleared)
            for (int j = 0; j < N; j++)
                gameGrid[k][j] = gameGrid[i][j];
            k--;
        }
    }
    // Clear any rows above the new top
    for (int i = k; i >= 0; i--)
        for (int j = 0; j < N; j++)
            gameGrid[i][j] = -1;
}

void spawnPiece(int type, int colorNum) {
    for (int i = 0; i < 4; i++) {
        point_1[i][0] = (BLOCKS[type][i] % 2) + (N / 2) - 1;
        point_1[i][1] = (BLOCKS[type][i] / 2);
    }
}

void fallingPiece(float& timer, float& delay, int& colorNum, bool& gameOver) {
    if (timer <= delay) return;
    timer = 0;

    // Try to move piece one row down
    for (int i = 0; i < 4; i++) {
        point_2[i][0] = point_1[i][0];
        point_2[i][1] = point_1[i][1];
        point_1[i][1] += 1;
    }

    if (!anamoly()) {
        // Restore position – piece has landed
        for (int i = 0; i < 4; i++) {
            point_1[i][0] = point_2[i][0];
            point_1[i][1] = point_2[i][1];
        }

        // Lock the piece into the grid
        for (int i = 0; i < 4; i++) {
            int px = point_1[i][0], py = point_1[i][1];
            if (py >= 0 && py < M && px >= 0 && px < N)
                gameGrid[py][px] = colorNum;
        }

        // Clear completed lines and update score
        int cleared = 0;
        checkLines(cleared);
        if (cleared > 0) {
            const int pts[] = { 0, 100, 300, 500, 800 };
            g_score += pts[minInt(cleared, 4)] * g_level;
            g_lines += cleared;
            g_level  = g_lines / 10 + 1;
        }

        // Spawn the next piece (already stored in g_nextType / g_nextColor)
        colorNum = g_nextColor;
        spawnPiece(g_nextType, colorNum);

        // Prepare a brand-new "next" piece
        g_nextType  = rand() % 7;
        g_nextColor = PIECE_COLOR[g_nextType];

        // Game over check: if spawn position is immediately invalid
        if (!anamoly()) gameOver = true;
    }
}

void hardDrop(int& colorNum, bool& gameOver) {
    // Move piece all the way down
    while (true) {
        for (int i = 0; i < 4; i++) { point_1[i][1] += 1; }
        if (!anamoly()) {
            for (int i = 0; i < 4; i++) { point_1[i][1] -= 1; }
            break;
        }
        g_score += 2;   // bonus points per cell dropped
    }

    // Lock
    for (int i = 0; i < 4; i++) {
        int px = point_1[i][0], py = point_1[i][1];
        if (py >= 0 && py < M && px >= 0 && px < N)
            gameGrid[py][px] = colorNum;
    }

    int cleared = 0;
    checkLines(cleared);
    if (cleared > 0) {
        const int pts[] = { 0, 100, 300, 500, 800 };
        g_score += pts[minInt(cleared, 4)] * g_level;
        g_lines += cleared;
        g_level  = g_lines / 10 + 1;
    }

    colorNum    = g_nextColor;
    spawnPiece(g_nextType, colorNum);
    g_nextType  = rand() % 7;
    g_nextColor = PIECE_COLOR[g_nextType];

    if (!anamoly()) gameOver = true;
}


// ── Pink polka-dot background ──────────────────────────────────
void drawPolkaBg(sf::RenderWindow& win) {
    sf::RectangleShape bg({ (float)WIN_W, (float)WIN_H });
    bg.setFillColor(C_BG);
    win.draw(bg);

    sf::CircleShape dot(3.5f);
    dot.setFillColor(sf::Color(255, 255, 255, 160));
    for (float dy = 0; dy < WIN_H; dy += 26.f)
        for (float dx = 0; dx < WIN_W; dx += 26.f) {
            dot.setPosition(dx, dy);
            win.draw(dot);
        }
}

// ── White info-card with pink border + shadow ──────────────────
void drawCard(sf::RenderWindow& win, float x, float y, float w, float h) {
    sf::RectangleShape shadow({ w, h });
    shadow.setPosition(x + 4.f, y + 5.f);
    shadow.setFillColor(C_BTN_SHADOW);
    win.draw(shadow);

    sf::RectangleShape card({ w, h });
    card.setPosition(x, y);
    card.setFillColor(C_WHITE_SOFT);
    card.setOutlineColor(C_BTN);
    card.setOutlineThickness(3.f);
    win.draw(card);
}

// ── Pink pill-shaped button ────────────────────────────────────
void drawButton(sf::RenderWindow& win, sf::Font& font,
                const std::string& label,
                float x, float y, float w, float h)
{
    sf::RectangleShape shadow({ w, h });
    shadow.setPosition(x + 2.f, y + 5.f);
    shadow.setFillColor(C_BTN_SHADOW);
    win.draw(shadow);

    sf::RectangleShape btn({ w, h });
    btn.setPosition(x, y);
    btn.setFillColor(C_BTN);
    win.draw(btn);

    sf::Text text(label, font, 13);
    text.setFillColor(sf::Color::White);
    sf::FloatRect b = text.getLocalBounds();
    text.setPosition(x + w / 2.f - b.width / 2.f - b.left,
                     y + h / 2.f - b.height / 2.f - b.top);
    win.draw(text);
}

// ── Centered text ──────────────────────────────────────────────
void drawTextC(sf::RenderWindow& win, sf::Font& font,
               const std::string& str, unsigned size,
               sf::Color col, float cx, float y)
{
    sf::Text t(str, font, size);
    t.setFillColor(col);
    sf::FloatRect b = t.getLocalBounds();
    t.setPosition(cx - b.width / 2.f - b.left, y);
    win.draw(t);
}

// ── Left-aligned text ──────────────────────────────────────────
void drawTextL(sf::RenderWindow& win, sf::Font& font,
               const std::string& str, unsigned size,
               sf::Color col, float x, float y)
{
    sf::Text t(str, font, size);
    t.setFillColor(col);
    t.setPosition(x, y);
    win.draw(t);
}

// ── Hit-test ──────────────────────────────────────────────────
bool mouseIn(sf::Vector2i m, float x, float y, float w, float h) {
    return (float)m.x >= x && (float)m.x <= x + w &&
           (float)m.y >= y && (float)m.y <= y + h;
}

// ── Board frame (layered kawaii pixel border) ──────────────────
void drawBoardFrame(sf::RenderWindow& win) {
    auto rect = [&](float x, float y, float w, float h, sf::Color c) {
        sf::RectangleShape r({ w, h });
        r.setPosition(x, y);
        r.setFillColor(c);
        win.draw(r);
    };

    float fx = (float)BOARD_X - 12.f, fy = (float)BOARD_Y - 12.f;
    float fw = (float)BOARD_W + 24.f, fh = (float)BOARD_H + 24.f;

    rect(fx - 8, fy - 8, fw + 16, fh + 16, sf::Color(255, 182, 193)); // outer pink
    rect(fx - 4, fy - 4, fw + 8,  fh + 8,  C_BORDER);                  // dark ring
    rect(fx,     fy,     fw,      fh,       C_BTN);                     // pink ring
    rect((float)BOARD_X - 2.f, (float)BOARD_Y - 2.f,
         (float)BOARD_W + 4.f, (float)BOARD_H + 4.f, C_BORDER);        // inner dark ring

    // Board background
    rect((float)BOARD_X, (float)BOARD_Y, (float)BOARD_W, (float)BOARD_H, C_BOARD);

    // Grid lines
    for (int r = 0; r <= M; r++) {
        rect((float)BOARD_X, (float)(BOARD_Y + r * CELL), (float)BOARD_W, 1.f, C_GRID);
    }
    for (int c = 0; c <= N; c++) {
        rect((float)(BOARD_X + c * CELL), (float)BOARD_Y, 1.f, (float)BOARD_H, C_GRID);
    }
}

// ── Ghost piece (landing guide) ───────────────────────────────
void drawGhost(sf::RenderWindow& win, sf::Sprite& sprite) {
    // Copy current piece
    int gx[4], gy[4];
    for (int i = 0; i < 4; i++) {
        gx[i] = point_1[i][0];
        gy[i] = point_1[i][1];
    }

    // Drop ghost as far as possible
    bool canDrop = true;
    while (canDrop) {
        for (int i = 0; i < 4; i++) gy[i]++;
        for (int i = 0; i < 4; i++) {
            int x = gx[i], y = gy[i];
            if (x < 0 || x >= N || y >= M || (y >= 0 && gameGrid[y][x] != -1)) {
                canDrop = false;
                break;
            }
        }
        if (!canDrop) for (int i = 0; i < 4; i++) gy[i]--;
    }

    // Don't draw ghost if it overlaps the real piece
    bool same = true;
    for (int i = 0; i < 4; i++) if (gy[i] != point_1[i][1]) { same = false; break; }
    if (same) return;

    // Draw ghost as semi-transparent pink outlines
    for (int i = 0; i < 4; i++) {
        if (gy[i] < 0) continue;
        float px = (float)(BOARD_X + gx[i] * CELL);
        float py = (float)(BOARD_Y + gy[i] * CELL);
        sf::RectangleShape g2({ (float)CELL, (float)CELL });
        g2.setPosition(px, py);
        g2.setFillColor(sf::Color(255, 180, 220, 60));
        g2.setOutlineColor(sf::Color(255, 105, 180, 120));
        g2.setOutlineThickness(2.f);
        win.draw(g2);
    }
}

// ── Next-piece preview ─────────────────────────────────────────
void drawNextPiece(sf::RenderWindow& win, sf::Sprite& sprite,
                   float boxX, float boxY, float boxW, float boxH)
{
    const float sz = 24.f;
    float startX = boxX + (boxW - 2 * sz) / 2.f;
    float startY = boxY + (boxH - 2 * sz) / 2.f;

    for (int i = 0; i < 4; i++) {
        int col = BLOCKS[g_nextType][i] % 2;
        int row = BLOCKS[g_nextType][i] / 2;

        float px = startX + col * sz;
        float py = startY + row * sz;

        // Scale sprite down to sz×sz
        sprite.setTextureRect(sf::IntRect(g_nextColor * CELL, 0, CELL, CELL));
        sprite.setScale(sz / (float)CELL, sz / (float)CELL);
        sprite.setPosition(px, py);
        win.draw(sprite);
    }
    sprite.setScale(1.f, 1.f);   // restore scale
}