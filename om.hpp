#ifndef W_OM_HPP_OM

#define W_OM_HPP_OM

#include <cstddef>
#include <climits>
#include <cassert>

#define W_OM_NS_START \
namespace TeamWobble { \
	namespace Icfpc { \
		namespace Om {

#define W_OM_NS_END \
		} \
	} \
}

#undef W_OM_OPT_BOUNDS_CHECKING

W_OM_NS_START

	static const unsigned char addrSpace = 14;

	typedef unsigned short Addr;
	typedef unsigned int Word;
	typedef Word Instr;
	typedef double Double;
	typedef Double PortData;

	const Word teamId = 55; // wobbling along

	static const Addr scoreOutPort = 0;
	static const Addr fuelOutPort = 1;
	static const Addr scenarioInPort = 0x3e80;

	static const Addr dvxInPort = 2;
	static const Addr dvyInPort = 3;

W_OM_NS_END

#endif
