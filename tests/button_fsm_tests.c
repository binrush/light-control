#include "minunit.h"
#include <button_fsm.h>

#define PIN_BIT 4

void timer_set(uint16_t value);

uint8_t fake_pin = 0xff;

uint8_t state = STATE_BUTTON_RELEASED;

Button button = {
    .timer_id = 0,
    .msg_id_short = 0,
    .msg_id_long = 1,
    .pin = &fake_pin,
    .bit = PIN_BIT
};

void tests_setup()
{
    msg_init(2);
    timers_init(1);
    fake_pin = 0xff;
}

void tests_teardown()
{
    msg_destroy();
    timer_stop(0);
    timers_destroy();
    state = STATE_BUTTON_RELEASED;
}

char *test_button_read() {
    mu_assert(Button_Read(&button), "Incorrect button read, should be true if pin is 1");
    fake_pin &= ~(1<<PIN_BIT);
    mu_assert(! Button_Read(&button), "Incorrect button read, should be false if pin is 0");
    return NULL;
}

char *test_button_not_pressed()
{
    tests_setup();

    state = button_fsm(state, &button);
    mu_assert(state == STATE_BUTTON_RELEASED, "State shouldn't be changed if button not pressed");
    mu_assert(msg_get(button.msg_id_short) == 0 && msg_get(button.msg_id_short) == 0, "Message should not be sent if button not pressed");

    tests_teardown();
    return NULL;    
}


char *test_button_short_push()
{
    tests_setup();

    fake_pin &= ~(1<<PIN_BIT);
    state = button_fsm(state, &button);
    mu_assert(state == STATE_BUTTON_NOISE_DELAY, "State should be changed to NOISE_DELAY after button pressed");
    timer_set(BUTTON_NOISE_DELAY + 2);
    state = button_fsm(state, &button);
    mu_assert(state == STATE_BUTTON_COUNT_LENGTH, "FSM should wait button to be released");
    mu_assert(msg_get(button.msg_id_short) == 0, "Message should not be sent until button released");
    fake_pin = 0xff;
    state = button_fsm(state, &button);
    mu_assert(state == STATE_BUTTON_RELEASED, "After short button push state should be RELEASED");
    msg_process();
    mu_assert(msg_get(button.msg_id_short), "Message should be sent after short button push");
    mu_assert(timer_get(button.timer_id) == 0, "Timer should be stopped after button released");
    msg_process();

    tests_teardown();
    return NULL;    
}

char *test_button_long_push()
{
    tests_setup();

    fake_pin &= ~(1<<PIN_BIT);
    state = button_fsm(state, &button);
    mu_assert(state == STATE_BUTTON_NOISE_DELAY, "State should be changed to NOISE_DELAY after button pressed");
    timer_set(BUTTON_NOISE_DELAY + 2);
    state = button_fsm(state, &button);
    mu_assert(state == STATE_BUTTON_COUNT_LENGTH, "FSM should wait button to be released");
    timer_set(BUTTON_LONG_PUSH + 5);
    state = button_fsm(state, &button);
    mu_assert(state == STATE_BUTTON_WAIT_RELEASE, "Should wait release after long push");
    msg_process();
    mu_assert(msg_get(button.msg_id_long), "Message should be sent after long push");
    msg_process();
    fake_pin = 0xff;
    state = button_fsm(state, &button);
    mu_assert(state == STATE_BUTTON_RELEASED, "After long button push state should be RELEASED");
    msg_process();

    tests_teardown();
    return NULL;    
}

char *test_button_noise_delay()
{
    tests_setup();

    fake_pin &= ~(1<<PIN_BIT);
    state = button_fsm(state, &button);
    fake_pin = 0xff;
    state = button_fsm(state, &button);
    fake_pin &= ~(1<<PIN_BIT);
    state = button_fsm(state, &button);
    msg_process();

    mu_assert(state == STATE_BUTTON_NOISE_DELAY, "State should not be changed during noise prevention delay");
    mu_assert(msg_get(button.msg_id_long) == 0 && msg_get(button.msg_id_short) == 0, "Messages should not be sent during noise prevention delay");

    fake_pin = 0xff;
    timer_set(BUTTON_NOISE_DELAY + 2);
    state = button_fsm(state, &button);

    mu_assert(state == STATE_BUTTON_RELEASED, "Button FSM should return to init state if button released during noise timeout");

    tests_teardown();

    return NULL;
}


char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_button_read);
    mu_run_test(test_button_not_pressed);
    mu_run_test(test_button_short_push);
    mu_run_test(test_button_long_push);
    mu_run_test(test_button_noise_delay);
    return NULL;
}

RUN_TESTS(all_tests);
