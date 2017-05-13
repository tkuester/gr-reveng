/* -*- c++ -*- */
/*
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
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

#include <iostream>
#include <gnuradio/io_signature.h>
#include "packet_deframer_impl.h"

namespace gr {
  namespace reveng {

    packet_deframer::sptr
    packet_deframer::make(const std::vector<char> &sync, int pkt_len)
    {
      return gnuradio::get_initial_sptr
        (new packet_deframer_impl(sync, pkt_len));
    }

    /*
     * The private constructor
     */
    packet_deframer_impl::packet_deframer_impl(const std::vector<char> &sync, int pkt_len)
      : gr::sync_block("packet_deframer",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(0, 0, 0)),
      d_sync(sync),
      d_pkt_len(pkt_len)
    {
    }

    /*
     * Our virtual destructor.
     */
    packet_deframer_impl::~packet_deframer_impl()
    {
    }

    int
    packet_deframer_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];
      int i;
      std::cout << "I has a sync len: " << d_sync.size() << std::endl;
      for(i = 0; i < d_sync.size(); i++)
          std::cout << (int)d_sync[i] << ", ";
      std::cout << std::endl;
      std::cout << "Packet Len: " << d_pkt_len << std::endl;
      /*
      std::cout << "I got " << noutput_items << " items!" << std::endl;
      for(i = 0; i < noutput_items; i++)
          std::cout << i << " - " << (int)in[i] << std::endl;
      */

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace reveng */
} /* namespace gr */

