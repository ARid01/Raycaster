#include "settings.h"

extern int SCN_W = 1280;
extern int SCN_H = 720;
extern std::string WIN_TTL = "Raycaster Version 1";
extern int NUM_RAYS = 640;
extern int FPS_LIM = 60;
extern int DEFR_H = SCN_H / 2;
extern sf::Color FLOOR_COL = sf::Color(182, 182, 182);
extern sf::Color CEIL_COL = sf::Color(80, 80, 80);
extern sf::Vector2f FLOOR_POS = sf::Vector2f(0, SCN_H / 2);
extern sf::Vector2f CEIL_POS = sf::Vector2f(0, 0);
extern float PWLK_S = 0.05f;
extern float PROT_S = 4.0f;
extern float PRUN_S = PWLK_S * 2.0f;
extern const float PI = 3.14159f;
extern int SCALE = 4;
extern int TEXTWID = 8;
extern int TEXTHIG = 8;
extern float CXPOS = SCN_W - 2.0f * CNEED_H;
extern float CYPOS = 1.5f * CNEED_H;
extern float ANGL_WEST = -90.0f;
extern float CNEED_W = 5.0f;
extern float CNEED_H = 45.0f;
extern int COMP_THIC = 12;