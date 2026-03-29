#pragma once
#include <string>
#include "SFML/Graphics.hpp"

extern int SCN_W;
extern int SCN_H;
extern std::string WIN_TTL;
extern int NUM_RAYS;
extern int FPS_LIM;
extern int DEFR_H;
extern sf::Color FLOOR_COL;
extern sf::Color CEIL_COL;
extern sf::Vector2f FLOOR_POS;
extern sf::Vector2f CEIL_POS;
extern float PWLK_S;
extern float PROT_S;
extern float PRUN_S;
extern const float PI;
extern int SCALE;
extern int TEXTWID;
extern int TEXTHIG;
extern float CXPOS, CYPOS;
extern float ANGL_WEST;
extern float CNEED_W;
extern float CNEED_H;
extern int COMP_THIC;
constexpr int MAP_W = 80, MAP_H = 80;