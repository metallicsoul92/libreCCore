#include "../../include/core/estring.h"

#include <stdlib.h>
#include <string.h>

struct estring{
  uint32_t _length;
  char * _string;
};

//construction/destruction
estring_t * malloc_estring(const char * data){
  estring_t * out = malloc(sizeof(estring_t));
  out->_length = strlen(data);
  out->_string = malloc(sizeof(char) * out->_length);
  strcpy(out->_string,data);
  return out;
}
void free_estring(estring_t * self){
  free(self->_string);
  free(self);
}
//getters
uint32_t estring_length(estring_t *self){
  return self->_length;
}
const char * estring_str(estring_t*self){
  return (const char *)self->_string;
}
const char estring_getAt(estring_t *self, uint32_t index){
    if(index >= self->_length){
      return (const char)NULL;
    }
    return (const char)self->_string[index];
}
bool estring_isEqual(estring_t * a, estring_t * b){
  //first check if they are the same length
  if(a->_length != b->_length){
    return false;
  }

  for(uint32_t i = 0; i < a->_length; i++ ){
    if(a->_string[i] != b->_string[i]){
      return false;
    }
  }
  return true;
}


//setters
void estring_append(estring_t * self, const char * extra){
  uint32_t added = strlen(extra);
  self->_length += added;
  self->_string = realloc(self->_string,self->_length);
  strcat(self->_string,extra);
}
void estring_setAt(estring_t * self, const char letter, uint32_t index){
  if(self->_length > index){
    self->_string[index] = letter;
  }
}

estring_t * estring_substr(estring_t * str, uint32_t start, uint32_t end){
  //if the string is smaller than the start or end of the string, return NULL
  if(str->_length < start || str->_length < end){
    return NULL;
  }
  uint32_t size = end - start;
  char * _intdata = malloc(sizeof(char) * size);
  memcpy(_intdata,str+start, size);
  estring_t * out = malloc_estring(_intdata);
  free(_intdata);
  return out;
}
