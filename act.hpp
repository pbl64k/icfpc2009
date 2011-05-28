#ifndef W_OM_HPP_ACT

#define W_OM_HPP_ACT

#include <utility>
#include <vector>

#include "om.hpp"

#include "page.hpp"
#include "phys.hpp"
#include "sptn.hpp"
#include "orcl.hpp"

W_OM_NS_START

	typedef Page<PortData, addrSpace> IoPage;
	typedef std::pair<Addr, PortData> PortAction;
	typedef std::vector<PortAction> Action;

	class Actuator {
		public:
			virtual ~Actuator();

			// IoPage has to be non-const: tweaking the sensor data comes
			// useful sometimes
			virtual Action actuate(IoPage &outp, bool interactive = false)
					= 0;

		protected:
			void makeThrusterAction(Action &act, const Vector &dv) const;
			void makeFreefallAction(Action &act) const;
	};

W_OM_NS_END

#endif
