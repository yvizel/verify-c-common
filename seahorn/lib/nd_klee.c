#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* #######################################
   #      The KLEE Function Declaration      #
   #######################################
 *  The following declarations are borrowed from
 *  https://github.com/klee/klee/blob/master/include/klee/klee.h
 *  We only put some functions that we used for running KLEE.
 */

/* klee_make_symbolic - Make the contents of the object pointer to by \arg
   * addr symbolic.
   *
   * \arg addr - The start of the object.
   * \arg nbytes - The number of bytes to make symbolic; currently this *must*
   * be the entire contents of the object.
   * \arg name - A name used for identifying the object in messages, output
   * files, etc. If NULL, object is called "unnamed".
   E.g.
   int a;
   klee_make_symbolic(&a, sizeof(a), "name_for_a");
*/
extern void klee_make_symbolic(void *addr, size_t nbytes, const char *name);

/* klee_range - Construct a symbolic value in the signed interval
  * [begin,end).
  *
  * \arg name - A name used for identifying the object in messages, output
  * files, etc. If NULL, object is called "unnamed".
*/
extern int klee_range(int begin, int end, const char *name);

/*  klee_int - Construct an unconstrained symbolic integer.
  *
  * \arg name - An optional name, used for identifying the object in messages,
  * output files, etc.
*/
extern int klee_int(const char *name);
extern void klee_assume(uintptr_t condition);

/* klee_assert - Special klee assert macro. 
  * this assert should be used when path consistency
  * across platforms is desired (e.g., in tests).
  * NB: __assert_fail is a klee "special" function
*/
extern void __assert_fail (const char *__assertion, const char *__file,
			   unsigned int __line, const char *__function);
# define klee_assert(expr)                                            \
  ((expr)                                                             \
   ? (void) (0)                                                       \
   : __assert_fail (#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__))  \

size_t nd_size_t(void) {
  size_t res;
  klee_make_symbolic(&res, sizeof(res), "res_size");
<<<<<<< HEAD

=======
  klee_assume(res >= 0);
>>>>>>> 23bee82 (feat(lib): definition of nd() functions for klee)
  return res;
}

bool nd_bool(void) {
  bool res;
  klee_make_symbolic(&res, sizeof(res), "res_bool");

  return res;
}

int nd_int(void) {
  int res;
  klee_make_symbolic(&res, sizeof(res), "res_int");
  
  return res;
<<<<<<< HEAD
}

uint8_t nd_uint8_t(void) {
  uint8_t res;
  klee_make_symbolic(&res, sizeof(res), "res_uint8_t");

  return res;
}

uint16_t nd_uint16_t(void) {
  uint16_t res;
  klee_make_symbolic(&res, sizeof(res), "res_uint16_t");

  return res;
}

uint32_t nd_uint32_t(void) {
  uint32_t res;
  klee_make_symbolic(&res, sizeof(res), "res_uint32_t");
=======
}

uint8_t nd_uint8_t(void) {
  uint8_t res;
  klee_make_symbolic(&res, sizeof(res), "res_uint8_t");
>>>>>>> 23bee82 (feat(lib): definition of nd() functions for klee)

  return res;
}

<<<<<<< HEAD
=======
uint16_t nd_uint16_t(void) {
  uint16_t res;
  klee_make_symbolic(&res, sizeof(res), "res_uint16_t");

  return res;
}

uint32_t nd_uint32_t(void) {
  uint32_t res;
  klee_make_symbolic(&res, sizeof(res), "res_uint32_t");

  return res;
}

>>>>>>> 23bee82 (feat(lib): definition of nd() functions for klee)
uint64_t nd_uint64_t(void) {
  uint64_t res;
  klee_make_symbolic(&res, sizeof(res), "res_uint64_t");

  return res;
}

void __VERIFIER_assert(bool v) {
  klee_assert(v);
}

void __VERIFIER_assume(bool v) {
  klee_assume(v);
}

void __VERIFIER_error(void) {
  klee_assert(0);
}

/* memhavoc - Special function for memory block initialization. 
  * initialize memory content as non-deterministic value.
*/
void memhavoc(void *ptr, size_t size) {
<<<<<<< HEAD
  klee_make_symbolic(ptr, size, "memhavoc");
=======
  for (size_t i = 0; i < size; ++i){
    char *tmp_ptr = (char *)(ptr + i);
    klee_make_symbolic(&tmp_ptr, sizeof(tmp_ptr), "havoc_content");
  }
  return;
>>>>>>> 23bee82 (feat(lib): definition of nd() functions for klee)
}

void *nd_voidp(void) {
  void *res;

  klee_make_symbolic(&res, sizeof(res), "res_void_p_t");

  return res;
}

<<<<<<< HEAD
bool nd_malloc_is_fail(void) { return nd_bool(); }
=======
bool nd_malloc_is_fail(void) { return nd_bool(); }
>>>>>>> 23bee82 (feat(lib): definition of nd() functions for klee)
