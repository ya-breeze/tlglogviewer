tlglogviewer
============

Qt-based GUI utility to show sequence diagrams in Teligent TLG log files

![](tlglog.png?raw=true)

Features:
* parse p90ctlg/workstation formats
* determine sessions and paint them in different colors
* show values of main ficses right on the callflow - for example EXECNAME for ALE
* drag&drop of components (with Ctrl key - insert before, without - swap)
* scroll with mouse wheel, zoom with Ctrl
* reverse callflow upside down
* filters by: sessions, components etc
* fast-filter: $DESTFUNC=INBC && TO_NUMBER=79262164700 - it means, that tlglogV should search for INBC transactions with given number as a FICS value

Wishlist:
* center on current scene point on zooming - don't work at the moment (
* import: tcpdumped (from 191AE060-907 for example), exported format )))
