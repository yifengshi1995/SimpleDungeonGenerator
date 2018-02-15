#include <vector>
#include "RoomObject.h"

#pragma once

//NOTE: the position of a room is its top-left corner.
class Room {
public:
	Room(int row, int col, int height, int width);
	Room();
	int getRow();
	int getCol();
	int getHeight();
	int getWidth();
	std::vector<RoomObject> getObjectsInRoom();
	bool getDoorsExist(int side);
	void addObjectToRoom(RoomObject obj);
	void setDoorExist(int side, bool exist);

private:
	int _row;     //Row coordinate of this room
	int _col;     //Column coordinate of this room
	int _height;  //Height of this room
	int _width;   //Width of this room
	std::vector<RoomObject> objects;  //List of objects in this room. Not currently used.
	bool doorsExist[4]{ false, false, false, false };  //Indicate either a side has a door.
};