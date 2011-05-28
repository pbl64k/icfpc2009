#ifndef W_OM_HPP_ORCL

#define W_OM_HPP_ORCL

#include "om.hpp"

W_OM_NS_START

	class OracleInterface {
		public:
			virtual ~OracleInterface();

			virtual Double prophesize() = 0;
	};

W_OM_NS_END

#endif
