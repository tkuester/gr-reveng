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

#ifndef INCLUDED_REVENG_PACKET_DEFRAMER_IMPL_H
#define INCLUDED_REVENG_PACKET_DEFRAMER_IMPL_H

#include <reveng/packet_deframer.h>

namespace gr {
  namespace reveng {

    class packet_deframer_impl : public packet_deframer
    {
     private:
      const std::string d_name;

      // Circular buffers to hold sync word and search pattern
      // NOTE: int64_t would be more efficient, but wouldn't let us (easily)
      // search for symbols, and limits us to 64 bits. The trade off I suppose
      // is efficiency in comparing d_search to d_sync
      boost::circular_buffer<char> d_search;
      boost::circular_buffer<char> d_sync;
      bool d_in_sync;

      std::vector<char> d_packet;
      int d_pkt_len;
      int d_pkt_idx;

     public:
      packet_deframer_impl(const std::string &name, const std::vector<char> &sync, int pkt_len);
      ~packet_deframer_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace reveng
} // namespace gr

#endif /* INCLUDED_REVENG_PACKET_DEFRAMER_IMPL_H */

