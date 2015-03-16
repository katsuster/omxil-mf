#include <vector>
#include <mutex>
#include <pthread.h>
#include <sys/prctl.h>

#include "consts.h"
#include "util/omx_enum_name.hpp"
#include "component/component.hpp"
#include "component/port.hpp"
#include "debug/scoped_log.hpp"

namespace mf {

bool port_buffer::size() const {
	return header->nFilledLen;
}

bool port_buffer::empty() const {
	return size() == 0;
}

bool port_buffer::full() const {
	return remain() == 0;
}

size_t port_buffer::remain() const {
	if (p->get_dir() == OMX_DirInput) {
		//input
		return header->nFilledLen;
	} else if (p->get_dir() == OMX_DirOutput) {
		//output
		return header->nAllocLen - index - header->nFilledLen;
	} else {
		errprint("unknown port direction.\n");
		return 0;
	}
}

size_t port_buffer::skip(size_t count) {
	size_t n;

	if (p->get_dir() != OMX_DirInput) {
		errprint("cannot skip on output port.\n");
		return 0;
	}

	n = std::min(count, remain());
	set_index(get_index() + n);

	return n;
}

size_t port_buffer::read_array(uint8_t *buf, size_t count) {
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

size_t port_buffer::write_array(uint8_t *buf, size_t count) {
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

uint8_t *port_buffer::get_ptr() {
	return &header->pBuffer[get_index()];
}

size_t port_buffer::get_index() const {
	if (p->get_dir() == OMX_DirInput) {
		//input
		return index;
	} else if (p->get_dir() == OMX_DirOutput) {
		//output
		return index + header->nFilledLen;
	} else {
		errprint("unknown port direction.\n");
		return 0;
	}
}

void port_buffer::set_index(size_t new_index) {
	size_t skip_size = new_index - get_index();

	if (p->get_dir() == OMX_DirInput) {
		//input
		index = new_index;
		header->nFilledLen -= skip_size;
	} else if (p->get_dir() == OMX_DirOutput) {
		//output
		header->nFilledLen += skip_size;
	} else {
		errprint("unknown port direction.\n");
	}
}

void port_buffer::dump(const char *msg) {
	dprint("port_buffer dump: %s: nFlags:0x%08x, index:%6d, remain:%6d, "
		"nOffset:%6d, nFill:%6d, nAlloc:%d, nTime:%d.\n",
		msg, (int)header->nFlags, (int)index, (int)remain(),
		(int)header->nOffset, (int)header->nFilledLen,
		(int)header->nAllocLen, (int)header->nTimeStamp);
}




//----------------------------------------
//コンポーネント利用者 → コンポーネントへのバッファ送付
//----------------------------------------

OMX_ERRORTYPE port::empty_buffer(OMX_BUFFERHEADERTYPE *bufhead) {
	//scoped_log_begin;
	OMX_ERRORTYPE err;

	if (get_dir() != OMX_DirInput) {
		errprint("port:%d is not input.\n", (int)get_index());
		return OMX_ErrorBadPortIndex;
	}

	err = push_buffer(bufhead);

	return err;
}

OMX_ERRORTYPE port::fill_buffer(OMX_BUFFERHEADERTYPE *bufhead) {
	//scoped_log_begin;
	OMX_ERRORTYPE err;

	if (get_dir() != OMX_DirOutput) {
		errprint("port:%d is not output.\n", (int)get_index());
		return OMX_ErrorBadPortIndex;
	}

	err = push_buffer(bufhead);

	return err;
}

OMX_ERRORTYPE port::push_buffer(OMX_BUFFERHEADERTYPE *bufhead) {
	port_buffer pb;
	OMX_ERRORTYPE err;

	switch (get_component()->get_state()) {
	case OMX_StateIdle:
	case OMX_StateExecuting:
	case OMX_StatePause:
		//OK
		break;
	default:
		//NG
		errprint("invalid state:%s.\n",
			omx_enum_name::get_OMX_STATETYPE_name(get_component()->get_state()));
		return OMX_ErrorInvalidState;
	}

	if (!find_buffer(bufhead)) {
		dprint("buffer header:%p is not registered.\n", bufhead);
	}

	try {
		pb.p          = this;
		pb.f_allocate = false;
		pb.header     = bufhead;
		pb.index      = bufhead->nOffset;

		bound_send->write_fully(&pb, 1);

		err = OMX_ErrorNone;
	} catch (const std::runtime_error& e) {
		errprint("runtime_error: %s\n", e.what());

		err = OMX_ErrorInsufficientResources;
	}

	return err;
}

OMX_ERRORTYPE port::pop_buffer(port_buffer *pb) {
	//scoped_log_begin;
	OMX_ERRORTYPE err;

	try {
		bound_send->read_fully(pb, 1);

		err = OMX_ErrorNone;
	} catch (const std::runtime_error& e) {
		errprint("runtime_error: %s\n", e.what());

		err = OMX_ErrorInsufficientResources;
	}

	return err;
}




//----------------------------------------
//コンポーネント → コンポーネント利用者へのバッファ返却
//----------------------------------------

OMX_ERRORTYPE port::empty_buffer_done(OMX_BUFFERHEADERTYPE *bufhead) {
	//scoped_log_begin;
	OMX_ERRORTYPE err;

	if (get_dir() != OMX_DirInput) {
		errprint("port:%d is not input.\n", (int)get_index());
		return OMX_ErrorBadPortIndex;
	}

	err = push_buffer_done(bufhead);

	return err;
}

OMX_ERRORTYPE port::empty_buffer_done(port_buffer *pb) {
	//scoped_log_begin;

	if (!find_buffer(pb)) {
		dprint("port_buffer:%p is not registered.\n", pb);
	}

	return empty_buffer_done(pb->header);
}

OMX_ERRORTYPE port::fill_buffer_done(OMX_BUFFERHEADERTYPE *bufhead) {
	//scoped_log_begin;
	OMX_ERRORTYPE err;

	if (get_dir() != OMX_DirOutput) {
		errprint("port:%d is not output.\n", (int)get_index());
		return OMX_ErrorBadPortIndex;
	}

	err = push_buffer_done(bufhead);

	return err;
}

OMX_ERRORTYPE port::fill_buffer_done(port_buffer *pb) {
	//scoped_log_begin;

	if (!find_buffer(pb)) {
		dprint("port_buffer:%p is not registered.\n", pb);
	}

	return fill_buffer_done(pb->header);
}

OMX_ERRORTYPE port::push_buffer_done(OMX_BUFFERHEADERTYPE *bufhead) {
	//scoped_log_begin;
	port_buffer pb;
	OMX_ERRORTYPE err;

	try {
		pb.p = this;
		pb.f_allocate = false;
		pb.header = bufhead;

		bound_ret->write_fully(&pb, 1);

		err = OMX_ErrorNone;
	} catch (const std::runtime_error& e) {
		errprint("runtime_error: %s\n", e.what());

		err = OMX_ErrorInsufficientResources;
	}

	return err;
}




//----------------------------------------
//コンポーネント利用者へのバッファ返却スレッド
//----------------------------------------

void *port::buffer_done() {
	scoped_log_begin;
	port_buffer pb;
	component *comp;
	bool f_callback;
	OMX_ERRORTYPE err, err_handler;

	while (1) {
		//blocked read
		bound_ret->read_fully(&pb, 1);

		comp = pb.p->get_component();

		err = OMX_ErrorNone;
		err_handler = OMX_ErrorNone;
		f_callback = true;

		//callback Done event
		if (pb.p->get_dir() == OMX_DirInput) {
			err = comp->EmptyBufferDone(&pb);
		} else if (pb.p->get_dir() == OMX_DirOutput) {
			err = comp->FillBufferDone(&pb);
		} else {
			errprint("unknown direction.\n");
			err = OMX_ErrorBadPortIndex;
		}

		//error event callback
		if (f_callback && err != OMX_ErrorNone) {
			err_handler = comp->EventHandler(OMX_EventError,
				err, 0, nullptr);
		}
		if (err_handler != OMX_ErrorNone) {
			errprint("error handler returns error: %s\n",
				omx_enum_name::get_OMX_ERRORTYPE_name(err_handler));
		}
	}

	return nullptr;
}

void *port::buffer_done_thread_main(port *p) {
	scoped_log_begin;
	std::string thname;

	try {
		//スレッド名をつける
		thname = "omx:p";
		thname += std::to_string(p->get_index());
		thname += ":";
		thname += p->get_component()->get_name();
		prctl(PR_SET_NAME, thname.c_str());

		p->buffer_done();
	} catch (const std::runtime_error& e) {
		errprint("runtime_error: %s\n", e.what());
	}

	return nullptr;
}

} //namespace mf
