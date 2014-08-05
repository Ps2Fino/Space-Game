#ifndef RS_CONSTANTS
#define RS_CONSTANTS

// #define USE_CPP_RANDOM // uncomment this to use the experimental 64 bit <random> library

// The events that the input handler will check for
enum GAME_EVENT {
	UP_DOWN = 1,
	DOWN_DOWN,
	ESC_DOWN,
	SPACE_DOWN,
	UP_UP,
	DOWN_UP,
	ESCAPE_UP,
	SPACE_UP,
	GAME_QUIT,
	NONE, // A null value
};

///////////////////////////////////////////
//// Keyboard states //////////////////////
///////////////////////////////////////////

#define KEY_UP_PRESSED 0x0001
#define KEY_DOWN_PRESSED 0x0002
#define KEY_ESCAPE_PRESSED 0x0004
#define KEY_SPACE_PRESSED 0x0008
#define KEY_P_PRESSED 0x0010
#define KEY_RELEASED 0x0000

///////////////////////////////////////////
//// Misc definitions and constants ///////
///////////////////////////////////////////

// #define LOGGING_FPS YES // Uncomment this to log the fps during the game loop

// The default level background to play in
#define PLAY_LEVEL 3
#define SCREEN_FPS 60
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

///////////////////////////////////////////
//// Sizes for entities ///////////////////
///////////////////////////////////////////

#define GAME_WINDOW_WIDTH 640
#define GAME_WINDOW_HEIGHT 480

#define SHIP_WIDTH 64
#define SHIP_HEIGHT 48

///////////////////////////////////////////
//// Start positions for entities /////////
///////////////////////////////////////////

#define SHIP_START_POSITION_X 20
#define SHIP_START_POSITION_Y 240

///////////////////////////////////////////
//// Velocities for entities //////////////
///////////////////////////////////////////

#define SHIP_VELOCITY 8

#endif