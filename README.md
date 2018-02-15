# SimpleDungeonGenerator

This program is a very simple dungeon generator.

### Algorithms:

Preconditions:

- Rooms are rectangular.
- Each side of the room can only have one door.

Procedures:

- Step 1: Generate the initial room with random position and size, then push the room into a stack.
- Step 2: Create random number of doors for this room. Remove this room from the stack, and push the doors into another stack.
- Step 3: For each door, extends a path from this door. Each time the path is extended one tile, it will randomly turns or stops.The path can turn at most twice. At the end of the path, add a door. If the path will encouter a map border or other occupied tiles, stop proceeding and discard this path. Remove this door from the stack, and add the path into another stack.
- Step 4: For each path, create a room at its end (the position of the end door). Generate up to ROOM_MAX_TRY times until successfully generated. If failed, discard the path. Remove the processed path from the stack, and add the successfully generated room into the room stack.
- Step 5: For each room, repeat step 2-4. Stop the process if no more rooms in the stack.
- Step 6: Print out the dungeon in console.

### Usage:

I used Visual Studio 2017. Elder versions should be fine, but not guarateed. Somehow MinGW cannot compile this program. This is a windows console program, so may not able to be executed on other OS.

I also included the executable in the folder called "SimpleDungeonGenerator.exe". To only see how it works, you can just use that executable.

### Future Improvements:

- Add objects such as monsters, treasures, stairs into the rooms.
- Implement a movable camera so we can try generating bigger, random-sized dungeons.
- Make the path tiles clearer (now they are the same as room tiles, and will be hard to distinguish if multiple paths are together.
- The dungeon can be very small, such as only 1-2 rooms. This is because the path can be very long or mostly extends to the border of the dungeon. Should add some restrictions to make the dungeon have at least some number of rooms.

### Bugs:
- Very rare chance, there will be several missing tiles. I am currently working on it. 

### Note:
- This program is officially my first C++ program (I started using C++ two weeks ago, as 02/15/2018), so there might be many rules that not really compatible to C++.
- I have only classes but no struct. It is intended to make this program extendable.

