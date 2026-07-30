# NPSPEC-DATATYPE-PRIMITIVE-0001: Primitive Datentypen

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-28
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:**
  - NPSPEC-DATATYPE-0001
  - NPSPEC-DATAFLOW-0001
  - NPSPEC-CAPABILITY-CONTRACT-0001
  - NPSPEC-UNITS-0001
  - NPSPEC-UNITS-0003
  - NPSPEC-UNITS-0004
  - ADR-DATAFLOW-0002

---

# 1. Zweck

Diese Spezifikation definiert sämtliche primitiven Datentypen der Nova Platform.

Primitive Datentypen bilden die kleinste atomare Einheit des Nova-Typsystems und dienen als Grundlage für:

- komplexe Datentypen
- Collections
- Objekte
- Capability Contracts
- Serialisierung
- Reflection
- Compiler
- Runtime

Alle höheren Datentypen bauen ausschließlich auf primitiven Datentypen auf.

Primitive Datentypen sind grundlegende, nicht aus anderen Datentypen zusammengesetzte Werte und bilden die Basis moderner Typsysteme. :contentReference[oaicite:0]{index=0}

---

# 2. Ziele

Das primitive Typsystem verfolgt folgende Ziele:

- Plattformunabhängigkeit
- eindeutige Speicherrepräsentation
- deterministisches Verhalten
- Compileroptimierung
- hohe Performance
- starke Typisierung
- einfache Serialisierung
- langfristige ABI-Stabilität

---

# 3. Grundprinzipien

Alle primitiven Typen besitzen folgende Eigenschaften:

- unveränderlich
- atomar
- wertbasiert
- keine Objektidentität
- keine Vererbung
- keine internen Referenzen
- vollständig serialisierbar
- vollständig vergleichbar

Primitive Typen besitzen ausschließlich einen Wert.

---

# 4. Architekturübersicht

```text
Primitive Type
├── Name
├── Größe
├── Speicherlayout
├── Wertebereich
├── Standardwert
├── Serialisierung
└── Operatoren
```

---

# 5. Kategorien primitiver Typen

Nova definiert folgende Kategorien:

```text
Boolean

Integer

Floating Point

Decimal

Character

String

Binary

Identifier

Time

Null
```

Jeder primitive Typ gehört genau einer Kategorie an.

---

# 6. Boolesche Typen

Nova definiert:

```text
Bool
```

Werte:

```text
true

false
```

Eigenschaften:

- 1 Bit logischer Wert
- standardisierte Serialisierung
- logische Operatoren
- Vergleichsoperatoren

Standardwert:

```text
false
```

---

# 7. Ganzzahltypen

Nova unterstützt:

```text
Int8

UInt8

Int16

UInt16

Int32

UInt32

Int64

UInt64

Int128

UInt128
```

Eigenschaften:

- Zweierkomplement
- deterministische Überlaufregeln
- feste Bitbreite
- architekturunabhängig

---

# 8. Native Ganzzahltypen

Zusätzlich definiert Nova:

```text
Int

UInt
```

Diese besitzen die native Registergröße der Zielplattform.

Sie dienen ausschließlich Performanceoptimierungen.

Für persistente Datenformate dürfen ausschließlich Typen mit fester Bitbreite verwendet werden.

---

# 9. Gleitkommatypen

Nova unterstützt:

```text
Float16

Float32

Float64

Float128
```

Eigenschaften:

- IEEE-754 kompatibel
- NaN
- ±Infinity
- Signed Zero
- standardisierte Rundung

Alle Plattformen müssen identisches Verhalten garantieren.

---

# 10. Dezimaltypen

Für exakte Berechnungen existieren:

```text
Decimal32

Decimal64

Decimal128

BigDecimal
```

Anwendungsgebiete:

- Finanzen
- Wissenschaft
- Messtechnik
- Steuerberechnungen

Diese Typen vermeiden typische Rundungsfehler binärer Gleitkommazahlen.

---

# 11. Zeichen

Nova definiert:

```text
Char
```

Eigenschaften:

- Unicode-Codepunkt
- UTF-Unterstützung
- sprachunabhängig
- unveränderlich

Der Typ repräsentiert genau ein Unicode-Zeichen.

---

# 12. Zeichenketten

Nova definiert:

```text
String
```

Eigenschaften:

- Unicode
- immutable
- UTF-8 als Standardserialisierung
- variable Länge
- effizientes Slicing

Strings sind primitive Werttypen.

---

# 13. Binärtypen

Nova definiert:

```text
Byte

Bytes
```

Byte entspricht:

```text
UInt8
```

Bytes repräsentiert beliebige Binärdaten.

Beispiele:

- Dateien
- Netzwerkpakete
- Speicherabbilder
- kryptographische Daten

---

# 14. Identifier

Nova besitzt primitive Identifikatortypen.

Beispiele:

```text
UUID

ObjectID

CapabilityID

TypeID

VersionID
```

Diese besitzen standardisierte Formate und eindeutige Semantik.

---

# 15. Zeittypen

Folgende primitive Zeittypen existieren:

```text
Instant

Duration

Timestamp
```

Komplexere Zeittypen werden in eigenen Spezifikationen definiert.

Zeitwerte besitzen Nanosekundenauflösung.

---

# 16. Nullwert

Nova definiert einen eindeutigen Nullwert.

```text
Null
```

Eigenschaften:

- eigener primitiver Typ
- nicht identisch mit "leer"
- nicht identisch mit "0"
- nicht identisch mit "false"

Optionalwerte werden separat modelliert.

---

# 17. Standardwerte

Jeder primitive Typ besitzt einen Standardwert.

Beispiele:

```text
Bool        → false

Int32       → 0

Float64     → 0.0

Decimal64   → 0

Char        → '\0'

String      → ""

Bytes       → leer

Null        → null
```

---

# 18. Operatoren

Primitive Typen definieren standardisierte Operatoren.

Unterstützt werden:

```text
+

-

*

/

%

==

!=

<

>

<=

>=

&&

||

!

&
|
^

<<

>>
```

Nicht jeder Operator ist für jeden Typ zulässig.

---

# 19. Typkonvertierungen

Implizite Konvertierungen sind stark eingeschränkt.

Automatische Konvertierungen sind nur zulässig, wenn:

- kein Informationsverlust entsteht
- Compiler dies garantiert

Alle anderen Konvertierungen erfolgen explizit oder über Konverter-Capabilities gemäß ADR-DATAFLOW-0005.

---

# 20. Speicherlayout

Alle primitiven Typen besitzen ein definiertes Speicherlayout.

Eigenschaften:

- Byte-Reihenfolge definiert
- Alignment definiert
- Bitbreite festgelegt
- ABI-stabil

Dadurch entstehen reproduzierbare Binärformate.

---

# 21. Reflection

Alle primitiven Typen unterstützen Reflection.

Abfragbar sind:

- Name
- Größe
- Kategorie
- Wertebereich
- Standardwert
- Operatoren
- Serialisierung

Reflection ist Bestandteil der Runtime.

---

# 22. Serialisierung

Alle primitiven Typen besitzen standardisierte Serialisierungen.

Unterstützt werden:

```text
Binary

JSON

CBOR

MessagePack

Protocol Buffers
```

Das Ergebnis muss auf allen Plattformen identisch sein.

---

# 23. Erweiterbarkeit

Primitive Typen sind Bestandteil des Plattformkerns.

Benutzer dürfen keine neuen primitiven Typen definieren.

Neue primitive Typen können ausschließlich durch eine neue Version der Nova Platform eingeführt werden.

---

# 24. YAML-Beispiel

```yaml
primitiveType:

  name: Float64

  category: FloatingPoint

  size: 64

  default: 0.0

  nullable: false

  serialization:

    - binary

    - json

    - cbor

  operators:

    - arithmetic

    - comparison
```

---

# 25. Validierungsregeln

Ein primitiver Datentyp gilt als gültig, wenn:

- Name eindeutig ist,
- Bitbreite definiert ist,
- Wertebereich vollständig beschrieben ist,
- Standardwert existiert,
- Operatoren spezifiziert sind,
- Reflection unterstützt wird,
- standardisierte Serialisierung vorhanden ist.

---

# 26. Qualitätsanforderungen

Das primitive Typsystem muss:

- deterministisch arbeiten,
- ABI-stabil sein,
- plattformunabhängig funktionieren,
- vollständig serialisierbar sein,
- Compileroptimierungen ermöglichen,
- speichereffizient sein,
- langfristig kompatibel bleiben.

---

# 27. Konformitätskriterien

Eine Nova-Implementierung erfüllt diese Spezifikation, wenn:

- alle definierten primitiven Datentypen implementiert sind,
- Bitbreiten exakt eingehalten werden,
- Wertebereiche identisch sind,
- Operatoren normgerecht arbeiten,
- Reflection verfügbar ist,
- standardisierte Serialisierung unterstützt wird,
- keine plattformabhängigen Abweichungen auftreten.

---

# 28. Zusammenfassung

Die **Primitive Datentypen** bilden das Fundament des Nova-Typsystems.

Sie definieren eine kleine, vollständig standardisierte Menge atomarer Werttypen mit festem Speicherlayout, deterministischem Verhalten und plattformunabhängiger Semantik.

Auf dieser Grundlage entstehen sämtliche komplexen Datentypen, Capability Contracts und Datenflüsse der Nova Platform. Durch feste Bitbreiten, standardisierte Operatoren, Reflection und reproduzierbare Serialisierung gewährleisten die primitiven Datentypen langfristige Kompatibilität, hohe Performance und eine stabile Basis für Compiler, Runtime und verteilte Systeme.