#include "../../include/core/allocator.h"

#include <sys/mman.h>

#define HEAD 0xAF
#define FOOT 0xFA

#define FLAG_UINIT 0
#define FLAG_USED  1
#define FLAG_FREED 255

#define KB 1024
#define MB 1024 * KB


////////////////////////////////////////////////////////
//    PRIVATE API
////////////////////////////////////////////////////////

struct _allocHeader{
	uint8_t _HEADMAGIC;
	uint8_t _flags;
	uint16_t _binID;
	uint32_t _size;
	uint16_t _offset; 
	uint8_t _bPad;
	uint8_t _FOOTMAGIC;
};

typedef _allocHeader aHeader_t;

aHeader_t int_alloc_createHeader(size_t size,uint16_t binID, uint16_t offset){
	aHeader_t out;
	out._HEADMAGIC = HEAD;
	out._flags = FLAG_USED;
	out._bPad = 0;
	out._size = size;
	out._binID = binID;
	out._offset = offset;
	out._sPad = 0;
	out._FOOTMAGIC = FOOT;
	return out;
}




struct _allocBlock{
	aHeader_t _head;
	struct _allocBlock *_prev;	//pointer to the previous block (NULL IF ROOT)
	struct _allocBlock *_next;	//pointer to the next block 
	void * _addr;				//place within the chunk of allocated memory
};

/*
Block Struct
|          ALLOC BLOCK                       |         ALLOC BLOCK          |
|HEADER|_prev|_next| DATA                    |HEADER|_prev|_next|DATA       |




*/


typedef struct _allocBlock  aBlock_t;

size_t int_allocBlock_offset(){
	return (sizeof(aHeader_t) + (sizeof(aBlock_t*) * 2));
}

size_t int_allocBlock_fullSize(aBlock_t * block){
	size_t out;
	out = int_allocBlock_offset() + block->_head.size;
	return out;
}

void int_allocBlock_placeAtAdress(aBlock_t * block , void * address){
	memset(address,block, int_allocBlock_fullSize(block));
}

void int_allocBlock_initBlockAtAddress(size_t size, aBlock_t * prev, aBlock_t * next);

void int_allocBlock_setPrevious(aBlock_t * self, aBlock_t * prev){
	self->_prev = prev;
}
void int_allocBlock_setNext(aBlock_t * self, aBlock_t * next){
	self->_next = next;
}


aBlock_t * int_alloc_getBlock(void * address){
	return  (aBlock_t *)(address - int_allocBlock_offset());
}

aHeader_t * int_alloc_getHeader(aBlock_t * self){
	return self->_header;
}
aBlock_t * int_alloc_getPrev(aBlock_t * self){
	return self->_prev;
}
aBlock_t * int_alloc_getNext(aBlock_t * self){
	return self->_next;
}

void * int_alloc_getData(aBlock_t * self){
	return self->_data;
}

struct _allocBin{
	uint16_t _binID;
	uint16_t _sPad;
	uint32_t _elementMaxSize;
	uint32_t _elementCount;
	uint32_t _binSize;
	void * _baseAddr;
};

/*  
Bin Struct:
| _binID| _sPad|  _elementMaxSize| _elementCount| _binSize|
| 2bytes|2bytes|        4bytes   |    4bytes    |  4bytes |               
|				_baseAddr                                 |
|            _elementMaxSize bytes                        |





*/

typedef struct _allocBin aBin_t;


void int_allocBin_placeBlock(aBin_t * bin , aBlock_t * block){
	uint32_t offset = bin->_elementMaxSize * bin->_elementCount;
	void * blockAddress = bin->_baseAddr + offset;
	int_allocBlock_placeAtAddress(block, blockAddess);
	bin->_elementCount++;
}


/*
  1,048,576 bytes Per Bin?
  16,777,216 total ? Is this worth it?            

*/

struct _allocator{
	aBin_t _binData[16];
	void * _baseAddress; 
};

/*

Allocator:
|_binData[0] |_binData[1] |_binData[2] |_binData[3] |
|_binData[4] |_binData[5] |_binData[6] |_binData[7] |
|_binData[8] |_binData[9] |_binData[10]|_binData[11]|
|_binData[12]|_binData[13]|_binData[14]|_binData[15]|
|_baseAddress      _binData[0]._baseAddr            |
|                  _binData[1]._baseAddr            |
|                  _binData[2]._baseAddr            |
|                  _binData[3]._baseAddr            |
|                  _binData[4]._baseAddr            |
|                  _binData[5]._baseAddr            |
|                  _binData[6]._baseAddr            |
|                  _binData[7]._baseAddr            |
|                  _binData[8]._baseAddr            |
|                  _binData[9]._baseAddr            |
|                  _binData[10]._baseAddr           |
|                  _binData[11]._baseAddr           |
|                  _binData[12]._baseAddr           |
|                  _binData[13]._baseAddr           |
|                  _binData[14]._baseAddr           |
|                  _binData[15]._baseAddr           |
*/



// size = sizeof(aBin_t) + 16 * MB? 

typedef struct _allocator allocator_t;


static allocator_t * _currAllocator = NULL;

void int_allocator_initBins(allocator_t * self){
	uint32_t baseSize = 2;
	uint32_t baseBinSize = MB;
	for(uint8_t i = 0; i < 16; i++){
		self->_binData[i]._binID = i;
		self->_binData[i]._elementMaxSize = 1 << ( i + 2);
		self->_binData[i]._elementCount = 0;
		self->_binData[i]._binSize = baseBinSize;
		self->_binData[i]._address = self->_baseAddress + ( i * baseBinSize);
	}
}

size_t static_allocator_size(size_t binSize){
	return  (size_t)(sizeof(aBin_t) + (binSize * 16));
}


allocator_t * int_create_allocator(size_t binSize){
	if(binSize == NULL){
	binSize = MB;
	}

	allocator_t * out = mmap(NULL, static_allocator_size(binSize), 
							 PROT_READ | PROT_WRITE ,
							 MAP_SHARED | MAP_ANONYMOUS,
							 -1 , 0);
	out->_baseAddr = (out + (sizeof(aBin_t) * 16));
	init_allocator_initBins(out);

	return out;
}

uint8_t int_determine_bin(size_t size){
	size_t currAlignedSize;
	size_t fullSize = int_
	for(size_t i < i ; i < 16; i++){
	currAlignedSize = 1 << ( i + 2);
	if( size < currAlignedSize){
		return i;
	}
	return ALLOC_BINFAIL; // FAILURE
}

////////////////////////////////////////////////////////
//    PUBLIC API
////////////////////////////////////////////////////////


uint8_t allocator_init(allocator_t * allocator){

	allocator = int_create_allocator(MB);

	if(allocator == NULL){
		return ALLOC_NOMEM;
	}

	_currAllocator = allocator;
	return ALLOC_SUCCESS;
}
uint8_t static_allocator_init(){

	allocator_init(_currAllocator);
	if(_currAllocator == NULL){
		return ALLOC_NOMEM;
	}

	return ALLOC_SUCCESS;
}

void allocator_destroy(allocator_t * self){
	munmap(self, static_allocator_size(self->_binData[0]._binSize);
}

void static_set_allocator(allocator_t * alloc){
	if(_currAllocator != NULL){
		allocator_destroy(_currAllocator);
	}
	_currAllocator = alloc;
}


//allocation from the current alllocator that is initialized
void * static_allocator_malloc(size_t size){
	return allocator_malloc(_currAllocator,size);
}

void * static_allocator_calloc(size_t size){
	return static_allocator_malloc(size);
}

void * static_allocator_realloc(void * address, size_t size){
	return allocator_realloc(_currAllocator,address,size);
}

//allocation from the specified allocator.
void * allocator_malloc(allocator_t * allocator, size_t size){
/*
  TODO: Implement
	  1. determine bin via size
	  2. get the last block of the bin
	  3. create a new block at the last blocks base address + offset
	  4. place header at the start of the new block
	  5. set last block's next to new block
	  6. set new block's prev to last block
	  7. return new block's _addr member variable;


*/
}

void * allocator_calloc(allocator_t * allocator, size_t size){
	return allocator_malloc(allocator,size);
}
void * allocator_realloc(allocator_t * allocator,void * address, size_t size);

//freeing memory within the allocator
void static_allocator_free(void * address);
void allocator_free(allocator_t * self, void * address)




