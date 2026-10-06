# NPSPEC-GLOBALIZATION-NEGOTIATION-0001 – Nova Globalization Negotiation

## Status

Angenommen

## Kategorie

Globalization / Negotiation

## Zweck

NovaOS definiert die Aushandlung eines effektiven Globalization-Kontexts zwischen System, Nutzer, Sitzung, Programmen, Solutions und externen Diensten.

Dabei werden Sprache, Locale und Region anhand verfügbarer Unterstützung und expliziter Präferenzen bestimmt, ohne interne Identitäten oder Sicherheitsentscheidungen davon abhängig zu machen.

## Grundprinzipien

```text
Negotiation ≠ Permission
Negotiation ≠ Translation
Preference ≠ Requirement
Fallback ≠ Failure
Region ≠ Location
Negotiated Context ≠ System Identity
Explicit User Choice > Automatic Preference
```

## Modell

```text
GlobalizationRequest
├── PreferredLanguages[]
├── PreferredLocales[]
├── PreferredRegions[]
├── SupportedLanguages[]
├── SupportedLocales[]
├── SupportedRegions[]
└── Requirements
        ↓
Globalization Negotiation
        ↓
EffectiveGlobalizationContext
```

## Kontext

Das Ergebnis kann unabhängig bestimmte Werte enthalten:

```text
EffectiveGlobalizationContext
├── Language
├── Locale
├── Region
├── Script
└── Fallbacks[]
```

Sprache, Locale und Region müssen nicht aus derselben Quelle stammen.

Beispiel:

```text
Language = de
Locale   = de-CH
Region   = CH
```

## Priorität

NovaOS verwendet eine definierte Präferenzreihenfolge:

```text
Explicit Selection
      ↓
Program / Solution Preference
      ↓
Session Preference
      ↓
User Preference
      ↓
System Default
      ↓
Fallback
```

Explizite Entscheidungen dürfen nicht durch adaptive oder automatische Auswahl überschrieben werden.

## Aushandlung

Eine Aushandlung erfolgt durch Schnittbildung zwischen gewünschten und unterstützten Eigenschaften:

```text
Requested Preferences
        ∩
Supported Contexts
        ↓
Best Compatible Context
```

Nicht unterstützte Kombinationen führen zur nächstgeeigneten gültigen Kombination oder zur definierten Fallback-Kette.

## Fallback

Fallbacks müssen deterministisch und nachvollziehbar sein.

Beispiel:

```text
de-CH
 ↓
de
 ↓
Default Language
 ↓
Invariant Resource
```

Ein Fallback darf keine Funktion deaktivieren, nur weil eine spezifische Übersetzung oder regionale Darstellung fehlt.

## Programme und Solutions

Programme und Solutions dürfen ihre unterstützten Sprachen, Locales und Regionen deklarieren.

NovaOS bestimmt daraus zusammen mit dem aktuellen Nutzerkontext die effektiv verwendete Konfiguration.

```text
User Preference
      ∩
Supported Context
      ↓
Effective Context
```

## Externe Kommunikation

Bei Protokollen oder Diensten, die Sprach- oder Locale-Aushandlung unterstützen, darf NovaOS den effektiven Kontext bereitstellen.

Dabei dürfen keine unnötigen Globalization-Informationen übertragen werden.

Region oder Sprache dürfen insbesondere nicht als impliziter Standortnachweis verwendet werden.

## Laufzeitänderung

Ändert sich der relevante Globalization-Kontext, darf eine erneute Aushandlung stattfinden.

Betroffene Komponenten sollen ihre Darstellung aktualisieren können, ohne ihren internen Zustand oder ihre Identität neu aufzubauen.

## Normative Anforderungen

1. NovaOS MUSS Globalization-Kontexte deterministisch aushandeln können.
2. Sprache, Locale und Region MÜSSEN unabhängig ausgehandelt werden können.
3. Explizite Nutzerentscheidungen MÜSSEN Vorrang vor automatischen Präferenzen besitzen.
4. Unterstützte und gewünschte Kontexte MÜSSEN getrennt beschrieben werden.
5. Nicht unterstützte Präferenzen MÜSSEN über definierte Fallbacks behandelt werden.
6. Fallbacks MÜSSEN deterministisch sein.
7. Fehlende Lokalisierung DARF keine interne Systemidentität verändern.
8. Programme und Solutions DÜRFEN ihre unterstützten Globalization-Kontexte deklarieren.
9. Eine erneute Aushandlung MUSS bei relevanten Kontextänderungen möglich sein.
10. Negotiation DARF keine Permission oder Authority erzeugen.
11. Extern DÜRFEN nur notwendige Globalization-Informationen übertragen werden.
12. Ergebnis, Quelle und verwendeter Fallback MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-GLOBALIZATION-LOCALE-0001`
- `NPSPEC-GLOBALIZATION-LANGUAGE-0001`
- `NPSPEC-GLOBALIZATION-REGION-0001`
- `NPSPEC-SYSTEM-LOCALE-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann Sprache, Locale und Region für unterschiedliche Ausführungskontexte kontrolliert und deterministisch aushandeln. Explizite Nutzerentscheidungen bleiben maßgeblich, während unterstützte Fallbacks eine funktionsfähige Darstellung sicherstellen, ohne Systemidentität, Berechtigungen oder Sicherheitsgrenzen zu beeinflussen.