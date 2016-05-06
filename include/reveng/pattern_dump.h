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


#ifndef INCLUDED_REVENG_PATTERN_DUMP_H
#define INCLUDED_REVENG_PATTERN_DUMP_H

#include <reveng/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace reveng {

    /*!
     * \brief <+description of block+>
     * \ingroup reveng
     *
     */
    class REVENG_API pattern_dump : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<pattern_dump> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of reveng::pattern_dump.
       *
       * To avoid accidental use of raw pointers, reveng::pattern_dump's
       * constructor is in a private implementation
       * class. reveng::pattern_dump::make is the public interface for
       * creating new instances.
       */
      static sptr make(const std::vector<unsigned char> &pattern, unsigned int dump_len, const char *output_fmt, bool rel_time = true, const char *file_name = 0, bool stdout = true);
    };

  } // namespace reveng
} // namespace gr

#endif /* INCLUDED_REVENG_PATTERN_DUMP_H */

