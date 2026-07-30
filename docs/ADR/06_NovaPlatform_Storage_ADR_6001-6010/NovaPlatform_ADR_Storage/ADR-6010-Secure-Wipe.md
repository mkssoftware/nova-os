# ADR-6010: Mediengerechtes Secure Wipe

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage & Virtual File System
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Rescue und NovaOS
- **Ersetzt:** Keine

---

## 1. Kontext

Nova Rescue soll Dateien, Partitionen und Datenträger sicher löschen können. HDD, SSD und NVMe benötigen unterschiedliche Verfahren.

## 2. Problemstellung

Mehrfaches Überschreiben ist bei SSDs wegen Wear Leveling nicht zuverlässig; falsche Verfahren können eine falsche Sicherheitsgarantie vermitteln.

## 3. Betrachtete Alternativen

- Immer mehrfach überschreiben
- Nur Hersteller-Secure-Erase
- Medientypabhängige Verfahren
- Keine Secure-Wipe-Funktion

## 4. Entscheidung

Nova erkennt Medientyp und Fähigkeiten. HDDs verwenden kontrolliertes Überschreiben; SATA-SSDs ATA Secure Erase/Sanitize; NVMe-Geräte Format/Sanitize; verschlüsselte Datenträger können Crypto-Erase verwenden. Die UI zeigt die tatsächliche Garantie und Einschränkungen.

## 5. Begründung

- Technisch korrekt je Medium
- Keine irreführenden Mehrfachpass-Versprechen
- Nutzt standardisierte Controllerbefehle

## 6. Positive Konsequenzen

- Höhere Sicherheit
- Korrekte Behandlung moderner Medien
- Auditierbare Löschberichte

## 7. Negative Konsequenzen und Risiken

- Hersteller- und Firmwarefehler möglich
- Befehle sind irreversibel
- Stromverlust muss berücksichtigt werden

## 8. Auswirkungen auf andere Module

Betrifft Secure Wipe App, Storage HAL, Security, Audit Logging und Bestätigungsdialoge.

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

- ADR-0007
- ADR-0023
- ADR-6006
- ADR-6007

## 12. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
