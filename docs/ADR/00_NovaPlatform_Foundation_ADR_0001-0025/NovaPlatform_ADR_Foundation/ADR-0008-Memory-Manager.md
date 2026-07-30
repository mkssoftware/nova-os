# ADR-0008: Geschichtete Speicherverwaltung

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Nova benötigt physische Speicherverwaltung, virtuelle Adressräume und verschiedene Allokatoren.

## 2. Problemstellung

Ein einzelner Heap kann Anforderungen von Kernel, Treibern, DMA, kleinen Objekten und großen Puffern nicht optimal abdecken.

## 3. Betrachtete Alternativen

- Einfacher Bump Allocator
- Nur malloc/free-ähnlicher Heap
- Geschichtete Memory-Architektur
- Externe Runtime

## 4. Entscheidung

Nova trennt Physical Memory Manager, Virtual Memory Manager, Kernel Heap, Pools und Slab-Allocator. Die Foundation stellt nur stabile, abstrakte APIs bereit.

## 5. Begründung

- Spezialisierte Anforderungen bleiben getrennt
- Öffentliche API kann stabil bleiben
- Debug-Tracking möglich
- Spätere Erweiterung auf NUMA und Shared Memory

## 6. Positive Konsequenzen

- Skalierbar
- Bessere Diagnose
- Geeignet für Treiber und Kernelobjekte

## 7. Negative Konsequenzen und Risiken

- Höhere Implementierungskomplexität
- Initialisierungsreihenfolge ist kritisch

## 8. Auswirkungen auf andere Module

Betrifft Kernel, HAL, Treiber, Object System und alle dynamischen Datenstrukturen.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
