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

#ifdef HAVE_CONFIG_H
#include "config.h"
#include <iostream>
#endif

#include <gnuradio/io_signature.h>
#include "ImecSensingEngine_impl.h"
#include "transceiver.hpp"

namespace gr {
  namespace xcvr {

    ImecSensingEngine::sptr
    ImecSensingEngine::make(float frequency, unsigned int samples)
    {
      return gnuradio::get_initial_sptr
        (new ImecSensingEngine_impl(frequency, samples));
    }

    /*
     * The private constructor
     */
    ImecSensingEngine_impl::ImecSensingEngine_impl(float frequency, unsigned int samples)
      : gr::sync_block("ImecSensingEngine",
              gr::io_signature::make(0, 0, 0),
		       gr::io_signature::make(1, 1, sizeof(gr_complex)*samples))
    {
      ReceiverCallback = new Receiver;
      SensingEngine = new ImecSE(ReceiverCallback);
      d_frequency = frequency;
      d_samples = samples;
    }

    /*
     * Our virtual destructor.
     */
    ImecSensingEngine_impl::~ImecSensingEngine_impl()
    {
      delete SensingEngine;
      delete ReceiverCallback;
    }

    void ImecSensingEngine_impl::set_frequency(float frequency)
    {
      std::cout << "XX set_frequency callback XX\n";
      d_frequency = frequency;
    }
    
    int
    ImecSensingEngine_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        assert(noutput_items > 0);
        gr_complex *out = (gr_complex *) output_items[0];

      
        // Do <+signal processing+>
      
      Transceiver::Time start(Transceiver::immediateDiscriminator);
      Transceiver::Time stop(Transceiver::undefinedDiscriminator);
	

      ReceiverCallback->sample = out;

      SensingEngine->createReceiveCycleProfile(
				   start,
				   stop,
				   d_samples,
				   1,
				   d_frequency);

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace xcvr */
} /* namespace gr */

