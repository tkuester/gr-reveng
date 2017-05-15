#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2017 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

import random
import numpy
import pmt

from gnuradio import gr, gr_unittest
from gnuradio import blocks
try:
    import reveng_swig as reveng
except ImportError:
    import reveng

class qa_packet_deframer (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        ''' Test fixed length packet '''
        # set up fg
        sync = map(int, bin(0xd391)[2:].zfill(16))
        data = map(int, bin(0xdeadbeef)[2:].zfill(32))
        stream = ([0] * 30) + sync + data + ([0] * 30)

        src = blocks.vector_source_b(stream)
        test_blk = reveng.packet_deframer('boop', sync, True, len(data), 0, 0)
        sink = blocks.message_debug()

        self.tb.connect(src, test_blk)
        self.tb.msg_connect(test_blk, 'out', sink, 'store')
        self.tb.run()

        # check data
        rec_msg = pmt.to_python(sink.get_message(0))

        self.assertTrue(isinstance(rec_msg, tuple))
        self.assertTrue(len(rec_msg) == 2)

        (meta, bits) = rec_msg
        self.assertTrue(isinstance(meta, dict))
        self.assertTrue(isinstance(bits, numpy.ndarray))
        self.assertTrue(list(bits) == data)

    def test_002_t (self):
        '''
        Test variable length packet. Length byte straight after sync, no
        additional bytes.
        '''
        # set up fg
        sync = map(int, bin(0xd391)[2:].zfill(16))
        plen = map(int, bin(4)[2:].zfill(8))
        data = map(int, bin(0xdeadbeef)[2:].zfill(32))

        pkt = plen + data
        stream = ([0] * 30) + sync + pkt + ([0] * 30)

        src = blocks.vector_source_b(stream)
        test_blk = reveng.packet_deframer('boop', sync, False, 0, 0, 0)
        sink = blocks.message_debug()

        self.tb.connect(src, test_blk)
        self.tb.msg_connect(test_blk, 'out', sink, 'store')
        self.tb.run()

        # check data
        rec_msg = pmt.to_python(sink.get_message(0))

        self.assertTrue(isinstance(rec_msg, tuple))
        self.assertTrue(len(rec_msg) == 2)

        (meta, bits) = rec_msg
        self.assertTrue(isinstance(meta, dict))
        self.assertTrue(meta.get('name') == "boop")

        self.assertTrue(isinstance(bits, numpy.ndarray))
        self.assertTrue(list(bits) == pkt)

    def test_003_t (self):
        '''
        Test variable length packet. Two additional bytes for checksum
        after data packet
        '''
        # set up fg
        sync = map(int, bin(0xd391)[2:].zfill(16))
        plen = map(int, bin(4)[2:].zfill(8))
        data = map(int, bin(0xdeadbeef)[2:].zfill(32))
        csum = map(int, bin(0xa55a)[2:].zfill(16))

        pkt = plen + data + csum
        stream = ([0] * 30) + sync + pkt + ([0] * 30)

        src = blocks.vector_source_b(stream)
        test_blk = reveng.packet_deframer('boop', sync, False, 0, 0, 2)
        sink = blocks.message_debug()

        self.tb.connect(src, test_blk)
        self.tb.msg_connect(test_blk, 'out', sink, 'store')
        self.tb.run()

        # check data
        rec_msg = pmt.to_python(sink.get_message(0))

        self.assertTrue(isinstance(rec_msg, tuple))
        self.assertTrue(len(rec_msg) == 2)

        (meta, bits) = rec_msg
        self.assertTrue(isinstance(meta, dict))
        self.assertTrue(isinstance(bits, numpy.ndarray))
        self.assertTrue(list(bits) == pkt)

    def test_004_t (self):
        '''
        Test variable length packet. Length is indexed one byte after
        sync. Two additional bytes for checksum after data packet.
        '''
        # set up fg
        sync = map(int, bin(0xd391)[2:].zfill(16))
        txid = map(int, bin(0x0001)[2:].zfill(16))
        plen = map(int, bin(4)[2:].zfill(8))
        data = map(int, bin(0xdeadbeef)[2:].zfill(32))
        csum = map(int, bin(0xa55a)[2:].zfill(16))

        pkt = txid + plen + data + csum
        stream = ([0] * 30) + sync + pkt + ([0] * 30)

        src = blocks.vector_source_b(stream)
        test_blk = reveng.packet_deframer('boop', sync, False, 0, 2, 2)
        sink = blocks.message_debug()

        self.tb.connect(src, test_blk)
        self.tb.msg_connect(test_blk, 'out', sink, 'store')
        self.tb.run()

        # check data
        rec_msg = pmt.to_python(sink.get_message(0))

        self.assertTrue(isinstance(rec_msg, tuple))
        self.assertTrue(len(rec_msg) == 2)

        (meta, bits) = rec_msg
        self.assertTrue(isinstance(meta, dict))
        self.assertTrue(isinstance(bits, numpy.ndarray))
        self.assertTrue(list(bits) == pkt)

if __name__ == '__main__':
    gr_unittest.run(qa_packet_deframer, "qa_packet_deframer.xml")
