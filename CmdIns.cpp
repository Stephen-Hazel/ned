// CmdIns.cpp - commands for inserting and deleting text.

#include "ned.h"

static bool Indent ()
// FAIL IF: no prev row defined | no word in prev row starting after CsrCol
// in CsrRow-1, find a valid space starting at CsrCol, find next valid nonspace.
// if found, set new CsrCol.
{ ubyte len, NewCol;
  pcol  ptr;
   if ((CsrRow == 0) || (CsrRow > EndRow))  return false;
   ptr = & Row [CsrRow-1]->Col [CsrCol];   len = Row [CsrRow-1]->Len;
   NewCol = CsrCol;
   while ((NewCol < len) && (*ptr != ' '))  {NewCol++; ptr++;}
   while ((NewCol < len) && (*ptr == ' '))  {NewCol++; ptr++;}
   if (NewCol >= len)                      return false;
   CsrCol = NewCol;   return true;
}

bool TglOver ()  {Over = ! Over;   return true;}

bool Del ()
// delete a col and fix screen.
{  DelCols (CsrRow, CsrCol, 1);   PutRow (ScrRow);   return true;  }

bool Cntr ()
// center the cursor row
{ ubyte bgn, cbgn, len;
  pcol  col;
   if (CsrRow < EndRow) {
      col = CsrRowPtr ();   len = CsrRowLen ();
      for (bgn = 0;  (bgn < len) && (col [bgn] == ' ');  bgn++)  ;
      if (bgn < (cbgn = (80 - len + bgn) >> 1))
           InsCols (CsrRow, 0, SC(ubyte,cbgn-bgn));
      else DelCols (CsrRow, 0, SC(ubyte,bgn-cbgn));
      PutRow (ScrRow);
   }
   return true;
}

bool InsRow ()
// FAIL IF: another row won't fit
// insert a row and fix screen.
{  if (! InsRows (CsrRow, 1))  return false;
   PutScr ();   return true;
}

bool DelRow ()
// delete a row and fix screen.
{  DelRows (CsrRow, 1);   PutScr ();   return true;  }

bool Tab ()
// FAIL IF: can't indent | can't shift cols CsrCol..Len-1 right.
// if not past CsrRow's Len, InsCols between CsrCol and CsrCol after Indent.
{ ubyte Co = CsrCol;
   if (! Indent ())  return false;
   if (Co < CsrRowLen ()) {
      if (! InsCols (CsrRow, Co, SC(ubyte,CsrCol-Co)))  return false;
      PutRow (ScrRow);
   }
   return true;
}

bool Rtrn ()
// FAIL IF: can't move down and insert a row
// move down, insert a row, move to col 0.  indent if prev row col 0 is a space.
{  if (! Dn ())      return false;
   if (! InsRow ())  return false;
   CsrCol = 0;
   if (Indent () && (Row [CsrRow-1]->Col [0] != ' '))  CsrCol = 0;
   return true;   // ^Indent fails if CsrRow==0 or CsrRow-1's Len==0
}

bool Split ()
// FAIL IF: can't Rtrn  ...duhhhhhh
// if past CsrRow's Len, just Rtrn.
// else CopyCols CsrCol..Len-1 to "CsrCol after Rtrn", fix screen.
{ uword Ro = CsrRow;
  ubyte Co = CsrCol, Ln = CsrRowLen ();
   if (! Rtrn ())  return false;
   if (Co < Ln) {
      if (! MoveCols (Ro, Co, CsrRow, CsrCol, SC(ubyte,Ln-Co)))  return false;
      if (ScrRow)  PutRow (SC(ubyte,ScrRow-1));
      PutRow (ScrRow);
   }     // ^a single row screen can happen if the screen can be resized
   return true;
}

bool DelWrdRit ()
// FAIL IF: can't move next row up.
// if not past last char of row,
//      if not on a space, delete all chars up to next space, redo screen.
//      else               delete all spaces up to next char, redo screen.
// else if next row exists, find it's first nonspace char.
//      MoveCols next row to CsrCol of CsrRow if possible.
//      delete next row.
{ ubyte len, DelLen, SrcCol;
  pcol  col;
   if (CsrCol < (len = CsrRowLen ())) {
      len -= CsrCol; DelLen = 1;
      if (*(col = CsrRowPtr () + CsrCol) != ' ')
            while ((DelLen < len) && (*(++col) != ' '))  DelLen++;
      else  while ((DelLen < len) && (*(++col) == ' '))  DelLen++;
      DelCols (CsrRow, CsrCol, DelLen);
   }
   else
      if (CsrRow + 1 < EndRow) {
         len = Row [CsrRow+1]->Len;   col = & Row [CsrRow+1]->Col [0];
         for (SrcCol = 0;  (SrcCol < len) && (*(col++) == ' ');  SrcCol++)  ;
         if (! MoveCols (SC(uword,CsrRow+1), SrcCol, CsrRow, CsrCol,
                         SC(ubyte,len-SrcCol)))  return false;
         Dn ();   DelRow ();   Up ();  // always works cuz CsrRow+1 < EndRow
      }
   PutRow (ScrRow);   return true;
}

bool DelWrdLft ()
// FAIL IF: no prev row | can't move row up to CsrRow-1.
// if past col of Len, move to this col.
// else if not on col 0,
//    if prev col has a nonspace char, backspace till prev col has space.
//    else                             backspace till prev col has nonspace.
// else
//    if no prev row, fail.
//    if can't MoveCols CsrRow's cols to end of prev row, fail.
//    delete CsrRow, move to old Len of prev row.
{ ubyte len, DelLen;
  pcol  col;
   if      (CsrCol > (len = CsrRowLen ()))  CsrCol = len;
   else if (CsrCol) {
      DelLen = 1;
      if (*(col = CsrRowPtr () + CsrCol-1) != ' ')
            while ((--CsrCol) && (*(--col) != ' '))  DelLen++;
      else  while ((--CsrCol) && (*(--col) == ' '))  DelLen++;
      DelCols (CsrRow, CsrCol, DelLen);
   }
   else {
      if (CsrRow == 0) return false;
      len = (CsrRow-1 < EndRow) ? Row [CsrRow-1]->Len : 0;
      if (! MoveCols (CsrRow, 0, SC(uword,CsrRow-1), len, CsrRowLen ()))
                      return false;
      DelRow ();   Up ();   if ((CsrCol = len) == 80)  CsrCol--;
   }
   PutRow (ScrRow);   return true;
}
