# NPSPEC-CAPABILITY-DISCOVERY-0001: Erkennung und Auflösung installierter Fähigkeiten

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:** ADR-CAPABILITY-0005, ADR-CAPABILITY-0008, ADR-CAPABILITY-0011, ADR-CAPABILITY-0012, NPSPEC-CAPABILITY-0001, NPSPEC-CAPABILITY-MODEL-0001, NPSPEC-CAPABILITY-DESCRIPTOR-0001, NPSPEC-CAPABILITY-IDENTITY-0001

---

# 1. Zweck

Diese Spezifikation definiert den Discovery-Mechanismus der Nova Capability Architecture.

Discovery beschreibt die automatische Erkennung, Suche, Auswahl und Auflösung installierter Fähigkeiten.

Die Discovery-Komponente stellt sicher, dass jede Capability unabhängig von ihrer Implementierung oder ihrem Ausführungsort eindeutig gefunden werden kann.

---

# 2. Ziele

Die Discovery verfolgt folgende Ziele:

- automatische Erkennung installierter Capabilities
- deterministische Auflösung
- schnelle Suche
- semantische Suche
- Versionserkennung
- Berücksichtigung von Berechtigungen
- Berücksichtigung der Hardware
- Unterstützung mehrerer Implementierungen
- Unterstützung verteilter Systeme

---

# 3. Architekturübersicht

```text
Benutzer
↓
Intent Planner
↓
Capability Discovery
↓
Capability Registry
↓
Version Resolution
↓
Policy Engine
↓
Runtime
↓
Capability Instance
```

Discovery trennt die Suche vollständig von der eigentlichen Ausführung.

---

# 4. Discovery-Komponenten

Das Discovery-System besteht aus:

```text
Discovery Engine
Registry
Index
Resolver
Matcher
Policy Engine
Version Resolver
Execution Resolver
```

Jede Komponente besitzt genau eine Verantwortlichkeit.

---

# 5. Capability Registry

Alle installierten Capabilities werden in einer zentralen Registry verwaltet.

Die Registry speichert:

- Identity
- Descriptor
- Contract
- Version
- Tags
- Kategorien
- Berechtigungen
- Implementierungen
- Ausführungsorte
- Status

Sie dient als autoritative Quelle für alle Discovery-Vorgänge. Das Registry-Prinzip ist ein bewährtes Architekturmuster für die dynamische Auflösung verteilter Komponenten. :contentReference[oaicite:0]{index=0}

---

# 6. Discovery Index

Zusätzlich zur Registry existiert ein optimierter Suchindex.

Er enthält:

- Capability ID
- Namespace
- Tags
- Schlüsselwörter
- Kategorien
- Datentypen
- unterstützte Eingänge
- unterstützte Ausgänge

Der Index dient ausschließlich der schnellen Suche.

---

# 7. Discovery-Arten

Nova unterstützt mehrere Discovery-Verfahren.

```text
ID Lookup
↓
Name Lookup
↓
Tag Lookup
↓
Kategorie
↓
Datentyp
↓
Semantische Suche
↓
Intent-basierte Suche
```

Alle Verfahren können kombiniert werden.

---

# 8. Lookup über Capability ID

Die eindeutigste Suche erfolgt über die Capability ID.

Beispiel:

```text
nova.image.resize
```

Diese Suche liefert höchstens eine Capability-Familie.

---

# 9. Suche über Namespace

Capabilities können über ihren Namespace gefunden werden.

Beispiele:

```text
nova.image.*

nova.audio.*

nova.ai.*

nova.robotics.*
```

Diese Suche liefert mehrere Ergebnisse.

---

# 10. Tag-basierte Suche

Capabilities können anhand ihrer Tags gesucht werden.

Beispiele:

```text
image

vision

ocr

translate

physics

robotics
```

Tags dienen ausschließlich der Suche.

---

# 11. Datentypbasierte Suche

Discovery kann anhand unterstützter Datentypen erfolgen.

Beispiele:

```text
Input:
RasterImage
↓
Ergebnisse:
Resize
Rotate
Crop
Blur
```

Dadurch können Pipelines automatisch aufgebaut werden.

---

# 12. Semantische Suche

Der Discovery-Service unterstützt semantische Suchanfragen.

Beispiele:

```text
Bild verkleinern
↓
Resize
```

```text
Dokument übersetzen
↓
Translate
```

```text
Gesichter erkennen
↓
Face Detection
```

Die semantische Suche basiert auf Metadaten, Beschreibungen und Tags.

---

# 13. Intent Discovery

Der Intent Planner verwendet Discovery direkt.

```text
Benutzer
↓
Intent
↓
Discovery
↓
Capability Graph
↓
Pipeline
```

Discovery liefert geeignete Kandidaten.

Der Planner entscheidet anschließend über die endgültige Pipeline.

---

# 14. Version Resolution

Discovery berücksichtigt Versionsregeln.

Beispiel:

```text
Capability ID
↓
Version Constraint
↓
Version Resolver
↓
Passende Version
```

Nur kompatible Versionen dürfen zurückgegeben werden.

---

# 15. Policy Resolution

Vor der Rückgabe einer Capability werden Richtlinien geprüft.

Berücksichtigt werden:

- Berechtigungen
- Sicherheitsrichtlinien
- Benutzerrechte
- Organisation
- Datenschutz
- Vertrauensstufe

Nicht zulässige Capabilities werden nicht zurückgegeben.

---

# 16. Execution Resolution

Nach erfolgreicher Discovery wird die geeignete Implementierung ausgewählt.

Mögliche Ziele:

```text
CPU
GPU
NPU
DSP
FPGA
Remote
Cluster
Cloud
```

Die Discovery liefert ausschließlich Kandidaten.

Die Runtime entscheidet über die endgültige Ausführung.

---

# 17. Ranking

Mehrere passende Capabilities werden bewertet.

Mögliche Kriterien:

- Kompatibilität
- Qualität
- Performance
- Energieverbrauch
- Vertrauensstufe
- Benutzerpräferenzen
- lokale Verfügbarkeit
- Ausführungskosten

Die höchste Bewertung wird bevorzugt.

---

# 18. Discovery Cache

Die Runtime darf Discovery-Ergebnisse zwischenspeichern.

Cache-Einträge enthalten:

```text
Capability ID
Version
Implementierung
Zeitstempel
Signatur
```

Änderungen an der Registry invalidieren den Cache automatisch.

---

# 19. Dynamische Aktualisierung

Discovery reagiert automatisch auf Änderungen.

Beispiele:

```text
Installation
Aktualisierung
Entfernung
Aktivierung
Deaktivierung
```

Der Discovery Index wird entsprechend aktualisiert.

---

# 20. Verteilte Discovery

Discovery kann mehrere Registry-Quellen verwenden.

```text
Lokale Registry
↓
Unternehmens-Registry
↓
Cluster-Registry
↓
Cloud Registry
```

Die Quellen werden in einer definierten Priorität ausgewertet.

---

# 21. Fehlerbehandlung

Kann keine passende Capability gefunden werden, liefert Discovery:

```text
Capability Not Found
```

Oder:

```text
No Compatible Version
```

Oder:

```text
Permission Denied
```

Die Runtime darf in diesen Fällen keine Ausführung starten.

---

# 22. Suchablauf

```text
Suchanfrage
↓
Discovery Engine
↓
Registry
↓
Index
↓
Version Resolver
↓
Policy Engine
↓
Ranking
↓
Execution Resolver
↓
Ergebnis
```

Alle Schritte erfolgen deterministisch.

---

# 23. YAML-Beispiel

```yaml
discovery:

  lookup:

    capabilityId: nova.image.resize

  version:

    compatible: "2.x"

  filters:

    tags:

      - image

      - resize

    permissions:

      - filesystem.read

  execution:

    preferred:

      - gpu

      - cpu
```

---

# 24. Qualitätsanforderungen

Das Discovery-System muss:

- deterministisch arbeiten,
- skalierbar sein,
- mehrere Discovery-Verfahren unterstützen,
- semantische Suche ermöglichen,
- Versionierung berücksichtigen,
- Sicherheitsrichtlinien anwenden,
- parallele Registry-Quellen unterstützen,
- Suchergebnisse reproduzierbar liefern.

Eine Registry mit automatischer Auflösung und klar definierten Suchregeln verbessert Skalierbarkeit, Wartbarkeit und Fehlertoleranz komplexer Systeme. :contentReference[oaicite:1]{index=1}

---

# 25. Konformitätskriterien

Ein Discovery-System gilt als konform, wenn:

- alle installierten Capabilities automatisch registriert werden,
- Capability IDs eindeutig aufgelöst werden,
- Versionen korrekt berücksichtigt werden,
- Sicherheitsrichtlinien angewendet werden,
- semantische Suchanfragen unterstützt werden,
- mehrere Registry-Quellen verarbeitet werden können,
- Discovery-Ergebnisse deterministisch sind,
- die Runtime ausschließlich gültige Capabilities erhält.

---

# 26. Zusammenfassung

Das Capability Discovery System bildet die zentrale Such- und Auflösungsschicht der Nova Platform.

Es ermöglicht die automatische Erkennung installierter Fähigkeiten anhand von Identität, Metadaten, Datentypen, Kategorien, Tags oder Benutzerabsichten und berücksichtigt dabei Versionierung, Berechtigungen, Sicherheitsrichtlinien sowie verfügbare Implementierungen.

Durch die Trennung von Discovery, Registry, Versionsauflösung und Runtime entsteht eine skalierbare, reproduzierbare und erweiterbare Architektur, in der Capabilities unabhängig von ihrem Speicherort oder ihrer technischen Umsetzung zuverlässig gefunden und genutzt werden können.