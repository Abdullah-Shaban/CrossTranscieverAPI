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

#include <iostream>

#include "transceiver.hpp"
#include "ImecSE-Scaldio-2b.hpp"

extern "C"
{
#include "sensing.h"
#include "types.h"
}

using namespace Transceiver;

static se_t se_1_h = 0;
static struct se_config_s my_se_config;

ImecSE::ImecSE(I_ReceiveDataPush *Rec) : receiver(Rec) 
{
  int result = 0;
  int spider = -1; // auto detect spider board
#warning scaldio front end hard coded as number 25, adjust for other setup

  // wilab 2
  // node 30, spider board 129, scaldio-2b front end 1
  // node 41, spider board 130, scaldio-2b front end 19
  // imec development board
  // spider board 137, scaldio 2b front end 25

  int scaldio = 25; 

  if (se_1_h == 0) {
    se_1_h = se_open(spider, scaldio);

    if (se_get_status(se_1_h)) {
      result = se_init(se_1_h, &my_se_config);
    }
    assert (result==1);
  } else {
    std::cerr << "Error: imec sensing engine handler already open, no action" << std::endl;
  }

}

ImecSE::~ImecSE() 
{
  se_close(se_1_h);   
}

ULong ImecSE::createReceiveCycleProfile(
			Time			requestedReceiveStartTime,
			Time			requestedReceiveStopTime,
			ULong			requestedPacketSize,
			UShort			requestedPresetId,
			Frequency		requestedCarrierFrequency)
{
  ADClog1mode(requestedPacketSize,requestedCarrierFrequency, 10000000, 100);  
  return 0;
}

void ImecSE::configureReceiveCycle(
					     ULong			targetCycleId,
					     Time			requestedReceiveStartTime,
					     Time			requestedReceiveStopTime,
					     ULong			requestedPacketSize,
					     Frequency		requestedCarrierFrequency) 
{
  ADClog1mode(requestedPacketSize,requestedCarrierFrequency, 10000000, 100);  
}

void ImecSE::setReceiveStopTime(
			ULong			targetCycleId,
			Time			requestedReceiveStopTime)
{
  std::cerr << "dummy implementation of setReceiveStopTime()" << std::endl;
}


// supported frequency bands
#define fb1_min (93.75e6)
#define fb2_min (187.5e6)
#define fb3_min (375e6)
#define fb4_min (750e6)
#define fb5_min (1.5e9)
#define fb6_min (3.0e9)
#define fb6_max (4.0e9)

void ImecSE::ADClog1mode(Transceiver::ULong PacketSize, 
			      Transceiver::Frequency CarrierFrequency, 
			      Transceiver::Frequency BandWidth, 
			      Transceiver::ULong FrontEndGain) 
{
  int result = 0;
  int i, samples, requested_samples;
  double freq = 0;
  
  // determine channel, round downwards if not matching
  int channel = 0;
  
  if (CarrierFrequency < fb1_min) {
    channel = 1;
    freq = fb1_min;
  } else if (CarrierFrequency < fb2_min) {
    channel = 1+(CarrierFrequency - fb1_min)/625e3;
    freq = fb1_min + 625e3*(channel-1);
  } else if (CarrierFrequency < fb3_min) {
    channel = 151+(CarrierFrequency - fb2_min)/1.25e6;
    freq = fb2_min + 1.25e6*(channel-151);
  } else if (CarrierFrequency < fb4_min) {
    channel = 301+(CarrierFrequency - fb3_min)/2.5e6;
    freq = fb3_min + 2.5e6*(channel-301);
  } else if (CarrierFrequency < fb5_min) {
    channel = 451+(CarrierFrequency - fb4_min)/5e6;
    freq = fb4_min + 5e6*(channel-451);
  } else if (CarrierFrequency < fb6_min) {
    channel = 601+(CarrierFrequency - fb5_min)/10e6;
    freq = fb5_min + 10e6*(channel-601); 
  } else if (CarrierFrequency <= fb6_max) {
    channel = 751+(CarrierFrequency - fb6_min)/20e6;
    freq = fb6_min+20e6*(channel - 751); 
  } else {
    channel = 801;
    freq = fb6_max;
  }

  if (CarrierFrequency != freq) {
    std::cerr << "warning: configuring frequency " << freq << " Hz instead of " <<  CarrierFrequency << " Hz" << std::endl;
  }

  my_se_config.first_channel = 651; //channel; 
  my_se_config.last_channel = 651; //channel; 
  my_se_config.fe_gain = uint16_t(FrontEndGain); // 100 = max gain
  my_se_config.se_mode = ADC_LOG1;
  // my_se_config.bandwidth = uint32_t(BandWidth);

  result = se_check_config(se_1_h, my_se_config);
  assert(result == 1);

  samples = se_configure(se_1_h, my_se_config, 0); // 0 for single sweep, 1 for continuous sweeping
  assert(samples > 0);
  
  std::cerr << "samples XX " << samples << std::endl;
  
  // IQ samples result in one I and Q value
  requested_samples = 2*PacketSize;
  
  Boolean endOfBurst;
  
  if (requested_samples > samples) {
    std::cerr << "error: PacketSize "<< PacketSize << " is bigger than storage buffer of " << samples/2 << "samples - reducing generated samples to buffer size" << std::endl;
    endOfBurst = false;
  } else {
    samples = requested_samples;
    endOfBurst = true;
  }

  std::cerr << "samples XX2 " << samples << std::endl;
  // ADC_result size overdimensioned with factor 2 to avoid segfault
  float *ADC_result = (float *) malloc(2*samples*sizeof(float)); 
  assert(ADC_result != NULL);

  se_start_measurement(se_1_h);

  std::cerr << "XX3 " << std::endl;

  // ADC_LOG1 Scaldio-2b 
  result = se_get_result(se_1_h, ADC_result, samples);

  std::cerr << "XX4 " << std::endl;

  assert(result==1);
  
  std::vector<Transceiver::BBSample> bbsamples;

  bbsamples.resize(samples>>1);

  for (i=0; i< samples; i+=2) {
    bbsamples[i>>1] = BBSampleStruct(ADC_result[i], ADC_result[i+1]);
  }

  BBPacket samplebuffer(samples/2, &bbsamples.front());

  receiver->pushBBSamplesRx(&samplebuffer, endOfBurst);

  free(ADC_result);
}
