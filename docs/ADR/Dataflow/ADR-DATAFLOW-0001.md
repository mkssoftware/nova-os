# ADR-DATAFLOW-0001: Streng typisierte Datenflüsse zwischen Fähigkeiten

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Dataflow Architecture
- **Erstellt:** 2026-07-26
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine
- **Abhängigkeiten:** ADR-CAPABILITY-0001, NPSPEC-CAPABILITY-0001
- **Folgedokumente:** NPSPEC-DATAFLOW-0001, NPSPEC-DATATYPE-0001

---

## 1. Kontext

NovaOS verwendet Fähigkeiten als primäres Modell zur Bereitstellung von Softwarefunktionen.

Eine Fähigkeit erfüllt eine klar definierte Aufgabe und kann mit anderen Fähigkeiten zu größeren Arbeitsabläufen verbunden werden.

Beispiele für Fähigkeiten sind:

- Text aus einem Dokument extrahieren
- ein Bild skalieren
- eine Sprache erkennen
- Text übersetzen
- Daten validieren
- ein PDF erzeugen
- eine Nachricht versenden
- Messwerte analysieren
- Audiodaten transkribieren
- strukturierte Daten visualisieren

Mehrere Fähigkeiten können zu einer Pipeline verbunden werden.

```text
Bild
  ↓
Texterkennung
  ↓
Spracherkennung
  ↓
Übersetzung
  ↓
Zusammenfassung
  ↓
PDF-Erzeugung
```

Damit NovaOS solche Verbindungen automatisch, sicher und nachvollziehbar erzeugen kann, muss eindeutig definiert sein:

- welche Daten eine Fähigkeit akzeptiert,
- welche Daten eine Fähigkeit erzeugt,
- welche semantische Bedeutung diese Daten besitzen,
- welche Struktur die Daten haben,
- welche Einschränkungen gelten,
- ob Daten vollständig oder als Stream übertragen werden,
- ob Daten veränderlich oder unveränderlich sind,
- welche Herkunft die Daten besitzen,
- welche Berechtigungen für ihre Verarbeitung erforderlich sind.

Ein ausschließlich auf unstrukturiertem Text, Bytefolgen oder generischen Objekten basierender Datenfluss wäre für diese Architektur nicht ausreichend.

Bei einem schwach typisierten Modell könnte NovaOS beispielsweise versuchen, die Ausgabe einer Audiofähigkeit direkt an eine PDF-Signaturfähigkeit zu übergeben.

Technisch könnten beide Komponenten lediglich ein generisches Bytefeld verwenden. Semantisch wären sie jedoch inkompatibel.

Eine solche Verbindung würde erst während der Ausführung fehlschlagen oder zu einem unvorhersehbaren Ergebnis führen.

NovaOS benötigt deshalb ein Datenflussmodell, bei dem die Kompatibilität von Fähigkeiten möglichst bereits vor der Ausführung geprüft werden kann.

---

## 2. Problemstellung

Ohne streng typisierte Datenflüsse entstehen folgende Risiken:

### 2.1 Fehlerhafte Verbindungen

Fähigkeiten können miteinander verbunden werden, obwohl ihre Ein- und Ausgaben nicht kompatibel sind.

### 2.2 Späte Fehlererkennung

Fehler werden erst während der Verarbeitung erkannt, obwohl sie bereits bei der Erstellung einer Pipeline feststellbar gewesen wären.

### 2.3 Unklare Semantik

Ein technischer Datentyp wie `String`, `ByteArray` oder `Object` beschreibt nicht ausreichend, welche Bedeutung die Daten besitzen.

Ein String kann beispielsweise darstellen:

- einen Dateipfad,
- einen Personennamen,
- eine E-Mail-Adresse,
- einen natürlichsprachlichen Text,
- eine Programmiersprache,
- eine URL,
- eine Seriennummer,
- eine Datenbankabfrage.

### 2.4 Unsichere automatische Pipeline-Erstellung

Der Pipeline-Planner kann ohne verlässliche Typinformationen keine sicheren Verbindungen zwischen Fähigkeiten erzeugen.

### 2.5 Verdeckte Datenkonvertierungen

Implizite Umwandlungen können Daten verändern, Informationen verlieren oder Sicherheitsgrenzen umgehen.

### 2.6 Schlechte Diagnosemöglichkeiten

Bei generischen Datenobjekten ist schwer nachvollziehbar, an welcher Stelle einer Pipeline ein inkompatibles Objekt entstanden ist.

### 2.7 Unzureichende Optimierung

Ohne bekannte Datentypen kann NovaOS keine geeigneten Übertragungs-, Speicher- oder Hardwarepfade auswählen.

### 2.8 Unklare Berechtigungen

Die Art der verarbeiteten Daten beeinflusst, welche Schutzmaßnahmen notwendig sind.

Beispiele:

- personenbezogene Daten,
- Zugangsdaten,
- medizinische Informationen,
- Standortdaten,
- ausführbarer Code,
- Gerätesteuerungsbefehle.

---

## 3. Entscheidung

NovaOS verwendet zwischen Fähigkeiten ausschließlich **streng typisierte Datenflüsse**.

Jeder Eingangs- und Ausgangsport einer Fähigkeit muss einen vollständig definierten Datentyp besitzen.

Die Kompatibilität zweier Fähigkeiten wird vor ihrer Verbindung geprüft.

Eine Verbindung darf nur hergestellt werden, wenn:

1. der Ausgangstyp der Quellfähigkeit mit dem Eingangstyp der Zielfähigkeit kompatibel ist,
2. alle erforderlichen Typbedingungen erfüllt sind,
3. notwendige Berechtigungen vorhanden sind,
4. die Übertragungsform unterstützt wird,
5. keine unzulässige implizite Konvertierung erforderlich ist.

Der Datentyp beschreibt nicht nur die technische Speicherform, sondern auch die semantische Bedeutung der Daten.

NovaOS unterscheidet daher mindestens zwischen:

- primitiven Typen,
- strukturellen Typen,
- semantischen Typen,
- Medientypen,
- Dokumenttypen,
- Streaming-Typen,
- Referenztypen,
- Ressourcenobjekten,
- Steuer- und Ereignistypen.

---

## 4. Grundprinzipien

### 4.1 Jeder Port ist typisiert

Jeder Eingangs- und Ausgangsport einer Fähigkeit besitzt einen expliziten Typ.

Unbeschränkte Ports vom Typ `Any`, `Object`, `Variant` oder vergleichbare universelle Typen sind im regulären Fähigkeitenmodell nicht zulässig.

Beispiel:

```text
Fähigkeit: OCR

Eingang:
    image: Nova.Image.Raster

Ausgang:
    text: Nova.Document.PlainText
    layout: Nova.Document.LayoutMap
```

### 4.2 Technischer und semantischer Typ werden getrennt betrachtet

Ein technischer Typ beschreibt die Speicherstruktur.

Ein semantischer Typ beschreibt die Bedeutung.

Beispiel:

```text
Technischer Typ:
    String

Semantischer Typ:
    Nova.Identity.EmailAddress
```

Ein semantischer Typ darf zusätzliche Regeln besitzen:

```text
Nova.Identity.EmailAddress
├── technische Repräsentation: UTF-8 String
├── Validierungsregel: gültige E-Mail-Syntax
├── Datenschutzklasse: personenbezogen
└── Normalisierung: kanonische Schreibweise
```

### 4.3 Typkompatibilität wird vor der Ausführung geprüft

Eine Pipeline muss vor ihrer Ausführung validiert werden.

Nicht kompatible Verbindungen werden abgelehnt.

Beispiel einer gültigen Verbindung:

```text
Nova.Image.Raster
        ↓
OCR
        ↓
Nova.Document.PlainText
```

Beispiel einer ungültigen Verbindung:

```text
Nova.Audio.Stream
        ↓
OCR
```

Die OCR-Fähigkeit akzeptiert in diesem Beispiel ausschließlich Bilddaten.

### 4.4 Konvertierungen sind explizite Fähigkeiten

Eine Typumwandlung darf nicht unbemerkt innerhalb des Datenflusses erfolgen.

Stattdessen wird eine explizite Konverterfähigkeit eingefügt.

Beispiel:

```text
Nova.Document.Markdown
        ↓
Markdown-zu-HTML
        ↓
Nova.Document.HTML
```

Unzulässig wäre:

```text
Nova.Document.Markdown
        ↓
Nova.Document.HTML
```

wenn keine eindeutig definierte und sichtbare Konvertierung stattfindet.

### 4.5 Verlustbehaftete Konvertierungen werden gekennzeichnet

Konvertierungen werden mindestens in folgende Klassen eingeteilt:

- verlustfrei,
- normalisierend,
- potenziell verlustbehaftet,
- verlustbehaftet,
- nicht umkehrbar.

Beispiel:

```text
PNG → JPEG
Konvertierung: potenziell verlustbehaftet
```

Der Pipeline-Planner darf eine verlustbehaftete Konvertierung nicht automatisch einfügen, wenn dadurch wesentliche Daten verändert werden könnten.

### 4.6 Daten behalten ihre Herkunft

Jedes Datenobjekt kann Herkunftsinformationen besitzen.

Dazu gehören:

- ursprüngliche Quelle,
- erzeugende Fähigkeit,
- Zeitpunkt der Erzeugung,
- vorherige Verarbeitungsschritte,
- verwendete Konverter,
- relevante Versionen,
- Vertrauensstatus,
- Prüfsummen,
- Datenschutzklassifikation.

Beispiel:

```text
Nova.Document.PlainText
├── source: scan_001.png
├── createdBy: nova.ocr.tesseract
├── createdAt: 2026-07-26T14:20:00Z
├── confidence: 0.94
└── pipelineRun: run-7f30c2
```

### 4.7 Daten werden möglichst unveränderlich übertragen

Datenobjekte sollen zwischen Fähigkeiten standardmäßig unveränderlich sein.

Eine Fähigkeit erzeugt ein neues Ergebnisobjekt, anstatt das Eingabeobjekt unkontrolliert zu verändern.

Beispiel:

```text
Originalbild
    ↓
Bild zuschneiden
    ↓
Neues zugeschnittenes Bild
```

Das Original bleibt erhalten, sofern die Pipeline nicht ausdrücklich eine ersetzende Operation vorsieht.

### 4.8 Große Daten werden bevorzugt per Referenz übertragen

Große Datenobjekte sollen nicht unnötig zwischen Prozessen kopiert werden.

NovaOS kann dafür verwenden:

- unveränderliche Objektreferenzen,
- Shared Memory,
- speicherabgebildete Objekte,
- Zero-Copy-Puffer,
- Stream-Handles,
- verwaltete Ressourcenkennungen.

Die Typprüfung gilt auch bei referenzbasierter Übertragung.

### 4.9 Batch- und Streaming-Daten sind unterschiedliche Typen

Ein vollständiges Datenobjekt und ein kontinuierlicher Datenstrom sind nicht automatisch derselbe Typ.

Beispiel:

```text
Nova.Audio.Recording
```

ist nicht identisch mit:

```text
Nova.Audio.Stream
```

Eine Fähigkeit muss ausdrücklich deklarieren, welche Form sie unterstützt.

### 4.10 Fehler sind typisierte Ergebnisse

Fehler dürfen nicht ausschließlich als unstrukturierter Text übertragen werden.

NovaOS verwendet typisierte Fehlerobjekte.

Beispiel:

```text
Nova.Error.Document.UnsupportedEncoding
Nova.Error.Network.Timeout
Nova.Error.Permission.Denied
Nova.Error.Data.ValidationFailed
```

Ein Fehlerobjekt kann enthalten:

- Fehlerklasse,
- Fehlercode,
- verständliche Beschreibung,
- technische Details,
- betroffener Port,
- verursachende Fähigkeit,
- Wiederholbarkeit,
- empfohlene Gegenmaßnahmen,
- alternative Pipeline-Pfade.

---

## 5. Typklassen

NovaOS definiert mindestens die folgenden Typklassen.

### 5.1 Primitive Typen

Primitive Typen bilden grundlegende technische Werte ab.

Beispiele:

```text
Nova.Bool
Nova.Int8
Nova.Int16
Nova.Int32
Nova.Int64
Nova.UInt8
Nova.UInt16
Nova.UInt32
Nova.UInt64
Nova.Float32
Nova.Float64
Nova.Decimal
Nova.String
Nova.Binary
Nova.Timestamp
Nova.Duration
Nova.UUID
```

### 5.2 Strukturierte Typen

Strukturierte Typen bestehen aus mehreren benannten Feldern.

Beispiel:

```text
Nova.Person.Contact
├── name: Nova.Identity.PersonName
├── email: Nova.Identity.EmailAddress?
├── phone: Nova.Identity.PhoneNumber?
└── organization: Nova.Identity.OrganizationName?
```

### 5.3 Semantische Typen

Semantische Typen beschreiben fachliche Bedeutung.

Beispiele:

```text
Nova.Identity.EmailAddress
Nova.Identity.PersonName
Nova.Location.PostalAddress
Nova.Finance.CurrencyAmount
Nova.Finance.InvoiceNumber
Nova.Measurement.Temperature
Nova.Measurement.Distance
Nova.Security.Certificate
Nova.Network.URL
Nova.Code.SourceFile
```

### 5.4 Dokumenttypen

Beispiele:

```text
Nova.Document.PlainText
Nova.Document.RichText
Nova.Document.Markdown
Nova.Document.HTML
Nova.Document.PDF
Nova.Document.Spreadsheet
Nova.Document.Presentation
Nova.Document.Form
Nova.Document.LayoutMap
```

### 5.5 Medientypen

Beispiele:

```text
Nova.Image.Raster
Nova.Image.Vector
Nova.Image.Raw
Nova.Audio.Recording
Nova.Audio.Stream
Nova.Video.Recording
Nova.Video.Stream
Nova.Media.Subtitle
```

Medientypen können zusätzliche Parameter enthalten:

```text
Nova.Image.Raster
├── width
├── height
├── pixelFormat
├── colorSpace
├── alphaMode
├── orientation
└── metadata
```

### 5.6 Tabellen- und Datentypen

Beispiele:

```text
Nova.Data.Record
Nova.Data.RecordSet
Nova.Data.Table
Nova.Data.TimeSeries
Nova.Data.Graph
Nova.Data.Tensor
Nova.Data.QueryResult
```

### 5.7 Stream-Typen

Beispiele:

```text
Nova.Stream<Byte>
Nova.Stream<Nova.Audio.Frame>
Nova.Stream<Nova.Video.Frame>
Nova.Stream<Nova.Sensor.Measurement>
Nova.Stream<Nova.Log.Entry>
```

### 5.8 Ereignistypen

Ereignisse bilden Zustandsänderungen oder Auslöser ab.

Beispiele:

```text
Nova.Event.FileChanged
Nova.Event.DeviceConnected
Nova.Event.TimerElapsed
Nova.Event.PipelineCompleted
Nova.Event.UserConfirmed
Nova.Event.NetworkAvailable
```

### 5.9 Steuerungstypen

Steuerungstypen lösen Aktionen aus oder steuern Abläufe.

Beispiele:

```text
Nova.Control.Start
Nova.Control.Stop
Nova.Control.Pause
Nova.Control.Resume
Nova.Control.Cancel
Nova.Control.Commit
Nova.Control.Rollback
```

Steuerungstypen dürfen nicht mit normalen Nutzdaten vermischt werden.

### 5.10 Ressourcen- und Referenztypen

Beispiele:

```text
Nova.Resource.File
Nova.Resource.Directory
Nova.Resource.Device
Nova.Resource.NetworkEndpoint
Nova.Resource.Database
Nova.Resource.Secret
Nova.Reference<ObjectType>
```

Eine Ressourcenreferenz gewährt nicht automatisch Zugriff auf die Ressource.

Der Zugriff ist zusätzlich durch das Berechtigungsmodell geschützt.

---

## 6. Portmodell

Jede Fähigkeit definiert ihre Schnittstelle über Ports.

Ein Port besitzt mindestens:

```text
Port
├── identifier
├── direction
├── datatype
├── required
├── cardinality
├── transportMode
├── mutability
├── constraints
├── securityClassification
└── documentation
```

### 6.1 Richtung

```text
input
output
bidirectional
event-input
event-output
control-input
control-output
```

Bidirektionale Ports sollen nur verwendet werden, wenn ein klarer technischer Bedarf besteht.

### 6.2 Kardinalität

Ein Port kann folgende Kardinalitäten unterstützen:

```text
exactly-one
zero-or-one
one-or-more
zero-or-more
stream
```

Beispiel:

```text
images:
    type: Nova.Image.Raster
    cardinality: one-or-more
```

### 6.3 Pflichtstatus

Ein Eingangsport kann verpflichtend oder optional sein.

Beispiel:

```text
sourceText:
    required: true

targetLanguage:
    required: true

sourceLanguage:
    required: false
```

### 6.4 Einschränkungen

Ports dürfen zusätzliche Einschränkungen definieren.

Beispiel:

```text
image:
    type: Nova.Image.Raster
    constraints:
        minWidth: 64
        minHeight: 64
        maxWidth: 32768
        maxHeight: 32768
        colorSpaces:
            - sRGB
            - DisplayP3
```

---

## 7. Typkompatibilität

Zwei Ports sind kompatibel, wenn mindestens eine der folgenden Bedingungen erfüllt ist:

1. Beide verwenden exakt denselben Typ.
2. Der Ausgangstyp ist ein zulässiger Untertyp des Eingangstyps.
3. Der Eingangstyp akzeptiert eine definierte Schnittstelle, die vom Ausgangstyp erfüllt wird.
4. Eine explizite Konverterfähigkeit kann zwischen beiden Typen eingefügt werden.
5. Eine standardisierte Adapterfähigkeit stellt die erforderliche Übertragungsform bereit.

Nicht ausreichend sind:

- ähnliche Typnamen,
- dieselbe technische Speicherform,
- dieselbe Dateiendung,
- dieselbe MIME-Kategorie,
- eine mögliche Interpretation durch eine KI,
- eine heuristische Vermutung.

---

## 8. Subtyping

NovaOS unterstützt kontrolliertes Subtyping.

Beispiel:

```text
Nova.Document.Text
├── Nova.Document.PlainText
├── Nova.Document.Markdown
└── Nova.Document.RichText
```

Eine Fähigkeit mit folgendem Eingang:

```text
input: Nova.Document.Text
```

kann alle definierten Untertypen akzeptieren, sofern keine zusätzlichen Einschränkungen dagegen sprechen.

Eine Fähigkeit mit folgendem Eingang:

```text
input: Nova.Document.Markdown
```

akzeptiert dagegen nicht automatisch beliebige Textdokumente.

Subtyping muss explizit im Typregister definiert sein.

---

## 9. Generische Typen

NovaOS kann generische Typen verwenden.

Beispiele:

```text
Nova.List<Nova.Image.Raster>
Nova.Optional<Nova.Identity.EmailAddress>
Nova.Stream<Nova.Audio.Frame>
Nova.Result<Nova.Document.PDF, Nova.Error>
Nova.Reference<Nova.Resource.File>
```

Generische Typen müssen vollständig auflösbar sein.

Ungebundene generische Typen sind in einer ausführbaren Pipeline nicht zulässig.

---

## 10. Union- und Variantentypen

Eine Fähigkeit darf mehrere klar definierte Typen akzeptieren.

Beispiel:

```text
Nova.Union<
    Nova.Document.PDF,
    Nova.Image.Raster
>
```

Dies ist nicht mit einem unbeschränkten `Any`-Typ gleichzusetzen.

Alle möglichen Varianten müssen explizit angegeben sein.

Die Fähigkeit muss für jede Variante ein definiertes Verhalten besitzen.

---

## 11. Nullable und optionale Werte

Das Fehlen eines Wertes wird typisiert dargestellt.

Unzulässig sind mehrdeutige Ersatzwerte wie:

```text
""
0
-1
null
unknown
```

wenn nicht eindeutig spezifiziert ist, was sie bedeuten.

Stattdessen werden explizite Modelle verwendet:

```text
Nova.Optional<T>
Nova.Maybe<T>
Nova.Result<T, Error>
Nova.Unknown<T>
```

Zwischen folgenden Zuständen wird unterschieden:

- Wert vorhanden,
- Wert nicht vorhanden,
- Wert unbekannt,
- Wert noch nicht berechnet,
- Wert nicht zugänglich,
- Fehler bei der Ermittlung.

---

## 12. Einheiten und Dimensionen

Messwerte müssen ihre physikalische Einheit und Dimension enthalten.

Beispiel:

```text
Nova.Measurement.Temperature
├── value: 21.5
├── unit: Celsius
└── dimension: ThermodynamicTemperature
```

Eine reine Zahl ist für physikalische Daten nicht ausreichend.

NovaOS darf beispielsweise nicht ohne explizite Konvertierung verbinden:

```text
Millimeter
```

mit:

```text
Meter
```

Auch wenn beide dieselbe physikalische Dimension besitzen, muss die Konvertierung nachvollziehbar und deterministisch erfolgen.

Die verbindlichen Regeln werden in den Nova-Units- und Dimensionsspezifikationen definiert.

---

## 13. Schema-Versionierung

Jeder strukturierte Typ besitzt eine Version.

Beispiel:

```text
Nova.Finance.Invoice@2.1
```

Eine neue Version muss deklarieren:

- welche Felder hinzugefügt wurden,
- welche Felder entfernt wurden,
- welche Felder geändert wurden,
- ob Rückwärtskompatibilität besteht,
- ob automatische Migration möglich ist,
- ob eine Adapterfähigkeit erforderlich ist.

Breaking Changes erfordern eine neue inkompatible Hauptversion.

---

## 14. Laufzeitvalidierung

Statische Pipeline-Validierung ersetzt nicht vollständig die Laufzeitvalidierung.

NovaOS prüft während der Ausführung weiterhin:

- tatsächliche Datenstruktur,
- Wertebereiche,
- Schema-Konformität,
- Prüfsummen,
- Sicherheitskennzeichnungen,
- Größenbeschränkungen,
- Encoding,
- Ressourcenverfügbarkeit,
- Vertrauensstatus.

Ein formal korrekter Typ kann dennoch ungültige Daten enthalten.

Beispiel:

```text
Typ:
    Nova.Identity.EmailAddress

Wert:
    "keine-gueltige-adresse"
```

Die Pipeline muss solche Fehler als typisierte Validierungsfehler behandeln.

---

## 15. Sicherheitsklassifizierung

Jeder Datentyp kann einer oder mehreren Sicherheitsklassifizierungen zugeordnet werden.

Mögliche Klassifizierungen sind:

```text
Public
Internal
Confidential
Restricted
PersonalData
SensitivePersonalData
MedicalData
FinancialData
Credential
CryptographicMaterial
ExecutableCode
SystemCritical
```

Die Sicherheitsklassifizierung beeinflusst unter anderem:

- zulässige Fähigkeiten,
- Speicherorte,
- Verschlüsselung,
- Protokollierung,
- Netzwerkübertragung,
- Berechtigungsprüfung,
- Audit-Anforderungen.

Eine Fähigkeit darf nur auf Daten zugreifen, für die sie die erforderlichen Berechtigungen besitzt.

---

## 16. Transportmodi

Nicht jeder Datentyp wird auf dieselbe Weise übertragen.

NovaOS definiert standardisierte Transportmodi.

### 16.1 By Value

Das Objekt wird vollständig kopiert.

Geeignet für:

- kleine Konfigurationsobjekte
- primitive Werte
- Metadaten

### 16.2 By Reference

Nur eine Referenz wird übertragen.

Geeignet für:

- große Dateien
- Bilder
- Videos
- Modelle
- Datenbanken

### 16.3 Shared Memory

Mehrere Prozesse greifen gemeinsam auf dieselben Daten zu.

Geeignet für:

- GPU-Pipelines
- Multimedia
- Hochleistungsberechnungen

### 16.4 Stream

Die Daten werden fortlaufend übertragen.

Geeignet für:

- Audio
- Video
- Sensoren
- Netzwerkverkehr
- Live-Daten

---

## 17. Serialisierung

Jeder Typ muss mindestens eine standardisierte Serialisierung besitzen.

Unterstützte Formate können sein:

```text
Nova Binary
CBOR
JSON
YAML
Protocol Buffers
FlatBuffers
Arrow
```

Die Serialisierung darf die semantische Bedeutung eines Typs nicht verändern.

Alle Serialisierungen müssen deterministisch sein.

---

## 18. Beispiel einer Fähigkeitsbeschreibung

```yaml
capability:
  id: nova.ocr.tesseract

inputs:
  - id: image
    type: Nova.Image.Raster
    required: true

outputs:
  - id: text
    type: Nova.Document.PlainText

  - id: layout
    type: Nova.Document.LayoutMap
```

---

## 19. Beispiel einer gültigen Pipeline

```text
Nova.Image.Raster
          │
          ▼
      OCR Engine
          │
          ▼
Nova.Document.PlainText
          │
          ▼
      Translator
          │
          ▼
Nova.Document.PlainText
          │
          ▼
   Summarizer
          │
          ▼
Nova.Document.Summary
          │
          ▼
 PDF Generator
          │
          ▼
Nova.Document.PDF
```

Alle Verbindungen sind typkompatibel.

Die Pipeline kann daher bereits vor ihrer Ausführung vollständig validiert werden.

---

## 20. Beispiel einer Pipeline mit Konverter

```text
Nova.Document.Markdown
            │
            ▼
 Markdown → HTML
            │
            ▼
Nova.Document.HTML
            │
            ▼
 HTML → PDF
            │
            ▼
Nova.Document.PDF
```

Die Konverter sind eigenständige Fähigkeiten und erscheinen sichtbar im Datenfluss.

Dadurch bleiben alle Transformationen nachvollziehbar.

---

## 21. Beispiel einer ungültigen Pipeline

```text
Nova.Audio.Stream
        │
        ▼
 PDF Generator
```

Der PDF-Generator erwartet einen Dokumenttyp.

Ein Audiostream erfüllt diese Anforderung nicht.

Die Pipeline wird bereits bei der Validierung abgelehnt.

---

## 22. Konsequenzen

Die Entscheidung führt zu folgenden Vorteilen:

### Vorteile

- Frühe Fehlererkennung
- Höhere Zuverlässigkeit
- Sichere automatische Pipeline-Erstellung
- Klare semantische Datenmodelle
- Deterministische Ausführung
- Verbesserte Optimierungsmöglichkeiten
- Einfachere Parallelisierung
- Bessere Debugbarkeit
- Einheitliche Schnittstellen
- Höhere Wiederverwendbarkeit von Fähigkeiten
- Nachvollziehbare Datenherkunft
- Verbesserte Sicherheitsprüfung
- Stärkere API-Stabilität

### Nachteile

- Höherer Modellierungsaufwand
- Größeres Typregister
- Zusätzliche Konverterfähigkeiten erforderlich
- Komplexere Versionsverwaltung
- Mehr Aufwand bei der Entwicklung neuer Fähigkeiten

Diese Nachteile werden als akzeptabel bewertet, da sie langfristig die Wartbarkeit, Erweiterbarkeit und Sicherheit des Gesamtsystems erheblich verbessern.

---

## 23. Auswirkungen auf andere Komponenten

Dieses ADR beeinflusst insbesondere folgende Bereiche:

- Capability Registry
- Pipeline Planner
- Pipeline Executor
- Capability SDK
- Capability Descriptor
- Typregister
- Konverterbibliothek
- Sicherheitsmodell
- Berechtigungsmodell
- Nova Compute Platform
- Compiler
- Debugger
- Visual Pipeline Editor
- Runtime
- Remote Capability Framework

Alle genannten Komponenten müssen das hier definierte Typmodell unterstützen.

---

## 24. Implementierungshinweise

Die Umsetzung erfolgt in mehreren Stufen.

### Phase 1

- Definition des Basistypsystems
- Aufbau des Typregisters
- Primitive Datentypen

### Phase 2

- Semantische Typen
- Dokumenttypen
- Medientypen
- Ereignistypen

### Phase 3

- Automatische Typprüfung
- Pipeline-Validierung
- Konverterframework

### Phase 4

- Generische Typen
- Versionierung
- Adapter
- Verteilte Datenflüsse

### Phase 5

- Optimierungen
- Zero-Copy
- Shared Memory
- GPU-Pipelines
- Remote-Execution

---

## 25. Konformitätskriterien

Eine Implementierung gilt als konform, wenn sie mindestens folgende Anforderungen erfüllt:

- Jeder Port besitzt einen expliziten Datentyp.
- Semantische Typen sind registriert.
- Typkompatibilität wird vor der Ausführung geprüft.
- Implizite Konvertierungen finden nicht statt.
- Konverter werden explizit dargestellt.
- Fehler werden typisiert übertragen.
- Datenherkunft kann nachvollzogen werden.
- Sicherheitsklassifizierungen werden berücksichtigt.
- Versionierte Typen werden unterstützt.
- Laufzeitvalidierung ergänzt die statische Typprüfung.

---

## 26. Zusammenfassung

NovaOS führt ein streng typisiertes Datenflussmodell für sämtliche Fähigkeiten ein.

Jede Verbindung zwischen Fähigkeiten basiert auf explizit definierten technischen und semantischen Datentypen.

Typkompatibilität wird bereits während der Planung einer Pipeline geprüft.

Konvertierungen erfolgen ausschließlich durch explizite Konverterfähigkeiten.

Dadurch entstehen reproduzierbare, sichere und nachvollziehbare Datenflüsse, die sowohl lokale als auch verteilte Ausführungsmodelle unterstützen und eine zentrale Grundlage für die Capability-Plattform von NovaOS bilden.

