/*************************************************************************************
 * \filename
 * \brief transmog implementation
 *  
 * \details
 * \copyright (c) 2014 Thomas Ruschival <thomas@ruschival.de>
 * \author ruschi
 * 
 * \since  
 *
 *************************************************************************************/

#include <stdio.h>

#include "config.h"
#include "transmog.h"


/*****************************************************************************/
const char* tmg_banner()
{
    return TRANSMOG_VERSION;
}

/*****************************************************************************/
int tmg_transmogrify(int a)
{
    return a * a + TRANSMOG_MAGIC;
}

/*****************************************************************************/
int tmg_foo(int a, int b)
{
    return a + b - TRANSMOG_MAGIC;
}
