#include <sys/prctl.h>

#include <omxil_mf/component.hpp>
#include <omxil_mf/port.hpp>
#include <omxil_mf/scoped_log.hpp>

#include "api/consts.hpp"
#include "util/omx_enum_name.hpp"

//port クラス

namespace mf {

port::port(int ind, component *c)
	: shutting_read(false), shutting_write(false), comp(c),
	port_index(ind), dir(OMX_DirMax),
	buffer_count_actual(0), buffer_count_min(0), buffer_size(0),
	f_enabled(OMX_TRUE), f_populated(OMX_FALSE),
	domain(OMX_PortDomainMax),
	buffers_contiguous(OMX_FALSE), buffer_alignment(0),
	f_no_buffer(OMX_TRUE),
	default_format(-1),
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

	shutdown(true, true);

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

void port::shutdown(bool rd, bool wr)
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lk_port(mut);

	if (rd) {
		shutting_read = true;
	}
	if (wr) {
		shutting_write = true;
	}
	cond.notify_all();
}

void port::abort_shutdown(bool rd, bool wr)
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lk_port(mut);

	if (rd) {
		shutting_read = false;
	}
	if (wr) {
		shutting_write = false;
	}
	cond.notify_all();
}

bool port::is_shutting_read()
{
	return shutting_read;
}

bool port::is_shutting_write()
{
	return shutting_write;
}


//以下 OMX_PARAM_PORTDEFINITIONTYPE に基づくメンバ


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
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lk_port(mut);

	f_populated = v;
	cond.notify_all();
}

void port::wait_populated(OMX_BOOL v)
{
	scoped_log_begin;
	std::unique_lock<std::recursive_mutex> lk_port(mut);

	cond.wait(lk_port, [&] { return is_broken() || f_populated == v; });
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


//以上 OMX_PARAM_PORTDEFINITIONTYPE に基づくメンバ


OMX_BOOL port::get_no_buffer() const
{
	return f_no_buffer;
}

void port::set_no_buffer(OMX_BOOL v)
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lk_port(mut);

	f_no_buffer = v;
	cond.notify_all();
}

void port::wait_no_buffer(OMX_BOOL v)
{
	scoped_log_begin;
	std::unique_lock<std::recursive_mutex> lk_port(mut);

	cond.wait(lk_port, [&] { return is_broken() || f_no_buffer == v; });
	error_if_broken(lk_port);
}

void port::update_buffer_status()
{
	if (list_bufs.size() >= buffer_count_actual) {
		set_populated(OMX_TRUE);
	} else {
		set_populated(OMX_FALSE);
	}

	if (list_bufs.size() == 0) {
		set_no_buffer(OMX_TRUE);
	} else {
		set_no_buffer(OMX_FALSE);
	}
}

void port::wait_buffer_returned()
{
	scoped_log_begin;
	std::unique_lock<std::recursive_mutex> lk_port(mut);

	cond.wait(lk_port, [&] { return is_broken() || bound_ret->size() == 0; });
	error_if_broken(lk_port);
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

OMX_ERRORTYPE port::set_definition_from_client(const OMX_PARAM_PORTDEFINITIONTYPE& v)
{
	scoped_log_begin;

	//nBufferCountActual 以外は全て read-only
	buffer_count_actual = v.nBufferCountActual;
	//definition.format is ignored

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::add_port_format(const port_format& f)
{
	formats.push_back(f);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::add_port_format(const OMX_AUDIO_PARAM_PORTFORMATTYPE& f)
{
	return add_port_format(port_format(f));
}

OMX_ERRORTYPE port::add_port_format(const OMX_VIDEO_PARAM_PORTFORMATTYPE& f)
{
	return add_port_format(port_format(f));
}

OMX_ERRORTYPE port::add_port_format(const OMX_IMAGE_PARAM_PORTFORMATTYPE& f)
{
	return add_port_format(port_format(f));
}

OMX_ERRORTYPE port::add_port_format(const OMX_OTHER_PARAM_PORTFORMATTYPE& f)
{
	return add_port_format(port_format(f));
}

OMX_ERRORTYPE port::remove_port_format(size_t index)
{
	if (index < 0 || formats.size() <= index) {
		return OMX_ErrorBadParameter;
	}

	formats.erase(formats.begin() + index);

	return OMX_ErrorNone;
}

const port_format *port::get_port_format(size_t index) const
{
	if (index < 0 || formats.size() <= index) {
		return nullptr;
	}

	return &formats.at(index);
}

const port_format *port::get_port_format(const port_format& f) const
{
	size_t index;
	OMX_ERRORTYPE err;

	err = get_port_format_index(f, &index);
	if (err != OMX_ErrorNone) {
		return nullptr;
	}

	return get_port_format(index);
}

OMX_ERRORTYPE port::get_port_format_index(const port_format& f, size_t *ind) const
{
	if (ind != nullptr) {
		*ind = (size_t)~0;
	}

	return OMX_ErrorNone;
}

const port_format *port::get_default_format() const
{
	return get_port_format(default_format);
}

OMX_ERRORTYPE port::set_default_format(size_t index)
{
	if (index < 0 || formats.size() <= index) {
		return OMX_ErrorBadParameter;
	}

	default_format = index;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::set_default_format(const port_format& f)
{
	size_t index;
	OMX_ERRORTYPE err;

	err = get_port_format_index(f, &index);
	if (err != OMX_ErrorNone) {
		return err;
	}

	return set_default_format(index);
}

OMX_ERRORTYPE port::disable_port()
{
	scoped_log_begin;

	if (!get_enabled()) {
		errprint("port %d is already disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	//TODO: flush all buffers?

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

	//TODO: flush all buffers?

	set_enabled(OMX_TRUE);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::begin_flush()
{
	scoped_log_begin;
	std::unique_lock<std::recursive_mutex> lk_port(mut);

	if (!get_enabled()) {
		errprint("port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	//フラッシュ開始、ポートへの読み書き禁止
	shutdown(true, true);
	bound_send->shutdown(true, true);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::flush_buffers()
{
	scoped_log_begin;
	std::unique_lock<std::recursive_mutex> lk_port(mut);

	if (!get_enabled()) {
		errprint("port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	//NOTE: flush の前に shutdown し、
	//バッファ処理要求の read/write を停止させる必要があります。
	//コンポーネントのメインスレッドなど、
	//別スレッドとの間に race condition が存在するため、
	//別スレッドが同時に bound_send から read した場合、
	//入力順と出力順が入れ替わったり、デッドロックする可能性があります。
	while (bound_send->size() > 0) {
		port_buffer pb;
		size_t cnt;

		//NOTE: shutdown 中に read_fully を使うと
		//runtime_error がスローされるため、
		//代わりに read_array を使います。
		cnt = bound_send->read_array(&pb, 1);
		if (cnt != 0) {
			bound_ret->write_fully(&pb, 1);
			cond.notify_all();
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::end_flush()
{
	scoped_log_begin;
	std::unique_lock<std::recursive_mutex> lk_port(mut);

	if (!get_enabled()) {
		errprint("port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	//フラッシュ終了、ポートへの読み書き許可
	bound_send->abort_shutdown(true, true);
	abort_shutdown(true, true);

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

		list_bufs.push_back(pb);
		update_buffer_status();
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

		list_bufs.push_back(pb);
		update_buffer_status();
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
	bool deleted = false;

	for (it = list_bufs.begin(); it != list_bufs.end(); it++) {
		pb = *it;

		if (pb->header != bufhead) {
			//not match
			continue;
		}

		list_bufs.erase(it);
		update_buffer_status();

		if (pb->f_allocate) {
			delete[] pb->header->pBuffer;
			pb->header->pBuffer = nullptr;
		}
		delete pb->header;
		pb->header = nullptr;
		delete pb;
		deleted = true;

		break;
	}

	if (!deleted) {
		return OMX_ErrorBadParameter;
	} else {
		return OMX_ErrorNone;
	}
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
			return true;
		}
	}

	return false;
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

	if (!get_enabled()) {
		errprint("port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}
	if (is_shutting_write()) {
		errprint("port %d is flushing.\n",
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
	if (!find_buffer(pb)) {
		dprint("port_buffer:%p is not registered.\n", pb);
	}

	return empty_buffer_done(pb->header);
}

OMX_ERRORTYPE port::fill_buffer_done(OMX_BUFFERHEADERTYPE *bufhead)
{
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
	if (!find_buffer(pb)) {
		dprint("port_buffer:%p is not registered.\n", pb);
	}

	return fill_buffer_done(pb->header);
}

OMX_ERRORTYPE port::push_buffer_done(OMX_BUFFERHEADERTYPE *bufhead)
{
	port_buffer pb;
	OMX_ERRORTYPE err;

	try {
		pb.p = this;
		pb.f_allocate = false;
		pb.header = bufhead;

		bound_ret->write_fully(&pb, 1);
		cond.notify_all();

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

const std::vector<port_format>& port::get_port_format_list() const
{
	return formats;
}

void port::error_if_broken(std::unique_lock<std::recursive_mutex>& lk_port)
{
	if (is_broken()) {
		std::string msg(__func__);
		msg += ": interrupted by shutdown.";
		throw std::runtime_error(msg);
	}
}

bool port::is_broken()
{
	return is_shutting_read() && is_shutting_write();
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
		cond.notify_all();

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
