# Nova Studio – Debugger

## Status: NICHT IMPLEMENTIERT

Kein Debug Adapter Protocol, keine Breakpoints, kein Call Stack, keine
Register-/Speicheranzeige, keine QEMU-Debug-Anbindung.

Die Ribbon-Einträge "Debuggen starten"/"Ohne Debugger starten"/"Stopp"
existieren (siehe `ribbon.cpp`), rufen aber nur `DoBuild()`+`DoRun()`
auf bzw. geben eine Status-Textzeile aus - es handelt sich NICHT um
echtes Debugging, nur um "bauen und (noch nicht automatisiert)
ausführen" unter einem vertrauten Menünamen. Das ist im Code selbst
nicht irreführend benannt (die IDM-Namen entsprechen Visual-Studio-
Konventionen für "Start"/"Start without debugging"), aber hier noch
einmal ausdrücklich klargestellt: **kein Debugger-Backend ist beteiligt.**

## Warum

Ein echter Quellcode-Debugger für NovaOS-Programme braucht
mindestens:

1. **Debug-Informationen vom Compiler** - `novac.exe` erzeugt aktuell
   KEINE Debug-Symbole (keine Zeilen-zu-Adresse-Tabelle, keine
   Variablen-/Typinformationen im NEF). Ohne das lässt sich eine
   Maschinenadresse nicht auf eine Quellcodezeile zurückführen.
2. **Eine QEMU-Debug-Schnittstelle** (GDB-Stub o. Ä.) tatsächlich
   angebunden und getestet - in dieser Session nicht begonnen.
3. **Eine laufende NovaOS-Instanz mit dem zu debuggenden Programm** -
   setzt die (ebenfalls fehlende) QEMU-/NovaFS-Transfer-Integration
   voraus, siehe `IMPLEMENTATION_STATUS.md`.

Der Auftrag verlangt ausdrücklich "Implementiere keine simulierten
Debugger-Ergebnisse" - ein Debugger-UI ohne echtes Backend zu bauen
würde genau das erzeugen (Breakpoint-Symbole, die nichts bewirken;
ein Call-Stack-Fenster, das nie echte Daten zeigt). Deshalb wurde hier
nichts angefasst, solange die Voraussetzungen (1)-(3) fehlen.

## Nächster realistischer Schritt

Punkt 1 (Debug-Symbole im Compiler) ist die Voraussetzung für alles
Weitere und wäre der sinnvolle erste Teilschritt - selbst dann bleibt
ein vollständiger Quellcode-Debugger ein mehrwöchiges Vorhaben.
