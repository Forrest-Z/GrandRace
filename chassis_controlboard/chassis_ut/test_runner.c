
/*=======Automagically Detected Files To Include=====*/
#include "unity.h"
#include <setjmp.h>
#include <stdio.h>
#include "test_alarm.h"

#ifdef __TEST_CASE_EN

#include "test_runner.h"

/*=======MAIN=====*/
int unity_main(void)
{
  test_alarm();

  return (UnityEnd());
}

#endif //__TEST_CASE_EN

