#include <iostream>
#include <vector>
#include "3rdparty/mlibc_log.h"
#include "mem/memory_area.h"
#include "mem/rom.h"
#include "mem/ram.h"
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

	std::vector<word> breakpoints;
	breakpoints.push_back(0x006a);
	hgb::CPU * cpu = new hgb::CPU(breakpoints);

	// Run the CPU for n instructions
	for (size_t i = 0; i < 100000; i++)
	{
		cpu->tick();
	}

	/*// dump ROM
	mlibc_dbg("::main(), ROM dump: ");
	for (uint16_t i = 0x0000; i < 0x4000; i++)
	{
		printf("%02x ", cpu->getMMU()->getByte(i));

		if (i != 0 && i % 16 == 0)
			printf("\n");
	}*/

	getchar();
	delete cpu;
	getchar();

	return 0;
}