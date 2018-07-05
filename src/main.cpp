#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include "3rdparty/mlibc_log.h"
#include "emu/window.h"
#include "mem/memory_area.h"
#include "mem/rom.h"
#include "mem/ram.h"
#include "cpu/irq.h"
#include "io/joypad.h"
#include "io/timer.h"
#include "ppu/ppu.h"
#include "mem/mmu.h"
#include "cpu/cpu.h"

mlibc_log_logger * mlibc_log_instance = NULL;

int main(int argc, char * argv[])
{
	int return_code = 0;

	// Init mlibc_log
	return_code = mlibc_log_init(MLIBC_LOG_LEVEL_DBG);
	if (return_code != MLIBC_LOG_CODE_OK)
	{
		throw std::runtime_error("::main(), mlibc_log_init error: " + return_code);
	}
	mlibc_inf("::main(), mlibc_log_init successful.");

	// Init SDL2
	return_code = SDL_Init(SDL_INIT_VIDEO);
	if (return_code != 0)
	{
		throw std::runtime_error("::main(), SDL_Init error: " + std::string(SDL_GetError()));
	}
	mlibc_inf("::main(), SDL2 Initialized successfully.");

	// Create memory debug window
	auto window_memory = Window::create("MEMORY", 256, 256, 2, false);

	// Create I/O devices
	hgb::IRQ irq;
	hgb::Joypad joy;
	hgb::Timer timer;
	hgb::PPU ppu;

	// Create MMU
	hgb::MMU mmu(irq, joy, timer, ppu);

	// Create CPU
	hgb::CPU cpu(mmu);

	cpu.getBreakpoints().push_back(0x0100);

	// Load ROM file
	cpu.getMMU().loadROM("Tetris-USA.gb");

	// Run the CPU, visualize memory
	bool running = true;
	int frame = 0;
	while (running)
	{
		// CPU tick
		cpu.tick();

		// PPU tick
		ppu.tick();

		if (frame % 1000 == 0)
		{
			// Render memory
			for (word i = 0; i < 0xFFFF; i++)
			{
				byte val = cpu.getMMU().read(i);

				byte r = 0x00, g = 0x00, b = 0x00;
				r = (i >= 0x0000 && i < 0x8000) ? val : 0x00;
				g = (i >= 0x8000 && i < 0xA000) ? val : 0x00;
				b = (i >= 0xA000) ? val : 0x00;

				Window::set_pixel(window_memory, i, r, g, b);
			}

			Window::render(window_memory);

			// Handle SDL2 events
			SDL_Event evt;
			while (SDL_PollEvent(&evt))
			{
				switch (evt.type)
				{
					case SDL_QUIT: running = false; break;
				}
			}
		}

		frame++;
	}

	Window::free(window_memory);

	return 0;
}