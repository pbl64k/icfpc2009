#include "dumb.hpp"

W_OM_NS_START

	Action DumbActuator::actuate(IoPage &outp, bool interactive) {
		if (interactive) {
			std::cout << "Seriously Dumb Actuator doing its" <<
					" Seriously Dumb Thing.\r";
		}

		outp[0] = outp[0]; // damn warnings.

		return Action();
	}

W_OM_NS_END
