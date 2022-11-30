#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

struct block_metadata{
    size_t size;
    struct blck_md *next;
    int free;
};

#define META_SIZE sizeof(struct block_metadata)

void *malloc(size_t size);
struct block_metadata *get_free_block(struct block_metadata **previous, size_t size);

void *global_current_block = NULL;

int main(void){
    uint32_t *mem = malloc(sizeof(uint32_t) * 11);
    for(int i = 0; i < 11; i++){
        mem[i] = 11 - i;
    }

    printf("results:\n");

    for(int i = 0; i < 11; i++){
        printf("%d element of array is: %d\n", i, mem[i]);
    }
}

void *malloc(size_t size){
    void *heapPointer = sbrk(0);

    void *requested = sbrk(size);

    if(requested == (void*) -1){
        return NULL;
    }
    else{
        assert(heapPointer == requested);
        return heapPointer;
    }
}

struct block_metadata *get_free_block(struct block_metadata **previous, size_t size){
    struct block_metadata *current = global_current_block;

    while(current && !(current -> free && current -> size >= size)){
        *previous = current;
        current = current -> next;
    }

    return current;
}

struct block_metadata *allocate_space(struct block_metadata *previous, size_t size){
    struct block_metadata *block;
    block = sbrk(0);

    void *requested = sbrk(size + META_SIZE);

    assert((void*)block == requested);
    if(requested == (void*) -1){
        return NULL;
    }

    if(previous){
        previous -> next = block;
    }

    block -> size = size;
    block -> next = NULL;
    block -> free = 0;
    
    return block;
}