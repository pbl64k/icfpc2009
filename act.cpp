#include "act.hpp"

W_OM_NS_START

	Actuator::~Actuator() {
	}

	void Actuator::makeThrusterAction(Action &act, const Vector &dv) const {
		act.clear();

		act.push_back(PortAction(dvxInPort, dv.first));
		act.push_back(PortAction(dvyInPort, dv.second));
	}

	void Actuator::makeFreefallAction(Action &act) const {
		makeThrusterAction(act, Vector(0, 0));
	}

W_OM_NS_END
