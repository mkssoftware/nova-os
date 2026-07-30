# Mitwirken an Nova OS

## Grundregel

Jede Änderung muss zur Architektur passen.

Vor größeren Änderungen ist ein RFC erforderlich.

## Code-Regeln

- keine magischen Zahlen
- keine undokumentierten öffentlichen Funktionen
- keine sichtbaren englischen Fehlermeldungen
- keine direkten Framebuffer-Zugriffe außerhalb des Framebuffer-Moduls
- keine festen Bildschirmauflösungen in UI-Code
- keine versteckten Abhängigkeiten

## Sprache

Kommentare und sichtbare Meldungen werden auf Deutsch geschrieben.

## Modulstatus

Ein Modul kann folgende Zustände besitzen:

```text
ENTWURF
SPEZIFIZIERT
IN ENTWICKLUNG
TESTBAR
STABIL
VERALTET
ENTFERNT
