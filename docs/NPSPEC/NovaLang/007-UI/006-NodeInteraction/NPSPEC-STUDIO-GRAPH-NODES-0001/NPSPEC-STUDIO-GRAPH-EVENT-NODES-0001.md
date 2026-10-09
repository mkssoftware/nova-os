# NPSPEC-STUDIO-GRAPH-EVENT-NODES-0001

**Status:** Angenommen

## Zweck
Ereignisse starten einen kontrollierten Ausführungspfad.

## Festlegungen
- Ereignisknoten besitzen einen typisierten Ereignisausgang und optional Datenpins für Ereignisparameter.
- Ereignisquellen werden explizit an UI-, Timer- oder Capability-Ereignisse gebunden; unbekannte Quellen werden nicht ausgeführt.
- Mehrfach ausgelöste Ereignisse werden gemäß konfigurierter Reihenfolge und Parallelitätsregel behandelt.
- Ereignisabonnements werden beim Deaktivieren des Graphen freigegeben.

## Ergebnis
Ereignisse sind nachvollziehbare, typisierte Einstiegspunkte.
