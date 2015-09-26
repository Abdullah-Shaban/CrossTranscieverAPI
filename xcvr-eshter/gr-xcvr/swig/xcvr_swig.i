/* -*- c++ -*- */

#define XCVR_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "xcvr_swig_doc.i"

%{
#include "xcvr/eshter.h"
%}


%include "xcvr/eshter.h"
GR_SWIG_BLOCK_MAGIC2(xcvr, eshter);
