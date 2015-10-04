
#include "util/omx_enum_name.hpp"

namespace mf {

const char *omx_enum_name::get_OMX_BOOL_name(OMX_BOOL id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_FALSE:
		name = "OMX_FALSE";
		break;
	case OMX_TRUE:
		name = "OMX_TRUE";
		break;
	case OMX_BOOL_MAX:
		name = "OMX_BOOL_MAX";
		break;
	default:
		name = "(unknown)";
		break;
	}

	return name;
}

const char *omx_enum_name::get_OMX_DIRTYPE_name(OMX_DIRTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_DirInput:
		name = "OMX_DirInput";
		break;
	case OMX_DirOutput:
		name = "OMX_DirOutput";
		break;
	case OMX_DirMax:
		name = "OMX_DirMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	return name;
}

const char *omx_enum_name::get_OMX_ENDIANTYPE_name(OMX_ENDIANTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_EndianBig:
		name = "OMX_EndianBig";
		break;
	case OMX_EndianLittle:
		name = "OMX_EndianLittle";
		break;
	case OMX_EndianMax:
		name = "OMX_EndianMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	return name;
}

const char *omx_enum_name::get_OMX_NUMERICALDATATYPE_name(OMX_NUMERICALDATATYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_NumericalDataSigned:
		name = "OMX_NumericalDataSigned";
		break;
	case OMX_NumericalDataUnsigned:
		name = "OMX_NumericalDataUnsigned";
		break;
	case OMX_NumercialDataMax:
		name = "OMX_NumercialDataMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	return name;
}

} //namespace mf
