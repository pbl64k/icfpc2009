#ifndef W_OM_HPP_3CLC

#define W_OM_HPP_3CLC

#include "om.hpp"

#include "2clc.hpp"

W_OM_NS_START

	class EccChaserCalculatingActuator : public ChaserCalculatingActuator {
		public:
			EccChaserCalculatingActuator();
			virtual ~EccChaserCalculatingActuator();

			virtual Action actuate(IoPage &outp, bool interactive = false);
	};

W_OM_NS_END

#endif
