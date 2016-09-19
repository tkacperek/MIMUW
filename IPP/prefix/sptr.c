//------------------------------------------------------------------------------
// File: sptr.c
// Project: Prefix dictionary
// Author: Tomasz Kacperek (tk334578@students.mimuw.edu.pl)
// Description: Library of functions to manipulate string pointers. 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// preprocessor directives
//------------------------------------------------------------------------------

#include "config.h"
#include "sptr.h"

//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------

uint min (uint a, uint b)
{
  return (a <= b) ? a : b;
}

// creates new sptr
// doesn't change arguments
sptr * newSptr (char * str, uint length)
{
  sptr * s = malloc (sizeof (sptr));
  s -> str = str;
  s -> length = length;
  return s;
}

// creates a copy of given sptr
sptr * cpySptr (sptr * old)
{
  return newSptr (old -> str, old -> length);
}

// returns first char standing after prefix in str
// assumes it's a valid prefix 
char afterPrefix (sptr * str, sptr * prefix)
{
  return str -> str [prefix -> length];
}

// returns length of common prefix of given strings
// compare from position i
uint commonPrefix (sptr * s1, sptr * s2, uint i)
{
  uint n, m; 
    
  n = 0;
  m = min (s1 -> length, s2 -> length);

  while (i + n < m)
    if (s1 -> str [i + n] == s2 -> str [i + n])
      ++n;
    else
      i = m + 1;
  return n;
}

// creates new sptr with substring of str from beg to end (included)
sptr * subStr (sptr * str, uint beg, uint end)
{
  return newSptr ( (str -> str) + beg, end - beg + 1);
}
