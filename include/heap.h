#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stdbool.h>

#define MACHINE_WORD_64  // comment it out if your machine is 32-bit

#ifdef MACHINE_WORD_64
typedef uint64_t unsigned_int_type;
typedef int64_t signed_int_type;
#else
typedef uint32_t unsigned_int_type;
typedef int32_t signed_int_type;
#endif


typedef struct
{
    void* storage;
    unsigned_int_type storage_size;
    uint32_t item_size;
    unsigned_int_type items_count;
    // comparator should return n where n>0 if item1>item2, n==0 if item1==item2 and n<0 if item1<item2
    int (*comparator)(const void* item1, const void* item2);
    bool max_heap;
}
heap;

void heap_init(
    heap* self, void* storage, int(*comparator)(const void*, const void*),
    bool max_heap, uint32_t item_size, uint64_t storage_size
);

/// Return true if inserted successfully
bool heap_insert(heap* self, void* item);

/// Return true if popped successfully
bool heap_pop(heap* self, void* item_receiver);

void heap_inplace_heapify(
    heap* dest, void* array, uint64_t size, uint32_t item_size,
    int(*comparator)(const void*, const void*), bool max_heap
);

void heap_inplace_sort(
    void* array, uint64_t size, uint32_t item_size,
    int(*comparator)(const void*, const void*), bool ascending
);

#endif
