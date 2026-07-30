# NPSPEC-CAPABILITY-IDENTITY-0001: Identität und eindeutige Benennung von Fähigkeiten

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:** ADR-CAPABILITY-0005, ADR-CAPABILITY-0006, ADR-CAPABILITY-0008, NPSPEC-CAPABILITY-0001, NPSPEC-CAPABILITY-MODEL-0001, NPSPEC-CAPABILITY-DESCRIPTOR-0001

---

# 1. Zweck

Diese Spezifikation definiert die Identität einer Nova Capability.

Sie legt fest,

- wie Fähigkeiten eindeutig identifiziert werden,
- wie sie benannt werden,
- welche Bestandteile unveränderlich sind,
- welche Informationen versioniert werden,
- wie Referenzen innerhalb der gesamten Plattform erfolgen.

Eine stabile Identität bildet die Grundlage für:

- Capability Registry
- Capability Contracts
- Pipelines
- Intent Planner
- Package Manager
- Runtime
- SDK
- Signaturen
- Provenance
- Langzeitkompatibilität

---

# 2. Ziele

Das Identitätsmodell verfolgt folgende Ziele:

- weltweit eindeutige Identifikation
- langfristig stabile Referenzen
- menschenlesbare Namen
- maschinenlesbare IDs
- Versionsunabhängigkeit
- eindeutige Auflösung
- Wiederverwendbarkeit
- Zukunftssicherheit

---

# 3. Grundprinzipien

Die Identität einer Capability ist:

- eindeutig,
- dauerhaft,
- unveränderlich,
- implementierungsunabhängig,
- sprachunabhängig,
- weltweit referenzierbar.

Die Identität beschreibt ausschließlich **welche Fähigkeit** vorhanden ist.

Sie beschreibt niemals:

- welche Implementierung verwendet wird,
- wo sie ausgeführt wird,
- welche Hardware genutzt wird,
- welche Programmiersprache verwendet wurde.

---

# 4. Aufbau einer Capability Identity

Jede Capability besitzt folgende Bestandteile:

```text
Capability Identity
├── Capability ID
├── Namespace
├── Kurzname
├── Anzeigename
├── Version
├── Anbieter
└── Signatur
```

---

# 5. Capability ID

Die Capability ID identifiziert eine Fähigkeit eindeutig.

Eigenschaften:

- weltweit eindeutig
- dauerhaft
- unveränderlich
- ASCII-kompatibel
- klein geschrieben
- punktgetrennt

Beispiel:

```text
nova.image.resize
```

Die Capability ID darf nach ihrer Veröffentlichung niemals geändert werden.

---

# 6. Namespace

Namespaces verhindern Namenskonflikte.

Schema:

```text
anbieter.bereich.funktion
```

Beispiele:

```text
nova.image

nova.audio

nova.ai

nova.robotics

nova.math

community.image

company.erp

research.physics
```

Namespaces dürfen hierarchisch aufgebaut sein.

---

# 7. Kurzname

Der Kurzname identifiziert die eigentliche Funktion.

Beispiele:

```text
resize

rotate

translate

summarize

ocr

detect-language

classify
```

Der Kurzname ist innerhalb seines Namespace eindeutig.

---

# 8. Anzeigename

Der Anzeigename dient ausschließlich der Benutzeroberfläche.

Beispiele:

```text
Resize Image

Bild skalieren

OCR

Dokument übersetzen
```

Der Anzeigename darf lokalisiert werden.

Er besitzt keinerlei technische Bedeutung.

---

# 9. Version

Jede Capability besitzt eine Version.

Beispiel:

```text
1.0.0

1.2.5

2.0.0
```

Versionierung erfolgt nach Semantic Versioning.

Die Version gehört nicht zur Identität der Capability selbst, sondern zu ihrer veröffentlichten Definition.

Mehrere Versionen derselben Capability dürfen gleichzeitig existieren.

---

# 10. Anbieter

Optional kann ein Anbieter angegeben werden.

Beispiele:

```text
Nova Foundation

Community

Research Lab

Example Company
```

Der Anbieter dient ausschließlich organisatorischen Zwecken.

Er verändert die Capability ID nicht.

---

# 11. Signatur

Jede veröffentlichte Capability kann digital signiert werden.

Die Signatur schützt:

- Authentizität,
- Integrität,
- Herkunft.

Die Signatur ist kein Bestandteil der Identität.

---

# 12. Eindeutigkeitsregeln

Innerhalb der Capability Registry gilt:

- jede Capability ID darf genau einmal existieren,
- jede Version ist eindeutig,
- doppelte IDs sind unzulässig,
- Anzeigenamen dürfen mehrfach vorkommen,
- Capability IDs dürfen niemals wiederverwendet werden.

---

# 13. Schreibregeln

Capability IDs verwenden ausschließlich:

```text
a-z

0-9

.

-
```

Nicht zulässig sind:

- Leerzeichen
- Umlaute
- Sonderzeichen
- Großbuchstaben

Beispiele:

```text
nova.image.resize
nova.ai.translate
nova.math.matrix.multiply
```

Ungültig:

```text
ResizeImage
Nova.Image.Resize
Bild Skalieren
nova/image
```

---

# 14. Referenzierung

Capabilities werden ausschließlich über ihre Capability ID referenziert.

Beispiel:

```yaml
capability:
  id: nova.image.resize
```

Nicht zulässig:

```yaml
name: Resize Image
```

Da Anzeigenamen lokalisiert werden können.

---

# 15. Lebenszyklus

Die Identität bleibt während des gesamten Lebenszyklus unverändert.

```text
Registrierung
↓
Veröffentlichung
↓
Versionierung
↓
Aktualisierung
↓
Veraltet
↓
Archiviert
```

Die Capability ID bleibt in jeder Phase identisch.

---

# 16. Versionierung

Änderungen an der Implementierung verändern die Identität nicht.

Beispiel:

```text
nova.image.resize

Version 1.0
↓
Version 1.1
↓
Version 2.0
```

Alle Versionen besitzen dieselbe Capability ID.

---

# 17. Umbenennung

Eine Capability darf einen neuen Anzeigenamen erhalten.

Beispiel:

```text
Resize Image
```

↓

```text
Image Resize
```

Die Capability ID bleibt unverändert.

Eine Änderung der Capability ID ist nicht zulässig.

Soll eine neue ID verwendet werden, muss eine neue Capability erstellt werden.

---

# 18. Aliase

Optional dürfen Aliase definiert werden.

Beispiel:

```yaml
aliases:
- image.resize
- resize-image
```

Aliase dienen ausschließlich der Suche.

Intern wird immer die primäre Capability ID verwendet.

---

# 19. Identitätsauflösung

Die Capability Registry löst Referenzen anhand der Capability ID auf.

```text
Capability ID
↓
Registry
↓
Descriptor
↓
Contract
↓
Implementierungen
```

Die Runtime verwendet ausschließlich die aufgelöste Capability.

---

# 20. Beispiele

## Bild skalieren

```text
Capability ID

nova.image.resize
```

---

## OCR

```text
Capability ID

nova.document.ocr
```

---

## Übersetzen

```text
Capability ID

nova.language.translate
```

---

## KI-Zusammenfassung

```text
Capability ID

nova.ai.summarize
```

---

# 21. YAML-Beispiel

```yaml
identity:

  id: nova.image.resize

  namespace: nova.image

  shortName: resize

  displayName: Resize Image

  version: 1.2.0

  vendor: Nova Foundation

  aliases:

    - image.resize
```

---

# 22. Qualitätsanforderungen

Eine Capability Identity muss:

- eindeutig sein,
- dauerhaft stabil bleiben,
- maschinenlesbar sein,
- menschenlesbar dokumentiert sein,
- implementierungsunabhängig sein,
- referenzierbar sein,
- versionierbar sein,
- validierbar sein.

Ein konsistentes Identitätsmodell erleichtert die Verwaltung, Referenzierung und langfristige Wartbarkeit großer Softwareplattformen. Architekturentscheidungen und ihre Artefakte sollten eindeutig identifizierbar und nachvollziehbar dokumentiert werden. :contentReference[oaicite:0]{index=0}

---

# 23. Konformitätskriterien

Eine Capability Identity gilt als konform, wenn:

- eine weltweit eindeutige Capability ID existiert,
- die ID den Namensregeln entspricht,
- die ID nach Veröffentlichung unveränderlich bleibt,
- Versionen korrekt verwaltet werden,
- Referenzen ausschließlich über die Capability ID erfolgen,
- Anzeigenamen unabhängig von der technischen Identität geändert werden können,
- Aliase niemals die primäre Identität ersetzen.

---

# 24. Zusammenfassung

Jede Nova Capability besitzt eine unveränderliche und weltweit eindeutige Identität.

Die Capability ID bildet die einzige technische Referenz innerhalb der gesamten Plattform und bleibt unabhängig von Implementierungen, Versionen, Anzeigenamen oder Ausführungsorten dauerhaft bestehen.

Dieses Identitätsmodell schafft eine stabile Grundlage für Registrierung, Versionierung, Planung, Ausführung und langfristige Kompatibilität aller Fähigkeiten innerhalb der Nova Platform.