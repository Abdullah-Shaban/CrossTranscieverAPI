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
    eshter::make(int samples)
    {
      return gnuradio::get_initial_sptr
        (new eshter_impl(samples));
    }

    /*
     * The private constructor
     */
    eshter_impl::eshter_impl(size_t samples)
      : gr::sync_block("eshter",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(float)*samples)),
			packet_size(samples),
			eshter(NULL)
    {}

    /*
     * Our virtual destructor.
     */
    eshter_impl::~eshter_impl()
    {
			if(eshter != NULL) {
				stop();
			}
    }

    bool eshter_impl::start(void)
    {
			sensor = new VESNA::SpectrumSensor("/dev/ttyUSB0");
			receiver = new ReceiveDataPush;
			eshter = new DeviceImp(receiver, sensor);

			Transceiver::Time start(Transceiver::immediateDiscriminator);
			Transceiver::Time stop(Transceiver::undefinedDiscriminator);

			// 1 MHz bandwidth, 2 MHz sampling freq
			Transceiver::UShort tuning_preset = 2;

			// 700 MHz center frequency
			Transceiver::Frequency freq = 700000000;

			cycle_id = eshter->receiveChannel.createReceiveCycleProfile(start, stop, packet_size,
					tuning_preset, freq);

    }

    bool eshter_impl::stop(void)
    {
			Transceiver::Time stop(Transceiver::immediateDiscriminator);
			eshter->receiveChannel.setReceiveStopTime(cycle_id, stop);

			delete eshter;
			delete receiver;
	    delete sensor;

			eshter = NULL;
    }

    int
    eshter_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
			assert(noutput_items > 0);

			float *out = (float*) output_items[0];

			receiver->push_output(out);
			printf("push\n");

      return 1;
    }

  } /* namespace xcvr */
} /* namespace gr */

