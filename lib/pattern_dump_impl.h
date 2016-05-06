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

#ifndef INCLUDED_REVENG_PATTERN_DUMP_IMPL_H
#define INCLUDED_REVENG_PATTERN_DUMP_IMPL_H

#include <reveng/pattern_dump.h>

namespace gr {
  namespace reveng {

    class pattern_dump_impl : public pattern_dump
    {
     private:
      // Nothing to declare in this block.

     public:
      pattern_dump_impl(const std::vector<unsigned char> &pattern, unsigned int dump_len, const char *output_fmt, bool rel_time, const char *file_name, bool stdout);
      ~pattern_dump_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace reveng
} // namespace gr

#endif /* INCLUDED_REVENG_PATTERN_DUMP_IMPL_H */

