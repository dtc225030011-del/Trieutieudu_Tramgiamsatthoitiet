#include "unity.h"

#include <math.h>
#include <string.h>

struct UNITY_STORAGE_T Unity;

void UnityBegin(const char *filename)
{
    memset(&Unity, 0, sizeof(Unity));
    Unity.TestFile = filename;
    printf("\n--------------------------------------------------\n");
    printf("UNITY TEST EXECUTION: %s\n", filename);
    printf("--------------------------------------------------\n");
}

int UnityEnd(void)
{
    printf("--------------------------------------------------\n");
    printf("%u Tests %u Failures %u Ignored\n", Unity.NumberOfTests,
           Unity.TestFailures, Unity.TestIgnores);
    printf(Unity.TestFailures == 0U ? "OK - ALL TESTS PASSED!\n" : "FAILED!\n");
    printf("--------------------------------------------------\n\n");
    return (int)Unity.TestFailures;
}

static void conclude_test(void)
{
    if (Unity.CurrentTestIgnored) {
        Unity.TestIgnores++;
    } else if (Unity.CurrentTestFailed) {
        Unity.TestFailures++;
    } else {
        printf("%s:%u:%s:PASS\n", Unity.TestFile, Unity.CurrentTestLineNumber,
               Unity.CurrentTestName);
    }
}

void UnityDefaultTestRun(UnityTestFunction function, const char *name, int line)
{
    Unity.CurrentTestName = name;
    Unity.CurrentTestLineNumber = (uint32_t)line;
    Unity.NumberOfTests++;
    Unity.CurrentTestFailed = 0;
    Unity.CurrentTestIgnored = 0;

    if (setjmp(Unity.AbortFrame) == 0) {
        setUp();
        function();
    }
    tearDown();
    conclude_test();
}

void UnityAssertEqualNumber(int32_t expected, int32_t actual, const char *message,
                            uint32_t line, UNITY_DISPLAY_STYLE_T style)
{
    (void)style;
    if (expected != actual) {
        Unity.CurrentTestFailed = 1;
        printf("%s:%u:%s:FAIL: Expected %d Was %d%s%s\n", Unity.TestFile, line,
               Unity.CurrentTestName, expected, actual, message ? " - " : "",
               message ? message : "");
        longjmp(Unity.AbortFrame, 1);
    }
}

void UnityAssertEqualString(const char *expected, const char *actual,
                            const char *message, uint32_t line)
{
    if (expected == actual) {
        return;
    }
    if (expected == NULL || actual == NULL || strcmp(expected, actual) != 0) {
        Unity.CurrentTestFailed = 1;
        printf("%s:%u:%s:FAIL: Expected '%s' Was '%s'%s%s\n", Unity.TestFile,
               line, Unity.CurrentTestName, expected ? expected : "NULL",
               actual ? actual : "NULL", message ? " - " : "",
               message ? message : "");
        longjmp(Unity.AbortFrame, 1);
    }
}

void UnityAssertFloatsWithin(float delta, float expected, float actual,
                             const char *message, uint32_t line)
{
    const float difference = fabsf(actual - expected);
    if (difference > delta || isnan(expected) || isnan(actual)) {
        Unity.CurrentTestFailed = 1;
        printf("%s:%u:%s:FAIL: Expected %.4f (+/- %.4f) Was %.4f%s%s\n",
               Unity.TestFile, line, Unity.CurrentTestName, expected, delta, actual,
               message ? " - " : "", message ? message : "");
        longjmp(Unity.AbortFrame, 1);
    }
}

void UnityFail(const char *message, uint32_t line)
{
    Unity.CurrentTestFailed = 1;
    printf("%s:%u:%s:FAIL: %s\n", Unity.TestFile, line, Unity.CurrentTestName,
           message ? message : "Test failed");
    longjmp(Unity.AbortFrame, 1);
}

void UnityIgnore(const char *message, uint32_t line)
{
    Unity.CurrentTestIgnored = 1;
    printf("%s:%u:%s:IGNORE: %s\n", Unity.TestFile, line, Unity.CurrentTestName,
           message ? message : "Test ignored");
    longjmp(Unity.AbortFrame, 1);
}
