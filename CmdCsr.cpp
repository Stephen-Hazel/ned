// CmdCsr.cpp - commands for moving the cursor.

#include "ned.h"

// FAIL IF: already in col 79
bool Rit ()  {if (CsrCol == 79) return false;  CsrCol++;  return true;  }

// FAIL IF: already in col 0
bool Lft ()  {if (CsrCol ==  0) return false;  CsrCol--;  return true;  }

bool Up ()
// FAIL IF: already in row 0
// move up if you can and scroll the screen if you need to
{  if (CsrRow == 0)           return false;
   CsrRow--;
   if (ScrRow)          ScrRow--;   else PutScr ();
   return true;
}

bool Dn ()
// FAIL IF: already in MAX_ROW-1  (pretty damn unlikely but gotta check)
// move down if you can and scroll the screen if you need to
{  if (CsrRow == (MAX_ROW-1)) return false;
   CsrRow++;
   if (ScrRow < EndScr) ScrRow++;   else PutScr ();
   return true;
}

bool Top ()
// leave ScrRow the same.  set first row on screen to row 0.
{  CsrRow = ScrRow;  PutScr ();  return true;  }

bool Bot ()
// leave ScrRow the same.  set last row on screen to EndRow-1, else ScrRow.
{  CsrRow = (EndRow > (EndScr+1)) ? (EndRow-1 - (EndScr-ScrRow)) : ScrRow;
   PutScr ();  return true;
}

bool BlkHop ()
{  if ((CsrCol != BlkBgnCol) || (CsrRow != BlkBgnRow))
         MoveToMid (BlkBgnRow, BlkBgnCol);
   else  MoveToMid (BlkEndRow, BlkEndCol);
   PutScr ();  return true;
}

bool PgUp ()
// FAIL IF: already at top
// leave ScrRow the same.  go up half a screen (stopping at top)  fix screen.
{     if (CsrRow == ScrRow)  return false;
      if (CsrRow < (EndScr >> 1) + ScrRow) CsrRow = ScrRow;
      else                                 CsrRow -= (EndScr >> 1);
      PutScr ();  return true;
}

bool PgDn ()
// FAIL IF: already at bottom
// leave ScrRow the same.  go down half a screen. (stopping at bot)  fix screen.
// note the seperate case when EndRow isn't larger than screen.
{ uword TmpRow;
   if (EndRow > (EndScr+1)) {
      TmpRow = EndRow-1 - (EndScr-ScrRow);
      if (CsrRow == TmpRow)  return false;
      if (CsrRow + (EndScr >> 1) > TmpRow) CsrRow = TmpRow;
      else                                 CsrRow += (EndScr >> 1);
   }
   else {
      if (CsrRow == ScrRow)  return false;
      CsrRow = ScrRow;
   }
   PutScr ();  return true;
}

bool WrdRit ()
// move to first char of next word, else col of row's Len, else next row col 0.
// FAIL IF: wrapping and no next row.
// if past len of row, wrap to col 0 of next row.
// while on a valid nonspace, move right.
// if now on a valid space, move right to first nonspace (guaranteed to exist).
{ ubyte len = CsrRowLen ();
  pcol  ptr = CsrRowPtr () + CsrCol;
   if ((CsrCol >= len) || (CsrCol == 79))
      {if (! Dn ()) return false;  CsrCol = 0;}
   else {
      while ((CsrCol < len) && (* ptr    != ' ')) {CsrCol++; ptr++;}
      if (CsrCol < len)  while (*(ptr++) == ' ')   CsrCol++;
      if (CsrCol == 80) CsrCol--;
   }
   return true;
}

bool WrdEnd ()
// move to next space after a word, col of row's Len, or next row's col 0.
// FAIL IF: wrapping and no next row.
// if past Len of row, wrap to col 0 of next row.
// while on a valid space, move right.
// while on a valid nonspace, move right.
{ ubyte len = CsrRowLen ();
  pcol  ptr = CsrRowPtr () + CsrCol;
   if ((CsrCol >= len) || (CsrCol == 79))
      {if (! Dn ()) return false;  CsrCol = 0;}
   else {
      while ((CsrCol < len) && (*ptr     == ' ')) {CsrCol++; ptr++;}
      while ((CsrCol < len) && (*(ptr++) != ' '))  CsrCol++;
      if (CsrCol == 80) CsrCol--;
   }
   return true;
}

bool WrdLft ()
// move to first char of prev word, else col 0, else col of prev row's Len
// FAIL IF: wrapping and no prev row.
// if on col 0,      wrap to last col of prev row.
// else if Len is 0, move to col 0.
// else              if on start of a word already, move left 1.
//                   while not in col 0 and on a space, move left.
//                   while not in col 0 and prev col is a nonspace, move left.
{ ubyte len = CsrRowLen ();
  pcol  ptr = CsrRowPtr () + CsrCol;
   if (CsrCol == 0) {
      if (! Up ()) return false;
      if ((CsrCol = CsrRowLen ()) == 80) CsrCol--;
   }
   else {
      if (len == 0) CsrCol = 0;
      else {
         if (CsrCol >= len)
           {ptr -= CsrCol-len+1;  CsrCol = len-1;}
         else
            if (CsrCol && (*(ptr-1) == ' ') && (*ptr != ' ')) {CsrCol--; ptr--;}
         while (CsrCol && (*ptr     == ' ')) {CsrCol--; ptr--;}
         while (CsrCol && (*(--ptr) != ' '))  CsrCol--;
      }
   }
   return true;
}
