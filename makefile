CC = g++
CFLAGS = -std=c++11

all: social_media_app

social_media_app: SocialMediaApp.cpp
	$(CC) $(CFLAGS) -o social_media_app SocialMediaApp.cpp

clean:
	rm -f social_media_app
