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

import time
import sys
import traceback
import re
import numpy
import pmt
from gnuradio import gr

class packet_formatter(gr.basic_block):
    '''
    Simple block to format a PDU message to the screen.

    Output Fmt Variables:
     - TODO: Any python datetime format string (ie: %H:%M:%S)
     - %(name)  - The name from the message
     - %(bits)  - (ie: 10010110)
     - %(hex)   - (ie: [0x55, 0x3f, ...]
     - %(ascii) - (ie: \\xfeHello!)
     - TODO: %[man-bits] Manchester encoded bits
     - TODO: %[pwm-bits] 100 = 1, 110 = 0
     - TODO: %[hdlcd] HDLC payload data, un-zero-stuffed and bit reversed, in hex

    Format variables can be addressed. For example:
       %(bits[3])
       %(hex[0:32])
    '''
    def __init__(self, format_str, file_name=None, log_to='stdout', append=False, flush=True):
        gr.basic_block.__init__(self,
            name="packet_formatter",
            in_sig=None,
            out_sig=None)

        self.message_port_register_in(pmt.intern('pdus'))
        self.set_msg_handler(pmt.intern('pdus'), self.handle_pdu)

        self.format_str = format_str.replace('\\n', '\n')
        self.file_name = file_name
        self.fp = None
        self.flush = flush

        self.stdout = 'stdout' in log_to
        if log_to in ['file', 'stdout_file']:
            try:
                self.fp = open(file_name, 'a' if append else 'w')
            except (OSError, IOError):
                print >> sys.stderr, "gr-reveng: Unable to open '%s' for writing" % file_name

    def stop(self):
        if not self.fp:
            return

        print >> sys.stderr, "gr-reveng: Closing %s" % (self.file_name)
        try:
            self.fp.close()
        except (OSError, IOError):
            pass
        self.fp = None

        return True

    def handle_pdu(self, pdu):
        if not pmt.is_pair(pdu):
            return

        meta = pmt.to_python(pmt.car(pdu)) or {}
        bits = pmt.to_python(pmt.cdr(pdu))

        if meta.get('packed', False):
            bits = numpy.unpackbits(bytearray(bits))

        try:
            output = self.format_output(meta, bits)
            if self.stdout:
                print output
            if self.fp:
                self.fp.write(output)
                self.fp.write('\n')
                if self.flush:
                    self.fp.flush()
        except IndexError as e:
            print >> sys.stderr, "gr-reveng: IndexError: Ran out of bits?"
            print >> sys.stderr, traceback.format_exc(e)
        except ValueError as e:
            print >> sys.stderr, "gr-reveng: TypeError: Something casted wrong!"
            print >> sys.stderr, traceback.format_exc(e)
        except IOError as e:
            print >> sys.stderr, "gr-reveng: IOError: Unable to write to file, closing"
            try:
                self.fp.close()
            except (OSError, IOError):
                pass
            self.fp = None
        except StandardError as e:
            print >> sys.stderr, "gr-reveng: %s: Something went horribly wrong!" % type(e)
            print >> sys.stderr, traceback.format_exc(e)

    def format_output(self, meta, bits):
        '''
        if self.rel_time == None:
            out = self.format_str
        elif self.rel_time == True:
            diff = datetime.now() - self.start_time
            time_str = '%d.%06d' % (diff.seconds, diff.microseconds)
            out = self.output_fmt.replace('%s', time_str)
        else:
            out = datetime.now().strftime(self.output_fmt)
        '''

        out = self.format_str

        # Search for %(what[start:stop])
        input_str = out # Probs shouldn't change what we're re.finditer'ing
        for match in re.finditer(r'%\(([^\)]+)\)', input_str):
            to_replace = match.group()

            what = match.groups()[0].rstrip(']').split('[')
            tipe = what[0]

            # If we have a second part, it contains [start:stop], [idx], or [start:]
            if len(what) == 2:
                substr = what[1].split(':')
                if len(substr) == 1:
                    start = int(substr[0])
                    stop = int(substr[0]) + 1
                else:
                    start = int(substr[0])
                    if substr[1] == "":
                        stop = None
                    else:
                        stop = int(substr[1])
            else:
                start = None
                stop = None

            sub_bits = bits[start:stop]
            l_bites = numpy.packbits(bytearray(sub_bits))
            r_bites = numpy.packbits(bytearray([0] * (8 - (len(sub_bits) % 8) + sub_bits)))

            if tipe == 'name':
                tmp = meta.get('name', "None")
            elif tipe == 'ts':
                tmp = '%.6f' % meta.get('ts', -1)
            elif tipe == 'bits':
                tmp = ''.join(map(str, sub_bits))
            elif tipe == 'hex':
                tmp = ''.join(['%02x' % byte for byte in l_bites])
            elif tipe == 'int':
                tmp = str(int(''.join(map(str, sub_bits)), 2))
            elif tipe == 'ascii':
                tmp = repr(''.join([chr(c) for c in l_bites]))[1:-1]
            elif tipe == 'man-bits':
                continue
                tmp = man_decode(sub_bits)
            elif tipe == 'pwm-bits':
                continue
                tmp = pwm_decode(sub_bits)
            elif tipe == 'hdlcd':
                continue
                tmp = hdlc_data_decode(sub_bits)
            else:
                # Unknown type
                continue

            out = out.replace(to_replace, tmp, 1)

        return out
