#include <cstdarg>
#include <cstdint>
#include <deque>
#include <map>
#include <mutex>
#include <condition_variable>

#include <OMX_Component.h>
#include <OMX_Core.h>

#include <omxil_mf/component_worker.hpp>
#include <omxil_mf/component.hpp>
#include <omxil_mf/scoped_log.hpp>

#include "api/consts.hpp"
#include "util/util.hpp"
#include "util/omx_enum_name.hpp"

namespace mf {

component_worker::component_worker(component *c)
	: comp(c), th_work(nullptr),
	f_running(false), f_broken(false),
	f_request_flush(false), f_flush_done(false),
	f_request_restart(false), f_restart_done(false)
{
	scoped_log_begin;
	//Do nothing
}

component_worker::~component_worker()
{
	scoped_log_begin;

	set_broken(true);
	join();
}

const char *component_worker::get_name() const
{
	return "worker";
}

std::mutex& component_worker::mutex() const
{
	return mut;
}

std::condition_variable& component_worker::condition() const
{
	return cond;
}

const component *component_worker::get_component() const
{
	return comp;
}

component *component_worker::get_component()
{
	return comp;
}

bool component_worker::is_running() const
{
	return f_running;
}

void component_worker::set_running(bool f)
{
	scoped_log_begin;
	std::lock_guard<std::mutex> lock(mut);

	f_running = f;
	cond.notify_all();
}

bool component_worker::is_broken() const
{
	return f_broken;
}

void component_worker::set_broken(bool f)
{
	scoped_log_begin;
	std::lock_guard<std::mutex> lock(mut);

	f_broken = f;
	cond.notify_all();
}

bool component_worker::is_request_flush() const
{
	return f_request_flush;
}

void component_worker::set_request_flush(bool f)
{
	scoped_log_begin;
	std::lock_guard<std::mutex> lock(mut);

	f_request_flush = f;
	cond.notify_all();
}

bool component_worker::is_flush_done() const
{
	return f_flush_done;
}

void component_worker::set_flush_done(bool f)
{
	scoped_log_begin;
	std::lock_guard<std::mutex> lock(mut);

	f_flush_done = f;
	cond.notify_all();
}

bool component_worker::is_request_restart() const
{
	return f_request_restart;
}

void component_worker::set_request_restart(bool f)
{
	scoped_log_begin;
	std::lock_guard<std::mutex> lock(mut);

	f_request_restart = f;
	cond.notify_all();
}

bool component_worker::is_restart_done() const
{
	return f_restart_done;
}

void component_worker::set_restart_done(bool f)
{
	scoped_log_begin;
	std::lock_guard<std::mutex> lock(mut);

	f_restart_done = f;
	cond.notify_all();
}

void component_worker::wait_request_flush()
{
	scoped_log_begin;
	std::unique_lock<std::mutex> lock(mut);

	cond.wait(lock, [&]() {
			return is_broken() || !is_running() || is_request_flush();
		});
	f_request_flush = false;
	error_if_broken(lock);
}

void component_worker::wait_flush_done()
{
	scoped_log_begin;
	std::unique_lock<std::mutex> lock(mut);

	cond.wait(lock, [&]() {
			return is_broken() || !is_running() || is_flush_done();
		});
	f_flush_done = false;
	error_if_broken(lock);
}

void component_worker::wait_request_restart()
{
	scoped_log_begin;
	std::unique_lock<std::mutex> lock(mut);

	cond.wait(lock, [&]() {
			return is_broken() || !is_running() || is_request_restart();
		});
	f_request_restart = false;
	error_if_broken(lock);
}

void component_worker::wait_restart_done()
{
	scoped_log_begin;
	std::unique_lock<std::mutex> lock(mut);

	cond.wait(lock, [&]() {
			return is_broken() || !is_running() || is_restart_done();
		});
	f_restart_done = false;
	error_if_broken(lock);
}

void component_worker::start()
{
	th_work = new std::thread(component_worker_thread_main, this);
}

void component_worker::join()
{
	if (th_work) {
		th_work->join();
	}
	delete th_work;
	th_work = nullptr;
}

void component_worker::run()
{
	scoped_log_begin;
	//Do nothing
}


/*
 * protected member functions
 */

void component_worker::error_if_broken(std::unique_lock<std::mutex>& lock) const
{
	if (is_broken()) {
		std::string msg(__func__);
		msg += ": ";
		msg += get_name();
		msg += ": interrupted.";
		throw std::runtime_error(msg);
	}
}


/*
 * static protected functions
 */

void *component_worker::component_worker_thread_main(component_worker *arg)
{
	std::string thname;

	//スレッド名をつける
	thname = "omx:wrk:";
	thname += arg->get_name();
	set_thread_name(thname.c_str());

	try {
		//メイン処理が始まったことを通知する
		arg->set_running(true);
		arg->set_request_restart(true);

		while (arg->is_running()) {
			arg->wait_request_restart();
			if (!arg->is_running()) {
				break;
			}
			arg->set_restart_done(true);

			try {
				arg->run();
			} catch (const std::runtime_error& e) {
				errprint("runtime_error: worker %s: %s\n",
					arg->get_name(), e.what());
			}
			arg->set_request_flush(false);
			arg->set_flush_done(true);
		}
	} catch (const std::runtime_error& e) {
		errprint("runtime_error: worker %s: %s\n",
			arg->get_name(), e.what());
	}

	return nullptr;
}

} //namespace mf
