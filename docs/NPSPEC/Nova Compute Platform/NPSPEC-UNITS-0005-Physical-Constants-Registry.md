# NPSPEC-UNITS-0005
# Physical Constants Registry

| Feld | Wert |
|---|---|
| Dokument | NPSPEC-UNITS-0005 |
| Titel | Physical Constants Registry |
| Version | 1.0.0 |
| Status | Angenommen |
| Kategorie | System Specification |
| Bereich | Nova Compute Platform |
| Abhängigkeiten | NPSPEC-COMPUTE-0001, NPSPEC-UNITS-0001, NPSPEC-UNITS-0002, NPSPEC-UNITS-0003, NPSPEC-UNITS-0004 |
| Letzte Änderung | 2026-07-13 |

---

# 1. Zweck

Diese Spezifikation definiert die zentrale Registry für physikalische Konstanten innerhalb der Nova Compute Platform.

Alle Konstanten werden als versionierte, typisierte und nachvollziehbare Objekte verwaltet.

---

# 2. Ziele

- Einheitliche Quelle für Naturkonstanten
- Versionierung (z. B. CODATA)
- Vollständige Rückführbarkeit
- Typ- und Dimensionssicherheit
- Integration in Quantity- und Measurement-System
- Nutzung durch Compiler, Runtime und Compute-Engines

---

# 3. Grundprinzipien

- Jede Konstante besitzt eine eindeutige UUID.
- Jede Konstante besitzt eine Quantity.
- Jede Konstante besitzt eine Dimension.
- Jede Konstante besitzt eine Unsicherheit.
- Jede Konstante verweist auf ihre Quelle.
- Jede Änderung erfolgt versionsgesteuert.

---

# 4. Constant Object Model

```c
typedef struct nova_physical_constant
{
    nova_object_header_t object;

    nova_constant_uuid_t uuid;

    const char* canonical_name;
    const char* symbol;

    nova_quantity_t value;
    nova_measurement_t uncertainty;

    nova_version_t dataset_version;

    const char* source;
    const char* reference;

    nova_constant_flags_t flags;
} nova_physical_constant_t;
```

---

# 5. Registry

Die Registry enthält:

- SI-Basiskonstanten
- Elektromagnetische Konstanten
- Atomphysikalische Konstanten
- Thermodynamische Konstanten
- Astronomische Konstanten
- Mathematische Konstanten
- Benutzerdefinierte Konstanten

---

# 6. Standardkonstanten

Mindestens bereitzustellen:

- Lichtgeschwindigkeit c
- Planck-Konstante h
- Reduzierte Planck-Konstante ħ
- Elementarladung e
- Avogadro-Konstante
- Boltzmann-Konstante
- Universelle Gaskonstante
- Gravitationskonstante G
- Elektrische Feldkonstante ε₀
- Magnetische Feldkonstante μ₀
- Stefan-Boltzmann-Konstante
- Feinstrukturkonstante α

---

# 7. API

```c
nova_result_t nova_constant_find(
    const char* name,
    nova_constant_handle_t* out_handle);

nova_result_t nova_constant_get(
    nova_constant_handle_t handle,
    nova_physical_constant_t* out_constant);

nova_result_t nova_constant_register(
    const nova_physical_constant_t* constant);
```

---

# 8. Versionierung

Konstantensätze werden separat versioniert.

Beispiel:

```text
CODATA-2022
CODATA-2026
NOVA-LOCAL
```

Anwendungen können gezielt eine Version auswählen.

---

# 9. Validierung

Vor Aufnahme in die Registry werden geprüft:

- UUID
- Dimension
- Einheit
- Unsicherheit
- Quelle
- Versionskonsistenz
- Signatur (optional)

---

# 10. Integration

Die Registry wird verwendet von:

- Physics Engine
- Chemistry Engine
- Matrix Engine
- Tensor Engine
- FFT Engine
- Signal Engine
- AI Runtime
- Robotics Runtime
- Measurement Runtime

Alle Komponenten greifen auf dieselbe Konstantenquelle zu.

---

# 11. Sicherheit

Systemkonstanten sind schreibgeschützt.

Benutzerdefinierte Konstanten dürfen ausschließlich im eigenen Namespace registriert werden.

---

# 12. Serialisierung

Konstanten unterstützen:

- Nova Binary
- JSON
- CBOR
- MessagePack

Alle Formate müssen Wert, Einheit, Unsicherheit, Version und Quelle enthalten.

---

# 13. Tests

Pflichttests:

- Registry Lookup
- Versionierung
- Dimensionsprüfung
- Serialisierung
- Reflection
- API-Kompatibilität

---

# 14. Roadmap

Geplante Erweiterungen:

- Historische Konstantensätze
- Domänenspezifische Konstantenpakete
- Digitale Signaturen
- Compiler-Inlining von Konstanten
- GPU-optimierte Konstantencaches

---

# 15. Zusammenfassung

Die Physical Constants Registry stellt eine zentrale, konsistente und wissenschaftlich nachvollziehbare Quelle für Naturkonstanten bereit und bildet die Grundlage aller physikalischen Berechnungen innerhalb der Nova Compute Platform.
