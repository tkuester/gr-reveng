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
#include <boost/circular_buffer.hpp>
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
      d_pkt_len(pkt_len),
      d_in_sync(false),
      d_pkt_idx(0)
    {
        // Match the size of the sync word
        d_sync = boost::circular_buffer<char>(sync.size());
        d_search = boost::circular_buffer<char>(sync.size());

        for(int i = 0; i < sync.size(); i++)
            d_sync.push_back(sync[i]);

        d_packet.resize(pkt_len);
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
      for(int i = 0; i < noutput_items; i++)
      {
          if(d_in_sync)
          {
              // Add bits onto buffer
              d_packet[d_pkt_idx] = in[i];
              d_pkt_idx += 1;

              // Once we hit the end
              if(d_pkt_idx == d_pkt_len)
              {
                  d_in_sync = false;

                  std::cout << "Found a packet: [";
                  for(int j = 0; j < d_packet.size(); j++)
                      std::cout << (int)d_packet[j] << ", ";

                  std::cout << "]" << std::endl;
              }
          }
          else
          {
              // Shift new bit onto search pattern
              d_search.push_back(in[i]);

              // Check if we have sync.
              if(d_search == d_sync) {
                  std::cout << "Yay found sync at idx: " << i << std::endl;

                  // TODO: More efficiently merge remaining (?) samples into d_packet
                  d_in_sync = true;
                  d_pkt_idx = 0;
              }
          }
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace reveng */
} /* namespace gr */

