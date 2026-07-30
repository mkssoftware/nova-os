# ADR-6004: NTFS zunächst read-only

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage & Virtual File System
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Rescue und NovaOS
- **Ersetzt:** Keine

---

## 1. Kontext

Viele zu rettende Windows-Systeme verwenden NTFS. Schreibzugriffe auf beschädigte NTFS-Volumes bergen jedoch hohe Risiken.

## 2. Problemstellung

Nova Rescue muss Daten von NTFS lesen können, ohne durch eine frühe unvollständige Schreibimplementierung Daten zu gefährden.

## 3. Betrachtete Alternativen

- NTFS vollständig lesen/schreiben
- NTFS nur lesen
- Externe NTFS-Bibliothek
- Keine NTFS-Unterstützung

## 4. Entscheidung

Die erste NTFS-Unterstützung ist strikt read-only. Schreibzugriffe werden erst nach separatem RFC, Transaktionstests und Journalbehandlung freigegeben.

## 5. Begründung

- Datenrettung ist früh möglich
- Minimiert Korruptionsrisiko
- Komplexe Metadaten und LogFile-Verarbeitung können schrittweise entwickelt werden

## 6. Positive Konsequenzen

- Sicherer Zugriff auf Windows-Daten
- Geeignet für Explorer und Backup
- Klarer Sicherheitsstatus

## 7. Negative Konsequenzen und Risiken

- Keine direkte Reparatur oder Wiederherstellung auf NTFS in der ersten Version
- Komplexe Parser bleiben erforderlich

## 8. Auswirkungen auf andere Module

Betrifft NVFS, Explorer, Backup, Restore und UI-Warnungen.

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
- ADR-0025

## 12. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
