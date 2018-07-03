#include "window.h"
#include <SDL2/SDL.h>
#include "3rdparty/mlibc_log.h"

namespace Window
{

Window * create(
	const std::string & title,
	int width,
	int height,
	int scale,
	bool fullscreen
)
{
	Window * window = new Window;

	window->title = title;
	window->width = width;
	window->height = height;
	window->scale = scale;
	window->handle = SDL_CreateWindow(
		title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width * scale,
		height * scale,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | ((fullscreen) ? SDL_WINDOW_FULLSCREEN : 0)
	);

	if (window->handle == NULL)
	{
		delete window;
		mlibc_err("Window::create(%s). Error creating a new SDL_Window instance!", title.c_str());
		return nullptr;
	}

	window->renderer = SDL_CreateRenderer(
		window->handle,
		-1,
		SDL_RENDERER_ACCELERATED
	);

	if (window->renderer == NULL)
	{
		SDL_DestroyWindow(window->handle);
		delete window;
		mlibc_err("Window::create(%s). Error creating a new SDL_Renderer instance!", title.c_str());
		return nullptr;
	}

	window->texture = SDL_CreateTexture(
		window->renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STATIC,
		width,
		height
	);

	if (window->texture == NULL)
	{
		SDL_DestroyRenderer(window->renderer);
		SDL_DestroyWindow(window->handle);
		delete window;
		mlibc_err("Window::create(%s). Error creating a new SDL_Texture instance!", title.c_str());
		return nullptr;
	}

	window->framebuffer = new int32_t[width * height];
	if (window->framebuffer == nullptr)
	{
		SDL_DestroyTexture(window->texture);
		SDL_DestroyRenderer(window->renderer);
		SDL_DestroyWindow(window->handle);
		delete window;
		mlibc_err("Window::create(%s). Error allocating a new framebuffer object!", title.c_str());
		return nullptr;
	}

	mlibc_dbg("Window::create(%s)", title.c_str());

	return window;
}

void free(Window * w)
{
	delete w->framebuffer;
	SDL_DestroyTexture(w->texture);
	SDL_DestroyRenderer(w->renderer);
	SDL_DestroyWindow(w->handle);
	delete w;

	mlibc_dbg("Window::free()");
}

void render(Window * w)
{
	SDL_UpdateTexture(
		w->texture,
		NULL,
		w->framebuffer,
		w->width * sizeof(int32_t)
	);
	SDL_RenderClear(w->renderer);
	SDL_RenderCopy(w->renderer, w->texture, NULL, NULL);
	SDL_RenderPresent(w->renderer);
}

void clear(Window * w, const int32_t argb)
{
	size_t resolution = static_cast<size_t>(w->width * w->height);

	for (size_t i = 0; i < resolution; i++)
		w->framebuffer[i] = argb;
}

void set_pixel(
	Window * w,
	int i,
	uint8_t r,
	uint8_t g,
	uint8_t b
)
{
	if (i < 0 || i >= w->width * w->height)
		return;

	w->framebuffer[i] = ((r << 16) | (g << 8) | b);
}

}

