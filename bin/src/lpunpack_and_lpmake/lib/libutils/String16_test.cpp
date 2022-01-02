/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <utils/String16.h>
#include <utils/String8.h>

#include <gtest/gtest.h>

namespace android {

::testing::AssertionResult Char16_tStringEquals(const char16_t* a, const char16_t* b) {
    if (strcmp16(a, b) != 0) {
        return ::testing::AssertionFailure()
               << "\"" << String8(a).c_str() << "\" not equal to \"" << String8(b).c_str() << "\"";
    }
    return ::testing::AssertionSuccess();
}

#define EXPECT_STR16EQ(a, b) EXPECT_TRUE(Char16_tStringEquals(a, b))

TEST(String16Test, FromChar16_t) {
    String16 tmp(u"Verify me");
    EXPECT_STR16EQ(u"Verify me", tmp);
}

TEST(String16Test, FromChar16_tSized) {
    String16 tmp(u"Verify me", 7);
    EXPECT_STR16EQ(u"Verify ", tmp);
}

TEST(String16Test, FromChar) {
    String16 tmp("Verify me");
    EXPECT_STR16EQ(u"Verify me", tmp);
}

TEST(String16Test, FromCharSized) {
    String16 tmp("Verify me", 7);
    EXPECT_STR16EQ(u"Verify ", tmp);
}

TEST(String16Test, Copy) {
    String16 tmp("Verify me");
    String16 another = tmp;
    EXPECT_STR16EQ(u"Verify me", tmp);
    EXPECT_STR16EQ(u"Verify me", another);
}

TEST(String16Test, Move) {
    String16 tmp("Verify me");
    String16 another(std::move(tmp));
    EXPECT_STR16EQ(u"Verify me", another);
}

TEST(String16Test, Size) {
    String16 tmp("Verify me");
    EXPECT_EQ(9U, tmp.size());
}

TEST(String16Test, setTo) {
    String16 tmp("Verify me");
    tmp.setTo(u"New content");
    EXPECT_EQ(11U, tmp.size());
    EXPECT_STR16EQ(u"New content", tmp);
}

TEST(String16Test, Append) {
    String16 tmp("Verify me");
    tmp.append(String16("Hello"));
    EXPECT_EQ(14U, tmp.size());
    EXPECT_STR16EQ(u"Verify meHello", tmp);
}

TEST(String16Test, Insert) {
    String16 tmp("Verify me");
    tmp.insert(6, u"Insert");
    EXPECT_EQ(15U, tmp.size());
    EXPECT_STR16EQ(u"VerifyInsert me", tmp);
}

TEST(String16Test, Remove) {
    String16 tmp("Verify me");
    tmp.remove(2, 6);
    EXPECT_EQ(2U, tmp.size());
    EXPECT_STR16EQ(u" m", tmp);
}

TEST(String16Test, MakeLower) {
    String16 tmp("Verify Me!");
    tmp.makeLower();
    EXPECT_EQ(10U, tmp.size());
    EXPECT_STR16EQ(u"verify me!", tmp);
}

TEST(String16Test, ReplaceAll) {
    String16 tmp("Verify verify Verify");
    tmp.replaceAll(u'r', u'!');
    EXPECT_STR16EQ(u"Ve!ify ve!ify Ve!ify", tmp);
}

TEST(String16Test, Compare) {
    String16 tmp("Verify me");
    EXPECT_EQ(String16(u"Verify me"), tmp);
}

TEST(String16Test, StaticString) {
    String16 nonStaticString("NonStatic");
    StaticString16 staticString(u"Static");

    EXPECT_TRUE(staticString.isStaticString());
    EXPECT_FALSE(nonStaticString.isStaticString());
}

TEST(String16Test, StaticStringCopy) {
    StaticString16 tmp(u"Verify me");
    String16 another = tmp;
    EXPECT_STR16EQ(u"Verify me", tmp);
    EXPECT_STR16EQ(u"Verify me", another);
    EXPECT_TRUE(tmp.isStaticString());
    EXPECT_TRUE(another.isStaticString());
}

TEST(String16Test, StaticStringMove) {
    StaticString16 tmp(u"Verify me");
    String16 another(std::move(tmp));
    EXPECT_STR16EQ(u"Verify me", another);
    EXPECT_TRUE(another.isStaticString());
}

TEST(String16Test, StaticStringSize) {
    StaticString16 tmp(u"Verify me");
    EXPECT_EQ(9U, tmp.size());
}

TEST(String16Test, StaticStringSetTo) {
    StaticString16 tmp(u"Verify me");
    tmp.setTo(u"New content");
    EXPECT_EQ(11U, tmp.size());
    EXPECT_STR16EQ(u"New content", tmp);
    EXPECT_FALSE(tmp.isStaticString());
}

TEST(String16Test, StaticStringAppend) {
    StaticString16 tmp(u"Verify me");
    tmp.append(String16("Hello"));
    EXPECT_EQ(14U, tmp.size());
    EXPECT_STR16EQ(u"Verify meHello", tmp);
    EXPECT_FALSE(tmp.isStaticString());
}

TEST(String16Test, StaticStringInsert) {
    StaticString16 tmp(u"Verify me");
    tmp.insert(6, u"Insert");
    EXPECT_EQ(15U, tmp.size());
    EXPECT_STR16EQ(u"VerifyInsert me", tmp);
    EXPECT_FALSE(tmp.isStaticString());
}

TEST(String16Test, StaticStringRemove) {
    StaticString16 tmp(u"Verify me");
    tmp.remove(2, 6);
    EXPECT_EQ(2U, tmp.size());
    EXPECT_STR16EQ(u" m", tmp);
    EXPECT_FALSE(tmp.isStaticString());
}

TEST(String16Test, StaticStringMakeLower) {
    StaticString16 tmp(u"Verify me!");
    tmp.makeLower();
    EXPECT_EQ(10U, tmp.size());
    EXPECT_STR16EQ(u"verify me!", tmp);
    EXPECT_FALSE(tmp.isStaticString());
}

TEST(String16Test, StaticStringReplaceAll) {
    StaticString16 tmp(u"Verify verify Verify");
    tmp.replaceAll(u'r', u'!');
    EXPECT_STR16EQ(u"Ve!ify ve!ify Ve!ify", tmp);
    EXPECT_FALSE(tmp.isStaticString());
}

TEST(String16Test, StaticStringCompare) {
    StaticString16 tmp(u"Verify me");
    EXPECT_EQ(String16(u"Verify me"), tmp);
}

TEST(String16Test, StringSetToStaticString) {
    StaticString16 tmp(u"Verify me");
    String16 another(u"nonstatic");
    another = tmp;
    EXPECT_STR16EQ(u"Verify me", tmp);
    EXPECT_STR16EQ(u"Verify me", another);
}

TEST(String16Test, StringMoveFromStaticString) {
    StaticString16 tmp(u"Verify me");
    String16 another(std::move(tmp));
    EXPECT_STR16EQ(u"Verify me", another);
}

TEST(String16Test, EmptyStringIsStatic) {
    String16 tmp("");
    EXPECT_TRUE(tmp.isStaticString());
}

}  // namespace android
