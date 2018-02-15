#include "Path.h"

Path::Path(tuple<int, int> startDoor, tuple<int, int> endDoor, int startDoorSide,
			int endDoorSide, vector<tuple<int, int>> path, Room startRoom) {
	_startDoor = startDoor;
	_endDoor = endDoor;
	_endDoorSide = (endDoorSide + 2) % 4; //End door side will be opposite to the path direction.
	_startDoorSide = startDoorSide;       //But start door side will be the same.
	_path = path;
	_startRoom = startRoom;
}

tuple<int, int> Path::getStartDoor() {
	return _startDoor;
}

tuple<int, int> Path::getEndDoor() {
	return _endDoor;
}

int Path::getEndDoorSide() {
	return _endDoorSide;
}

int Path::getStartDoorSide() {
	return _startDoorSide;
}

vector<tuple<int, int>> Path::getPath() {
	return _path;
}

Room Path::getStartRoom() {
	return _startRoom;
}