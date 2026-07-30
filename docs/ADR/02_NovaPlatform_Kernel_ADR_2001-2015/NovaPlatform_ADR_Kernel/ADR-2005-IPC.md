# ADR-2005: Interprozesskommunikation

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Kernel Architecture
- **Erstellt:** 2026-07-11

## 1. Kontext

Dieses Dokument definiert die Architekturentscheidung für **Interprozesskommunikation** innerhalb des Nova-Kernels.

## 2. Problemstellung

Der Kernel benötigt für dieses Teilgebiet eine klar definierte, langfristig stabile Architektur, damit Nova Rescue, NovaOS und zukünftige Plattformen dieselbe Kernimplementierung verwenden können.

## 3. Entscheidung

Es wird eine modulare, versionierte Kernel-Implementierung mit klaren öffentlichen APIs und internen Implementierungsgrenzen verwendet. Öffentliche Datenstrukturen besitzen StructSize- und ABI-Versionen. Interne Implementierungen bleiben austauschbar.

## 4. Begründung

- Klare Trennung zwischen API und Implementierung.
- Gute Testbarkeit.
- Langfristige ABI-Stabilität.
- Wiederverwendung in Nova Rescue und NovaOS.
- Unterstützung zukünftiger Erweiterungen.

## 5. Konsequenzen

### Positiv

- Hohe Wartbarkeit.
- Austauschbare Implementierungen.
- Einheitliche Architektur.

### Risiken

- Höherer Dokumentationsaufwand.
- Konsequente Versionspflege erforderlich.

## 6. Auswirkungen

Diese Entscheidung betrifft alle Kernelmodule, die mit **Interprozesskommunikation** interagieren.

## 7. Referenzen

- RFC-0001
- ADR-0015
- ADR-0019

## 8. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
