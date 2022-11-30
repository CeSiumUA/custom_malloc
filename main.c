#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

void *malloc(size_t size);

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