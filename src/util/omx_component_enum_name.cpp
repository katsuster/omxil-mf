
#include "util/omx_enum_name.hpp"

namespace mf {

const char *omx_enum_name::get_OMX_PORTDOMAINTYPE_name(OMX_PORTDOMAINTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_PortDomainAudio:
		name = "OMX_PortDomainAudio";
		break;
	case OMX_PortDomainVideo:
		name = "OMX_PortDomainVideo";
		break;
	case OMX_PortDomainImage:
		name = "OMX_PortDomainImage";
		break;
	case OMX_PortDomainOther:
		name = "OMX_PortDomainOther";
		break;
	case OMX_PortDomainMax:
		name = "OMX_PortDomainMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_PortDomainKhronosExtensions <= id && 
		id < OMX_PortDomainVendorStartUnused) {
		name = "(OMX_PortDomainKhronosExtensions)";
	}
	if (OMX_PortDomainVendorStartUnused <= id && 
		id < OMX_PortDomainMax) {
		name = "(OMX_PortDomainVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_SUSPENSIONPOLICYTYPE_name(OMX_SUSPENSIONPOLICYTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_SuspensionDisabled:
		name = "OMX_SuspensionDisabled";
		break;
	case OMX_SuspensionEnabled:
		name = "OMX_SuspensionEnabled";
		break;
	case OMX_SuspensionPolicyMax:
		name = "OMX_SuspensionPolicyMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_SuspensionPolicyKhronosExtensions <= id && 
		id < OMX_SuspensionPolicyStartUnused) {
		name = "(OMX_SuspensionPolicyKhronosExtensions)";
	}
	if (OMX_SuspensionPolicyStartUnused <= id && 
		id < OMX_SuspensionPolicyMax) {
		name = "(OMX_SuspensionPolicyStartUnused)";
	}
	return name;
}

const char *omx_enum_name::get_OMX_SUSPENSIONTYPE_name(OMX_SUSPENSIONTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_NotSuspended:
		name = "OMX_NotSuspended";
		break;
	case OMX_Suspended:
		name = "OMX_Suspended";
		break;
	case OMX_SuspendMax:
		name = "OMX_SuspendMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_SuspensionKhronosExtensions <= id && 
		id < OMX_SuspensionVendorStartUnused) {
		name = "(OMX_SuspensionKhronosExtensions)";
	}
	if (OMX_SuspensionVendorStartUnused <= id && 
		id < OMX_SuspendMax) {
		name = "(OMX_SuspensionVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_METADATACHARSETTYPE_name(OMX_METADATACHARSETTYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_MetadataCharsetUnknown:
		name = "OMX_MetadataCharsetUnknown";
		break;
	case OMX_MetadataCharsetASCII:
		name = "OMX_MetadataCharsetASCII";
		break;
	case OMX_MetadataCharsetBinary:
		name = "OMX_MetadataCharsetBinary";
		break;
	case OMX_MetadataCharsetCodePage1252:
		name = "OMX_MetadataCharsetCodePage1252";
		break;
	case OMX_MetadataCharsetUTF8:
		name = "OMX_MetadataCharsetUTF8";
		break;
	case OMX_MetadataCharsetJavaConformantUTF8:
		name = "OMX_MetadataCharsetJavaConformantUTF8";
		break;
	case OMX_MetadataCharsetUTF7:
		name = "OMX_MetadataCharsetUTF7";
		break;
	case OMX_MetadataCharsetImapUTF7:
		name = "OMX_MetadataCharsetImapUTF7";
		break;
	case OMX_MetadataCharsetUTF16LE:
		name = "OMX_MetadataCharsetUTF16LE";
		break;
	case OMX_MetadataCharsetUTF16BE:
		name = "OMX_MetadataCharsetUTF16BE";
		break;
	case OMX_MetadataCharsetGB12345:
		name = "OMX_MetadataCharsetGB12345";
		break;
	case OMX_MetadataCharsetHZGB2312:
		name = "OMX_MetadataCharsetHZGB2312";
		break;
	case OMX_MetadataCharsetGB2312:
		name = "OMX_MetadataCharsetGB2312";
		break;
	case OMX_MetadataCharsetGB18030:
		name = "OMX_MetadataCharsetGB18030";
		break;
	case OMX_MetadataCharsetGBK:
		name = "OMX_MetadataCharsetGBK";
		break;
	case OMX_MetadataCharsetBig5:
		name = "OMX_MetadataCharsetBig5";
		break;
	case OMX_MetadataCharsetISO88591:
		name = "OMX_MetadataCharsetISO88591";
		break;
	case OMX_MetadataCharsetISO88592:
		name = "OMX_MetadataCharsetISO88592";
		break;
	case OMX_MetadataCharsetISO88593:
		name = "OMX_MetadataCharsetISO88593";
		break;
	case OMX_MetadataCharsetISO88594:
		name = "OMX_MetadataCharsetISO88594";
		break;
	case OMX_MetadataCharsetISO88595:
		name = "OMX_MetadataCharsetISO88595";
		break;
	case OMX_MetadataCharsetISO88596:
		name = "OMX_MetadataCharsetISO88596";
		break;
	case OMX_MetadataCharsetISO88597:
		name = "OMX_MetadataCharsetISO88597";
		break;
	case OMX_MetadataCharsetISO88598:
		name = "OMX_MetadataCharsetISO88598";
		break;
	case OMX_MetadataCharsetISO88599:
		name = "OMX_MetadataCharsetISO88599";
		break;
	case OMX_MetadataCharsetISO885910:
		name = "OMX_MetadataCharsetISO885910";
		break;
	case OMX_MetadataCharsetISO885913:
		name = "OMX_MetadataCharsetISO885913";
		break;
	case OMX_MetadataCharsetISO885914:
		name = "OMX_MetadataCharsetISO885914";
		break;
	case OMX_MetadataCharsetISO885915:
		name = "OMX_MetadataCharsetISO885915";
		break;
	case OMX_MetadataCharsetShiftJIS:
		name = "OMX_MetadataCharsetShiftJIS";
		break;
	case OMX_MetadataCharsetISO2022JP:
		name = "OMX_MetadataCharsetISO2022JP";
		break;
	case OMX_MetadataCharsetISO2022JP1:
		name = "OMX_MetadataCharsetISO2022JP1";
		break;
	case OMX_MetadataCharsetISOEUCJP:
		name = "OMX_MetadataCharsetISOEUCJP";
		break;
	case OMX_MetadataCharsetSMS7Bit:
		name = "OMX_MetadataCharsetSMS7Bit";
		break;
	case OMX_MetadataCharsetTypeMax:
		name = "OMX_MetadataCharsetTypeMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_MetadataCharsetKhronosExtensions <= id && 
		id < OMX_MetadataCharsetVendorStartUnused) {
		name = "(OMX_MetadataCharsetKhronosExtensions)";
	}
	if (OMX_MetadataCharsetVendorStartUnused <= id && 
		id < OMX_MetadataCharsetTypeMax) {
		name = "(OMX_MetadataCharsetVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_METADATASCOPETYPE_name(OMX_METADATASCOPETYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_MetadataScopeAllLevels:
		name = "OMX_MetadataScopeAllLevels";
		break;
	case OMX_MetadataScopeTopLevel:
		name = "OMX_MetadataScopeTopLevel";
		break;
	case OMX_MetadataScopePortLevel:
		name = "OMX_MetadataScopePortLevel";
		break;
	case OMX_MetadataScopeNodeLevel:
		name = "OMX_MetadataScopeNodeLevel";
		break;
	case OMX_MetadataScopeTypeMax:
		name = "OMX_MetadataScopeTypeMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_MetadataScopeKhronosExtensions <= id && 
		id < OMX_MetadataScopeVendorStartUnused) {
		name = "(OMX_MetadataScopeKhronosExtensions)";
	}
	if (OMX_MetadataScopeVendorStartUnused <= id && 
		id < OMX_MetadataScopeTypeMax) {
		name = "(OMX_MetadataScopeVendorStartUnused)";
	}

	return name;
}

const char *omx_enum_name::get_OMX_METADATASEARCHMODETYPE_name(OMX_METADATASEARCHMODETYPE id)
{
	const char *name = "(error)";

	switch (id) {
	case OMX_MetadataSearchValueSizeByIndex:
		name = "OMX_MetadataSearchValueSizeByIndex";
		break;
	case OMX_MetadataSearchItemByIndex:
		name = "OMX_MetadataSearchItemByIndex";
		break;
	case OMX_MetadataSearchNextItemByKey:
		name = "OMX_MetadataSearchNextItemByKey";
		break;
	case OMX_MetadataSearchTypeMax:
		name = "OMX_MetadataSearchTypeMax";
		break;
	default:
		name = "(unknown)";
		break;
	}

	if (OMX_MetadataSearchKhronosExtensions <= id && 
		id < OMX_MetadataSearchVendorStartUnused) {
		name = "(OMX_MetadataSearchKhronosExtensions)";
	}
	if (OMX_MetadataSearchVendorStartUnused <= id && 
		id < OMX_MetadataSearchTypeMax) {
		name = "(OMX_MetadataSearchVendorStartUnused)";
	}

	return name;
}

} //namespace mf
