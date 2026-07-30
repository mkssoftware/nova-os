# ADR-0003: Leichtgewichtiges Objektmodell in C

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Nova benötigt gemeinsame Lebenszyklus- und Identitätsregeln für verwaltete Laufzeitentitäten, ohne C++ oder eine Garbage Collection einzuführen.

## 2. Problemstellung

Fenster, Assets, Anwendungen, Dateien, Datenträger, Tasks und Plugins benötigen unterschiedliche Fähigkeiten, aber gemeinsame Mechanismen für Typ, ID, Zustand und Zerstörung.

## 3. Betrachtete Alternativen

- Keine gemeinsame Objektbasis
- C++-Klassenhierarchie
- Entity-Component-System für alles
- Leichtgewichtige C-Basisstruktur

## 4. Entscheidung

Verwaltete Laufzeitentitäten betten NovaObject als erstes Feld ein. NovaObject enthält Strukturgröße, ABI-Version, ID, Typ, Zustand, Flags, Namen, PrivateData und optionale Lebenszyklus-Callbacks.

## 5. Begründung

- Einheitliche Diagnose und Registrierung
- Kontrollierte Polymorphie ohne C++-Runtime
- Kompatibel mit spezialisierten Strukturen
- Nicht jede Datenstruktur wird unnötig zu einem Objekt

## 6. Positive Konsequenzen

- Einheitlicher Lebenszyklus
- Gemeinsame Debug- und Inspector-Funktionen
- Erweiterbarkeit über ABI-Version und StructSize

## 7. Negative Konsequenzen und Risiken

- Manuelle Casts und Typprüfungen
- Fehlerhafte Einbettung kann zu ABI-Problemen führen
- Nicht für extrem hochfrequente Kleinstobjekte geeignet

## 8. Auswirkungen auf andere Module

Betrifft Foundation, UI, Assets, Storage, Anwendungen, Plugins, Services und Debugging.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001.1

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
