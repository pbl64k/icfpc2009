#ifndef W_OM_HPP_PROC

#define W_OM_HPP_PROC

#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <cmath>

#include <string>
#include <iostream>
#include <ostream>
#include <istream>
#include <sstream>

#include "om.hpp"

#include "page.hpp"
#include "orcl.hpp"
#include "act.hpp"
#include "actm.hpp"

W_OM_NS_START

	#define W_OM_DOPC(x) (x >> (addrSpace * 2))
	#define W_OM_SOPC(x) (x >> ((addrSpace * 2) - opcodeWidth))
	#define W_OM_DR1(x) ((x >> addrSpace) & ((1 << addrSpace) - 1))
	#define W_OM_DR2(x) (x & ((1 << addrSpace) - 1))
	#define W_OM_SR(x) W_OM_DR2(x)
	#define W_OM_CMPZ(x) ((x >> \
			((addrSpace * 2) - opcodeWidth - cmpztypeWidth)) & \
			((1 << (cmpztypeWidth)) - 1))

	class Proc : public OracleInterface {
		private:
			typedef Page<Instr, addrSpace> PrgPage;
			typedef Page<Double, addrSpace> DataPage;
		
			typedef Action Trace;

			struct TraceData {
				public:
					void *datumPtr_;
					size_t datumSize_;

					TraceData(void *ptr, size_t size);
					virtual ~TraceData();
			};

			template<typename T> class TraceDataWrapper : public TraceData {
				public:
					TraceDataWrapper(T datum);
					virtual ~TraceDataWrapper();
			};

			static const unsigned char opcodeWidth = 4;
			static const unsigned char cmpztypeWidth = 3;
		
			static const Word magicId = 0xcafebabe;
		
			static const Word dStype = 0;
			static const Word dAdd = 1;
			static const Word dSub = 2;
			static const Word dMult = 3;
			static const Word dDiv = 4;
			static const Word dOutput = 5;
			static const Word dPhi = 6;
		
			static const Word sNoop = 0;
			static const Word sCmpz = 1;
			static const Word sSqrt = 2;
			static const Word sCopy = 3;
			static const Word sInput = 4;

			static const Word cmpzLt = 0;
			static const Word cmpzLte = 1;
			static const Word cmpzEq = 2;
			static const Word cmpzGte = 3;
			static const Word cmpzGt = 4;
		
			static const Word timeLimit = 3000000;
		
			static const size_t recLen = sizeof(Instr) + sizeof(Double);

		public:
			static void checkSanity();

			Proc(Word scenarioId, bool interactive = false);
			virtual ~Proc();

			void init();
			void run();

			virtual Double prophesize();

		private:
			bool inited_;

			bool interactive_;

			PrgPage *prg_;
			DataPage *data_;
			IoPage *inp_;
			IoPage *outp_;

			Actuator *act_;

			Word scenarioId_;

			Word t_;

			bool status_;

			Addr pc_;
			Addr stop_;

			FILE *trace_;

			template<typename T> static TraceDataWrapper<T> wrapTraceData(
					T data);

			void control();
			void actuate();

			void tick();

			bool isSimulationOver() const;

			void exec();
			void execStype();
			void execAdd();
			void execSub();
			void execMult();
			void execDiv();
			void execOutput();
			void execPhi();
			void execCmpz();
			void execSqrt();
			void execCopy();
			void execInput();

			void feedScenarioIdPort();

			void openTrace();
			void closeTrace();
			void trace(const Trace &trace);
			void writeTrace(const TraceData &data);

			void load();
			void parse(const unsigned char *buf, Addr size);

			bool hasValidObjects() const;
			bool hasValidScenarioId() const;
	};

	template<typename T> Proc::TraceDataWrapper<T>::TraceDataWrapper(
			T datum): TraceData(&datum, sizeof(datum)) {
	}

	template<typename T> Proc::TraceDataWrapper<T>::~TraceDataWrapper() {
	}

	template<typename T> typename Proc::TraceDataWrapper<T> Proc::wrapTraceData(
			T data) {
		return Proc::TraceDataWrapper<T>(data);
	}

W_OM_NS_END

#endif
