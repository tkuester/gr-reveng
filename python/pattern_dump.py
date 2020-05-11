#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2014 <+YOU OR YOUR COMPANY+>.
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
import re
from datetime import datetime
import numpy
from gnuradio import gr
import pmt
from collections import deque

class pattern_dump(gr.sync_block):
    '''
    THIS BLOCK HAS BEEN DEPRECATED!

    Simple block to detect a pattern, and dump the next N bits for debug
    purposes.

    Often times, a packet structure will contain a fixed length "access code"
    or unique identifier that may be of interest. While it is possible to
    search through a dumped file, problems occur with bit and byte alignment.
    For example, [0x55, 0xaa] may be recorded as [0xab, 0x54] because of a bit
    shift.

    This block searches through the incoming bitstream for a given pattern.
    Upon finding it, the block will output the next N bits.

    Input:
     - char in the range of 0 or 1

    Config:
     - pattern: The pattern to search for
     - dump_len: The number of bits to analyze afterwards
     - output_fmt: How to print the message

    Output Fmt Variables:
     - Any python datetime format string (ie: %H:%M:%S)
     - %[bits] (ie: 10010110)
     - %[hex] (ie: [0x55, 0x3f, ...]
     - %[ascii] (ie: \\xfeHello!)
     - %[man-bits] Manchester encoded bits
     - %[pwm-bits] 100 = 1, 110 = 0
     - %[hdlcd] HDLC payload data, un-zero-stuffed and bit reversed, in hex

    Example: You are trying to capture a packet burst with the following
    format - [0x55, 0x55, 0x3e] + 14 bytes of manchester encoded data. You
    want to analyze two hours of captured traffic in a spreadsheet.

    We will need to account for Manchester encoding in the dump_len parameter,
    since it doubles the data length. Keep in mind that a single bit error in
    the preamble and/or access code will cause the entire packet to be
    silently dropped.

     - pattern: [0, 1] * 16 + [0, 0, 1, 0, 1, 1, 1, 0]
     - dump_len: 224 (ie: 14 bytes * 8 bits / byte * 2)
     - output_fmt: %s,%[man-bits]
     - rel_time: True

    Notes:
    The only time format for relative time is %s, and represents the number of
    seconds since the start of processing

    This is a python implementation, and may not preform well for high bit
    rates.
    '''

    def __init__(self, pattern, dump_len, output_fmt, rel_time=True,
            file_name=None, stdout=True):
        gr.sync_block.__init__(self,
            name="pattern_dump",
            in_sig=[numpy.byte],
            out_sig=None)

        # Using python's neat "deque" object. If you set maxlen, it acts
        # like a shift register.
        self.pattern = deque(pattern, maxlen=len(pattern))
        self.pattern_check = deque(maxlen=len(pattern))
        self.output = deque(maxlen=dump_len)
        self.count = 0

        self.output_fmt = output_fmt
        self.rel_time = rel_time

        self.fp = None
        self.file_name = file_name
        self.stdout = stdout

        if self.file_name:
            try:
                self.fp = open(self.file_name, 'w')
                print('gr-reveng: Opened %s for output' % file_name)
            except (OSError, IOError) as e:
                print("Couldn't open file for writing")
                print(str(e))

        self.start_time = datetime.now()

        self.message_port_register_out(pmt.intern('out'))

    def work(self, input_items, output_items):
        in0 = input_items[0]
        for bit in in0:
            if self.pattern_check == self.pattern:
                self.output.append(bit)

                if len(self.output) == self.output.maxlen:
                    output = list(self.output)
                    outs = ''.join(['1' if ch == 1 else '0' for ch in output])
                    pmt_out = (None, outs)
                    self.message_port_pub(pmt.intern('out'), pmt.to_pmt(pmt_out))
                    output = self.format_output(output)

                    if self.stdout:
                        print(output)

                    if self.fp:
                        try:
                            self.fp.write(output + '\n')
                            self.fp.flush()
                        except IOError as e:
                            print('Error while writing to file: %s' % str(e))
                            self.fp.close()
                            self.fp = None

                    self.output.clear()
                    self.pattern_check.clear()
            else:
                self.pattern_check.append(bit)

        return len(input_items[0])

    def stop(self):
        if self.fp:
            print("gr-reveng: Closing %s" % (self.file_name))
            self.fp.close()
            self.fp = None

    def format_output(self, bitarray):
        if self.rel_time == None:
            out = self.output_fmt
        elif self.rel_time == True:
            diff = datetime.now() - self.start_time
            time_str = '%d.%06d' % (diff.seconds, diff.microseconds)
            out = self.output_fmt.replace('%s', time_str)
        else:
            out = datetime.now().strftime(self.output_fmt)

        bites = numpy.packbits(bytearray(bitarray))
        if '%[bits]' in out:
            tmp = ''.join([str(bit) for bit in bitarray])
            out = out.replace('%[bits]', tmp)
        if '%[hex]' in out:
            tmp = ''.join([hex(byte)[2:].zfill(2) for byte in bites])
            out = out.replace('%[hex]', tmp)
        if '%[ascii]' in out:
            tmp = repr(''.join([chr(c) for c in bites]))[1:-1]
            out = out.replace('%[ascii]', tmp)
        if '%[man-bits]' in out:
            out = out.replace('%[man-bits]', man_decode(bitarray))
        if '%[pwm-bits]' in out:
            out = out.replace('%[pwm-bits]', pwm_decode(bitarray))
        if '%[hdlcd]' in out:
            out = out.replace('%[hdlcd]', hdlc_data_decode(bitarray))

        return out

def pwm_decode(bitarray):
    out = ''
    for i in xrange(0, len(bitarray)-1, 3):
        bits = bitarray[i:(i+3)]
        if bits == [1, 0, 0]:
            out += '1'
        elif bits == [1, 1, 0]:
            out += '0'
        else:
            out += 'x'

    return out

def man_decode(bitarray):
    # TODO: Add differential, polarity options
    out = ''
    for i in xrange(0, len(bitarray)-1, 2):
        bits = bitarray[i:(i+2)]
        if bits == [0, 1]:
            out += '0'
        elif bits == [1, 0]:
            out += '1'
        else:
            out += 'x'

    return out

def hdlc_data_decode(bitarray):
    out = ''
    obyte = 0
    bits = 0
    ones = 0
    for i in xrange(0, len(bitarray), 1):
        if bitarray[i] == 0:
            if ones != 5:
                bits += 1
            ones = 0
        else:
            obyte |= (1 << bits)
            bits += 1
            ones += 1
            # TODO: ignoring too many ones in a row, for now.
            # But we really ought to handle the HDLC 0x7e flag properly.
        if bits == 8:
            out += hex(obyte)[2:].zfill(2)
            obyte = 0
            bits = 0
    return out
