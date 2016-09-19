//------------------------------------------------------------------------------
// File: dictionary.c
// Project: Prefix dictionary
// Author: Tomasz Kacperek (tk334578@students.mimuw.edu.pl)
// Description: Main program implementing prefix dictionary.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// preprocessor directives
//------------------------------------------------------------------------------

#include "config.h"
#include "parse.h"

#define CMD_LENGTH 100001

//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------

// returns: -1: error, 0: nothing, 1: v flag 
char handleArguments(int argc, char * argv [])
{
  int i;
  char vArg, optionError, * illegalOption;
  
  vArg = optionError = 0; 

  if (argc > 1)
  {
    i = 1;
    while (i < argc && optionError == 0)
    {
      if (strcmp (argv [i], "-v") != 0)
      {
        optionError = 1;
        illegalOption = argv [i];
      }
      ++i;
    }

    if (optionError)
    {
      fprintf (stderr, "illegal option -- %s\n", illegalOption);
      vArg = -1;
    }
    else
      vArg = 1;
  }

  return vArg;
}

void runInsert (sptr ** args)
{
  uint n = insertNew (args [0]);
  printf ("word number: %u\n", n);
}

void runPrev (sptr ** args)
{
  uint n = insertPrev ((uint) strtoul (args [0] -> str, NULL, 10),
                       (uint) strtoul (args [1] -> str, NULL, 10),
                       (uint) strtoul (args [2] -> str, NULL, 10));
  printf ("word number: %u\n", n);
}

void runDelete (sptr ** args)
{
  uint n = (uint) strtoul (args [0] -> str, NULL, 10);
  deleteNumber (n);
  printf ("deleted: %u\n", n);
}

void runFind (sptr ** args)
{
  char found;
  char * var, * yes, * no;

  found = prefixFind (args [0]);
  yes = "YES";
  no = "NO";
  var = (found == 1) ? yes : no;
  printf ("%s\n", var);
}

void runClear ()
{
  clearTree ();
  printf ("cleared\n");
}

void run (command * cmd)
{
  switch (cmd -> type)
  {
    case insert:
      runInsert (cmd -> args);
      break;
    case prev:
      runPrev (cmd -> args);
      break;
    case delete:
      runDelete (cmd -> args);
      break;
    case find:
      runFind (cmd -> args);
      break;
    case clear:
      runClear ();
      break;
    case invalid:
      break;
  }
}

//------------------------------------------------------------------------------
// main function
//------------------------------------------------------------------------------

int main (int argc, char * argv [])
{
  char ret, * commandString, vArg;
  command * cmd;

  ret = 0;
  vArg = handleArguments (argc, argv);
  if (vArg == -1)
    ret = 1;
  else
  {
    commandString = calloc (CMD_LENGTH + 1, sizeof (char));
    initTree ();
    
    while (fgets (commandString, CMD_LENGTH + 1, stdin) != NULL)
    {
      cmd = parse (commandString);
      if (cmd -> type == invalid)
        printf ("ignored\n");
      else
      {
        run (cmd);
        if (vArg && cmd -> type != find)
          fprintf (stderr, "nodes: %u\n", treeNodeCount ()); 
      }
      freeCmd (cmd); 
    }

    freeTree ();
    free (commandString);
    fclose (stdin);
    fclose (stdout);
    fclose (stderr);
  }
  return ret;
}
