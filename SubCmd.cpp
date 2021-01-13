// SubCmd.cpp - general functions used by the command routines.

#include "ned.h"

char ToUpper (char c)
{  if ((c >= 'a') && (c <= 'z')) c += ('A'-'a');  return c;  }

bool StrICmp (char *str, pcol col)
// compare str to where col points, ignoring upper/lower case of col
{  while (*str) if (*(str++) != ToUpper (*(col++))) return false;
   return true;
}

uword St2In (char *Str)
// keep reading a number from a string until you hit a nonnumeric character
{ uword i = 0;
   while ((*Str >= '0') && (*Str <= '9'))  i = i*10 + SC(uword,*(Str++) - '0');
   return i;
}

void In2St (uword Int, ubyte Len, char *Str)
// store an ascii representation of Int at Str
{  Str += Len;
   do {*(--Str) = SC(char,Int % 10) + '0';  Int /= 10;}  while (--Len);
}

ubyte CsrRowLen ()
{  if (CsrRow < EndRow) return Row [CsrRow]->Len;  return 0;  }

pcol CsrRowPtr ()
{  if (CsrRow < EndRow) return Row [CsrRow]->Col;  return nullptr;  }

bool OnScr (uword Ro)
// tell if Ro is currently visible on the screen
{ uword TopScrRow = CsrRow - ScrRow;
   return (Ro >= TopScrRow) && (Ro <= TopScrRow+EndScr);
}

void MoveTo (uword Ro, ubyte Co)
// set Csr pos, move ScrRow as little as possible.
// make sure Ro,Co exists.
// be sure top of screen is at least row 0, bot of screen is at most MAX_ROW-1.
{  if (Ro >= MAX_ROW) Ro = MAX_ROW-1;  if (Co >= 80) Co = 79;
   CsrRow = Ro;  CsrCol = Co;
   if (ScrRow > Ro)                    ScrRow = SC(ubyte,Ro);
   if (ScrRow+(MAX_ROW-1) < Ro+EndScr) ScrRow = SC(ubyte,Ro+EndScr-(MAX_ROW-1));
}

void MoveToMid (uword Ro, ubyte Co)
// set Csr pos, set ScrRow as close to middle of screen as possible.
{  ScrRow = EndScr >> 1;  MoveTo (Ro, Co);  }

bool ExtendSpc (uword Ro, ubyte Ln)
// EFFECTS: Ro is left defined. (Ro's Len & EndRow may now be incorrect)
// FAIL IF: Ro >= MAX_ROW | Ln > 80 | no mem for new rows...
// keep allocating rowmem structures and setting up Row[] to point to them.
// if error in allocation, return false.  else set new EndRow.
// inc Ro's Len if needed.
{ ubyte OldLn;
  uword r;
  pmem  RMp;
  prow *row;
   if ((Ro >= MAX_ROW) || (Ln > 80))   return false;
   if (Ro >= EndRow) {
      while (Ro >= EndRow + FreeRows) {
         if (! (RMp = new mem))        return false;
         MemSet (RMp, 0, sizeof (mem));
         RMp->Nxt = MemTop;  MemTop = RMp;
         for (row = & Row [EndRow+FreeRows], r = 0;  r < ROWS2GRAB;  r++)
            *(row++) = & RMp->Mem [r];
         FreeRows += ROWS2GRAB;
      }
      FreeRows -= Ro+1 - EndRow;
      EndRow = Ro+1;
   }
   if (Ln > (OldLn = Row [Ro]->Len)) {
      MemSet (& Row [Ro]->Col [OldLn], ' ', Ln-OldLn);
      Row [Ro]->Len = Ln;
   }
   return true;
}

void ShrinkEnd ()
// keep dec'ing EndRow till it hits 0 or else a non empty row is found
{  while (EndRow && (Row [EndRow-1]->Len == 0))  {EndRow--; FreeRows++;}  }

void ShrinkLen (uword Ro)
// dec Ro's Len while you've got spaces at the end. call ShrinkEnd if needed.
{ pcol  ptr;
  ubyte len;
   if (Ro < EndRow) {
      len = Row [Ro]->Len;  ptr = & Row [Ro]->Col [len];
      while (len && (*(--ptr) == ' '))  len--;
      if ((Row [Ro]->Len = len) == 0)  ShrinkEnd ();
   }
}

bool InsRows (uword Ro, uword Ln)
// FAIL IF: Ro+Ln > MAX_ROW | can't ExtendSpc to get new rows...
// if Ro is undefined,
//   allocate the new rows.
//   shift the pointers to rows Ro..EndRow-1 down Ln slots, bubbling up the
//                                         pointers to rows EndRow..EndRow-1+Ln.
//   update block parameters.
{ prow *row, *bub, Tmp;
  uword r;
   if ((Ro+Ln) > MAX_ROW)                return false;
   if ((Ro < EndRow) && Ln) {
      if (! ExtendSpc (EndRow+Ln-1, 0))  return false;
      for (row = (bub = & Row [EndRow-1]) - Ln, r = EndRow-Ro-Ln;  r--;) {
         Tmp      = *bub;
         *(bub--) = *row;
         *(row--) =  Tmp;
      }
      if (BlkBgnRow >= Ro)  BlkBgnRow += Ln;
      if (BlkEndRow >= Ro)  BlkEndRow += Ln;
   }
   return true;
}

void DelRows (uword Ro, uword Ln)
// if Ro is defined,
//   chop Ln at EndRow.
//   shift the pointers to rows Ro+Ln..EndRow-1 up Ln slots, bubbling down the
//                                                 pointers to rows Ro..Ro+Ln-1.
//   set Len of each deleted row to 0.
//   update EndRow, FreeRows, and block parameters.
{ prow *row, *bub, Tmp;
  uword r;
   if ((Ro < EndRow) && Ln) {
      if (Ro+Ln > EndRow) Ln = EndRow-Ro;
      for (row = (bub = & Row [Ro]) + Ln, r = EndRow-Ro-Ln;  r--;) {
         Tmp      = *bub;
         *(bub++) = *row;
         *(row++) =  Tmp;
      }
      for (r = EndRow-Ln;  r < EndRow;  r++)  Row [r]->Len = 0;
      EndRow -= Ln;   FreeRows += Ln;   ShrinkEnd ();
      if      (BlkBgnRow >= Ro+Ln)  BlkBgnRow -= Ln;
      else if (BlkBgnRow > Ro)      BlkBgnRow  = Ro;
      if      (BlkEndRow >= Ro+Ln)  BlkEndRow -= Ln;
      else if (BlkEndRow > Ro)      BlkEndRow  = Ro;
   }
}

bool InsCols (uword Ro, ubyte Co, ubyte Ln)
// FAIL IF: Ro's Len + Ln > 80
// if Ro,Co is defined,
//  set new Len, shift cols Co..Len-1 right Ln slots, fill in slots with spaces.
{ ubyte len, i;
  pcol  Src, Dst;
   if ((Ro < EndRow) && (Co < (len = Row [Ro]->Len)) && Ln) {
      if ((len + Ln) > 80)  return false;
      Row [Ro]->Len = len + Ln;
      i = len - Co;   Dst = (Src = Row [Ro]->Col + len-1) + Ln;
      while (i--)  *(Dst--) = *(Src--);
      while (Ln--) *(++Src) = ' ';
   }
   return true;
}

void DelCols (uword Ro, ubyte Co, ubyte Ln)
// if Ro,Co is defined,
//   if deleting last col, chop Len at Co and ShrinkLen.
//   else                  set new Len, shift columns Co+Ln..Len-1 left by Ln.
{ ubyte len;
  pcol  Src, Dst;
   if ((Ro < EndRow) && (Co < (len = Row [Ro]->Len)) && Ln) {
      if (Co+Ln >= len)  {Row [Ro]->Len = Co;   ShrinkLen (Ro);}
      else {
         Src = (Dst = & Row [Ro]->Col [Co]) + Ln;
         Ln = (Row [Ro]->Len -= Ln) - Co;
         while (Ln--)  *(Dst++) = *(Src++);
      }
   }
}

bool CopyCols (uword SRo, ubyte SCo, uword DRo, ubyte DCo, ubyte Ln)
// FAIL IF: dst inside of src | no room to ExtendSpc/InsCols in DRo
// if possible, insert room at Dst.
// copy defined part of Src.
// ShrinkLen to lose any trailing spaces.
{ pcol  Src, Dst;
  ubyte len;
   if ((DRo == SRo) && (DCo > SCo) && (DCo < SCo+Ln))  return false;
   if ((DRo < EndRow) && (DCo < Row [DRo]->Len)) {
        if (! InsCols    (DRo,         DCo,Ln))        return false;
   }
   else if (! ExtendSpc  (DRo, DCo+Ln))                return false;
   if ((SRo == DRo) && (SCo >= DCo)) SCo += Ln;
   if ((SRo < EndRow) && (SCo < (len = Row [SRo]->Len))) {
      if (SCo+Ln > len)  Ln = len-SCo;
      Dst = & Row [DRo]->Col [DCo];   Src = & Row [SRo]->Col [SCo];
      while (Ln--)  *(Dst++) = *(Src++);
   }
   ShrinkLen (DRo);
   return true;
}

bool MoveCols (uword SRo, ubyte SCo, uword DRo, ubyte DCo, ubyte Ln)
// FAIL IF: dst inside of src | can't fit src in DRo.
// if src/dst on dif rows, Copy+DelCols.
// else                    put Blk in Buf, DelCols, InsCols, store Buf.
{ ubyte len, BufLen;
  char  Buf [80];
   if (SRo != DRo) {
      if (! CopyCols (SRo, SCo, DRo, DCo, Ln)) return false;
      DelCols        (SRo, SCo, Ln);
   }
   else {
      if ((DCo > SCo) && (DCo < SCo+Ln))       return false;
      if (DRo < EndRow) {           // else can ALWAYS MoveCols into same row
         BufLen = Ln;
         if (SCo+Ln > (len = Row [SRo]->Len))  BufLen = len - SCo;
         MemCp (Buf, & Row [SRo]->Col [SCo], BufLen);
         DelCols (SRo, SCo, Ln);
         if (DCo >= SCo+Ln) DCo -= Ln;
         if (DCo < Row [DRo]->Len) InsCols   (DRo, DCo,Ln);
         else                      ExtendSpc (DRo, DCo+Ln);
         MemCp (& Row [DRo]->Col [DCo], Buf, BufLen);
         ShrinkLen (DRo);
      }
   }
   return true;
}
