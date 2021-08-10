#include <assert.h>
#include "bitmap.h"

// returns the number of bytes to store bits booleans
int BitMap_getBytes(int bits){
    int eccesso = 0;
    if ((bits%8)!=0) eccesso = 1;
  return bits/8 + eccesso; // prendo il numero di bytes (per eccesso) per contenere tutti i bit
}

// initializes a bitmap on an external array
void BitMap_init(BitMap* bit_map, int num_bits, char* buffer){
  bit_map->buffer=buffer;
  bit_map->num_bits=num_bits;
  bit_map->buffer_size=BitMap_getBytes(num_bits);
}

// sets a the bit bit_num in the bitmap
// status= 0 or 1
void BitMap_setBit(BitMap* bit_map, int bit_num, int status){
  // get byte
  int byte_num=bit_num>>3;
  assert(byte_num<bit_map->buffer_size);
  int bit_in_byte=bit_num&0x07;
  if (status) {
    bit_map->buffer[byte_num] |= (1<<bit_in_byte);
  } else {
    bit_map->buffer[byte_num] &= ~(1<<bit_in_byte);
  }
}

// inspects the status of the bit bit_num
int BitMap_bit(const BitMap* bit_map, int bit_num){
  int byte_num=bit_num>>3; 
  assert(byte_num<bit_map->buffer_size);
  int bit_in_byte=bit_num&0x07;
  return (bit_map->buffer[byte_num] & (1<<bit_in_byte))!=0;
}
