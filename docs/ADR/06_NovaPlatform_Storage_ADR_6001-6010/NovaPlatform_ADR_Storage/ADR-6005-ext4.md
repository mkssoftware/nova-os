# ADR-6005: ext4 zunächst read-only

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage & Virtual File System
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Rescue und NovaOS
- **Ersetzt:** Keine

---

## 1. Kontext

Linux-Systeme verwenden häufig ext4. Nova Rescue soll auch Linux-Daten sichern können.

## 2. Problemstellung

ext4 besitzt Extents, Journaling und zahlreiche Feature-Flags, die bei unvollständiger Schreibunterstützung Daten gefährden können.

## 3. Betrachtete Alternativen

- ext4 sofort lesen/schreiben
- ext4 nur lesen
- ext2 zuerst
- Keine Linux-Dateisysteme

## 4. Entscheidung

ext4 wird zunächst read-only implementiert. Nur bekannte und validierte Feature-Kombinationen werden gemountet.

## 5. Begründung

- Datenrettung von Linux-Systemen
- Reduziertes Korruptionsrisiko
- Feature-Flags können sauber geprüft werden

## 6. Positive Konsequenzen

- Breitere Rescue-Fähigkeiten
- Sichere schrittweise Entwicklung

## 7. Negative Konsequenzen und Risiken

- Keine ext4-Reparatur in der ersten Version
- Viele optionale Features müssen erkannt werden

## 8. Auswirkungen auf andere Module

Betrifft NVFS, Explorer, Backup und Mount Manager.

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

- ADR-6001

## 12. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
