# NPSPEC-UNITS-0003
# Unit Registry and Conversion Engine

| Feld | Wert |
|---|---|
| Dokument | NPSPEC-UNITS-0003 |
| Teil | 3 von 6 |
| Kapitel | 21–30 |
| Version | 1.0.0 |
| Status | Angenommen |
| Letzte Änderung | 2026-07-13 |

---

# 21. Conversion Graph

Alle zulässigen Umrechnungen werden als gerichteter Graph modelliert.

```text
Meter ──► Kilometer
  │
  ├──► Zentimeter
  ├──► Millimeter
  └──► Zoll
```

Jeder Knoten repräsentiert eine registrierte Einheit.
Jede Kante beschreibt eine gültige Konvertierungsregel.

Eigenschaften:

- azyklisch innerhalb eines Referenzsystems
- versionierbar
- signierbar
- cachefähig

---

# 22. Linear Converter

Lineare Konvertierungen folgen:

```text
target = source × scale
```

Beispiele:

- m ↔ km
- Pa ↔ bar
- g ↔ kg

API:

```c
nova_result_t nova_convert_linear(
    const nova_quantity_t* source,
    nova_unit_uuid_t target,
    nova_quantity_t* result
);
```

---

# 23. Affine Converter

Affine Umrechnungen verwenden:

```text
target = source × scale + offset
```

Beispiele:

- Celsius ↔ Kelvin
- Fahrenheit ↔ Kelvin

Offsets müssen exakt gespeichert werden.

---

# 24. Logarithmic Converter

Logarithmische Einheiten werden über spezielle Konverter behandelt.

Unterstützte Klassen:

- dB
- dBm
- dBW
- pH
- Magnitude

Eine lineare Umrechnung ist hierbei unzulässig.

---

# 25. Contextual Converter

Manche Umrechnungen benötigen Kontext.

Beispiele:

- Währungen
- historische Maße
- Kalendereinheiten

Der Kontext wird als separates Objekt übergeben.

```c
nova_conversion_context_t context;
```

Ohne gültigen Kontext muss die Umrechnung fehlschlagen.

---

# 26. Conversion Cache

Häufig verwendete Konvertierungen werden zwischengespeichert.

Cache-Schlüssel:

```text
Source UUID
Target UUID
Conversion Type
Version
```

Der Cache muss nach Änderungen an der Registry automatisch invalidiert werden.

---

# 27. Runtime Optimierung

Die Runtime soll:

- direkte Tabellenzugriffe bevorzugen
- Multiplikationen statt Divisionen verwenden
- konstante Faktoren vorab berechnen
- SIMD-freundliche Pfade bereitstellen
- unnötige Speicherallokationen vermeiden

Konstante Umrechnungen dürfen bereits beim Kompilieren ausgewertet werden.

---

# 28. Thread Safety

Mehrere Threads dürfen gleichzeitig lesen.

Anforderungen:

- Lock-Free Reads
- atomare Handle-Aktualisierung
- konsistente Snapshots
- keine globalen Schreibsperren für Leseoperationen

Schreibzugriffe erfolgen ausschließlich transaktional.

---

# 29. Performance-Modell

Zielwerte:

| Operation | Ziel |
|---|---:|
| Handle Lookup | O(1) |
| UUID Lookup | O(log n) |
| Symbol Lookup | O(log n) |
| Cache Hit | O(1) |
| Lineare Umrechnung | O(1) |

Die Engine soll Millionen einfacher Konvertierungen pro Sekunde ermöglichen.

---

# 30. Fehlerbehandlung

Fehlercodes:

```text
NOVA_ERROR_UNIT_NOT_FOUND
NOVA_ERROR_DIMENSION_MISMATCH
NOVA_ERROR_INVALID_CONVERSION
NOVA_ERROR_CONTEXT_REQUIRED
NOVA_ERROR_CYCLE_DETECTED
NOVA_ERROR_CACHE_INVALID
NOVA_ERROR_REGISTRY_CORRUPT
```

Fehlerobjekt:

```c
typedef struct nova_conversion_error
{
    nova_error_code_t code;
    nova_unit_uuid_t source;
    nova_unit_uuid_t target;
    const char* message;
} nova_conversion_error_t;
```

Jede fehlgeschlagene Konvertierung muss reproduzierbar diagnostizierbar sein.

---

# Ausblick auf Teil 4

Kapitel 31–40 behandeln:

- Öffentliche C-API
- C++ API
- Reflection
- ABI
- IPC
- Serialisierung
- Persistenz
- Plugin-Schnittstellen
- Versionierung
- Kompatibilitätsregeln
