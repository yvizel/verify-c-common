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

#include <seahorn/seahorn.h>
#include <aws/common/array_list.h>
#include <array_list_helper.h>

/**
 * Runtime: 10s
 */
void aws_array_list_copy_harness() {
    /* data structure */
    struct aws_array_list from;
    struct aws_array_list to;

    initialize_array_list(&from);
    initialize_array_list(&to);

    /* assumptions */
    assume(aws_array_list_is_bounded(&from, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    assume(aws_array_list_is_valid(&from));

    assume(aws_array_list_is_bounded(&to, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    assume(aws_array_list_is_valid(&to));

    assume(from.item_size == to.item_size);
    assume(from.data != NULL);

    /* perform operation under verification */
    if (!aws_array_list_copy(&from, &to)) {
        /* In the case aws_array_list_copy is successful, both lists have the same length */
        sassert(to.length == from.length);
        sassert(to.current_size >= (from.length * from.item_size));
    }

    /* assertions */
    sassert(aws_array_list_is_valid(&from));
    sassert(aws_array_list_is_valid(&to));
    sassert(from.item_size == to.item_size);
}
