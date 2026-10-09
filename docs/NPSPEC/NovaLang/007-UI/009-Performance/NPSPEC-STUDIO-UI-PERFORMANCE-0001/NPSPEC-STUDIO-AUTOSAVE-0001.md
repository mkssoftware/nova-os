# NPSPEC-STUDIO-AUTOSAVE-0001

**Status:** Angenommen

## Zweck
Automatische Sicherung ungespeicherter Arbeit.

## Festlegungen
- Geänderte Dokumente, UI-Layouts und Logic Graphs müssen automatisch als wiederherstellbare Stände gesichert werden.
- Sicherungen erfolgen atomar und dürfen das reguläre Speichern nicht ersetzen oder überschreiben.
- Wiederherstellungsstände müssen Projekt, Dokumentidentität, Versionsstand und Zeitpunkt zuordnen können.
- Sensible Inhalte bleiben innerhalb derselben Sicherheits- und Berechtigungsgrenzen wie das Original.

## Ergebnis
Ungespeicherte Änderungen überstehen typische Abstürze.
