#include "SFML/Graphics.hpp"
#include "settings.h"
#include "player.cpp"
#include "texture.h"
#include <math.h>
#include <string>
#include <iostream>


class Vector2D;
float degreeToRads(float degree);
int indexFromCoord(int x, int y);

//Andrew Ridgely - 8/12/24 - Maze Generator with Rooms
#include <vector>
#include <stack>
#include <unordered_map>
#include <queue>

namespace settings {
	//Window
	constexpr int borderWidth{ 1 };
	constexpr int gridX{ 80 };
	constexpr int gridY{ 80 };
	constexpr int minRoomSpacing{ 3 };

	//Generation
	constexpr int minRoomX{ 6 };
	constexpr int maxRoomX{ 24 };
	constexpr int minRoomY{ 6 };
	constexpr int maxRoomY{ 24 };
	constexpr int failureLimit{ 2000 };
	constexpr int maxNumDoors{ 4 };
	constexpr int joinLimit{ 75 };
	constexpr int failLimit{ 150 };
}

using namespace std;
using namespace settings;

//Works
struct Point {
	int x, y;
};

//Works
struct Room {
	Point tl, br;
};

//Works
class Vector2D {
private:
	int gridX, gridY;
	vector<vector<int>> grid;

	//Key
	//0 - Open
	//1 - Wall
	//2 - Room Tile (2...)

public:
	Vector2D(int x, int y, vector<Room>& rooms) {
		gridX = x;
		gridY = y;
		grid = vector<vector<int>>(y, vector<int>(x, 1));
		int roomid = 2;

		for (Room& r : rooms) {
			for (int y = r.tl.y; y < r.br.y; y++) {
				for (int x = r.tl.x; x < r.br.x; x++) {
					grid[y][x] = roomid;
				}
			}
			roomid++;
		}
	}

	int getGridX() {
		return gridX;
	}
	int getGridY() {
		return gridY;
	}
	int getPoint(int x, int y) {
		return grid[y][x];
	}
	void setPoint(int x, int y, int val) {
		grid[y][x] = val;
	}
};

//Works
bool outOfGridBounds(Room& r1) {
	return (r1.tl.x < borderWidth || r1.br.x >= gridX - borderWidth || r1.tl.y < borderWidth || r1.br.y >= gridY - borderWidth);
}

//Works
bool doRoomsOverlap(Room& r1, Room& r2) {
	if (r1.tl.x - minRoomSpacing > r2.br.x || r2.tl.x - minRoomSpacing > r1.br.x) return false;
	if (r1.br.y + minRoomSpacing < r2.tl.y || r2.br.y + minRoomSpacing < r1.tl.y) return false;
	return true;
}

//Works
bool isAllOdd(Room& r) {
	if ((r.br.y - r.tl.y) % 2 == 0) return false;
	if ((r.br.x - r.tl.x) % 2 == 0) return false;
	if (r.tl.x % 2 == 0 || r.tl.y % 2 == 0) return false;
	return true;
}

//Works
Room getNextRoom() {
	Room tmp;

	do {
		int currX = minRoomX + rand() % (maxRoomX - minRoomX);
		int currY = minRoomY + rand() % (maxRoomY - minRoomY);
		int posX = rand() % gridX;
		int posY = rand() % gridY;

		tmp = {
			{posX, posY},
			{posX + currX, posY + currY}
		};

	} while (outOfGridBounds(tmp) || !isAllOdd(tmp));

	return tmp;
}

//Works
vector<Room> spawnRooms() {
	vector<Room> rooms = { getNextRoom() };
	int currFails = 0;

	//Create rest of rooms
	while (currFails < failureLimit) {
		Room tmpRoom = getNextRoom();

		//Check other rooms
		bool doesOver = false;
		for (Room& room : rooms) {
			if (doRoomsOverlap(tmpRoom, room)) {
				currFails++;
				doesOver = true;
				break;
			}
		}
		if (!doesOver) {
			currFails = 0;
			rooms.push_back(tmpRoom);
		}
	}

	return rooms;
}

//Works
bool isValidCoord(int x, int y) {
	return x % 2 && y % 2;
}

//Works
bool outOfBoundsCoord(int x, int y) {
	return x < borderWidth || x >= gridX - borderWidth || y < borderWidth || y >= gridY - borderWidth;
}

//Works
void beginMazeWalk(int x, int y, Vector2D& board) {
	stack<Point> s;
	board.setPoint(x, y, 0);		// Mark initial cell as visited (open)

	s.push({ x, y });				// Push initial to stack

	while (!s.empty()) {
		Point curr = s.top();		// While !empty, pop from stack and make current
		s.pop();

		//Get neighbors				// If current has any unvisited neighbors
		vector<Point> neigh;

		if (!outOfBoundsCoord(curr.x - 2, curr.y) && board.getPoint(curr.x - 2, curr.y) == 1) neigh.push_back({ curr.x - 2, curr.y });
		if (!outOfBoundsCoord(curr.x + 2, curr.y) && board.getPoint(curr.x + 2, curr.y) == 1) neigh.push_back({ curr.x + 2, curr.y });
		if (!outOfBoundsCoord(curr.x, curr.y - 2) && board.getPoint(curr.x, curr.y - 2) == 1) neigh.push_back({ curr.x, curr.y - 2 });
		if (!outOfBoundsCoord(curr.x, curr.y + 2) && board.getPoint(curr.x, curr.y + 2) == 1) neigh.push_back({ curr.x, curr.y + 2 });

		if (!neigh.empty()) {
			s.push(curr);

			Point next = neigh[rand() % neigh.size()];

			int dx = next.x - curr.x;
			int dy = next.y - curr.y;

			if (dx != 0) {
				dx += (dx < 0) ? 1 : -1;
			}
			else {
				dy += (dy < 0) ? 1 : -1;
			}

			board.setPoint(curr.x + dx, curr.y + dy, 0);		// Break down wall between current and next

			board.setPoint(next.x, next.y, 0);
			s.push(next);
		}
	}
}

//Works
void connectRooms(Vector2D& board) {
	unordered_map<int, vector<Point>> connectors;

	//Store list of points of walls that connect either two rooms or a room and hallway, hashed under their room number
	for (int y = borderWidth; y < gridY - borderWidth; y++) {
		for (int x = borderWidth; x < gridX - borderWidth; x++) {
			if (board.getPoint(x, y) != 1) continue;

			int roomNum = -1;
			bool found = false;
			if (!outOfBoundsCoord(x - 1, y) && board.getPoint(x - 1, y) > 1 && !outOfBoundsCoord(x + 1, y) && board.getPoint(x + 1, y) != 1) roomNum = board.getPoint(x - 1, y), found = true;
			else if (!outOfBoundsCoord(x + 1, y) && board.getPoint(x + 1, y) > 1 && !outOfBoundsCoord(x - 1, y) && board.getPoint(x - 1, y) != 1) roomNum = board.getPoint(x + 1, y), found = true;
			else if (!outOfBoundsCoord(x, y - 1) && board.getPoint(x, y - 1) > 1 && !outOfBoundsCoord(x, y + 1) && board.getPoint(x, y + 1) != 1) roomNum = board.getPoint(x, y - 1), found = true;
			else if (!outOfBoundsCoord(x, y + 1) && board.getPoint(x, y + 1) > 1 && !outOfBoundsCoord(x, y - 1) && board.getPoint(x, y - 1) != 1) roomNum = board.getPoint(x, y + 1), found = true;

			if (!found || roomNum == -1) continue;

			Point toAdd = { x, y };

			if (connectors.find(roomNum) != connectors.end()) {
				connectors[roomNum].push_back(toAdd);
			}
			else connectors.insert({ roomNum, {toAdd} });
		}
	}

	for (auto i = connectors.begin(); i != connectors.end(); i++) {
		for (int iter = 0; iter < maxNumDoors; iter++) {
			int randInd = rand() % i->second.size();
			Point target = i->second[randInd];
			board.setPoint(target.x, target.y, 0);
			i->second.erase(i->second.begin() + randInd);
		}
	}
}

//Works
bool isDeadEnd(Vector2D& board, int x, int y) {
	int surrWalls = 0;
	if (!outOfBoundsCoord(x - 1, y) && board.getPoint(x - 1, y) == 1) surrWalls++;
	if (!outOfBoundsCoord(x + 1, y) && board.getPoint(x + 1, y) == 1) surrWalls++;
	if (!outOfBoundsCoord(x, y - 1) && board.getPoint(x, y - 1) == 1) surrWalls++;
	if (!outOfBoundsCoord(x, y + 1) && board.getPoint(x, y + 1) == 1) surrWalls++;
	return surrWalls == 3;
}

//Works
void uncarveDeadEnds(Vector2D& board) {
	queue<Point> deadEnds;  //Starting points of dead ends, having 3 surrounding walls

	for (int y = borderWidth; y < gridY - borderWidth; y++) {
		for (int x = borderWidth; x < gridX - borderWidth; x++) {
			if (isDeadEnd(board, x, y)) {
				deadEnds.push({ x, y });
			}
		}
	}

	while (!deadEnds.empty()) {
		Point curr = deadEnds.front();
		deadEnds.pop();

		board.setPoint(curr.x, curr.y, 1);		//Set dead end to wall

		//Add any other dead ends to list
		for (int y = curr.y - 1; y <= curr.y + 1; y++) {
			for (int x = curr.x - 1; x <= curr.x + 1; x++) {
				if (outOfBoundsCoord(x, y)) continue;
				else if (curr.x == x && curr.y == y) continue;
				else if (board.getPoint(x, y) != 0) continue;
				else if (isDeadEnd(board, x, y)) {
					Point np = { x, y };
					deadEnds.emplace(np);
				}
			}
		}
	}
}

//Works
void joinHalls(Vector2D& board) {
	int joins = 0, fails = 0;

	while (joins < joinLimit && fails < failLimit) {
		int sx, sy;
		do {
			sx = rand() % gridX;
			sy = rand() % gridY;
		} while (outOfBoundsCoord(sx, sy) || board.getPoint(sx, sy) != 1);

		if ((!outOfBoundsCoord(sx - 1, sy) && !outOfBoundsCoord(sx + 1, sy) && board.getPoint(sx - 1, sy) != 1 && board.getPoint(sx + 1, sy) != 1) ||
			(!outOfBoundsCoord(sx, sy - 1) && !outOfBoundsCoord(sx, sy + 1) && board.getPoint(sx, sy - 1) != 1 && board.getPoint(sx, sy + 1) != 1)) {

			board.setPoint(sx, sy, 0);
			joins++;
		}
		else fails++;
	}
}

Vector2D GenerateMap() {
	vector<Room> rooms = spawnRooms();
	Vector2D board(gridX, gridY, rooms);

	//Start for mazewalk
	int sx, sy;
	do {
		sx = rand() % gridX;
		sy = rand() % gridY;
	} while (board.getPoint(sx, sy) != 1 || !isValidCoord(sx, sy) || outOfBoundsCoord(sx, sy));
	beginMazeWalk(sx, sy, board);

	//Generation
	connectRooms(board);
	uncarveDeadEnds(board);
	joinHalls(board);

	return board;
}


int main()
{
    //Seed RNG
    srand(time(NULL));

    //Window setup & basic objects
    sf::RenderWindow window(sf::VideoMode(SCN_W, SCN_H), WIN_TTL);
    sf::RectangleShape ray(sf::Vector2f(SCN_W / NUM_RAYS, DEFR_H));
    sf::RectangleShape ceil_floor(sf::Vector2f(SCN_W, SCN_H / 2));
    sf::RectangleShape compass(sf::Vector2f(CNEED_W, CNEED_H));
    sf::CircleShape compBase(CNEED_H, COMP_THIC);

    compBase.setOutlineColor(sf::Color(95, 95, 95));
    compBase.setOutlineThickness(10.0f);
    compBase.setFillColor(sf::Color(0, 0, 0, 0));
    compBase.setPosition(sf::Vector2f(CXPOS - CNEED_H, CYPOS - CNEED_H));

    compass.setFillColor(sf::Color::Red);

    window.setFramerateLimit(FPS_LIM);

    //Generate map
    Vector2D map1 = GenerateMap();

    //Create player
    Player player = Player(60);
    
	int x, y;
	do {
		x = rand() % MAP_W;
		y = rand() % MAP_H;
	} while (map1.getPoint(x, y) != 0);
	player.setX(x + 0.5f);
	player.setY(y + 0.5f);
	player.setDir(rand() % 360);

    float currPlayerSpeed = PWLK_S;

    while (window.isOpen())
    {
        //Event Handling
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
        }

        //Check sprint
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            currPlayerSpeed = PRUN_S;
        }
        else {
            currPlayerSpeed = PWLK_S;
        }

        //Input Handling
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            float playerCos = cosf(degreeToRads(player.getDir())) * currPlayerSpeed;
            float playerSin = sinf(degreeToRads(player.getDir())) * currPlayerSpeed;
            float newX = player.getX() + playerCos;
            float newY = player.getY() + playerSin;

            //Collision check
            if (map1.getPoint((int)newX,(int)newY) == 0) {
                player.setX(newX);
                player.setY(newY);
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            float playerCos = cosf(degreeToRads(player.getDir())) * currPlayerSpeed;
            float playerSin = sinf(degreeToRads(player.getDir())) * currPlayerSpeed;
            float newX = player.getX() - playerCos;
            float newY = player.getY() - playerSin;

            //Collision check
            if (map1.getPoint((int)newX, (int)newY) == 0) {
                player.setX(newX);
                player.setY(newY);
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
        {
            float angle = player.getDir() + 90.0f;
            float playerCos = cosf(degreeToRads(angle)) * currPlayerSpeed;
            float playerSin = sinf(degreeToRads(angle)) * currPlayerSpeed;

            float newX = player.getX() - playerCos;
            float newY = player.getY() - playerSin;

            //Collision check
            if (map1.getPoint((int)newX, (int)newY) == 0) {
                player.setX(newX);
                player.setY(newY);
            }

        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player.setDir(player.getDir() - PROT_S);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
        {
            float angle = player.getDir() - 90.0f;
            float playerCos = cosf(degreeToRads(angle)) * currPlayerSpeed;
            float playerSin = sinf(degreeToRads(angle)) * currPlayerSpeed;

            float newX = player.getX() - playerCos;
            float newY = player.getY() - playerSin;

            //Collision check
            if (map1.getPoint((int)newX, (int)newY) == 0) {
                player.setX(newX);
                player.setY(newY);
            }

        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player.setDir(player.getDir() + PROT_S);
        }

        //Set compass
        compass.setRotation(ANGL_WEST - player.getDir() + 180.0f);
        compass.setPosition(sf::Vector2f(CXPOS - cosf(CNEED_H * compass.getRotation()),
            CYPOS - sinf(CNEED_H * compass.getRotation())));

        //Rendering
        window.clear();

        //Draw floor and ceiling
        ceil_floor.setFillColor(FLOOR_COL);
        ceil_floor.setPosition(FLOOR_POS);
        window.draw(ceil_floor);
        ceil_floor.setFillColor(CEIL_COL);
        ceil_floor.setPosition(CEIL_POS);
        window.draw(ceil_floor);

        //Raycasting
		float rayAngle = player.getDir() - player.getHalfFov();

		for (int r = 0; r < NUM_RAYS / SCALE; r++) {

			float rayX = player.getX();
			float rayY = player.getY();
			float rayCos = cosf(degreeToRads(rayAngle));
			float raySin = sinf(degreeToRads(rayAngle));
			int wall = 0;

			while (!wall) {
				rayX += rayCos / 100;
				rayY += raySin / 100;
				wall = map1.getPoint((int)rayX, (int)rayY);
			}

			int texture = wall - 1;
			int textPosX = (int)(TEXTWID * (rayX + rayY)) % TEXTWID;

			float dist = sqrtf(powf(player.getX() - rayX, 2) + powf(player.getY() - rayY, 2));
			dist = dist * cosf(degreeToRads(rayAngle - player.getDir()));
			int wallHeight = (int)((SCN_H / 2.0f) / dist);
			float shadeFactor = (dist / 2) + 1;

			//Raydraw
			float startY = (SCN_H / 2.0f) - (wallHeight / 2.0f);
			float ySize = wallHeight / TEXTHIG;
			float xSize = SCN_W / (NUM_RAYS / SCALE);
			sf::Color color;

			ray.setSize(sf::Vector2f(xSize, ySize));

			for (int y = 0; y < TEXTHIG; y++) {
				switch (textures.at(texture)[indexFromCoord(textPosX, y)]) {
				case '0':
					color = TCOL0;
					break;
				case '1':
					color = TCOL1;
					break;
				}
				color.r /= shadeFactor;
				color.g /= shadeFactor;
				color.b /= shadeFactor;

				ray.setFillColor(color);
				ray.setPosition(sf::Vector2f(r * ray.getSize().x, startY + (ySize * y)));
				window.draw(ray);
			}
			rayAngle += player.getFov() / (NUM_RAYS / SCALE);
		}

        //Draw compass
        window.draw(compBase);
        window.draw(compass);
        
        window.display();
        //End of Rendering
    }

    return 0;
}

float degreeToRads(float degree) {
    return degree * PI / 180.0f;
}

int indexFromCoord(int x, int y) {
    return y * TEXTWID + x;
}
