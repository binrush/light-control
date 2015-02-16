#include <messages.h>
#include "minunit.h"

char *test_send_receive()
{
    msg_init(5);
    mu_assert(msg_send(0) == 0, "msg_send for correct id should return 0");
    mu_assert(msg_send(4) == 0, "msg_send for correct id should return 0");
    mu_assert(msg_get(0) == 0, "Message should not be active after sending");
    mu_assert(msg_get(4) == 0, "Message should not be active after sending");
    msg_process();
    mu_assert(msg_get(0) == 1, "Sent message should be active after processing");
    mu_assert(msg_get(4) == 1, "Sent message should be active after processing");
    mu_assert(msg_get(3) == 0, "Message that was not send should not be active.");
    msg_process();
    mu_assert(msg_get(0) == 0, "Active message should be deactivated after processing");
    mu_assert(msg_get(4) == 0, "Active message should be deactivated after processing");
    msg_destroy();

    return NULL;
}

char *test_invalid_id()
{
    msg_init(5);
    mu_assert(msg_send(10) == -1, "Send should return error on incorrect id");
    mu_assert(msg_get(10) == 0, "Invalid message should be considered inactive");
    msg_destroy();

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_send_receive);
    mu_run_test(test_invalid_id);
    return NULL;
}

RUN_TESTS(all_tests);
