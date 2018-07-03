#ifndef WINDOW_H
#define WINDOW_H

#include <string>

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;

namespace Window
{

struct Window
{
	std::string title;
	int width, height, scale;
	SDL_Window * handle;
	SDL_Renderer * renderer;
	SDL_Texture * texture;
	int32_t * framebuffer;
};

Window * create(
	const std::string & title,
	int width = 640,
	int height = 467,
	int scale = 1,
	bool fullscreen = false
);
void free(Window * w);
void render(Window * w);
void clear(Window * w, const int32_t argb);
void set_pixel(
	Window * w,
	int i,
	uint8_t r,
	uint8_t g,
	uint8_t b
);

}

#endif // WINDOW_H