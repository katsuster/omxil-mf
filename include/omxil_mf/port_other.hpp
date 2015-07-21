#ifndef OMX_MF_PORT_OTHER_HPP__
#define OMX_MF_PORT_OTHER_HPP__

#include <OMX_Component.h>
#include <OMX_Core.h>
#include <OMX_Other.h>

#include <omxil_mf/port.hpp>

namespace mf {

class port_other : public port {
public:
	//親クラス
	typedef port super;

	//disable default constructor
	port_other() = delete;

	port_other(int ind, component *c);

	virtual ~port_other();

	//disable copy constructor
	port_other(const port_other& obj) = delete;

	//disable operator=
	port_other& operator=(const port_other& obj) = delete;

	virtual const char *get_name() const;

	virtual OMX_OTHER_FORMATTYPE get_format() const;
	virtual void set_format(OMX_OTHER_FORMATTYPE v);

	/**
	 * Get OpenMAX IL definition data of this port.
	 *
	 * OMX_PARAM_PORTDEFINITIONTYPE の各メンバに加え、
	 * format.other メンバにも設定値がセットされます。
	 *
	 * struct OMX_OTHER_PORTDEFINITIONTYPE {
	 *     OMX_OTHER_FORMATTYPE eFormat;
	 * }
	 *
	 * @return port definition data of OpenMAX IL
	 */
	virtual const OMX_PARAM_PORTDEFINITIONTYPE *get_definition() const;

private:
	OMX_OTHER_FORMATTYPE format;

};

} //namespace mf

#endif //OMX_MF_PORT_OTHER_HPP__
