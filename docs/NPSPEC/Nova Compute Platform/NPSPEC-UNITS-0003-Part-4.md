# NPSPEC-UNITS-0003
# Unit Registry and Conversion Engine

| Feld | Wert |
|---|---|
| Dokument | NPSPEC-UNITS-0003 |
| Teil | 4 von 6 |
| Kapitel | 31–40 |
| Version | 1.0.0 |
| Status | Angenommen |
| Letzte Änderung | 2026-07-13 |

---

# 31. Öffentliche C-API

Die Unit Engine stellt eine stabile C-API bereit.

## Ziele

- ABI-Stabilität
- Compilerunabhängigkeit
- Sprachübergreifende Nutzung
- Freestanding-Unterstützung

Beispiel:

```c
nova_result_t nova_unit_find(
    const char* name,
    nova_unit_handle_t* out_handle
);
```

Alle öffentlichen Funktionen beginnen mit dem Präfix `nova_`.

---

# 32. C++-API

Die C++-API baut vollständig auf der C-API auf.

Beispiel:

```cpp
nova::Unit meter = registry.find("meter");
auto kilometer = registry.find("kilometer");

auto value = meter.convert(25.0, kilometer);
```

Ziele:

- RAII
- Exceptions optional
- constexpr-Unterstützung
- starke Typsicherheit

---

# 33. Reflection API

Alle Registry-Objekte müssen zur Laufzeit introspektierbar sein.

Abfragbar sind u. a.:

- UUID
- Name
- Symbol
- Namespace
- Version
- Dimension
- Flags
- Quelle
- Dokumentation

Beispiel:

```c
nova_unit_descriptor_t descriptor;
nova_unit_get_descriptor(handle, &descriptor);
```

---

# 34. ABI

Die öffentliche ABI muss über Major-Versionen stabil bleiben.

Regeln:

- keine Änderung bestehender Strukturen ohne Versionswechsel
- neue Felder nur kompatibel ergänzen
- reservierte Felder für zukünftige Erweiterungen vorsehen

Alle ABI-relevanten Header erhalten eine eigene Versionskennung.

---

# 35. IPC

Die Registry ist systemweit nutzbar.

Anwendungen kommunizieren mit dem Registry-Service über Nova IPC.

Operationen:

- Lookup
- Reflection
- Registrierung
- Validierung
- Konvertierung

Handles dürfen prozessübergreifend übertragen werden.

---

# 36. Serialisierung

Registry-Objekte besitzen ein standardisiertes Austauschformat.

Pflichtfelder:

- Schema-Version
- UUID
- Namespace
- Name
- Symbol
- Dimension
- Konvertierungsmodell
- Metadaten

Unterstützte Formate:

- Binärformat
- JSON
- CBOR
- MessagePack

---

# 37. Persistenz

Die Registry wird dauerhaft gespeichert.

Eigenschaften:

- atomare Updates
- Prüfsummen
- digitale Signaturen
- Rollback
- Versionshistorie

Die Boot-Registry bleibt unveränderlich.

---

# 38. Plugin-Schnittstellen

Erweiterungen dürfen neue Einheiten und Konvertierer registrieren.

Voraussetzungen:

- gültiger Namespace
- signiertes Plugin (optional je nach Sicherheitsrichtlinie)
- erfolgreiche Validierung

Plugins dürfen keine offiziellen Nova-Systemeinträge überschreiben.

---

# 39. Versionierung

Jeder Registry-Eintrag besitzt:

- Major
- Minor
- Patch

Kompatibilitätsregeln:

- Major: inkompatible Änderungen
- Minor: Erweiterungen
- Patch: Fehlerkorrekturen

Veraltete Einträge werden als "deprecated" markiert, jedoch nicht sofort entfernt.

---

# 40. Kompatibilitätsregeln

Die Engine muss langfristige Stabilität gewährleisten.

Anforderungen:

- reproduzierbare Konvertierungen
- deterministische Ergebnisse
- stabile UUIDs
- stabile öffentliche APIs
- dokumentierte Migrationspfade

Neue Funktionen dürfen bestehende Anwendungen nicht beeinträchtigen.

---

# Ausblick auf Teil 5

Kapitel 41–50 behandeln:

- Sicherheitsmodell
- Vertrauensstufen
- Signaturen
- Zugriffsrechte
- Sandboxing
- Performance-Monitoring
- Diagnose
- Logging
- Telemetrie (optional)
- Testframework
