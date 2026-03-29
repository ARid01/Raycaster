#include "player.h"

Player::Player(float fieldOfView) {
	setFov(fieldOfView);
}

float Player::getFov() {
	return fov;
}
float Player::getHalfFov() {
	return halfFov;
}
float Player::getX() {
	return x;
}
float Player::getY() {
	return y;
}
float Player::getDir() {
	return dir;
}
void Player::setFov(float nFov) {
	fov = nFov;
	halfFov = nFov / 2.0f;
}
void Player::setX(float nX) {
	x = nX;
}
void Player::setY(float nY) {
	y = nY;
}
void Player::setDir(float nDir) {
	dir = nDir;
}