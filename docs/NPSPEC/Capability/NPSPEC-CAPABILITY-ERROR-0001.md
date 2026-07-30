# NPSPEC-CAPABILITY-ERROR-0001: Einheitliches Fehlermodell für Fähigkeiten

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:** ADR-CAPABILITY-0005, ADR-CAPABILITY-0012, ADR-CAPABILITY-0015, NPSPEC-CAPABILITY-0001, NPSPEC-CAPABILITY-CONTRACT-0001, NPSPEC-CAPABILITY-STATE-0001, NPSPEC-CAPABILITY-COMPOSITION-0001, NPSPEC-CAPABILITY-PROFILE-0001

---

# 1. Zweck

Diese Spezifikation definiert ein einheitliches Fehlermodell für sämtliche Nova Capabilities.

Das Fehlermodell standardisiert:

- Fehlerklassen
- Fehlercodes
- Fehlerobjekte
- Fehlerweitergabe
- Fehlerbehandlung
- Wiederherstellung
- Diagnose
- Protokollierung

Dadurch können alle Capabilities Fehler auf identische Weise beschreiben und behandeln.

---

# 2. Ziele

Das Fehlermodell verfolgt folgende Ziele:

- einheitliche Fehlerdarstellung
- reproduzierbare Diagnose
- automatische Fehlerbehandlung
- einfache Analyse
- sichere Fehlerweitergabe
- verständliche Fehlermeldungen
- maschinenlesbare Fehler
- langfristige Erweiterbarkeit

Ein konsistentes Fehlermodell erhöht Zuverlässigkeit und Wartbarkeit komplexer Softwaresysteme. :contentReference[oaicite:0]{index=0}

---

# 3. Grundprinzipien

Jeder Fehler ist ein eigenständiges Objekt.

Ein Fehler besitzt:

- eindeutige Identität
- Typ
- Fehlercode
- Ursache
- Schweregrad
- Kontext
- mögliche Lösung

Fehler sind Bestandteil des Capability Contracts.

---

# 4. Architekturübersicht

```text
Capability
├── Error Object
├── Error Category
├── Error Code
├── Severity
├── Context
├── Diagnostics
├── Recovery
└── Resolution
```

---

# 5. Fehlerobjekt

Jeder Fehler besitzt folgende Struktur:

```text
Error ID
Error Code
Category
Severity
Title
Description
Cause
Context
Timestamp
Recovery
Diagnostics
```

Das Fehlerobjekt ist vollständig maschinenlesbar.

---

# 6. Fehlerkategorien

Nova unterscheidet folgende Hauptkategorien:

```text
Validation
Input
Output
Configuration
Permission
Security
Resource
State
Runtime
Dependency
Communication
Timeout
Storage
Hardware
Internal
Unknown
```

Jeder Fehler gehört genau einer Hauptkategorie an.

---

# 7. Fehlercodes

Jeder Fehler besitzt einen stabilen Code.

Beispiel:

```text
NOVA-VAL-0001
NOVA-DEP-0104
NOVA-RUN-1007
NOVA-HW-2002
```

Fehlercodes bleiben versionsübergreifend stabil.

---

# 8. Schweregrade

Nova definiert folgende Severity-Level:

```text
Information
↓
Warning
↓
Recoverable
↓
Error
↓
Critical
↓
Fatal
```

Der Schweregrad bestimmt die Reaktion der Runtime.

---

# 9. Recoverable Errors

Recoverable Errors erlauben eine Fortsetzung.

Beispiele:

```text
Datei fehlt
↓
Alternative Quelle verwenden
```

```text
GPU nicht verfügbar
↓
CPU verwenden
```

Die Runtime darf automatische Korrekturen durchführen.

---

# 10. Fatal Errors

Fatal Errors verhindern die weitere Ausführung.

Beispiele:

```text
Beschädigter Capability Contract
Ungültige Signatur
Nicht auflösbare Abhängigkeiten
Beschädigte Zustände
```

Die Capability wird beendet.

---

# 11. Validierungsfehler

Validierungsfehler entstehen vor der Ausführung.

Beispiele:

```text
Ungültiger Parameter
Ungültiger Datentyp
Fehlender Parameter
Verletzung des Contracts
```

Diese Fehler sind vollständig reproduzierbar.

---

# 12. Laufzeitfehler

Runtime Errors entstehen während der Ausführung.

Beispiele:

```text
Speicher erschöpft
Division durch Null
GPU ausgefallen
Thread beendet
```

Die Runtime entscheidet über die Wiederherstellung.

---

# 13. Abhängigkeitsfehler

Dependency Errors entstehen durch fehlende oder inkompatible Capabilities.

Beispiele:

```text
Capability fehlt
Version inkompatibel
Dienst nicht verfügbar
Contract verletzt
```

Discovery kann diese Fehler häufig bereits vor der Ausführung erkennen.

---

# 14. Zustandsfehler

State Errors betreffen zustandsbehaftete Capabilities.

Beispiele:

```text
Beschädigter Zustand
Snapshot ungültig
Checkpoint fehlt
Inkonsistenter Zustand
```

Recovery-Mechanismen werden automatisch aktiviert.

---

# 15. Ressourcenfehler

Resource Errors beschreiben Ressourcenmangel.

Beispiele:

```text
RAM erschöpft
VRAM erschöpft
Dateisystem voll
GPU nicht verfügbar
Keine Netzwerkverbindung
```

Die Runtime kann alternative Implementierungen auswählen.

---

# 16. Sicherheitsfehler

Security Errors entstehen bei Sicherheitsverletzungen.

Beispiele:

```text
Ungültige Signatur
Manipulierte Capability
Berechtigung verweigert
Sandbox verletzt
```

Diese Fehler besitzen mindestens den Schweregrad **Error**.

---

# 17. Fehlerkontext

Jeder Fehler enthält Kontextinformationen.

Beispiele:

```text
Capability ID
Implementation
Version
Pipeline
Node
Benutzer
Parameter
Zeitpunkt
```

Der Kontext dient ausschließlich der Diagnose.

---

# 18. Fehlerursache

Fehler können verkettet werden.

```text
Storage Error
↓
Filesystem Error
↓
Hardware Error
```

Dadurch bleibt die vollständige Fehlerursache nachvollziehbar.

---

# 19. Fehlerweitergabe

Fehler werden entlang des Capability Graphen propagiert.

```text
Capability A
↓
Capability B
↓
Capability C
↓
Runtime
```

Jede Ebene darf den Fehler ergänzen, jedoch nicht verändern.

---

# 20. Fehlerbehandlung

Nova unterstützt folgende Strategien:

```text
Retry
Fallback
Rollback
Alternative Capability
Ignorieren
Abbruch
```

Die Strategie wird deklarativ definiert.

---

# 21. Fehlerdiagnose

Jeder Fehler enthält Diagnosedaten.

Beispiele:

```text
Stack Trace
Systemzustand
Hardware
Performance
Speicher
Log-ID
Snapshot-ID
```

Diagnosedaten unterstützen Entwickler und Administratoren.

---

# 22. Fehlerprotokollierung

Alle Fehler werden standardisiert protokolliert.

Erfasst werden:

- Zeit
- Capability
- Fehlercode
- Severity
- Ursache
- Recovery
- Ausführungsumgebung

Die Protokollierung erfolgt strukturiert und maschinenlesbar.

---

# 23. Benutzerdarstellung

Ein Fehler besitzt zwei Darstellungen.

## Technische Darstellung

Für Entwickler:

```text
Error Code
Stack
Diagnostics
Recovery
```

## Benutzerdarstellung

Für Anwender:

```text
Kurztitel
Beschreibung
Empfohlene Lösung
```

Interne Diagnosedetails dürfen Benutzern standardmäßig nicht angezeigt werden. Dies folgt etablierten Empfehlungen zur Trennung interner Diagnoseinformationen von externen Fehlermeldungen. :contentReference[oaicite:1]{index=1}

---

# 24. Fehlerereignisse

Fehler erzeugen standardisierte Events.

Beispiele:

```text
ErrorCreated
RecoveryStarted
RetryStarted
RecoverySucceeded
RecoveryFailed
CapabilityAborted
```

Diese Ereignisse können überwacht werden.

---

# 25. YAML-Beispiel

```yaml
error:
  id: err-6af3c1
  code: NOVA-DEP-0104
  category: dependency
  severity: recoverable
  title: Missing Capability
  description: Required capability not found.
  cause:
    capability: nova.image.decoder
  recovery:
    strategy: fallback
  diagnostics:
    implementation: nova.image.viewer
    version: 2.4.1
```

---

# 26. Validierungsregeln

Ein Fehlerobjekt gilt als gültig, wenn:

- eine Error ID vorhanden ist,
- ein stabiler Fehlercode existiert,
- genau eine Kategorie definiert ist,
- genau ein Schweregrad angegeben wird,
- Diagnosedaten strukturiert vorliegen,
- eine Recovery-Strategie angegeben werden kann,
- das Objekt maschinenlesbar ist.

---

# 27. Qualitätsanforderungen

Das Fehlermodell muss:

- vollständig standardisiert sein,
- reproduzierbare Fehler liefern,
- maschinenlesbar sein,
- Recovery unterstützen,
- strukturierte Diagnose ermöglichen,
- sichere Benutzerdarstellung erlauben,
- automatische Fehleranalyse unterstützen,
- langfristig kompatibel bleiben.

Einheitliche Fehlerobjekte und klar definierte Fehlerklassen erleichtern automatische Analyse, Wiederherstellung und zuverlässige Systemarchitekturen. :contentReference[oaicite:2]{index=2}

---

# 28. Konformitätskriterien

Eine Capability erfüllt diese Spezifikation, wenn:

- alle Fehler dem standardisierten Fehlerobjekt entsprechen,
- stabile Fehlercodes verwendet werden,
- Fehler eindeutig kategorisiert sind,
- Schweregrade definiert sind,
- Diagnosedaten bereitgestellt werden,
- Recovery-Strategien unterstützt werden,
- Fehler entlang des Capability Graphen propagiert werden können.

---

# 29. Zusammenfassung

Das Nova-Fehlermodell definiert eine einheitliche Architektur zur Beschreibung, Verarbeitung und Diagnose sämtlicher Fehler innerhalb der Capability Platform.

Durch standardisierte Fehlerobjekte, stabile Fehlercodes, klar definierte Kategorien, strukturierte Diagnosedaten und deklarative Recovery-Strategien können Capabilities Fehler konsistent behandeln und die Runtime automatische Maßnahmen wie Wiederholungen, Fallbacks oder Rollbacks durchführen.

Dadurch entsteht eine robuste, nachvollziehbare und langfristig wartbare Fehlerarchitektur für die gesamte Nova Platform.