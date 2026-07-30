# ADR-6009: Verifizierte Restore Engine mit Schutzmechanismen

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage & Virtual File System
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Rescue und NovaOS
- **Ersetzt:** Keine

---

## 1. Kontext

Wiederherstellungen überschreiben Daten und gehören zu den gefährlichsten Funktionen von Nova Rescue.

## 2. Problemstellung

Falsche Quell-/Zielwahl, beschädigte Backups oder Abbrüche können Systeme unbrauchbar machen.

## 3. Betrachtete Alternativen

- Direktes Zurückschreiben
- Transaktionsähnliche Restore-Schritte
- Nur manuelle Expertenwerkzeuge
- Externe Restore-Tools

## 4. Entscheidung

Restore arbeitet in Phasen: Analyse, Zielvalidierung, Preflight, optionale Metadatensicherung, Schreibphase, Verifikation und Abschlussbericht.

## 5. Begründung

- Gefahren werden vor Schreibbeginn erkannt
- Backups werden vor Nutzung geprüft
- Abbruch- und Recovery-Punkte sind definierbar

## 6. Positive Konsequenzen

- Höhere Datensicherheit
- Nachvollziehbare Abläufe
- Geeignet für geführte Assistenten

## 7. Negative Konsequenzen und Risiken

- Längere Laufzeit
- Zusätzlicher temporärer Speicher kann nötig sein

## 8. Auswirkungen auf andere Module

Betrifft Restore Center, Backup Format, Disk Manager, UI und Logging.

## 9. Sicherheits- und Integritätsregeln

- Schreibende Operationen müssen Quelle, Ziel und Umfang eindeutig validieren.
- Rescue-Mounts erfolgen standardmäßig read-only, sofern der Benutzer nicht ausdrücklich Schreibzugriff aktiviert.
- Kritische Metadaten werden vor Änderungen gesichert, soweit technisch möglich.
- Teiloperationen, Abbrüche und Stromausfälle müssen in der jeweiligen Implementierung berücksichtigt werden.
- Dateisystem- und Partitionsparser behandeln alle Datenträgerdaten als nicht vertrauenswürdig.
- Größen, Offsets, Überläufe, Prüfsummen und Feature-Flags werden vor Verwendung geprüft.
- Gefährliche Operationen erzeugen einen Audit-Eintrag und benötigen eine eindeutige Bestätigung.

## 10. Testanforderungen

- Unit-Tests mit gültigen und absichtlich beschädigten Images.
- QEMU-Integrationstests mit reproduzierbaren Datenträgerabbildern.
- Abbruchtests während Schreiboperationen.
- Grenzwerttests für Größen, Cluster, Sektoren und Offsets.
- Read-only-Treiber dürfen unter keinen Umständen Schreibbefehle auslösen.
- Wiederherstellungs- und Löschfunktionen werden nur auf Testmedien freigegeben.

## 11. Referenzen

- ADR-6008
- ADR-0023

## 12. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
