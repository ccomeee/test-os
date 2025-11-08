#include "mem.h"
#include "errno.h"
#include <stdint.h>

typedef struct block_header {
    size_t size;
    int free;
    struct block_header *next;
} block_header_t;

#define HEAP_SIZE (1024 * 1024)
#define ALIGNMENT (sizeof(void *))
#define MIN_SPLIT (sizeof(block_header_t) + ALIGNMENT)

static unsigned char heap_area[HEAP_SIZE];
static size_t heap_used = 0;
static block_header_t *block_list_head = NULL;
static block_header_t *block_list_tail = NULL;

static size_t align_size(size_t size) {
    if (size == 0) {
        return ALIGNMENT;
    }
    size_t rem = size % ALIGNMENT;
    return rem == 0 ? size : size + (ALIGNMENT - rem);
}

static block_header_t *find_fit(size_t size) {
    block_header_t *block = block_list_head;
    while (block) {
        if (block->free && block->size >= size) {
            return block;
        }
        block = block->next;
    }
    return NULL;
}

static void split_block(block_header_t *block, size_t size) {
    if (!block) {
        return;
    }

    if (block->size <= size + MIN_SPLIT) {
        return;
    }

    unsigned char *new_block_addr = (unsigned char *)block + sizeof(block_header_t) + size;
    block_header_t *new_block = (block_header_t *)new_block_addr;
    new_block->size = block->size - size - sizeof(block_header_t);
    new_block->free = 1;
    new_block->next = block->next;

    block->size = size;
    block->next = new_block;

    if (block_list_tail == block) {
        block_list_tail = new_block;
    }
}

static block_header_t *extend_heap(size_t size) {
    size_t total_size = sizeof(block_header_t) + size;
    if (heap_used + total_size > HEAP_SIZE) {
        return NULL;
    }

    block_header_t *block = (block_header_t *)(heap_area + heap_used);
    block->size = size;
    block->free = 0;
    block->next = NULL;

    if (!block_list_head) {
        block_list_head = block;
        block_list_tail = block;
    } else {
        block_list_tail->next = block;
        block_list_tail = block;
    }

    heap_used += total_size;
    return block;
}

static block_header_t *find_previous(block_header_t *block) {
    if (block == block_list_head) {
        return NULL;
    }
    block_header_t *current = block_list_head;
    while (current && current->next != block) {
        current = current->next;
    }
    return current;
}

static void coalesce_forward(block_header_t *block) {
    while (block && block->next && block->next->free) {
        block_header_t *next = block->next;
        block->size += sizeof(block_header_t) + next->size;
        block->next = next->next;
        if (block_list_tail == next) {
            block_list_tail = block;
        }
    }
}

static void coalesce(block_header_t *block) {
    if (!block) {
        return;
    }

    coalesce_forward(block);

    block_header_t *prev = find_previous(block);
    if (prev && prev->free) {
        coalesce_forward(prev);
    }
}

void *malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    size = align_size(size);

    block_header_t *block = find_fit(size);
    if (!block) {
        block = extend_heap(size);
        if (!block) {
            errno = ENOMEM;
            return NULL;
        }
    } else {
        block->free = 0;
        split_block(block, size);
    }

    return (void *)((unsigned char *)block + sizeof(block_header_t));
}

void free(void *ptr) {
    if (!ptr) {
        return;
    }

    block_header_t *block = (block_header_t *)((unsigned char *)ptr - sizeof(block_header_t));
    block->free = 1;
    coalesce(block);
}

void *sbrk(intptr_t increment) {
    if (increment == 0) {
        return heap_area + heap_used;
    }

    if (increment < 0) {
        errno = EINVAL;
        return (void *)-1;
    }

    size_t aligned = align_size((size_t)increment);
    if (heap_used + aligned > HEAP_SIZE) {
        errno = ENOMEM;
        return (void *)-1;
    }

    void *prev_break = heap_area + heap_used;
    heap_used += aligned;
    return prev_break;
}

void *mmap(void *addr, size_t length, int prot, int flags, int fd, size_t offset) {
    (void)addr;
    (void)length;
    (void)prot;
    (void)flags;
    (void)fd;
    (void)offset;
    errno = ENOSYS;
    return NULL;
}