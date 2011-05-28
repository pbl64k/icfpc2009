#ifndef W_OM_HPP_SPTN

#define W_OM_HPP_SPTN

#include "om.hpp"

#include "phys.hpp"

W_OM_NS_START

	class Sputnik {
		public:
			Scalar targetR_;

			Vector pos_;
			Scalar r_;
			Scalar h_;

			Vector lastPos_;
			Scalar lastR_;
			Scalar lastH_;

			Vector mov_;

			Scalar dH_;

			Sputnik();
			virtual ~Sputnik();

			void pushPos(const Vector &pos);

		private:
			void recalc();
	};

W_OM_NS_END

#endif
