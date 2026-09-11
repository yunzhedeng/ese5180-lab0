#include <zephyr/logging/log.h>
#include "sum_log.h"

LOG_MODULE_REGISTER(sum_module);

int sum_log(int a, int b)
{
    int inputs[2] = {a, b};
    int result = a + b;

    LOG_ERR("Demo error message (not an actual failure)");
    LOG_WRN("Demo warning message");
    LOG_INF("%d + %d = %d", a, b, result);
    LOG_DBG("Debug: calculation complete");
    LOG_HEXDUMP_INF(inputs, sizeof(inputs), "Input bytes:");

    return result;
}