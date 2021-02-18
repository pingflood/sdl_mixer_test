#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <signal.h>
#include "font.h"

#define WIDTH  320
#define HEIGHT 240

#define RG_A              SDLK_LCTRL
#define RG_B              SDLK_LALT
#define RG_X              SDLK_SPACE
#define RG_Y              SDLK_LSHIFT
#define RG_L              SDLK_TAB
#define RG_R              SDLK_BACKSPACE
#define RG_POWER          SDLK_END
#define RG_BACKLIGHT      SDLK_3

SDL_Surface *screen = NULL;
Mix_Music *music = NULL;
TTF_Font *font = NULL;
SDL_Color txtColor = {200, 200, 220};
SDL_Color titleColor = {200, 200, 0};
SDL_Color subTitleColor = {0, 200, 0};
SDL_Color powerColor = {200, 0, 0};
uint8_t *keys;

uint8_t nextline = 24;

int draw_text(int x, int y, const char buf[64], SDL_Color txtColor) {
	if (!strcmp(buf, "")) return y;
	SDL_Surface *msg = TTF_RenderText_Blended(font, buf, txtColor);
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = msg->w;
	rect.h = msg->h;
	SDL_BlitSurface(msg, NULL, screen, &rect);
	SDL_FreeSurface(msg);
	return y + msg->h + 2;
}

int draw_screen(const char title[64], const char footer[64]) {
	SDL_Rect rect;
	rect.w = WIDTH;
	rect.h = HEIGHT;
	rect.x = 0;
	rect.y = 0;

	// SDL_BlitSurface(bg, NULL, screen, NULL);
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

	// title
	draw_text(247, 4, "RetroFW", titleColor);
	draw_text(10, 4, title, titleColor);

	rect.w = WIDTH - 20;
	rect.h = 1;
	rect.x = 10;
	rect.y = 20;
	SDL_FillRect(screen, &rect,  SDL_MapRGB(screen->format, 200, 200, 200));

	rect.y = HEIGHT - 20;
	SDL_FillRect(screen, &rect,  SDL_MapRGB(screen->format, 200, 200, 200));

	draw_text(10, 222, footer, powerColor);

	return 32;
}

void chunkPlay(const char *file) {
	nextline = draw_screen("SDL MIXER TEST - CHUNK", "Select: Stop   Select+Start: Exit");
	nextline = draw_text(10, nextline, file, txtColor);

	Mix_Chunk *chunk = Mix_LoadWAV(file);

    if (chunk == NULL) {
		nextline = draw_text(10, nextline, "Mix_LoadWAV Error:", txtColor);
		nextline = draw_text(10, nextline, Mix_GetError(), txtColor);
		printf("Mix_LoadWAV: %s\n", Mix_GetError());
    }

	Mix_PlayChannel(-1, chunk, 0);
	SDL_Flip(screen);
}

void musicPlay(const char *file) {
	nextline = draw_screen("SDL MIXER TEST - MUSIC", "Select: Stop   Select+Start: Exit");
	nextline = draw_text(10, nextline, file, txtColor);

	music = Mix_LoadMUS(file);

    if (music == NULL) {
		nextline = draw_text(10, nextline, "Mix_LoadMUS Error:", txtColor);
		nextline = draw_text(10, nextline, Mix_GetError(), txtColor);
		printf("Mix_LoadMUS: %s\n", Mix_GetError());
    }

	Mix_PlayMusic(music, 0);
	// Mix_HookMusicFinished(musicStop);
	SDL_Flip(screen);
}

void musicSetPosition(double time) {
	// Read: https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_65.html
	Mix_RewindMusic();

	if (Mix_SetMusicPosition(time)) {
		nextline = draw_text(10, nextline, "Mix_SetMusicPosition Error:", txtColor);
		nextline = draw_text(10, nextline, Mix_GetError(), txtColor);
		printf("Mix_SetMusicPosition: %s\n", Mix_GetError());
	}
}

void quit(int err) {
	Mix_HaltMusic();
	Mix_FreeMusic(music);
	font = NULL;
	Mix_CloseAudio();
	SDL_Quit();
	TTF_Quit();
	exit(err);
}

int main(void) {
	signal(SIGINT, &quit);
	signal(SIGSEGV,&quit);
	signal(SIGTERM,&quit);

	keys = SDL_GetKeyState(NULL);
	SDL_Event event;
	int done = 0;

	setenv("SDL_NOMOUSE", "1", 1);

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		printf("%s:%d SDL_Init failed: %s \n", __func__, __LINE__, SDL_GetError());
		quit(1);
	}

	if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096)) {
		printf("%s:%d Mix_OpenAudio failed: %s \n", __func__, __LINE__, SDL_GetError());
		quit(1);
	}

	SDL_ShowCursor(0);

	screen = SDL_SetVideoMode(320, 240, 16, SDL_HWSURFACE);
	if (TTF_Init() == -1) {
		printf("%s:%d TTF_Init failed: %s \n", __func__, __LINE__, SDL_GetError());
		quit(1);
	}

	font = TTF_OpenFontRW(SDL_RWFromMem(rwfont, sizeof(rwfont)), 1, 12);
	TTF_SetFontHinting(font, TTF_HINTING_NORMAL);
	TTF_SetFontOutline(font, 0);

	nextline = draw_screen("SDL MIXER TEST", "Select: Stop   Select+Start: Exit");
	SDL_Flip(screen);

	while(!done) {
		while (SDL_WaitEvent(&event)) {
			if (keys[SDLK_ESCAPE] && keys[SDLK_RETURN]) {
				quit(0);
			}


			if (event.type == SDL_KEYDOWN) {
				printf("%d\n", event.key.keysym.sym);

				switch(event.key.keysym.sym) {
					case SDLK_ESCAPE:
						Mix_HaltChannel(-1);
						Mix_HaltMusic();

						nextline = draw_screen("SDL MIXER TEST", "Select: Stop   Select+Start: Exit");
						nextline = draw_text(10, nextline, "Music Stop", txtColor);
						SDL_Flip(screen);
						break;

					case RG_A:
					case SDLK_a:
						musicPlay("sf2.mp3");
						break;

					case RG_B:
					case SDLK_b:
						musicPlay("pinball_mania_2.mod");
						break;

					case RG_X:
					case SDLK_x:
						musicPlay("ryu.wav");
						break;

					case RG_Y:
					case SDLK_y:
						musicPlay("zangief.ogg");
						break;

					case RG_R:
					case SDLK_r:
						musicPlay("bt-theme.mid");
						break;

					case RG_L:
					case SDLK_l:
						// musicPlay("lg.mp3");
						break;

					case SDLK_RIGHT:
						musicSetPosition(15.0);
						break;

					case SDLK_LEFT:
						chunkPlay("sonicboom.mp3");
						break;

					case SDLK_UP:
						chunkPlay("shoryuken.ogg");
						break;

					case SDLK_DOWN:
						chunkPlay("hadouken.wav");
						break;

					default:
						nextline = draw_screen("SDL MIXER TEST", "Select: Stop   Select+Start: Exit");
						nextline = draw_text(10, nextline, "No sound assigned to button", txtColor);
						SDL_Flip(screen);
				}
			}
		}
	}
}
