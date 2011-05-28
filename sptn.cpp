#include "sptn.hpp"

W_OM_NS_START

	Sputnik::Sputnik(): targetR_(0.0),
			pos_(Vector(0, 0)), r_(0.0), h_(0.0),
			lastPos_(Vector(0, 0)), lastR_(0.0), lastH_(0.0),
			mov_(Vector(0, 0)),
			dH_(0.0) {
	}

	Sputnik::~Sputnik() {
	}

	void Sputnik::pushPos(const Vector &pos) {
		lastPos_ = pos_;
		lastR_ = r_;
		lastH_ = h_;

		pos_ = pos;

		recalc();
	}

	void Sputnik::recalc() {
		// quirky
		mov_ = PhysUtil::subtractVectors(lastPos_, pos_);

		r_ = PhysUtil::calcModulus(pos_);
		h_ = targetR_ - r_;
		dH_ = lastH_ - h_;
	}

W_OM_NS_END
