
#include "util/omx_enum_name.hpp"

namespace mf {

const char *omx_enum_name::get_OMX_COMMANDTYPE_name(OMX_COMMANDTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_CommandStateSet:
		name = "OMX_CommandStateSet";
		break;
	case OMX_CommandFlush:
		name = "OMX_CommandFlush";
		break;
	case OMX_CommandPortDisable:
		name = "OMX_CommandPortDisable";
		break;
	case OMX_CommandPortEnable:
		name = "OMX_CommandPortEnable";
		break;
	case OMX_CommandMarkBuffer:
		name = "OMX_CommandMarkBuffer";
		break;
	case OMX_CommandMax:
		name = "OMX_CommandMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_CommandKhronosExtensions <= id && 
		id < OMX_CommandVendorStartUnused) {
		name = "(OMX_CommandKhronosExtensions)";
	}
	if (OMX_CommandVendorStartUnused <= id && 
		id < OMX_CommandMax) {
		name = "(OMX_CommandVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_STATETYPE_name(OMX_STATETYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_StateInvalid:
		name = "OMX_StateInvalid";
		break;
	case OMX_StateLoaded:
		name = "OMX_StateLoaded";
		break;
	case OMX_StateIdle:
		name = "OMX_StateIdle";
		break;
	case OMX_StateExecuting:
		name = "OMX_StateExecuting";
		break;
	case OMX_StatePause:
		name = "OMX_StatePause";
		break;
	case OMX_StateWaitForResources:
		name = "OMX_StateWaitForResources";
		break;
	case OMX_StateMax:
		name = "OMX_StateMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_StateKhronosExtensions <= id && 
		id < OMX_StateVendorStartUnused) {
		name = "(OMX_StateKhronosExtensions)";
	}
	if (OMX_StateVendorStartUnused <= id && 
		id < OMX_StateMax) {
		name = "(OMX_StateVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_ERRORTYPE_name(OMX_ERRORTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_ErrorNone:
		name = "OMX_ErrorNone";
		break;
	case OMX_ErrorInsufficientResources:
		name = "OMX_ErrorInsufficientResources";
		break;
	case OMX_ErrorUndefined:
		name = "OMX_ErrorUndefined";
		break;
	case OMX_ErrorInvalidComponentName:
		name = "OMX_ErrorInvalidComponentName";
		break;
	case OMX_ErrorComponentNotFound:
		name = "OMX_ErrorComponentNotFound";
		break;
	case OMX_ErrorInvalidComponent:
		name = "OMX_ErrorInvalidComponent";
		break;
	case OMX_ErrorBadParameter:
		name = "OMX_ErrorBadParameter";
		break;
	case OMX_ErrorNotImplemented:
		name = "OMX_ErrorNotImplemented";
		break;
	case OMX_ErrorUnderflow:
		name = "OMX_ErrorUnderflow";
		break;
	case OMX_ErrorOverflow:
		name = "OMX_ErrorOverflow";
		break;
	case OMX_ErrorHardware:
		name = "OMX_ErrorHardware";
		break;
	case OMX_ErrorInvalidState:
		name = "OMX_ErrorInvalidState";
		break;
	case OMX_ErrorStreamCorrupt:
		name = "OMX_ErrorStreamCorrupt";
		break;
	case OMX_ErrorPortsNotCompatible:
		name = "OMX_ErrorPortsNotCompatible";
		break;
	case OMX_ErrorResourcesLost:
		name = "OMX_ErrorResourcesLost";
		break;
	case OMX_ErrorNoMore:
		name = "OMX_ErrorNoMore";
		break;
	case OMX_ErrorVersionMismatch:
		name = "OMX_ErrorVersionMismatch";
		break;
	case OMX_ErrorNotReady:
		name = "OMX_ErrorNotReady";
		break;
	case OMX_ErrorTimeout:
		name = "OMX_ErrorTimeout";
		break;
	case OMX_ErrorSameState:
		name = "OMX_ErrorSameState";
		break;
	case OMX_ErrorResourcesPreempted:
		name = "OMX_ErrorResourcesPreempted";
		break;
	case OMX_ErrorPortUnresponsiveDuringAllocation:
		name = "OMX_ErrorPortUnresponsiveDuringAllocation";
		break;
	case OMX_ErrorPortUnresponsiveDuringDeallocation:
		name = "OMX_ErrorPortUnresponsiveDuringDeallocation";
		break;
	case OMX_ErrorPortUnresponsiveDuringStop:
		name = "OMX_ErrorPortUnresponsiveDuringStop";
		break;
	case OMX_ErrorIncorrectStateTransition:
		name = "OMX_ErrorIncorrectStateTransition";
		break;
	case OMX_ErrorIncorrectStateOperation:
		name = "OMX_ErrorIncorrectStateOperation";
		break;
	case OMX_ErrorUnsupportedSetting:
		name = "OMX_ErrorUnsupportedSetting";
		break;
	case OMX_ErrorUnsupportedIndex:
		name = "OMX_ErrorUnsupportedIndex";
		break;
	case OMX_ErrorBadPortIndex:
		name = "OMX_ErrorBadPortIndex";
		break;
	case OMX_ErrorPortUnpopulated:
		name = "OMX_ErrorPortUnpopulated";
		break;
	case OMX_ErrorComponentSuspended:
		name = "OMX_ErrorComponentSuspended";
		break;
	case OMX_ErrorDynamicResourcesUnavailable:
		name = "OMX_ErrorDynamicResourcesUnavailable";
		break;
	case OMX_ErrorMbErrorsInFrame:
		name = "OMX_ErrorMbErrorsInFrame";
		break;
	case OMX_ErrorFormatNotDetected:
		name = "OMX_ErrorFormatNotDetected";
		break;
	case OMX_ErrorContentPipeOpenFailed:
		name = "OMX_ErrorContentPipeOpenFailed";
		break;
	case OMX_ErrorContentPipeCreationFailed:
		name = "OMX_ErrorContentPipeCreationFailed";
		break;
	case OMX_ErrorSeperateTablesUsed:
		name = "OMX_ErrorSeperateTablesUsed";
		break;
	case OMX_ErrorTunnelingUnsupported:
		name = "OMX_ErrorTunnelingUnsupported";
		break;
	case OMX_ErrorMax:
		name = "OMX_ErrorMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_ErrorKhronosExtensions <= id && 
		id < OMX_ErrorVendorStartUnused) {
		name = "(OMX_ErrorKhronosExtensions)";
	}
	if (OMX_ErrorVendorStartUnused <= id && 
		id < OMX_ErrorMax) {
		name = "(OMX_ErrorVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_EXTRADATATYPE_name(OMX_EXTRADATATYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_ExtraDataNone:
		name = "OMX_ExtraDataNone";
		break;
	case OMX_ExtraDataQuantization:
		name = "OMX_ExtraDataQuantization";
		break;
	case OMX_ExtraDataMax:
		name = "OMX_ExtraDataMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_ExtraDataKhronosExtensions <= id && 
		id < OMX_ExtraDataVendorStartUnused) {
		name = "(OMX_ExtraDataKhronosExtensions)";
	}
	if (OMX_ExtraDataVendorStartUnused <= id && 
		id < OMX_ExtraDataMax) {
		name = "(OMX_ExtraDataVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_EVENTTYPE_name(OMX_EVENTTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_EventCmdComplete:
		name = "OMX_EventCmdComplete";
		break;
	case OMX_EventError:
		name = "OMX_EventError";
		break;
	case OMX_EventMark:
		name = "OMX_EventMark";
		break;
	case OMX_EventPortSettingsChanged:
		name = "OMX_EventPortSettingsChanged";
		break;
	case OMX_EventBufferFlag:
		name = "OMX_EventBufferFlag";
		break;
	case OMX_EventResourcesAcquired:
		name = "OMX_EventResourcesAcquired";
		break;
	case OMX_EventComponentResumed:
		name = "OMX_EventComponentResumed";
		break;
	case OMX_EventDynamicResourcesAvailable:
		name = "OMX_EventDynamicResourcesAvailable";
		break;
	case OMX_EventPortFormatDetected:
		name = "OMX_EventPortFormatDetected";
		break;
	case OMX_EventMax:
		name = "OMX_EventMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_EventKhronosExtensions <= id && 
		id < OMX_EventVendorStartUnused) {
		name = "(OMX_EventKhronosExtensions)";
	}
	if (OMX_EventVendorStartUnused <= id && 
		id < OMX_EventMax) {
		name = "(OMX_EventVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_BUFFERSUPPLIERTYPE_name(OMX_BUFFERSUPPLIERTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_BufferSupplyUnspecified:
		name = "OMX_BufferSupplyUnspecified";
		break;
	case OMX_BufferSupplyInput:
		name = "OMX_BufferSupplyInput";
		break;
	case OMX_BufferSupplyOutput:
		name = "OMX_BufferSupplyOutput";
		break;
	case OMX_BufferSupplyMax:
		name = "OMX_BufferSupplyMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_BufferSupplyKhronosExtensions <= id && 
		id < OMX_BufferSupplyVendorStartUnused) {
		name = "(OMX_BufferSupplyKhronosExtensions)";
	}
	if (OMX_BufferSupplyVendorStartUnused <= id && 
		id < OMX_BufferSupplyMax) {
		name = "(OMX_BufferSupplyVendorStartUnused)";
	}

	return name;
}

} //namespace mf
