#include <gtest/gtest.h>
#include "transmog/transmog.h"

#include <string>

TEST(transmog, banner)
{
  ASSERT_EQ("unknown",std::string(tmg_banner()));
}


TEST(transmog, foo)
{
    ASSERT_EQ(5-TRANSMOG_MAGIC, tmg_foo(3,2));
}


TEST(transmog, transmogrify)
{
    ASSERT_EQ(64+TRANSMOG_MAGIC, tmg_transmogrify(8));
}

