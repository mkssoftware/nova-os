# NPSPEC-GLOBALIZATION-RESOURCE-0001 – Nova Localization Resource

## Status

Angenommen

## Kategorie

Globalization / Resource

## Zweck

NovaOS definiert ein einheitliches Ressourcenmodell für lokalisierbare Inhalte.

Texte und andere lokalisierbare Ressourcen werden über stabile `ResourceID`s referenziert. Programme, Solutions und Systemkomponenten müssen dadurch keine übersetzten Inhalte direkt in ihrer Logik hinterlegen.

## Grundprinzipien

```text
ResourceID ≠ Display Text
Resource ≠ Language
Translation ≠ Identity
Missing Translation ≠ Missing Function
Localized Resource ≠ Authority
Resource Update ≠ Program Update
```

## Modell

```text
LocalizationResource
├── ResourceID
├── Type
├── Variants[]
├── DefaultValue
├── Version
└── Metadata
```

Eine `ResourceID` bleibt unabhängig von Sprache, Locale und tatsächlichem Inhalt stabil.

Beispiel:

```text
ResourceID: nova.settings.network.title

de → "Netzwerk"
en → "Network"
fr → "Réseau"
```

## Ressourcentypen

Lokalisierbare Ressourcen dürfen insbesondere umfassen:

```text
Text
Message
Label
Description
Tooltip
Accessibility Text
Plural Form
Formatted Message
```

Weitere registrierte Ressourcentypen dürfen ergänzt werden.

## Auflösung

```text
ResourceID
    +
Language Context
    +
Locale Context
      ↓
Resource Resolver
      ↓
Best Matching Variant
```

Die Auflösung verwendet die definierte Globalization-Negotiation und deren Fallback-Regeln.

## Fallback

Fehlt eine spezifische Variante:

```text
Requested Variant
      ↓
Language Fallback
      ↓
Default Resource
      ↓
Invariant Resource
```

Eine fehlende Übersetzung darf nicht dazu führen, dass die zugrunde liegende Funktion verschwindet.

## Formatierte Ressourcen

Ressourcen dürfen typisierte Parameter enthalten:

```text
Resource:
"{count} Dateien wurden kopiert."

Parameters:
count = 12
```

Parameterwerte werden entsprechend ihrem Typ und dem effektiven Locale formatiert.

Übersetzungen dürfen Parameter nicht in untypisierte String-Operationen umwandeln müssen.

## Pluralisierung

Sprachabhängige Pluralregeln müssen unterstützt werden können.

```text
count = 1 → Singular
count = 2 → passende Pluralform
```

Die Auswahl erfolgt anhand der Regeln der effektiven Sprache und nicht durch fest codierte Annahmen wie `count == 1`.

## Herkunft

Ressourcen dürfen bereitgestellt werden durch:

```text
System
Program
Solution
Capability
Module
Compatibility Provider
```

Ressourcen verschiedener Komponenten müssen durch stabile Namespaces voneinander getrennt bleiben.

## Aktualisierung

Sprachressourcen dürfen unabhängig von ausführbarem Code aktualisiert werden, sofern Integrität, Version und Kompatibilität erhalten bleiben.

Aktualisierte Ressourcen dürfen keine zusätzliche Authority erzeugen.

## Sicherheit

Lokalisierte Inhalte dürfen nicht zur Änderung sicherheitsrelevanter Identitäten oder Entscheidungen verwendet werden.

Insbesondere bleiben folgende Werte unabhängig von Übersetzungen:

```text
CapabilityID
ObjectID
SolutionID
ServiceID
Security Principal
Policy Identifier
```

## Normative Anforderungen

1. Lokalisierbare Ressourcen MÜSSEN über stabile `ResourceID`s referenzierbar sein.
2. `ResourceID` und dargestellter Inhalt MÜSSEN getrennt bleiben.
3. Mehrere Sprachvarianten pro Ressource MÜSSEN unterstützt werden.
4. Ressourcenauflösung MUSS definierte Fallback-Regeln verwenden.
5. Fehlende Übersetzungen DÜRFEN die zugrunde liegende Funktion nicht entfernen.
6. Typisierte Parameter MÜSSEN in lokalisierten Ressourcen unterstützt werden können.
7. Sprachabhängige Pluralregeln MÜSSEN unterstützt werden können.
8. Ressourcen verschiedener Komponenten MÜSSEN eindeutig voneinander unterscheidbar sein.
9. Ressourcen DÜRFEN unabhängig vom ausführbaren Code aktualisierbar sein.
10. Ressourcenupdates DÜRFEN keine zusätzliche Authority erzeugen.
11. Übersetzungen DÜRFEN keine internen oder sicherheitsrelevanten Identitäten verändern.
12. ResourceID, gewählte Variante, Sprache und Fallback-Pfad MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-GLOBALIZATION-LOCALE-0001`
- `NPSPEC-GLOBALIZATION-LANGUAGE-0001`
- `NPSPEC-GLOBALIZATION-REGION-0001`
- `NPSPEC-GLOBALIZATION-NEGOTIATION-0001`
- `NPSPEC-GLOBALIZATION-NUMBER-0001`
- `NPSPEC-GLOBALIZATION-DATETIME-0001`

## Ergebnis

NovaOS besitzt ein einheitliches Ressourcenmodell, bei dem lokalisierte Inhalte über stabile ResourceIDs von Systemlogik und Identitäten getrennt bleiben. Sprache, Fallback, Formatierung und Pluralisierung können unabhängig aufgelöst und aktualisiert werden, ohne Funktionen oder Sicherheitssemantik zu verändern.