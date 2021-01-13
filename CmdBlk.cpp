// CmdBlk.cpp - commands for manipulating blocks of text.

#include "ned.h"

#define BOX_BLK()  (BlkBgnCol != BlkEndCol)

static bool ShiftBlk (ubyte d, ubyte i)
// FAIL IF: nonspace chars will be deleted
// delete d Col and insert a space at i Col for each row in block.
{ uword r;
   for (r = BlkBgnRow; (r <= BlkEndRow) && (r < EndRow); r++)
      if ((Row [r]->Len > d) && (Row [r]->Col [d] != ' ')) return (false);
   for (r = BlkBgnRow; r <= BlkEndRow; r++)
      {DelCols (r, d, 1);  InsCols (r, i, 1);}
   PutScr ();  return true;
}

bool BlkBgn ()
{  BlkBgnRow = CsrRow; BlkBgnCol = CsrCol;
   if (BlkBgnRow > BlkEndRow) BlkEndRow = BlkBgnRow;
   if (BlkBgnCol > BlkEndCol) BlkEndCol = BlkBgnCol;
   return true;
}

bool BlkEnd ()
{  BlkEndRow = CsrRow; BlkEndCol = CsrCol;
   if (BlkBgnRow > BlkEndRow) BlkBgnRow = BlkEndRow;
   if (BlkBgnCol > BlkEndCol) BlkBgnCol = BlkEndCol;
   return true;
}

bool BlkAll ()
{  BlkBgnRow = 0;  BlkEndRow = EndRow;  BlkBgnCol = 0;  BlkEndCol = 0;
   if (BlkEndRow)  BlkEndRow--;
   return true;
}

bool BlkRit ()
// FAIL IF: chars will be lost.   shift block right one col.
{  if (BOX_BLK ()) return ShiftBlk (BlkEndCol, BlkBgnCol);
   else            return ShiftBlk (79,        0);
}

bool BlkLft ()
// FAIL IF: chars will be lost.   shift block left one col.
{  if (BOX_BLK ()) return ShiftBlk (BlkBgnCol, BlkEndCol);
   else            return ShiftBlk (0,         79);
}

bool BlkDel ()
// if box block, DelCols in each row. else delete row block.  fix screen.
{ uword r;
  ubyte BlkW = BlkEndCol-BlkBgnCol+1;
   if (BOX_BLK ())
      for (r = BlkBgnRow; r <= BlkEndRow; r++) DelCols (r, BlkBgnCol, BlkW);
   else {
      DelRows (BlkBgnRow, SC(uword,BlkEndRow-BlkBgnRow+1));
      MoveTo  (BlkBgnRow, CsrCol);
   }
   PutScr ();  return true;
}

bool BlkCopy ()
// FAIL IF: copying into middle of src | no room for dst.
// if box block,
//   copy block cols in correct direction.
//   fix BlkBgn/EndCol.
// else row block,
//   for each new row: copy Lens and the used Cols.
//   fix EndRow.
// fix BlkBgn/EndRow and screen.
{ pcol     Dst, Src;
  uword r, DRo, SRo, BlkH = BlkEndRow-BlkBgnRow+1;
  ubyte len,         BlkW = BlkEndCol-BlkBgnCol+1;
   if (BOX_BLK ()) {
      if (((CsrRow >= BlkBgnRow) && (CsrRow <= BlkEndRow) &&
           (CsrCol >= BlkBgnCol) && (CsrCol <= BlkEndCol)) ||
          ((CsrCol + BlkW) > 80))                               return false;
      for (r = CsrRow; (r < (CsrRow + BlkH)) && (r < EndRow); r++)
         if (Row [r]->Len + BlkW > 80)                          return false;
      if (CsrRow < BlkBgnRow) {
         for (SRo = BlkBgnRow, DRo = CsrRow,        r = BlkH; r--; SRo++, DRo++)
            if (! CopyCols (SRo, BlkBgnCol, DRo, CsrCol, BlkW)) return false;
      }
      else
         for (SRo = BlkEndRow, DRo = CsrRow+BlkH-1, r = BlkH; r--; SRo--, DRo--)
            if (! CopyCols (SRo, BlkBgnCol, DRo, CsrCol, BlkW)) return false;
      BlkBgnCol = CsrCol;  BlkEndCol = CsrCol+BlkW-1;
   }
   else {
      if ((CsrRow > BlkBgnRow) && (CsrRow <= BlkEndRow))        return false;
      if (CsrRow < EndRow)
           {if (! InsRows (CsrRow, BlkH))                       return false;}
      else {if (! ExtendSpc (SC(uword,CsrRow+BlkH-1), 0))       return false;}
      for (DRo = CsrRow, SRo = BlkBgnRow, r = BlkH; r--; DRo++, SRo++) {
         for (Dst = Row [DRo]->Col, Src = Row [SRo]->Col,
              len = Row [DRo]->Len = Row [SRo]->Len; len--;)
            *(Dst++) = *(Src++);
      }
      ShrinkEnd ();
   }
   BlkBgnRow = CsrRow;  BlkEndRow = CsrRow+BlkH-1;
   PutScr ();
   return true;
}

bool BlkMove ()
// FAIL IF: moving into middle of src | no room for dst.
// if box block,
//   move block cols in correct direction.
//   fix CsrCol and BlkBgn/EndCol.
// else row block,
//   if EndRow can't be extended, fail.
//   if moving block up, set Top/Bot to CsrRow..BgnRow-1 / BgnRow..EndRow.
//   else                 "     "    "  BgnRow..EndRow / EndRow+1..CsrRow-1,
//                       and scoot CsrRow up BlkH rows.
//   for each Top+Bot row:
//     remember a row, find where it goes, swap it with the current contents.
//     if you come to a row that's already moved, bump down a row.
//   fix EndRow.
// fix BlkBgn/EndRow and screen.
{ prow  tmp, rem;
  uword r, i, Top, Bot, TopLen, BotLen, SRo, DRo,
                 BlkH = BlkEndRow-BlkBgnRow+1;
  ubyte DCo, BlkW = BlkEndCol-BlkBgnCol+1;
   if (BOX_BLK ()) {
      if ((CsrRow >= BlkBgnRow) && (CsrRow <= BlkEndRow) &&
          (CsrCol >= BlkBgnCol) && (CsrCol <= BlkEndCol))        return false;
      if ((CsrRow > BlkBgnRow) && (CsrRow <= BlkEndRow) && (CsrCol > BlkEndCol))
           DCo = CsrCol - BlkW;
      else DCo = CsrCol;
      if ((CsrRow != BlkBgnRow) && (DCo + BlkW > 80))            return false;
      if (CsrRow < BlkBgnRow) {r = CsrRow;       Bot = BlkBgnRow;}
      else                    {r = BlkEndRow+1;  Bot = CsrRow + BlkH;}
      while ((r < Bot) && (r < EndRow))
         if (Row [r++]->Len + BlkW > 80)                         return false;
      if (CsrRow < BlkBgnRow) {
         for (SRo = BlkBgnRow, DRo = CsrRow,        r = BlkH; r--; SRo++, DRo++)
            if (! MoveCols (SRo, BlkBgnCol, DRo, DCo, BlkW))     return false;
      }
      else
         for (SRo = BlkEndRow, DRo = CsrRow+BlkH-1, r = BlkH; r--; SRo--, DRo--)
            if (! MoveCols (SRo, BlkBgnCol, DRo, DCo, BlkW))     return false;
      if ((CsrRow >= BlkBgnRow) && (CsrRow <= BlkEndRow) &&
          (CsrCol > BlkEndCol)) CsrCol -= BlkW;
      BlkBgnCol = CsrCol;  BlkEndCol = CsrCol+BlkW-1;
   }
   else {
      if ((CsrRow >= BlkBgnRow) && (CsrRow <= BlkEndRow))        return false;
      if (! (ExtendSpc (CsrRow, 0) || ExtendSpc (BlkEndRow, 0))) return false;
      if (CsrRow < BlkBgnRow) {
         Top = CsrRow;    Bot = BlkBgnRow;   TopLen = Bot-CsrRow; BotLen = BlkH;
      }
      else {
         Top = BlkBgnRow; Bot = BlkEndRow+1; TopLen = BlkH; BotLen = CsrRow-Bot;
         CsrRow -= BlkH;  if (CsrRow < ScrRow) ScrRow = SC(ubyte,CsrRow);
      }
      for (r = TopLen+BotLen, rem = Row [i = Top]; r--;) {
         if (i < Bot) i += BotLen;  else i -= TopLen;
         tmp = Row [i];  Row [i] = rem;  rem = tmp;     // swap rem w Row [i]
         if (i == Top) rem = Row [i = ++Top];        // ...already been moved
      }
      ShrinkEnd ();
   }
   BlkBgnRow = CsrRow;  BlkEndRow = CsrRow+BlkH-1;
   PutScr ();
   return true;
}

bool BlkFmt ()
// FAIL IF: can't fit a word withing BlkW | can't insert a new row (4 xs txt).
// for each row in block, format text into BlkBgnCol..BlkEndCol:
//   if excess text for BlkW (starting at BgnCol),
//        find where to clip extra off, move it to next (inserted) row.
//        shift row to BgnCol.
//   else shift row to BgnCol.
//        repeat till next row won't fit,
//          find where to tack on new stuff, move stuff up if it'll fit.
//          delete next row if now empty.
{ ubyte len, len2, SCo, DCo, bgn, end, BgnCol, EndCol, BlkW;
  pcol  col, col2;
  uword r;
  char  c;
  bool  mofit;
   if (BOX_BLK ()) {BgnCol = BlkBgnCol;  EndCol = BlkEndCol;}
   else            {BgnCol = 0;          EndCol = 79;}
   BlkW = EndCol-BgnCol+1;
   for (r = BlkBgnRow; (r <= BlkEndRow) && (r < EndRow); r++) {
      col = & Row [r]->Col [0];  len = Row [r]->Len;
      for (bgn = 0; (bgn < len) && (col [bgn] == ' '); bgn++);
      if ((len - bgn) > BlkW) {
         for (SCo = bgn + BlkW; (bgn < SCo) && (col [SCo] != ' '); SCo--);
         if ((bgn == SCo) ||
             (! InsRows (SC(uword,r+1), 1)) ||
             (! MoveCols (r, SCo, SC(uword,r+1), 0, SC(ubyte,len-SCo))))
            {MoveTo (r, CsrCol);  PutScr ();  return false;}
         if (r == BlkEndRow) BlkEndRow++;
         if (bgn < BgnCol) InsCols (r, 0, SC(ubyte,BgnCol-bgn));
         else              DelCols (r, 0, SC(ubyte,bgn-BgnCol));
      }
      else {
         if (bgn < BgnCol) InsCols (r, 0, SC(ubyte,BgnCol-bgn));
         else              DelCols (r, 0, SC(ubyte,bgn-BgnCol));
         do {
            if ((len = Row [r]->Len) == 0) DCo = BgnCol;
            else {
               DCo = len+1;
               c = col [len-1];
               if ((c == '.') || (c == '!') || (c == '?')) DCo++;
            }
            mofit = false;
            if ((DCo <= EndCol) && (r < BlkEndRow)) {
               for (col2 = & Row [r+1]->Col [0], len2 = Row [r+1]->Len,
                    SCo = 0; (SCo < len2) && (col2 [SCo] == ' '); SCo++);
               if ((end = SCo + EndCol-DCo+1) >= len2) {
                  if (len2)
                     MoveCols (SC(uword,r+1), SCo, r, DCo, SC(ubyte,len2-SCo));
                  if (r+1 == BlkEndRow) BlkEndRow--;
                  DelRows (SC(uword,r+1), 1);
                  mofit = true;
               }
               else {
                  while ((SCo < end) && (col2 [end] != ' ')) end--;
                  if (SCo != end) {
                     while (col2 [end] == ' ') end--;
                     MoveCols (SC(uword,r+1), SCo, r, DCo, SC(ubyte,end-SCo+1));
                  }
               }
            }
         } while (mofit);
      }
   }
   MoveTo (BlkBgnRow, CsrCol);  PutScr ();  return true;
}

bool BlkCpy ()
// if box block,
// else row block,
//   get size of buf we'll need.  build it.  copy it to clipboard
{ uword r, Ro, BlkH = BlkEndRow-BlkBgnRow+1;
  ubyte        BlkW = BlkEndCol-BlkBgnCol+1;
  ulong len = 0;
  char *buf;
   for (Ro = BlkBgnRow, r = BlkH; r--; Ro++) {
      if (BOX_BLK ())  len += BlkW;
      else             {if (Ro < EndRow) len += Row [Ro]->Len;}
      if (r)  len += 2;  else len++;
   }
   buf = new char [len];
   len = 0;
   for (Ro = BlkBgnRow, r = BlkH; r--; Ro++) {
      if (BOX_BLK ()) {
         MemSet (& buf [len], ' ', BlkW);
         if ((Ro < EndRow) && (BlkBgnCol < Row [Ro]->Len)) {
            if (BlkEndCol < Row [Ro]->Len)
                 MemCp (& buf [len], & Row [Ro]->Col [BlkBgnCol], BlkW);
            else MemCp (& buf [len], & Row [Ro]->Col [BlkBgnCol],
                        Row [Ro]->Len - BlkBgnCol);
         }
         len += BlkW;
      }
      else {
         if (Ro < EndRow) {
            MemCp (& buf [len], Row [Ro]->Col, Row [Ro]->Len);
            len += Row [Ro]->Len;
         }
      }
      if (r)  {StrCp (& buf [len], CC("\n"));  len++;}
      else              buf [len] = '\0';
   }
   ClipPut (APP, buf);
   delete [] buf;
   return true;
}

bool BlkPst ()
{ char *buf;
  ulong len, pos;
  uword ro = CsrRow;
  ubyte co = 0;
  char  txt, *col;
   if ((len = ClipGetLen (APP)) == 0)  return false;
   buf = new char [len+1];
   ClipGet (APP, buf, len+1);
   if ((StrCh (buf, '\n') == nullptr) &&
       ((ro >= EndRow) || (Row [ro]->Len + len <= 80)) &&
       (CsrCol + len <= 80)) {
   // single line that'll fit right here
      if ((ro < EndRow) && (CsrCol < Row [ro]->Len)) {
           if (! InsCols   (ro,          CsrCol,SC(ubyte,len)))
              {delete [] buf;  return false;}
      }
      else if (! ExtendSpc (ro, SC(ubyte,CsrCol+len)))
              {delete [] buf;  return false;}
      MemCp (& Row [ro]->Col [CsrCol], buf, len);
      ShrinkLen (ro);
   }
   else {
   // just like loading a file
      for (pos = 0; pos < len; pos++) {
         txt = buf [pos];
         if (((txt < ' ') || (txt > '~')) && (txt != '\n') && (txt != '\t'))
            continue;
         if (co == 0) {
            if ((ro < EndRow) ? InsRows (ro, 1) : ExtendSpc (ro, 0))
                  col = Row [ro]->Col;
            else {delete [] buf;  return false;}
         }
         if ((co == 80) || (txt == '\n')) {
            Row [ro]->Len = co;  ShrinkLen (ro);
            ro++;  co = 0;
            if (txt != '\n') pos--;  // use it next time thru
         }
         else if ((txt >= ' ') && (txt <= '~'))  col [co++] = txt;
         else if (txt == '\t')
            do col [co++] = ' ';  while ((co < 80) && (co % TAB_LEN));
      }
      Row [ro]->Len = co;  ShrinkLen (ro);
   }
   delete [] buf;
   ShrinkEnd ();  PutScr ();  return true;
}

bool BlkSum ()
// sum up block and ins into line below it
{ uword r, Ro, BlkH = BlkEndRow-BlkBgnRow+1;
  ubyte        BlkW = BlkEndCol-BlkBgnCol+1;
  ulong len = 0, sum = 0;
  char  buf [81], *p;
   for (Ro = BlkBgnRow, r = BlkH; r--; Ro++) {
      len = 0;
      if (BOX_BLK ()) {
         if ((Ro < EndRow) && (BlkBgnCol < Row [Ro]->Len)) {
            if (BlkEndCol < Row [Ro]->Len)
                 MemCp (buf, & Row [Ro]->Col [BlkBgnCol], len = BlkW);
            else MemCp (buf, & Row [Ro]->Col [BlkBgnCol],
                        len = Row [Ro]->Len - BlkBgnCol);
         }
      }
      else
         if (Ro < EndRow)
                 MemCp (buf, Row [Ro]->Col, len = Row [Ro]->Len);
      buf [len] = '\0';
      sum += SC(ulong,Str2Int (buf));
   }
   for (p = Int2Str (SC(slong,sum), buf);  *p;  p++)  JotDn (*p);
   return true;
}
