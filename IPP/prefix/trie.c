//------------------------------------------------------------------------------
// File: trie.c
// Project: Prefix dictionary
// Author: Tomasz Kacperek (tk334578@students.mimuw.edu.pl)
// Description: Library of functions to manipulate a trie. 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// preprocessor directives
//------------------------------------------------------------------------------

#include "config.h"
#include "trie.h"

#define CHILDREN 26
// ASCII for 'a' - 1 
#define LOW 97
#define MAX_DATA 2000000
#define MAX_WORDS 181818

#define foreach_child for (int tmp = 0; tmp < CHILDREN; ++tmp)

//------------------------------------------------------------------------------
// type definitions
//------------------------------------------------------------------------------

struct nodeStruct
{
  struct nodeStruct * parent;
  struct nodeStruct * children [CHILDREN];
  // points to entire word / prefix
  sptr * str;
  char isWord;
  char childCount;
};
typedef struct nodeStruct nodeType;

struct treeStruct
{
  nodeType * root;
  uint nodeCount;

  char data [MAX_DATA];
  uint size;

  nodeType * words [MAX_WORDS]; 
  uint wordCount;
};
typedef struct treeStruct treeType;

struct queryType
{
  sptr * str;
  nodeType * node;
};

//------------------------------------------------------------------------------
// global variables
//------------------------------------------------------------------------------

treeType * tree;
struct queryType * lastQuery;

//------------------------------------------------------------------------------
// basic node operations
//------------------------------------------------------------------------------

nodeType * newNode (sptr * str, char isWord)
{
  nodeType * node = malloc (sizeof (nodeType));
  node -> parent = NULL;
  foreach_child
    node -> children [tmp] = NULL;
  node -> str = cpySptr (str);
  node -> isWord = isWord; 
  node -> childCount = 0;
  return node;
}

// sets some pointers to make family bounds <3
void addChild (nodeType * parent, nodeType * child)
{
  parent -> children [afterPrefix (child -> str, parent -> str) - LOW] = child;
  parent -> childCount += 1;
  child -> parent = parent;
}

nodeType * getChild (nodeType * parent, char c)
{
  return parent -> children [c - LOW];
}

// breaks family bounds
void removeChild (nodeType * parent, nodeType * child)
{
  parent -> children [(int) afterPrefix (child -> str, parent -> str) - LOW] = NULL;
  parent -> childCount -= 1;
  child -> parent = NULL;
}

void killNode (nodeType * node)
{
  free (node -> str);
  free (node);
}

void killFamily (nodeType * node)
{
  if (node != NULL)
  {
    foreach_child
      killFamily (node -> children [tmp]);
    killNode (node);
  }
}

//------------------------------------------------------------------------------
// query operations 
//------------------------------------------------------------------------------

char testQuery (sptr * str)
{
  uint i;
  char n;

  i = 0;
  n = 0;

  if (str -> length == lastQuery -> str -> length)
  {
    while (i < str -> length)
      if (str -> str [i] != lastQuery -> str -> str [i])
        i = str -> length + 1;
      else
        ++i;
    if (i == str -> length)
      n = 1; 
  }
  return n;
}

void updateQuery (sptr * str, nodeType * node)
{
  int i;
  lastQuery -> node = node;

  free (lastQuery -> str -> str);
  lastQuery -> str -> length = str -> length;
  lastQuery -> str -> str = calloc (str -> length, sizeof (char));
  for (i = 0; i < str -> length; ++i)
    lastQuery -> str -> str [i] = str -> str [i];
}

void clearQuery ()
{
  lastQuery -> node = NULL;
  free (lastQuery -> str -> str);
  lastQuery -> str -> str = malloc (sizeof (char));
  lastQuery -> str -> length = 0;
}

//------------------------------------------------------------------------------
// basic tree operations
//------------------------------------------------------------------------------

// initialize on clear(ed) memory
void initTree ()
{
  uint i;
  sptr * s;
  tree = malloc (sizeof (treeType));

  s = newSptr (tree -> data, 0);
  tree -> root = newNode (s, 0);
  tree -> nodeCount = 1;
  free (s);
  
  for (i = 0; i < MAX_DATA; ++i)
    tree -> data [i] = 0;
  tree -> size = 0;

  for (i = 0; i < MAX_WORDS; ++i)
    tree -> words [i] = NULL;
  tree -> wordCount = 0;

  lastQuery = malloc (sizeof (struct queryType));
  lastQuery -> str = newSptr (malloc (sizeof (char)), 0);
  lastQuery -> node = NULL;
}

void freeTree ()
{
  killFamily (tree -> root);
  free (tree);
  tree = NULL;

  free (lastQuery -> str -> str);
  free (lastQuery -> str);
  free (lastQuery);
}

void clearTree ()
{
  freeTree (); 
  initTree ();
}

uint addWord (nodeType * node)
{
  tree -> words [tree -> wordCount] = node;
  tree -> wordCount += 1;
  return tree -> wordCount - 1;
}

// copies given string into tree data
sptr * addData (sptr * out)
{
  int i;
  sptr * in;
    
  in = newSptr (tree -> data + tree -> size, out -> length);

  for (i = 0; i < in -> length; ++i)
    in -> str [i] = out -> str [i];

  tree -> size += in -> length;
  return in;
}

//------------------------------------------------------------------------------
// low level operations
//------------------------------------------------------------------------------

// returns pointer to a node with the longest prefix of str
nodeType * partialFind (sptr * str)
{
  char found;
  uint i, common;
  nodeType * parent, * current;

  if (testQuery (str) == 1)
    current = lastQuery -> node;
  else
  {
    found = i = 0;
    parent = NULL;
    current = tree -> root;

    while (! found)
    {
      common = commonPrefix (str, current -> str, i);
      if (common < current -> str -> length - i)
      {
        current = parent;
        found = 1;
      }
      else if (common == str -> length - i)
      {
        found = 1;
      }
      else
      {
        if (getChild (current, afterPrefix (str, current -> str)) != NULL)
        {
          parent = current;
          current = getChild (current, afterPrefix (str, current -> str));
          i += common;
        }
        else
          found = 1;
      }
    }
    updateQuery (str, current);
  }
  return current;
}

nodeType * insertWordNode (sptr * str)
{
  sptr * s;
  nodeType * node, * mid, * child, * new;

  node = partialFind (str);

  if (node -> str -> length == str -> length)
  {
    node -> isWord = 1;
    new = node;
  }
  else if (getChild (node, afterPrefix (str, node -> str)) == NULL)
  {
    new = newNode (str, 1);
    tree -> nodeCount += 1;
    addChild (node, new);
  }
  else
  {
    child = getChild (node, afterPrefix (str, node -> str));

    s = newSptr (child -> str -> str,
                 commonPrefix (child -> str, str, 0));
    mid = newNode (s, 0);
    tree -> nodeCount += 1;
    free (s);

    removeChild (node, child);
    addChild (node, mid);
    addChild (mid, child);

    if (mid -> str -> length == str -> length)
    {
      mid -> isWord = 1;
      new = mid;
    }
    else
    {
      new = newNode (str, 1);
      tree -> nodeCount += 1;
      addChild (mid, new);
    }
  }
  clearQuery ();
  return new;
}

void deleteWordNode (nodeType * node)
{
  nodeType * grand, * parent, * child;

  parent = node -> parent;
  grand = parent -> parent;

  if (node -> childCount == 0)
  {
    if (node -> parent -> isWord == 1 || parent -> childCount > 2 || parent == tree -> root)
    {
      removeChild (node -> parent, node);
      tree -> nodeCount -= 1;
      killNode (node);
    }
    else
    {
      // he had only 1 sibling
      removeChild (parent, node);
      removeChild (grand, parent);
      foreach_child
        if (parent -> children [tmp] != NULL)
          child = parent -> children [tmp];
      addChild (grand, child);
      killNode (parent);
      killNode (node);
      tree -> nodeCount -= 2;
    }
  }
  else if (node -> childCount == 1)
  {
    removeChild (parent, node);
    foreach_child
      if (node -> children [tmp] != NULL)
        child = node -> children [tmp];
    addChild (parent, child);
    killNode (node);
    tree -> nodeCount -= 1;
  }
  else
  {
    node -> isWord = 0;
  }
  clearQuery ();
}

//------------------------------------------------------------------------------
// high level operations
//------------------------------------------------------------------------------

uint treeNodeCount ()
{
  uint out;
  if (tree -> nodeCount == 1)
    out = 0;
  else 
    out = tree -> nodeCount;
  return out; 
}

uint getLengthOfWordNumber (uint i)
{
  uint ret = 0;
  if (i < tree -> wordCount)
    if (tree -> words [i] != NULL)
      ret = tree -> words [i] -> str -> length;
  return ret;
}

// test if there is a word node with given prefix
char prefixFind (sptr * str)
{
  char found;
  nodeType * node, * child;
  
  found = 0;
  node = partialFind (str);

  if (str -> length == node -> str -> length)
    found = 1;
  else if (getChild (node, afterPrefix (str, node -> str)) != NULL)
  {
    child = getChild (node, afterPrefix (str, node -> str)); 
    if (commonPrefix (str, child -> str, node -> str -> length) 
        == str -> length - node -> str -> length)
      found = 1;
  }
  return found;
}

// test if the given word is present 
char fullFind (sptr * str)
{
  char found;
  nodeType * node; 

  found = 0;
  node = partialFind (str);

  if (str -> length == node -> str -> length && node -> isWord == 1)
    found = 1;
  return found;
}

uint insertNew (sptr * out)
{
  sptr * str;
  nodeType * node;

  str = addData (out);
  node = insertWordNode (str);
  free (str);
  return addWord (node);
}

uint insertPrev (uint n, uint beg, uint end)
{
  sptr * str;
  nodeType * node;
  
  node = tree -> words [n];
  str = subStr (node -> str, beg, end);
  
  node = insertWordNode (str);
  free (str);
  return addWord (node);
}

void deleteNumber (uint n)
{
  nodeType * node = tree -> words [n];
  tree -> words [n] = NULL;
  deleteWordNode (node);
}

char validPrev (uint n, uint beg, uint end)
{
  nodeType * node = tree -> words [n];
  sptr * str = subStr (node -> str, beg, end);
  char c = fullFind (str);
  free (str);
  return (c + 1) % 2;
}
