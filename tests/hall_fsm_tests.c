#include "minunit.h"
#include "hall_fsm.h"
#include <messages.h>

volatile uint8_t fake_port;
uint8_t state;

Port port_hall = { .port = &fake_port, .bit = 0 };
Port port_kitchen = { .port = &fake_port, .bit = 1 };

Hall hall = {
    .msg_id_hall = 0,
    .msg_id_kitchen = 1,
    .msg_id_off = 2,
    .port_hall = &port_hall,
    .port_kitchen = &port_kitchen
};


void tests_setup()
{
    msg_init(3);
    fake_port = 0x00;
    state = STATE_HALL_DARK;
}

void tests_teardown()
{
    msg_destroy();
}

void switch_by_message(uint8_t msg_id)
{
    msg_send(msg_id);
    msg_process();
    state = hall_fsm(state, &hall);
    msg_process();
}
char *test_no_messages()
{
    tests_setup();

    state = hall_fsm(state, &hall);
    mu_assert(state == STATE_HALL_DARK, "State should not be changed when no messages received");
    mu_assert(fake_port == 0x00, "Port state should not be changed when no messages received");

    tests_teardown();
    return NULL;
}

char *test_switch_hall()
{
    tests_setup();

    switch_by_message(0);
    mu_assert(state == STATE_HALL_LIGHTS, "State should be STATE_HALL_LIGHTS after msg_hall received");
    mu_assert(fake_port == 0x03, "State should be 0x03 (all ports on) after switch message receiving");

    switch_by_message(1);
    mu_assert(state == STATE_HALL_LIGHTS, "msg_kitchen should not affect state when it is STATE_HALL_LIGHTS");
    mu_assert(fake_port == 0x03, "msg_kitchen should not affect port state when state is STATE_HALL_LIGHTS");

    switch_by_message(0);
    mu_assert(state == STATE_HALL_DARK, "State should be STATE_HALL_DARK after switching off");
    mu_assert(fake_port == 0x00, "Ports should be switched off after switching off");

    tests_teardown();

    return NULL;
}

char *test_switch_kitchen()
{
    tests_setup();
    
    switch_by_message(1);
    mu_assert(state == STATE_HALL_KITCHEN_LIGHTS, "State should be STATE_HALL_KITCHEN_LIGHTS after msg_kitchen received");
    mu_assert(fake_port == 0x02, "Kitchen port should be switched on after msg_kitchen received");
    
    switch_by_message(1);
    mu_assert(state == STATE_HALL_DARK, "State should be STATE_HALL_DARK after receiving msg_kitchen");
    mu_assert(fake_port == 0x00, "Kitchen port should be switchef off after seconf msg_kitchen received");

    tests_teardown();

    return NULL;
}

char *test_hall_when_kitchen_lights()
{
    tests_setup();
    
    switch_by_message(1);
    
    switch_by_message(0);
    mu_assert(state == STATE_HALL_LIGHTS, "State should be STATE_HALL_LIGHTS after receiving msg_kitchen");
    mu_assert(fake_port == 0x03, "msg_hall should switch hall on when state is STATE_HALL_KITCHEN_LIGHTS");

    tests_teardown();

    return NULL;
}

char *test_off()
{
    tests_setup();

    switch_by_message(0);

    switch_by_message(2);
    mu_assert(state == STATE_HALL_DARK, "State should be STATE_HALL_DARK after msg_off received");
    mu_assert(fake_port == 0x00, "msg_off should off everything");

    switch_by_message(1);

    switch_by_message(2);
    mu_assert(state == STATE_HALL_DARK, "State should be STATE_HALL_DARK after msg_off received when kitchen lights");
    mu_assert(fake_port == 0x00, "msg_off should off kitchen when lights");

    tests_teardown();

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_no_messages);
    mu_run_test(test_switch_hall);
    mu_run_test(test_switch_kitchen);
    mu_run_test(test_hall_when_kitchen_lights);
    mu_run_test(test_off);
    return NULL;
}

RUN_TESTS(all_tests);
