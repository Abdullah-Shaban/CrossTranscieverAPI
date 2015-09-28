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

#include "transceiver.hpp"
#include "ImecSE-Scaldio-2b.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <time.h>

using namespace Transceiver;

class Receiver : public I_ReceiveDataPush
{
public:
  void pushBBSamplesRx(BBPacket* BB,
		       Boolean endOfBurst) {

    bbsamples.resize(BB->SampleNumber);

    for (ULong i=0; i<BB->SampleNumber; i++) {
      bbsamples[i].valueI = BB->packet[i].valueI;
      bbsamples[i].valueQ = BB->packet[i].valueQ;
    }
  };

  std::vector<Transceiver::BBSample> bbsamples;
};

int main() {
  Receiver demoReceiver;
  ImecSE SensingEngine(&demoReceiver);

  Time start(immediateDiscriminator);
  Time stop(undefinedDiscriminator);
  Frequency freq = 2.412E9;

  SensingEngine.createReceiveCycleProfile(
				   start,
				   stop,
				   256,
				   1,
				   freq);

  timespec tS;
  tS.tv_sec = 0;
  tS.tv_nsec = 0;
  // reset the clock
  clock_settime(CLOCK_PROCESS_CPUTIME_ID, &tS);

    
  SensingEngine.configureReceiveCycle(1,
				      start,
				      stop,
				      128,
				      freq);
    
 
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tS);
  std::cout << "Time taken is: " << tS.tv_sec + tS.tv_nsec/1.0e9 << 
    " seconds" << std::endl;

  std::ofstream OUTF;
  std::stringstream buffer;

  buffer << "warp_single_ADC2.log";
  
  OUTF.open(buffer.str());

  for (int i=0; i<2048; i++) {
    OUTF << i << "\t" << demoReceiver.bbsamples[i].valueI << "\t" << demoReceiver.bbsamples[i].valueQ << std::endl;
  }

  OUTF.close();
                                            
  return 0;
}
