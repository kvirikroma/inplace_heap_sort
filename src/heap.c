#include <string.h>

#include "heap.h"


void heap_init(
    heap* self, void* storage, int(*comparator)(const void*, const void*),
    bool max_heap, uint32_t item_size, uint64_t storage_size
)
{
    self->storage = storage;
    self->comparator = comparator;
    self->max_heap = max_heap;
    self->item_size = item_size;
    self->storage_size = storage_size;
    self->items_count = 0;
}

static inline void exchange(void* item1, void* item2, uint32_t item_size)
{
    uint8_t* p;
    uint8_t* q;
    uint8_t* const sentry = (uint8_t*)item1 + item_size;

    for (p = item1, q = item2; p < sentry; ++p, ++q) {
        uint8_t t = *p;
        *p = *q;
        *q = t;
    }
}

/// -1 if does not exist
static inline signed_int_type get_left(heap* self, unsigned_int_type index)
{
    index *= 2;
    index += 1;
    if (index > self->items_count)
    {
        return -1;
    }
    return (signed_int_type)index;
}

/// -1 if does not exist
static inline signed_int_type get_right(heap* self, unsigned_int_type index)
{
    index *= 2;
    index += 2;
    if (index > self->items_count)
    {
        return -1;
    }
    return (signed_int_type)index;
}

/// -1 if does not exist
static inline signed_int_type get_parent(unsigned_int_type index)
{
    return (index / 2) - (signed_int_type)(!(bool)(index % 2));
}

static inline void flow_up(heap* self)
{
    unsigned_int_type current_item_index = self->items_count - 1;
    while (true)
    {
        unsigned_int_type parent_item_index = get_parent(current_item_index);
        if (parent_item_index == -1)
        {
            return;
        }
        void* parent_item = (void*)((uint8_t*)self->storage + (parent_item_index * self->item_size));
        void* current_item = (void*)((uint8_t*)self->storage + (current_item_index * self->item_size));
        int comparing_result = self->comparator(current_item, parent_item);
        if (((comparing_result > 0) && (self->max_heap)) || ((comparing_result < 0) && (!self->max_heap)))
        // ((current_item > parent_item) and (max_heap)) or ((current_item < parent_item) and (not max_heap))
        {
            exchange(current_item, parent_item, self->item_size);
            current_item_index = parent_item_index;
        }
        else
        {
            return;
        }
    }
}

static inline void flow_down(heap* self)
{
    signed_int_type current_item_index = 0;
    while (true)
    {
        signed_int_type left_index = get_left(self, current_item_index);
        signed_int_type right_index = get_right(self, current_item_index);
        void* current_item = (void*)((uint8_t*)self->storage + (current_item_index * self->item_size));
        void* left = (void*)((uint8_t*)self->storage + (left_index * self->item_size));
        // left is unsafe ptr; use only if left_index != -1
        void* right = (void*)((uint8_t*)self->storage + (right_index * self->item_size));
        // right is unsafe ptr; use only if right_index != -1
        if ((left_index != -1) && (right_index != -1))
        {
            int lr_comparing_result = self->comparator(left, right);
            if (((lr_comparing_result > 0) && self->max_heap) || ((lr_comparing_result < 0) && !self->max_heap))
            {
                right_index = -1;
            }
            else
            {
                left_index = -1;
            }
        }
        bool should_exit = true;
        if (left_index != -1)
        {
            int comparing_result = self->comparator(current_item, left);
            if (((comparing_result > 0) && (!self->max_heap)) || ((comparing_result < 0) && (self->max_heap)))
            {
                exchange(left, current_item, self->item_size);
                current_item_index = left_index;
                should_exit = false;
            }
        }
        else if (right_index != -1)
        {
            int comparing_result = self->comparator(current_item, right);
            if (((comparing_result > 0) && (!self->max_heap)) || ((comparing_result < 0) && (self->max_heap)))
            {
                exchange(right, current_item, self->item_size);
                current_item_index = right_index;
                should_exit = false;
            }
        }
        if (should_exit)
        {
            return;
        }
    }
}

bool heap_insert(heap* self, void* item)
{
    if (self->items_count == self->storage_size)
    {
        return false;
    }
    void* dest = (void*)((uint8_t*)self->storage + (self->item_size * self->items_count));
    memmove(dest, item, self->item_size);
    self->items_count++;
    flow_up(self);
    return true;
}

bool heap_pop(heap* self, void* item_receiver)
{
    if (self->items_count == 0)
    {
        return false;
    }
    memcpy(item_receiver, self->storage, self->item_size);
    self->items_count--;
    if (self->items_count)
    {
        memcpy(
            self->storage,
            (void*)(
                (uint8_t*)self->storage + (self->item_size * self->items_count)
            ),
            self->item_size
        );
        flow_down(self);
    }
    return true;
}

void heap_inplace_heapify(
    heap* dest, void* array, uint64_t size, uint32_t item_size,
    int(*comparator)(const void*, const void*), bool max_heap
)
{
    heap_init(dest, array, comparator, max_heap, item_size, size);
    for (unsigned_int_type i = 0; i < size; i++)
    {
        heap_insert(dest, (void*)((uint8_t*)array + (item_size * i)));
    }
}

void heap_inplace_sort(
    void* array, uint64_t size, uint32_t item_size,
    int(*comparator)(const void*, const void*), bool ascending
)
{
    heap sorter;
    heap_inplace_heapify(&sorter, array, size, item_size, comparator, ascending);
    for (signed_int_type i = size - 1; i >= 0; i--)
    {
        uint8_t tmp_storage[item_size];
        heap_pop(&sorter, (void*)tmp_storage);
        memcpy((void*)((uint8_t*)array + (item_size * i)), tmp_storage, item_size);
    }
}
