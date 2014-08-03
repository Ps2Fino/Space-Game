#ifndef RS_CONSTANTS
#define RS_CONSTANTS

// The events that the input handler will check for
enum GAME_EVENT {
	UP_DOWN,
	DOWN_DOWN,
	ESC_DOWN,
	SPACE_DOWN,
	UP_UP,
	DOWN_UP,
	ESCAPE_UP,
	SPACE_UP,
	GAME_QUIT,	
} GameEvent;

enum GAME_STATE {
	MENU,
	GAME,
	GAME_OVER,
} state;

///////////////////////////////////////////
//// Misc definitions and constants ///////
///////////////////////////////////////////

// #define LOGGING_FPS YES // Uncomment this to log the fps during the game loop

// The default level background to play in
#define PLAY_LEVEL 3
#define TEXT_FONT_SIZE 20
#define NUMBER_PLAYER_LIVES 10
#define SCREEN_FPS 60
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

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
//// Sizes for the score table ////////////
///////////////////////////////////////////

#define SCORE_TEXT_LOCATION_X (GAME_WINDOW_WIDTH - 300)
#define SCORE_TEXT_LOCATION_Y (-10)
#define SCORE_TEXT_WIDTH 300
#define SCORE_TEXT_HEIGHT 50

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

#define NUMBER_ASTEROIDS_TO_WIN 50
#define NUMBER_BULLETS 5
#define NUMBER_ASTEROIDS 20
#define ASTEROID_INTERVAL_RANGE_MIN 300
#define ASTEROID_INTERVAL_RANGE_MAX 500
#define NUMBER_OF_ASTEROID_SPRITES 8

///////////////////////////////////////////
//// Collision detection definitions //////
///////////////////////////////////////////

#define BULLET_COLLISION_WIDTH 95
#define BULLET_COLLISION_HEIGHT 68

#define ASTEROID_COLLISION_WIDTH 64
#define ASTEROID_COLLISION_HEIGHT 64

#endif