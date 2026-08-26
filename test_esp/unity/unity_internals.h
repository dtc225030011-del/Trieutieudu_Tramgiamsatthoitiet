/*
 * Unity Project - A Test Framework for C
 * Copyright (c) 2007-21 Mike Karlesky, Mark VanderVoord, Greg Williams
 * Released under the MIT License.
 */

#ifndef UNITY_INTERNALS_H
#define UNITY_INTERNALS_H

#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef void (*UnityTestFunction)(void);

typedef enum {
    UNITY_DISPLAY_STYLE_INT = 0,
    UNITY_DISPLAY_STYLE_UINT,
    UNITY_DISPLAY_STYLE_HEX32
} UNITY_DISPLAY_STYLE_T;

struct UNITY_STORAGE_T {
    const char *TestFile;
    const char *CurrentTestName;
    uint32_t CurrentTestLineNumber;
    uint32_t NumberOfTests;
    uint32_t TestFailures;
    uint32_t TestIgnores;
    uint32_t CurrentTestFailed;
    uint32_t CurrentTestIgnored;
    jmp_buf AbortFrame;
};

extern struct UNITY_STORAGE_T Unity;

void UnityBegin(const char *filename);
int UnityEnd(void);
void UnityDefaultTestRun(UnityTestFunction function, const char *name, int line);
void UnityAssertEqualNumber(int32_t expected, int32_t actual, const char *message,
                            uint32_t line, UNITY_DISPLAY_STYLE_T style);
void UnityAssertEqualString(const char *expected, const char *actual,
                            const char *message, uint32_t line);
void UnityAssertFloatsWithin(float delta, float expected, float actual,
                             const char *message, uint32_t line);
void UnityFail(const char *message, uint32_t line);
void UnityIgnore(const char *message, uint32_t line);

#endif
