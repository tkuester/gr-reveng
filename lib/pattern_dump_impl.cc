/* -*- c++ -*- */
/* 
 * Copyright 2014-2015 tkuester.
 * Copyright 2016 Mike Walters.
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
#include "pattern_dump_impl.h"

namespace gr {
  namespace reveng {

    pattern_dump::sptr
    pattern_dump::make(const std::vector<unsigned char> &pattern, unsigned int dump_len, const char *output_fmt, bool rel_time, const char *file_name, bool stdout)
    {
      return gnuradio::get_initial_sptr
        (new pattern_dump_impl(pattern, dump_len, output_fmt, rel_time, file_name, stdout));
    }

    /*
     * The private constructor
     */
    pattern_dump_impl::pattern_dump_impl(const std::vector<unsigned char> &pattern, unsigned int dump_len, const char *output_fmt, bool rel_time, const char *file_name, bool stdout)
      : gr::sync_block("pattern_dump",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(0, 0, 0))
    {}

    /*
     * Our virtual destructor.
     */
    pattern_dump_impl::~pattern_dump_impl()
    {
    }

    int
    pattern_dump_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];

      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace reveng */
} /* namespace gr */

