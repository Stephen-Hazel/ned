// CmdMap.cpp

#include "ned.h"

bool (*Cmd [])() = {
   EndCmd,  /*00*/
   Rit,     /*01*/  Lft,    /*02*/  Up,       /*03*/  Dn,       /*04*/
   Top,     /*05*/  Bot,    /*06*/  PgUp,     /*07*/  PgDn,     /*08*/
   WrdRit,  /*09*/  WrdEnd, /*0A*/  WrdLft,   /*0B*/
   TglOver, /*0C*/  Del,    /*0D*/  Tab,      /*0E*/  Cntr,     /*0F*/
   Rtrn,    /*10*/  Split,  /*11*/
   InsRow,  /*12*/  DelRow, /*13*/  DelWrdRit,/*14*/  DelWrdLft,/*15*/
   PutBlk,  /*16*/  BlkBgn, /*17*/  BlkEnd,   /*18*/  BlkAll,   /*19*/
   BlkRit,  /*1A*/  BlkLft, /*1B*/
   BlkDel,  /*1C*/  BlkCopy,/*1D*/  BlkMove,  /*1E*/  BlkFmt,   /*1F*/
   BlkCpy,  /*20*/  BlkPst, /*21*/
   Find,    /*22*/  FindNxt,/*23*/  Rplc,     /*24*/
   Wipe,    /*25*/  Load,   /*26*/  Save,     /*27*/  SaveAs,   /*28*/
   Quit,    /*29*/
   UnDelRow,/*2A*/
   TglSqnc, /*2B*/  RptSqnc,/*2C*/
   UpCase,  /*2D*/  LoCase, /*2E*/
   BlkSum,  /*2F*/  BlkHop  /*30  outofseq*/
};

cmdmap CmdMap [] = {
   {CTL|'u',     "\x03"},      // Up
   {CTL|'m',     "\x04"},      // Dn
   {CTL|'j',     "\x01"},      // Rit
   {CTL|'h',     "\x02"},      // Lft
   {CTL|'y',     "\x07"},      // PgUp
   {CTL|'n',     "\x08"},      // PgDn
   {CTL|'\'',    "\x09"},      // WrdRit
   {CTL|'/',     "\x0A"},      // WrdEnd
   {CTL|';',     "\x0B"},      // WrdLft
   {    UP_KEY,  "\x03"},      // Up
   {    DN_KEY,  "\x04"},      // Dn
   {    RIT_KEY, "\x01"},      // Rit
   {    LFT_KEY, "\x02"},      // Lft
   {    HOM_KEY, "\x05"},      // Top
   {    END_KEY, "\x06"},      // Bot
   {CTL|'b',     "\x30"},      // BlkHop
   {    PUP_KEY, "\x07"},      // PgUp
   {    PDN_KEY, "\x08"},      // PgDn
   {    INS_KEY, "\x0C"},      // TglOver
   {CTL|'k',     "\x0D"},      // Del
   {    DEL_KEY, "\x0D"},      // Del
   {    BSP_KEY, "\x02\x0D"},  // Lft Del
   {    TAB_KEY, "\x0E"},      // Tab
   {CTL|'8',     "\x0F"},      // Cntr
   {    RET_KEY, "\x10"},      // Rtrn
   {CTL|RET_KEY, "\x11"},      // Split
   {CTL|',',     "\x13"},      // DelRow
   {CTL|'o',     "\x14"},      // DelWrdRit
   {CTL|'i',     "\x15"},      // DelWrdLft
   {CTL|'p',     "\x16"},      // PutBlk
   {CTL|'[',     "\x17\x16"},  // BlkBgn PutBlk
   {CTL|']',     "\x18\x16"},  // BlkEnd PutBlk
   {CTL|'a',     "\x19\x16"},  // BlkAll PutBlk
   {CTL|'0',     "\x1A"},      // BlkRit
   {CTL|'9',     "\x1B"},      // BlkLft
   {CTL|BSP_KEY, "\x1C"},      // BlkDel
   {CTL|'=',     "\x1D"},      // BlkCopy
   {CTL|'-',     "\x1E"},      // BlkMove
   {CTL|'\\',    "\x1F"},      // BlkFmt
   {CTL|'c',     "\x20"},      // BlkCpy
   {CTL|'v',     "\x21"},      // BlkPst
   {CTL|'s',     "\x2F"},      // BlkSum
   {CTL|'f',     "\x22"},      // Find
   {CTL|'g',     "\x23"},      // FindNxt
   {CTL|'r',     "\x24"},      // Rplc
   {    F01_KEY, "\x26"},      // Load
   {    F02_KEY, "\x27"},      // Save
   {    F03_KEY, "\x28"},      // SaveAs
   {    F06_KEY, "\x25"},      // Wipe
   {    F05_KEY, "\x29"},      // Quit
   {    ESC_KEY, "\x27\x29"},  // Save Quit
   {    F11_KEY, "\x2B"},      // TglSqnc
   {    F12_KEY, "\x2C"},      // RptSqnc
   {CTL|ALT|',', "\x2A"},      // UnDelRow
   {CTL|SHF|' ', "\x2D\x01"},  // UpCase Rit
   {CTL|' ',     "\x2E\x01"}   // LoCase Rit
};

ubyte CmdLen = BITS (CmdMap);
