
//にほんご

#include "unit.hpp"

namespace mf {

unit::unit()
	: unit_size()
{
}

unit::~unit()
{
}

uint64_t unit::size() const
{
	return unit_size;
}

void unit::size(uint64_t newsize)
{
	unit_size = newsize;
}

void unit::read(avbuffer& b)
{
	read(b, *this);
}

void unit::read(avbuffer& b, unit& p)
{
	p.size(0);
}

void unit::write(avbuffer& b)
{
	write(b, *this);
}

void unit::write(avbuffer& b, unit& p)
{
	p.size(0);
}

void unit::dump() const
{
	dump(*this);
}

void unit::dump(const unit& p)
{
	//do nothing
}

}
