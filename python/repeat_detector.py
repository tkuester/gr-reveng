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

import numpy
from gnuradio import gr
import pmt
import sys

class repeat_detector(gr.sync_block):
    """
    Take PDUs as input and print them to stdout.
    If the current PDU matches the previous one then print '.' instead.
    """
    def __init__(self):
        gr.sync_block.__init__(self,
            name="repeat_detector",
            in_sig=None,
            out_sig=None)
        self.message_port_register_in(pmt.intern("pdus"))
        self.set_msg_handler(pmt.intern("pdus"), self.handler)
        self.prev_msg = None

    def handler(self, pdu):
        msg = pmt.to_python(pdu)[1]
        if self.prev_msg != msg:
            sys.stdout.write("\n" + msg + ' ')
            self.prev_msg = msg
        else:
            sys.stdout.write('.')
        sys.stdout.flush()


    def work(self, input_items, output_items):
        pass

