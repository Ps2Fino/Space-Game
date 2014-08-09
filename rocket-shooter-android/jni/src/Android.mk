LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL2-2.0.3
SDL_TTF_PATH = ../SDL_ttf-2.0.12
SDL_MIXER_PATH = ../SDL_mixer-2.0.0
SDL_IMAGE_PATH = ../SDL_image-2.0.0

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/$(SDL_TTF_PATH) \
	$(LOCAL_PATH)/$(SDL_MIXER_PATH) $(LOCAL_PATH)/$(SDL_IMAGE_PATH) $(LOCAL_PATH)/../include

LOCAL_CPPFLAGS := -std=c++11 -fexceptions -DANDROID_BUILD

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	main.cpp \
	Asteroid.cpp \
	Background.cpp \
	Bullet.cpp \
	Destructible.cpp \
	MenuScreen.cpp \
	Ship.cpp \
	SoundFX.cpp \
	Sprite.cpp \
	Stats.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf SDL2_mixer SDL2_image

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
