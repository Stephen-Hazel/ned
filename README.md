## ned (Nifty text EDitor)
``` 
an old school 80 columns monospace (ONLY!) text editor.
originally on the C=64 in 6510 asm.  Then to C on Amiga500, then msdos, then MacClassic, then on win95/10 in barely C++

environment:  
  qtcreator and c++
  Kubuntu   Qt6
  ChromeOS  Qt6 Crostini container (based on Debian buster)
  RasPi     Qt5 (until i can apt install Qt6 sigh - use qmake there)

Uses my Stv library of c++ assumed to be in ../stv/*
  add ../stv/os.cpp, ui.cpp, uiKey.cpp

Also note that font comes from ~/.config/StephenHazel/NEd.conf 's font and fontpt lines
(or defaults to Monospace 14 and 30 rows)

```

## TODO:
```
* get ridda Qt6 runtime error messages on chromeos and see what can be fixed

* find
      find all at start with count (maybe cache of row,cols?)
         if FindStr exists,
            pos/count right after filename
            hilite all on screen
      list of FindStr's, not just 1.  sepd by ||| ?
      during replace, if it won't fit within line, warn but don't stop
