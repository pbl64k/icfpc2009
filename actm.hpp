#ifndef W_OM_HPP_ACTM

#define W_OM_HPP_ACTM

#include <string>

#include "om.hpp"

#include "act.hpp"
#include "dumb.hpp"
#include "1wkd.hpp"
#include "2clc.hpp"
#include "3clc.hpp"

W_OM_NS_START

	class ActuatorMaker {
		public:
			static Actuator *makeActuator(Word scenarioId);
	};

W_OM_NS_END

#endif
