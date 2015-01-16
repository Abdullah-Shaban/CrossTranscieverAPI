#ifndef HAVE_DEVICEIMP_H
#define HAVE_DEVICEIMP_H

#include "transceiver.hpp"

class DeviceImp
{
	public:
		DeviceImp(Transceiver::I_ReceiveDataPush* rx_if);
};

#endif
