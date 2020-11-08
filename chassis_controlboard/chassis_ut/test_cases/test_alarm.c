
#include "test_alarm.h"
#include "unity.h"
#include "sys_monitor.h"
#include "alarm_com.h"

#ifdef __TEST_CASE_EN

void test_alarm(void)
{
  UnityBegin("test_alarm.c");
	RUN_TEST(test_ult_alarm);


}

void test_ult_alarm(void)
{
  /* All of these should pass */
  //TEST_ASSERT_EQUAL(1, 1);
  clearAlarm(eA_EXTE);

  // 测试超声波断连
  checkUltAlarm(1, 0);
  TEST_ASSERT_EQUAL(1, getAlarm(eA_EXTE, eb05_ultCommunWarn));
  checkUltAlarm(0, 0);  // 第二个参数错误
  TEST_ASSERT_EQUAL(1, getAlarm(eA_EXTE, eb05_ultCommunWarn));

  // 测试超声波断连恢复
  checkUltAlarm(0, 1);
  TEST_ASSERT_EQUAL(0, getAlarm(eA_EXTE, eb05_ultCommunWarn));

  clearAlarm(eA_EXTE);

  // 测试1号超声波故障
  checkUltAlarm(2, 0);
  TEST_ASSERT_EQUAL(1, getAlarm(eA_EXTE, eb6_ultChannel_1_warn));

  // 测试1号超声波故障恢复
  checkUltAlarm(0, 2);
  TEST_ASSERT_EQUAL(0, getAlarm(eA_EXTE, eb6_ultChannel_1_warn));
}


void test_alarm_Ignored(void)
{
    TEST_IGNORE_MESSAGE("This Test Was Ignored On Purpose ==TEST_IGNORE_MESSAGE()");
}

void test_alarm_func_needs_implemented(void)
{
    TEST_IGNORE(); /* Like This */
}

void test_alarm_assert_messgae(void)
{
    int a = 2;
    //TEST_ASSERT_MESSAGE( a == 2 , "a isn't 2, end of the world!==TEST_ASSERT_MESSAGE()"); /* Like This */
    TEST_ASSERT_EQUAL_INT(2,a);
    TEST_ASSERT_EQUAL_HEX8(0x02,a);
    TEST_ASSERT_EQUAL_UINT16(0x03, a);

}

#endif //__TEST_CASE_EN

