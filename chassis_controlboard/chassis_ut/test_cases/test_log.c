#include "test_log.h"
#include "unity.h"
#ifdef __TEST_CASE_EN

void test_log(void)
{
  UnityBegin("test_log.c");
	RUN_TEST(test_log_1);
}

void test_log_1(void)
{
  int a = 2;
  /* All of these should pass */
  TEST_ASSERT_EQUAL(1, 1);
  TEST_IGNORE_MESSAGE("This Test Was Ignored On Purpose ==TEST_IGNORE_MESSAGE()");
  //TEST_ASSERT_MESSAGE( a == 2 , "a isn't 2, end of the world!==TEST_ASSERT_MESSAGE()"); /* Like This */
  TEST_ASSERT_EQUAL_INT(2,a);
  TEST_ASSERT_EQUAL_HEX8(0x02,a);
  TEST_ASSERT_EQUAL_UINT16(0x03, a);
}

#endif //__TEST_CASE_EN

