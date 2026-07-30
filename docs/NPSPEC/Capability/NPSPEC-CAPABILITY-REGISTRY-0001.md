# NPSPEC-CAPABILITY-REGISTRY-0001: Lokale Fähigkeiten-Registry

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:** ADR-CAPABILITY-0005, ADR-CAPABILITY-0008, ADR-CAPABILITY-0009, ADR-CAPABILITY-0011, NPSPEC-CAPABILITY-0001, NPSPEC-CAPABILITY-DISCOVERY-0001, NPSPEC-CAPABILITY-IDENTITY-0001, NPSPEC-CAPABILITY-DESCRIPTOR-0001

---

# 1. Zweck

Diese Spezifikation definiert die lokale Nova Capability Registry.

Die Registry bildet die zentrale Wissensbasis aller auf einem Nova-System installierten Fähigkeiten.

Sie verwaltet ausschließlich Metadaten und beschreibt:

- welche Capabilities vorhanden sind,
- welche Versionen existieren,
- welche Implementierungen verfügbar sind,
- welche Berechtigungen benötigt werden,
- welchen Status eine Capability besitzt.

Die Registry enthält niemals Anwendungsdaten oder Laufzeitzustände.

---

# 2. Ziele

Die Registry verfolgt folgende Ziele:

- zentrale Verwaltung aller Capabilities
- schnelle Auflösung
- deterministische Suche
- Versionierung
- Integritätsprüfung
- hohe Performance
- Offline-Funktion
- Erweiterbarkeit
- vollständige Nachvollziehbarkeit

Registries dienen als autoritative Quelle für Metadaten und ermöglichen eine klare Trennung zwischen Beschreibung, Suche und Laufzeitverhalten. :contentReference[oaicite:0]{index=0}

---

# 3. Grundprinzipien

Die Capability Registry ist:

- lokal
- persistent
- transaktional
- versioniert
- signaturfähig
- schreibgeschützt für normale Anwendungen
- ausschließlich über Registry-APIs veränderbar

Direkte Änderungen an den Registry-Daten sind unzulässig.

---

# 4. Architekturübersicht

```text
Capability Installer

↓

Registry Manager

↓

Capability Registry

├── Identity
├── Descriptor
├── Contract
├── Versionen
├── Implementierungen
├── Berechtigungen
├── Tags
├── Status
└── Signaturen

↓

Discovery

↓

Runtime
```

Die Registry dient als zentrale Informationsquelle für Discovery und Runtime.

---

# 5. Verantwortlichkeiten

Die Registry verwaltet:

- installierte Capabilities
- Versionen
- Descriptoren
- Capability Contracts
- Kategorien
- Tags
- Berechtigungen
- Abhängigkeiten
- Signaturen
- Lebenszyklusstatus

Sie führt selbst keine Capability aus.

---

# 6. Registry-Eintrag

Jede Capability besitzt genau einen Registry-Eintrag pro Version.

```text
Registry Entry

├── Identity
├── Descriptor
├── Contract
├── Version
├── Status
├── Installationsinformationen
├── Implementierungen
├── Signatur
└── Prüfsummen
```

---

# 7. Identity

Gespeichert werden:

```text
Capability ID

Namespace

Kurzname

Anzeigename

Version

Anbieter
```

Die Identity ist unveränderlich.

---

# 8. Descriptor

Die Registry speichert den vollständigen Capability Descriptor.

Beispiele:

- Eingänge
- Ausgänge
- Parameter
- Kategorien
- Dokumentation
- Tags
- Ressourcenbedarf

Der Descriptor dient der Discovery und Validierung.

---

# 9. Capability Contract

Die Registry speichert den öffentlichen Contract.

Dieser enthält:

- Inputs
- Outputs
- Datentypen
- Garantien
- Fehlerdefinitionen

Die Runtime verwendet ausschließlich den registrierten Contract.

---

# 10. Versionen

Mehrere Versionen derselben Capability dürfen gleichzeitig registriert sein.

Beispiel:

```text
nova.image.resize

├── 1.0.0

├── 1.3.2

└── 2.0.0
```

Jede Version besitzt einen eigenen Registry-Eintrag.

---

# 11. Implementierungen

Zu jeder Capability können mehrere Implementierungen existieren.

Beispiel:

```text
CPU

GPU

SIMD

Vulkan

CUDA

OpenCL

Remote
```

Die Registry speichert ausschließlich deren Beschreibung.

---

# 12. Status

Jeder Registry-Eintrag besitzt einen Status.

Mögliche Werte:

```text
Installed

Validated

Registered

Active

Inactive

Deprecated

Archived

Removed
```

Der Status wird ausschließlich vom Lifecycle Manager geändert.

---

# 13. Berechtigungen

Alle benötigten Berechtigungen werden gespeichert.

Beispiele:

```text
filesystem.read

filesystem.write

camera

network

usb

bluetooth
```

Discovery und Runtime nutzen diese Informationen zur Richtlinienprüfung.

---

# 14. Abhängigkeiten

Capabilities können andere Capabilities voraussetzen.

Beispiel:

```text
nova.math

nova.image.core

nova.ai.runtime
```

Die Registry verwaltet sämtliche Abhängigkeiten.

---

# 15. Kategorien

Capabilities können kategorisiert werden.

Beispiele:

```text
Image

Audio

Video

AI

Office

Robotics

Physics

Network
```

Die Kategorien dienen ausschließlich der Suche.

---

# 16. Tags

Zusätzliche Tags verbessern die Auffindbarkeit.

Beispiele:

```text
ocr

vision

translate

speech

camera

resize

compression
```

Tags besitzen keine technische Bedeutung.

---

# 17. Signaturen

Jeder Registry-Eintrag kann signiert werden.

Gespeichert werden:

```text
Signatur

Aussteller

Algorithmus

Zeitstempel

Zertifikatskette
```

Die Runtime prüft Signaturen vor der Aktivierung.

---

# 18. Prüfsummen

Zusätzlich werden Integritätsinformationen gespeichert.

Beispiele:

```text
SHA-256

SHA-512
```

Die Prüfsummen dienen der Erkennung manipulierter Dateien.

---

# 19. Registry-Operationen

Die Registry unterstützt folgende Operationen:

```text
Register

Update

Remove

Activate

Deactivate

Lookup

List

Validate
```

Alle Änderungen erfolgen transaktional.

---

# 20. Suchmöglichkeiten

Die Registry unterstützt Suchanfragen über:

```text
Capability ID

Namespace

Kategorie

Tags

Version

Datentyp

Berechtigungen

Status
```

Die eigentliche semantische Suche erfolgt durch die Discovery Engine.

---

# 21. Speicherung

Die Registry wird lokal persistent gespeichert.

Anforderungen:

- atomare Änderungen
- Journaling
- Prüfsummen
- automatische Wiederherstellung
- transaktionale Konsistenz

Ein unerwarteter Systemabsturz darf die Registry nicht in einen inkonsistenten Zustand versetzen.

---

# 22. Aktualisierung

Änderungen erfolgen ausschließlich über den Registry Manager.

Ablauf:

```text
Installation

↓

Validierung

↓

Transaktion

↓

Registry Update

↓

Index Update

↓

Discovery Benachrichtigung
```

Unvollständige Aktualisierungen werden vollständig zurückgerollt.

---

# 23. Sicherheit

Die Registry ist gegen Manipulation geschützt.

Anforderungen:

- digitale Signaturen
- Prüfsummen
- transaktionale Änderungen
- Zugriffskontrolle
- Audit-Protokoll
- Rollback-Unterstützung

Normale Anwendungen besitzen ausschließlich Lesezugriff über definierte APIs.

---

# 24. YAML-Beispiel

```yaml
registry:

  capabilityId: nova.image.resize

  version: 2.0.0

  status: active

  descriptor: image.resize.capability.yaml

  contractVersion: 2.0.0

  implementations:

    - cpu

    - gpu

    - vulkan

  permissions:

    - filesystem.read

    - filesystem.write

  signature:

    verified: true

  checksum:

    sha256: 4F7C9A...
```

---

# 25. Qualitätsanforderungen

Die Capability Registry muss:

- vollständig lokal funktionieren,
- transaktionale Änderungen unterstützen,
- deterministisch arbeiten,
- mehrere Versionen parallel verwalten,
- digitale Signaturen speichern,
- Integritätsprüfungen ermöglichen,
- schnelle Suchoperationen unterstützen,
- reproduzierbare Registry-Zustände gewährleisten.

Eine Registry sollte als autoritative Quelle für Metadaten dienen und Beschreibung strikt von Laufzeitlogik trennen, um Konsistenz und Wartbarkeit sicherzustellen. :contentReference[oaicite:1]{index=1}

---

# 26. Konformitätskriterien

Eine Registry gilt als konform, wenn:

- jede installierte Capability registriert ist,
- Capability IDs eindeutig sind,
- mehrere Versionen parallel verwaltet werden,
- Descriptoren vollständig gespeichert werden,
- Capability Contracts referenzierbar sind,
- alle Änderungen transaktional erfolgen,
- Integritätsinformationen gespeichert werden,
- Discovery und Runtime ausschließlich über definierte Registry-Schnittstellen auf die Daten zugreifen.

---

# 27. Zusammenfassung

Die lokale Capability Registry bildet das autoritative Verzeichnis aller auf einem Nova-System installierten Fähigkeiten.

Sie verwaltet Identitäten, Descriptoren, Capability Contracts, Versionen, Implementierungen, Berechtigungen, Signaturen und Integritätsinformationen in einer transaktionalen und persistenten Struktur.

Durch die klare Trennung zwischen Registry, Discovery und Runtime entsteht eine skalierbare und langfristig wartbare Architektur, in der Capabilities eindeutig identifiziert, sicher verwaltet und deterministisch aufgelöst werden können.