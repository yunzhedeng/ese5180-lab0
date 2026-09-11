#include <zephyr/ztest.h>
#include "sum_log.h"

ZTEST(sum_log_test_suite, test_sum_log_basic)
{
    int result = sum_log(3, 5);
    zassert_equal(result, 8, "3 + 5 should equal 8");
}

ZTEST(sum_log_test_suite, test_sum_log_negative)
{
    zassert_equal(sum_log(-3, -5), -8,
                  "-3 + -5 should equal -8");

    zassert_equal(sum_log(-3, 5), 2,
                  "-3 + 5 should equal 2");
}

ZTEST(sum_log_test_suite, test_sum_log_zero)
{
    zassert_equal(sum_log(0, 0), 0, "0 + 0 should equal 0");

    zassert_equal(sum_log(5, 0), 5, "5 + 0 should equal 5");

    zassert_equal(sum_log(0, 5), 5, "0 + 5 should equal 5");
}

ZTEST_SUITE(sum_log_test_suite, NULL, NULL, NULL, NULL, NULL);