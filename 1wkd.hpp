#ifndef W_OM_HPP_1WKD

#define W_OM_HPP_1WKD

#include <iostream>
#include <ostream>
#include <istream>

#include "om.hpp"

#include "act.hpp"

W_OM_NS_START

	class HohmannWickedActuator : public Actuator {
		protected:
			static const Addr sxOutPort = 2;
			static const Addr syOutPort = 3;
			static const Addr targetOutPort = 4;

			static const Word reserveFuel = 20;
			static const Word orbitalTolerance = 1000;

		public:
			HohmannWickedActuator();
			virtual ~HohmannWickedActuator();

			virtual Action actuate(IoPage &outp, bool interactive = false);

		protected:
			Sputnik me_;

			Scalar fuel_;

			bool firstThrust_;
			bool secondThrust_;

			Scalar firstThrustDv_;
			Scalar secondThrustDv_;

			Word delay_;

			Scalar waypoint_;

			Scalar startingR_;
			Scalar midR_;
			Scalar targetR_;

			Scalar dvsm_;
			Scalar dvms_;
			Scalar dvst_;
			Scalar dvmt_;

			void pickDestination();
			void calcOrbits();
			void calcThrusts(Scalar cur, Scalar target);
			Scalar calcTotalThrust(Scalar cur, Scalar target) const;
			Scalar calcFirstThrust(Scalar cur, Scalar target) const;
			Scalar calcSecondThrust(Scalar cur, Scalar target) const;
			void calcIntermediateThrusts();

			void makeThrusterCorrectionAction(Action &act, const Vector &dv);
	};

W_OM_NS_END

#endif
