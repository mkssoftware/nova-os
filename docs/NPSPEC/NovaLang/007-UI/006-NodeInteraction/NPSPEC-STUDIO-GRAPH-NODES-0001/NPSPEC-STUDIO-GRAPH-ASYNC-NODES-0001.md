# NPSPEC-STUDIO-GRAPH-ASYNC-NODES-0001

**Status:** Angenommen

## Zweck
Nichtblockierende Aufgaben und deren Lebenszyklus modellieren.

## Festlegungen
- Start, Await, Join, Timeout und Cancel sind ausdrücklich modellierbare Operationen.
- Jede Aufgabe besitzt eine definierte Elternaufgabe und einen kontrollierten Abbruchpfad.
- Timeouts und Fehler sind sichtbare Resultate.
- Asynchrone Tasks erben nur zuvor autorisierte und explizit delegierte Capability-Zugriffe.

## Ergebnis
Asynchrone Abläufe bleiben strukturiert, abbrechbar und überprüfbar.
