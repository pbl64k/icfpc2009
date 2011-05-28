#ifndef W_OM_HPP_PHYS

#define W_OM_HPP_PHYS

#include <cmath>

#include <utility>
#include <algorithm>

#include "om.hpp"

W_OM_NS_START

	typedef Double Scalar;
	typedef std::pair<Scalar, Scalar> Vector;

	static const Scalar earthMass = 6e24;
	static const Scalar G = 6.67428e-11;
	static const Scalar earthGM = G * earthMass;

	class PhysUtil {
		public:
			static Vector negVector(const Vector &a);
			static Scalar calcDistance(const Vector &a, const Vector &b);
			static Scalar calcModulus(const Vector &a);
			static Scalar calcCodirectionality(const Vector &a,
					const Vector &b);
			static Vector addVectors(const Vector &a, const Vector &b);
			static Vector subtractVectors(const Vector &a, const Vector &b);
			static Vector multVector(const Vector &a, Scalar k);
			static Vector scaleVectorToLength(const Vector &base,
					Scalar len);
			static Vector scaleAndRotateVector(const Vector &base,
					Scalar len, const Vector &ref);
			static Scalar calcFreefallAcceleration(Scalar radius);
			static Scalar calcCircOrbitalSpeed(Scalar radius);
			static Scalar calcCircOrbitalSpeed(const Vector &toCenter);
	};

W_OM_NS_END

#endif
