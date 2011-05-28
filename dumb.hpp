#ifndef W_OM_HPP_DUMB

#define W_OM_HPP_DUMB

#include <iostream>
#include <ostream>
#include <istream>

#include "om.hpp"

#include "act.hpp"

W_OM_NS_START

	class DumbActuator : public Actuator {
		public:
			virtual Action actuate(IoPage &outp, bool interactive = false);
	};

W_OM_NS_END

#endif
