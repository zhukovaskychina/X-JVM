#include <gtest/gtest.h>
#include <string>
#include "lang/ByteQueue.h"

static std::string bytes(const char* data, size_t n) {
    return std::string(data, n);
}

TEST(ByteQueue, PopU1) {
    Lang::ByteQueue q;
    q.pushStringChars(bytes("\x42", 1));
    EXPECT_EQ(q.size(), 1);
    EXPECT_EQ(q.popU1(), 0x42u);
    EXPECT_EQ(q.size(), 0);
}

TEST(ByteQueue, PopU2BigEndian) {
    Lang::ByteQueue q;
    q.pushStringChars(bytes("\x01\x02", 2));
    EXPECT_EQ(q.popU2(), 0x0102u);
}

TEST(ByteQueue, JavaClassMagic) {
    Lang::ByteQueue q;
    std::string magic;
    magic.push_back(static_cast<char>(0xCA));
    magic.push_back(static_cast<char>(0xFE));
    magic.push_back(static_cast<char>(0xBA));
    magic.push_back(static_cast<char>(0xBE));
    q.pushStringChars(magic);
    EXPECT_EQ(q.popU4(), 0xCAFEBABEu);
}
