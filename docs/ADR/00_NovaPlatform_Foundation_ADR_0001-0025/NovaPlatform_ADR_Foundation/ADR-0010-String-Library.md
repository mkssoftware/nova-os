# ADR-0010: Eigene sichere String Library

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Die Plattform benötigt ASCII-, UTF-8- und später Unicode-Verarbeitung ohne Host-libc.

## 2. Problemstellung

Klassische C-Stringfunktionen sind fehleranfällig und nicht Unicode-fähig.

## 3. Betrachtete Alternativen

- libc-Funktionen
- Nur nullterminierte ASCII-Strings
- Length-aware String Views und Owned Strings
- C++ std::string

## 4. Entscheidung

Nova führt NStringView für nicht-besitzende Ansichten und NString für verwaltete Strings ein. UTF-8 ist das Standard-Encoding außerhalb spezieller Low-Level-Pfade.

## 5. Begründung

- Längenbegrenzte Operationen
- Weniger Buffer Overflows
- Unicode-fähige Basis
- Explizite Besitzregeln

## 6. Positive Konsequenzen

- Sicherere APIs
- Geeignet für Explorer, VFS und UI
- Hosttests möglich

## 7. Negative Konsequenzen und Risiken

- Zusätzliche Konvertierungsfunktionen
- UTF-8-Indizes sind nicht konstantzeitlich nach Zeichen

## 8. Auswirkungen auf andere Module

Betrifft Foundation, VFS, UI, Assets, Logs und Anwendungen.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
