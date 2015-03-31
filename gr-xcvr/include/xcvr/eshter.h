/* -*- c++ -*- */
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


#ifndef INCLUDED_XCVR_ESHTER_H
#define INCLUDED_XCVR_ESHTER_H

#include <xcvr/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace xcvr {

    /*!
     * \brief <+description of block+>
     * \ingroup xcvr
     *
     */
    class XCVR_API eshter : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<eshter> sptr;

      /*!
       * \brief Make a new SNE-ESHTER source block.
       *
       * \param device_path Path to device file to use for communication with
       * the sensor.
       * \param frequency Central frequency in hertz.
       * \param tuning_preset Preset ID (sets sample rate and filter bandwidth).
       * \param packet_size Number of consecutive baseband samples to take.
       */
      static sptr make(std::string device_path,
		      float frequency,
		      int tuning_preset,
		      int packet_size);

      virtual void set_frequency(float frequency) = 0;
      virtual float frequency() = 0;
    };

  } // namespace xcvr
} // namespace gr

#endif /* INCLUDED_XCVR_ESHTER_H */

