#include "proc.hpp"

W_OM_NS_START

	const unsigned char Proc::opcodeWidth;
	const unsigned char Proc::cmpztypeWidth;

	const Word Proc::magicId;

	const Word Proc::dStype;
	const Word Proc::dAdd;
	const Word Proc::dSub;
	const Word Proc::dMult;
	const Word Proc::dDiv;
	const Word Proc::dOutput;
	const Word Proc::dPhi;

	const Word Proc::sNoop;
	const Word Proc::sCmpz;
	const Word Proc::sSqrt;
	const Word Proc::sCopy;
	const Word Proc::sInput;

	const Word Proc::cmpzLt;
	const Word Proc::cmpzLte;
	const Word Proc::cmpzEq;
	const Word Proc::cmpzGte;
	const Word Proc::cmpzGt;

	const Word Proc::timeLimit;

	const size_t Proc::recLen;

	Proc::TraceData::TraceData(void *ptr, size_t size): datumPtr_(ptr),
			datumSize_(size) {
		assert(datumPtr_ != NULL);
		assert(datumSize_ > 0);
	}

	Proc::TraceData::~TraceData() {
	}

	void Proc::checkSanity() {
		assert(CHAR_BIT == 8);

		assert(sizeof(Addr) == 2);
		assert(sizeof(Word) == 4);
		assert(sizeof(Instr) == 4);
		assert(sizeof(Double) == 8);
		assert(sizeof(PortData) == 8);

		assert((sizeof(Addr) * CHAR_BIT) >= addrSpace);
	}

	Proc::Proc(Word scenarioId, bool interactive):
			inited_(false), interactive_(interactive),
			prg_(new PrgPage()), data_(new DataPage()),
			inp_(new IoPage()), outp_(new IoPage()),
			act_(ActuatorMaker::makeActuator(scenarioId)),
			scenarioId_(scenarioId),
			t_(0), status_(false), pc_(0), stop_(0),
			trace_(NULL) {
		assert(hasValidObjects());
		assert(hasValidScenarioId());

		if (interactive_) {
			std::cout << std::endl << "checking sanity...";
		}

		Proc::checkSanity();

		if (interactive_) {
			std::cout << " done." << std::endl;
		}
	}

	Proc::~Proc() {
		if (inited_) {
			closeTrace();
		}

		assert(hasValidObjects());

		delete act_;
		delete prg_;
		delete data_;
		delete inp_;
		delete outp_;
	}

	void Proc::init() {
		assert(!inited_);

		assert(hasValidObjects());
		assert(hasValidScenarioId());

		feedScenarioIdPort();
		load();
		openTrace();

		inited_ = true;
	}

	void Proc::run() {
		assert(inited_);

		assert(hasValidObjects());
		assert(hasValidScenarioId());

		if (interactive_) {
			std::cout << std::endl << "running simulation..." << std::endl;
		}

		do {
			control();
			if (isSimulationOver()) {
				if (interactive_) {
					std::cout << std::endl << "*** TIME: " << t_ <<
							" FUEL: " << (*outp_)[fuelOutPort] <<
							" SCORE: " << (*outp_)[scoreOutPort] << " ***" <<
							std::endl << std::endl;
				}
				break;
			}
			actuate();
			tick();
		} while(true);

		if (interactive_) {
			std::cout << std::endl << "simulation complete." << std::endl;
		}
	}

	void Proc::control() {
		pc_ = 0;

		while (pc_ != stop_) {
			exec();
			++pc_;
		}
	}

	void Proc::actuate() {
		Action act = act_->actuate(*outp_, interactive_);
		Trace trc = Trace();

		for (Action::iterator i = act.begin(); i != act.end(); ++i) {
			if ((*inp_)[i->first] != i->second) {
				(*inp_)[i->first] = i->second;
				trc.push_back(*i);
			}
		}

		if (trc.size() > 0) {
			trace(trc);
		}
	}

	void Proc::tick() {
		++t_; // tock
	}

	bool Proc::isSimulationOver() const {
		if (t_ > timeLimit) {
			return true;
		}

		if ((*outp_)[scoreOutPort] != 0) {
			return true;
		}

		return false;
	}

	void Proc::exec() {
		Word opcode = W_OM_DOPC((*prg_)[pc_]);

		switch (opcode) {
			case dStype:
				execStype();
				break;
			case dAdd:
				execAdd();
				break;
			case dSub:
				execSub();
				break;
			case dMult:
				execMult();
				break;
			case dDiv:
				execDiv();
				break;
			case dOutput:
				execOutput();
				break;
			case dPhi:
				execPhi();
				break;
			default:
				assert(false);
		}
	}

	void Proc::execStype() {
		Word opcode = W_OM_SOPC((*prg_)[pc_]);
		
		switch (opcode) {
			case sNoop:
				//
				break;
			case sCmpz:
				execCmpz();
				break;
			case sSqrt:
				execSqrt();
				break;
			case sCopy:
				execCopy();
				break;
			case sInput:
				execInput();
				break;
			default:
				assert(false);
		}
	}

	void Proc::execAdd() {
		(*data_)[pc_] =
				(*data_)[W_OM_DR1((*prg_)[pc_])]
				+
				(*data_)[W_OM_DR2((*prg_)[pc_])];
	}

	void Proc::execSub() {
		(*data_)[pc_] =
				(*data_)[W_OM_DR1((*prg_)[pc_])]
				-
				(*data_)[W_OM_DR2((*prg_)[pc_])];
	}

	void Proc::execMult() {
		(*data_)[pc_] =
				(*data_)[W_OM_DR1((*prg_)[pc_])]
				*
				(*data_)[W_OM_DR2((*prg_)[pc_])];
	}

	void Proc::execDiv() {
		Double divisor = (*data_)[W_OM_DR2((*prg_)[pc_])];

		if (divisor == 0) {
			(*data_)[pc_] = 0;
			return;
		}

		(*data_)[pc_] = (*data_)[W_OM_DR1((*prg_)[pc_])] / divisor;
	}

	void Proc::execOutput() {
		(*outp_)[W_OM_DR1((*prg_)[pc_])] = (*data_)[W_OM_DR2((*prg_)[pc_])];
	}

	void Proc::execPhi() {
		(*data_)[pc_] = (status_
				? (*data_)[W_OM_DR1((*prg_)[pc_])]
				: (*data_)[W_OM_DR2((*prg_)[pc_])]);
	}

	void Proc::execCmpz() {
		Double value = (*data_)[W_OM_SR((*prg_)[pc_])];

		switch (W_OM_CMPZ((*prg_)[pc_])) {
			case cmpzLt:
				status_ = (value < 0.0);
				break;
			case cmpzLte:
				status_ = (value <= 0.0);
				break;
			case cmpzEq:
				status_ = (value == 0.0);
				break;
			case cmpzGte:
				status_ = (value >= 0.0);
				break;
			case cmpzGt:
				status_ = (value > 0.0);
				break;
			default:
				assert(false);
		}
	}

	void Proc::execSqrt() {
		(*data_)[pc_] = std::sqrt((*data_)[W_OM_SR((*prg_)[pc_])]);
	}

	void Proc::execCopy() {
		(*data_)[pc_] = (*data_)[W_OM_SR((*prg_)[pc_])];
	}

	void Proc::execInput() {
		(*data_)[pc_] = (*inp_)[W_OM_SR((*prg_)[pc_])];
	}

	void Proc::feedScenarioIdPort() {
		(*inp_)[scenarioInPort] = static_cast<PortData>(scenarioId_);
	}

	void Proc::openTrace() {
		if (!interactive_) {
			return;
		}

		std::ostringstream scenarioIdStream;
		scenarioIdStream << scenarioId_;
		std::string traceFn = "trace" + scenarioIdStream.str() + ".osf";

		trace_ = std::fopen(traceFn.c_str(), "w");

		assert(trace_ != NULL);

		writeTrace(wrapTraceData(magicId));
		writeTrace(wrapTraceData(teamId));
		writeTrace(wrapTraceData(scenarioId_));

		Trace trc = Trace();

		trc.push_back(PortAction(scenarioInPort, scenarioId_));

		trace(trc);
	}

	void Proc::closeTrace() {
		if (!interactive_) {
			return;
		}

		trace(Trace());

		assert(std::fclose(trace_) == 0);
	}

	void Proc::trace(const Trace &trace) {
		if (!interactive_) {
			return;
		}

		Word num = trace.size();

		writeTrace(wrapTraceData(t_));
		writeTrace(wrapTraceData(num));

		for (Trace::const_iterator i = trace.begin(); i != trace.end(); ++i) {
			Word portNum = i->first;

			writeTrace(wrapTraceData(portNum));
			writeTrace(wrapTraceData(i->second));
		}
	}

	void Proc::writeTrace(const TraceData &data) {
		if (!interactive_) {
			return;
		}

		assert(std::fwrite(data.datumPtr_, data.datumSize_, 1, trace_) == 1);
	}

	void Proc::load() {
		std::ostringstream problemIdStream;
		problemIdStream << (scenarioId_ / 1000);
		std::string file = "bin" + problemIdStream.str() + ".obf";

		if (interactive_) {
			std::cout << "loading [" << file << "]..." << std::endl;
		}

		struct stat fileStat;

		assert(stat(file.c_str(), &fileStat) == 0);

		size_t fileSize = fileStat.st_size;

		assert(fileSize > 0);

		if (interactive_) {
			std::cout << "file size is " << fileSize << " octets." <<
					std::endl;
		}

		FILE *pageFile = std::fopen(file.c_str(), "r");

		assert(pageFile != NULL);

		unsigned char *buf = new unsigned char[fileSize];

		assert(std::fread(buf, sizeof(char), fileSize, pageFile) ==
				fileSize);

		if (interactive_) {
			std::cout << fileSize << " octets read successfully." <<
					std::endl;
		}

		assert(std::fclose(pageFile) == 0);

		parse(buf, fileSize);

		delete[] buf;
	}

	void Proc::parse(const unsigned char *buf, Addr size) {
		if (interactive_) {
			std::cout << "parsing " << size << " octets of raw data..."
					<< std::endl;
		}

		assert((size % recLen) == 0);

		stop_ = size / recLen;

		if (interactive_) {
			std::cout << stop_ <<
					" instructions and data entries to be loaded." <<
					std::endl;
		}

		for (Addr i = 0; i != stop_; ++i) {
			if (i % 2) {
				(*prg_)[i] = *(reinterpret_cast<const Instr *>(buf +
						(i * recLen)));
				(*data_)[i] = *(reinterpret_cast<const Double *>(buf +
						(i * recLen) + sizeof(Instr)));
			}
			else {
				(*prg_)[i] = *(reinterpret_cast<const Instr *>(buf +
						(i * recLen) + sizeof(Double)));
				(*data_)[i] = *(reinterpret_cast<const Double *>(buf +
						(i * recLen)));
			}
		}

		if (interactive_) {
				std::cout << stop_ <<
						" instructions and data entries loaded " <<
						"successfully." << std::endl;
		}
	}

	bool Proc::hasValidObjects() const {
		assert(prg_ != NULL);
		assert(data_ != NULL);
		assert(inp_ != NULL);
		assert(outp_ != NULL);
		assert(act_ != NULL);

		return true;
	}

	bool Proc::hasValidScenarioId() const {
		assert((scenarioId_ / 1000) >= 1);
		assert((scenarioId_ / 1000) <= 5);
		assert((scenarioId_ % 1000) >= 1);
		// to allow for more than 4 scenarios per problem
		//assert((scenarioId_ % 1000) <= 4);

		return true;
	}

	Double Proc::prophesize() {
		assert(!inited_);

		interactive_ = false;

		init();

		run();

		return (*outp_)[scoreOutPort];
	}

W_OM_NS_END
