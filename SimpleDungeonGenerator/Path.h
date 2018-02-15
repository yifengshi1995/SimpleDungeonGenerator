#pragma once

#include <vector>
#include <tuple>
#include "Room.h"

using namespace std;

class Path {
public:
	Path(tuple<int, int> startDoor, tuple<int, int> endDoor, int startDoorSide,
		int endDoorSide, vector<tuple<int, int>> path, Room startRoom);
	tuple<int, int> getStartDoor();
	tuple<int, int> getEndDoor();
	int getEndDoorSide();
	int getStartDoorSide();
	vector<tuple<int, int>> getPath();
	Room getStartRoom();

private:
	tuple<int, int> _startDoor;
	tuple<int, int> _endDoor;
	int _endDoorSide;
	int _startDoorSide;
	vector<tuple<int, int>> _path;
	Room _startRoom;
};