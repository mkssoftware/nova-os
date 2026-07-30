# ADR-0025: Versionierte und eingeschränkte Plugin-APIs

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Nova soll erweiterbar sein, ohne dass Plugins interne Kernelstrukturen verwenden.

## 2. Problemstellung

Unkontrollierte Plugins gefährden Stabilität und ABI-Kompatibilität.

## 3. Betrachtete Alternativen

- Keine Plugins
- Plugins mit direktem Kernelzugriff
- SDK-basierte Plugins in User Space
- Signierte Kernelmodule

## 4. Entscheidung

Anwendungsplugins verwenden ausschließlich versionierte Nova-SDK- und Service-APIs. Kernelmodule werden separat spezifiziert und standardmäßig nicht mit normalen Plugins gleichgesetzt.

## 5. Begründung

- Klare Sicherheitsgrenze
- Stabile Erweiterbarkeit
- Plugins können getestet und isoliert werden

## 6. Positive Konsequenzen

- Weniger Kernelrisiko
- Saubere API-Grenzen
- Drittanbieter-Erweiterungen möglich

## 7. Negative Konsequenzen und Risiken

- Einige Low-Level-Erweiterungen benötigen separate Modulmechanismen
- API-Design muss langfristig gepflegt werden

## 8. Auswirkungen auf andere Module

Betrifft SDK, Service Bus, Loader, Signierung und Anwendungen.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
