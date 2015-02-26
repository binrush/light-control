#include "minunit.h"
#include <room_fsm.h>
#include <timers.h>
#include <messages.h>

void timer_set(uint16_t value);

volatile uint8_t fake_port;
uint8_t state;

Port port0 = { .port = &fake_port, .bit = 0 };
Port port1 = { .port = &fake_port, .bit = 1 };

Room room = {
    .timer_id = 0,
    .msg_id_switch = 0,
    .msg_id_off = 1,
    .port0 = &port0,
    .port1 = &port1
};

void tests_setup()
{
    msg_init(2);
    timers_init(1);
    fake_port = 0x00;
    state = STATE_ROOM_DARK;
}

void tests_teardown()
{
    msg_destroy();
    timer_stop(0);
    timers_destroy();
}

void switch_by_message(uint8_t msg_id)
{
    msg_send(msg_id);
    msg_process();
    state = room_fsm(state, &room);
    msg_process();
}

char *test_no_messages()
{
    tests_setup();

    state = room_fsm(state, &room);
    mu_assert(state == STATE_ROOM_DARK, "State should not be changed without messages");
    mu_assert(fake_port == 0x00, "Ports state should not be changed without messages");

    tests_teardown();

    return NULL;
}

char *test_switch()
{
    tests_setup();

    switch_by_message(0);
    mu_assert(state == STATE_ROOM_LIGHT_WAIT_SWITCH, "State should be STATE_ROOM_LIGHT_WAIT_SWITCH after msg_switch");
    mu_assert(fake_port == 0x03, "Ports should be switched on after msg_switch");
    mu_assert(timer_get(0) > 0, "Timer should be started after switching on");

    timer_set(2);
    switch_by_message(0);
    mu_assert(state == STATE_ROOM_CHAN0_WAIT_SWITCH, "State should be STATE_ROOM_CHAN0_WAIT_SWITCH after second msg_switch");
    mu_assert(fake_port = 0x01, "First port should be switched on after seconf msg_switch");
    mu_assert(timer_get(0) == 1, "Timer should be restarted after seconf msg_switch");

    timer_set(3);
    switch_by_message(0);
    mu_assert(state == STATE_ROOM_LIGHT, "State should be STATE_ROOM_LIGHT after third msg_switch");
    mu_assert(fake_port = 0x02, "Second port should be switched on after third msg_switch");
    mu_assert(timer_get(0) == 0, "Timer should be stopped after third msg_switch");

    switch_by_message(0);
    mu_assert(state == STATE_ROOM_DARK, "State should be STATE_ROOM_DARK after forth msg_switch");
    mu_assert(fake_port == 0x00, "All ports should be switched off after forth msg_switch");
    mu_assert(timer_get(0) == 0, "Timer should be stopped after forth msg_switch");   

    tests_teardown();
    return NULL;
}

char *test_switch_timeout()
{
    tests_setup();

    switch_by_message(0);
    timer_set(SWITCH_TIMEOUT + 10);
    state = room_fsm(state, &room);
    mu_assert(state == STATE_ROOM_LIGHT, "State should be STATE_ROOM_LIGHT when switch timeout passed");
    mu_assert(fake_port == 0x03, "All ports should be switched on after switch");
    mu_assert(timer_get(0) == 0, "Timer should be stopped after timeout pass");

    switch_by_message(0);
    switch_by_message(0);
    timer_set(10);
    switch_by_message(0);
    timer_set(SWITCH_TIMEOUT + 30);
    state = room_fsm(state, &room);
    mu_assert(state == STATE_ROOM_LIGHT, "State should be STATE_ROOM_LIGHT when switch timeout passed");
    mu_assert(fake_port == 0x01, "Port 0 shoyld be switched on after switch");
    mu_assert(timer_get(0) == 0, "Timer should be stopped after timeout pass");

    tests_teardown();

    return NULL;
}

char *test_switch_off()
{
    tests_setup();

    switch_by_message(0);
    timer_set(SWITCH_TIMEOUT + 10);
    state = room_fsm(state, &room);

    switch_by_message(1);
    mu_assert(state == STATE_ROOM_DARK, "State should be STATE_ROOM_DARK after msg_off");
    mu_assert(fake_port == 0x00, "Ports should be switched off after msg_off");

    tests_teardown();

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_switch_off);
    mu_run_test(test_no_messages);
    mu_run_test(test_switch);
    mu_run_test(test_switch_timeout);
    return NULL;
}

RUN_TESTS(all_tests);
