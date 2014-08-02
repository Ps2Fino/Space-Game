#ifndef RS_CONSTANTS
#define RS_CONSTANTS

// The default level background to play in
#define PLAY_LEVEL 3
#define TEXT_FONT_SIZE 20

///////////////////////////////////////////
//// Sizes for entities ///////////////////
///////////////////////////////////////////

#define GAME_WINDOW_WIDTH 640
#define GAME_WINDOW_HEIGHT 480

#define BULLET_WIDTH 95
#define BULLET_HEIGHT 68

#define SHIP_WIDTH 64
#define SHIP_HEIGHT 48

#define ASTEROID_WIDTH 64
#define ASTEROID_HEIGHT 64

///////////////////////////////////////////
//// Start positions for entities /////////
///////////////////////////////////////////

#define SHIP_START_POSITION_X 20
#define SHIP_START_POSITION_Y 240

#define BULLET_OFFSET 10
#define ASTEROID_OFFSET 20

///////////////////////////////////////////
//// Velocities for entities //////////////
///////////////////////////////////////////

#define BULLET_VELOCITY 10
#define SHIP_VELOCITY 8
#define ASTEROID_VELOCITY 3

///////////////////////////////////////////
//// Entitiy counts for the game //////////
///////////////////////////////////////////

#define NUMBER_BULLETS 5
#define NUMBER_ASTEROIDS 10
#define NUMBER_OF_ASTEROID_SPRITES 8

///////////////////////////////////////////
//// Collision detection definitions //////
///////////////////////////////////////////

#define BULLET_COLLISION_WIDTH 95
#define BULLET_COLLISION_HEIGHT 68

#define ASTEROID_COLLISION_WIDTH 64
#define ASTEROID_COLLISION_HEIGHT 64

#endif