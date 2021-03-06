/*
 * 
 */

#include <seahorn/seahorn.h>
#include <aws/common/byte_buf.h>
#include <byte_buf_helper.h>
#include <utils.h>

int main() {
    struct aws_byte_buf buf;
    initialize_byte_buf(&buf);
    
    assume(aws_byte_buf_is_valid(&buf));

    //struct aws_byte_buf old = buf;
    size_t requested_capacity = nd_size_t();
    #ifdef __KLEE__
        if (requested_capacity > MAX_BUFFER_SIZE) return 0;
        if (buf.capacity >= MAX_BUFFER_SIZE) return 0;
    #else 
        assume(requested_capacity <= MAX_BUFFER_SIZE);
        assume(buf.capacity < MAX_BUFFER_SIZE);
    #endif

    if (aws_byte_buf_reserve(&buf, requested_capacity) == AWS_OP_SUCCESS) {
        sassert(buf.capacity >= requested_capacity);
        //sassert(aws_byte_buf_has_allocator(&buf));
    }

    /*sassert(aws_byte_buf_is_valid(&buf));
    sassert(old.len == buf.len);
    sassert(old.allocator == buf.allocator);
    if (!buf.buffer) {
        assert_bytes_match(old.buffer, buf.buffer, buf.len);
    }*/

    return 0;
}
