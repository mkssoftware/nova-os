# ADR-DATAFLOW-0005: Automatische Konvertierungen erfolgen nur über explizite Konverterfähigkeiten

- **Status:** Akzeptiert
- **Datum:** 2026-07-27
- **Kategorie:** Data Flow
- **Betrifft:** Nova Runtime, Pipeline Engine, Capability Runtime, Discovery Engine, Compiler, Capability Registry
- **Entscheider:** Nova Architecture Board

---

# Kontext

In vielen Softwaresystemen werden Datentypen automatisch konvertiert.

Beispiele:

```text
String
↓
Integer
```

oder

```text
JPEG
↓
PNG
```

oder

```text
Markdown
↓
HTML
```

Diese Konvertierungen erfolgen häufig implizit innerhalb von Bibliotheken oder Laufzeitumgebungen.

Dadurch entstehen Probleme:

- versteckte Seiteneffekte
- schwer nachvollziehbares Verhalten
- uneinheitliche Implementierungen
- unterschiedliche Ergebnisse je Plattform
- schwierige Fehleranalyse
- eingeschränkte Austauschbarkeit

Besonders problematisch wird dies, wenn mehrere mögliche Konvertierungswege existieren.

---

# Problem

Angenommen eine Capability erwartet:

```text
PNG Image
```

Der Benutzer liefert jedoch:

```text
JPEG Image
```

Eine klassische Runtime könnte automatisch intern konvertieren.

Dabei bleibt jedoch unklar:

- Welche Bibliothek wurde verwendet?
- Welche Qualität wurde gewählt?
- Wurde Farbmanagement berücksichtigt?
- Welche Metadaten gingen verloren?
- Welche Kompressionsparameter wurden genutzt?

Dasselbe Problem tritt auf bei:

- Audioformaten
- Videocodecs
- Maßeinheiten
- Zeichensätzen
- Dokumentformaten
- wissenschaftlichen Daten
- KI-Tensoren

Automatische, implizite Konvertierungen erschweren Vorhersagbarkeit und Wiederholbarkeit von Verarbeitungsketten.

---

# Entscheidung

Nova führt **keine impliziten Datenkonvertierungen** durch.

Jede Konvertierung wird als eigenständige Capability modelliert.

Die Runtime darf ausschließlich Konverter verwenden, die:

- registriert,
- versioniert,
- validiert,
- discoverbar

sind.

Konvertierungen werden dadurch vollständig transparent.

---

# Architektur

```text
JPEG Image
↓
JPEG → PNG Converter
↓
PNG Image
↓
Image Resize
```

Es existiert keine versteckte Konvertierungslogik innerhalb der Runtime.

---

# Konverter sind Capabilities

Jeder Konverter besitzt:

- Descriptor
- Capability Contract
- Version
- Tests
- Observability
- Profil
- Dokumentation

Konverter unterscheiden sich architektonisch nicht von anderen Capabilities.

---

# Discovery

Die Discovery Engine kennt alle verfügbaren Konverter.

Beispiel:

```text
JPEG
↓
PNG
↓
Resize
```

Fehlt eine direkte Verbindung,

kann Discovery automatisch nach einer gültigen Konverterkette suchen.

---

# Pipeline Planner

Der Pipeline Planner darf automatisch Konverter einfügen.

Beispiel:

```text
Markdown
↓
Markdown → HTML
↓
HTML → PDF
↓
PDF Signieren
```

Die eingefügten Schritte bleiben vollständig sichtbar.

---

# Compiler

Der Compiler erkennt:

```text
Input
↓
Output
```

Sind Typen nicht kompatibel,

wird geprüft:

```text
Existiert ein Konverter?
```

Falls ja,

kann eine Pipeline vorgeschlagen oder automatisch ergänzt werden.

Fehlt ein gültiger Konverter,

ist der Datenfluss ungültig.

---

# Semantische Typen

Konverter arbeiten sowohl auf technischen als auch auf semantischen Typen.

Beispiele:

```text
Float64<Temperature Celsius>
↓
Float64<Temperature Fahrenheit>
```

oder

```text
Markdown
↓
HTML
```

oder

```text
PointCloud
↓
Mesh
```

---

# Mehrstufige Konvertierungen

Mehrere Konverter dürfen kombiniert werden.

Beispiel:

```text
DOCX
↓
HTML
↓
Markdown
↓
PDF
```

Jeder Schritt bleibt einzeln nachvollziehbar.

---

# Qualitätsprofile

Konverter können mehrere Qualitätsprofile besitzen.

Beispiele:

```text
Fast
Balanced
High Quality
Lossless
GPU
CPU
```

Discovery kann anhand des gewünschten Profils den passenden Konverter auswählen.

---

# Mehrere Implementierungen

Mehrere Anbieter können denselben Konverter bereitstellen.

Beispiel:

```text
PNG Converter
Vendor A
Vendor B
Vendor C
```

Die Runtime wählt anhand deklarierter Regeln eine Implementierung aus.

---

# Observability

Jede Konvertierung wird protokolliert.

Erfasst werden:

- verwendeter Konverter
- Version
- Laufzeit
- Qualitätsprofil
- Eingabetyp
- Ausgabetyp
- Fehler
- Warnungen

Dadurch bleibt jede Pipeline vollständig nachvollziehbar.

---

# Fehlerbehandlung

Konverter liefern standardisierte Fehler.

Beispiele:

```text
Unsupported Format
Invalid Data
Conversion Failed
Out of Memory
Quality Loss
Unsupported Version
```

Das Fehlermodell entspricht dem allgemeinen Capability-Fehlermodell.

---

# Determinismus

Ein Konverter muss für identische Eingaben und identische Konfiguration reproduzierbare Ergebnisse liefern, sofern keine explizit nichtdeterministische Arbeitsweise dokumentiert ist.

Dadurch bleiben:

- Tests reproduzierbar
- Pipelines nachvollziehbar
- Zertifizierungen möglich

---

# Sicherheit

Konverter laufen innerhalb derselben Sicherheitsarchitektur wie alle anderen Capabilities.

Sie besitzen:

- deklarierte Berechtigungen
- Sandbox
- Ressourcenlimits
- Signaturen

Unsichere Konvertierungen können durch Richtlinien deaktiviert werden.

---

# Erweiterbarkeit

Neue Konverter können jederzeit registriert werden.

Beispiele:

```text
CSV → JSON
JSON → XML
XML → YAML
OBJ → GLTF
WAV → FLAC
FLAC → MP3
GeoJSON → Shapefile
```

Die Runtime benötigt hierfür keine Änderungen.

---

# KI-Unterstützung

KI-Agenten können automatisch geeignete Konverter auswählen.

Beispiel:

```text
Benutzer:
"Konvertiere dieses Word-Dokument in PDF."
```

Pipeline:

```text
DOCX
↓
DOCX → PDF
↓
Fertig
```

Oder:

```text
DOCX
↓
DOCX → HTML
↓
HTML → PDF
```

Die vollständige Pipeline bleibt für den Benutzer sichtbar.

---

# Vorteile

- keine versteckten Konvertierungen
- vollständig transparente Pipelines
- reproduzierbare Ergebnisse
- austauschbare Konverter
- versionierbare Konvertierungslogik
- einfache Testbarkeit
- bessere Observability
- höhere Sicherheit
- intelligente Discovery
- KI kann Konvertierungswege planen

---

# Nachteile

- mehr einzelne Capabilities
- größere Registry
- längere Pipelines
- Discovery muss Konvertierungspfade berechnen
- zusätzliche Pflege für Konverterbibliotheken

Diese Nachteile werden als akzeptabel bewertet.

---

# Konsequenzen

Nova betrachtet jede Datenkonvertierung als eigenständige fachliche Fähigkeit.

Die Runtime führt keine impliziten Typ-, Format- oder Semantikkonvertierungen durch.

Stattdessen werden ausschließlich explizit registrierte Konverter-Capabilities verwendet, die denselben Qualitäts-, Sicherheits- und Versionsanforderungen wie alle anderen Capabilities unterliegen.

Dadurch bleiben sämtliche Datenflüsse transparent, reproduzierbar und nachvollziehbar. Gleichzeitig können Discovery Engine, Compiler und KI-Agenten Konvertierungspfade automatisch planen, ohne dass verborgenes Laufzeitverhalten entsteht.

Dieses Architekturprinzip stärkt die Wartbarkeit, Interoperabilität und langfristige Stabilität der gesamten Nova Dataflow-Architektur.