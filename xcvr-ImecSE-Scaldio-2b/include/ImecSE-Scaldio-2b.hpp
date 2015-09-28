 /*
 * Copyright (C) 2015 Imec, Belgium
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 * Written by Hans Cappelle, Hans.Cappelle(at)imec.be
 */

#ifndef IMECSE_HPP_INCLUDED
#define IMECSE_HPP_INCLUDED

#include "transceiver.hpp"

// maximum samples per packet 
#define MAX_SAMPLES 24575

class ImecSE : public Transceiver::I_ReceiveControl
{
public:
  ImecSE(Transceiver::I_ReceiveDataPush *Rec);
  ~ImecSE();

  Transceiver::ULong 
  createReceiveCycleProfile (Transceiver::Time requestedReceiveStartTime,
			     Transceiver::Time requestedReceiveStopTime,
			     Transceiver::ULong requestedPacketSize,
			     Transceiver::UShort requestedPresetId,
			     Transceiver::Frequency requestedCarrierFrequency);
  void 
  configureReceiveCycle(Transceiver::ULong targetCycleId,
			Transceiver::Time requestedReceiveStartTime,
			Transceiver::Time requestedReceiveStopTime,
			Transceiver::ULong requestedPacketSize,
			Transceiver::Frequency requestedCarrierFrequency) ;
  void 
  setReceiveStopTime(Transceiver::ULong targetCycleId,
		     Transceiver::Time requestedReceiveStopTime);

private:   
  void 
  ADClog1mode(Transceiver::ULong PacketSize, 
	      Transceiver::Frequency CarrierFrequency, 
	      Transceiver::Frequency BandWidth, 
	      Transceiver::ULong FrontEndGain);

  Transceiver::I_ReceiveDataPush* receiver;
};

#endif // IMECSE_HPP_INCLUDED 
