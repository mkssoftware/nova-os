# ADR-6008: Block- und dateibasierte Backup Engine

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage & Virtual File System
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Rescue und NovaOS
- **Ersetzt:** Keine

---

## 1. Kontext

Nova Rescue soll einzelne Dateien, Ordner, Partitionen und ganze Datenträger sichern können.

## 2. Problemstellung

Ein einzelner Kopiermechanismus deckt weder Dateimetadaten noch konsistente Blockimages vollständig ab.

## 3. Betrachtete Alternativen

- Nur Dateikopien
- Nur Rohimages
- Getrennte Datei- und Block-Engines mit gemeinsamer Task-API
- Externe Backup-Tools

## 4. Entscheidung

Die Backup Engine besitzt einen dateibasierten und einen blockbasierten Pfad. Beide verwenden gemeinsame Tasks, Progress, Verification, Hashing und Zielabstraktionen.

## 5. Begründung

- Passender Modus je Rettungsszenario
- Metadaten können bei Dateibackups erhalten werden
- Beschädigte Dateisysteme können blockweise gesichert werden

## 6. Positive Konsequenzen

- Hohe Flexibilität
- Gemeinsame Fortschritts- und Fehlerlogik
- Grundlage für inkrementelle Backups

## 7. Negative Konsequenzen und Risiken

- Mehr Implementierungsaufwand
- Konsistenz und Snapshot-Themen müssen getrennt behandelt werden

## 8. Auswirkungen auf andere Module

Betrifft Backup Center, Task Manager, NVFS, Storage und Crypto/Compression.

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

- ADR-0005
- ADR-6001

## 12. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
