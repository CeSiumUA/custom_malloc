#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct block_metadata{
    size_t size;
    struct block_metadata *next;
    int free;
};

#define META_SIZE sizeof(struct block_metadata)

void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nelem, size_t elsize);
void free(void *ptr);
struct block_metadata *get_free_block(struct block_metadata **previous, size_t size);
struct block_metadata *allocate_space(struct block_metadata *last, size_t size);
struct block_metadata *get_segment_block_metadata(void *ptr);

void *global_base_block = NULL;

int main(void){
    uint32_t *mem = calloc(11, sizeof(uint32_t));
    for(int i = 0; i < 11; i++){
        mem[i] = i;
    }

    printf("calloc results:\n");
    for(int i = 0; i < 11; i++){
        printf("%d element of array is: %d\n", i + 1, mem[i]);
    }

    mem = realloc(mem, sizeof(uint32_t) * 20);
    for(int i = 11; i < 20; i++){
        mem[i] = i;
    }

    printf("realloc results:\n");
    for(int i = 0; i < 20; i++){
        printf("%d element of array is: %d\n", i + 1, mem[i]);
    }

    free(mem);

    printf("free results:\n");
    for(int i = 0; i < 20; i++){
        printf("%d element of array is: %d\n", i + 1, mem[i]);
    }
}

void *malloc(size_t size){
    struct block_metadata *block;

    if(size <= 0){
        return NULL;
    }

    if(global_base_block){
        block = allocate_space(NULL, size);
        if(!block){
            return NULL;
        }
        global_base_block = block;
    }
    else{
        struct block_metadata *last = global_base_block;
        block = get_free_block(&last, size);
        if(!block){
            block = allocate_space(last, size);
            if(!block){
                return NULL;
            }
        }
        else{
            block -> free = 0;
        }
    }

    return (block + 1);
}

struct block_metadata *get_free_block(struct block_metadata **last, size_t size){
    struct block_metadata *current = global_base_block;

    while(current && !(current -> free && current -> size >= size)){
        *last = current;
        current = current -> next;
    }

    return current;
}

struct block_metadata *allocate_space(struct block_metadata *last, size_t size){
    struct block_metadata *block;
    block = sbrk(0);

    void *requested = sbrk(size + META_SIZE);

    assert((void*)block == requested);
    if(requested == (void*) -1){
        return NULL;
    }

    if(last){
        last -> next = block;
    }

    block -> size = size;
    block -> next = NULL;
    block -> free = 0;

    return block;
}

struct block_metadata *get_segment_block_metadata(void *ptr){
    return (struct block_metadata *)ptr - 1;
}

void free(void *ptr){
    if(!ptr){
        return;
    }

    struct block_metadata *block_ptr = get_segment_block_metadata(ptr);
    assert(block_ptr -> free == 0);
    block_ptr -> free = 1;
}

void *realloc(void *ptr, size_t size){
    if(!ptr){
        return malloc(size);
    }

    struct block_metadata *block_ptr = get_segment_block_metadata(ptr);
    if(block_ptr -> size >= size){
        
        //TODO Implement splitting

        return ptr;
    }

    void *new_ptr;
    new_ptr = malloc(size);

    if(!new_ptr){
        return NULL;
    }

    memcpy(new_ptr, ptr, block_ptr -> size);
    free(ptr);
    return new_ptr;
}

void *calloc(size_t nelem, size_t elsize){
    size_t size = nelem * elsize;
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}