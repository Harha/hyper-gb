#ifndef OAM_H
#define OAM_H

#include "mem/memory_area.h"

namespace hgb
{

class OAM : public MemoryArea
{
public:
	OAM();

	virtual byte read(word addr) override;
	virtual void write(word addr, byte value) override;
};

}

#endif // OAM_H