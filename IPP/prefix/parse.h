//------------------------------------------------------------------------------
// File: parse.h
// Project: Prefix dictionary
// Author: Tomasz Kacperek (tk334578@students.mimuw.edu.pl)
// Description: Definitions of functions to parse strings into commands.
//------------------------------------------------------------------------------

#ifndef PARSEH
#define PARSEH

//------------------------------------------------------------------------------
// type definitions
//------------------------------------------------------------------------------

enum cmdType {invalid, insert, prev, delete, find, clear};
typedef enum cmdType commandType;

struct commandStruct
{
  commandType type;
  char argc;
  sptr ** args;
};
typedef struct commandStruct command;

//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------

void freeCmd (command *);
command * parse (char *);

#endif
