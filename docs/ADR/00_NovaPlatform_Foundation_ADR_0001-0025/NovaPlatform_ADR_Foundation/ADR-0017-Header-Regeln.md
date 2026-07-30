# ADR-0017: Minimale öffentliche Header-Abhängigkeiten

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Headerketten können Compilezeiten erhöhen und interne Implementierungen nach außen leaken.

## 2. Problemstellung

Unnötige Includes erzeugen starke Kopplung und zyklische Abhängigkeiten.

## 3. Betrachtete Alternativen

- Beliebige Includes
- Ein globaler Mega-Header
- Forward Declarations und minimale Includes
- Automatische Modulimports

## 4. Entscheidung

Öffentliche Header binden nur zwingend benötigte Header ein. Opaque Types und Forward Declarations werden bevorzugt.

## 5. Begründung

- Geringere Kopplung
- Schnellere Builds
- Stabilere APIs
- Weniger Zyklen

## 6. Positive Konsequenzen

- Saubere Modulgrenzen
- Bessere Wartbarkeit

## 7. Negative Konsequenzen und Risiken

- Mehr interne Header und explizite Includes in .c-Dateien

## 8. Auswirkungen auf andere Module

Betrifft alle C-Module.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
