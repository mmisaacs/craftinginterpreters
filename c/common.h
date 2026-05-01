//> Chunks of Bytecode common-h
#ifndef clox_common_h
#define clox_common_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
//> A Virtual Machine define-debug-trace

//> Optimization define-nan-boxing
#define NAN_BOXING
//< Optimization define-nan-boxing
//> Compiling Expressions define-debug-print-code
#define DEBUG_PRINT_CODE
//< Compiling Expressions define-debug-print-code
#define DEBUG_TRACE_EXECUTION
//< A Virtual Machine define-debug-trace
//> Garbage Collection define-stress-gc

#define DEBUG_STRESS_GC
//< Garbage Collection define-stress-gc
//> Garbage Collection define-log-gc
#define DEBUG_LOG_GC
//< Garbage Collection define-log-gc
// define safe amount of local variables
#define MAX_LOCALS 1024
#define STACK_MAX (MAX_LOCALS * 2)
//> Local Variables uint8-count
#define UINT8_COUNT (UINT8_MAX + 1)
//< Local Variables uint8-count

#endif
//> omit
// In the book, we show them defined, but for working on them locally,
// we don't want them to be.
#undef DEBUG_PRINT_CODE
#undef DEBUG_TRACE_EXECUTION
#undef DEBUG_STRESS_GC
#undef DEBUG_LOG_GC
//< omit
typedef struct Block {
	size_t size;        // Size of this data block
	bool isFree;        // Is it available?
	struct Block* next; // Next block in our big slab
} Block;

static uint8_t* globalHeap = NULL;
static Block* freeList = NULL;
#define HEAP_SIZE 1024 * 1024 // 1MB personal heap