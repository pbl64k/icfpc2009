#ifndef W_OM_HPP_2CLC

#define W_OM_HPP_2CLC

#include "om.hpp"

#include "1wkd.hpp"

W_OM_NS_START

	class ChaserCalculatingActuator : public HohmannWickedActuator {
		protected:
			static const Addr msxOutPort = 2;
			static const Addr msyOutPort = 3;
			static const Addr tsxOutPort = 4;
			static const Addr tsyOutPort = 5;

		public:
			ChaserCalculatingActuator();
			virtual ~ChaserCalculatingActuator();

			virtual Action actuate(IoPage &outp, bool interactive = false);

		protected:
			Sputnik him_;

			Word cnt_;

			Scalar reserveFuel_;

			bool wait_;
	};

W_OM_NS_END

#endif
