# MemAlloc - A Custom Dynamic Memory Allocator
## Introduction
Welcome to MemAlloc, a personal project , designed to explore the intricacies of dynamic memory allocation in C. This project introduces mymalloc, a custom memory allocator implemented with a focus on efficiency and simplicity.

## Overview
## Project Structure
mymalloc.c: The heart of MemAlloc, containing the implementation of the custom memory allocator.
mymalloc.h: The header file with necessary declarations and includes.

## Memory Management Philosophy
MemAlloc adopts a segregated free list approach with a first-fit allocation strategy. It keeps track of free blocks and efficiently allocates memory based on the first available block that meets the allocation request.

### Core Data Structures

### Header Structure
    typedef struct Header {
    size_t size;
    bool allocated;
    blockHeader *next;
    blockHeader *prev;
    } Header;

### Boundary_Tag Structure
    typedef struct Boundary_Tag {
    size_t size;
    bool allocated;
    } Boundary_Tag;

### full_block Structure
    typedef struct full_block {
    bool allocated;
    size_t size;
    } full_block;

## Constants
kMaxAllocationSize: Maximum allocation size (16 MB).
kBlockMetadataSize_full: Size of metadata per free block.
kBlockMetadataSize_Tag: Size of metadata per boundary tag.
kBlockMetaDataTag_Size: Size of metadata per block.
kBlockMetaDataHeader_Size: Size of header metadata.
## Functionality
### void *my_malloc(size_t size)
Allocates memory dynamically, incorporating a first-fit strategy.
Acquires more memory from the OS if no suitable block is found.
Supports flexible allocation sizes, up to the maximum limit.

### void my_free(void *ptr)
Frees memory associated with the provided pointer.
Implements coalescing to merge adjacent free blocks for optimized memory usage.

### static void linear_coalesce()
Coalesces adjacent free blocks using a linear strategy.
static blockHeader *obtain_proceeding_block(blockHeader *header, size_t size)
Retrieves the next block of a specified size from the free list.

### static Header *split_chunk(Header *header, size_t size)
Splits a larger block into a smaller one to enhance memory utilization.

### static blockHeader *acquire_chunk(int factor)
Requests a new memory chunk from the operating system.

## Integration and Exploration
Integrate mymalloc.h into your C projects to leverage MemAlloc's features.
Compile and link mymalloc.c with your program.
Experiment with my_malloc and my_free for dynamic memory allocation and deallocation.
Inspiration and References
Drawing inspiration from personal exploration and various online resources on memory management and dynamic allocation.
Acknowledgments
A special thanks to the curious minds exploring the world of C programming and dynamic memory allocation. This project is a testament to the passion for understanding and creating efficient solutions.





