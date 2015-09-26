// vim:sw=2 ts=2
/* 
 * Copyright (C) 2015 SensorLab, Jozef Stefan Institute
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
 *
 * Written by Tomaz Solc, tomaz.solc@ijs.si
 */

#ifndef INCLUDED_XCVR_ESHTER_IMPL_H
#define INCLUDED_XCVR_ESHTER_IMPL_H

#include <xcvr/eshter.h>

#include "SpectrumSensor.hpp"
#include "DeviceImp.hpp"

class ReceiveDataPush : public Transceiver::I_ReceiveDataPush
{
	private:
		std::vector<Transceiver::BBSample> buffer;
		bool clear;

		boost::mutex m;
		boost::condition_variable cv;

	public:
		void pushBBSamplesRx(Transceiver::BBPacket* thePushedPacket,
				Transceiver::Boolean endOfBurst);

		void push_output(float *out);
};

namespace gr {
  namespace xcvr {

    class eshter_impl : public eshter
    {
     private:
				VESNA::SpectrumSensor* sensor;
				ReceiveDataPush* receiver;
				DeviceImp* eshter;

				std::string d_device_path;
				Transceiver::Frequency d_frequency;
				Transceiver::UShort d_tuning_preset;
				Transceiver::ULong d_packet_size;

				Transceiver::ULong cycle_id;
				bool running;

     public:
			eshter_impl(std::string device_path, float frequency, int tuning_preset, int packet_size);
      ~eshter_impl();

	    bool start(void);
			bool stop(void);

      void set_frequency(float frequency);
      float frequency();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace xcvr
} // namespace gr

#endif /* INCLUDED_XCVR_ESHTER_IMPL_H */

