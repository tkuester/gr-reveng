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
#include <sys/time.h>

#include <boost/circular_buffer.hpp>
#include <gnuradio/io_signature.h>
#include "packet_deframer_impl.h"

using namespace std;

namespace gr {
  namespace reveng {

    packet_deframer::sptr
    packet_deframer::make(const std::string &name, const std::vector<char> &sync, bool fixed_len,
            int pkt_len, int max_len, int pkt_len_offset, int pkt_len_adj, bool pack_bytes)
    {
      return gnuradio::get_initial_sptr
        (new packet_deframer_impl(name, sync, fixed_len,
                                  pkt_len, max_len, pkt_len_offset, pkt_len_adj, pack_bytes));
    }

    /*
     * The private constructor
     */
    packet_deframer_impl::packet_deframer_impl(const std::string &name, const std::vector<char> &sync,
            bool fixed_len, int pkt_len, int max_len, int pkt_len_offset, int pkt_len_adj, bool pack_bytes)
      : gr::sync_block("packet_deframer",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(0, 0, 0)),
      d_name(name),
      d_pack_bytes(pack_bytes),
      d_fixed_len(fixed_len),
      d_pkt_len(pkt_len),
      d_max_len(max_len),
      d_pkt_len_adj(pkt_len_adj),
      d_in_sync(false)
    {
        message_port_register_out(pmt::mp("out"));

        // Match the size of the sync word
        d_sync = boost::circular_buffer<char>(sync.size());
        d_search = boost::circular_buffer<char>(sync.size());
        d_pkt_len_offset = pkt_len_offset * 8;

        for(int i = 0; i < sync.size(); i++)
            d_sync.push_back(sync[i]);
    }

    /*
     * Our virtual destructor.
     */
    packet_deframer_impl::~packet_deframer_impl()
    {
    }

    // FIXME: Indicate what's counting bytes, what's counting bits

    void packet_deframer_impl::add_symbol(char symbol)
    {
        d_rx_bit_cnt += 1;
        // Add bits onto buffer
        if(d_pack_bytes)
        {
            d_tmp_byte = (d_tmp_byte << 1) | (symbol & 0x01);
            if((d_rx_bit_cnt % 8) == 0)
            {
                d_packet.push_back(d_tmp_byte);
                d_tmp_byte = 0;
            }
        }
        else
        {
            d_packet.push_back(symbol);
        }

        // If we don't have the length
        if(!d_have_len)
        {
            // Wait until we're at the beginning of the length byte
            if(d_rx_bit_cnt > d_pkt_len_offset)
            {
                // Shift onto the length
                d_pkt_len <<= 1;
                d_pkt_len |= symbol;

                // When we have the full length byte
                // TODO: Allow user to specify number of bits in length byte
                if(d_rx_bit_cnt == (d_pkt_len_offset + 8))
                {
                    if(d_max_len > 0 && d_pkt_len > d_max_len)
                    {
                        d_in_sync = false;
                    }

                    // Account for the bits we've captured so far,
                    // plus the remaining length
                    d_pkt_len = d_rx_bit_cnt + (d_pkt_len * 8) + (d_pkt_len_adj * 8);
                    d_have_len = true;
                    if(d_pack_bytes)
                    {
                        d_packet.reserve(d_pkt_len / 8);
                    }
                    else
                    {
                        d_packet.reserve(d_pkt_len);
                    }
                }
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
              if(d_have_len && (d_rx_bit_cnt >= d_pkt_len))
              {
                  d_in_sync = false;

                  boost::posix_time::ptime pkt_time = boost::posix_time::microsec_clock::universal_time();
                  pmt::pmt_t pdu, meta, data;
                  uint8_t *data_buff;
                  size_t o0;
                  struct timeval tv;
                  double ts;

                  meta = pmt::make_dict();
                  meta = pmt::dict_add(meta, pmt::mp("name"), pmt::intern(d_name));
                  meta = pmt::dict_add(meta, pmt::mp("packed"), pmt::from_bool(d_pack_bytes));

                  // TODO: Pull the timestamp from the stream tags, if possible.
                  // Real time doesn't necessarily correspond to when we rx the samp buffer
                  if(gettimeofday(&tv, NULL) == 0) {
                      meta = pmt::dict_add(meta, pmt::mp("tv_sec"), pmt::from_long(tv.tv_sec));
                      meta = pmt::dict_add(meta, pmt::mp("tv_usec"), pmt::from_long(tv.tv_usec));
                  } else {
                      meta = pmt::dict_add(meta, pmt::mp("tv_sec"), pmt::from_long(-1));
                      meta = pmt::dict_add(meta, pmt::mp("tv_usec"), pmt::from_long(-1));
                  }

                  // FIXME: Does this not work in all cases?
                  data = pmt::make_u8vector(d_packet.size(), 0x00);
                  data_buff = pmt::u8vector_writable_elements(data, o0);
                  memcpy(data_buff, &d_packet[0], sizeof(uint8_t) * d_packet.size());

                  //data = pmt::make_blob((char *)&d_packet[0], d_packet.size());

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
                  d_rx_bit_cnt = 0;
                  d_tmp_byte = 0;
                  d_packet.clear();

                  if(d_fixed_len)
                  {
                      if(d_pack_bytes)
                      {
                          d_packet.reserve(d_pkt_len / 8);
                      }
                      else
                      {
                          d_packet.reserve(d_pkt_len);
                      }
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

