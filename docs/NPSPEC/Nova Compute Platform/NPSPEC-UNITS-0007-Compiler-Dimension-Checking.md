# NPSPEC-UNITS-0007
# Compiler Dimension Checking

| Feld | Wert |
|---|---|
| Dokument | NPSPEC-UNITS-0007 |
| Titel | Compiler Dimension Checking |
| Version | 1.0.0 |
| Status | Angenommen |
| Kategorie | System Specification |
| Bereich | Nova Compute Platform |
| Abhängigkeiten | NPSPEC-UNITS-0001 bis NPSPEC-UNITS-0006 |
| Letzte Änderung | 2026-07-13 |

---

# 1. Zweck

Diese Spezifikation definiert die statische Dimensionsprüfung des Nova-Compilers.

Ziel ist es, Dimensions-, Einheiten- und Quantity-Fehler bereits während der Übersetzung zu erkennen, bevor ein Programm ausgeführt wird.

---

# 2. Ziele

- Compile-Time-Prüfung physikalischer Dimensionen
- Typsichere Quantities
- Automatische Ableitung abgeleiteter Dimensionen
- Konstantenfaltung für Einheitenumrechnungen
- Aussagekräftige Fehlermeldungen
- Optimierte Laufzeit ohne zusätzliche Dimensionsprüfungen

---

# 3. Grundprinzipien

- Dimensionen sind Bestandteil des Typs.
- Einheiten sind Metadaten einer Dimension.
- Dimensionsfehler sind Compilerfehler.
- Konstante Umrechnungen werden zur Compile-Zeit ausgewertet.
- Laufzeitprüfungen werden nur für dynamische Fälle benötigt.

---

# 4. Typsystem

Konzeptioneller Typparameter:

```cpp
template<typename T,int L,int M,int Ti,int I,int Th,int N,int J>
class Quantity;
```

Beispiele:

```cpp
using Length = Quantity<double,1,0,0,0,0,0,0>;
using Time   = Quantity<double,0,0,1,0,0,0,0>;
using Speed  = Quantity<double,1,0,-1,0,0,0,0>;
```

---

# 5. Zulässige Operationen

```text
Length + Length            ✓
Length - Length            ✓
Length / Time              ✓ -> Velocity
Mass * Acceleration        ✓ -> Force
Energy / Time              ✓ -> Power
```

Nicht zulässig:

```text
Length + Time
Mass + Temperature
Voltage + Pressure
```

---

# 6. Typableitung

Der Compiler berechnet Dimensionsvektoren automatisch.

```text
Force × Length
↓

Energy
```

```text
Length / Time
↓

Velocity
```

---

# 7. Konstantenfaltung

Beispiel:

```cpp
auto x = 5.0_km + 250.0_m;
```

Zur Compile-Zeit:

```text
5250.0 m
```

Keine Laufzeitkonvertierung erforderlich.

---

# 8. Fehlermeldungen

Beispiel:

```cpp
Length d;
Mass m;

auto x = d + m;
```

Compilerdiagnose:

```text
Error:
Cannot add Quantity<Length> to Quantity<Mass>.

Expected:
[L¹]

Received:
[M¹]
```

---

# 9. Reflection

Der Compiler stellt Dimensionsinformationen für IDEs, Debugger und Reflection bereit.

Abfragbar:

- Dimension
- Einheit
- Quantity-Typ
- Ursprungsdefinition
- abgeleitete Dimension

---

# 10. Optimierungen

Mögliche Optimierungen:

- constexpr-Auswertung
- Dead-Code-Elimination
- Einheitennormalisierung
- SIMD-freier Dimensionsabgleich
- Inlining von Umrechnungsfaktoren

---

# 11. Öffentliche Compiler-API

```c
nova_result_t nova_compiler_check_dimensions(
    const nova_ast_t* ast);

nova_result_t nova_compiler_fold_units(
    const nova_ast_t* ast);
```

---

# 12. Integration

Zusammenarbeit mit:

- Quantity Type System
- Unit Registry
- Measurement Runtime
- Physics Engine
- Tensor Engine
- Nova Language
- C++
- Rust

---

# 13. Tests

Pflichttests:

- gültige Operationen
- ungültige Operationen
- automatische Typableitung
- constexpr-Konvertierung
- Compilerdiagnosen
- Reflection
- Optimierungen

---

# 14. Roadmap

Geplant:

- symbolische Dimensionsalgebra
- automatische Formelprüfung
- generische Dimensionsinferenz
- IDE-Unterstützung
- Linter-Regeln
- formale Verifikation

---

# 15. Zusammenfassung

Compiler Dimension Checking macht physikalische Dimensionen zu einem Bestandteil des Typsystems. Dadurch werden fehlerhafte Operationen bereits beim Übersetzen erkannt, Einheiten automatisch normalisiert und viele Berechnungen zur Compile-Zeit optimiert. Dies erhöht die Sicherheit wissenschaftlicher, technischer und industrieller Software innerhalb der Nova Compute Platform erheblich.
