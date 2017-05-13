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
//#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <gnuradio/io_signature.h>
#include "packet_deframer_impl.h"

using namespace std;

namespace gr {
  namespace reveng {

    packet_deframer::sptr
    packet_deframer::make(const std::string &name, const std::vector<char> &sync, bool fixed_len, int pkt_len)
    {
      return gnuradio::get_initial_sptr
        (new packet_deframer_impl(name, sync, fixed_len, pkt_len));
    }

    /*
     * The private constructor
     */
    packet_deframer_impl::packet_deframer_impl(const std::string &name, const std::vector<char> &sync, bool fixed_len, int pkt_len)
      : gr::sync_block("packet_deframer",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(0, 0, 0)),
      d_name(name),
      d_fixed_len(fixed_len),
      d_pkt_len(pkt_len),
      d_in_sync(false)
    {
        message_port_register_out(pmt::mp("out"));

        // Match the size of the sync word
        d_sync = boost::circular_buffer<char>(sync.size());
        d_search = boost::circular_buffer<char>(sync.size());

        for(int i = 0; i < sync.size(); i++)
            d_sync.push_back(sync[i]);
    }

    /*
     * Our virtual destructor.
     */
    packet_deframer_impl::~packet_deframer_impl()
    {
    }

    void packet_deframer_impl::add_symbol(char symbol)
    {
        // Add bits onto buffer
        d_packet.push_back(symbol);

        if(!d_have_len)
        {
            d_pkt_len <<= 1;
            d_pkt_len |= symbol;

            if(d_packet.size() == 8)
            {
                d_pkt_len = (d_pkt_len + 1) * 8;
                d_have_len = true;
                d_packet.reserve(d_pkt_len);
            }
        }
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
              add_symbol(in[i]);

              // Once we hit the end
              if(d_have_len && (d_packet.size() >= d_pkt_len))
              {
                  d_in_sync = false;

                  // TODO: This is stupid, how do I get full unix time w/ microseconds? ._.
                  boost::posix_time::ptime pkt_time = boost::posix_time::microsec_clock::universal_time();
                  pmt::pmt_t pdu, meta, data;

                  meta = pmt::make_dict();
                  meta = pmt::dict_add(meta, pmt::mp("ts"), pmt::from_long(pkt_time.time_of_day().total_microseconds()));
                  meta = pmt::dict_add(meta, pmt::mp("name"), pmt::intern(d_name));

                  // FIXME: Does this not work in all cases?
                  data = pmt::make_blob((char *)&d_packet[0], d_packet.size());

                  pdu = pmt::cons(meta, data);
                  message_port_pub(pmt::mp("out"), pdu);
              }
          }
          else
          {
              // Shift new bit onto search pattern
              d_search.push_back(in[i]);

              // Check if we have sync.
              if(d_search == d_sync) {
                  // TODO: More efficiently merge remaining (?) samples into d_packet
                  d_in_sync = true;
                  d_packet.clear();

                  if(d_fixed_len)
                  {
                      d_packet.reserve(d_pkt_len);
                      d_have_len = true;
                  }
                  else
                  {
                      d_have_len = false;
                      d_pkt_len = 0;
                  }
              }
          }
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace reveng */
} /* namespace gr */

