// CmdFile.cpp

#include "ned.h"

bool Wipe ()
// free up all the rowmem structures and init everything to 0.
{ pmem tmp;
   while (MemTop)
      {tmp = MemTop;  MemTop = tmp->Nxt;  delete tmp;}
   EndRow = FreeRows = CsrRow = ScrRow = 0;   CsrCol = 0;
   BlkBgnRow = BlkEndRow = 0;  BlkBgnCol = BlkEndCol = 0;
   PutScr ();   return true;
}

bool Load ()
// FAIL IF: can't open file | can't read part of it | no room for text
// if no text, Wipe just in case, and use FName, else GetFname to append.
// if open() fails, return false.
// keep read()ing buf till a 0 len is returned, or error.
//   keep copying buf into Row[]s until buffer is used up or error:
//     if on column 0, insert a row.
//     if '\n' is hit or row is filled, set row's Len and move to next row.
//     if normal ascii, stick it in row.
//     if '\t' is hit, pad in spaces.
// close file, shrink EndRow, fix screen, return ok status.
{ File  f;
  ulong pos, len;
  pcol  col;
  char  txt;
  uword ro = CsrRow;
  ubyte co = 0;
  bool  ok = false;
  TStr  fn;
  PStr  buf;
   if (EndRow) {   StrCp (fn, FName);             AskR (WIN, fn,  "Append:");}
   else        {Wipe ();  if (FName [0] == '\0')  AskR (WIN, FName, "Load:");}
   if (f.Open (EndRow ? fn : FName, "r")) {
      col = Row [ro]->Col;             // stupid c++ compiler :(
      do {
         ok = (len = f.Get (buf, sizeof (buf))) != 0;
         for (pos = 0; ok && (pos < len); pos++) {
            txt = buf [pos];
            if (((txt < ' ') || (txt > '~')) && (txt != '\n') && (txt != '\t'))
               continue;
            if (co == 0) {
               if ((ok = (ro < EndRow) ? InsRows (ro, 1) : ExtendSpc (ro, 0)))
                  col = Row [ro]->Col;
               else break;
            }
            if ((co == 80) || (txt == '\n')) {
               Row [ro]->Len = co;  ShrinkLen (ro);
               ro++;   co = 0;
               if (txt != '\n') pos--;  /* use it next time thru */
            }
            else if ((txt >= ' ') && (txt <= '~'))  col [co++] = txt;
            else if (txt == '\t')
               do col [co++] = ' ';  while ((co < 80) && (co % TAB_LEN));
         }
      } while (ok && len);
      if (ok)  {Row [ro]->Len = co;  ShrinkLen (ro);}
      f.Shut ();
   }
   ShrinkEnd ();  PutScr ();
   return ok;
}

bool Save ()
// FAIL IF: can't open file | save a part of it.
// if open() fails, set ok to false, fall thru.
// for each defined row:
//   if needed, flush buf.
//   copy the defined columns plus a '\n' into buf.
// if needed, flush buf.  close file.  return ok status.
{ File  f;
  ulong len = 0;
  char *Dst;
  pcol  Src;
  uword ro;
  ubyte i;
  bool  ok;
  PStr  buf;
   if (! f.Open (FName, "w"))  return false;
   for (ok = true, ro = 0;  ok && (ro < EndRow);  ro++) {
      i = Row [ro]->Len;
      if ((len + i + 1) >= sizeof (buf)) {  // the 1 is for the '\n' added
         if ((ok = (f.Put (buf, len) == len)))  len = 0;
         else  break;
      }
      Dst = buf + len;   Src = Row [ro]->Col;   len += i;
      while (i--)  *Dst++ = *Src++;
      buf [len++] = '\n';
   }
   if (ok && len)  f.Put (buf, len);
   f.Shut ();
   return ok;
}

bool SaveAs ()
// ...just set FName, Len.
{ TStr fn;
   StrCp (fn, FName);
   if (AskW (WIN, fn, "Save as"))  StrCp (FName, fn);   else return false;
   return Save ();
}

bool Quit ()  {APP->quit ();   return false;}
