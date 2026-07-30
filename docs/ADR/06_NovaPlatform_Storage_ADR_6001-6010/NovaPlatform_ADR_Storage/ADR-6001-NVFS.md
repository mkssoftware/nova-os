# ADR-6001: Nova Virtual File System (NVFS)

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage & Virtual File System
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Rescue und NovaOS
- **Ersetzt:** Keine

---

## 1. Kontext

Nova Rescue und NovaOS müssen unterschiedliche Dateisysteme über eine einheitliche API ansprechen. Anwendungen dürfen nicht von FAT32-, NTFS-, ext4- oder ISO9660-spezifischen Details abhängen.

## 2. Problemstellung

Direkte Zugriffe auf konkrete Dateisystemtreiber würden Explorer, Backup und Restore stark koppeln und spätere Erweiterungen erschweren.

## 3. Betrachtete Alternativen

- Direkte Dateisystemtreiber-APIs
- POSIX-ähnliche globale Dateifunktionen
- Ein einheitliches Virtual File System
- Vollständige Übernahme eines externen VFS

## 4. Entscheidung

Nova führt NVFS als gemeinsame Abstraktionsschicht für Dateien, Verzeichnisse, Mounts, Pfade, Metadaten und Berechtigungen ein. Dateisystemtreiber registrieren sich über eine versionierte Driver-API.

## 5. Begründung

- Anwendungen bleiben dateisystemunabhängig
- Mounts und virtuelle Pfade werden zentral verwaltet
- Treiber können lesend oder schreibend unterschiedliche Fähigkeiten deklarieren
- Rescue kann Datenträger standardmäßig read-only einbinden

## 6. Positive Konsequenzen

- Einheitliche API
- Leichtere Erweiterung
- Bessere Sicherheitskontrolle
- Gemeinsame Basis für Explorer, Backup und Restore

## 7. Negative Konsequenzen und Risiken

- Zusätzliche Abstraktionsschicht
- Pfad- und Semantikunterschiede müssen normalisiert werden

## 8. Auswirkungen auf andere Module

Betrifft Explorer, Backup, Restore, Storage Service, Dateisystemtreiber und Sicherheitsmodell.

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

- RFC-0001
- ADR-0006
- ADR-0015

## 12. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
