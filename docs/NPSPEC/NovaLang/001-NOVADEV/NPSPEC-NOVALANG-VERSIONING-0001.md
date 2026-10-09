
# NPSPEC-NOVALANG-VERSIONING-0001 – NovaLang Versioning

## Status

Angenommen

## Kategorie

NovaLang / Versionierung und Kompatibilität

## Zweck

Definiert die Versionierung der Sprache, ihrer Laufzeit und ihrer Bibliotheken. Ziel sind langfristige Kompatibilität, reproduzierbare Builds und kontrollierte Weiterentwicklung.

## Versionsmodell

NovaLang verwendet Semantic Versioning:

`MAJOR.MINOR.PATCH`

| Bestandteil | Bedeutung |
|---|---|
| MAJOR | Inkompatible Änderungen |
| MINOR | Rückwärtskompatible Erweiterungen |
| PATCH | Fehlerkorrekturen ohne beabsichtigte Semantikänderung |

Sprachversion, Runtime-Version und Bibliotheksversion werden unabhängig verwaltet.

## Sprachversion

Jedes Projekt definiert seine verwendete NovaLang-Sprachversion in der Projektkonfiguration.

```xml
<novalang version="1.0.0" />
```

Der Compiler muss die angeforderte Sprachversion berücksichtigen.

Neuere Compiler dürfen ältere Sprachversionen unterstützen, ohne deren definierte Semantik zu verändern.

## Kompatibilität

- Bestehender gültiger Quellcode soll bei kompatiblen Versionen unverändert funktionieren.
- Inkompatible Syntax- oder Semantikänderungen erfordern eine neue Major-Version.
- Neue Sprachfunktionen dürfen bestehende Programme nicht unbeabsichtigt verändern.
- Binärkompatibilität wird getrennt von Quellcodekompatibilität bewertet.
- Veraltete Funktionen werden vor ihrer Entfernung als `Obsolete` gekennzeichnet.
- Sicherheitskorrekturen dürfen notwendige Einschränkungen einführen und müssen dokumentiert werden.

## Runtime und Bibliotheken

- Runtime und Standardbibliothek besitzen eigene Versionskennungen.
- Abhängigkeiten müssen eindeutig auflösbar sein.
- Mehrere kompatible Bibliotheksversionen dürfen parallel existieren.
- Inkompatible Laufzeit- oder ABI-Anforderungen müssen vor der Ausführung erkannt werden.
- Versionswechsel dürfen keine Capability-Berechtigungen automatisch erweitern.

## Reproduzierbare Builds

Ein Build muss seine verwendeten Versionen dokumentieren:

- NovaLang-Sprachversion
- Compiler-Version
- Runtime-Zielversion
- Standardbibliotheksversion
- Abhängigkeitsversionen
- Zielarchitektur

Identische Quellen und festgelegte Build-Abhängigkeiten müssen reproduzierbare Ergebnisse ermöglichen.

## Migration

NovaLang Studio soll bei Versionswechseln unterstützen durch:

- Kompatibilitätsdiagnosen
- Hinweise auf veraltete Sprachfunktionen
- Automatisierbare Quellcodeanpassungen
- Prüfung betroffener APIs und Abhängigkeiten

Automatische Migrationen dürfen die Programmsemantik nicht unbemerkt verändern.

## Normative Anforderungen

1. NovaLang MUSS semantische Versionierung unterstützen.
2. Sprach-, Runtime- und Bibliotheksversionen MÜSSEN getrennt verwaltet werden.
3. Projekte MÜSSEN ihre Zielsprachversion festlegen können.
4. Inkompatible Änderungen MÜSSEN eindeutig versioniert werden.
5. Compiler MÜSSEN Versionskonflikte diagnostizieren.
6. Builds MÜSSEN ihre relevanten Versionsabhängigkeiten dokumentieren.
7. Versionswechsel DÜRFEN keine Capability-Berechtigungen automatisch erweitern.
8. `.nova`, `.nlf` und `.nui` MÜSSEN demselben Sprachversionsmodell folgen.

## Ergebnis

NovaLang erhält ein langfristig wartbares Versionsmodell mit klaren Kompatibilitätsregeln, reproduzierbaren Builds und kontrollierter Migration – unabhängig von der Weiterentwicklung der NovaOS-Runtime.
