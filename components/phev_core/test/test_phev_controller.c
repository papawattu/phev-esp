#include "unity.h"
#include "msg_core.h"
#include "mock_msg_pipe.h"
#include "phev_controller.h"
#include "mock_phev_core.h"
#include "mock_msg_utils.h"


void test_handle_event(void)
{
    phevEvent_t event = {
        .type = CONNECT_REQUEST,
    };

    TEST_ASSERT_EQUAL(PHEV_OK, phev_controller_handleEvent(&event));
}

void test_phev_controller_init(void)
{
    messagingClient_t inClient;
    messagingClient_t outClient;

    msg_pipe_ctx_t pipe;

    msg_pipe_IgnoreAndReturn(&pipe);
    
    phevSettings_t settings = {
        .in     = &inClient,
        .out    = &outClient,
    };

    phevCtx_t * ctx = phev_controller_init(&settings);

    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL(&pipe, ctx->pipe);
    TEST_ASSERT_EQUAL(0, ctx->queueSize);
} 

void test_phev_controller_init_set_phev_ctx(void)
{
    messagingClient_t inClient;
    messagingClient_t outClient;

    void * dummyCtx;

    msg_pipe_ctx_t pipe = {
        .user_context = dummyCtx,
    };

    msg_pipe_IgnoreAndReturn(&pipe);
    
    phevSettings_t settings = {
        .in     = &inClient,
        .out    = &outClient,
    };

    phevCtx_t * ctx = phev_controller_init(&settings);

    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL(dummyCtx, ctx->pipe->user_context);
} 