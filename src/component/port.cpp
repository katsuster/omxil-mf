#include <sys/prctl.h>

#include <omxil_mf/component.hpp>
#include <omxil_mf/port.hpp>

#include "api/consts.hpp"
#include "util/omx_enum_name.hpp"
#include "debug/scoped_log.hpp"

//前半: port_buffer クラス
//後半: port クラス

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

void port_buffer::set_index(size_t new_index)
{
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

void port_buffer::dump(const char *msg)
{
	dprint("port_buffer dump: %s: nFlags:0x%08x, index:%6d, remain:%6d, "
		"nOffset:%6d, nFill:%6d, nAlloc:%d, nTime:%d.\n",
		msg, (int)header->nFlags, (int)index, (int)remain(),
		(int)header->nOffset, (int)header->nFilledLen,
		(int)header->nAllocLen, (int)header->nTimeStamp);
}




port::port(int ind, component *c)
	: broken(false), comp(c),
	port_index(ind), dir(OMX_DirMax),
	buffer_count_actual(0), buffer_count_min(1), buffer_size(1),
	f_enabled(OMX_TRUE), f_populated(OMX_FALSE),
	domain(OMX_PortDomainMax),
	buffers_contiguous(OMX_FALSE), buffer_alignment(0),
	ring_send(nullptr), bound_send(nullptr),
	ring_ret(nullptr), bound_ret(nullptr), th_ret(nullptr)
{
	scoped_log_begin;

	try {
		//creating ring buffer for sending OpenMAX buffers
		ring_send  = new ring_buffer<port_buffer>(nullptr, OMX_MF_BUFS_DEPTH + 1);
		bound_send = new bounded_buffer<ring_buffer<port_buffer>, port_buffer>(*ring_send);

		//creating ring buffer for returning OpenMAX buffers
		ring_ret  = new ring_buffer<port_buffer>(nullptr, OMX_MF_BUFS_DEPTH + 1);
		bound_ret = new bounded_buffer<ring_buffer<port_buffer>, port_buffer>(*ring_ret);

		//start returning OpenMAX buffers thread
		th_ret = new std::thread(buffer_done_thread_main, this);
	} catch (const std::bad_alloc& e) {
		errprint("failed to construct '%s'.\n", e.what());

		delete th_ret;
		th_ret = nullptr;

		delete bound_ret;
		bound_ret = nullptr;
		delete ring_ret;
		ring_ret = nullptr;

		delete bound_send;
		bound_send = nullptr;
		delete ring_send;
		ring_send = nullptr;

		throw;
	}
}

port::~port()
{
	scoped_log_begin;

	shutdown();

	//shutdown returning OpenMAX buffers thread
	if (bound_ret) {
		bound_ret->shutdown();
	}
	if (th_ret) {
		th_ret->join();
	}
	delete th_ret;
	delete bound_ret;
	delete ring_ret;

	//shutdown sending OpenMAX buffers thread
	if (bound_send) {
		bound_send->shutdown();
	}
	delete bound_send;
	delete ring_send;
}

const char *port::get_name() const
{
	return "port";
}

const component *port::get_component() const
{
	return comp;
}


component *port::get_component()
{
	return comp;
}

void port::shutdown()
{
	std::lock_guard<std::recursive_mutex> lk_port(mut);

	broken = true;
	cond.notify_all();
}

OMX_U32 port::get_port_index() const
{
	return port_index;
}

void port::set_port_index(OMX_U32 v)
{
	port_index = v;
}

OMX_DIRTYPE port::get_dir() const
{
	return dir;
}

void port::set_dir(OMX_DIRTYPE v)
{
	dir = v;
}

OMX_U32 port::get_buffer_count_actual() const
{
	return buffer_count_actual;
}

void port::set_buffer_count_actual(OMX_U32 v)
{
	buffer_count_actual = v;
}

OMX_U32 port::get_buffer_count_min() const
{
	return buffer_count_min;
}

void port::set_buffer_count_min(OMX_U32 v)
{
	buffer_count_min = v;
}

OMX_U32 port::get_buffer_size() const
{
	return buffer_size;
}

void port::set_buffer_size(OMX_U32 v)
{
	buffer_size = v;
}

OMX_BOOL port::get_enabled() const
{
	return f_enabled;
}

void port::set_enabled(OMX_BOOL v)
{
	std::lock_guard<std::recursive_mutex> lk_port(mut);

	f_enabled = v;
	cond.notify_all();
}

OMX_BOOL port::get_populated() const
{
	return f_populated;
}

void port::set_populated(OMX_BOOL v)
{
	std::lock_guard<std::recursive_mutex> lk_port(mut);

	f_populated = v;
	cond.notify_all();
}

void port::wait_populated(OMX_BOOL v)
{
	std::unique_lock<std::recursive_mutex> lk_port(mut);

	cond.wait(lk_port, [&] { return broken || f_populated == v; });
	error_if_broken(lk_port);
}

OMX_PORTDOMAINTYPE port::get_domain() const
{
	return domain;
}

void port::set_domain(OMX_PORTDOMAINTYPE v)
{
	domain = v;
}

OMX_BOOL port::get_buffers_contiguous() const
{
	return buffers_contiguous;
}

void port::set_buffers_contiguous(OMX_BOOL v)
{
	buffers_contiguous = v;
}

OMX_U32 port::get_buffer_alignment() const
{
	return buffer_alignment;
}

void port::set_buffer_alignment(OMX_U32 v)
{
	buffer_alignment = v;
}

const OMX_PARAM_PORTDEFINITIONTYPE *port::get_definition() const
{
	scoped_log_begin;

	memset(&definition, 0, sizeof(definition));

	definition.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	definition.nVersion.s.nVersionMajor = OMX_MF_IL_MAJOR;
	definition.nVersion.s.nVersionMinor = OMX_MF_IL_MINOR;
	definition.nVersion.s.nRevision     = OMX_MF_IL_REVISION;
	definition.nVersion.s.nStep         = OMX_MF_IL_STEP;
	definition.nPortIndex         = port_index;
	definition.eDir               = dir;
	definition.nBufferCountActual = buffer_count_actual;
	definition.nBufferCountMin    = buffer_count_min;
	definition.nBufferSize        = buffer_size;
	definition.bEnabled           = f_enabled;
	definition.bPopulated         = f_populated;
	definition.eDomain            = domain;
	//definition.format is not set
	definition.bBuffersContiguous = buffers_contiguous;
	definition.nBufferAlignment   = buffer_alignment;

	return &definition;
}

OMX_ERRORTYPE port::set_definition(const OMX_PARAM_PORTDEFINITIONTYPE& v)
{
	scoped_log_begin;

	dir                 = v.eDir;
	buffer_count_actual = v.nBufferCountActual;
	buffer_count_min    = v.nBufferCountMin;
	buffer_size         = v.nBufferSize;
	f_enabled           = v.bEnabled;
	f_populated         = v.bPopulated;
	domain              = v.eDomain;
	//definition.format is ignored
	buffers_contiguous  = v.bBuffersContiguous;
	buffer_alignment    = v.nBufferAlignment;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::disable_port()
{
	scoped_log_begin;

	if (!get_enabled()) {
		errprint("port %d is already disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}



	set_enabled(OMX_FALSE);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::enable_port()
{
	scoped_log_begin;

	if (get_enabled()) {
		errprint("port %d is already enabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}


	set_enabled(OMX_TRUE);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::flush_buffers()
{
	scoped_log_begin;

	if (!get_enabled()) {
		errprint("port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}



	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::component_tunnel_request(OMX_HANDLETYPE omx_comp, OMX_U32 index, OMX_TUNNELSETUPTYPE *setup)
{
	scoped_log_begin;
	//do nothing

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::allocate_tunnel_buffer(OMX_U32 index)
{
	scoped_log_begin;
	//do nothing

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::free_tunnel_buffer(OMX_U32 index)
{
	scoped_log_begin;
	//do nothing

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::use_buffer(OMX_BUFFERHEADERTYPE **bufhead, OMX_PTR priv, OMX_U32 size, OMX_U8 *buf)
{
	scoped_log_begin;
	port_buffer *pb = nullptr;
	OMX_BUFFERHEADERTYPE *header = nullptr;
	OMX_ERRORTYPE err;

	if (!get_enabled()) {
		errprint("port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	try {
		//allocate buffer of port
		pb = new port_buffer();

		//allocate OpenMAX BUFFERHEADER
		header = new OMX_BUFFERHEADERTYPE();
	} catch (const std::bad_alloc& e) {
		errprint("failed to allocate '%s'.\n", e.what());
		err = OMX_ErrorInsufficientResources;
		goto err_out;
	}

	{
		std::lock_guard<std::recursive_mutex> lk_port(mut);
		std::lock_guard<std::recursive_mutex> lk_buf(mut_list_bufs);

		//init buffer of port
		pb->p           = this;
		pb->f_allocate  = false;
		pb->header      = header;
		pb->index       = 0;

		list_bufs.push_back(pb);
		if (list_bufs.size() >= buffer_count_actual) {
			set_populated(OMX_TRUE);
		}

		//init OpenMAX BUFFERHEADER
		header->nSize = sizeof(OMX_BUFFERHEADERTYPE);
		header->nVersion.s.nVersionMajor = OMX_MF_IL_MAJOR;
		header->nVersion.s.nVersionMinor = OMX_MF_IL_MINOR;
		header->nVersion.s.nRevision     = OMX_MF_IL_REVISION;
		header->nVersion.s.nStep         = OMX_MF_IL_STEP;
		header->pBuffer              = buf;
		header->nAllocLen            = size;
		header->nFilledLen           = 0;
		header->nOffset              = 0;
		header->pAppPrivate          = priv;
		header->pPlatformPrivate     = pb;
		if (get_dir() == OMX_DirInput) {
			header->pInputPortPrivate  = nullptr;
			header->nInputPortIndex    = get_port_index();
		} else if (get_dir() == OMX_DirOutput) {
			header->pOutputPortPrivate = nullptr;
			header->nOutputPortIndex   = get_port_index();
		} else {
			errprint("unknown direction.\n");
			err = OMX_ErrorBadPortIndex;
			goto err_out;
		}
		header->hMarkTargetComponent = nullptr;
		header->pMarkData            = nullptr;
		header->nTickCount           = 0;
		header->nTimeStamp           = 0;
		header->nFlags               = 0;
	}

	//return result
	*bufhead = header;

	//success
	err = OMX_ErrorNone;

err_out:
	if (err != OMX_ErrorNone) {
		delete header;
		delete pb;
	}

	return err;
}

OMX_ERRORTYPE port::allocate_buffer(OMX_BUFFERHEADERTYPE **bufhead, OMX_PTR priv, OMX_U32 size)
{
	scoped_log_begin;
	OMX_U8 *backbuf = nullptr;
	port_buffer *pb = nullptr;
	OMX_BUFFERHEADERTYPE *header = nullptr;
	OMX_ERRORTYPE err;

	if (!get_enabled()) {
		errprint("port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	try {
		//allocate back buffer of OpenMAX buffer
		backbuf = new OMX_U8[size];

		//allocate buffer of port
		pb = new port_buffer();

		//allocate OpenMAX BUFFERHEADER
		header = new OMX_BUFFERHEADERTYPE();
	} catch (const std::bad_alloc& e) {
		errprint("failed to allocate '%s'.\n", e.what());
		err = OMX_ErrorInsufficientResources;
		goto err_out;
	}

	{
		std::lock_guard<std::recursive_mutex> lk_port(mut);
		std::lock_guard<std::recursive_mutex> lk_buf(mut_list_bufs);

		//init buffer of port
		pb->p           = this;
		pb->f_allocate  = true;
		pb->header      = header;
		pb->index       = 0;

		list_bufs.push_back(pb);
		if (list_bufs.size() >= buffer_count_actual) {
			set_populated(OMX_TRUE);
		}

		//init OpenMAX BUFFERHEADER
		header->nSize = sizeof(OMX_BUFFERHEADERTYPE);
		header->nVersion.s.nVersionMajor = OMX_MF_IL_MAJOR;
		header->nVersion.s.nVersionMinor = OMX_MF_IL_MINOR;
		header->nVersion.s.nRevision     = OMX_MF_IL_REVISION;
		header->nVersion.s.nStep         = OMX_MF_IL_STEP;
		header->pBuffer              = backbuf;
		header->nAllocLen            = size;
		header->nFilledLen           = 0;
		header->nOffset              = 0;
		header->pAppPrivate          = priv;
		header->pPlatformPrivate     = pb;
		if (get_dir() == OMX_DirInput) {
			header->pInputPortPrivate  = nullptr;
			header->nInputPortIndex    = get_port_index();
		} else if (get_dir() == OMX_DirOutput) {
			header->pOutputPortPrivate = nullptr;
			header->nOutputPortIndex   = get_port_index();
		} else {
			errprint("unknown direction.\n");
			err = OMX_ErrorBadPortIndex;
			goto err_out;
		}
		header->hMarkTargetComponent = nullptr;
		header->pMarkData            = nullptr;
		header->nTickCount           = 0;
		header->nTimeStamp           = 0;
		header->nFlags               = 0;
	}

	//return result
	*bufhead = header;

	//success
	err = OMX_ErrorNone;

err_out:
	if (err != OMX_ErrorNone) {
		delete header;
		delete pb;
		delete[] backbuf;
	}

	return err;
}

OMX_ERRORTYPE port::free_buffer(OMX_BUFFERHEADERTYPE *bufhead)
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lk_port(mut);
	std::lock_guard<std::recursive_mutex> lk_buf(mut_list_bufs);
	std::vector<port_buffer *>::iterator it;
	port_buffer *pb;

	for (it = list_bufs.begin(); it != list_bufs.end(); it++) {
		pb = *it;

		if (pb->header != bufhead) {
			//not match
			continue;
		}

		list_bufs.erase(it);
		if (list_bufs.size() < buffer_count_actual) {
			set_populated(OMX_FALSE);
		}

		if (pb->f_allocate) {
			delete[] pb->header->pBuffer;
			pb->header->pBuffer = nullptr;
		}
		delete pb->header;
		pb->header = nullptr;
		delete pb;

		break;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::free_buffer(port_buffer *pb)
{
	scoped_log_begin;
	return free_buffer(pb->header);
}

bool port::find_buffer(OMX_BUFFERHEADERTYPE *bufhead)
{
	std::lock_guard<std::recursive_mutex> lk_buf(mut_list_bufs);

	for (port_buffer *pb : list_bufs) {
		if (pb->header->pBuffer == bufhead->pBuffer) {
			//found
			return OMX_ErrorNone;
		}
	}

	return OMX_ErrorBadParameter;
}

bool port::find_buffer(port_buffer *pb)
{
	return find_buffer(pb->header);
}


//----------------------------------------
//コンポーネント利用者 → コンポーネントへのバッファ送付
//----------------------------------------

OMX_ERRORTYPE port::empty_buffer(OMX_BUFFERHEADERTYPE *bufhead)
{
	//scoped_log_begin;
	OMX_ERRORTYPE err;

	if (get_dir() != OMX_DirInput) {
		errprint("port:%d is not input.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	err = push_buffer(bufhead);

	return err;
}

OMX_ERRORTYPE port::fill_buffer(OMX_BUFFERHEADERTYPE *bufhead)
{
	//scoped_log_begin;
	OMX_ERRORTYPE err;

	if (get_dir() != OMX_DirOutput) {
		errprint("port:%d is not output.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	err = push_buffer(bufhead);

	return err;
}

OMX_ERRORTYPE port::push_buffer(OMX_BUFFERHEADERTYPE *bufhead)
{
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

	if (!get_enabled()) {
		errprint("port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
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

OMX_ERRORTYPE port::pop_buffer(port_buffer *pb)
{
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

int port::get_buffer_count()
{
	return bound_send->size();
}




//----------------------------------------
//コンポーネント → コンポーネント利用者へのバッファ返却
//----------------------------------------

OMX_ERRORTYPE port::empty_buffer_done(OMX_BUFFERHEADERTYPE *bufhead)
{
	//scoped_log_begin;
	OMX_ERRORTYPE err;

	if (get_dir() != OMX_DirInput) {
		errprint("port:%d is not input.\n",
			(int)get_port_index());
		return OMX_ErrorBadPortIndex;
	}

	err = push_buffer_done(bufhead);

	return err;
}

OMX_ERRORTYPE port::empty_buffer_done(port_buffer *pb)
{
	//scoped_log_begin;

	if (!find_buffer(pb)) {
		dprint("port_buffer:%p is not registered.\n", pb);
	}

	return empty_buffer_done(pb->header);
}

OMX_ERRORTYPE port::fill_buffer_done(OMX_BUFFERHEADERTYPE *bufhead)
{
	//scoped_log_begin;
	OMX_ERRORTYPE err;

	if (get_dir() != OMX_DirOutput) {
		errprint("port:%d is not output.\n",
			(int)get_port_index());
		return OMX_ErrorBadPortIndex;
	}

	err = push_buffer_done(bufhead);

	return err;
}

OMX_ERRORTYPE port::fill_buffer_done(port_buffer *pb)
{
	//scoped_log_begin;

	if (!find_buffer(pb)) {
		dprint("port_buffer:%p is not registered.\n", pb);
	}

	return fill_buffer_done(pb->header);
}

OMX_ERRORTYPE port::push_buffer_done(OMX_BUFFERHEADERTYPE *bufhead)
{
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




/*
 * protected functions
 */

void port::error_if_broken(std::unique_lock<std::recursive_mutex>& lk_port)
{
	if (broken) {
		std::string msg(__func__);
		msg += ": interrupted.";
		throw std::runtime_error(msg);
	}
}

//----------------------------------------
//コンポーネント利用者へのバッファ返却スレッド
//----------------------------------------

void *port::buffer_done()
{
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

void *port::buffer_done_thread_main(port *p)
{
	scoped_log_begin;
	std::string thname;

	try {
		//スレッド名をつける
		thname = "omx:p";
		thname += std::to_string(p->get_port_index());
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
