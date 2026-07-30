# NPSPEC-CAPABILITY-VERSIONING-0001: Versionierung und Kompatibilität

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:** ADR-CAPABILITY-0005, ADR-CAPABILITY-0006, ADR-CAPABILITY-0012, NPSPEC-CAPABILITY-0001, NPSPEC-CAPABILITY-MODEL-0001, NPSPEC-CAPABILITY-IDENTITY-0001

---

# 1. Zweck

Diese Spezifikation definiert das Versionierungs- und Kompatibilitätsmodell der Nova Capability Architecture.

Sie beschreibt:

- Versionierung von Capabilities
- Versionierung von Capability Contracts
- Kompatibilitätsregeln
- Abwärtskompatibilität
- Aufwärtskompatibilität
- Lebenszyklus von Versionen
- Deprecation
- Migration
- Auswahl geeigneter Versionen

Ziel ist eine langfristig stabile Plattform, in der mehrere Versionen derselben Capability parallel existieren können.

---

# 2. Ziele

Das Versionierungsmodell verfolgt folgende Ziele:

- langfristige Stabilität
- reproduzierbare Pipelines
- sichere Aktualisierungen
- kontrollierte Migrationen
- parallele Versionen
- minimale Inkompatibilitäten
- eindeutige Versionsauflösung
- hohe Zukunftssicherheit

---

# 3. Grundprinzipien

Die Versionierung gilt für:

- Capability
- Capability Contract
- Descriptor
- Implementierung
- Pipeline
- Solution

Diese Elemente besitzen jeweils einen eigenen Lebenszyklus.

---

# 4. Semantic Versioning

NovaOS verwendet Semantic Versioning.

Format:

```text
MAJOR.MINOR.PATCH
```

Beispiel:

```text
1.0.0

1.3.5

2.0.0
```

---

# 5. Bedeutung der Versionsbestandteile

## MAJOR

Eine neue Hauptversion kennzeichnet inkompatible Änderungen.

Beispiele:

- geänderter Capability Contract
- entfernte Ports
- geänderte Datentypen
- nicht kompatible Semantik

Beispiel:

```text
1.x.x

↓

2.0.0
```

---

## MINOR

Eine Nebenversion erweitert die Capability abwärtskompatibel.

Beispiele:

- neue optionale Parameter
- zusätzliche Funktionen
- neue Metadaten
- Optimierungen
- zusätzliche Implementierungen

Beispiel:

```text
1.2.0

↓

1.3.0
```

---

## PATCH

Eine Patch-Version enthält ausschließlich Fehlerkorrekturen oder interne Optimierungen.

Sie verändert weder den Capability Contract noch das fachliche Verhalten.

Beispiel:

```text
1.3.2

↓

1.3.3
```

---

# 6. Versionierte Elemente

Jede Capability besitzt mindestens folgende Versionen:

```text
Capability Version

Contract Version

Descriptor Version

Implementation Version
```

Diese Versionen dürfen unabhängig voneinander verwaltet werden.

---

# 7. Capability Version

Die Capability Version beschreibt die veröffentlichte fachliche Definition.

Beispiel:

```text
nova.image.resize

Version 1.4.0
```

---

# 8. Contract Version

Der Capability Contract besitzt eine eigene Version.

Der Contract bestimmt die Kompatibilität.

Ändert sich der Contract inkompatibel, muss eine neue Hauptversion erzeugt werden.

---

# 9. Descriptor Version

Der Descriptor besitzt eine eigene Version.

Änderungen an:

- Dokumentation
- Tags
- Icons
- Beschreibungen

dürfen den Capability Contract nicht verändern.

---

# 10. Implementation Version

Jede Implementierung besitzt eine eigene Version.

Beispiel:

```text
Capability

Resize Image

├── CPU 1.4.1

├── Vulkan 1.6.2

├── CUDA 2.0.0
```

Alle Implementierungen erfüllen denselben Capability Contract.

---

# 11. Kompatibilitätsregeln

Eine Capability ist kompatibel, wenn:

- der Contract unverändert bleibt,
- verpflichtende Ports erhalten bleiben,
- bestehende Datentypen unverändert bleiben,
- Garantien eingehalten werden.

Die Runtime darf kompatible Versionen automatisch auswählen.

---

# 12. Abwärtskompatibilität

Neue Versionen sollen ältere Pipelines weiterhin unterstützen.

Beispiele:

Zulässig:

```text
Neuer optionaler Parameter

Neue Metadaten

Neue Implementierung
```

Nicht zulässig:

```text
Pflichtparameter entfernen

Datentyp ändern

Port löschen
```

---

# 13. Aufwärtskompatibilität

Eine ältere Runtime darf neuere Versionen verwenden, sofern:

- der Capability Contract vollständig verstanden wird,
- keine unbekannten Pflichtbestandteile vorhanden sind.

Andernfalls muss die Runtime die Capability ablehnen.

---

# 14. Mehrere Versionen

Mehrere Versionen dürfen parallel installiert sein.

Beispiel:

```text
nova.image.resize

├── 1.0.0

├── 1.2.4

├── 1.9.3

└── 2.0.0
```

Pipelines referenzieren explizit ihre gewünschte Version oder definieren zulässige Versionsbereiche.

---

# 15. Version Resolution

Die Runtime löst Capability-Versionen anhand definierter Regeln auf.

```text
Pipeline

↓

Capability ID

↓

Versionsregel

↓

Capability Registry

↓

Geeignete Version

↓

Runtime
```

Die Auflösung muss deterministisch erfolgen.

---

# 16. Deprecation

Eine Capability kann als veraltet markiert werden.

Beispiel:

```yaml
deprecated: true

replacement:

nova.image.resize.v2
```

Veraltete Capabilities bleiben weiterhin ausführbar.

Neue Entwicklungen sollen jedoch die empfohlene Nachfolgeversion verwenden.

---

# 17. End of Support

Nach Ablauf des Supports kann eine Capability den Status erhalten:

```text
End of Support
```

Dies bedeutet:

- keine Fehlerkorrekturen,
- keine Sicherheitsupdates,
- keine Weiterentwicklung.

Bestehende Pipelines bleiben weiterhin reproduzierbar, sofern die entsprechende Version verfügbar ist.

---

# 18. Migration

Eine Capability kann Migrationsinformationen enthalten.

Beispiel:

```yaml
migration:

from:

- 1.x

guide:

documentation/migrate.md
```

Migrationen dürfen automatisiert unterstützt werden.

---

# 19. Kompatibilitätsstufen

NovaOS unterscheidet folgende Stufen:

```text
Vollständig kompatibel

↓

Abwärtskompatibel

↓

Teilweise kompatibel

↓

Nicht kompatibel
```

Diese Einstufung dient der automatischen Versionsauswahl.

---

# 20. Versionierung von Pipelines

Pipelines speichern:

- Pipeline-Version
- Capability-Versionen
- Contract-Versionen

Beispiel:

```text
Pipeline 2.1

↓

OCR 1.5

↓

Translate 3.2

↓

Summarize 2.0
```

Dadurch bleiben Ausführungen reproduzierbar.

---

# 21. Versionierung von Solutions

Solutions speichern ausschließlich referenzierte Capability-Versionen.

Sie enthalten selbst keine Implementierungen.

---

# 22. YAML-Beispiel

```yaml
identity:

  id: nova.image.resize

  version: 2.1.0

contract:

  version: 2.0.0

descriptor:

  version: 4

implementation:

  version: 2.3.1

compatibility:

  deprecated: false

  supportedFrom: 2.0

  compatibleWith:

    - 2.x
```

---

# 23. Qualitätsanforderungen

Das Versionierungsmodell muss:

- deterministisch sein,
- reproduzierbare Pipelines ermöglichen,
- mehrere Versionen unterstützen,
- Semantic Versioning verwenden,
- automatische Auflösung erlauben,
- Migration unterstützen,
- langfristige Kompatibilität gewährleisten,
- klare Deprecation-Regeln besitzen.

Diese Anforderungen unterstützen eine kontrollierte Weiterentwicklung der Plattform und reduzieren Risiken bei Aktualisierungen und Migrationen.

---

# 24. Konformitätskriterien

Eine Implementierung gilt als konform, wenn:

- Semantic Versioning verwendet wird,
- Capability-, Contract-, Descriptor- und Implementierungsversionen getrennt verwaltet werden,
- mehrere Versionen parallel unterstützt werden,
- Versionsauflösung deterministisch erfolgt,
- Deprecation eindeutig gekennzeichnet wird,
- reproduzierbare Pipelines gewährleistet sind,
- Kompatibilitätsregeln eingehalten werden.

---

# 25. Zusammenfassung

Die Nova Capability Architecture verwendet ein mehrstufiges Versionierungsmodell, das Capability, Contract, Descriptor und Implementierung getrennt betrachtet.

Durch Semantic Versioning, parallele Versionen, explizite Kompatibilitätsregeln und deterministische Versionsauflösung können Capabilities über lange Zeiträume weiterentwickelt werden, ohne bestehende Pipelines oder Solutions zu gefährden.

Dieses Modell bildet die Grundlage für eine stabile, reproduzierbare und langfristig wartbare Capability-Plattform.