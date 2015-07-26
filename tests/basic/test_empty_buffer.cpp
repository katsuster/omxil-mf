
#include <cstdio>
#include <cstring>
#include <vector>
#include <mutex>
#include <condition_variable>

#include <unistd.h>

#include <OMX_Core.h>
#include <OMX_Component.h>

#include "common/test_omxil.h"
#include "common/omxil_utils.h"
#include "common/omxil_comp.hpp"

struct buffer_attr {
	bool used;
};

class comp_test_empty_buffer : public omxil_comp {
public:
	comp_test_empty_buffer(const char *comp_name)
		: omxil_comp(comp_name), state_done(OMX_StateInvalid)
	{
		//do nothing
	}

	virtual ~comp_test_empty_buffer()
	{
		//do nothing
	}

	virtual void push_back_buffer(OMX_BUFFERHEADERTYPE *buf)
	{
		std::unique_lock<std::mutex> lock(mut_command);

		buf_in.push_back(buf);
	}

	virtual OMX_BUFFERHEADERTYPE *find_free_buffer() const
	{
		std::unique_lock<std::mutex> lock(mut_command);

		for (OMX_BUFFERHEADERTYPE *buf : buf_in) {
			buffer_attr *pbattr = static_cast<buffer_attr *>(buf->pAppPrivate);

			if (!pbattr->used) {
				return buf;
			}
		}

		return nullptr;
	}

	/**
	 * 全てのバッファが利用中かどうかを取得します。
	 *
	 * N 個のバッファがあったとき、
         *
	 * 利用中の
	 * バッファ数  | is_used_all | is_free_all |
	 * ------------+-------------+-------------+
	 * 0           | false       | true        |
	 * 1 ... N - 1 | false       | false       |
	 * N           | true        | false       |
	 * ------------+-------------+-------------+
	 *
	 * @return 全てのバッファが利用中ならば true、
	 * 1つでもバッファが利用可能ならば false
	 */
	virtual bool is_used_all_buffer() const
	{
		std::unique_lock<std::mutex> lock(mut_command);

		for (OMX_BUFFERHEADERTYPE *buf : buf_in) {
			buffer_attr *pbattr = static_cast<buffer_attr *>(buf->pAppPrivate);

			if (!pbattr->used) {
				return false;
			}
		}

		return true;
	}

	/**
	 * 全てのバッファが利用可能かどうかを取得します。
	 *
	 * N 個のバッファがあったとき、
         *
	 * 利用中の
	 * バッファ数  | is_used_all | is_free_all |
	 * ------------+-------------+-------------+
	 * 0           | false       | true        |
	 * 1 ... N - 1 | false       | false       |
	 * N           | true        | false       |
	 * ------------+-------------+-------------+
	 *
	 * @return 全てのバッファが利用可能ならば true、
	 * 1つでもバッファが利用中ならば false
	 */
	virtual bool is_free_all_buffer() const
	{
		std::unique_lock<std::mutex> lock(mut_command);

		for (OMX_BUFFERHEADERTYPE *buf : buf_in) {
			buffer_attr *pbattr = static_cast<buffer_attr *>(buf->pAppPrivate);

			if (pbattr->used) {
				return false;
			}
		}

		return true;
	}

	virtual OMX_ERRORTYPE EventHandler(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
	{
		if (eEvent == OMX_EventCmdComplete &&
			nData1 == OMX_CommandStateSet) {
			std::unique_lock<std::mutex> lock(mut_command);

			printf("comp_test_empty_buffer::EventHandler state '%s' set.\n",
				get_omx_statetype_name((OMX_STATETYPE)nData2));
			state_done = static_cast<OMX_STATETYPE>(nData2);
			cond_command.notify_all();
		} else {
			printf("comp_test_empty_buffer::EventHandler ignored.\n");
		}

		return OMX_ErrorNone;
	}

	virtual OMX_ERRORTYPE EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
	{
		std::unique_lock<std::mutex> lock(mut_command);
		buffer_attr *pbattr;

		printf("comp_test_empty_buffer::EmptyBufferDone.\n");

		pbattr = static_cast<buffer_attr *>(pBuffer->pAppPrivate);

		pbattr->used = false;
		cond_command.notify_all();

		return OMX_ErrorNone;
	}

	/**
	 * コンポーネントが指定した状態に変化するまで待ちます。
	 *
	 * @param s コンポーネントの状態
	 */
	virtual void wait_state_changed(OMX_STATETYPE s) const
	{
		std::unique_lock<std::mutex> lock(mut_command);

		cond_command.wait(lock, [&] { return state_done == s; });
	}

	/**
	 * 1つ以上のバッファが利用可能になるまで待ちます。
	 */
	virtual void wait_buffer_free() const
	{
		std::unique_lock<std::mutex> lock(mut_command);

		cond_command.wait(lock, [&] { return !is_used_all_buffer(); });
	}

	/**
	 * 全てのバッファが利用可能になるまで待ちます。
	 */
	virtual void wait_all_buffer_free() const
	{
		std::unique_lock<std::mutex> lock(mut_command);

		cond_command.wait(lock, [&] { return is_free_all_buffer(); });
	}

private:
	mutable std::mutex mut_command;
	mutable std::condition_variable cond_command;
	OMX_STATETYPE state_done;
	std::vector<OMX_BUFFERHEADERTYPE *>buf_in;

};

int main(int argc, char *argv[])
{
	const char *arg_comp;
	comp_test_empty_buffer *comp;
	OMX_PARAM_PORTDEFINITIONTYPE def_in;
	std::vector<OMX_BUFFERHEADERTYPE *> buf_in;
	OMX_ERRORTYPE result;
	OMX_U32 i;

	//get arguments
	if (argc < 2) {
		arg_comp = "OMX.st.video_decoder.avc";
	} else {
		arg_comp = argv[1];
	}

	//Reference:
	//    OpenMAX IL specification version 1.1.2
	//    3.4.2.1 Non-tunneled Data Flow
	//    3.2.2.17 OMX_EmptyThisBuffer

	comp = nullptr;
	result = OMX_ErrorNone;

	result = OMX_Init();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Init failed.\n");
		goto err_out1;
	}

	comp = new comp_test_empty_buffer(arg_comp);
	if (comp == nullptr || comp->get_component() == nullptr) {
		fprintf(stderr, "OMX_GetHandle failed.\n");
		result = OMX_ErrorInsufficientResources;
		goto err_out2;
	}
	printf("OMX_GetHandle: name:%s, comp:%p\n",
		arg_comp, comp);

	//Get port definition(before)
	result = comp->get_param_port_definition(0, &def_in);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "get_port_definition(before) failed.\n");
		goto err_out2;
	}
	printf("IndexParamPortDefinition: before in %d -----\n", (int)def_in.nPortIndex);
	dump_port_definitiontype(&def_in);

	//Set StateIdle
	result = comp->SendCommand(OMX_CommandStateSet, OMX_StateIdle, 0);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SendCommand(StateSet, Idle) failed.\n");
		goto err_out2;
	}

	buf_in.clear();
	for (i = 0; i < def_in.nBufferCountActual; i++) {
		OMX_BUFFERHEADERTYPE *buf;
		OMX_U8 *pb = nullptr;
		buffer_attr *pbattr = nullptr;

		pb = new OMX_U8[def_in.nBufferSize];
		pbattr = new buffer_attr;

		result = comp->UseBuffer(&buf,
			0, pbattr, def_in.nBufferSize, pb);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_AllocateBuffer(in) failed.\n");
			goto err_out2;
		}
		printf("OMX_UseBuffer: in \n");
		dump_port_bufferheadertype(buf);

		comp->push_back_buffer(buf);
		buf_in.push_back(buf);
	}

	//Wait for StatusIdle
	printf("wait for StateIdle...\n");
	comp->wait_state_changed(OMX_StateIdle);
	printf("wait for StateIdle... Done!\n");


	//Set StateExecuting
	result = comp->SendCommand(OMX_CommandStateSet, OMX_StateExecuting, 0);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SendCommand(StateSet, Executing) failed.\n");
		goto err_out2;
	}

	//Wait for StatusExecuting
	printf("wait for StateExecuting...\n");
	comp->wait_state_changed(OMX_StateExecuting);
	printf("wait for StateExecuting... Done!\n");


	//EmptyThisBuffer
	for (i = 0; i < 100; i++) {
		OMX_BUFFERHEADERTYPE *buf;
		buffer_attr *pbattr;

		comp->wait_buffer_free();

		buf = comp->find_free_buffer();
		if (buf == nullptr) {
			fprintf(stderr, "find_free_buffer() failed.\n");
			goto err_out2;
		}

		pbattr = static_cast<buffer_attr *>(buf->pAppPrivate);
		pbattr->used = true;

		result = comp->EmptyThisBuffer(buf);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "EmptyThisBuffer() failed.\n");
			goto err_out2;
		}
	}


	//Set StateIdle
	result = comp->SendCommand(OMX_CommandStateSet, OMX_StateIdle, 0);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SendCommand(StateSet, Idle) failed.\n");
		goto err_out2;
	}

	//Wait for StatusIdle
	printf("wait for StateIdle...\n");
	comp->wait_state_changed(OMX_StateIdle);
	printf("wait for StateIdle... Done!\n");


	//Set StateLoaded
	result = comp->SendCommand(OMX_CommandStateSet, OMX_StateLoaded, 0);
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_SendCommand(StateSet, Loaded) failed.\n");
		goto err_out2;
	}

	printf("wait for EmptyDone of all buffers...\n");
	comp->wait_all_buffer_free();
	printf("wait for EmptyDone of all buffers... Done!\n");

	//Free buffer
	for (auto it = buf_in.begin(); it != buf_in.end(); it++) {
		OMX_U8 *pb = (*it)->pBuffer;
		buffer_attr *pbattr = static_cast<buffer_attr *>((*it)->pAppPrivate);

		result = comp->FreeBuffer(0, *it);
		if (result != OMX_ErrorNone) {
			fprintf(stderr, "OMX_FreeBuffer(in) failed.\n");
			goto err_out2;
		}

		delete pbattr;
		delete[] pb;
	}
	buf_in.clear();

	//Wait for StatusLoaded
	printf("wait for StateLoaded...\n");
	comp->wait_state_changed(OMX_StateLoaded);
	printf("wait for StateLoaded... Done!\n");


	//Terminate
	delete comp;

	result = OMX_Deinit();
	if (result != OMX_ErrorNone) {
		fprintf(stderr, "OMX_Deinit failed.\n");
		goto err_out1;
	}

	return 0;

err_out2:
	for (auto it = buf_in.begin(); it != buf_in.end(); it++) {
		OMX_U8 *pb = (*it)->pBuffer;
		buffer_attr *pbattr = static_cast<buffer_attr *>((*it)->pAppPrivate);

		comp->FreeBuffer(0, *it);

		delete pbattr;
		delete[] pb;
	}

	delete comp;

	OMX_Deinit();

err_out1:
	return -1;
}
