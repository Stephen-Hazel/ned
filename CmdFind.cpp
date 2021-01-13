// CmdFind.cpp - find and replace functions - hellish due to UI messin

#include "ned.h"

bool FindNxt ()
// FAIL IF: no FindStr | couldn't find it  ...duhhhh
// check rows, wrapping at EndRow, stopping at CsrRow:
//   until a match is found:        leave cursor here, fix screen.
//      or old position is reached: fail (no match found).
{ uword ro, roend;
  ubyte co, coend;
  pcol  col;
   if (FindLen) {
      ro = CsrRow;   co = CsrCol+1;
      if (ro >= EndRow)  ro = co = 0;
      roend = ro;
      do {
         if ((coend = Row [ro]->Len) >= FindLen)
            for (coend -= FindLen, col = Row [ro]->Col;  co <= coend;  co++)
               if (StrICmp (FindStr, & col [co]))
                  {MoveToMid (ro, co);   PutScr ();   return true;}
         co = 0;   if (++ro == EndRow)  ro = 0;
      } while (ro != roend);
   }
   return false;
}

bool Find2 ()
{ char *ptr;
   FindLen = SC(ubyte,StrLn (FindStr));
   for (ptr = FindStr;  (*ptr = ToUpper (*ptr));  ptr++);
   return FindNxt ();
}

bool Find ()
// just get string n use Find2 to uppercase it, get FindLen, FindNxt()
{  KeyMode = 's';   KM1 = 'y';
   KMMsg = CC("Find: ");   KMBuf = FindStr;   KMCmd = Find2;
   return true;
}


// Rplc stuff - lotsa hellish "state" stuff...:(
struct {
  uword Ro, OldCsrRow;
  ubyte Co, OldCsrCol, OldScrRow;
  char  Ch;
} R;

bool Rplc5 ()  // comin back from No room.  Stop here?  [Y|N]...
{  if (R.Ch == 'Y')  {PutScr ();   return false;}      // stay here n done
                      R.Co++;      return FindNxt ();  // go on
}

bool Rplc4 ()
// if Q, restore pos n exit
// if Y, replace it else bump R.Co;  RplcNxt()
{ ubyte i;
  pcol  Src;
  char *Dst;
   if (R.Ch == 'Q') {
      CsrRow = R.OldCsrRow;   CsrCol = R.OldCsrCol;   ScrRow = R.OldScrRow;
      PutScr ();   return false;
   }
   if (R.Ch == 'Y') {
      if ((Row [R.Ro]->Len - FindLen + RplcLen) > 80) {
      // whoops - one more dang Rplc5 func :(
         KMMsg = CC("No room.  Stop here?  [Y|N]");
         KeyMode = 'c';   KMBuf = & R.Ch;   KMCmd = Rplc5;
         return true;
      }
      if (RplcLen < FindLen)
           DelCols (R.Ro, R.Co, FindLen-RplcLen);
      else InsCols (R.Ro, R.Co, RplcLen-FindLen);
      i = RplcLen;   Dst = & Row [R.Ro]->Col [R.Co];   Src = RplcStr;
      while (i--)  *(Dst++) = *(Src++);
      ShrinkLen (R.Ro);
      R.Co += RplcLen;
   }
   else // R.Ch == 'N'
      R.Co++;
   return RplcNxt ();
}

bool RplcNxt ()
// if you can find it, ask n send to Rplc4
// else restore cursor pos n done
{ pcol  col;
  ubyte coend;
   for (;  R.Ro < EndRow;  R.Ro++)
      if ((coend = Row [R.Ro]->Len) >= FindLen) {
         for (coend -= FindLen, col = Row [R.Ro]->Col;  R.Co <= coend;  R.Co++)
            if (StrICmp (FindStr, & col [R.Co])) {
            // show where n ask...
               MoveToMid (R.Ro, R.Co);   PutScr ();
               KeyMode = 'c';    KMMsg = CC("Replace this?  [Y|N|Q]");
               KMBuf = & R.Ch;   KMCmd = Rplc4;
               return true;
            }
         R.Co = 0;
      }
// restore cursor spot, quit
   CsrRow = R.OldCsrRow;   CsrCol = R.OldCsrCol;   ScrRow = R.OldScrRow;
   PutScr ();
   return false;
}

bool Rplc3 ()
// get len of RplcStr, initialize state info n do RplcNxt()
{  RplcLen = SC(ubyte,StrLn (RplcStr));
   R.OldCsrRow = CsrRow;   R.OldCsrCol = CsrCol;   R.OldScrRow = ScrRow;
   R.Ro = 0;   R.Co = 0;
   return RplcNxt ();
}

bool Rplc2 ()
// upper case, get len of FindStr n send to get RplcStr
{ char *ptr;
   FindLen = SC(ubyte,StrLn (FindStr));
   for (ptr = FindStr;  (*ptr = ToUpper (*ptr));  ptr++);
   if (FindLen) {
      KeyMode = 's';   KM1 = 'y';
      KMMsg = CC("With: ");      KMBuf = RplcStr;   KMCmd = Rplc3;
   }
   return true;
}

bool Rplc ()
// get FindStr n send to Rplc2
{     KeyMode = 's';   KM1 = 'y';
      KMMsg = CC("Replace: ");   KMBuf = FindStr;   KMCmd = Rplc2;
   return true;
}
