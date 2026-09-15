# ADR-UPDATE-0008 – Side-by-Side-Versionen

## Status

Angenommen

## Kategorie

Update Architecture / Version Management / Compatibility

## Kontext

NovaOS muss neue Softwareversionen bereitstellen können, ohne bestehende Anwendungen oder laufende Prozesse sofort auf diese Version umzustellen.

Ein direktes Ersetzen gemeinsam verwendeter Komponenten kann Kompatibilitätsprobleme erzeugen:

```text
Application A → Runtime V1
Application B → Runtime V1

Update Runtime → V2

Application A → kompatibel
Application B → inkompatibel
```

Dies betrifft insbesondere:

- Runtimes,
- Systembibliotheken,
- Capability Provider,
- Frameworks,
- Anwendungen,
- Entwicklerwerkzeuge.

NovaOS benötigt deshalb die Möglichkeit, mehrere Versionen derselben Komponente gleichzeitig bereitzuhalten.

## Entscheidung

NovaOS unterstützt **Side-by-Side-Versionen**.

```text
Component
 ├── Version 1
 ├── Version 2
 └── Version 3
```

Mehrere Versionen derselben Komponente können gleichzeitig vorhanden und verwendbar sein.

Consumer erhalten über den Resolver eine zu ihren Anforderungen passende konkrete Version.

## Grundprinzip

```text
Install new version
      ≠
Remove old version
```

Die Bereitstellung einer neuen Version verändert bestehende Versionsbindungen nicht automatisch.

## Versionsidentität

Jede konkrete Version muss eindeutig identifizierbar sein.

```text
Component
Version
Content ID
```

Die konkrete Artefaktidentität erfolgt über Content Addressing.

```text
Runtime V1 → H1
Runtime V2 → H2
```

## Versionsanforderungen

Consumer deklarieren ihre Versionsanforderungen.

Beispiele:

```text
Runtime >= 2
Runtime = 2.4
Runtime >= 2 && < 3
```

Der Resolver bestimmt daraus eine konkrete kompatible Version.

## Resolver

Die Auflösung erfolgt grundsätzlich:

```text
Consumer Requirement
        ↓
Available Versions
        ↓
Compatibility Rules
        ↓
Resolver
        ↓
Concrete Version
```

Nach der Auflösung kann die konkrete Content ID im Execution- oder Software-State festgehalten werden.

## Exakte Bindung

Falls Reproduzierbarkeit oder Kompatibilität dies erfordert, kann ein Consumer an eine konkrete Version beziehungsweise Content ID gebunden werden.

```text
Application A
     ↓
Runtime V2
     ↓
Content ID H42
```

Dadurch bleibt die verwendete Implementierung eindeutig bestimmt.

## Mehrere Consumer

Unterschiedliche Consumer können gleichzeitig unterschiedliche Versionen verwenden.

```text
Application A → Runtime V1
Application B → Runtime V2
Application C → Runtime V3
```

Eine Aktualisierung eines Consumers zwingt andere Consumer nicht automatisch zu einem Versionswechsel.

## Laufende Prozesse

Bereits laufende Prozesse dürfen ihre verwendete Version grundsätzlich weiterverwenden.

```text
Process A
   ↓
Runtime V1

Update
   ↓
Runtime V2 available

Process A → V1
New Process B → V2
```

Dadurch kann ein Update erfolgen, ohne laufende Prozesse unnötig zu unterbrechen.

## Neue Prozesse

Neue Prozesse können nach einem Update standardmäßig die neu bevorzugte Version erhalten, sofern ihre Contracts dies erlauben.

```text
Available:
V1
V2

Preferred:
V2
```

Bestehende Bindungen bleiben davon unberührt.

## Preferred Version

NovaOS kann eine Version als bevorzugte Version markieren.

```text
Runtime
 ├── V1
 ├── V2
 └── V3 ← Preferred
```

`Preferred` bedeutet nicht:

```text
Older Versions = Invalid
```

Die bevorzugte Version dient lediglich als Resolver-Präferenz für kompatible neue Bindungen.

## Update

Ein Update kann eine neue Version zunächst zusätzlich bereitstellen.

```text
V1
 ↓
Install V2
 ↓
V1 + V2
 ↓
Validate V2
 ↓
V2 Preferred
```

Die alte Version bleibt erhalten, solange sie noch benötigt wird.

## Atomic Activation

Die Änderung der Preferred Version oder einer konkreten Versionsbindung muss atomar erfolgen können.

```text
Preferred = V1
      ↓
Atomic Change
      ↓
Preferred = V2
```

Consumer dürfen keinen undefinierten Zwischenzustand beobachten.

## Rollback

Ein Rollback kann durch erneute Auswahl einer vorherigen Version erfolgen.

```text
Preferred V1
     ↓
Preferred V2
     ↓
Failure
     ↓
Preferred V1
```

Voraussetzung ist, dass die vorherige Version und ihre benötigten Abhängigkeiten noch verfügbar sind.

## Content-Addressed Packages

Side-by-Side-Versionen werden mit Content-Addressed Packages kombiniert.

```text
Component V1 → H1
Component V2 → H2
Component V3 → H3
```

Dadurch können Versionen unveränderlich und eindeutig referenziert werden.

Identische Inhalte müssen nicht mehrfach physisch gespeichert werden.

## Immutable System Basis

Side-by-Side-Versionen verändern die aktive Immutable System Basis nicht direkt.

Neue Versionen werden als neue Artefakte beziehungsweise als Bestandteil eines neuen Softwarezustands bereitgestellt.

```text
Immutable State N
      ↓
Prepare new artifacts
      ↓
State N+1
```

## Capability Provider

Mehrere Versionen eines Capability Providers können gleichzeitig vorhanden sein.

```text
Capability X
 ├── Provider V1
 ├── Provider V2
 └── Provider V3
```

Der Capability Resolver wählt anhand von:

```text
Capability Contract
Version Requirement
Trust Requirement
Execution Contract
Compatibility
Policy
```

eine geeignete Version.

## Systembibliotheken

Anwendungen dürfen nicht davon ausgehen, dass genau eine globale Version einer Bibliothek existiert.

```text
Application A → Library V1
Application B → Library V2
```

Dadurch wird das klassische Problem global erzwungener Bibliotheksversionen reduziert.

## Abhängigkeitsgraph

Jeder Softwarezustand kann einen konkreten aufgelösten Abhängigkeitsgraph besitzen.

```text
Application A
 ├── Runtime V2 → H2
 └── Library V4 → H7
```

Damit bleibt nachvollziehbar, welche konkrete Version tatsächlich verwendet wird.

## ABI-Kompatibilität

Versionsnummern allein bestimmen keine ABI-Kompatibilität.

Provider und Bibliotheken müssen ihre relevanten Kompatibilitätsinformationen deklarieren können.

```text
Version
ABI
API
Capability Contract
```

Der Resolver darf eine Version nicht allein aufgrund einer höheren Versionsnummer als kompatibel betrachten.

## Major-Versionen

Inkompatible Major-Versionen können vollständig parallel existieren.

```text
Runtime 1.x
Runtime 2.x
Runtime 3.x
```

Dadurch müssen Anwendungen nicht gleichzeitig migriert werden.

## Security Updates

Side-by-Side-Versionen dürfen nicht dazu führen, dass bekannte unsichere Versionen unbegrenzt verwendet werden.

Eine Version kann beispielsweise klassifiziert werden als:

```text
Supported
Deprecated
Blocked
Revoked
```

Eine durch Security Policy gesperrte Version darf nicht allein aufgrund einer bestehenden Versionsanforderung weiter ausgewählt werden.

## Revocation

Eine Version kann systemweit gesperrt werden, wenn ihre weitere Verwendung nicht vertretbar ist.

```text
Version V1
   ↓
Revoked
   ↓
Resolver must not select V1
```

Bereits laufende Instanzen werden entsprechend der Security- und Recovery-Policy behandelt.

## Deprecation

Eine veraltete Version kann zunächst als `Deprecated` markiert werden.

```text
V1 = Deprecated
V2 = Preferred
```

Bestehende Consumer können dadurch weiter funktionieren, während neue Bindungen bevorzugt auf V2 erfolgen.

## Garbage Collection

Eine alte Version darf entfernt werden, wenn keine relevante Referenz mehr darauf besteht.

Zu berücksichtigen sind mindestens:

```text
Running Processes
Desired State
Active State
Snapshots
Rollback States
A/B Bases
Recovery
Pinned Dependencies
Retention
```

Erst danach darf das Artefakt für Garbage Collection freigegeben werden.

## Referenzen

Die Lebensdauer einer Version darf nicht ausschließlich von sichtbaren Anwendungen abhängen.

Auch folgende Zustände können eine Version benötigen:

```text
Rollback
Snapshot
Recovery
Transaction
Known-Good State
```

Diese Referenzen müssen vor Entfernung berücksichtigt werden.

## Ressourcen

Side-by-Side-Versionen können zusätzlichen Storage und gegebenenfalls zusätzlichen Arbeitsspeicher benötigen.

NovaOS soll identische Inhalte über Content Addressing deduplizieren können.

Nicht mehr benötigte Versionen sollen kontrolliert entfernt werden.

## Security

Die parallele Existenz mehrerer Versionen darf keine Security-Grenze umgehen.

Jede Version unterliegt weiterhin:

```text
Integrity
Trust
Provenance
Authorization
Security Policy
```

Eine installierte Version ist nicht automatisch zur Ausführung autorisiert.

## Trust

Unterschiedliche Versionen desselben Components können unterschiedliche Trust-Evidenz besitzen.

```text
V1 → Trust Evidence A
V2 → Trust Evidence B
```

Der Resolver muss die für den jeweiligen Execution Contract erforderlichen Trust-Anforderungen berücksichtigen.

## Recovery

Recovery muss konkrete Versionsbindungen rekonstruieren können.

```text
Recovery State
     ↓
Dependency Graph
     ↓
Content IDs
     ↓
Restore exact versions
```

Dadurch kann ein vorheriger Softwarezustand reproduzierbar wiederhergestellt werden.

## NovaDOS

NovaDOS muss für kritische Systemkomponenten erkennen können, welche Versionen zu:

```text
Active State
Candidate State
Known-Good State
Rollback State
```

gehören.

Notwendige Recovery-Versionen dürfen nicht durch normale Garbage Collection entfernt werden.

## Normative Anforderungen

1. NovaOS MUSS mehrere Versionen derselben Softwarekomponente gleichzeitig unterstützen können.
2. Die Installation einer neuen Version DARF eine bestehende Version nicht automatisch entfernen.
3. Consumer MÜSSEN Versionsanforderungen deklarieren können.
4. Der Resolver MUSS Anforderungen auf konkrete Versionen auflösen können.
5. Konkrete Versionen MÜSSEN über Content IDs eindeutig referenzierbar sein.
6. Unterschiedliche Consumer MÜSSEN unterschiedliche Versionen derselben Komponente verwenden können.
7. Laufende Prozesse SOLLEN ihre bestehende Versionsbindung während ihrer Lebensdauer behalten können.
8. Neue Prozesse DÜRFEN eine neuere Preferred Version verwenden, sofern ihre Contracts dies erlauben.
9. Preferred Version und konkrete Versionsbindung MÜSSEN getrennt behandelt werden.
10. Die Änderung einer Preferred Version MUSS atomar erfolgen können.
11. Rollback MUSS auf eine vorherige noch verfügbare Version zurückschalten können.
12. Side-by-Side-Versionen MÜSSEN mit Content-Addressed Packages integrierbar sein.
13. Side-by-Side-Versionen MÜSSEN mit der Immutable System Basis kompatibel sein.
14. Capability Provider MÜSSEN parallel in mehreren Versionen existieren können.
15. Versionsnummern DÜRFEN nicht allein als Nachweis von API- oder ABI-Kompatibilität gelten.
16. Inkompatible Major-Versionen MÜSSEN parallel existieren können.
17. Security Policies MÜSSEN Versionen als Deprecated, Blocked oder Revoked kennzeichnen können.
18. Revoked Versions DÜRFEN vom Resolver nicht neu ausgewählt werden.
19. Garbage Collection DARF keine noch referenzierte Version entfernen.
20. Rollback-, Snapshot-, A/B- und Recovery-Referenzen MÜSSEN vor Garbage Collection berücksichtigt werden.
21. Side-by-Side-Versionen MÜSSEN den normalen Integrity-, Trust- und Provenance-Regeln unterliegen.
22. Recovery MUSS konkrete frühere Versionsbindungen wiederherstellen können.
23. Kritische Recovery-Versionen MÜSSEN für NovaDOS identifizierbar sein.
24. Side-by-Side-Versionen DÜRFEN keine globale erzwungene Aktualisierung unabhängiger Consumer verursachen.

## Konsequenzen

### Positive Konsequenzen

- Anwendungen können schrittweise auf neue Versionen migrieren,
- Updates brechen ältere Consumer nicht automatisch,
- mehrere ABI-Generationen können parallel existieren,
- laufende Prozesse müssen bei vielen Updates nicht beendet werden,
- Rollback wird durch vorhandene ältere Versionen vereinfacht,
- reproduzierbare Abhängigkeitsgraphen werden möglich.

### Negative Konsequenzen

- mehrere Versionen benötigen zusätzlichen Storage,
- Resolver und Garbage Collection werden komplexer,
- Security Updates benötigen Regeln für alte verwundbare Versionen,
- Abhängigkeitsgraphen müssen konkrete Versionsbindungen verwalten.

## Verworfene Alternativen

### Genau eine globale Version jeder Komponente

Verworfen, da dadurch alle Consumer gleichzeitig kompatibel sein müssten.

### Neue Version ersetzt alte sofort

Verworfen, da laufende oder inkompatible Consumer dadurch beschädigt werden könnten.

### Versionsauflösung ausschließlich nach höchster Versionsnummer

Verworfen, da eine höhere Version nicht automatisch kompatibel oder vertrauenswürdig ist.

### Alte Versionen dauerhaft behalten

Verworfen, da dies unnötigen Storage verbraucht und unsichere Altversionen erhalten könnte.

### Side-by-Side nur für Anwendungen

Verworfen, da auch Runtimes, Bibliotheken und Capability Provider unterschiedliche Versionsanforderungen besitzen können.

## Abhängigkeiten

- `ADR-UPDATE-0001_Deklarativer_Softwarezustand`
- `ADR-UPDATE-0002_Transactional_Update_Pipeline`
- `ADR-UPDATE-0003_Atomic_Updates`
- `ADR-UPDATE-0004_A_B_Systembasis`
- `ADR-UPDATE-0005_Snapshot_Based_Rollback`
- `ADR-UPDATE-0006_Content_Addressed_Packages`
- `ADR-UPDATE-0007_Immutable_System_Basis`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-STORAGE-0003_Content_Addressed_Storage`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-TRUST-0003_Signatur_ist_nicht_Vertrauen`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-SIDEBYSIDE-0001`
- `NPSPEC-UPDATE-VERSION-RESOLVER-0001`
- `NPSPEC-UPDATE-VERSION-BINDING-0001`
- `NPSPEC-UPDATE-PREFERRED-VERSION-0001`
- `NPSPEC-UPDATE-VERSION-COMPATIBILITY-0001`
- `NPSPEC-UPDATE-VERSION-LIFECYCLE-0001`
- `NPSPEC-UPDATE-VERSION-REVOCATION-0001`
- `NPSPEC-UPDATE-VERSION-GC-0001`
- `NPSPEC-UPDATE-SIDEBYSIDE-RECOVERY-0001`
- `NPSPEC-UPDATE-SIDEBYSIDE-TEST-0001`

## Ergebnis

NovaOS erlaubt mehreren Versionen derselben Komponente, kontrolliert nebeneinander zu existieren:

```text
Component
 ├── V1 → Consumer A
 ├── V2 → Consumer B
 └── V3 → New Consumers
```

Neue Versionen ersetzen alte Versionen nicht automatisch.

Stattdessen entscheidet der Resolver anhand von Version, Kompatibilität, Content ID, Trust und Contract über die konkrete Bindung.

Der zentrale Grundsatz lautet:

```text
Eine neue Version
wird zuerst zusätzlich verfügbar.

Erst wenn eine alte Version
nicht mehr benötigt wird,
kann sie entfernt werden.
```