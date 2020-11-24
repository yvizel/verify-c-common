/*
 *
 */

#include <seahorn/seahorn.h>
#include <aws/common/array_list.h>
#include <array_list_helper.h>
#include <utils.h>
#include <proof_allocators.h>

/**
 * Runtime: 7s
 */
int main() {
    /* data structure */
    struct aws_array_list list;
<<<<<<< HEAD
    initialize_bounded_array_list(&list, MAX_INITIAL_SIZE,
        MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);

    /* assumptions */
=======
    initialize_bounded_array_list(&list, MAX_BLOCK_SIZE);

    /* assumptions */
    assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
>>>>>>> 9e23b71 (test(harness): add remain bounded array list harnesses work both for klee and seahorn)
    assume(aws_array_list_is_valid(&list));

    /* save current state of the data structure */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* perform operation under verification */
    size_t len = aws_array_list_length(&list);

    /* assertions */
    sassert(aws_array_list_is_valid(&list));
    assert_array_list_equivalence(&list, &old, &old_byte);

    return 0;
}
