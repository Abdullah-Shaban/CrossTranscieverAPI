#include "serial/serial.h"

namespace VESNA {

class SpectrumSensor
{
	public:
		SpectrumSensor(const std::string &port);
		~SpectrumSensor();

	private:
		serial::Serial *comm;
};

};
