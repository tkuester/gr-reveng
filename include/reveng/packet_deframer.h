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


#ifndef INCLUDED_REVENG_PACKET_DEFRAMER_H
#define INCLUDED_REVENG_PACKET_DEFRAMER_H

#include <reveng/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace reveng {

    /*!
     * \brief Sink that deframes packets from a byte stream into a message
     * \ingroup reveng
     *
     * \details
     * Block details go here!
     *
     */
    class REVENG_API packet_deframer : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<packet_deframer> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of reveng::packet_deframer.
       *
       * To avoid accidental use of raw pointers, reveng::packet_deframer's
       * constructor is in a private implementation
       * class. reveng::packet_deframer::make is the public interface for
       * creating new instances.
       */
      static sptr make(const std::string &name, const std::vector<char> &sync, int pkt_len);
    };

  } // namespace reveng
} // namespace gr

#endif /* INCLUDED_REVENG_PACKET_DEFRAMER_H */

