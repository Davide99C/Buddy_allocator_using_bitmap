#include "buddy_allocator.h"
#include <stdio.h>

#define BUFFER_SIZE 256
#define BUDDY_LEVELS 5
#define MEMORY_SIZE 128
#define MIN_BUCKET_SIZE (MEMORY_SIZE>>(BUDDY_LEVELS))

char buffer[BUFFER_SIZE]; 
char memory[MEMORY_SIZE]; // for the bitmap

BuddyAllocator alloc;

int main(int argc, char** argv) {

  // we initialize the allocator
  printf("init... \n");
  BuddyAllocator_init(&alloc, 
                      BUDDY_LEVELS,
                      buffer,  // buffer per l'allocator
                      BUFFER_SIZE,
                      memory,  // buffer per la bitmap
                      MEMORY_SIZE,
                      MIN_BUCKET_SIZE);
  printf("DONE\n");

  int* p1=BuddyAllocator_malloc(&alloc, sizeof(int));
  int* p2=BuddyAllocator_malloc(&alloc, sizeof(int));
  int* p3=BuddyAllocator_malloc(&alloc, sizeof(int));
  int* p4=BuddyAllocator_malloc(&alloc, sizeof(int));
  int* p5=BuddyAllocator_malloc(&alloc, sizeof(int));
  BuddyAllocator_free(&alloc, p4);
  BuddyAllocator_free(&alloc, p5);
  BuddyAllocator_free(&alloc, p1);
  BuddyAllocator_free(&alloc, p2);
  BuddyAllocator_free(&alloc, p3); 
  
}
