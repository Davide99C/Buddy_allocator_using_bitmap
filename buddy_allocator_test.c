#include "buddy_allocator.h"
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 260
#define BUDDY_LEVELS 5
#define MEMORY_SIZE 128
#define MIN_BUCKET_SIZE (BUFFER_SIZE>>(BUDDY_LEVELS))

//char buffer[BUFFER_SIZE]; 
char memory[MEMORY_SIZE]; // for the bitmap

BuddyAllocator alloc;

int main(int argc, char** argv) {

  int mybuf_size = BUFFER_SIZE;
  int mymin_bucket_size = MIN_BUCKET_SIZE;
  int mylevels = BUDDY_LEVELS;

  if (argc > 1) {
    mybuf_size = atoi(argv[1]);
    printf("Hai scelto un buffer di dimensione %d\n", mybuf_size);
    if (argc > 2) {
      mylevels = atoi(argv[2]);
      printf("Hai scelto un numero di livelli pari a %d\n", mylevels);
    }
  }
  
  char buffer[mybuf_size]; 

  if (mybuf_size != BUFFER_SIZE) {
    mymin_bucket_size = (mybuf_size >> (mylevels));
  }

  if (mylevels > 6) printf("Poichè il numero di livelli è troppo elevato, per ragioni di visualizzazione verrà stampato a schermo l'allocator fino al livello 6\n");

  // we initialize the allocator
  printf("init... \n");
  int init = BuddyAllocator_init(&alloc, 
                      mylevels,
                      buffer,  // buffer per l'allocator
                      mybuf_size,
                      memory,  // buffer per la bitmap
                      MEMORY_SIZE,
                      mymin_bucket_size);
  printf("DONE\n");

  if (init == 0) return 0; // se l'inizializzazione non è andata a buon fine non proseguire

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
  if (mybuf_size >= 512) BuddyAllocator_free(&alloc, p3);
  // BuddyAllocator_free(&alloc, p1); // double free 

  printf("\n<---- FINE TEST ---->\n");

}
