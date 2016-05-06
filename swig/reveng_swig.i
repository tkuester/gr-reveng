/* -*- c++ -*- */

#define REVENG_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "reveng_swig_doc.i"

%{
#include "reveng/pattern_dump.h"
%}


%include "reveng/pattern_dump.h"
GR_SWIG_BLOCK_MAGIC2(reveng, pattern_dump);
