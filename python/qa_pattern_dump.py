#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2016 Mike Walters.
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

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import pmt
import reveng_swig as reveng

class qa_pattern_dump(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block ()
        self.dbg = blocks.message_debug()

    def tearDown(self):
        self.tb = None

    def get_matches(self, src_data, pattern, dump_len):
        src = blocks.vector_source_b(src_data)
        pd = reveng.pattern_dump(pattern, dump_len, "%[bits]", True, None, False)
        self.tb.connect(src, pd)
        self.tb.msg_connect(pd, "out", self.dbg, "store")
        self.tb.run()
        matches = [pmt.to_python(self.dbg.get_message(i))[1] for i in range(self.dbg.num_messages())]
        return matches

    def test_no_match(self):
        src_data = [1,0,1,0,1,0,1,1,0,1,1,1,0,0,0,0,0]
        pattern = [1,1,1,0,1]
        matches = self.get_matches(src_data, pattern, 5)
        self.assertEqual(len(matches), 0)

    def test_single_match(self):
        pattern = [1,0,1,0]
        payload = [1,1,0,1,1]
        src_data = [0]*3 + pattern + payload + [1]*3
        matches = self.get_matches(src_data, pattern, len(payload))
        self.assertEqual(len(matches), 1)
        self.assertEqual(matches[0], ''.join(map(str, payload)))

    def test_multiple_matches(self):
        pattern = [1,0,1,0]
        payloads = [
            [1,1,0,1,1],
            [1,1,1,0,0],
            [0,1,1,0,0]
        ]
        src_data = []
        for i in range(len(payloads)):
            src_data += [0]*3 + pattern + payloads[i] + [1]*3
        matches = self.get_matches(src_data, pattern, len(payloads[0]))
        self.assertEqual(len(matches), 3)
        for i in range(len(payloads)):
            self.assertEqual(matches[i], ''.join(map(str, payloads[i])))


if __name__ == '__main__':
    gr_unittest.run(qa_pattern_dump, "qa_pattern_dump.xml")
