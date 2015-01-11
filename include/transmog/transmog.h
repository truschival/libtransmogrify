/**
 * \file
 * \brief Project definitions
 * \copyright (c) 2014 Thomas Ruschival <thomas@ruschival.de>
 *
 */

#ifndef __transmog_H
#define __transmog_H

#ifdef __cplusplus
#include <cstdint>
#include <cstddef>
extern "C" {
#else
#include <stdint.h>
#include <stddef.h>
#endif

/**
 * \brief Some function
 * \param a
 * \param b
 * \return a+b
 */
int tmg_foo(int a, int b);

/* a^2 */
int tmg_transmogrify(int a);

/* return the version string */
const char* tmg_banner();

#ifdef __cplusplus
} /* extern "C" */
#endif

#ifndef TRANSMOG_MAGIC
#define TRANSMOG_MAGIC (17)
#endif

#endif /* __transmog_H */












