#ifndef CPU_STATE_H
#define CPU_STATE_H

namespace hgb
{

struct CPUState
{
	// # of current clock cycle
	int CLOCK;

	// Interrupts enabled or not
	bool IME;

	// Interrupts enabled after next machine cycle
	bool IME_scheduled;

	enum CPUState_t
	{
		NORMAL = 0,
		PREFIX_CB = 1,
		HALT = 2,
		STOP = 3
	} STATE;
};

}

#endif // CPU_STATE_H