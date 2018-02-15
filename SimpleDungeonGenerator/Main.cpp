#include "Main.h"

int main() {
	//Disable resizing and maximizing; Specify window size.
	HWND consoleWindow = GetConsoleWindow();
	RECT rect;
	SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
	GetWindowRect(consoleWindow, &rect);
	MoveWindow(consoleWindow, rect.left, rect.top, WINDOW_WIDTH, WINDOW_HEIGHT, TRUE);

	int dungeonHeight, dungeonWidth; //determined by initialize method.

	initialize(dungeonHeight, dungeonWidth);
	generate();
	
	//Used for camera movement in the future. 
	int cursorPosX = 0, cursorPosY = 0; 
	int destHeight = dungeonHeight, destWidth = dungeonWidth;
	if (dungeonHeight > WINDOW_TILE_HEIGHT) {
		destHeight = WINDOW_TILE_HEIGHT + cursorPosY;
	}
	if (dungeonWidth > WINDOW_TILE_WIDTH) {
		destWidth = WINDOW_TILE_WIDTH + cursorPosX;
	}

	//Print the dungeon.
	for (int r = cursorPosY; r < destHeight; r++) {
		for (int c = cursorPosX; c < destWidth; c++) {
			printf("%c", dungeon[r][c]);
		}
		printf("\n");
	}

	system("pause");
	return 0;
}

void initialize(int& dungeonHeight, int& dungeonWidth) {
	//Initialize the dungeon with all unexplored tiles.
	//If camera movement has been implemented, we can 
	//generate dungeons with randomized width and height.
	//Now we only generate 60x30 dungeon.

	/*
	uniform_int_distribution<int> disWidth(40, 80);
	uniform_int_distribution<int> disHeight(30, 60);

	dungeonHeight = disHeight(randGen);
	dungeonWidth = disWidth(randGen);
	*/

	randGen.seed(chrono::system_clock::now().time_since_epoch().count());

	dungeonHeight = WINDOW_TILE_HEIGHT;
	dungeonWidth = WINDOW_TILE_WIDTH;

	dungeon = vector<vector<char>>();

	for (int r = 0; r < dungeonHeight; r++) {
		vector<char> row = vector<char>();
		for (int c = 0; c < dungeonWidth; c++) {
			row.push_back(UNEXPLORED_TILE);
		}
		dungeon.push_back(row);
	}
		
}

void generate() {
	//Make the initial room
	makeRoom();

	int roomsGenerated = 1;

	//Keep generating new rooms and paths until there's no
	//more room to be generated, or reach the maximum number 
	//of rooms allowed in this dungeon.
	while (!rooms.empty() && roomsGenerated <= ROOM_MAX_NUM) {
		Room room = rooms.back();
		rooms.pop_back();

		//Create doors for this room
		randGen.seed(chrono::system_clock::now().time_since_epoch().count());
		uniform_int_distribution<int> disNumDoorsFirstRoom(2, 4);
		int numDoors = disNumDoorsFirstRoom(randGen);
		createDoors(room, numDoors);

		//Create paths according to the doors
		while (!doors.empty()) {
			tuple<int, int, int> door = doors.back();
			doors.pop_back();
			createPath(get<0>(door), get<1>(door), get<2>(door), room);
		}

		//Create extended rooms according to the valid paths
		while (!paths.empty()) {
			Path path = paths.back();
			paths.pop_back();
			int endDoorRow = get<0>(path.getEndDoor());
			int endDoorCol = get<1>(path.getEndDoor());
			int endDoorSide = path.getEndDoorSide();
			int numTry = 0;
			while (numTry < ROOM_TRY_NUM) {
				//If successed, terminate this loop and increment the
				//number of rooms.
				if (makeRoom(endDoorRow, endDoorCol, endDoorSide)) {
					roomsGenerated++;
					break;
				}	
				numTry++;
			}
			//If the room is not created within the max try, clear this path.
			if (numTry >= ROOM_TRY_NUM) {
				vector<tuple<int, int>> pathVector = path.getPath();
				room.setDoorExist(path.getStartDoorSide(), false);
				clearPath(pathVector, pathVector.size() - 1);
				dungeon[get<0>(pathVector.back())][get<1>(pathVector.back())] = UNEXPLORED_TILE;
			}
		}
	}
}

bool makeRoom() {
	//Make the intial room. The position and size are completely random.
	randGen.seed(chrono::system_clock::now().time_since_epoch().count());

	uniform_int_distribution<int> disRoomLength(ROOM_LENGTH_MIN, ROOM_LENGTH_MAX);
	uniform_int_distribution<int> disRoomHeight(ROOM_LENGTH_MIN, ROOM_LENGTH_MAX);

	int roomWidth = disRoomLength(randGen), roomHeight = disRoomLength(randGen);

	uniform_int_distribution<int> disRoomPosCol(0, WINDOW_TILE_WIDTH - roomWidth);
	uniform_int_distribution<int> disRoomPosRow(0, WINDOW_TILE_HEIGHT - roomHeight);

	int roomPosCol = disRoomPosCol(randGen), roomPosRow = disRoomPosRow(randGen);

	//If the first room will be out of the window , discard this room.
	for (int r = roomPosRow ; r < roomPosRow + roomHeight; r++) {
		for (int c = roomPosCol; c < roomPosCol + roomWidth; c++) {
			if (r >= WINDOW_TILE_HEIGHT || c >= WINDOW_TILE_WIDTH ||
				r < 0 || c < 0)
				return false;
		}
	}

	//make the room at specified location
	for (int r = roomPosRow; r < roomPosRow + roomHeight; r++) {
		for (int c = roomPosCol; c < roomPosCol + roomWidth; c++) {
			if (r == roomPosRow || r == roomPosRow + roomHeight - 1 ||
				c == roomPosCol || c == roomPosCol + roomWidth - 1)
				dungeon[r][c] = WALL_TILE;
			else 
				dungeon[r][c] = ROOM_TILE;
		}
	}

	//Create the made room object, and push it into the list of rooms
	rooms.push_back(Room(roomPosRow, roomPosCol, roomHeight, roomWidth));

	return true;
}

bool makeRoom(int doorRow, int doorCol, int doorSide) {
	//Make the extended rooms.
	randGen.seed(chrono::system_clock::now().time_since_epoch().count());

	//Invalid doors will always prevent a room generation.
	if (doorRow == 0 || doorRow == WINDOW_TILE_HEIGHT - 1 ||
		doorCol == 0 || doorCol == WINDOW_TILE_WIDTH - 1)
		return false;

	//generate a qualified start row and column position based on doorSide.
	int roomPosRow, roomPosCol, roomHeight = 0, roomWidth = 0;
	int rowExtends, colExtends;
	int heightMin, widthMin;

	//Based on the proposed side of the door, determine the room size and 
	//position.
	switch (doorSide) {
	case 0: {
		if (WINDOW_TILE_WIDTH - doorCol < 4)
			return false;
		roomPosCol = doorCol;
		rowExtends = fmax(0, doorRow - ROOM_LENGTH_MAX + 2);
		uniform_int_distribution<int> disRangeLeftRow(rowExtends, doorRow - 1);
		roomPosRow = disRangeLeftRow(randGen);
		if (doorRow - roomPosRow < ROOM_LENGTH_MIN)
			heightMin = ROOM_LENGTH_MIN;
		else
			heightMin = doorRow - roomPosRow + 2;
		uniform_int_distribution<int> disHeightRight(heightMin, ROOM_LENGTH_MAX);
		roomHeight = disHeightRight(randGen);
		uniform_int_distribution<int> disWidthLeft(ROOM_LENGTH_MIN, ROOM_LENGTH_MAX);
		roomWidth = disWidthLeft(randGen);
	}	
	break;
	case 1: {
		if (WINDOW_TILE_HEIGHT - doorRow < 4)
			return false;
		roomPosRow = doorRow;
		colExtends = fmax(0, doorCol - ROOM_LENGTH_MAX + 2);
		uniform_int_distribution<int> disRangeUpCol(colExtends, doorCol - 1);
		roomPosCol = disRangeUpCol(randGen);
		if (doorCol - roomPosCol < ROOM_LENGTH_MIN)
			widthMin = ROOM_LENGTH_MIN;
		else
			widthMin = doorCol - roomPosCol + 2;
		uniform_int_distribution<int> disWidthDown(widthMin, ROOM_LENGTH_MAX);
		roomWidth = disWidthDown(randGen);
		uniform_int_distribution<int> disHeightUp(ROOM_LENGTH_MIN, ROOM_LENGTH_MAX);
		roomHeight = disHeightUp(randGen);
	}	
	break;
	case 2: {
		if (doorCol < 3)
			return false;
		colExtends = fmax(0, doorCol - ROOM_LENGTH_MAX + 1);
		uniform_int_distribution<int> disRangeRightCol(colExtends, doorCol - 3);
		roomPosCol = disRangeRightCol(randGen);
		rowExtends = fmax(0, doorRow - ROOM_LENGTH_MAX + 2);
		uniform_int_distribution<int> disRangeRightRow(rowExtends, doorRow - 1);
		roomPosRow = disRangeRightRow(randGen);
		if (doorRow - roomPosRow < ROOM_LENGTH_MIN)
			heightMin = ROOM_LENGTH_MIN;
		else
			heightMin = doorRow - roomPosRow + 2;
		uniform_int_distribution<int> disHeightRight(heightMin, ROOM_LENGTH_MAX);
		roomHeight = disHeightRight(randGen);
		roomWidth = doorCol - roomPosCol + 1;
	}
	break;
	case 3: {
		if (doorRow < 3)
			return false;
		rowExtends = fmax(0, doorRow - ROOM_LENGTH_MAX + 1);
		uniform_int_distribution<int> disRangeDownRow(rowExtends, doorRow - 3);
		roomPosRow = disRangeDownRow(randGen);
		colExtends = fmax(0, doorCol - ROOM_LENGTH_MAX + 2);
		uniform_int_distribution<int> disRangeDownCol(colExtends, doorCol - 1);
		roomPosCol = disRangeDownCol(randGen);
		if (doorCol - roomPosCol < ROOM_LENGTH_MIN)
			widthMin = ROOM_LENGTH_MIN;
		else
			widthMin = doorCol - roomPosCol + 2;
		uniform_int_distribution<int> disWidthDown(widthMin, ROOM_LENGTH_MAX);
		roomWidth = disWidthDown(randGen);
		roomHeight = doorRow - roomPosRow + 1;
	}
	break;
	}

	
	//If the upcoming room will lie on another room or out of window, discard this room.
	for (int r = roomPosRow; r < roomPosRow + roomHeight; r++) {
		for (int c = roomPosCol; c < roomPosCol + roomWidth; c++) {
			if (r >= WINDOW_TILE_HEIGHT || c >= WINDOW_TILE_WIDTH ||
				r < 0 || c < 0) {
				return false;
			}
			if (dungeon[r][c] != UNEXPLORED_TILE && dungeon[r][c] != DOOR_TILE)
				return false;
		}
	}

	//make the room at specified location
	for (int r = roomPosRow; r < roomPosRow + roomHeight; r++) {
		for (int c = roomPosCol; c < roomPosCol + roomWidth; c++) {
			if (r == roomPosRow || r == roomPosRow + roomHeight - 1 ||
				c == roomPosCol || c == roomPosCol + roomWidth - 1) {
				if (r != doorRow || c != doorCol)
					dungeon[r][c] = WALL_TILE;
			}
			else
				dungeon[r][c] = ROOM_TILE;
		}
	}

	//Create the made room object, and push it into the list of rooms
	rooms.push_back(Room(roomPosRow, roomPosCol, roomHeight, roomWidth));

	return true;
}

void createDoors(Room room, int num) {
	//Create number of doors for a specific room.
	//Pick up a random side of walls in this room,
	//create a door on this side. Repeat this step
	//num times. If there has been a door in this 
	//side, skip it.
	//sides sequence: left, up, right ,down
	vector<int> sides = vector<int>{0, 1, 2, 3};
	randGen.seed(chrono::system_clock::now().time_since_epoch().count());
	shuffle(begin(sides), end(sides), randGen);
	for (int i = 0; i < num; i++) {
		int side = sides.back();
		sides.pop_back();
		int r = room.getRow(), c = room.getCol();
		uniform_int_distribution<int> disVertical(room.getRow() + 1, room.getRow() + room.getHeight() - 2);
		uniform_int_distribution<int> disHorizontal(room.getCol() + 1, room.getCol() + room.getWidth() - 2);
		if (!room.getDoorsExist(side)) {
			room.setDoorExist(side, true);
			switch (side) {
			case 0: //left
				if (c > 0)
					r = disVertical(randGen);
				break;
			case 1: //up
				if (r > 0)
					c = disHorizontal(randGen);
				break;
			case 2: //right
				c = c + room.getWidth() - 1;
				if (c < WINDOW_TILE_WIDTH - 1)
					r = disVertical(randGen);
				break;
			case 3: //down
				r = r + room.getHeight() - 1;
				if (r < WINDOW_TILE_HEIGHT - 1)
					c = disHorizontal(randGen);
				break;	
			}
			dungeon[r][c] = DOOR_TILE;
		}
		doors.push_back(make_tuple(side, r, c));
	}
}

void createPath(int originalDir, int doorRow, int doorCol, Room room) {
	//Randomly generate a path from a specified door.
	//Each time the path proceeds, it can randomly 
	//turn or stop. When stop, check if there's
	//enough space to create a new room.
	//
	//To make the dungeon good-looking, the path can
	//only turn at most twice; the direction will only
	//have two options: the original one, and the
	//firstly turned one.
	randGen.seed(chrono::system_clock::now().time_since_epoch().count());
	int dir = originalDir;
	uniform_real_distribution<double> dis(0.0, 1.0);
	vector<tuple<int, int>> path = vector<tuple<int, int>>();
	//Push the door into the vector
	tuple<int, int> startDoor(doorRow, doorCol);
	path.push_back(startDoor);
	tuple<int, int> startTile = proceedPath(dir, doorRow, doorCol);
	path.push_back(startTile);
	int currRow = get<0>(startTile), currCol = get<1>(startTile);
	int otherDir = originalDir;
	if (dis(randGen) < 0.5) {
		otherDir--;
		if (otherDir < 0)
			otherDir += 4;
	}
	else
		otherDir = (otherDir + 1) % 4;

	//Make the path
	int numTurned = 0;
	bool stop  = false;
	while (!stop) {
		if (dis(randGen) < PROB_STOP)
			break;
		if (dis(randGen) < PROB_TURN && numTurned < 2) {
			if (dir == originalDir)
				dir = otherDir;
			else
				dir = originalDir;
			numTurned++;
		}

		tuple<int, int> curr = proceedPath(dir, currRow, currCol);
		int tempCurrRow = get<0>(curr);
		int tempCurrCol = get<1>(curr);
		//Turn or stop if encounters:
		//1. borders of the dungeon; 2. any explored tiles.
		if (tempCurrRow < 0 || tempCurrRow >= WINDOW_TILE_HEIGHT ||
			tempCurrCol < 0 || tempCurrCol >= WINDOW_TILE_WIDTH ||
			dungeon[tempCurrRow][tempCurrCol] != UNEXPLORED_TILE) {
			//If this path has not turned twice, perform turning
			if (numTurned < 2) {
				if (dir == originalDir)
					dir = otherDir;
				else
					dir = originalDir;
				numTurned++;
				continue;
			}
			//If has turned twice, immediately stop
			else {
				break;
			}
		}
		currRow = tempCurrRow;
		currCol = tempCurrCol;
		path.push_back(curr);
	}
	//Push the coordinate of end door into the path to use later
	tuple<int, int> endDoor = proceedPath(dir, currRow, currCol);
	path.push_back(endDoor);
	bool isValidPath = true;

	//Write this path onto the dungeon
	for (int i = 1; i < path.size(); i++) {
		tuple<int, int> step = path[i];
		int row = get<0>(step), col = get<1>(step);

		//apply only to the last step (the end door). 
		//If the door will be out of window or lie on explored
		//tiles, clear this path.
		if ((row < 0 || row >= WINDOW_TILE_HEIGHT ||
			col < 0 || col >= WINDOW_TILE_WIDTH) ||
			dungeon[row][col] != UNEXPLORED_TILE) { 
			clearPath(path, i);
			isValidPath = false;
			break;
		}

		if (dungeon[row][col] == UNEXPLORED_TILE && i != path.size() - 1)
			dungeon[row][col] = PATH_TILE;

		if (i == path.size() - 1) //Mark the last tile in this path a door.
			dungeon[row][col] = DOOR_TILE;
	}

	if (isValidPath)
		paths.push_back(Path(startDoor, endDoor, originalDir, dir, path, room));
}

tuple<int, int> proceedPath(int dir, int currRow, int currCol) {
	//Based on the current direction, proceed to next tile,
	//return the coordinate of next tile.
	tuple<int, int> next;
	switch (dir) {
	case 0:
		next = make_tuple(currRow, currCol - 1);
		break;
	case 1:
		next = make_tuple(currRow - 1, currCol);
		break;
	case 2:
		next = make_tuple(currRow, currCol + 1);
		break;
	case 3:
		next = make_tuple(currRow + 1, currCol);
		break;
	}
	return next;
}

void clearPath(vector<tuple<int, int>> path, int index) {
	
	//restore the modification, and delete this path.
	for (int j = 0; j < index; j++) {
		tuple<int, int> istep = path[j];
		int iRow = get<0>(istep), iCol = get<1>(istep);
		if (dungeon[iRow][iCol] == DOOR_TILE)
			dungeon[iRow][iCol] = WALL_TILE;	
		else if (dungeon[iRow][iCol] == PATH_TILE)
			dungeon[iRow][iCol] = UNEXPLORED_TILE;
	}
}