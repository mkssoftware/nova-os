# ADR-6007: MBR-Unterstützung als Kompatibilitätsschicht

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Storage & Virtual File System
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Rescue und NovaOS
- **Ersetzt:** Keine

---

## 1. Kontext

Ältere Systeme und manche Wechselmedien verwenden MBR.

## 2. Problemstellung

Nova Rescue soll alte Systeme unterstützen, ohne MBR als zukünftiges Hauptschema zu behandeln.

## 3. Betrachtete Alternativen

- Keine MBR-Unterstützung
- MBR gleichrangig mit GPT
- MBR als Legacy-Kompatibilität

## 4. Entscheidung

MBR und Extended Partitions werden unterstützt, aber als Legacy-Kompatibilität gekennzeichnet. Schreibende Änderungen erzeugen vorher ein sektorweises Backup.

## 5. Begründung

- Unterstützung älterer Hardware
- Kompatibilität mit bestehenden Datenträgern
- Klare Priorisierung von GPT

## 6. Positive Konsequenzen

- Breitere Hardwareunterstützung
- Boot-Reparatur alter Systeme möglich

## 7. Negative Konsequenzen und Risiken

- CHS-Altlasten und Extended-Partition-Ketten
- Weniger Redundanz als GPT

## 8. Auswirkungen auf andere Module

Betrifft Disk Manager, Boot Manager, Rescue und Image Tools.

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

- ADR-1002
- ADR-6006

## 12. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
