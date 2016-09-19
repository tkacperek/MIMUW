//------------------------------------------------------------------------------
// File: sptr.h
// Project: Prefix dictionary
// Author: Tomasz Kacperek (tk334578@students.mimuw.edu.pl)
// Description: Definitions of functions to manipulate string pointers. 
//------------------------------------------------------------------------------

#ifndef SPTRH 
#define SPTRH

//------------------------------------------------------------------------------
// type definitions
//------------------------------------------------------------------------------

typedef unsigned int uint;

// string pointer
struct sptrStruct
{
  char * str;
  uint length;
};
typedef struct sptrStruct sptr;

//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------

// these functions don't change arguments

// next 3 functions allocate memory - remember to free it
sptr * newSptr (char *, uint);
sptr * cpySptr (sptr *);
// args: str, beg, end
sptr * subStr (sptr *, uint, uint);

// args: str, prefix
char afterPrefix (sptr *, sptr *);
// start comparing at given index
uint commonPrefix (sptr *, sptr *, uint);

#endif
