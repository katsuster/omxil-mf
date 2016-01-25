
//にほんご

#include "packet.hpp"

namespace mf {

packet::packet(uint64_t unique_id)
	: id(unique_id)
{
}

packet::~packet()
{
}

uint64_t packet::get_id() const
{
	return id;
}

void packet::dump() const
{
	dump(*this);
}

void packet::dump(const packet& p)
{
	printf(FORMAT_STRING_LL,
		"id",
		p.id);
}

}
