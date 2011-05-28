#include "3clc.hpp"

W_OM_NS_START

	EccChaserCalculatingActuator::EccChaserCalculatingActuator():
			ChaserCalculatingActuator() {
	}

	EccChaserCalculatingActuator::~EccChaserCalculatingActuator() {
	}

	Action EccChaserCalculatingActuator::actuate(IoPage &outp,
			bool interactive) {
		fuel_ = outp[fuelOutPort];

		Action act = Action();

		Vector tgt(outp[tsxOutPort], outp[tsyOutPort]);

		Vector g;
		
		if (me_.r_ > 0) {
			g = PhysUtil::scaleVectorToLength(me_.pos_, PhysUtil::calcFreefallAcceleration(me_.r_));
		}

		Scalar dist = PhysUtil::calcModulus(tgt);

		me_.pushPos(Vector(outp[msxOutPort], outp[msyOutPort]));
		him_.pushPos(PhysUtil::subtractVectors(me_.pos_, tgt));

		bool newLine = (delay_ > 0);

		if (me_.lastR_ < 1) { // little is known about the world
			if (interactive) {
				std:: cout << std::endl << "Just starting..." <<
					std::endl;
				newLine = true;
			}

			//waypoint_ = 200000.0;
			waypoint_ = 0.0;
			me_.targetR_ = him_.r_;

			makeFreefallAction(act);
		}

		/*
		else if (((delay_ == 0) || (waypoint_ != 0)) && wait_) {
std::cout<<"D0: "<<(dist / 1000)<<" sgn: "<<PhysUtil::calcCodirectionality(me_.mov_,tgt)<<std::endl;
//Word aa;std::cin>>aa;
			//if (dist < (std::abs(me_.r_ - him_.r_) * 1.5)) {
			if ((dist < (std::abs(waypoint_) * 4)) || (dist > (std::abs(waypoint_) * 25)) ||
					((PhysUtil::calcCodirectionality(me_.mov_, tgt) / waypoint_) < 0)) {
				if (interactive) {
					std::cout << std::endl << std::endl <<
							"*** WAYPOINT  ***" <<
							std::endl << std::endl;
					//Word aa;std::cin>>aa;
				}

				//waypoint_ = ((waypoint_ < dist) ? 100000 : (waypoint_ / 2));
				//waypoint_ -= 25000.0;
				//waypoint_ /= 4.0;
				
				waypoint_ = (dist / 8) * PhysUtil::calcCodirectionality(me_.mov_, tgt);

				if (interactive) {
					std::cout << std::endl << "dist: " << dist << " cod: " <<
							PhysUtil::calcCodirectionality(me_.mov_, tgt);
					std::cout << std::endl << "selected new waypoint: " <<
							waypoint_ << std::endl;
				}

				wait_ = false;

				if (dist < 50000) {
					secondThrust_ = true;
				}
			}

			if (delay_ == 0) {
				Vector vd = PhysUtil::addVectors(PhysUtil::scaleAndRotateVector(me_.pos_, PhysUtil::calcCircOrbitalSpeed(me_.r_), me_.mov_), PhysUtil::multVector(g, 0.5));
				Vector vn = PhysUtil::addVectors(me_.mov_, g);
	
				Vector cThrust = PhysUtil::subtractVectors(vd, vn);

				//std::cout << std::endl << "ct:" << PhysUtil::calcModulus(cThrust) << std::endl;
				//Word aa;std::cin >> aa;

				if (PhysUtil::calcModulus(cThrust) > 0.1) {
					delay_ = 3;

					makeThrusterCorrectionAction(act, cThrust);
				}
				else {
					makeFreefallAction(act);
				}
			}
			else {
				makeFreefallAction(act);
			}
		}
		else if (((delay_ == 0) || (waypoint_ != 0)) && !firstThrust_) {
			firstThrust_ = true;

			me_.targetR_ = him_.r_ - waypoint_;

			calcThrusts(me_.r_, me_.targetR_);

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
	
				//if (waypoint_ > 0) {
					firstThrust_ = false;
					waypoint_ = 0.1;

					wait_ = true;
					delay_ = 10;
				//}
				//else {
				//	secondThrust_ = true;
				//	delay_ = 10;
				//}
			}
			else {
				makeFreefallAction(act);
			}
		}
		else {
			makeFreefallAction(act);
		}
		*/

		//if ((delay_ == 0) && secondThrust_) { // chase!
		else if (delay_ == 0) {
			if (reserveFuel_ == 0) {
				reserveFuel_ = fuel_ - 15000;
			}

			Vector g = PhysUtil::scaleVectorToLength(me_.pos_, PhysUtil::calcFreefallAcceleration(me_.r_));
	
			if (fuel_ > reserveFuel_) {
				Scalar toTgtSc = ((dist > 3) ? (dist * 0.1) : 0);

				Vector toTgt = PhysUtil::scaleVectorToLength(tgt, ((toTgtSc > 2500) ? 2500 : toTgtSc));
	
				Vector fixSpd = PhysUtil::subtractVectors(him_.mov_, me_.mov_);
	
				//Vector cThrust = PhysUtil::subtractVectors(PhysUtil::addVectors(toTgt, fixSpd), g);
				//Vector cThrust = PhysUtil::subtractVectors(PhysUtil::addVectors(toTgt, fixSpd), PhysUtil::multVector(g, 0.5));
				Vector cThrust = PhysUtil::addVectors(toTgt, fixSpd);
				
				if ((dist < 3) && PhysUtil::calcModulus(PhysUtil::subtractVectors(him_.mov_, me_.mov_)) < 0.5) {
				//(PhysUtil::calcModulus(cThrust) < 1)
					reserveFuel_ = 1000000;
				}

				makeThrusterCorrectionAction(act, cThrust);

				if (interactive) {
					std::cout << std::endl;
				}
	
				delay_ = 10;
			}
			/*
			if (dist > 100) {
				Vector toTgt = PhysUtil::scaleVectorToLength(PhysUtil::addVectors(tgt, PhysUtil::addVectors(him_.mov_, him_.mov_)), dist * 0.5);

				Vector cThrust = PhysUtil::subtractVectors(PhysUtil::subtractVectors(toTgt, PhysUtil::addVectors(me_.mov_, me_.mov_)), g);

				makeThrusterCorrectionAction(act, cThrust);
	
				delay_ = 2;
			}
			else {
				Vector dv = PhysUtil::subtractVectors(him_.mov_, me_.mov_);

				if (PhysUtil::calcModulus(dv) > 0.5) {
					makeThrusterCorrectionAction(act, dv);
				}
				else {
					makeFreefallAction(act);
				}
			}
			*/
		}
		else {
			makeFreefallAction(act);
		}

		Word iDist = dist;
		Word kmDist = iDist / 1000;
		Word mDist = iDist % 1000;

		if (kmDist) {
			cnt_ = 0;
		}
		else {
			++cnt_;
		}

		if (interactive) {
			std::cout.precision(0);
			std::cout << std::fixed <<
					" v: " << PhysUtil::calcModulus(him_.mov_) <<
					" dist: " << (kmDist ? kmDist : mDist) << (kmDist ? " km " : " m ") <<
					" dx: " << outp[4] <<
					" dy: " << outp[5] <<
					" f: " << fuel_ <<
					" mR: " << me_.r_ <<
					" tR: " << him_.r_ <<
					" div: " << PhysUtil::calcModulus(PhysUtil::subtractVectors(him_.mov_, me_.mov_)) <<
					" cnt: " << cnt_ <<
					" g: " << PhysUtil::calcModulus(g) <<
					"        " <<
					"\r";
					//(delay_ ? "\n" : "\r");

			//std::cout<<std::endl;Word aa;std::cin>>aa;
		}

		if (delay_ > 0) {
			--delay_;
		}

		return act;
	}

W_OM_NS_END
