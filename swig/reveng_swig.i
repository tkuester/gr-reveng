/* -*- c++ -*- */

#define REVENG_API

%include "gnuradio.i"            // the common stuff

//load generated python docstrings
%include "reveng_swig_doc.i"

%{
#include "reveng/packet_deframer.h"
%}

%include "reveng/packet_deframer.h"
GR_SWIG_BLOCK_MAGIC2(reveng, packet_deframer);
