/* -*- c++ -*- */
/* 
 * Copyright 2015 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
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
 */

#ifndef INCLUDED_XCVR_IMECSENSINGENGINE_IMPL_H
#define INCLUDED_XCVR_IMECSENSINGENGINE_IMPL_H

#include <xcvr/ImecSensingEngine.h>
#include "ImecSE.hpp"

namespace gr {
  namespace xcvr {

    class Receiver : public Transceiver::I_ReceiveDataPush
    {
    public:
      void pushBBSamplesRx(Transceiver::BBPacket* BB,
			   Transceiver::Boolean endOfBurst) {
	
	for (Transceiver::ULong i=0; i<BB->SampleNumber; i++) {
	  *sample = gr_complex(BB->packet[i].valueI, BB->packet[i].valueQ);
	  sample++;
	}
      };

      gr_complex *sample;
    };
    
    class ImecSensingEngine_impl : public ImecSensingEngine
    {
     private:
      Receiver* ReceiverCallback;
      ImecSE* SensingEngine;
      float d_frequency;
      unsigned int d_samples;

     public:
      ImecSensingEngine_impl(float frequency, unsigned int samples);
      ~ImecSensingEngine_impl();

      void set_frequency(float frequency);

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace xcvr
} // namespace gr

#endif /* INCLUDED_XCVR_IMECSENSINGENGINE_IMPL_H */

