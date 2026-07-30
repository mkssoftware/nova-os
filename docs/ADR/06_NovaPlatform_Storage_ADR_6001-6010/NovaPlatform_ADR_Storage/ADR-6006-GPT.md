# ADR-6006: GPT als primäres Partitionsschema

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage & Virtual File System
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Rescue und NovaOS
- **Ersetzt:** Keine

---

## 1. Kontext

Moderne Systeme und UEFI verwenden überwiegend GPT. Nova Rescue muss GPT lesen, validieren, sichern und später bearbeiten können.

## 2. Problemstellung

Partitionstabellen sind kritisch; fehlerhafte Änderungen können den Zugriff auf alle Daten verhindern.

## 3. Betrachtete Alternativen

- Nur MBR
- GPT und MBR gleichrangig
- GPT als primär, MBR für Kompatibilität
- Externe Partitionierungsbibliothek

## 4. Entscheidung

GPT ist das primäre Partitionsschema. Nova validiert Header, CRCs, Primary- und Backup-GPT und erstellt vor Änderungen eine Sicherung der Metadaten.

## 5. Begründung

- Moderne Standardarchitektur
- Redundante Header ermöglichen Prüfung
- Große Datenträger und viele Partitionen

## 6. Positive Konsequenzen

- Robuste Analyse
- Sicherungs- und Wiederherstellungsfunktionen
- UEFI-Kompatibilität

## 7. Negative Konsequenzen und Risiken

- CRC- und Konsistenzlogik erforderlich
- Teilweise beschädigte Tabellen benötigen vorsichtige Recovery-Strategien

## 8. Auswirkungen auf andere Module

Betrifft Disk Manager, Boot Repair, Backup, Restore und Secure Wipe.

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

- ADR-1003
- ADR-6001

## 12. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
