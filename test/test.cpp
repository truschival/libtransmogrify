#include <gtest/gtest.h>

#include "transmog/transmog.h"
#include "transmog/version.h"

#include <string>

TEST(Version, PrintHash)
{
	RecordProperty("SHA1", TRANSMOG_GIT_COMMIT_HASH);
	SUCCEED();
}

TEST(transmog, banner)
{
	ASSERT_EQ(TRANSMOG_VERSION, std::string(tmg_banner()));
}

TEST(transmog, foo)
{
	ASSERT_EQ(5 - TRANSMOG_MAGIC, tmg_foo(3, 2));
}

TEST(transmog, transmogrify)
{
	ASSERT_EQ(64 + TRANSMOG_MAGIC, tmg_transmogrify(8));
}
