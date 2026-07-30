# ADR-0002: ObjectIDs mit Slot- und Generationsteil

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Foundation Architecture
- **Erstellt:** 2026-07-11
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine

---

## 1. Kontext

Viele Nova-Subsysteme müssen Objekte referenzieren: Events, Fenster, Services, Tasks, Plugins, Dateien und Anwendungen. Direkte Zeiger sind nur innerhalb eines Adressraums gültig und können nach Zerstörung eines Objekts veraltet sein.

## 2. Problemstellung

Es wird ein stabiles, validierbares Handle-System benötigt, das stale handles erkennt und später auch für IPC, Debugger und Inspector geeignet ist.

## 3. Betrachtete Alternativen

- Direkte Zeiger
- Fortlaufende 32-Bit-IDs
- UUIDs für jedes Objekt
- 64-Bit-Handles aus Slot und Generation

## 4. Entscheidung

NovaObjectId wird als 64-Bit-Handle definiert. Die unteren 32 Bit kodieren den Slot, die oberen 32 Bit die Generation. Der Wert 0 bleibt ungültig.

## 5. Begründung

- Slotzugriff ist effizient.
- Generation Counter erkennen wiederverwendete Slots und veraltete Handles.
- Handles lassen sich in Logs, Events, IPC und Debugdaten transportieren.
- Die interne Objekttabelle kann später ausgetauscht werden, ohne die öffentliche ID-Struktur zu ändern.

## 6. Positive Konsequenzen

- Erkennung ungültiger und veralteter Objektverweise
- Keine Pointer in Events oder persistenten Strukturen notwendig
- Gute Grundlage für Inspector, Service Bus und IPC

## 7. Negative Konsequenzen und Risiken

- Object Manager wird verpflichtend
- Jeder Zugriff benötigt Validierung
- Generation Overflow muss definiert behandelt werden

## 8. Auswirkungen auf andere Module

Betrifft Object Manager, Event System, Service Bus, IPC, Plugins, Window Manager und Debugging.

## 9. Umsetzungsregeln

- Die Entscheidung ist für neue Implementierungen verbindlich.
- Abweichungen benötigen ein neues ADR oder eine dokumentierte Ersetzung dieses ADRs.
- Öffentliche APIs und persistente Formate müssen die festgelegten ABI- und Versionsregeln beachten.
- Tests und Dokumentation sind zusammen mit der Implementierung zu aktualisieren.

## 10. Referenzen

- RFC-0001.1

## 11. Statushistorie

- 2026-07-11: Version 1.0 angenommen.
