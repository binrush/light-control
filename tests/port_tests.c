#include "minunit.h"
#include <port.h>

uint8_t fake_port = 0x00;

Port port = {
    .port = &fake_port,
    .bit = 0
};

char *test_port()
{
    fake_port = 0x00;
    Port_Set(&port);
    mu_assert(fake_port == 0x01, "Set should set port bit");
    Port_Set(&port);
    mu_assert(fake_port == 0x01, "Set should not change port that is already set");
    Port_Clear(&port);
    mu_assert(fake_port == 0x00, "Clear should clear prot bit");
    Port_Clear(&port);
    mu_assert(fake_port == 0x00, "Clear should not change port that is already clean");
    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_port);
    return NULL;
}

RUN_TESTS(all_tests);
