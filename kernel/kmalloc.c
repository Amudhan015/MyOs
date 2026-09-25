#include <kernel/int.h>
#include <kernel/kmalloc.h>
#include <kernel/pmm.h>
#include <kernel/serial.h>

#define FRAME_SIZE 4096
#define NUM_SIZE_CLASSES 8
#define MAX_FRAMES 1048576 // must match pmm.c's MAX_FRAMES

static const u32 size_classes[NUM_SIZE_CLASSES] = {16,  32,  64,   128,
                                                   256, 512, 1024, 2048};

typedef struct free_chunk {
    struct free_chunk* next;
} free_chunk_t;

typedef struct {
    u32 chunk_size;
    free_chunk_t* free_list;
} slab_cache_t;

static slab_cache_t caches[NUM_SIZE_CLASSES];

// Maps a frame index -> which cache owns that frame (0xFF = not slab-owned).
// This is what lets kfree() figure out an object's size from a bare pointer.
static u8 frame_owner[MAX_FRAMES];

void kmalloc_init(void) {
    for (int i = 0; i < NUM_SIZE_CLASSES; i++) {
        caches[i].chunk_size = size_classes[i];
        caches[i].free_list = 0;
    }
    for (u32 i = 0; i < MAX_FRAMES; i++) {
        frame_owner[i] = 0xFF;
    }
    serial_writestring("kmalloc initialized\n");
}

// Pull one more physical frame from the PMM, slice it into chunk_size
// pieces, and thread them all onto this cache's free list.
static void cache_grow(int cache_idx) {
    slab_cache_t* cache = &caches[cache_idx];
    u32 frame_addr = pmm_alloc_frame();

    if (frame_addr == 0) {
        serial_writestring("kmalloc: out of physical memory\n");
        return;
    }

    frame_owner[frame_addr / FRAME_SIZE] = (u8)cache_idx;

    u32 chunks_per_frame = FRAME_SIZE / cache->chunk_size;
    for (u32 i = 0; i < chunks_per_frame; i++) {
        free_chunk_t* chunk =
            (free_chunk_t*)(frame_addr + i * cache->chunk_size);
        chunk->next = cache->free_list;
        cache->free_list = chunk;
    }
}

static int size_class_for(u32 size) {
    for (int i = 0; i < NUM_SIZE_CLASSES; i++) {
        if (size <= size_classes[i])
            return i;
    }
    return -1; // too big for any cache — see note below
}

void* kmalloc(u32 size) {
    int idx = size_class_for(size);
    if (idx < 0) {
        serial_writestring("kmalloc: requested size too large for any cache\n");
        return 0;
    }

    slab_cache_t* cache = &caches[idx];
    if (!cache->free_list) {
        cache_grow(idx);
        if (!cache->free_list)
            return 0; // cache_grow failed (OOM)
    }

    free_chunk_t* chunk = cache->free_list;
    cache->free_list = chunk->next;
    return (void*)chunk;
}

void kfree(void* ptr) {
    if (!ptr)
        return;

    u32 addr = (u32)ptr;
    u32 frame_idx = addr / FRAME_SIZE;

    if (frame_idx >= MAX_FRAMES) {
        serial_writestring("kfree: pointer out of range\n");
        return;
    }

    u8 cache_idx = frame_owner[frame_idx];
    if (cache_idx == 0xFF) {
        serial_writestring("kfree: pointer not owned by any slab cache\n");
        return;
    }

    slab_cache_t* cache = &caches[cache_idx];

    // Double-free check: walk the existing free list and refuse if this
    // exact pointer is already on it.
    for (free_chunk_t* c = cache->free_list; c != 0; c = c->next) {
        if (c == (free_chunk_t*)ptr) {
            serial_writestring("kfree: double-free detected\n");
            return;
        }
    }

    free_chunk_t* chunk = (free_chunk_t*)ptr;
    chunk->next = cache->free_list;
    cache->free_list = chunk;
}
