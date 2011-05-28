#include "phys.hpp"

W_OM_NS_START

	Vector PhysUtil::negVector(const Vector &a) {
		return Vector(-a.first, -a.second);
	}

	Scalar PhysUtil::calcDistance(const Vector &a, const Vector &b) {
		return std::sqrt(std::pow(a.first - b.first, 2) +
				std::pow(a.second - b.second, 2));
	}

	Scalar PhysUtil::calcModulus(const Vector &a) {
		return calcDistance(a, Vector(0, 0));
	}

	Scalar PhysUtil::calcCodirectionality(const Vector &a, const Vector &b) {
		Vector hyp = subtractVectors(a, b);

		return (std::pow(calcModulus(hyp), 2) <
				(std::pow(calcModulus(a), 2) +
				std::pow(calcModulus(b), 2)))
						? 1 : -1;
	}

	Vector PhysUtil::addVectors(const Vector &a, const Vector &b) {
		return Vector(a.first + b.first, a.second + b.second);
	}

	Vector PhysUtil::subtractVectors(const Vector &a, const Vector &b) {
		return Vector(a.first - b.first, a.second - b.second);
	}

	Vector PhysUtil::multVector(const Vector &a, Scalar k) {
		return Vector(a.first * k, a.second *k);
	}

	Vector PhysUtil::scaleVectorToLength(const Vector &base, Scalar len) {
		assert(calcModulus(base) != 0); // whoopsies.

		Scalar coeff = len / calcModulus(base);

		return multVector(base, coeff);
	}

	Vector PhysUtil::scaleAndRotateVector(const Vector &base, Scalar len,
			const Vector &ref) {
		Vector scaled = scaleVectorToLength(base, len);

		Vector rotated1(scaled.second, -scaled.first);
		Vector rotated2(-scaled.second, scaled.first);

		if (calcModulus(subtractVectors(ref, rotated1)) <
			calcModulus(subtractVectors(ref, rotated2))) {
			return rotated1;
		}
		else {
			return rotated2;
		}

		return scaled;
	}

	Scalar PhysUtil::calcFreefallAcceleration(Scalar radius) {
		assert(radius > 0.0);

		return earthGM / std::pow(radius, 2);
	}

	Scalar PhysUtil::calcCircOrbitalSpeed(Scalar radius) {
		assert(radius > 0.0);

		return std::sqrt(earthGM / radius);
	}

	Scalar PhysUtil::calcCircOrbitalSpeed(const Vector &toCenter) {
		return calcCircOrbitalSpeed(calcModulus(toCenter));
	}

W_OM_NS_END
