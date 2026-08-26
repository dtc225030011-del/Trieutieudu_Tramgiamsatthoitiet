/* Minimal Unity API used by this host-side ESP32 test suite. */

#ifndef UNITY_FRAMEWORK_H
#define UNITY_FRAMEWORK_H

#include "unity_internals.h"

void setUp(void);
void tearDown(void);

#define UNITY_BEGIN() UnityBegin(__FILE__)
#define UNITY_END() UnityEnd()
#define RUN_TEST(function) UnityDefaultTestRun(function, #function, __LINE__)

#define TEST_ASSERT(condition)                                                   \
    do {                                                                         \
        if (!(condition)) {                                                      \
            UnityAssertEqualNumber(1, 0, "Expression evaluated to false",       \
                                   __LINE__, UNITY_DISPLAY_STYLE_INT);           \
        }                                                                        \
    } while (0)

#define TEST_ASSERT_TRUE(condition) TEST_ASSERT(condition)
#define TEST_ASSERT_FALSE(condition) TEST_ASSERT(!(condition))
#define TEST_ASSERT_EQUAL_INT(expected, actual)                                  \
    UnityAssertEqualNumber((int32_t)(expected), (int32_t)(actual), NULL,         \
                           __LINE__, UNITY_DISPLAY_STYLE_INT)
#define TEST_ASSERT_EQUAL_UINT(expected, actual)                                 \
    UnityAssertEqualNumber((int32_t)(expected), (int32_t)(actual), NULL,         \
                           __LINE__, UNITY_DISPLAY_STYLE_UINT)
#define TEST_ASSERT_EQUAL_FLOAT(expected, actual)                                \
    UnityAssertFloatsWithin(0.0001F, (float)(expected), (float)(actual), NULL,    \
                            __LINE__)
#define TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)                        \
    UnityAssertFloatsWithin((float)(delta), (float)(expected), (float)(actual),  \
                            NULL, __LINE__)
#define TEST_ASSERT_EQUAL_STRING(expected, actual)                               \
    UnityAssertEqualString((expected), (actual), NULL, __LINE__)
#define TEST_FAIL_MESSAGE(message) UnityFail((message), __LINE__)

#endif
