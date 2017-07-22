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
       * The Packet Deframer is a block which takes in a bit stream, and
       * searches for "burst style" packets which start with a "sync word".
       * When the block finds the sync word within the bit stream, it gathers
       * the subsequent bits, timestamps the packet, and pushes them out as a
       * message PDU. Subsequently, you can create your own block to parse the
       * data, or use the reveng Packet Formatter for quick-and-dirty work.
       *
       * This block can handle a wide range of packet types and
       * configurations, including fixed length and variable legnth packets.
       * It also allows you to output PDUs a sequence of bits, or a sequence
       * of bytes, useful when packets do not adhere to byte boundaries.
       *
       * A few common configurations are listed below.
       *
       * The simplest format is a fixed length packet. In this scenario, the
       * radio transmits the sync word, followed by four bytes of data.
       *
       * preamble = [0x55, 0x55, 0x55, 0x55]
       * sync = [0xd3, 0x91]
       * data = [0x00, 0x01, 0x02, 0x03]
       *
       * For this configuration, set mode to "Fixed Length". Packet length
       * will be 32 bits (4 * 8). If you set "pack bytes" in the data format
       * tab, you will receive [0x00, 0x01, 0x02, 0x03] in the PDU.
       *
       * A variable length packet usually includes a length byte, which
       * indicates the size of the payload. To complicate matter, the length
       * byte may not include fixed length fields (such as a checksum, or
       * header). Consider the following packet:
       *
       * preamble = [0x55, 0x55, 0x55, 0x56]
       * sync = [0xd3, 0x91]
       * header = [0xca, 0xfe]
       * len = [0x04]
       * data = [0x00, 0x01, 0x02, 0x03]
       * csum = [0xd3]
       *
       * For this scenario, set mode to "Variable Length". Since there is a
       * two byte header, we will set the "Len Offset" to 2. The length does
       * not account for the checksum, so we will set "Additional Bytes" to 1.
       * (We have already captured the header, and do not need to account for
       * it.) For now, "Max Len" can be left at 0. This will output the
       * header, length, data, and checksum in the PDU.
       *
       * The "Max Length" setting is useful in squelching variable length
       * packets corrupted by noise. For example, a radio may never transmit
       * a message greater than 16 bytes. However, a burst of noise may
       * corrupt the length during transmission, and trick the receiver into
       * receiving 255 bytes. Setting "Max Length" to 16 will cause this block
       * to dump the packet prematurely, and resume search for a sync word on
       * the next bit. To disable this feature, set it to 0.
       *
       * In the future, there are plans to support different encoding schemes
       * (ie: manchester), endianness, and bit ordering. Until then, however,
       * these fields are just placeholders.
       */
      static sptr make(const std::string &name, const std::vector<char> &sync,
              bool fixed_len, int pkt_len, int max_len, int pkt_len_offset, int pkt_len_adj, bool pack_bytes);
    };

  } // namespace reveng
} // namespace gr

#endif /* INCLUDED_REVENG_PACKET_DEFRAMER_H */

