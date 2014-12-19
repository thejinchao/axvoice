%module(directors="1") axvoice

%{
#include "AV_Interface.h"
%}

%include "std_string.i"

/* turn on director wrapping Callback */

%feature("director") AxVoiceMessage;
%feature("director") AxVoiceCallback;

%include "AV_Interface.h"
