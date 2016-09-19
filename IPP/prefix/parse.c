//------------------------------------------------------------------------------
// File: parse.c
// Project: Prefix dictionary
// Author: Tomasz Kacperek (tk334578@students.mimuw.edu.pl)
// Description: Library of functions to parse strings into commands.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// preprocessor directives
//------------------------------------------------------------------------------

#include "config.h"
#include "parse.h"
#include <ctype.h>

// for ILP32 data model 
#define MAX_UINT 4294967295

//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------

void freeCmdArgs (command * cmd)
{
  int i;
  if (cmd -> argc > 0)
  {
    for (i = 0; i < cmd -> argc; ++i)
    {
      free (cmd -> args [i] -> str);
      free (cmd -> args [i]);
    }
    free (cmd -> args);
    cmd -> argc = 0;
  } 
}

void freeCmd (command * cmd)
{
  freeCmdArgs (cmd);
  free (cmd);
}

// cuts commandString into tokens
// returns length by value and tokens by argument
int tokenize (sptr *** finalTokens, char * commandString)
{
  char tokenCount, over;
  int i, tmp, beg, end, posi, * tokenPos;
  sptr ** tokens;
  char * word;

  over = tokenCount = 0;
  i = posi = 0;
  tokenPos = calloc (10, sizeof (sptr));

  while (tokenCount <= 4 && ! over)
  {
    // skip spaces
    while (commandString [i] == ' ')
      ++i;
    // first non-space character
    beg = i;
    // skip all but ugly characters
    while (commandString [i] != '\0' && 
           commandString [i] != '\n' &&
           commandString [i] != ' ')
      ++i;
    // first ugly character
    end = i - 1;
    // if there was a nice word
    if (end - beg >= 0)
    {
      // remember where it was
      tokenPos [posi++] = beg;
      tokenPos [posi++] = end;
      ++tokenCount;
    }
    // is it over?
    if (commandString [i] == '\0' ||
        commandString [i] == '\n')
        over = 1;
  }

  // if something went wrong
  if (0 == tokenCount || tokenCount > 4)
  {
    // throw a bone
    tokens = malloc (sizeof (sptr *));
    word = malloc (sizeof (char));
    * word = 32;
    tokens [0] = newSptr (word, 1);
    tokenCount = 1;
  }
  else
  {
    tokens = calloc (tokenCount, sizeof (sptr *));
    for (i = 0; i < tokenCount; ++i)
    {
      tmp = tokenPos [2 * i + 1] - tokenPos [2 * i] + 1;
      word = calloc (tmp + 1, sizeof (char));
      strncpy (word, commandString + tokenPos [2 * i], tmp);
      word [tmp] = '\0';
      tokens [i] = newSptr (word, tmp);
    }
  }

  free (tokenPos);
  * finalTokens = tokens;
  return tokenCount;
}

char * getFingerprint (sptr ** tokens, int length)
{
  int i, j, strPtr;
  int (* fp) (int);
  char * fingerprint;
  long valTest;

  strPtr = 0;
  fingerprint = calloc (5, sizeof (char));

  for (i = 0; i < length; ++i)
  {
    if (! (isdigit (tokens [i] -> str [0]) || islower (tokens [i] -> str [0])))
      fingerprint [strPtr++] = 'x';
    else
    {
      fp = (islower (tokens [i] -> str [0])) ? & islower : & isdigit;
      j = 1;
      while (j < tokens [i] -> length && (* fp) (tokens [i] -> str [j]))
        ++j;
      fingerprint [strPtr++] = (j != tokens [i] -> length) ? 'x' :
                             ((* fp == islower) ? 'w' : 'n');
      if (fingerprint [strPtr - 1] == 'n')
      {
        j = 0;
        // count leading zeros
        while (tokens [i] -> str [j] == '0')
          j++;
        if (j != 0 && (j != 1 || tokens [i] -> length != 1))
          fingerprint [strPtr - 1] = 'x';
        else {
          valTest = atol (tokens [i] -> str);
          if (valTest > MAX_UINT)
            fingerprint [strPtr - 1] = 'x';
        }
      }
    }
  }
  fingerprint [strPtr] = '\0';
  return fingerprint;
}

command * syntacticAnalysis (sptr ** tokens, int length)
{
  int i;
  char invalidFlag;
  command * cmd;
  char * fingerprint;
 
  invalidFlag = 0;
  cmd = malloc (sizeof (command));
  fingerprint = getFingerprint (tokens, length);

  if (fingerprint [0] != 'w')
    invalidFlag = 1;
  else 
  {
    if (strcmp (fingerprint, "ww") == 0 &&
        strcmp (tokens [0] -> str, "insert") == 0)
    {
      cmd -> type = insert;
      cmd -> argc = 1;
    }
    else if (strcmp (fingerprint, "ww") == 0 &&
             strcmp (tokens [0] -> str, "find") == 0)
    {
      cmd -> type = find;
      cmd -> argc = 1;
    }
    else if (strcmp (fingerprint, "wnnn") == 0 && 
             strcmp (tokens [0] -> str, "prev") == 0)
    {
      cmd -> type = prev;
      cmd -> argc = 3;
    }
    else if (strcmp (fingerprint, "wn") == 0 &&
             strcmp (tokens [0] -> str, "delete") == 0)
    {
      cmd -> type = delete;
      cmd -> argc = 1;
    }
    else if (strcmp (fingerprint, "w") == 0 &&
             strcmp (tokens [0] -> str, "clear") == 0)
    {
      cmd -> type = clear;
      cmd -> argc = 0;
    }
    else
    {
      invalidFlag = 1;
    }
  }

  if (invalidFlag == 1)
  {
    cmd -> type = invalid;
    cmd -> argc = 0;
    for (i = 1; i < length; ++i)
    {
      free (tokens [i] -> str);
      free (tokens [i]);
    }
  }
  else if (cmd -> type != clear)
  {
    cmd -> args = calloc (cmd -> argc, sizeof (sptr *));
    for (i = 1; i <= cmd -> argc; ++i)
      cmd -> args [i - 1] = tokens [i]; 
  }

  free (tokens [0] -> str);
  free (tokens [0]);
  free (tokens);
  free (fingerprint);
  return cmd;
}

void semanticAnalysis (command * cmd)
{
  char badboy = 0;
  uint tmp[3];

  switch (cmd -> type)
  {
    case insert:
      if (fullFind (cmd -> args [0]) == 1)
          badboy = 1;
      break;
    case prev:
      tmp [0] = getLengthOfWordNumber 
                  ((uint) strtoul (cmd -> args [0] -> str, NULL, 10));
      tmp [1] = (uint) strtoul (cmd -> args [1] -> str, NULL, 10);
      tmp [2] = (uint) strtoul (cmd -> args [2] -> str, NULL, 10);
      if (tmp [0] == 0 || tmp [1] > tmp [2] || tmp [2] >= tmp [0])
      {
        badboy = 1;
      }
      else if (! validPrev ((uint) strtoul (cmd -> args [0] -> str, NULL, 10),
                           tmp [1], tmp [2]))
      {
        badboy = 1;
      }
      break;
    case delete:
      if (getLengthOfWordNumber 
            ((uint) strtoul (cmd -> args [0] -> str, NULL, 10))
          == 0)
        badboy = 1;
      break;
    case find:
    case clear:
    case invalid:
      break;
  }
  if (badboy)
  {
    cmd -> type = invalid;
    freeCmdArgs (cmd);
  }
}

command * parse (char * commandString)
{
  sptr ** tokens;
  int tokenCount = tokenize (& tokens, commandString);
  command * cmd = syntacticAnalysis (tokens, tokenCount);
  if (cmd -> type != invalid)
    semanticAnalysis (cmd);
  return cmd;
}
