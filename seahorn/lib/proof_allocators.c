/*
 * Copyright 2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <aws/common/math.h>
#include <proof_allocators.h>
#include <nondet.h>
#include <stdarg.h>
#include <stdlib.h>

#include <seahorn/seahorn.h>
#include <sea_mem_helper.h>

int8_t *g_ptr0 = NULL;
size_t g_ptr0_size;

void *realloc( void *ptr, size_t new_size ) {
    if (ptr) {
        free(ptr);
    }
    ptr = malloc(new_size);
    if (nd_store_mem_size() && g_ptr0 == NULL) {
        g_ptr0 = (int8_t*)ptr;
        g_ptr0_size = new_size;
    }
    return ptr;
}

/**
 *
 */
static void *s_can_fail_calloc_allocator(struct aws_allocator *allocator, size_t num, size_t size) {
    (void)allocator;
    return can_fail_calloc(num, size);
}

/**
 *
 */
static void *s_can_fail_malloc_allocator(struct aws_allocator *allocator, size_t size) {
    (void)allocator;
    return can_fail_malloc(size);
}

/**
 * Since we always allocate with "malloc()", just free with "free()"
 */
static void s_can_fail_free_allocator(struct aws_allocator *allocator, void *ptr) {
    (void)allocator;
    free(ptr);
}

/**
 *
 */
static void *s_can_fail_realloc_allocator(struct aws_allocator *allocator, void *ptr, size_t oldsize, size_t newsize) {
    (void)allocator;
    (void)oldsize;
    return can_fail_realloc(ptr, newsize);
}

static struct aws_allocator s_can_fail_allocator_static = {
    .mem_acquire = s_can_fail_malloc_allocator,
    .mem_release = s_can_fail_free_allocator,
    .mem_realloc = s_can_fail_realloc_allocator,
    .mem_calloc = s_can_fail_calloc_allocator,
};


void *bounded_malloc(size_t size) {
    assume(size <= MAX_MALLOC);
    return malloc(size);
}

struct aws_allocator *can_fail_allocator() {
    return &s_can_fail_allocator_static;
}

void *can_fail_calloc(size_t num, size_t size) {
    return nondet_bool() ? NULL : bounded_calloc(num, size);
}

void *can_fail_malloc(size_t size) {
    return nondet_bool() ? NULL : bounded_malloc(size);
}

/************************************************************************************************************
 * These assert that the allocator is can_fail_allocator, and then do the allocations without using
 * function pointers. (Function pointers make CBMC slow).
 */

/**
 * This assert will fail if code ever uses a different allocator than expected during a proof
 */
bool aws_allocator_is_valid(const struct aws_allocator *alloc) {
    return alloc == can_fail_allocator();
}

void *aws_mem_acquire(struct aws_allocator *allocator, size_t size) {
    AWS_FATAL_PRECONDITION(aws_allocator_is_valid(allocator));
    /* Protect against https://wiki.sei.cmu.edu/confluence/display/c/MEM04-C.+Beware+of+zero-length+allocations */
    AWS_FATAL_PRECONDITION(size != 0);

    void *mem = can_fail_malloc(size);
    if (!mem) {
        aws_raise_error(AWS_ERROR_OOM);
    }
    return mem;
}

void *aws_mem_calloc(struct aws_allocator *allocator, size_t num, size_t size) {
    AWS_FATAL_PRECONDITION(aws_allocator_is_valid(allocator));
    /* Protect against https://wiki.sei.cmu.edu/confluence/display/c/MEM04-C.+Beware+of+zero-length+allocations */
    AWS_FATAL_PRECONDITION(num != 0 && size != 0);

    /* Defensive check: never use calloc with size * num that would overflow
     * https://wiki.sei.cmu.edu/confluence/display/c/MEM07-C.+Ensure+that+the+arguments+to+calloc%28%29%2C+when+multiplied%2C+do+not+wrap
     */
    size_t required_bytes;
    if (aws_mul_size_checked(num, size, &required_bytes)) {
        return NULL;
    }

    void *mem = can_fail_calloc(num, size);
    if (!mem) {
        aws_raise_error(AWS_ERROR_OOM);
    }
    return mem;
}

#define AWS_ALIGN_ROUND_UP(value, alignment) (((value) + ((alignment)-1)) & ~((alignment)-1))

void *aws_mem_acquire_many(struct aws_allocator *allocator, size_t count, ...) {

    enum { S_ALIGNMENT = sizeof(intmax_t) };

    va_list args_size;
    va_start(args_size, count);
    va_list args_allocs;
    va_copy(args_allocs, args_size);

    size_t total_size = 0;
    for (size_t i = 0; i < count; ++i) {

        /* Ignore the pointer argument for now */
        va_arg(args_size, void **);

        size_t alloc_size = va_arg(args_size, size_t);
        total_size += AWS_ALIGN_ROUND_UP(alloc_size, S_ALIGNMENT);
    }
    va_end(args_size);

    void *allocation = NULL;

    if (total_size > 0) {

        allocation = can_fail_malloc(total_size);
        if (!allocation) {
            aws_raise_error(AWS_ERROR_OOM);
            goto cleanup;
        }

        uint8_t *current_ptr = allocation;

        for (size_t i = 0; i < count; ++i) {

            void **out_ptr = va_arg(args_allocs, void **);

            size_t alloc_size = va_arg(args_allocs, size_t);
            alloc_size = AWS_ALIGN_ROUND_UP(alloc_size, S_ALIGNMENT);

            *out_ptr = current_ptr;
            current_ptr += alloc_size;
        }
    }

cleanup:
    va_end(args_allocs);
    return allocation;
}

#undef AWS_ALIGN_ROUND_UP

void aws_mem_release(struct aws_allocator *allocator, void *ptr) {
    AWS_FATAL_PRECONDITION(allocator != NULL);
    AWS_FATAL_PRECONDITION(allocator->mem_release != NULL);

    if (ptr != NULL) {
        free(ptr);
    }
}

int aws_mem_realloc(struct aws_allocator *allocator, void **ptr, size_t oldsize, size_t newsize) {
    AWS_FATAL_PRECONDITION(allocator != NULL);
    AWS_FATAL_PRECONDITION(allocator->mem_realloc || allocator->mem_acquire);
    AWS_FATAL_PRECONDITION(allocator->mem_release);

    /* Protect against https://wiki.sei.cmu.edu/confluence/display/c/MEM04-C.+Beware+of+zero-length+allocations */
    if (newsize == 0) {
        aws_mem_release(allocator, *ptr);
        *ptr = NULL;
        return AWS_OP_SUCCESS;
    }

    void *newptr = can_fail_realloc(*ptr, newsize);
    if (!newptr) {
        return aws_raise_error(AWS_ERROR_OOM);
    }
    *ptr = newptr;
    return AWS_OP_SUCCESS;
}
