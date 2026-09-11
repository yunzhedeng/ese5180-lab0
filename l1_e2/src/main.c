#include <zephyr/kernel.h>

#ifdef CONFIG_SUM_PRINT
#include "sum_printk.h"
#elif defined(CONFIG_SUM_LOG)
#include "sum_log.h"
#endif

int main(void)
{
    while (1) {
    #ifdef CONFIG_SUM_PRINT
            sum_printk(3, 5);
    #elif defined(CONFIG_SUM_LOG)
            sum_log(3, 5);
    #endif

            k_msleep(2000);
    }
    return 0;
}