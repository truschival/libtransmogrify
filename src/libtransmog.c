/*****************************************************************************
 * \file
 * \brief	libtransmog
 *****************************************************************************/

#include <stdio.h>

#include "config.h"
#include "transmogrify.h"


/*****************************************************************************/
const char* tmg_banner()
{
    return CMAKE_PROJECT_VERSION;
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

/*****************************************************************************/
const char* tmg_git_revision()
{
    return GIT_DESCRIBE_REV;
}