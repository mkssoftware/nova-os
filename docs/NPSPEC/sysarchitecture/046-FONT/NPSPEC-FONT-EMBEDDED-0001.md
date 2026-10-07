# NPSPEC-FONT-EMBEDDED-0001 – Nova Embedded Fonts

## Status

Angenommen

## Kategorie

Font / Embedded Fonts

## Zweck

NovaOS definiert die sichere Verwendung von Fonts, die direkt in Dokumente, Dateien, Pakete oder andere Inhalte eingebettet sind.

Eingebettete Fonts bleiben an ihren Ursprungskontext gebunden und werden nicht automatisch als systemweit installierte Fonts behandelt.

## Grundprinzipien

```text
Embedded Font ≠ Installed Font
Embedded Font ≠ System Font
Embedding ≠ Installation
Visibility ≠ Authority
Document Trust ≠ Font Trust
Font Name ≠ Font Identity
```

## Architektur

```text
Document / Container
        ↓
Embedded Font Discovery
        ↓
Extraction / Mapping
        ↓
Validation
        ↓
Scoped Registration
        ↓
Font Manager
        ↓
Shaping / Rendering
```

## Modell

```text
EmbeddedFont
├── FontID
├── ParentObjectID
├── ContentIdentity
├── Format
├── Scope
├── EmbeddingPolicy
├── Provenance
├── ValidationState
└── State
```

`ParentObjectID` verweist auf das Dokument oder den Container, aus dem der Font stammt.

## Scope

Ein eingebetteter Font erhält standardmäßig einen auf seinen Ursprung begrenzten Scope:

```text
Document
Container
Application
Solution
Temporary Session
```

Er darf nicht automatisch im globalen System- oder Benutzer-Fontbestand erscheinen.

## Aktivierung

```text
Open Document
     ↓
Discover Embedded Fonts
     ↓
Validate
     ↓
Create Scoped Font Registration
     ↓
Render Document
     ↓
Release Registration
```

Die Registrierung darf für die Lebensdauer des Dokuments oder eines kontrollierten Caches bestehen bleiben.

## Font-Auswahl

Besitzt ein Dokument einen eingebetteten Font, darf dieser innerhalb seines Scopes gegenüber einem gleichnamigen externen Font bevorzugt werden.

Die Auswahl erfolgt über die konkrete Fontidentität und nicht ausschließlich über den Family Name.

## Subsetting

Eingebettete Fonts dürfen als Subset vorliegen.

```text
Original Font
     ↓
Subset
     ↓
Embedded Font
```

Ein Subset besitzt eine eigene Inhaltsidentität und darf nur für tatsächlich enthaltene Glyphen als vollständig betrachtet werden.

Fehlende Glyphen können über normalen Font Fallback ergänzt werden.

## Provenance

Die Verbindung zum Ursprung muss erhalten bleiben:

```text
Parent ObjectID
      ↓
Embedded Font
      ↓
ContentIdentity
      ↓
Provenance Chain
```

Extraktion oder temporäre Materialisierung darf diese Beziehung nicht verlieren.

## Embedding-Rechte

Fontformate können Metadaten über zulässige Einbettungsformen enthalten.

NovaOS muss solche Einschränkungen erkennen und für Export-, Bearbeitungs- oder Weitergabefunktionen verfügbar machen können.

Die technische Sicherheitsarchitektur darf jedoch nicht allein auf diesen Metadaten vertrauen.

## Sicherheit

Eingebettete Fonts gelten als nicht vertrauenswürdige Inhalte.

Sie müssen dieselbe Fontvalidierung und Ressourcenbegrenzung wie externe Fonts durchlaufen.

Ein manipuliertes Dokument darf über einen eingebetteten Font keine höheren Systemrechte erhalten.

## Cache

Validierte eingebettete Fonts dürfen gecacht werden.

Der Cache muss mindestens an folgende Identitäten gebunden sein:

```text
ParentObjectID
ContentIdentity
FontID
ValidationGeneration
```

Eine Änderung des eingebetteten Fontinhalts muss den betroffenen Cache invalidieren.

## Normative Anforderungen

1. NovaOS MUSS eingebettete Fonts unterstützen können.
2. Eingebettete Fonts MÜSSEN vor Verwendung validiert werden.
3. Embedding DARF keine systemweite Fontinstallation auslösen.
4. Eingebettete Fonts MÜSSEN standardmäßig einen begrenzten Scope besitzen.
5. Fontnamen DÜRFEN nicht allein zur Identifikation eingebetteter Fonts verwendet werden.
6. Der Ursprung über `ParentObjectID` MUSS nachvollziehbar bleiben.
7. Font-Subsets MÜSSEN als eigenständige Inhaltsvarianten behandelbar sein.
8. Fehlende Glyphen MÜSSEN normalen Font Fallback verwenden können.
9. Embedding-Metadaten und Einschränkungen MÜSSEN auswertbar sein.
10. Eingebettete Fonts DÜRFEN keine zusätzliche Authority erzeugen.
11. Caches MÜSSEN an validierte Inhaltsidentitäten gebunden und invalidierbar sein.
12. FontID, ParentObjectID, Scope, Provenance, EmbeddingPolicy und Validierungsstatus MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FONT-MANAGER-0001`
- `NPSPEC-FONT-FALLBACK-0001`
- `NPSPEC-FONT-SECURITY-0001`
- `NPSPEC-FONT-PROVENANCE-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann eingebettete Fonts sicher und kontextgebunden verwenden, ohne sie mit installierten Systemfonts gleichzusetzen. Herkunft, Scope, Inhaltsidentität und Sicherheitsstatus bleiben erhalten, während Dokumente ihre vorgesehenen Schriften zuverlässig für Shaping und Rendering nutzen können.