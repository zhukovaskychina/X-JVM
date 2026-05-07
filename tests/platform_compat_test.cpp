#include <gtest/gtest.h>

#include <boost/filesystem.hpp>
#include <cmath>
#include <fstream>
#include <string>

#include "utils/PlatformCompat.h"

namespace fs = boost::filesystem;

TEST(PlatformCompat, Split) {
    auto p = PlatformCompat::split("a;b;c", ";");
    ASSERT_EQ(p.size(), 3u);
    EXPECT_EQ(p[0], "a");
    EXPECT_EQ(p[1], "b");
    EXPECT_EQ(p[2], "c");
}

TEST(PlatformCompat, SplitEmptyDelimiter) {
    auto p = PlatformCompat::split("abc", "");
    ASSERT_EQ(p.size(), 1u);
    EXPECT_EQ(p[0], "abc");
}

TEST(PlatformCompat, ReadFileContent) {
    fs::path p = fs::temp_directory_path() / "xjvm_readfile_test.txt";
    {
        std::ofstream out(p.string(), std::ios::binary);
        ASSERT_TRUE(out.is_open());
        out << "hello";
    }
    std::string content = PlatformCompat::readFileContent(p.string());
    fs::remove(p);
    EXPECT_EQ(content, "hello");
}

TEST(PlatformCompat, SafeFmod) {
    EXPECT_DOUBLE_EQ(PlatformCompat::safeFmod(5.0, 2.0), 1.0);
    EXPECT_TRUE(std::isnan(PlatformCompat::safeFmod(1.0, 0.0)));
}
