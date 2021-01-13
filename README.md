# ned (nifty text editor)
...
an old school 80 columns (ONLY!) text editor.
originally wrote on the C=64 in 6510 asm.  Then to C.  Then barely to C++.

environment:  ChromeOS' Debian linux container.  c++.  Qt 6.
via QtCreator dev env

originally written on in 6510 on C=64, then in C on Amiga500, then on msdos, 
then on MacClassic, then in c++ on win95.
Raspberry Pi OS is next.

Uses my Stv library of c++ assumed to be in ../stv/*


# TODO:
...
* get ridda Qt6 runtime error messages and see what can be fixed

* find
      find all at start with count (maybe cache of row,cols?)
         if FindStr exists,
            pos/count right after filename
            hilite all on screen
      list of FindStr's, not just 1.  sepd by ||| ?
      during replace, if it won't fit within line, warn but don't stop
