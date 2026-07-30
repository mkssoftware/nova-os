# ADR-8003: Plugin-API

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Nova SDK
- **Erstellt:** 2026-07-11

## 1. Kontext

Dieses ADR definiert die Architekturentscheidung für **Plugin-API** innerhalb des Nova SDK.

## 2. Problemstellung

Das SDK muss Anwendungen und Plugins eine stabile, dokumentierte und langfristig kompatible Schnittstelle bieten, ohne interne Kernelimplementierungen offenzulegen.

## 3. Entscheidung

Das SDK besteht ausschließlich aus versionierten öffentlichen APIs. Direkte Zugriffe auf Kernel- oder interne Bibliotheken sind nicht Bestandteil des SDK.

## 4. Begründung

- Klare Trennung zwischen internem Kernelcode und öffentlicher API
- Langfristige Binärkompatibilität
- Einfachere Wartung und Dokumentation
- Unterstützung externer Entwickler

## 5. Konsequenzen

### Positiv
- Stabile Entwicklungsplattform
- Geringe Kopplung
- Vorhersagbare Releases

### Risiken
- Änderungen an öffentlichen APIs benötigen Versionsmanagement
- Interne Optimierungen dürfen die SDK-Verträge nicht verletzen

## 6. Auswirkungen

Dieses ADR betrifft alle öffentlichen SDK-Schnittstellen und Entwicklerwerkzeuge.

## 7. Referenzen

- RFC-0001
- ADR-0015
- ADR-0019
- ADR-0025

## 8. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
