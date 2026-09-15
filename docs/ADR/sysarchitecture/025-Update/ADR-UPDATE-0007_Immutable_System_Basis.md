# ADR-UPDATE-0007 – Immutable System Basis

## Status

Angenommen

## Kategorie

Update Architecture / System Integrity / Recovery

## Kontext

Die grundlegenden Komponenten von NovaOS müssen vor unbeabsichtigten oder unkontrollierten Veränderungen geschützt werden.

Klassische Systeme erlauben häufig direkte Änderungen an der laufenden Systeminstallation:

```text
Running System
     ↓
Modify Files
     ↓
New State
```

Dadurch können:

- Updates Mischzustände erzeugen,
- Anwendungen Systemdateien verändern,
- Konfigurationen unkontrolliert driften,
- Malware persistente Systemänderungen durchführen,
- Recovery-Zustände schwer reproduzierbar werden.

NovaOS benötigt deshalb eine unveränderliche Systembasis.

## Entscheidung

NovaOS verwendet eine **Immutable System Basis**.

Die aktive Systembasis wird im normalen Betrieb nicht direkt verändert.

```text
Active System Basis
        ↓
     Read-Only
```

Änderungen erfolgen durch Erzeugung beziehungsweise Vorbereitung eines neuen versionierten Systemzustands.

```text
System State N
      ↓
Prepare N+1
      ↓
Verify
      ↓
Atomic Activation
      ↓
System State N+1
```

## Grundprinzip

```text
Running system state is consumed,
not modified in place.
```

Die aktive Systembasis ist ein verifizierter Zustand und kein frei veränderbarer Installationsbaum.

## Systembasis

Zur Systembasis gehören insbesondere kritische Komponenten wie:

```text
Kernel
Core Drivers
Core Services
System Libraries
Runtime Components
System Capabilities
Boot-Relevant Components
```

Welche Komponenten konkret Bestandteil der Systembasis sind, wird durch eine NPSPEC definiert.

## Read-Only

Die aktive Systembasis wird im normalen Betrieb logisch schreibgeschützt bereitgestellt.

```text
System Basis
    ↓
Read-Only
```

Normale Prozesse und Anwendungen dürfen die aktive Basis nicht direkt verändern.

Auch privilegierte Systemoperationen sollen Änderungen nicht durch direktes Überschreiben durchführen.

## Veränderbarer Zustand

Nicht alle Daten des Systems sind immutable.

Veränderbarer Zustand wird von der Systembasis getrennt.

```text
Immutable System Basis
          +
Mutable System State
          +
User Data
```

Mutable Daten können beispielsweise umfassen:

```text
Runtime State
User Configuration
System Configuration
Logs
Caches
Databases
User Data
```

Die genaue Trennung wird explizit spezifiziert.

## Updates

Ein Update verändert nicht die aktive Systembasis.

Stattdessen wird ein neuer Zustand vorbereitet.

```text
Active Basis N
      │
      └──── remains unchanged

Candidate Basis N+1
      ↓
Prepare
      ↓
Verify
      ↓
Activate
```

Dies entspricht der Transactional Update Pipeline.

## Versionierung

Jede Systembasis muss einem eindeutig identifizierbaren Softwarezustand zugeordnet werden können.

```text
System Basis 41
System Basis 42
System Basis 43
```

Damit können:

- Updates,
- Rollbacks,
- Recovery,
- Audit,
- Reproduktion

auf klar definierte Zustände verweisen.

## Content Addressing

Die Systembasis kann aus content-addressed Artefakten aufgebaut werden.

```text
System State
 ├── Kernel  → H1
 ├── Driver  → H2
 ├── Service → H3
 └── Library → H4
```

Dadurch können konkrete Systeminhalte eindeutig identifiziert und verifiziert werden.

## A/B-Systembasis

Die Immutable System Basis ergänzt das A/B-Modell.

```text
Base A
Immutable
Known-Good

Base B
Immutable
Candidate
```

Ein Update wird auf der inaktiven Basis vorbereitet und anschließend kontrolliert aktiviert.

Die aktive Basis muss dafür nicht verändert werden.

## Snapshot-Based Rollback

Für geeignete Zustände können Snapshots verwendet werden.

```text
Immutable Base
      +
Versioned Mutable State
      ↓
Snapshot
```

Systembasis und veränderbarer Zustand müssen dabei getrennt behandelt werden können.

## Konfiguration

Systemkonfiguration darf nicht durch beliebiges Editieren der Immutable Basis erfolgen.

Konfigurationsänderungen werden über den deklarativen und transaktionalen Konfigurationsmechanismus verwaltet.

```text
Configuration Intent
        ↓
Desired State
        ↓
Transaction
        ↓
Mutable Configuration State
```

Falls eine Konfiguration Bestandteil der Systembasis sein muss, erzeugt ihre Änderung einen neuen Systemzustand.

## Anwendungen

Anwendungen dürfen keine Dateien innerhalb der Immutable System Basis installieren oder verändern.

Sie verwenden stattdessen:

```text
Application Storage
Capabilities
Declared Dependencies
Content-Addressed Packages
User Data
```

Dadurch bleiben Anwendungen vom internen Layout der Systembasis entkoppelt.

## Systembibliotheken

Systembibliotheken innerhalb der Immutable Basis werden nicht durch Anwendungen ersetzt.

```text
Application
    ↓
Required Capability / Runtime
    ↓
System Resolver
```

Eine Anwendung darf keine zentrale Systembibliothek direkt überschreiben.

## Temporäre Änderungen

Temporäre Laufzeitänderungen dürfen die persistente Immutable Basis nicht verändern.

```text
Immutable Basis
      ↓
Runtime Overlay
```

Ein optionaler Overlay-Mechanismus kann für klar definierte temporäre Zustände verwendet werden.

Ein Overlay darf jedoch nicht stillschweigend zu dauerhaftem Systemzustand werden.

## Developer Mode

Entwicklungs- und Debugging-Funktionen dürfen die Immutable-Systemgarantie nicht unkontrolliert aufheben.

Falls NovaOS einen Developer Mode bereitstellt, müssen Abweichungen explizit sichtbar und isoliert sein.

```text
Normal Mode
    ↓
Immutable Guarantee

Developer Mode
    ↓
Explicit Controlled Exception
```

Eine modifizierte Entwicklungsbasis darf nicht automatisch als vertrauenswürdige Known-Good-Basis gelten.

## Integrität

Die Immutable Basis muss auf Integrität überprüfbar sein.

```text
System Basis
     ↓
Integrity Verification
     ↓
Valid / Invalid
```

Ungeplante Veränderungen an einem immutable Artefakt sind ein Integrity Failure.

## Trust

Unveränderlichkeit bedeutet nicht automatisch Vertrauen.

```text
Immutable
    ≠
Trusted
```

Die Systembasis muss weiterhin die geltenden:

```text
Signature
Provenance
Trust
Verified Boot
Measured Boot
```

Anforderungen erfüllen.

## Verified Boot

Die Immutable System Basis bildet eine geeignete Grundlage für Verified Boot.

```text
Boot
 ↓
Verify System Basis
 ↓
Start
```

Die gestartete Basis kann dadurch gegen den erwarteten Systemzustand geprüft werden.

## Self-Healing

Nova.Resilience kann Abweichungen von der erwarteten Systembasis erkennen.

```text
Expected Immutable State
          ↓
Integrity Check
          ↓
Deviation
          ↓
Recovery
```

Da die Basis nicht regulär verändert wird, ist eine unerwartete Veränderung ein starkes Fehlersignal.

## Recovery

Bei beschädigter Systembasis soll NovaOS nicht versuchen, unbekannte Einzeländerungen blind zu reparieren.

Bevorzugt wird die Wiederherstellung eines bekannten gültigen Zustands.

```text
Corrupted Basis
      ↓
Known-Good State
      ↓
Restore / Switch
      ↓
Verify
```

Dies kann über:

- A/B-Fallback,
- Snapshot,
- Content Store,
- Recovery Environment

erfolgen.

## NovaDOS

NovaDOS muss die Immutable System Basis offline prüfen können.

Mögliche Operationen:

```text
Inspect
Verify
Compare
Select Known-Good
Restore
Repair References
```

NovaDOS soll bevorzugt bekannte verifizierte Artefakte wiederherstellen, statt unbekannte beschädigte Dateien manuell zu verändern.

## Mutable State Drift

Veränderlicher Systemzustand kann weiterhin von seinem gewünschten Zustand abweichen.

```text
Desired Mutable State
        ↓
Compare
        ↓
Actual Mutable State
        ↓
Reconciliation
```

Die Immutable Basis verhindert daher nicht jede Form von State Drift, reduziert sie aber auf explizit veränderbare Bereiche.

## Security

Schreibzugriffe auf die Systembasis dürfen nicht als normaler administrativer Dateizugriff behandelt werden.

```text
Administrator
     ≠
Arbitrary System Mutation
```

Systemänderungen müssen über die dafür vorgesehenen Update- und Transaction-Mechanismen erfolgen.

Dies reduziert die dauerhafte Angriffsfläche.

## Malware Persistence

Eine Anwendung oder kompromittierter Prozess darf keine persistente Änderung der aktiven Systembasis erzeugen können.

Persistenzversuche müssen auf explizit veränderbare Bereiche begrenzt und durch die Security-Architektur kontrolliert werden.

Die Immutable Basis ersetzt jedoch keine Malware-Erkennung oder Capability-Sicherheit.

## Garbage Collection

Alte Systemzustände können entfernt werden, wenn sie nicht mehr benötigt werden für:

```text
Active State
Candidate State
Known-Good State
Rollback
Recovery
Retention
Audit
```

Referenzierte Content-Artefakte dürfen nicht vorzeitig entfernt werden.

## Normative Anforderungen

1. NovaOS MUSS eine logisch unveränderliche aktive Systembasis unterstützen.
2. Die aktive Systembasis DARF im normalen Betrieb nicht direkt verändert werden.
3. Systemupdates MÜSSEN durch Vorbereitung eines neuen Systemzustands erfolgen.
4. Systembasis und veränderbarer Zustand MÜSSEN logisch getrennt sein.
5. Benutzerdaten DÜRFEN nicht Bestandteil der Immutable System Basis sein.
6. Jede Systembasis MUSS eindeutig versionierbar beziehungsweise identifizierbar sein.
7. Die Systembasis MUSS aus eindeutig identifizierbaren Artefakten zusammensetzbar sein.
8. Anwendungen DÜRFEN keine Bestandteile der aktiven Systembasis überschreiben.
9. Anwendungen DÜRFEN zentrale Systembibliotheken nicht direkt ersetzen.
10. Konfigurationsänderungen MÜSSEN über definierte State- und Transaction-Mechanismen erfolgen.
11. Temporäre Overlays DÜRFEN die persistente Basis nicht stillschweigend verändern.
12. Developer-Ausnahmen MÜSSEN explizit und isoliert sein.
13. Eine modifizierte Entwicklungsbasis DARF nicht automatisch als Known-Good gelten.
14. Die Systembasis MUSS auf Integrität prüfbar sein.
15. Ungeplante Veränderungen MÜSSEN als Integrity Failure behandelbar sein.
16. Immutable DARF nicht mit Trusted gleichgesetzt werden.
17. Die Systembasis MUSS mit Verified Boot und Measured Boot integrierbar sein.
18. Recovery SOLL bekannte gültige Systemzustände gegenüber blindem In-Place-Repair bevorzugen.
19. A/B-Systembasis und Snapshot-Based Rollback MÜSSEN mit der Immutable Basis kombinierbar sein.
20. Schreibzugriff auf Systemdateien DARF keine normale administrative Änderungsmethode sein.
21. Nova.Resilience MUSS die erwartete Immutable Basis als Recovery-Evidenz verwenden können.
22. NovaDOS MUSS kritische Systembasen offline verifizieren und recovern können.
23. Alte Systemzustände DÜRFEN nicht entfernt werden, solange sie für notwendige Recovery benötigt werden.
24. Die Immutable-Systemarchitektur DARF Security-, Trust- oder Capability-Prüfungen nicht ersetzen.

## Konsequenzen

### Positive Konsequenzen

- kein unkontrolliertes Überschreiben der laufenden Systembasis,
- Updates erzeugen reproduzierbare Systemzustände,
- State Drift wird reduziert,
- Rollback und Recovery werden einfacher,
- Malware-Persistenz innerhalb der Systembasis wird erschwert,
- Verified Boot kann auf klar definierten Zuständen arbeiten,
- Systemzustände können exakt versioniert und verglichen werden.

### Negative Konsequenzen

- System- und Mutable State müssen konsequent getrennt werden,
- klassische Installationsprogramme können nicht direkt in Systemverzeichnisse schreiben,
- bestimmte Änderungen benötigen einen neuen Systemzustand,
- Legacy-Software kann Kompatibilitätsmechanismen benötigen.

## Verworfene Alternativen

### Frei beschreibbare Systembasis

Verworfen, da dadurch State Drift, Mischzustände und schwer reproduzierbare Systeme entstehen.

### Root-Zugriff erlaubt direkte Systemänderungen

Verworfen, da Authority nicht automatisch unkontrollierte Mutation der Systembasis bedeuten soll.

### Immutable nur während des Bootvorgangs

Verworfen, da auch während des laufenden Betriebs persistente Veränderungen verhindert werden müssen.

### Jede Konfiguration in der Immutable Basis

Verworfen, da häufig veränderbare Konfiguration sonst unnötig neue Systembasen erzeugen würde.

### Immutable Basis ersetzt Security

Verworfen, da unveränderliche, aber bösartige oder nicht vertrauenswürdige Software weiterhin ein Sicherheitsproblem darstellt.

## Abhängigkeiten

- `ADR-UPDATE-0001_Deklarativer_Softwarezustand`
- `ADR-UPDATE-0002_Transactional_Update_Pipeline`
- `ADR-UPDATE-0003_Atomic_Updates`
- `ADR-UPDATE-0004_A_B_Systembasis`
- `ADR-UPDATE-0005_Snapshot_Based_Rollback`
- `ADR-UPDATE-0006_Content_Addressed_Packages`
- `ADR-STORAGE-0003_Content_Addressed_Storage`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-STORAGE-0014_Snapshots_und_Rollback`
- `ADR-STORAGE-0015_Checksumming_für_Integrität`
- `ADR-TRUST-0003_Signatur_ist_nicht_Vertrauen`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`
- `ADR-BOOT-0006_Verified_Boot`
- `ADR-BOOT-0007_Measured_Boot`
- `ADR-RESILIENCE-0022_NovaDOS_für_Offline_Recovery`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-IMMUTABLE-BASE-0001`
- `NPSPEC-UPDATE-SYSTEM-MUTABLE-SEPARATION-0001`
- `NPSPEC-UPDATE-SYSTEM-STATE-0001`
- `NPSPEC-UPDATE-IMMUTABLE-OVERLAY-0001`
- `NPSPEC-UPDATE-IMMUTABLE-INTEGRITY-0001`
- `NPSPEC-UPDATE-IMMUTABLE-RECOVERY-0001`
- `NPSPEC-UPDATE-IMMUTABLE-DEVELOPER-MODE-0001`
- `NPSPEC-UPDATE-IMMUTABLE-BASE-TEST-0001`

## Ergebnis

NovaOS behandelt die laufende Systembasis als unveränderlichen, versionierten Zustand:

```text
Immutable System Basis N
          ↓
       Running
          │
          │ Update does not modify N
          ↓
Prepare System Basis N+1
          ↓
Verify
          ↓
Atomic Activation
          ↓
Immutable System Basis N+1
```

Veränderliche Konfigurationen, Laufzeitdaten und Benutzerdaten bleiben davon getrennt.

Der zentrale Grundsatz lautet:

```text
NovaOS verändert nicht
die laufende Systembasis.

NovaOS erzeugt einen neuen
verifizierten Systemzustand
und wechselt atomar auf ihn.
```