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
	
	// We set this variable from the underlying C++ code below
	// As well as from the Java code
	boolean isPlayingMusic = false;
	
	@Override
	protected void onPause() {
		if (isPlayingMusic) {
			pauseMusic();
		}
		super.onPause();
	}
	
	@Override
	protected void onResume() {
		// This method will also be called after
		// onCreate and onStart
		
		if (isPlayingMusic) {
			resumeMusic();
		}
		super.onResume();
	}
	
	@Override
	protected void onDestroy() {
		if (isPlayingMusic) {
			stopMusic();
		}
		super.onDestroy();
	}
}