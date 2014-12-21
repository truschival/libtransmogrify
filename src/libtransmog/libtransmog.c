
/*****************************************************************************/
/**
 * \file
 * \brief	libtransmog
 */
/*****************************************************************************/

#include <stdio.h>

#include "transmog/config.h"
#include "transmog/transmog.h"


const char* tmg_banner() {
  return TRANSMOG_VERSION;
}

int tmg_transmogrify(int a) {
  return a * a + TRANSMOG_MAGIC;
}

int tmg_foo(int a, int b)
{
  return a + b - TRANSMOG_MAGIC;
}
