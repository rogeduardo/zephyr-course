/*
 * Ring Buffer Module - Homework Test Skeleton
 *
 * test_fresh_state is provided as a worked example. Fill in the remaining
 * 7 ZTEST bodies according to TEST_SPEC.md. Stubs call ztest_test_skip()
 * so the binary builds and runs cleanly before each test is implemented.
 *
 * Run:
 *   west twister -T tests/ring_buf -p native_sim
 */

#include <zephyr/ztest.h>
#include <errno.h>
#include <stdbool.h>

#include "ring_buf.h"

/*
 * Shared before hook: every suite reinitialises the ring buffer with a
 * capacity of 4 so tests start from a clean, known state. Capacity 4 is
 * enough to exercise FIFO order (push 1, 2, 3) and overflow (full at 4).
 */
static void before(void *f)
{
	ARG_UNUSED(f);
	rb_init(4);
}

/*
 * ============================================================================
 * Test Suite: ring_buf_init
 *
 * Initial state and re-initialization behaviour.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_init, NULL, NULL, before, NULL, NULL);

/* PROVIDED — study this test before writing the rest. */
ZTEST(ring_buf_init, test_fresh_state)
{
	zassert_true(rb_is_empty(), "Fresh buffer must be empty");
	zassert_equal(rb_count(), 0, "Fresh buffer count must be 0");
}

ZTEST(ring_buf_init, test_reinit_clears_state)
{
	/* Push a value, call rb_init(4) again, then verify the buffer is empty and count is 0.
	 * See TEST_SPEC.md "Suite ring_buf_init" #2.
	 */
	zassume_true(rb_push(1) == 0);
	zassume_true (rb_init(4) == 0);
	zassert_true(rb_is_empty(), "Fresh buffer must be empty");
	zassert_equal (rb_count(), 0, "Fresh buffer count must be 0" );
}

/*
 * ============================================================================
 * Test Suite: ring_buf_push_pop
 *
 * Single push/pop round-trip, FIFO order, full error path.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_push_pop, NULL, NULL, before, NULL, NULL);

/* Push a value, pop this value and buffer must be empty after */
ZTEST(ring_buf_push_pop, test_single_push_pop)
{	
	int value = 42;
	int ret_value;

	zassert_true(rb_push(value) == 0, "Push value into buffer");
	zassert_true(rb_pop(&ret_value) == 0, "Check the value is correct");
	zassert_equal(ret_value, value, "Value returned from buffer should be %d", value);
	zassert_true(rb_is_empty(), "Buffer must be empty");
}

/* Push three values, pop them all and verify the order and the buffer must be empty in the end*/
ZTEST(ring_buf_push_pop, test_fifo_order)
{
	int value[3] = {1, 2, 3};
	int ret_value;
	
	for ( int i=0; i<3 ; i++ )
		zassume_true(rb_push(value[i]) == 0, "Fill the buffer");

	for ( int i=0; i<3 ; i++ )
	{
		zassume_true(rb_pop(&ret_value) == 0);
		zassert_equal(ret_value, value[i], "Value returned from buffer must be %d", value[i]);
	}
	zassert_true(rb_is_empty(), "Buffer must be empty");
}

/* Fill the buffer to its capacity of 4, then push
 * one more value -> -ENOSPC.
 */
ZTEST(ring_buf_push_pop, test_push_full_returns_enospc)
{
	int value[4] = {1, 2, 3, 4};

	for ( int i=0; i<4 ; i++ )
		zassume_true(rb_push(value[i]) == 0, "Fill the buffer");

	zassert_true (rb_is_full(), "Buffer must be full" );
	zassert_equal (rb_push(99), -ENOSPC, "Return a error because a buffer is full");
	zassert_equal (rb_count(), 4, "Buffer size must be 4");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_boundaries
 *
 * Peek semantics and NULL-pointer boundary conditions.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_boundaries, NULL, NULL, before, NULL, NULL);

/* Push one value, peek it twice and check the buffer size
 */
ZTEST(ring_buf_boundaries, test_peek_does_not_consume)
{
	int value = 7;
	int ret_value;	

	zassume_true (rb_push(value) == 0);
	zassume_true (rb_peek(&ret_value) == 0);
	zassert_equal (ret_value, value, "Returned value %d must be equal to pushed value %d", ret_value, value);
	ret_value = 0;
	zassume_true (rb_peek(&ret_value) == 0);
	zassert_equal (ret_value, value, "Returned value %d must be equal to pushed value %d", ret_value, value);
	zassert_equal (rb_count(), 1, "Buffer size must be 1");
}

/* Pop NULL
 * See TEST_SPEC.md "Suite ring_buf_boundaries" #2.
 */
ZTEST(ring_buf_boundaries, test_pop_null_returns_einval)
{
	zassert_equal (rb_pop(NULL), -EINVAL, "Pop NULL must return EINVAL error");
}

/* 
 * Push full buffer capacity and check if rb_is_full is true
 */
ZTEST(ring_buf_boundaries, test_is_full_after_fill)
{
	int value[4] = {1, 2, 3, 4};

	for ( int i=0; i<4 ; i++ )
		zassume_true( rb_push (value[i]) == 0, "Fill the buffer" );

	zassert_true ( rb_is_full(), "Buffer at full capacity" );
	zassert_equal ( rb_count(), 4, "Buffer size must be 4" );
}
