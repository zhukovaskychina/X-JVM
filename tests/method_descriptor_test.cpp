#include <gtest/gtest.h>
#include <list>
#include <string>
#include "runtime/heap/MethodDescriptor.h"

using Runtime::Heap::MethodDescriptorParser;

TEST(MethodDescriptorParser, VoidNoArgs) {
    MethodDescriptorParser parser;
    auto* md = parser.parse("()V");
    ASSERT_NE(md, nullptr);
    EXPECT_EQ(md->getReturnType(), "V");
    EXPECT_TRUE(md->getParameterType().empty());
    delete md;
}

TEST(MethodDescriptorParser, IntReturnNoArgs) {
    MethodDescriptorParser parser;
    auto* md = parser.parse("()I");
    ASSERT_NE(md, nullptr);
    EXPECT_EQ(md->getReturnType(), "I");
    EXPECT_TRUE(md->getParameterType().empty());
    delete md;
}

TEST(MethodDescriptorParser, GetParameterTypesRoundTripShape) {
    MethodDescriptorParser parser;
    auto* md = parser.parse("()V");
    ASSERT_NE(md, nullptr);
    EXPECT_EQ(md->getParameterTypes(), "()");
    delete md;
}

TEST(MethodDescriptorParser, IntStringArrayParams) {
    MethodDescriptorParser parser;
    auto* md = parser.parse("(ILjava/lang/String;[I)V");
    ASSERT_NE(md, nullptr);
    EXPECT_EQ(md->getReturnType(), "V");
    const auto& p = md->getParameterType();
    ASSERT_EQ(p.size(), 3u);
    auto it = p.begin();
    EXPECT_EQ(*it++, "I");
    EXPECT_EQ(*it++, "Ljava/lang/String;");
    EXPECT_EQ(*it++, "[I");
    EXPECT_EQ(md->getParameterTypes(), "(ILjava/lang/String;[I)");
    delete md;
}

TEST(MethodDescriptorParser, InvalidVoidParameter) {
    MethodDescriptorParser parser;
    EXPECT_EQ(parser.parse("(V)V"), nullptr);
}

TEST(MethodDescriptorParser, InvalidMissingSemicolonOnClass) {
    MethodDescriptorParser parser;
    EXPECT_EQ(parser.parse("(Lno/semi)V"), nullptr);
}
