#pragma once

#if !defined(MLISP_NOINCLUDE)
    #include "mpc.h"
#endif


mpc_parser_t* parser_get();

void parser_cleanup();
