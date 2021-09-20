#include <stdio.h>
#include <assert.h>
#include <math.h> // for floor and log2
#include "buddy_allocator.h"

// Uso la convenzione lvl 0 ha radice di indice 0

int levelIdx(size_t idx){
  return (int)floor(log2(idx+1)); //floor arrotonda al numero più vicino
}

int buddyIdx(int idx){ //fratello
  if (idx == 0)
    return 0; //0 non ha fatelli
  else if (idx & 0x1)
    return idx + 1; //il fratello di 1 è 2 e viceversa
  return idx - 1;
}

int parentIdx(int idx){
  return (idx-1)/2; //il padre di 1 è 0 di 3 è 1 e così via
}

int firstIdx(int lvl){
  return (1 << lvl) - 1; //il primo indice del livello 0 è 0, del livello 1 è 1, del livello 2 è 3 e cosi via
} // return (2^lvl)-1;

//ritorna l'offset dal primo indice del livello a cui si trova (quanto dista dal primo elemento del livello)
int startIdx(int idx){
  return (idx-(firstIdx(levelIdx(idx)))); // se idx=36 -> livello:5 e first(5)=31 -> offset: 36-31=5
}

// stampo a schermo l'albero della bitmap
void Bitmap_print(BitMap *bit_map){
  int remain_to_print = 0;
  int lvl = -1;
  int tot_lvls = levelIdx(bit_map->num_bits) - 1;
  if (tot_lvls>6) tot_lvls = 6; //evito di stampare più di 6 livelli per visualizzare l'albero al meglio 
  for (int i = 0; i < bit_map->num_bits; i++){
    if (remain_to_print == 0){ 
      if(lvl==tot_lvls) break;
      printf("\nLivello %d (bit iniziale %d):\t", ++lvl, i);
      for (int j = 0; j < (1 << tot_lvls) - (1 << lvl); j++) printf(" "); //print spazi per formattazione
      remain_to_print = 1 << lvl; // (2^lvl)
    }
    printf("%d ", BitMap_bit(bit_map, i));
    remain_to_print--;
  }
  printf("\n");
}

void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,
                         char* alloc_buf, // buffer per l'allocator
                         int alloc_buf_size,
                         char* bitmap_buf, // buffer per la bitmap
                         int bitmap_buf_size,
                         int min_bucket_size) {

  // we need room also for level 0
  assert("Min bucket troppo piccolo" && min_bucket_size >= 8); //troppo piccolo altrimenti
   
  assert ("Numero di livelli maggiore del massimo possibile!" && num_levels<MAX_LEVELS);
  
  //generazione numero di bit per la bit_map, ogni bit è un buddy di minbucket size
  int num_bits = (1 << (num_levels + 1)) - 1 ; // maximum number of allocations
  
  assert ("Memoria per la bitmap non sufficente!" && bitmap_buf_size>=BitMap_getBytes(num_bits));
  
  //controllo in più non presente nel codice originale, nel caso non si usi una potenza di 2 precisa si riuscirà ad usare meno memoria della disponibile
  if (levelIdx(alloc_buf_size) != log2(alloc_buf_size)){
    printf("****ATTENZIONE IL BUFFER NON È UNA POTENZA DI DUE PRECISA E IL BUDDY NON LO USERA' A PIENO,\n");
    printf("RIUSCIRAI AD UTILIZZARE SOLAMENTE %d BYTES DI %d FORNITI****\n", min_bucket_size << num_levels, alloc_buf_size);
    alloc_buf_size = min_bucket_size << num_levels; //la dimensione massima effettiva che può gestire
  }

  // we need room also for level 0
  alloc->num_levels=num_levels;
  alloc->buffer = alloc_buf;
  alloc->buffer_size = alloc_buf_size;
  alloc->min_bucket_size=min_bucket_size;


  printf("BUDDY INITIALIZING\n");
  printf("\tmanaged memory: %d bytes\n", alloc_buf_size); // (1<<num_levels)*min_bucket_size  (2^5)*8 = 256 bytes
  printf("\tlevels: %d\n", num_levels);
  printf("\tmin_bucket size: %d\n", min_bucket_size);
  printf("\tbits_bitmap: %d\n", num_bits);
  printf("\tbitmap memory: %d bytes usati di %d bytes forniti \n", BitMap_getBytes(num_bits), bitmap_buf_size);

  BitMap_init(&alloc->bitmap, num_bits, bitmap_buf);
  printf("Bitmap appena allocata:");
  Bitmap_print(&alloc->bitmap);
};


//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {
  size += sizeof(int); //sizeof(int) byte vengono usati per salvare l'indice della bitmap
  
  if (alloc->buffer_size < size) {
    printf("\nIl blocco da allocare con size %d è più grande di tutta la memoria disponibile!\n", size);
    return NULL;
  }
  // we determine the level of the page 
  // parto dal basso
  int lv_new_block = alloc->num_levels;
  int size_start = alloc->min_bucket_size;
  for (int i = 0; i < alloc->num_levels; i++){
    if (size_start >= size)
      break;
    else{
      size_start *= 2;
      lv_new_block--; //salgo di livello
    }
  }

  printf("\nAlloco il nuovo blocco di dimensione %d al livello %d utilizzando un blocco di dimensioni %d\n", size, lv_new_block, size_start);
  //scandisco da firstidx del livello per trovare un blocco libero
  int free_idx = -1;
  int i; 
  for (i= firstIdx(lv_new_block); i < firstIdx(lv_new_block + 1); i++){
    if (!BitMap_bit(&alloc->bitmap, i)){ //se non è occupato
      free_idx = i;
      break;
    }
  }
  //se non lo trovo ritorno NULL
  if (free_idx < 0){
    printf("Non c'è più memoria disponibile!\n");
    return NULL;
  }

  Set_status_parents(&alloc->bitmap, free_idx, 1);
  Set_status_children(&alloc->bitmap, free_idx, 1);
  //devo generare l'indirizzo da restituire, salvo l'indice così poi che potrò farne la free facilmente
  char *da_restituire = alloc->buffer + startIdx(free_idx) * size_start;
  ((int *)da_restituire)[0] = free_idx;
  printf("sto restituendo con indice %d il puntatore %p\n", free_idx, da_restituire + sizeof(int));
  printf("Bitmap dopo l'allocazione:");
  Bitmap_print(&alloc->bitmap);
  return (void *)(da_restituire + sizeof(int));
}


//releases allocated memory
void BuddyAllocator_free(BuddyAllocator *alloc, void *mem){
  printf("\nFreeing %p\n", mem);

  assert("Non posso fare il free di NULL" && mem); //deve essere diverso da null
  // recuperiamo l'indice dal puntatore
  int *p = (int *)mem;
  int idx_to_free = p[-1];

  printf("indice da liberare %d\n", idx_to_free);
  //sanity check deve essere un buddy corretto, calcolo la dim di un buddy a quel livello
  int dim_lvl = alloc->min_bucket_size * (1 << (alloc->num_levels - levelIdx(idx_to_free)));
  char *p_to_free = alloc->buffer + startIdx(idx_to_free) * dim_lvl;
  assert("Puntatore non allineato" && (int *)p_to_free == &p[-1]);
  //bisogna evitare double free
  assert("Double free" && BitMap_bit(&alloc->bitmap, idx_to_free)  );

  Set_status_children(&alloc->bitmap, idx_to_free, 0);
  Bitmap_merge(&alloc->bitmap, idx_to_free);

  printf("Bitmap dopo la free:");
  Bitmap_print(&alloc->bitmap);
}

void Set_status_parents(BitMap *bit_map, int bit_num, int status){
  BitMap_setBit(bit_map, bit_num, status);
  if (bit_num != 0)
    Set_status_parents(bit_map, parentIdx(bit_num), status);
}

void Set_status_children(BitMap *bit_map, int bit_num, int status){
  if (bit_num < bit_map->num_bits){
    BitMap_setBit(bit_map, bit_num, status);
    Set_status_children(bit_map, bit_num * 2 + 1, status);
    Set_status_children(bit_map, bit_num * 2 + 2, status);
  }
}

void Bitmap_merge(BitMap *bitmap, int idx){
  assert("Non puoi fare il merge su un bit libero" && !BitMap_bit(bitmap, idx)); //deve essere libero
  if (idx == 0) return;
  int idx_buddy = buddyIdx(idx);
  if (!BitMap_bit(bitmap, idx_buddy)){
    printf("Sto facendo il merge dei buddy %d e %d al livello %d\n", idx, idx_buddy, levelIdx(idx));
    int parent_idx = parentIdx(idx);
    BitMap_setBit(bitmap, parent_idx, 0);
    Bitmap_merge(bitmap, parent_idx);
  }
}