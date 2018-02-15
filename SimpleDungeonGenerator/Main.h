#pragma once

#include <stdio.h>
#include <Windows.h>
#include <random>
#include <stdlib.h>
#include <chrono>
#include <vector>
#include <tuple>
#include <cmath>
#include <stdexcept>
#include "Room.h"
#include "Path.h"

//the following four values should be fixed:
//Camera movement has not been implemented, 
//so changing these values may cause unexpected
//output result.
const int WINDOW_WIDTH = 630;
const int WINDOW_HEIGHT = 540;
const int WINDOW_TILE_WIDTH = 60;
const int WINDOW_TILE_HEIGHT = 30;

//The remaining values is free to modify.
const int ROOM_TRY_NUM = 100;    //Maximum number to try generating a new room
const int ROOM_MAX_NUM = 15;     //Maximum number of rooms allowed in this dungeon
const int ROOM_LENGTH_MIN = 4;   //Minimum length of a room
const int ROOM_LENGTH_MAX = 10;  //Maximum length of a room

const double PROB_TURN = 0.25;   //Probability to turn the under-generation path
const double PROB_STOP = 0.25;   //Probability to stop generating a path

const char UNEXPLORED_TILE = ' ';
const char ROOM_TILE = '.';
//Currently the path tile is the same as room tile.
//I may distinguish horizontal and vertical path tiles
//in the future, to make the dungeon looks easier (or 
//implementing a graphical output).
const char PATH_TILE = '.';
const char WALL_TILE = '#';
const char DOOR_TILE = '=';
//Will be used by addObjectToRoom. Not currently implemented in
//main program.
const char MONSTER_TILE = 'M';
const char TREATURE_TILE = '%';

using namespace std;

vector<vector<char>> dungeon;          //The dungeon to be generated
mt19937 randGen;                       //Random generator

//Following vectors are used for the loop.
vector<Room> rooms;                    
vector<tuple<int, int, int>> doors;    
vector<Path> paths;

void initialize(int& dungeonHeight, int& dungeonWidth);
void generate();
bool makeRoom();
bool makeRoom(int doorRow, int doorCol, int doorSide);
void createDoors(Room room, int num);
void createPath(int dir, int doorRow, int doorCol, Room room);
tuple<int, int> proceedPath(int dir, int currRow, int currCol);
void clearPath(vector<tuple<int, int>> path, int index);