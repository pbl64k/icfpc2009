#include "1wkd.hpp"

W_OM_NS_START

	const Addr HohmannWickedActuator::sxOutPort;
	const Addr HohmannWickedActuator::syOutPort;
	const Addr HohmannWickedActuator::targetOutPort;

	const Word HohmannWickedActuator::reserveFuel;
	const Word HohmannWickedActuator::orbitalTolerance;

	// lots of tweakage, should've kept track of it
	// can't believe I thought I won't need an svn repository

	HohmannWickedActuator::HohmannWickedActuator():
			me_(Sputnik()),
			fuel_(0.0),
			firstThrust_(false), secondThrust_(false),
			firstThrustDv_(0.0), secondThrustDv_(0.0),
			delay_(0),
			waypoint_(-1.0),
			startingR_(0.0), midR_(0.0), targetR_(0.0),
			dvsm_(0.0), dvms_(0.0), dvst_(0.0), dvmt_(0.0) {
	}

	HohmannWickedActuator::~HohmannWickedActuator() {
	}

	Action HohmannWickedActuator::actuate(IoPage &outp, bool interactive) {
		fuel_ = outp[fuelOutPort];

		targetR_ = outp[targetOutPort];
		me_.targetR_ = (waypoint_ > 0) ? waypoint_ : targetR_;

		me_.pushPos(Vector(outp[sxOutPort], outp[syOutPort]));

		Action act = Action();

		bool newLine = (delay_ > 0);

		if (me_.lastR_ < 1) { // little is known about the world
			calcOrbits();
			calcIntermediateThrusts();

			if (interactive) {
				std:: cout << std::endl << "Just starting..." <<
					std::endl;
				newLine = true;
			}

			makeFreefallAction(act);
		}
		else if (!firstThrust_) {
			firstThrust_ = true;

			pickDestination();
			calcThrusts(me_.r_, me_.targetR_);

			if (interactive) {
				std::cout << std::endl << std::endl <<
						"v1: " << firstThrustDv_ <<
						" v2: " << secondThrustDv_ << std::endl << std::endl;
				newLine = true;
			}

			Vector vThrust = PhysUtil::scaleVectorToLength(me_.mov_,
					firstThrustDv_);

			makeThrusterAction(act, vThrust);
		}
		else if (!secondThrust_) {
			if ((std::abs(me_.h_) < orbitalTolerance) &&
					(std::abs(me_.lastH_) < std::abs(me_.h_))) {
				if (interactive) {
					std::cout << std::endl << std::endl <<
							"Second thrust!" << std::endl << std::endl;
					newLine = true;
				}

				Vector vThrust = PhysUtil::scaleVectorToLength(me_.mov_,
						secondThrustDv_);

				makeThrusterAction(act, vThrust);
	
				if (waypoint_ > 0) {
					firstThrust_ = false;
				}
				else {
					secondThrust_ = true;
					delay_ = 25;
				}
			}
			else {
				makeFreefallAction(act);
			}
		}
		else { // in final orbit, may need corrections
			if ((delay_ == 0) && (std::abs(me_.h_) < orbitalTolerance) &&
					(std::abs(me_.lastH_) < std::abs(me_.h_))) {
				Vector cThrust = PhysUtil::subtractVectors(
						PhysUtil::scaleAndRotateVector(me_.pos_,
						PhysUtil::calcCircOrbitalSpeed(me_.targetR_),
						me_.mov_),
						me_.mov_);

				makeThrusterCorrectionAction(act, cThrust);
			}
			else if ((delay_ == 0) && (me_.lastH_ / me_.h_ > 0.0)) {
				Vector cThrust = PhysUtil::scaleVectorToLength(
						me_.pos_, (me_.r_ - me_.targetR_) * 0.01);
	
				makeThrusterCorrectionAction(act, cThrust);
			}
			else {
				makeFreefallAction(act);
			}
		}

		if (interactive) {
			std::cout.precision(2);
			std::cout << std::fixed <<
					" dH: " << me_.dH_ <<
					" H: " << me_.h_ <<
					" DSp: " <<
					PhysUtil::calcCircOrbitalSpeed(me_.targetR_) <<
					" Sp: " << PhysUtil::calcModulus(me_.mov_) <<
					"  R: " << me_.r_ <<
					"        " <<
					(newLine ? "\n" : "\r");
		}

		if (delay_ > 0) {
			--delay_;
		}

		return act;
	}

	void HohmannWickedActuator::pickDestination() {
		if (std::abs(me_.r_ - startingR_) < std::abs(me_.r_ - midR_)) {
			if (fuel_ > (dvsm_ + dvmt_ + reserveFuel)) {
				me_.targetR_ = waypoint_ = midR_;
			}
			else {
				waypoint_ = -1.0;
				me_.targetR_ = targetR_;
			}
		}
		else {
			if (fuel_ > (dvms_ + dvst_ + reserveFuel)) {
				me_.targetR_ = waypoint_ = startingR_;
			}
			else {
				waypoint_ = -1.0;
				me_.targetR_ = targetR_;
			}
		}
	}

	void HohmannWickedActuator::calcOrbits() {
		startingR_ = me_.r_;
		// arbitrary: tried 0.01, 0.02, 0.025 and 0.05
		midR_ = startingR_ + ((targetR_ - startingR_) * 0.025);
	}

	void HohmannWickedActuator::calcThrusts(Scalar cur, Scalar target) {
		firstThrustDv_ = calcFirstThrust(cur, target);
		secondThrustDv_ = calcSecondThrust(cur, target);
	}

	Scalar HohmannWickedActuator::calcTotalThrust(Scalar cur,
			Scalar target) const {
		return std::abs(calcFirstThrust(cur, target)) +
				std::abs(calcSecondThrust(cur, target));
	}

	Scalar HohmannWickedActuator::calcFirstThrust(Scalar cur,
			Scalar target) const {
		return std::sqrt(earthGM / cur) *
				(std::sqrt((2 * target) / (cur + target)) - 1);
	}

	Scalar HohmannWickedActuator::calcSecondThrust(Scalar cur,
			Scalar target) const {
		return std::sqrt(earthGM / target) *
				(1 - std::sqrt((2 * cur) / (cur + target)));
	}

	void HohmannWickedActuator::calcIntermediateThrusts() {
		dvsm_ = calcTotalThrust(startingR_, midR_);
		dvms_ = calcTotalThrust(midR_, startingR_);
		dvst_ = calcTotalThrust(startingR_, targetR_);
		dvmt_ = calcTotalThrust(midR_, targetR_);
	}

	void HohmannWickedActuator::makeThrusterCorrectionAction(Action &act,
			const Vector &dv) {
		Scalar dvSc = PhysUtil::calcModulus(dv);

		if ((dvSc < fuel_) && (dvSc > 1)) {
			delay_ = 10;

			makeThrusterAction(act, dv);
		}
		else {
			makeFreefallAction(act);
		}
	}

W_OM_NS_END
