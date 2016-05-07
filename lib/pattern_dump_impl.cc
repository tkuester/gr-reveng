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

#include <boost/algorithm/string/replace.hpp>
#include <ctime>
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
              gr::io_signature::make(0, 0, 0)),
        d_output_fmt(output_fmt),
        d_rel_time(rel_time),
        d_stdout(stdout),
        d_pattern(pattern.size()),
        d_pattern_check(pattern.size()),
        d_output(dump_len),
        d_pattern_check_len(0),
        d_output_len(0)
    {
      // Copy pattern vector to bitset
      int i = pattern.size() - 1;
      for (auto bit : pattern)
        d_pattern[i--] = bit;

      message_port_register_out(port_id);
    }

    /*
     * Our virtual destructor.
     */
    pattern_dump_impl::~pattern_dump_impl()
    {
    }

    bool
    pattern_dump_impl::start()
    {
      d_start_time = std::chrono::steady_clock::now();
    }

    int
    pattern_dump_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];

      for (int i = 0; i < noutput_items; i++) {
        bool bit = in[i];
        if (d_pattern_check_len == d_pattern_check.size() && d_pattern_check == d_pattern) {
          shift_bit(d_output, bit);
          d_output_len++;

          if (d_output_len == d_output.size()) {
            auto msg = pmt::cons(pmt::PMT_NIL, pmt::intern(get_output_bit_string()));
            message_port_pub(port_id, msg);

            if (d_stdout)
              std::cout << format_output() << std::endl;

            d_output_len = 0;
            d_pattern_check_len = 0;
          }

        } else {
          shift_bit(d_pattern_check, bit);
          d_pattern_check_len = std::min(d_pattern_check_len + 1, (int)d_pattern_check.size());
        }
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    std::string
    pattern_dump_impl::get_output_bit_string()
    {
      std::string out;
      boost::to_string(d_output, out);
      return out;
    }

    std::string
    pattern_dump_impl::format_output()
    {
      std::time_t t = std::time(nullptr);
      std::string out;
      if (d_rel_time) {
        out = d_output_fmt;
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = now - d_start_time;
        auto rel_time = elapsed.count();
        boost::replace_all(out, "%s", std::to_string(rel_time));
      } else {
        char buf[512];
        std::strftime(buf, sizeof(buf), d_output_fmt.c_str(), std::localtime(&t));
        out = std::string(buf);
      }
      boost::replace_all(out, "%[bits]", get_output_bit_string());
      return out;
    }

    void
    pattern_dump_impl::shift_bit(boost::dynamic_bitset<> &bitset, bool bit)
    {
      bitset <<= 1;
      bitset[0] = bit;
    }

  } /* namespace reveng */
} /* namespace gr */

