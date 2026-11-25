# Nova Studio – Erweiterungs-API (Plugins)

## Status: NICHT IMPLEMENTIERT

Keine Plugin-Schnittstelle, keine versionierte Plugin-API, kein
kontrollierter Plugin-Ladevorgang.

## Warum

Nova Studio ist ein natives Win32/C++-Programm ohne Skript- oder
Erweiterungslaufzeit (kein eingebettetes .NET, keine DLL-Plugin-
Architektur, kein Skriptinterpreter). Eine Plugin-API hinzuzufügen,
BEVOR die Kernfunktionalität (Build-Pipeline, Projektsystem) stabil
ist, hätte das Risiko einer API, die bei jeder künftigen Kernänderung
sofort wieder bricht - das widerspricht dem Auftrag selbst ("Die
Kernfunktionen der IDE müssen ohne Plugins funktionieren" impliziert:
Kernfunktionen zuerst).

## Was ein künftiges Minimalkonzept technisch bräuchte

- Ein stabiler C-ABI-Exportpunkt in `nova-studio.exe` (z. B.
  `LoadLibraryW`+`GetProcAddress` auf eine DLL mit definierten
  Einstiegsfunktionen) - architektonisch machbar in Win32/C++, aber
  nicht begonnen.
- Eine versionierte Struct-basierte API (kein COM, kein .NET nötig für
  ein natives C++-Programm) für: Ribbon-Buttons registrieren,
  Dokumentereignisse abonnieren, Build-Schritte einhängen.

Nicht begonnen, da alle bisherigen Kernfunktionen (Build-Pipeline,
Projektsystem, Probleme-Panel) selbst noch im Fluss sind - eine API
darüber zu stabilisieren wäre verfrüht.
