#include "actm.hpp"

W_OM_NS_START

	Actuator *ActuatorMaker::makeActuator(Word scenarioId) {
		assert(scenarioId >= 1000);

		Word problemId = scenarioId / 1000;

		if (problemId == 1) {
			return new HohmannWickedActuator();
		}
		else if (problemId == 2) {
			return new ChaserCalculatingActuator();
		}
		else if (problemId == 3) {
			return new EccChaserCalculatingActuator();
		}
		else {
			return new DumbActuator();
		}
	}

W_OM_NS_END
