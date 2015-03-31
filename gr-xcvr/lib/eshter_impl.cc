/* -*- c++ -*- */
// vim:sw=2 ts=2
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
#endif

#include <gnuradio/io_signature.h>
#include "eshter_impl.h"

void ReceiveDataPush::pushBBSamplesRx(Transceiver::BBPacket* thePushedPacket,
				Transceiver::Boolean endOfBurst)
{
	printf("==> pushBBSamplesRx received %lu samples\n", thePushedPacket->SampleNumber);

	printf("    [");
	int n;
	for(n = 0; n < 5; n++) {
		printf("%f ", thePushedPacket->packet[n].valueI);
	}
	printf("...]\n");

	boost::unique_lock<boost::mutex> l(m);

	buffer = std::vector<Transceiver::BBSample>(thePushedPacket->packet,
			thePushedPacket->packet+thePushedPacket->SampleNumber);

	l.unlock();
	cv.notify_one();
};

void ReceiveDataPush::push_output(float *out)
{
	boost::unique_lock<boost::mutex> l(m);
	cv.wait(l);

	std::vector<Transceiver::BBSample>::const_iterator i;
	for(i = buffer.begin(); i != buffer.end(); ++i) {
		*out = (i->valueI - 2048.) / 2048.;
		out++;
	}
}

namespace gr {
  namespace xcvr {

    eshter::sptr
    eshter::make(std::string device_path,
		      float frequency,
		      int tuning_preset,
		      int packet_size)
    {
      return gnuradio::get_initial_sptr
        (new eshter_impl(device_path, frequency, tuning_preset, packet_size));
    }

    /*
     * The private constructor
     */
		eshter_impl::eshter_impl(std::string device_path, float frequency, int tuning_preset, int packet_size)
      : gr::sync_block("eshter",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(float)*packet_size)),
			d_device_path(device_path),
			d_frequency(frequency),
			d_tuning_preset(tuning_preset),
			d_packet_size(packet_size),
			running(false)
    {
			sensor = new VESNA::SpectrumSensor(device_path);
			receiver = new ReceiveDataPush;
			eshter = new DeviceImp(receiver, sensor);
		}

    /*
     * Our virtual destructor.
     */
    eshter_impl::~eshter_impl()
    {
			if(running) {
				stop();
			}

			delete eshter;
			delete receiver;
	    delete sensor;
    }

    bool eshter_impl::start(void)
    {
			Transceiver::Time start(Transceiver::immediateDiscriminator);
			Transceiver::Time stop(Transceiver::undefinedDiscriminator);

			cycle_id = eshter->receiveChannel.createReceiveCycleProfile(start, stop, d_packet_size,
					d_tuning_preset, d_frequency);

			running = true;
    }

    bool eshter_impl::stop(void)
    {
			Transceiver::Time stop(Transceiver::immediateDiscriminator);
			eshter->receiveChannel.setReceiveStopTime(cycle_id, stop);

			running = false;
    }

    void eshter_impl::set_frequency(float frequency)
		{
			stop();
			d_frequency = frequency;
			start();
		}

    float eshter_impl::frequency()
		{
			return d_frequency;
		}


    int
    eshter_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
			assert(noutput_items > 0);

			float *out = (float*) output_items[0];

			receiver->push_output(out);

      return 1;
    }

  } /* namespace xcvr */
} /* namespace gr */

