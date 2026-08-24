
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <string>
#include "utils.h"
#include "pieces.h"
#include "functionality.h"

using namespace sf;


static float maxFloat(float a, float b) {
    return (a > b) ? a : b;
}

int main() {
    srand((unsigned int)time(nullptr));

    RenderWindow window(VideoMode(WIN_W, WIN_H),
                        WINDOW_TITLE,
                        Style::Close | Style::Titlebar);
    window.setFramerateLimit(60);

    Font font;
    font.loadFromFile("PressStart2P.ttf");

    Texture tilesTex;
    tilesTex.loadFromFile("tiles.png");
    Sprite  tileSprite(tilesTex);

    Texture menuBgTex, gameOverTex;
    Sprite  menuBgSpr, gameOverSpr;

    if (menuBgTex.loadFromFile("main_menu.jpeg")) {
        menuBgSpr.setTexture(menuBgTex);
        menuBgSpr.setScale(
            (float)WIN_W / menuBgTex.getSize().x,
            (float)WIN_H / menuBgTex.getSize().y);
    }
    if (gameOverTex.loadFromFile("game_over_and_continue.jpeg")) {
        gameOverSpr.setTexture(gameOverTex);
        float scale = 420.f / (float)gameOverTex.getSize().x;
        gameOverSpr.setScale(scale, scale);
        gameOverSpr.setPosition(WIN_W / 2.f - 210.f, 60.f);
    }

    const float MBW = 290.f, MBH = 50.f;
    const float MBX = WIN_W / 2.f - MBW / 2.f;
    const float MB_PLAY_Y = 370.f;
    const float MB_SET_Y  = 435.f;
    const float MB_EXIT_Y = 500.f;

    const float LP_X = 40.f, LP_W = 200.f;
    const float LP_HOME_Y = 400.f, LP_HOME_H = 40.f;

    const float RP_X = 620.f, RP_W = 200.f;
    const float RP_NEXT_Y = 120.f, RP_NEXT_H = 120.f;
    const float RP_CTRL_Y = 260.f, RP_CTRL_H = 230.f;

    const float GO_BW = 155.f, GO_BH = 46.f;
    const float GO_BY = 540.f;
    const float GO_YES_X  = WIN_W / 2.f - GO_BW - 10.f;
    const float GO_HOME_X = WIN_W / 2.f + 10.f;

    const float SET_BX = WIN_W / 2.f - 200.f;
    const float SET_BY = 636.f;
    const float SET_BW = 400.f, SET_BH = 46.f;

    int   colorNum = 1;
    bool  rotate   = false;
    int   delta_x  = 0;
    float timer    = 0.f;
    float delay    = 0.3f;
    bool  gameOver = false;
    Clock clock;

    auto initGame = [&]() {
        clearBoard();
        g_score  = 0; g_level = 1; g_lines = 0;
        g_paused = false;
        timer    = 0.f; delay = 0.3f;
        gameOver = false; rotate = false; delta_x = 0;

        int type = rand() % 7;
        colorNum = PIECE_COLOR[type];
        spawnPiece(type, colorNum);

        g_nextType  = rand() % 7;
        g_nextColor = PIECE_COLOR[g_nextType];
    };

    g_screen = Screen::MENU;

    while (window.isOpen()) {

        float dt = clock.restart().asSeconds();

        Event e;
        while (window.pollEvent(e)) {

            if (e.type == Event::Closed) window.close();

            if (e.type == Event::KeyPressed) {

                if (g_screen == Screen::GAME && !g_paused && !gameOver) {
                    switch (e.key.code) {
                    case Keyboard::Left:  delta_x = -1;   break;
                    case Keyboard::Right: delta_x =  1;   break;
                    case Keyboard::Up:    rotate  = true;  break;
                    case Keyboard::Space:
                        hardDrop(colorNum, gameOver);
                        timer = 0.f;
                        if (gameOver) g_screen = Screen::GAMEOVER;
                        break;
                    default: break;
                    }
                }

                if (e.key.code == Keyboard::P && g_screen == Screen::GAME)
                    g_paused = !g_paused;

                if (e.key.code == Keyboard::Escape && g_screen != Screen::MENU)
                    g_screen = Screen::MENU;
            }

            if (e.type == Event::MouseButtonPressed &&
                e.mouseButton.button == Mouse::Left)
            {
                Vector2i m = Mouse::getPosition(window);

                if (g_screen == Screen::MENU) {
                    if (mouseIn(m, MBX, MB_PLAY_Y, MBW, MBH)) { initGame(); g_screen = Screen::GAME; }
                    if (mouseIn(m, MBX, MB_SET_Y,  MBW, MBH))   g_screen = Screen::SETTINGS;
                    if (mouseIn(m, MBX, MB_EXIT_Y, MBW, MBH))   window.close();
                }
                else if (g_screen == Screen::GAME) {
                    if (mouseIn(m, LP_X, LP_HOME_Y, LP_W, LP_HOME_H)) g_screen = Screen::MENU;
                }
                else if (g_screen == Screen::GAMEOVER) {
                    if (mouseIn(m, GO_YES_X,  GO_BY, GO_BW, GO_BH)) { initGame(); g_screen = Screen::GAME; }
                    if (mouseIn(m, GO_HOME_X, GO_BY, GO_BW, GO_BH))   g_screen = Screen::MENU;
                }
                else if (g_screen == Screen::SETTINGS) {
                    if (mouseIn(m, SET_BX, SET_BY, SET_BW, SET_BH))   g_screen = Screen::MENU;
                }
            }
        }

        if (g_screen == Screen::GAME && !g_paused && !gameOver) {

            float dropDelay = delay;
            if (Keyboard::isKeyPressed(Keyboard::Down))
                dropDelay = 0.05f;

            rotation(rotate);
            movement(delta_x);

            timer += dt;
            fallingPiece(timer, dropDelay, colorNum, gameOver);

            delay = maxFloat(0.08f, 0.30f - (g_level - 1) * 0.025f);
            if (gameOver) g_screen = Screen::GAMEOVER;
        }

        window.clear();

        if (g_screen == Screen::MENU) {

            if (menuBgTex.getSize().x > 0) window.draw(menuBgSpr);
            else {
                RectangleShape sky({ (float)WIN_W,(float)WIN_H });
                sky.setFillColor(Color(180,220,255)); window.draw(sky);
            }
            RectangleShape tint({ (float)WIN_W,(float)WIN_H });
            tint.setFillColor(Color(255,200,230,55)); window.draw(tint);

            drawCard(window, WIN_W/2.f - 200.f, 100.f, 400.f, 130.f);
            drawTextC(window, font, " Pareshay's", 18, C_DEEP_PINK, WIN_W/2.f, 116.f);
            drawTextC(window, font, "TETRIS", 18, C_DEEP_PINK, WIN_W/2.f, 158.f);
            drawTextC(window, font, "* * *",  10, C_LAVENDER,  WIN_W/2.f, 204.f);

            drawButton(window, font, "PLAY     :)", MBX, MB_PLAY_Y, MBW, MBH);
            drawButton(window, font, "SETTINGS ::", MBX, MB_SET_Y,  MBW, MBH);
            drawButton(window, font, "EXIT     []", MBX, MB_EXIT_Y, MBW, MBH);
        }

        else if (g_screen == Screen::GAME) {

            drawPolkaBg(window);
            drawBoardFrame(window);

            // Locked cells
            for (int r = 0; r < M; r++)
                for (int c = 0; c < N; c++) {
                    if (gameGrid[r][c] == -1) continue;
                    tileSprite.setTextureRect(IntRect(gameGrid[r][c] * CELL, 0, CELL, CELL));
                    tileSprite.setScale(1.f, 1.f);
                    tileSprite.setPosition((float)(BOARD_X + c*CELL),(float)(BOARD_Y + r*CELL));
                    window.draw(tileSprite);
                }

            drawGhost(window, tileSprite);   // ghost (landing guide)

            // Current piece
            for (int i = 0; i < 4; i++) {
                if (point_1[i][1] < 0) continue;
                tileSprite.setTextureRect(IntRect(colorNum * CELL, 0, CELL, CELL));
                tileSprite.setScale(1.f, 1.f);
                tileSprite.setPosition((float)(BOARD_X + point_1[i][0]*CELL),
                                       (float)(BOARD_Y + point_1[i][1]*CELL));
                window.draw(tileSprite);
            }

            // Left panel
            drawCard(window, LP_X,  80.f, LP_W, 80.f);
            drawTextC(window, font, "SCORE", 7, C_TEXT_PINK, LP_X+LP_W/2, 96.f);
            drawTextC(window, font, std::to_string(g_score), 14, C_DEEP_PINK, LP_X+LP_W/2, 120.f);

            drawCard(window, LP_X, 180.f, LP_W, 80.f);
            drawTextC(window, font, "LEVEL", 7, C_TEXT_PINK, LP_X+LP_W/2, 196.f);
            drawTextC(window, font, std::to_string(g_level), 14, C_DEEP_PINK, LP_X+LP_W/2, 220.f);

            drawCard(window, LP_X, 280.f, LP_W, 80.f);
            drawTextC(window, font, "LINES", 7, C_TEXT_PINK, LP_X+LP_W/2, 296.f);
            drawTextC(window, font, std::to_string(g_lines), 14, C_DEEP_PINK, LP_X+LP_W/2, 320.f);

            drawButton(window, font, "HOME", LP_X, LP_HOME_Y, LP_W, LP_HOME_H);

            // Right panel
            drawCard(window, RP_X, 80.f, RP_W, 30.f);
            drawTextC(window, font, "NEXT", 7, C_TEXT_PINK, RP_X+RP_W/2, 91.f);
            drawCard(window, RP_X, RP_NEXT_Y, RP_W, RP_NEXT_H);
            drawNextPiece(window, tileSprite, RP_X, RP_NEXT_Y, RP_W, RP_NEXT_H);

            drawCard(window, RP_X, RP_CTRL_Y, RP_W, RP_CTRL_H);
            float hy = RP_CTRL_Y + 14.f; const float hs = 32.f;
            drawTextL(window, font, "[<>]  MOVE",   7, C_TEXT_PINK, RP_X+12.f, hy); hy+=hs;
            drawTextL(window, font, "[^]   ROTATE", 7, C_TEXT_PINK, RP_X+12.f, hy); hy+=hs;
            drawTextL(window, font, "[v]   SOFT",   7, C_TEXT_PINK, RP_X+12.f, hy); hy+=hs;
            drawTextL(window, font, "[SPC] SLAM",   7, C_TEXT_PINK, RP_X+12.f, hy); hy+=hs;
            drawTextL(window, font, "[P]   PAUSE",  7, C_TEXT_PINK, RP_X+12.f, hy); hy+=hs;
            drawTextL(window, font, "[ESC] HOME",   7, C_TEXT_PINK, RP_X+12.f, hy);

            // Pause overlay
            if (g_paused) {
                RectangleShape ov({ (float)BOARD_W,(float)BOARD_H });
                ov.setPosition((float)BOARD_X,(float)BOARD_Y);
                ov.setFillColor(Color(255,240,248,210)); window.draw(ov);
                drawTextC(window, font, "PAUSED",    16, C_DEEP_PINK,
                    BOARD_X+BOARD_W/2.f, BOARD_Y+BOARD_H/2.f-30.f);
                drawTextC(window, font, "[P] resume", 7, C_HOT_PINK,
                    BOARD_X+BOARD_W/2.f, BOARD_Y+BOARD_H/2.f+20.f);
            }
        }

        else if (g_screen == Screen::SETTINGS) {

            drawPolkaBg(window);
            float cx = WIN_W/2.f;
            float cX = cx-230.f, cY = 30.f, cW = 460.f, cH = 590.f;

            auto solidR = [&](float x,float y,float w,float h,Color c){
                RectangleShape r({w,h}); r.setPosition(x,y); r.setFillColor(c); window.draw(r);
            };
            solidR(cX-12,cY-12,cW+24,cH+24,Color(255,182,193));
            solidR(cX-6, cY-6, cW+12,cH+12,C_BORDER);
            drawCard(window, cX, cY, cW, cH);

            float ty = cY + 24.f;
            drawTextC(window, font, "HOW TO PLAY", 13, C_DEEP_PINK, cx, ty); ty += 56.f;

            RectangleShape div({cW-40.f, 3.f});
            div.setPosition(cX+20.f, ty); div.setFillColor(C_BTN); window.draw(div); ty += 22.f;

            struct Row { const char* key; const char* desc; };
            Row rows[] = {
                {"< >",   "Move left / right"},
                {"UP",    "Rotate piece"},
                {"DOWN",  "Soft drop"},
                {"SPACE", "Hard drop (slam!)"},
                {"P",     "Pause / Resume"},
                {"ESC",   "Back to menu"},
            };
            for (auto& row : rows) {
                RectangleShape badge({100.f,28.f});
                badge.setPosition(cX+24.f, ty); badge.setFillColor(C_BTN); window.draw(badge);
                RectangleShape bsh({100.f,4.f});
                bsh.setPosition(cX+24.f,ty+28.f); bsh.setFillColor(C_BTN_SHADOW); window.draw(bsh);
                drawTextC(window, font, row.key,  7, Color::White, cX+74.f,  ty+7.f);
                drawTextL(window, font, row.desc, 7, C_TEXT_PINK,  cX+138.f, ty+7.f);
                ty += 46.f;
            }

            RectangleShape tip({cW-40.f,66.f});
            tip.setPosition(cX+20.f,ty);
            tip.setFillColor(Color(255,182,193,55));
            tip.setOutlineColor(Color(255,182,193));
            tip.setOutlineThickness(2.f);
            window.draw(tip);
            drawTextC(window, font, "FILL A ROW TO CLEAR IT!", 7, C_TEXT_PINK, cx, ty+10.f);
            drawTextC(window, font, "4 ROWS AT ONCE = TETRIS!",7, C_TEXT_PINK, cx, ty+33.f);

            drawButton(window, font, "BACK TO MENU", SET_BX, SET_BY, SET_BW, SET_BH);
        }

        else if (g_screen == Screen::GAMEOVER) {

            drawPolkaBg(window);
            RectangleShape tint({(float)WIN_W,(float)WIN_H});
            tint.setFillColor(Color(255,182,210,90)); window.draw(tint);

            if (gameOverTex.getSize().x > 0)
                window.draw(gameOverSpr);
            else {
                drawTextC(window, font, "GAME OVER", 22, C_DEEP_PINK, WIN_W/2.f, 110.f);
                drawTextC(window, font, "CONTINUE?", 12, C_HOT_PINK,  WIN_W/2.f, 170.f);
            }

            drawCard(window, WIN_W/2.f-190.f, 390.f, 380.f, 125.f);
            drawTextC(window, font, "SCORE: "+std::to_string(g_score), 9, C_TEXT_PINK, WIN_W/2.f, 406.f);
            drawTextC(window, font, "LEVEL: "+std::to_string(g_level), 9, C_TEXT_PINK, WIN_W/2.f, 436.f);
            drawTextC(window, font, "LINES: "+std::to_string(g_lines), 9, C_TEXT_PINK, WIN_W/2.f, 466.f);

            drawButton(window, font, "YES!",  GO_YES_X,  GO_BY, GO_BW, GO_BH);
            drawButton(window, font, "HOME",  GO_HOME_X, GO_BY, GO_BW, GO_BH);
        }

        window.display();
    }

    return 0;
}