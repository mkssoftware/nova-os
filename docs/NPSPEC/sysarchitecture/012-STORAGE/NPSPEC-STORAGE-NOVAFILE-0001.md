# NPSPEC-STORAGE-NOVAFILE-0001 – NovaFile Container

## Status

Entwurf

## Kategorie

Storage / NovaFile / Container / Metadata

## Zweck

NovaOS definiert `NovaFile` als natives Containerformat für Dateien, bei dem Payload, Metadaten und NovaOS-spezifische Informationen als gemeinsame logische Einheit gespeichert werden können.

```text
NovaFile (.nf)
├── Header
├── Payload
├── Metadata
└── Relationships
```

NovaFile ergänzt klassische Dateiformate, ohne deren eigentlichen Inhalt ersetzen zu müssen.

## Grundprinzip

```text
NovaFile ≠ New Payload Format
Container ≠ Object Identity
Metadata ≠ Payload
```

Ein vorhandenes Format kann unverändert innerhalb eines NovaFile-Containers gespeichert werden.

Beispiel:

```text
bild.jpg.nf
└── Payload → JPEG
```

## Container-Modell

Ein NovaFile muss mindestens folgende logische Bereiche unterstützen:

```text
Header
Payload
Metadata
```

Optional:

```text
Relationships
Provenance
Integrity Data
Extended Attributes
Preview Data
```

Die interne Struktur muss versionierbar und erweiterbar sein.

## Header

Der Header enthält die zur Interpretation des Containers notwendigen Informationen.

Mindestens:

```text
Magic
Format Version
Container Flags
Section Information
```

Unbekannte zukünftige Erweiterungen sollen übersprungen werden können, sofern sie nicht als zwingend markiert sind.

## Payload

Der Payload enthält die eigentlichen Nutzdaten.

```text
NovaFile
   ↓
Payload
   ↓
PNG / PDF / Text / Model / Binary / ...
```

NovaFile darf keine unnötige Konvertierung des ursprünglichen Datenformats erzwingen.

## Metadaten

Metadaten können direkt innerhalb des Containers gespeichert werden.

Beispiele:

```text
Semantic Type
Title
Tags
ContentID
Version Information
Provenance
Application Metadata
```

Dadurch bleiben wichtige Metadaten beim Kopieren des NovaFile-Containers gemeinsam mit dem Payload erhalten.

## Beziehungen

NovaFile darf Referenzen auf andere NovaOS-Objekte enthalten.

```text
NovaFile A
├── references → ObjectID B
└── derived-from → ObjectID C
```

Interne NovaOS-Beziehungen sollen stabile `ObjectID`s statt Pfade verwenden.

## ObjectID

Die `ObjectID` gehört zum logischen Storage-Objekt und darf nicht ausschließlich aus dem Containerinhalt abgeleitet werden.

```text
ObjectID
   ↓
NovaFile
   ↓
Payload + Metadata
```

Kopieren eines NovaFile als neues unabhängiges Objekt erzeugt grundsätzlich eine neue `ObjectID`.

## Content Addressing

Der Payload darf über eine `ContentID` identifiziert werden.

```text
Payload
   ↓
ContentID
```

Container-Metadaten dürfen geändert werden, ohne dass sich die ContentID eines unveränderten Payloads ändern muss.

## Versionierung

NovaFile muss mit dem NovaOS-Versionierungsmodell kompatibel sein.

```text
ObjectID
├── Version 1 → NovaFile State A
├── Version 2 → NovaFile State B
└── Version 3 → NovaFile State C
```

Payload- und Metadatenänderungen können gemeinsam versioniert werden.

## Integrität

NovaFile soll Integritätsinformationen für kritische Containerbereiche unterstützen.

```text
Header
Metadata
Payload
   ↓
Checksum / Hash
```

Beschädigte Bereiche müssen erkennbar sein.

## Kompression und Verschlüsselung

Ein NovaFile darf mit Storage-Kompression und Storage-Verschlüsselung kombiniert werden.

```text
Logical NovaFile
      ↓
Compression
      ↓
Encryption
      ↓
Physical Storage
```

Diese Mechanismen dürfen die logische NovaFile-Struktur nicht unnötig beeinflussen.

## Kompatibilität

NovaOS muss klassische Dateien weiterhin direkt unterstützen.

```text
Traditional File
      ↓
VFS

NovaFile
      ↓
VFS
```

NovaFile darf daher nicht zwingend für jede Datei erforderlich sein.

Export eines Payloads in sein ursprüngliches Standardformat soll möglich bleiben.

## Sidecar-Fallback

Falls ein Storage-System NovaFile nicht direkt verwenden kann, dürfen Metadaten über kontrollierte Sidecar- oder Metadata-Store-Mechanismen erhalten werden.

Native NovaFile-Speicherung bleibt jedoch die bevorzugte Form für zusammengehörige Payload- und Metadateninformationen.

## Normative Anforderungen

1. NovaOS MUSS NovaFile als versionierbares Containerformat unterstützen können.
2. NovaFile MUSS Payload und Metadaten logisch trennen.
3. Bestehende Standardformate MÜSSEN ohne unnötige Konvertierung als Payload gespeichert werden können.
4. Das Containerformat MUSS erweiterbar und versionsfähig sein.
5. Unbekannte optionale Erweiterungen SOLLEN übersprungen werden können.
6. Beziehungen zu NovaOS-Objekten SOLLEN stabile `ObjectID`s verwenden.
7. Eine Metadatenänderung DARF die `ContentID` eines unveränderten Payloads NICHT verändern.
8. NovaFile MUSS mit Storage-Versionierung kompatibel sein.
9. Kritische Containerbereiche SOLLEN durch Integritätsinformationen geschützt werden.
10. NovaFile MUSS mit Kompression und Verschlüsselung kombinierbar sein.
11. Klassische Dateien MÜSSEN auch ohne NovaFile verwendbar bleiben.
12. Der ursprüngliche Payload SOLL ohne NovaFile-spezifische Abhängigkeit exportierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-VFS-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-CONTENTADDRESS-0001`
- `NPSPEC-STORAGE-VERSIONING-0001`
- `NPSPEC-STORAGE-SEMANTIC-0001`
- `NPSPEC-STORAGE-METADATA-0001`
- `NPSPEC-STORAGE-PROVENANCE-0001`
- `NPSPEC-STORAGE-CHECKSUM-0001`
- `ADR-STORAGE-0017`

## Ergebnis

```text
NovaFile (.nf)
├── Standard Payload
├── Nova Metadata
├── Semantic Information
├── Relationships
└── Provenance
```

NovaOS erhält damit ein natives, erweiterbares Containerformat, das bestehende Datenformate beibehält und gleichzeitig Metadaten, Semantik und Objektbeziehungen zuverlässig mit den eigentlichen Daten verbindet.