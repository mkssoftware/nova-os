# ADR-6003: exFAT für große Austauschmedien

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage & Virtual File System
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Rescue und NovaOS
- **Ersetzt:** Keine

---

## 1. Kontext

Backups und Images können deutlich größer als 4 GiB werden. FAT32 ist dafür ungeeignet.

## 2. Problemstellung

Nova Rescue benötigt ein weit verbreitetes Dateisystem für große Dateien auf USB- und externen Datenträgern.

## 3. Betrachtete Alternativen

- Nur FAT32
- exFAT
- NTFS als Austauschformat
- Eigenes Nova-Backup-Dateisystem

## 4. Entscheidung

exFAT wird nach FAT32 als lesend und schreibend unterstütztes Austauschdateisystem implementiert.

## 5. Begründung

- Große Dateien möglich
- Gute Verbreitung auf externen Medien
- Einfacher als NTFS
- Geeignet für Backup-Container

## 6. Positive Konsequenzen

- Backups über 4 GiB
- Bessere Interoperabilität
- Praktisch für Rettungsmedien

## 7. Negative Konsequenzen und Risiken

- Keine Journaling-Garantien
- Implementierung benötigt korrekte Allocation-Bitmap- und Upcase-Table-Behandlung

## 8. Auswirkungen auf andere Module

Betrifft NVFS, Backup Center, Explorer und Wechselmedien.

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
- ADR-6002

## 12. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
