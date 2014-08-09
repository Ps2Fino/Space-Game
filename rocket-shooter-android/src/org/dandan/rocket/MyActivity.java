package org.dandan.rocket;

import org.libsdl.app.SDLActivity;

/* 
 * A sample wrapper class that just calls SDLActivity
 * This game is so simple that the whole game can be
 * managed through the C++ native code and the SDL. 
 * There is no need to implement any fancy stuff on the
 * java side  
 */ 

public class MyActivity extends SDLActivity {
	
	public native void pauseMusic();
	public native void resumeMusic();
	public native void stopMusic();
	
	@Override
	protected void onPause() {
		pauseMusic();
		super.onPause();
//		pauseMusic();
	}
	
	@Override
	protected void onResume() {
		resumeMusic();
		super.onResume();
//		resumeMusic();
	}
	
	@Override
	protected void onDestroy() {
		stopMusic();
		super.onDestroy();
//		stopMusic();
	}
}