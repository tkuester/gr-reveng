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
from repeat_detector import repeat_detector

class qa_repeat_detector (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001 (self):
        # just check it can run
        rd = repeat_detector()
        rd.handler(pmt.to_pmt((None, "1010")))
        rd.handler(pmt.to_pmt((None, "1010")))


if __name__ == '__main__':
    gr_unittest.run(qa_repeat_detector, "qa_repeat_detector.xml")
