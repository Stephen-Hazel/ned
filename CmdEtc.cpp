// CmdEtc.cpp - commands that fall into no special group.

#include "ned.h"

bool EndCmd ()  {return false;}

bool UnDelRow ()
// FAIL IF: can't insert the row to restore.
// InsRow moves the last deleted row to the cursor row, then just fix Len.
{ ubyte len = 80;
  pcol  ptr;
   if (CsrRow >= EndRow) return false;
   if (! InsRow ())      return false;
   ptr = CsrRowPtr () + 79;
   while (len && ((*ptr == '\x00') || (*ptr == ' '))) {len--; ptr--;}
   Row [CsrRow]->Len = len;
   PutRow (ScrRow);  return true;
}

bool TglSqnc ()
{  Sqnc = ! Sqnc;   if (Sqnc) SqncLen = 0;
   return true;
}

bool RptSqnc ()
{  for (ubyte i = 0;  i < SqncLen;  i++)  DoKey (KeySqnc [i]);
   return true;
}

bool UpCase ()
{ pcol ptr;
  char c;
   if ((CsrRow >= EndRow) || (CsrCol >= CsrRowLen ())) return false;
   c = *(ptr = CsrRowPtr () + CsrCol);
   if ((c >= 'a') && (c <= 'z')) c += ('A'-'a');
   *ptr = c;
   PutRow (ScrRow);  return true;
}

bool LoCase ()
{ pcol ptr;
  char c;
   if ((CsrRow >= EndRow) || (CsrCol >= CsrRowLen ())) return false;
   c = *(ptr = CsrRowPtr () + CsrCol);
   if ((c >= 'A') && (c <= 'Z')) c += ('a'-'A');
   *ptr = c;
   PutRow (ScrRow);  return true;
}
