#include <gtest/gtest.h>
#include "utils/StringUtils.h"

TEST(StringUtils, ReplaceAll) {
    EXPECT_EQ(Utils::StringUtils::replaceAll("a.b.c", ".", "/"), "a/b/c");
    EXPECT_EQ(Utils::StringUtils::replaceAll("hello", "l", "L"), "heLLo");
    EXPECT_EQ(Utils::StringUtils::replaceAll("x", "y", "z"), "x");
}

TEST(StringUtils, EndsWithAndStartsWith) {
    EXPECT_TRUE(Utils::StringUtils::endsWith("Hello.class", ".class"));
    EXPECT_FALSE(Utils::StringUtils::endsWith("Hello.java", ".class"));
    EXPECT_TRUE(Utils::StringUtils::startsWith("java/lang/Object", "java/"));
    EXPECT_FALSE(Utils::StringUtils::startsWith("java/lang/Object", "javax/"));
}

TEST(StringUtils, Trim) {
    EXPECT_EQ(Utils::StringUtils::trim("  abc  "), "abc");
    EXPECT_EQ(Utils::StringUtils::trimLeft("  x"), "x");
    EXPECT_EQ(Utils::StringUtils::trimRight("y  "), "y");
}

TEST(StringUtils, EqualsIgnoreCase) {
    EXPECT_TRUE(Utils::StringUtils::equalsIgnoreCase("Java", "java"));
    EXPECT_FALSE(Utils::StringUtils::equalsIgnoreCase("Java", "kotlin"));
}
