#include <gtest/gtest.h>
#include "transmogrify.h"
#include "config.h"

#include <string>

TEST(transmog, banner)
{
  ASSERT_GT(std::string(tmg_banner()).length(),0);
  ASSERT_EQ(std::string(tmg_banner()), CMAKE_PROJECT_VERSION);
}


TEST(transmog, foo)
{
    ASSERT_EQ(5-TRANSMOG_MAGIC, tmg_foo(3,2));
}


TEST(transmog, transmogrify)
{
    ASSERT_EQ(64+TRANSMOG_MAGIC, tmg_transmogrify(8));
}
