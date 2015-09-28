/* -*- c++ -*- */

#define XCVR_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "xcvr_swig_doc.i"

%{
#include "xcvr/ImecSensingEngine.h"
%}


%include "xcvr/ImecSensingEngine.h"
GR_SWIG_BLOCK_MAGIC2(xcvr, ImecSensingEngine);
