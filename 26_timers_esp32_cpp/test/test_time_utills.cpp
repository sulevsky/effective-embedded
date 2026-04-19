#include <stdio.h>
#include <unity.h>
#include "time_utils.cpp"

void test_overflow()
{
    uint32_t now = 4000000000 + 1000000000;
    uint32_t last_event = 4000000000;
    bool result = is_expired(now, last_event, 1000000000 + 10);
    TEST_ASSERT_FALSE(result);
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_overflow);
    return UNITY_END();
}
