
#define __OMX_MF_EXPORTS

#include <string>
#include <sstream>

#include <omxil_mf/component.hpp>
#include <omxil_mf/port.hpp>
#include <omxil_mf/scoped_log.hpp>

#include "api/consts.hpp"
#include "util/util.hpp"
#include "util/omx_enum_name.hpp"

//port クラス

namespace mf {

port::port(int ind, component *c)
	: port_index(ind), dir(OMX_DirMax),
	buffer_count_actual(0), buffer_count_min(0), buffer_size(0),
	f_enabled(OMX_TRUE), f_populated(OMX_FALSE),
	domain(OMX_PortDomainMax),
	buffers_contiguous(OMX_FALSE), buffer_alignment(0),
	comp(c), shutting_read(false), shutting_write(false),
	f_no_buffer(OMX_TRUE),
	f_tunneled(OMX_FALSE), tunneled_comp(nullptr),
	tunneled_port(0), f_tunneled_supplier(OMX_FALSE),
	default_format(-1),
	ring_send(nullptr), bound_send(nullptr),
	ring_ret(nullptr), bound_ret(nullptr), th_ret(nullptr),
	cnt_send_wr(0), cnt_recv_rd(0)
{
	scoped_log_begin;

	try {
		//creating ring buffer for sending OpenMAX buffers
		vec_send.reserve(OMX_MF_BUFS_DEPTH + 1);
		ring_send  = new portbuf_ring_t(vec_send.begin(), vec_send.capacity());
		bound_send = new portbuf_bound_t(*ring_send);

		//creating ring buffer for returning OpenMAX buffers
		vec_ret.reserve(OMX_MF_BUFS_DEPTH + 1);
		ring_ret  = new portbuf_ring_t(vec_ret.begin(), vec_ret.capacity());
		bound_ret = new portbuf_bound_t(*ring_ret);

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
		notify_buffer_count();
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
		notify_buffer_count();
	}
	delete bound_send;
	delete ring_send;
}

const char *port::get_name() const
{
	return "port";
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

bool port::is_shutting_read() const
{
	return shutting_read;
}

bool port::is_shutting_write() const
{
	return shutting_write;
}

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

void port::wait_no_buffer(OMX_BOOL v) const
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

	if (list_bufs.empty()) {
		set_no_buffer(OMX_TRUE);
	} else {
		set_no_buffer(OMX_FALSE);
	}
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

OMX_BOOL port::get_tunneled() const
{
	return f_tunneled;
}

void port::set_tunneled(OMX_BOOL v)
{
	f_tunneled = v;
}

OMX_HANDLETYPE port::get_tunneled_component() const
{
	return tunneled_comp;
}

void port::set_tunneled_component(OMX_HANDLETYPE v)
{
	tunneled_comp = v;
}

OMX_U32 port::get_tunneled_port() const
{
	return tunneled_port;
}

void port::set_tunneled_port(OMX_U32 v)
{
	tunneled_port = v;
}

OMX_BOOL port::get_tunneled_supplier() const
{
	return f_tunneled_supplier;
}

void port::set_tunneled_supplier(OMX_BOOL v)
{
	f_tunneled_supplier = v;
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
	if (formats.size() <= index) {
		return OMX_ErrorBadParameter;
	}

	formats.erase(formats.begin() + index);

	return OMX_ErrorNone;
}

const port_format *port::get_port_format(size_t index) const
{
	if (formats.size() <= index) {
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
	if (formats.size() <= index) {
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

OMX_ERRORTYPE port::plug_client_request()
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lk_port(mut);

	if (!get_enabled()) {
		errprint("Port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	//IL クライアントからポートへのリクエスト禁止
	bound_send->shutdown(false, true);
	cond.notify_all();

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::unplug_client_request()
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lk_port(mut);

	if (!get_enabled()) {
		errprint("Port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	//IL クライアントからポートへのリクエスト許可
	bound_send->abort_shutdown(false, true);
	cond.notify_all();

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::plug_component_request()
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lk_port(mut);

	if (!get_enabled()) {
		errprint("Port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	//コンポーネントからポートへのリクエスト禁止
	bound_send->shutdown(true, false);
	cond.notify_all();

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::unplug_component_request()
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lk_port(mut);

	if (!get_enabled()) {
		errprint("Port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	//コンポーネントからポートへのリクエスト許可
	bound_send->abort_shutdown(true, false);
	cond.notify_all();

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::return_buffers_force()
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lk_port(mut);
	std::vector<port_buffer> list_held_copy = list_held_bufs;

	if (!get_enabled()) {
		errprint("Port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	//クライアントから受け取ったが返していないバッファを返却する
	//NOTE: flush メソッドとコンポーネントが同時に動作すると、
	//      同一のバッファが 2回返却される可能性があるため、
	//      返却前にコンポーネント側の動作を確実に止める必要があります。
	for (port_buffer pb_held : list_held_copy) {
		remove_held_buffer(&pb_held);
		pb_held.header->nFilledLen = 0;
		bound_ret->write_fully(&pb_held, 1);
	}

	bound_send->clear();
	notify_buffer_count();

	return OMX_ErrorNone;
}

void port::wait_buffer_returned() const
{
	scoped_log_begin;
	std::unique_lock<std::recursive_mutex> lk_port(mut);

	cond.wait(lk_port, [&] { return is_broken() ||
		cnt_send_wr == cnt_recv_rd; });
	error_if_broken(lk_port);
}

OMX_ERRORTYPE port::component_tunnel_request(OMX_HANDLETYPE omx_comp, OMX_U32 index, OMX_TUNNELSETUPTYPE *setup)
{
	scoped_log_begin;
	OMX_PARAM_PORTDEFINITIONTYPE def;
	OMX_ERRORTYPE err;

	//Change to non-tunneled communication
	if (omx_comp == nullptr) {
		set_tunneled(OMX_FALSE);
		set_tunneled_component(nullptr);
		set_tunneled_port(0);
		set_tunneled_supplier(OMX_FALSE);

		return OMX_ErrorNone;
	}

	//Change to tunneled communication
	def.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	def.nVersion.s.nVersionMajor = OMX_MF_IL_MAJOR;
	def.nVersion.s.nVersionMinor = OMX_MF_IL_MINOR;
	def.nVersion.s.nRevision     = OMX_MF_IL_REVISION;
	def.nVersion.s.nStep         = OMX_MF_IL_STEP;
	def.nPortIndex               = index;
	err = OMX_GetParameter(omx_comp, OMX_IndexParamPortDefinition, &def);
	if (err != OMX_ErrorNone) {
		errprint("Cannot get port definition from component:%p port:%d.\n",
			omx_comp, (int)index);
		return err;
	}

	//Check domain
	if (get_domain() != def.eDomain) {
		errprint("Wrong domain '%s' (component:%p, port:%d is '%s').\n",
			omx_enum_name::get_OMX_PORTDOMAINTYPE_name(get_domain()),
			omx_comp, (int)index,
			omx_enum_name::get_OMX_PORTDOMAINTYPE_name(def.eDomain));
		return OMX_ErrorPortsNotCompatible;
	}

	//Check format
	port_format f_cond = port_format(def);
	const port_format *f_found = get_port_format(f_cond);
	if (f_found == nullptr) {
		errprint("Not supported format (component:%p, port:%d).\n",
			omx_comp, (int)index);
		return OMX_ErrorPortsNotCompatible;
	}
	//f_cond.dump(" cond");
	//f_found->dump("found");

	//OMX_SetupTunnel() API は Output -> Input の順で、
	//ComponentTunnelRequest を呼び出す
	switch (get_dir()) {
	case OMX_DirInput:
		err = component_tunnel_request_input(omx_comp, index, setup, &def);
		break;
	case OMX_DirOutput:
		err = component_tunnel_request_output(omx_comp, index, setup, &def);
		break;
	default:
		errprint("Unknown direction.\n");
		err = OMX_ErrorPortsNotCompatible;
	}
	if (err != OMX_ErrorNone) {
		errprint("Cannot tunnel request (component:%p, port:%d, dir:%d(%s)).\n",
			omx_comp, (int)index, get_dir(),
			omx_enum_name::get_OMX_DIRTYPE_name(get_dir()));
		return OMX_ErrorPortsNotCompatible;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::component_tunnel_request_input(OMX_HANDLETYPE omx_comp, OMX_U32 index, OMX_TUNNELSETUPTYPE *setup, OMX_PARAM_PORTDEFINITIONTYPE *def)
{
	scoped_log_begin;
	OMX_PARAM_BUFFERSUPPLIERTYPE sup;
	OMX_ERRORTYPE err;

	sup.nSize                    = sizeof(sup);
	sup.nVersion.s.nVersionMajor = OMX_MF_IL_MAJOR;
	sup.nVersion.s.nVersionMinor = OMX_MF_IL_MINOR;
	sup.nVersion.s.nRevision     = OMX_MF_IL_REVISION;
	sup.nVersion.s.nStep         = OMX_MF_IL_STEP;
	sup.nPortIndex               = index;
	err = OMX_GetParameter(omx_comp, OMX_IndexParamCompBufferSupplier, &sup);
	if (err != OMX_ErrorNone) {
		errprint("Failed to get CompBufferSupplier param. (comp:%p, port:%d).\n",
			omx_comp, (int)index);
		return err;
	}

	//Setup tunneled mode
	if (setup->nTunnelFlags & OMX_PORTTUNNELFLAG_READONLY) {
		//FIXME: not implemented
		return OMX_ErrorNotImplemented;
	}

	set_tunneled(OMX_TRUE);
	set_tunneled_component(omx_comp);
	set_tunneled_port(index);

	//TODO: SupplyInput, SupplyOutput どちらにすべきかわからない
	//規格には特に言及無し、後で変えられるからどちらでも良い？
	//とりあえず SupplyInput にしておく
	if (1) {
		set_tunneled_supplier(OMX_TRUE);
		sup.eBufferSupplier = OMX_BufferSupplyInput;
	} else {
		set_tunneled_supplier(OMX_FALSE);
		sup.eBufferSupplier = OMX_BufferSupplyOutput;
	}

	//Negotiation
	err = OMX_SetParameter(omx_comp, OMX_IndexParamCompBufferSupplier, &sup);
	if (err != OMX_ErrorNone) {
		errprint("Failed to negotiation. (comp:%p, port:%d).\n",
			omx_comp, (int)index);
		return err;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::component_tunnel_request_output(OMX_HANDLETYPE omx_comp, OMX_U32 index, OMX_TUNNELSETUPTYPE *setup, OMX_PARAM_PORTDEFINITIONTYPE *def)
{
	scoped_log_begin;

	//Out 側を supplier としてネゴシエーションを始める
	//もし In 側にとって都合が悪ければ、
	//In 側の ComponentTunnelRequest が訂正するはず
	set_tunneled(OMX_TRUE);
	set_tunneled_component(omx_comp);
	set_tunneled_port(index);
	set_tunneled_supplier(OMX_TRUE);

	//現在 nTunnelFlags に設定すべき値はない
	//setup->nTunnelFlags = ;
	setup->eSupplier    = OMX_BufferSupplyOutput;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::allocate_tunnel_buffers()
{
	scoped_log_begin;
	OMX_PARAM_PORTDEFINITIONTYPE def;
	OMX_U32 buffercount, buffersize;
	OMX_U8 *backbuf = nullptr;
	port_buffer *pb = nullptr;
	OMX_BUFFERHEADERTYPE *header = nullptr;
	OMX_U32 i;
	OMX_ERRORTYPE err, result;

	if (!get_enabled()) {
		errprint("Port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	if (!get_tunneled() || !get_tunneled_supplier()) {
		errprint("Not tunneled nor supplier. (comp:%p, port:%d).\n",
			get_component(), (int)get_port_index());
		return OMX_ErrorBadPortIndex;
	}

	//Determine count and size of buffers
	def.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	def.nVersion.s.nVersionMajor = OMX_MF_IL_MAJOR;
	def.nVersion.s.nVersionMinor = OMX_MF_IL_MINOR;
	def.nVersion.s.nRevision     = OMX_MF_IL_REVISION;
	def.nVersion.s.nStep         = OMX_MF_IL_STEP;
	def.nPortIndex               = get_tunneled_port();
	err = OMX_GetParameter(get_tunneled_component(), OMX_IndexParamPortDefinition, &def);
	if (err != OMX_ErrorNone) {
		errprint("Cannot get port definition from component:%p port:%d.\n",
			get_tunneled_component(), (int)get_tunneled_port());
		return err;
	}

	buffercount = std::max(def.nBufferCountActual, get_buffer_count_actual());
	buffersize  = std::max(def.nBufferSize, get_buffer_size());

	//Wait for accept OMX_UseBuffer() calls
	//FIXME: not implemented

	for (i = 0; i < buffercount; i++) {
		//Allocate
		backbuf = nullptr;
		pb = nullptr;
		header = nullptr;

		try {
			//allocate back buffer of OpenMAX buffer
			backbuf = new OMX_U8[buffersize];

			//allocate buffer of port
			pb = new port_buffer();
		} catch (const std::bad_alloc& e) {
			errprint("Failed to allocate '%s'.\n", e.what());
			err = OMX_ErrorInsufficientResources;
			goto err_out;
		}

		//Use
		err = OMX_UseBuffer(get_tunneled_component(), &header,
			get_tunneled_port(), pb, buffersize, backbuf);
		if (err != OMX_ErrorNone) {
			errprint("Failed to OMX_UseBuffer (component:%p, port:%d).\n",
				get_tunneled_component(), (int)get_tunneled_port());
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

			//Set information for myself
			switch (get_dir()) {
			case OMX_DirInput:
				header->pInputPortPrivate = nullptr;
				header->nInputPortIndex   = get_port_index();
				break;
			case OMX_DirOutput:
				header->pOutputPortPrivate = nullptr;
				header->nOutputPortIndex   = get_port_index();
				break;
			default:
				errprint("Unknown direction.\n");
				err = OMX_ErrorPortsNotCompatible;
				goto err_out;
			}

			list_bufs.push_back(pb);
			update_buffer_status();
		}
	}

	//success
	err = OMX_ErrorNone;

	return err;

err_out:
	result = OMX_FreeBuffer(get_tunneled_component(), get_tunneled_port(), header);
	if (result != OMX_ErrorNone) {
		errprint("Failed to OMX_FreeBuffer (bufheader:%p, component:%p, port:%d).\n",
			header, get_tunneled_component(), (int)get_tunneled_port());
		//Ignore error
	}

	delete[] backbuf;
	delete pb;
	free_tunnel_buffers();

	return err;
}

OMX_ERRORTYPE port::free_tunnel_buffers()
{
	scoped_log_begin;
	OMX_BUFFERHEADERTYPE *header;
	OMX_ERRORTYPE err;

	if (!get_enabled()) {
		errprint("Port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	if (!get_tunneled() || !get_tunneled_supplier()) {
		errprint("Not tunneled nor supplier. (comp:%p, port:%d).\n",
			get_component(), (int)get_port_index());
		return OMX_ErrorBadPortIndex;
	}

	for (port_buffer *pb : list_bufs) {
		header = pb->header;

		err = OMX_FreeBuffer(get_tunneled_component(), get_tunneled_port(), header);
		if (err != OMX_ErrorNone) {
			errprint("Failed to OMX_FreeBuffer (bufheader:%p, component:%p, port:%d).\n",
				header, get_tunneled_component(), (int)get_tunneled_port());
			//Ignore error
		}

		delete pb;
		delete[] header->pBuffer;
	}

	list_bufs.clear();
	update_buffer_status();

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::use_buffer(OMX_BUFFERHEADERTYPE **bufhead, OMX_PTR priv, OMX_U32 size, OMX_U8 *buf)
{
	scoped_log_begin;
	port_buffer *pb = nullptr;
	OMX_BUFFERHEADERTYPE *header = nullptr;
	OMX_ERRORTYPE err;

	if (bufhead == nullptr || buf == nullptr) {
		errprint("Invalid bufferheader:%p or buffer:%p\n", bufhead, buf);
		return OMX_ErrorBadParameter;
	}

	if (!get_enabled()) {
		errprint("Port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	if (size < get_buffer_size()) {
		errprint("Invalid buffer size (buffer size:%d < minimum size:%d).\n",
			(int)size, (int)get_buffer_size());
		return OMX_ErrorBadPortIndex;
	}

	try {
		//allocate buffer of port
		pb = new port_buffer();

		//allocate OpenMAX BUFFERHEADER
		header = new OMX_BUFFERHEADERTYPE();
	} catch (const std::bad_alloc& e) {
		errprint("Failed to allocate '%s'.\n", e.what());
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
		switch (get_dir()) {
		case OMX_DirInput:
			header->pInputPortPrivate  = nullptr;
			header->nInputPortIndex    = get_port_index();
			break;
		case OMX_DirOutput:
			header->pOutputPortPrivate = nullptr;
			header->nOutputPortIndex   = get_port_index();
			break;
		default:
			errprint("Unknown direction.\n");
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

	if (bufhead == nullptr) {
		errprint("Invalid bufferheader:%p\n", bufhead);
		return OMX_ErrorBadParameter;
	}

	if (!get_enabled()) {
		errprint("Port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	if (size < get_buffer_size()) {
		errprint("Invalid buffer size (allocated size:%d < minimum size:%d).\n",
			(int)size, (int)get_buffer_size());
		return OMX_ErrorBadPortIndex;
	}

	try {
		//allocate back buffer of OpenMAX buffer
		backbuf = new OMX_U8[size];

		//allocate buffer of port
		pb = new port_buffer();

		//allocate OpenMAX BUFFERHEADER
		header = new OMX_BUFFERHEADERTYPE();
	} catch (const std::bad_alloc& e) {
		errprint("Failed to allocate '%s'.\n", e.what());
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
		switch (get_dir()) {
		case OMX_DirInput:
			header->pInputPortPrivate  = nullptr;
			header->nInputPortIndex    = get_port_index();
			break;
		case OMX_DirOutput:
			header->pOutputPortPrivate = nullptr;
			header->nOutputPortIndex   = get_port_index();
			break;
		default:
			errprint("Unknown direction.\n");
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

	if (bufhead == nullptr) {
		//Do nothing
		return OMX_ErrorNone;
	}

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

bool port::find_buffer(const OMX_BUFFERHEADERTYPE *bufhead) const
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

bool port::find_buffer(const port_buffer *pb) const
{
	return find_buffer(pb->header);
}

OMX_ERRORTYPE port::add_held_buffer(const port_buffer *pb)
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lk_buf(mut_list_bufs);

	list_held_bufs.push_back(*pb);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE port::remove_held_buffer(const port_buffer *pb)
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lk_buf(mut_list_bufs);

	for (auto it = list_held_bufs.begin(); it != list_held_bufs.end(); it++) {
		if (it->header->pBuffer == pb->header->pBuffer) {
			//found
			list_held_bufs.erase(it);
			return OMX_ErrorNone;
		}
	}

	//not found
	errprint("port_buffer:%p (buffer:%p) is not found.\n",
		pb, pb->header->pBuffer);

	return OMX_ErrorBadParameter;
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
	scoped_log_begin;
	port_buffer pb;
	OMX_ERRORTYPE err;

	if (!get_enabled()) {
		errprint("Port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}
	if (is_shutting_write()) {
		errprint("Port %d is flushing.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}
	if (!find_buffer(bufhead)) {
		dprint("Buffer header:%p is not registered.\n", bufhead);
	}

	pb.p          = this;
	pb.f_allocate = false;
	pb.header     = bufhead;
	pb.index      = bufhead->nOffset;

	add_held_buffer(&pb);

	try {
		bound_send->write_fully(&pb, 1);
		notify_buffer_count();

		err = OMX_ErrorNone;
	} catch (const std::runtime_error& e) {
		errprint("runtime_error: %s\n", e.what());

		remove_held_buffer(&pb);

		err = OMX_ErrorInsufficientResources;
	}

	return err;
}

OMX_ERRORTYPE port::pop_buffer(port_buffer *pb)
{
	scoped_log_begin;
	OMX_ERRORTYPE err;

	try {
		bound_send->read_fully(pb, 1);
		notify_buffer_count();

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
	scoped_log_begin;
	std::unique_lock<std::recursive_mutex> lk_port(mut);
	port_buffer pb;
	OMX_ERRORTYPE err;

	if (!get_enabled()) {
		errprint("Port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	pb.p = this;
	pb.f_allocate = false;
	pb.header = bufhead;

	remove_held_buffer(&pb);

	try {
		bound_ret->write_fully(&pb, 1);
		notify_buffer_count();

		err = OMX_ErrorNone;
	} catch (const std::runtime_error& e) {
		errprint("runtime_error: %s\n", e.what());

		add_held_buffer(&pb);

		err = OMX_ErrorInsufficientResources;
	}

	return err;
}

OMX_ERRORTYPE port::start_tunneling()
{
	scoped_log_begin;
	OMX_ERRORTYPE err, errtmp;

	if (!get_enabled()) {
		errprint("Port %d is disabled.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}
	if (!get_tunneled() || !get_tunneled_supplier()) {
		errprint("Port %d is not tunneled or supplier.\n",
			(int)get_port_index());
		return OMX_ErrorIncorrectStateOperation;
	}

	err = OMX_ErrorNone;
	for (port_buffer *pb : list_bufs) {
		switch (get_dir()) {
		case OMX_DirInput:
			errtmp = get_component()->EmptyThisBuffer(nullptr, pb->header);
			if (errtmp != OMX_ErrorNone) {
				errprint("Failed to EmptyThisBuffer() in comp:%p, pb:%p, buffer:%p.\n",
					get_component(), pb, pb->header);
			}
			break;
		case OMX_DirOutput:
			errtmp = get_component()->FillThisBuffer(nullptr, pb->header);
			if (errtmp != OMX_ErrorNone) {
				errprint("Failed to FillThisBuffer() in comp:%p, pb:%p, buffer:%p.\n",
					get_component(), pb, pb->header);
			}
			break;
		default:
			errprint("Unknown direction.\n");
			errtmp = OMX_ErrorBadPortIndex;
		}
		if (errtmp != OMX_ErrorNone) {
			err = errtmp;
		}
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

void port::error_if_broken(std::unique_lock<std::recursive_mutex>& lk_port) const
{
	if (is_broken()) {
		std::string msg(__func__);
		msg += ": interrupted by shutdown.";
		throw std::runtime_error(msg);
	}
}

bool port::is_broken() const
{
	return is_shutting_read() && is_shutting_write();
}

void port::notify_buffer_count()
{
	scoped_log_begin;
	std::lock_guard<std::recursive_mutex> lk_port(mut);

	cnt_send_wr = bound_send->get_write_count();
	cnt_recv_rd = bound_ret->get_read_count();

	cond.notify_all();
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
		notify_buffer_count();

		comp = pb.p->get_component();

		err = OMX_ErrorNone;
		err_handler = OMX_ErrorNone;
		f_callback = true;

		switch (pb.p->get_dir()) {
		case OMX_DirInput:
			if (pb.p->get_tunneled()) {
				err = OMX_FillThisBuffer(pb.p->get_tunneled_component(), pb.header);
			} else {
				err = comp->EmptyBufferDone(&pb);
			}
			break;
		case OMX_DirOutput:
			if (pb.p->get_tunneled()) {
				err = OMX_EmptyThisBuffer(pb.p->get_tunneled_component(), pb.header);
			} else {
				err = comp->FillBufferDone(&pb);
			}
			break;
		default:
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
		{
			//std::to_string is not supported on Android KitKat
			//so using std::stringstream instead.
			std::stringstream ss;

			thname = "omx:p";
			ss << p->get_port_index();
			thname += ss.str();
			thname += ":";
			thname += p->get_component()->get_name();
		}
		set_thread_name(thname.c_str());

		p->buffer_done();
	} catch (const std::runtime_error& e) {
		errprint("runtime_error: %s\n", e.what());
	}

	return nullptr;
}

} //namespace mf
