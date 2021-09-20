#include "buddy_allocator.h"
#include <stdio.h>

#define BUFFER_SIZE 260
#define BUDDY_LEVELS 5
#define MEMORY_SIZE 128
#define MIN_BUCKET_SIZE (BUFFER_SIZE>>(BUDDY_LEVELS))

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

  printf("\n<---- PRIMA ALLOCAZIONE ---->\n");

  int* p1=BuddyAllocator_malloc(&alloc, 12);
  int* p2=BuddyAllocator_malloc(&alloc, 4);
  int* p3=BuddyAllocator_malloc(&alloc, 4);
  int* p4=BuddyAllocator_malloc(&alloc, 4);
  int* p5=BuddyAllocator_malloc(&alloc, 4);
  
  BuddyAllocator_free(&alloc, p4);
  BuddyAllocator_free(&alloc, p5);
  BuddyAllocator_free(&alloc, p1);
  BuddyAllocator_free(&alloc, p2);
  BuddyAllocator_free(&alloc, p3); 
  
  printf("\n<---- SECONDA ALLOCAZIONE ---->\n");
  
  p1=BuddyAllocator_malloc(&alloc, sizeof(int)*9);
  p2=BuddyAllocator_malloc(&alloc, 13);
  p5=BuddyAllocator_malloc(&alloc, sizeof(int));
  p3=BuddyAllocator_malloc(&alloc, sizeof(int)*12);
  p4=BuddyAllocator_malloc(&alloc, sizeof(int));

  BuddyAllocator_free(&alloc, p1);
  BuddyAllocator_free(&alloc, p2);
  BuddyAllocator_free(&alloc, p3);   
  BuddyAllocator_free(&alloc, p5);
  BuddyAllocator_free(&alloc, p4);

  printf("\n<---- TEST FALLIMENTO ---->\n");

  p4=BuddyAllocator_malloc(&alloc, 10000000); // fallisce perchè blocco di dimensioni maggiori alla memoria disponibile
  p1=BuddyAllocator_malloc(&alloc, 200); // "alloco tutta la bitmap"
  p3=BuddyAllocator_malloc(&alloc, 8); // fallisce perchè non c'è più memoria disponibile
  BuddyAllocator_free(&alloc, p1);
  // BuddyAllocator_free(&alloc, p1); // double free 

  printf("\n<---- FINE TEST ---->\n");

}
