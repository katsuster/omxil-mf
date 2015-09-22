
#include <omxil_mf/port_buffer.hpp>
#include <omxil_mf/port.hpp>
#include <omxil_mf/scoped_log.hpp>

//port_buffer クラス

namespace mf {

bool port_buffer::size() const
{
	return header->nFilledLen;
}

bool port_buffer::empty() const
{
	return size() == 0;
}

bool port_buffer::full() const
{
	return remain() == 0;
}

size_t port_buffer::remain() const
{
	switch (p->get_dir()) {
	case OMX_DirInput:
		return header->nFilledLen;
	case OMX_DirOutput:
		return header->nAllocLen - index - header->nFilledLen;
	default:
		errprint("Unknown port direction.\n");
		return 0;
	}
}

size_t port_buffer::skip(size_t count)
{
	size_t n;

	if (p->get_dir() != OMX_DirInput) {
		errprint("cannot skip on output port.\n");
		return 0;
	}

	n = std::min(count, remain());
	set_index(get_index() + n);

	return n;
}

size_t port_buffer::read_array(uint8_t *buf, size_t count)
{
	size_t index, n;

	if (p->get_dir() != OMX_DirInput) {
		errprint("cannot read from output port.\n");
		return 0;
	}

	index = get_index();
	n = std::min(count, remain());

	memmove(buf, &header->pBuffer[index], n);

	set_index(index + n);

	return n;
}

size_t port_buffer::write_array(uint8_t *buf, size_t count)
{
	size_t index, n;

	if (p->get_dir() != OMX_DirOutput) {
		errprint("cannot write to input port.\n");
		return 0;
	}

	index = get_index();
	n = std::min(count, remain());

	memmove(&header->pBuffer[index], buf, n);

	set_index(index + n);

	return n;
}

uint8_t *port_buffer::get_ptr()
{
	return &header->pBuffer[get_index()];
}

size_t port_buffer::get_index() const
{
	switch (p->get_dir()) {
	case OMX_DirInput:
		return index;
	case OMX_DirOutput:
		return index + header->nFilledLen;
	default:
		errprint("Unknown port direction.\n");
		return 0;
	}
}

void port_buffer::set_index(size_t new_index)
{
	size_t skip_size = new_index - get_index();

	switch (p->get_dir()) {
	case OMX_DirInput:
		index = new_index;
		header->nFilledLen -= skip_size;
	case OMX_DirOutput:
		header->nFilledLen += skip_size;
	default:
		errprint("Unknown port direction.\n");
	}
}

void port_buffer::dump(const char *msg) const
{
	dprint("port_buffer dump: %s: nFlags:0x%08x, index:%6d, remain:%6d, "
		"nOffset:%6d, nFill:%6d, nAlloc:%d, nTime:%d.\n",
		msg, (int)header->nFlags, (int)index, (int)remain(),
		(int)header->nOffset, (int)header->nFilledLen,
		(int)header->nAllocLen, (int)header->nTimeStamp);
}

} //namespace mf
