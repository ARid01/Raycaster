#pragma once

class Player {

private:
	float fov;
	float halfFov;
	float x;
	float y;
	float dir;

public:
	Player(float fieldOfView);
	float getFov();
	float getHalfFov();
	float getX();
	float getY();
	float getDir();
	void setFov(float nFov);
	void setX(float nX);
	void setY(float nY);
	void setDir(float nDir);
};