#ifndef SPECIAL_EXCEPT_HPP__
#define SPECIAL_EXCEPT_HPP__

#include <stdexcept>
#include <string>

namespace mf {

class interrupted_error : public std::runtime_error {
public:
	explicit interrupted_error(const std::string& what_arg) : std::runtime_error(what_arg) {
	}

	explicit interrupted_error(const char *what_arg) : std::runtime_error(what_arg) {
	}
};

} //namespace mf

#endif //SPECIAL_EXCEPT_HPP__
