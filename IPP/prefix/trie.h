//------------------------------------------------------------------------------
// File: trie.h
// Project: Prefix dictionary
// Author: Tomasz Kacperek (tk334578@students.mimuw.edu.pl)
// Description: Definitions of functions to manipulate a trie.
//------------------------------------------------------------------------------

#ifndef TRIEH
#define TRIEH

//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------

void initTree ();
void freeTree ();
void clearTree ();

uint insertNew (sptr *);
uint insertPrev (uint, uint, uint);
void deleteNumber (uint);

uint treeNodeCount ();
uint getLengthOfWordNumber (uint);
char prefixFind (sptr *);
char fullFind (sptr *);
char validPrev (uint, uint, uint);

#endif
