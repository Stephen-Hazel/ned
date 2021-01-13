#ifndef NED_H
#define NED_H

#include <QMainWindow>
#include <QScreen>
#include <QWindow>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPixmap>

#include "../stv/ui.h"
#include "../stv/uiKey.h"

#define MAX_ROW    32000     // max # of rows Ned will hold
#define ROWS2GRAB  64        // rows to alloc at a time (must % MAX_ROW!!!)
#define MAX_SQNC   80        // maximum keys in a sequence
#define TAB_LEN    3         // # of spaces to turn tabs into on load/paste

typedef struct {key Key;   ubyte Map [3];}  cmdmap;

typedef struct {ubyte Len;   char Col [80];}  row;
typedef struct memTag {row Mem [ROWS2GRAB];   memTag *Nxt;} mem;
typedef row  *prow;
typedef char *pcol;          // pointer to a particular row's columns
typedef mem  *pmem;

typedef struct {QPixmap *pm;   uword wLn, hCh, hBL;   ubyte min, max;} ScrDef;

// all those nasty globs...
extern QApplication *APP;
extern QMainWindow  *WIN;

extern pmem   MemTop;                     // memory management
extern prow   Row [];
extern uword  FreeRows;
extern uword  CsrRow, EndRow;             // scrolling, cursor pos
extern ubyte  CsrCol, ScrRow, EndScr;
extern bool   Over, Sqnc;                 // those damn flag type vars
extern uword  BlkBgnRow, BlkEndRow;       // block marking params
extern ubyte  BlkBgnCol, BlkEndCol;
extern char   FindStr [], RplcStr [], FName [];  // strings
extern ubyte  FindLen,    RplcLen;
extern key    KeySqnc [];                 // (weak) macro control
extern ubyte  SqncLen;
extern bool (*Cmd [])();                  // command key mapping
extern cmdmap CmdMap [];
extern ubyte  CmdLen;
extern char   KeyMode, *KMMsg, *KMBuf, KM1;
extern bool (*KMCmd)();
extern row    Info, Empt;
extern ScrDef Scr;


// Ned UI stuff
void PutRow  (ubyte ScrPos);
void PutScr  ();
bool PutBlk  ();               // ...this guy is one of the BlkCmds
void JotDn   (char Key);
void DoKey   (key Key);

// declarations for SubCmds.c
char  ToUpper   (char c);
bool  StrICmp   (char *str, pcol col);
uword St2In     (char *Str);
void  In2St     (uword Int, ubyte Len, char *Str);
ubyte CsrRowLen (void);
pcol  CsrRowPtr (void);
bool  OnScr     (uword Ro);
void  MoveTo    (uword Ro, ubyte Co);
void  MoveToMid (uword Ro, ubyte Co);
bool  ExtendSpc (uword Ro, ubyte Ln);
void  ShrinkEnd (void);
void  ShrinkLen (uword Ro);
bool  InsRows   (uword Ro, uword Ln);
void  DelRows   (uword Ro, uword Ln);
bool  InsCols   (uword Ro, ubyte Co, ubyte Ln);
void  DelCols   (uword Ro, ubyte Co, ubyte Ln);
bool  CopyCols  (uword SRo, ubyte SCo, uword DRo, ubyte DCo, ubyte Ln);
bool  MoveCols  (uword SRo, ubyte SCo, uword DRo, ubyte DCo, ubyte Ln);

// all the standard Ned Cmds...
bool Rit(), Lft(), Up(), Dn(), Top(), Bot(), BlkHop(), PgUp(), PgDn(),
     WrdRit(), WrdEnd(), WrdLft(),
     TglOver(), TglWrap(), Del(), Tab(), Cntr (), Rtrn(), Split(),
     InsRow(), DelRow(), DelWrdRit(), DelWrdLft(),
     BlkBgn(), BlkEnd(), BlkAll(),
     BlkRit(), BlkLft(),
     BlkDel(), BlkCopy(), BlkMove(), BlkFmt (),
     BlkCpy(), BlkPst(), BlkSum(),
     Find(), FindNxt(), Rplc(), RplcNxt (),
     Wipe(), Load(), Save(), SaveAs(), Quit(),
     UnDelRow(), TglSqnc(), RptSqnc(), UpCase(), LoCase(), EndCmd();


//------------------------------------------------------------------------------
class NEd: public QMainWindow {
   Q_OBJECT
public:
   explicit  NEd (QWidget *parent = nullptr);
            ~NEd ();
   void Init ();

protected:
   void wheelEvent    (QWheelEvent  *e);
   void keyPressEvent (QKeyEvent    *e);
   void paintEvent    (QPaintEvent  *e);
   void resizeEvent   (QResizeEvent *e);
};

#endif // NED_H
