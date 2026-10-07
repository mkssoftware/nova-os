# NPSPEC-FONT-PROVENANCE-0001 – Nova Font Provenance

## Status

Angenommen

## Kategorie

Font / Provenance

## Zweck

NovaOS definiert die nachvollziehbare Herkunft von Fontressourcen.

Font Provenance beschreibt, woher ein Font stammt, wie er in das System gelangt ist, welche Transformationen er durchlaufen hat und welche Integritäts- oder Vertrauensinformationen zu ihm bekannt sind.

## Grundprinzipien

```text
Provenance ≠ Trust
Provenance ≠ Integrity
Provenance ≠ Signature
Provenance ≠ Font Identity
Known Origin ≠ Trusted Origin
Unknown Origin ≠ Malicious Font
```

## Modell

```text
FontProvenance
├── FontID
├── SourceType
├── SourceIdentity
├── Publisher
├── PackageIdentity
├── ContentIdentity
├── SignatureInfo
├── AcquisitionTime
├── Transformations[]
├── ParentProvenance
└── State
```

Nicht verfügbare Informationen müssen als `Unknown` dargestellt werden und dürfen nicht erfunden werden.

## Quellen

Provenance muss unterschiedliche Fontquellen abbilden können:

```text
System
User Installation
Application
Solution
Document Embedded
Remote Provider
Package
Imported File
Generated Font
Temporary Resource
```

## Provenance-Kette

Die Herkunft darf als nachvollziehbare Kette dargestellt werden:

```text
Original Source
      ↓
Package / Download
      ↓
Validation
      ↓
Transformation
      ↓
Installation
      ↓
Registered Font
```

Jeder bekannte Schritt darf seine eigene Provenance-Information besitzen.

## Transformationen

Fonts können vor ihrer Verwendung verändert werden:

```text
Subsetting
Conversion
Optimization
Embedding
Extraction
Repackaging
Generation
```

Eine Transformation muss von der ursprünglichen Quelle unterscheidbar bleiben.

Wird der Fontinhalt verändert, entsteht eine neue Inhaltsidentität.

## Integrität

Provenance darf mit kryptografischen Inhaltsidentitäten verknüpft werden:

```text
FontID
   ↓
ContentIdentity
   ↓
Hash / Signature
   ↓
Provenance Record
```

Ein gültiger Hash bestätigt Inhaltsgleichheit, aber nicht automatisch Vertrauen.

## Signaturen

Falls Signaturen vorhanden sind, müssen mindestens folgende Zustände unterscheidbar sein:

```text
Valid
Invalid
Expired
Revoked
Unknown
Unsigned
```

Die Signaturbewertung erfolgt über die NovaOS-Trust-Infrastruktur.

## Eingebettete Fonts

Dokumentgebundene Fonts müssen ihre Herkunft zum jeweiligen Dokument oder Container behalten.

```text
Document ObjectID
      ↓
Embedded Font
      ↓
Font Provenance
```

Das Extrahieren eines eingebetteten Fonts darf seine ursprüngliche Herkunft nicht verlieren.

## Weitergabe

Wird ein Font exportiert, kopiert oder in ein anderes Paket eingebettet, darf die bestehende Provenance erhalten und um den neuen Vorgang ergänzt werden.

```text
Existing Provenance
       +
New Provenance Event
       ↓
Extended Provenance Chain
```

## Datenschutz

Provenance darf keine unnötigen personenbezogenen oder gerätespezifischen Informationen speichern.

Herkunftsnachweise müssen dem Prinzip der Datenminimierung folgen.

## Normative Anforderungen

1. NovaOS MUSS Font Provenance systemweit abbilden können.
2. Provenance MUSS von Trust, Integrität und Fontidentität getrennt bleiben.
3. Unbekannte Herkunft MUSS explizit als `Unknown` darstellbar sein.
4. Fontquellen MÜSSEN eindeutig klassifizierbar sein.
5. Transformationen SOLLEN als Provenance-Ereignisse erhalten bleiben.
6. Inhaltsänderungen MÜSSEN eine neue Content Identity erzeugen.
7. Kryptografische Hashes und Signaturen MÜSSEN mit Provenance verknüpfbar sein.
8. Signaturstatus MUSS unabhängig vom Provenance-Status dargestellt werden.
9. Eingebettete Fonts MÜSSEN auf ihren Ursprungskontext zurückführbar sein.
10. Provenance-Ketten DÜRFEN bei Kopieren oder Export nicht stillschweigend verloren gehen.
11. Provenance-Daten MÜSSEN gegen unautorisierte Manipulation geschützt werden können.
12. Quelle, Content Identity, Transformationen, Signaturstatus und Provenance-Kette MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FONT-MANAGER-0001`
- `NPSPEC-FONT-SECURITY-0001`
- `NPSPEC-TRUST-SYSTEMCOMPONENT-0001`
- `NPSPEC-STORAGE-IDENTITY-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann die Herkunft einer Fontressource über ihren gesamten Lebenszyklus nachvollziehen. Quelle, Transformationen, Inhaltsidentität, Signaturen und Weitergabe bleiben unterscheidbar, ohne Provenance automatisch mit Vertrauen gleichzusetzen.