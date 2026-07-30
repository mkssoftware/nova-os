# Nova Platform – Storage ADR Index

Diese Sammlung enthält die angenommenen Storage- und Virtual-File-System-ADRs ADR-6001 bis ADR-6010.

## Dokumente

- [ADR-6001: Nova Virtual File System (NVFS)](ADR-6001-NVFS.md)
- [ADR-6002: FAT32 als erstes vollständig unterstütztes Dateisystem](ADR-6002-FAT32.md)
- [ADR-6003: exFAT für große Austauschmedien](ADR-6003-exFAT.md)
- [ADR-6004: NTFS zunächst read-only](ADR-6004-NTFS.md)
- [ADR-6005: ext4 zunächst read-only](ADR-6005-ext4.md)
- [ADR-6006: GPT als primäres Partitionsschema](ADR-6006-GPT.md)
- [ADR-6007: MBR-Unterstützung als Kompatibilitätsschicht](ADR-6007-MBR.md)
- [ADR-6008: Block- und dateibasierte Backup Engine](ADR-6008-Backup-Engine.md)
- [ADR-6009: Verifizierte Restore Engine mit Schutzmechanismen](ADR-6009-Restore-Engine.md)
- [ADR-6010: Mediengerechtes Secure Wipe](ADR-6010-Secure-Wipe.md)

## Grundsatz

Bei Nova Rescue hat Datenintegrität Vorrang vor Geschwindigkeit und Komfort. Dateisysteme werden standardmäßig read-only eingebunden, bis ihre Schreibimplementierung ausdrücklich freigegeben und getestet wurde.
