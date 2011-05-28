#include "2clc.hpp"

W_OM_NS_START

	const Addr ChaserCalculatingActuator::msxOutPort;
	const Addr ChaserCalculatingActuator::msyOutPort;
	const Addr ChaserCalculatingActuator::tsxOutPort;
	const Addr ChaserCalculatingActuator::tsyOutPort;

	ChaserCalculatingActuator::ChaserCalculatingActuator():
			HohmannWickedActuator(), him_(Sputnik()), cnt_(0),
			reserveFuel_(0), wait_(false) {
	}

	ChaserCalculatingActuator::~ChaserCalculatingActuator() {
	}

	Action ChaserCalculatingActuator::actuate(IoPage &outp,
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

		/*
		if (dist < 50000) {
			wait_ = false;
			secondThrust_ = true;
		}
		*/

		if (me_.lastR_ < 1) { // little is known about the world
			if (interactive) {
				std:: cout << std::endl << "Just starting..." <<
					std::endl;
				newLine = true;
			}

			//waypoint_ = 200000.0;
			waypoint_ = 0.0;
			me_.targetR_ = him_.r_;

			// long haul tweak
			if (him_.r_ > 20000000) {
				delay_ = 5416;
				//waypoint_ = (me_.targetR_ - me_.r_);
				//Word aa;std::cin>>aa;
			}

			// 2001
			//delay_ = 15925;
			
			// 2002
			//delay_ = 5416;

			// 2003
			//delay_ = 34140;
			
			// 2004
			//delay_ = 14727;

			makeFreefallAction(act);
		}
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

		if ((delay_ == 0) && secondThrust_) { // chase!
			if (reserveFuel_ == 0) {
				reserveFuel_ = fuel_ - 15000;
			}

			Vector g = PhysUtil::scaleVectorToLength(me_.pos_, PhysUtil::calcFreefallAcceleration(me_.r_));
	
			if (fuel_ > reserveFuel_) {
				Vector toTgt = PhysUtil::scaleVectorToLength(tgt, ((dist > 100) ? (dist * 0.1) : 0));
	
				Vector fixSpd = PhysUtil::subtractVectors(him_.mov_, me_.mov_);
	
				//Vector cThrust = PhysUtil::subtractVectors(PhysUtil::addVectors(toTgt, fixSpd), g);
				Vector cThrust = PhysUtil::addVectors(toTgt, fixSpd);
				
				if ((dist < 100) && PhysUtil::calcModulus(PhysUtil::subtractVectors(him_.mov_, me_.mov_)) < 1) {
				//(PhysUtil::calcModulus(cThrust) < 1)
					reserveFuel_ = 1000000;
				}

				makeThrusterCorrectionAction(act, cThrust);

				if (interactive) {
					std::cout << std::endl;
				}
	
				delay_ = 5;
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
					" f: " << fuel_ <<
					" mR: " << me_.r_ <<
					" dmH: " << me_.dH_ <<
					" tR: " << him_.r_ <<
					" dtH: " << him_.dH_ <<
					" dist: " << (kmDist ? kmDist : mDist) << (kmDist ? " km " : " m ") <<
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
