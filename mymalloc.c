// PARAS PATANGE: u7291471

//Checklist
// Fence post - Attempted - Boundary Tags - Lines 91 - 93
// Constant time coalescing with boundary tags - Attempted
// factor header lists - Attempted Logic

#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/mman.h>

#include "mymalloc.h"

typedef struct Header blockHeader;

// first word of the block hence "header" - stores size of block
typedef struct Header {
  size_t size;
  bool allocated;
  blockHeader *next;
  blockHeader *prev;
} Header;

typedef struct Boundary_Tag {
  size_t size;
  bool allocated;
} Boundary_Tag;

typedef struct full_block {
  bool allocated;
  size_t size;
} full_block;

// Size of meta-data per Free Block
const size_t kBlockMetadataSize_full = sizeof(Header) + sizeof(full_block);
// Maximum allocation size (16 MB) block
const size_t kMaxAllocationSize = (16ull << 20) - kBlockMetadataSize_full;
// Size of meta-data per Boundary_Tag
const size_t kBlockMetadataSize_Tag = sizeof(Boundary_Tag);
// Size of meta-data per Block
const size_t kBlockMetaDataTag_Size = sizeof(full_block);
// Size of header meta-data
const size_t kBlockMetaDataHeader_Size = sizeof(Header);
// Starting address of our heap

// const size_t kBlockMetaDataBoundary_tag_Size = sizeof(Boundary_tag);  Doesnt Work/Not Needed
// // Starting address of our heap

//
static blockHeader *rootHeader = NULL;


/// Round up size From lab
//This is a function which was used in Lab 6 which I have used to calculate/recalculate overall size of payload data,
// this is done using the original size and its alignment, which is then subtracted from a temporary variable, and a bitwise alignment
inline static size_t round_up(size_t size, size_t alignment) {
  const size_t mask = alignment - 1;
  return (size + mask) & ~mask;
}


/// Get data pointer of a block From lab. 
//This is a function which when given the block gets the address of a block, or given the address gives the block in memory.
inline static void *block_to_data(full_block *block) {
  return (void *)(((size_t)block) + kBlockMetadataSize_full);
}


/// Get the block of a data pointer From lab. This function is responsible for implementing the First fit placement policy, 
//this function when given a target size iterates through the linked list to find the next header chunk which can accommodate the new data.
// After which it checks that the first new chunk is NOT the root header and returns the address.
inline static blockHeader *data_to_block(void *ptr) {
  return (blockHeader *)(((size_t)ptr) - kBlockMetadataSize_full);
}

/// Get next or prev block
inline static blockHeader *obtain_proceeding_block(blockHeader *header, size_t size) { // change
  blockHeader *head_ptr;
  for (head_ptr = rootHeader->next; head_ptr != rootHeader && head_ptr->size < size; head_ptr = head_ptr->next);{ //Inspiration derived from https://www.youtube.com/watch?v=8REdBgL_bXA
    if (head_ptr != rootHeader){ 
    return head_ptr;
  }
  else{
    return NULL;
  }
  } // if next header's size is smaller than the specified, then return next one.
}

// constant_coalesce(kBlockMetaDataHeader_Size *header) {
//   //Boundary_tag *boundary_tag = Boundary_tag* size of header - sizeof the Boundary_tag;
//    // update header after take in account boundary tage size too
//   // Gotta update header after too
//    - 
//   } else if (boundary_tag->allocated == false) { //Free block leftSs } 
//     -
//   } else if (header_Next->allocated == false) { //Free block right }
//     - 
//   } else { //Error 
//     return Null;
//   }
// }

//This is a function which splits an existing chunk into a smaller piece, 
//so the Dynamic Memory Allocator can better allocate and deallocate memory.
static Header *split_chunk(Header *header, size_t size) {
  // Unallocated blocks get split
  // splitting block into smaller block to save space
  assert(!header->allocated); //asserts if the header is allocated
  size_t total_size = header->size; // save size of whole block  similar to Lab6
  Header *header_N = NULL; 
  size_t new_size = total_size - size - kBlockMetaDataHeader_Size; // new block size - account for header data space

  //Case where the new_Size < less then MetaDataHeader_Size not needed as wont be excuted ------------------- Report Element
  header_N = header;
  header_N->next = header->next;
  header_N->prev = header->prev; 
  header_N->size = new_size;
  header_N->allocated = false;
  return header_N;
}



/// Acquire more memory from OS
//Gets the chunk from the OS, it gets exactly 1 more chunk which is to be used by other functions.
static blockHeader *acquire_chunk(int factor) {
  // Acquire one more chunk from OS
  size_t new_chunk_size = ARENA_SIZE * factor;
  blockHeader *header;
  header = mmap(NULL, new_chunk_size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
  
  if (header == -1) { //checks if the header is invalid
    return NULL;
  }
  
  //updates variables to that of the header and then returns it
  header->prev = header;
  header->next = header;
  header->size = new_chunk_size;
  header->allocated = false;

  return header;
}

// from lab 6
/// Check if Block pointed by `block` is in our mmaped memory
static bool in_mmaped_memory(blockHeader *block) {
  size_t block_sz = (size_t) block;
  size_t start_sz = (size_t) rootHeader;
  size_t end_sz = start_sz + kMaxAllocationSize;
  if (rootHeader == NULL)
    // if we haven't mmaped anything then it is not in our memory
    return false;
  if (block_sz < start_sz)
    // if the block is before our rootHeader then it is not in our memory
    return false;
  if (block_sz > end_sz)
    // if the block is after our end then it is not in our memory
    return false;
  return true;
}

int *ptr_check(blockHeader header){ //was going to be a checking function used to verify input
  /// Root Node
}


//my_malloc, the memory allocator  searches through the header list and finds the first block which is header (based on requested size), 
//according to the assignment specifications we are asked to round to the nearest multiple of 8(and include enough space for the header and Boundary_tag of the block),
// If a block is found which is header the a block pointer to the content is returned. Once the block is found then the memory is assigned and then using the block to data function the address is stored. 
//If no header space is found then, it will return Null, an extension which I thought would be better was to just extend the heap. More accurate details of what each function does is found on the code itself as comments above the respective functions.

void *my_malloc(size_t size){ //ASK --------------------------------------------------------------------------------ASK RK

  //Initalise my Malloc
  rootHeader = mmap(NULL, kBlockMetaDataHeader_Size, PROT_READ | PROT_WRITE, // Initialise the start of the list
                      MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
  rootHeader->allocated=true;
  rootHeader->size = kBlockMetaDataHeader_Size;
  rootHeader->next = rootHeader;
  rootHeader->prev = NULL;

  // An allocation of 0bytes should return the NULL pointer
  if (size == 0 || size > kMaxAllocationSize){
    return NULL;
  }

  // Round up sizing that was inputted from the functions to include the header
  // and Boundary_tag allocation header. - idk if this is relevant
  size = round_up(size + kBlockMetadataSize_full, kAlignment);
  // Find a block in the freelist
  blockHeader *header_pointer = obtain_proceeding_block(rootHeader, size);
  // Return NULL if we are outside the bounds of our heap - from above call
  full_block *allocated_block = NULL;
  int factor = 1 ;
  if (header_pointer == NULL) { //No block get a block
    // Check if the size is bigger than the ARENA_SIZE (4MB) 
    while (size > (ARENA_SIZE * factor) && factor != 4){
      factor += 1;
    }
    header_pointer = acquire_chunk(factor); //based on a the round_up variable obtain chunks of size needed

      if (header_pointer->size >= size + (kBlockMetadataSize_full << 1) + kMinAllocationSize) {
        allocated_block = split_chunk(header_pointer, size);
        // allocated_block = ((size_t)header_pointer) + kBlockMetaDataHeader_Size + header_pointer->size; problem
      }
      else {
        allocated_block = header_pointer;
      } 
      allocated_block -> size = size;
      allocated_block -> allocated = true;
      
  } else { // Block to split
    // Split the block if it larger then needed
    // check if split next block is more than header size.
    if (header_pointer->size >= size + (kBlockMetadataSize_full << 2) + kMinAllocationSize) { //Bit shift by 2 (Idk why results in better time)
      header_pointer = split_chunk(header_pointer, size);
    }
    else{ // Allocate the memory
      header_pointer->prev->next = header_pointer->next;
      header_pointer->next->prev = header_pointer->prev;
    }
    // initialise final block 
    allocated_block = ((size_t)header_pointer) + kBlockMetaDataHeader_Size + header_pointer->size;
    allocated_block -> allocated = true;
    allocated_block -> size = size;
  }
  
  // process final block to return
  void *data = block_to_data(allocated_block);
  LOG("%llu", size);
  memset(data, 0, size);
  return data;

}


//Using 2 addresses for the current and next block, 
//and using a while loop,compare if the block you are at + size of the header = the next blocks address, if so then join both.
void linear_coalesce() {
   blockHeader *head = rootHeader->next;
   size_t curr_addr, next_addr;
   while( head->next == rootHeader) {
      if((size_t) head + kBlockMetadataSize_full + head->size == (size_t) head->next) {
         //two adjacent free blocks in a row
         //combine size and add meta data
         head->size += head->next->size + kBlockMetadataSize_full;
         //point the next accross
         head->next = head->next->next;
      }
      //iterate forwards
      head = head->next;
   }
}


//Frees up memory using the Coalescing function being used and fed using data_to_blocks function.
void my_free(void *ptr)
{
  if (ptr == NULL)  // FROM LAB 6
    return;
  blockHeader *block = data_to_block(ptr);
  if (!in_mmaped_memory(block) || !block->allocated) {
    errno = EINVAL;
    fprintf(stderr, "my_free: %s\n", strerror(errno));
    return;
  }
  
  //Last In First Out (Free the block and send back to front od list then un allocate)
  blockHeader *head = rootHeader;
  block->prev = head;
  head ->next = block;
  block->allocated = false;
  block->next = head->next;
  block->next->prev = block;
  linear_coalesce(); //Coalesce at the end
}

//----------------------------------------------BELOW IS ATTEMPTED IDEAS/CODE-----------------------------------------------------------------------------------------------------------------------------

//Attempt at header lists:
// inline static void free_list_append(Block *b, Block *list) {
//   if (list) {
//     // append to head
//     b->next = list;
//     b->prev = NULL;
//     list->prev = b;
//     list = b;
//   } else {
//     // no elements
//     list = b;
//     b->prev = NULL;
//     b->next = NULL;
//   }
// }

// /// Get right neighbour  //Lab 6 Code
// inline static Block *get_right_block(full_block *block) {
//   size_t ptr = ((size_t)block) + block->size;
//   // Return NULL if we are outside the bounds of our heap
//   if (ptr >= ((size_t)start) + kMemorySize)
//     return NULL;
//   return (Block *)ptr;
// }

// https://linuxhint.com/using_mmap_function_linux/
//https://www.youtube.com/watch?v=sZ8GJ1TiMdk
// https://medium.com/@pawanakhil.dasari/implementing-our-own-malloc-function-and-memory-management-schemes-in-c-c-using-linked-list-6da9fea73822
// https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/
// https://moss.cs.iit.edu/cs351/slides/slides-malloc.pdf
// https://www.youtube.com/watch?v=8REdBgL_bXA
// https://www.clear.rice.edu/comp321/html/pdf/09-memory.pdf
