# ADR-RESILIENCE-0022 – NovaDOS für Offline Recovery

## Status

Angenommen

## Kategorie

Resilience Architecture / Offline Recovery / NovaDOS

## Kontext

Nicht jeder Fehler kann innerhalb des laufenden NovaOS sicher behoben werden.

Bestimmte Fehler können Komponenten betreffen, die für die normale Recovery selbst benötigt werden, beispielsweise:

- Kernel,
- Boot-Komponenten,
- System-Storage,
- zentrale Systembibliotheken,
- Identity- oder Trust-Infrastruktur,
- Capability Registry,
- kritische Treiber,
- Dateisysteme,
- Recovery-Komponenten selbst.

In solchen Situationen kann eine Recovery innerhalb des laufenden Systems unzuverlässig, unmöglich oder sicherheitstechnisch nicht vertretbar sein.

NovaOS besitzt mit **NovaDOS** eine unabhängige, minimale Wartungs- und Recovery-Umgebung, die bereits über den Bootmanager gestartet werden kann.

Nova.Resilience benötigt deshalb einen definierten Übergang von Online Recovery zu NovaDOS-basierter Offline Recovery.

## Entscheidung

NovaOS verwendet **NovaDOS als primäre Offline-Recovery-Umgebung**, wenn eine sichere Recovery innerhalb des laufenden NovaOS nicht mehr gewährleistet werden kann.

```text
NovaOS Failure
      ↓
Online Recovery possible?
   ├── Yes → Nova.Resilience
   └── No
        ↓
   Preserve Recovery Context
        ↓
      NovaDOS
        ↓
   Offline Recovery
        ↓
      Verify
        ↓
     NovaOS Boot
```

NovaDOS ist damit Bestandteil der systemweiten Nova.Resilience-Architektur und kein separates, unabhängiges Reparatursystem.

## Grundprinzip

```text
Recover online when safe.
Recover offline when necessary.
```

Offline Recovery wird nur verwendet, wenn der laufende Systemzustand keine ausreichend vertrauenswürdige oder technisch geeignete Recovery-Umgebung mehr darstellt.

## Einsatzbedingungen

Nova.Resilience kann NovaDOS Recovery insbesondere vorsehen bei:

```text
Boot Failure
Kernel Corruption
Critical Filesystem Failure
System Storage Corruption
Critical Driver Failure
Recovery Infrastructure Failure
Persistent Boot Loop
Critical Integrity Failure
Unrecoverable System State
```

Die konkrete Eskalation hängt von Failure Class, Recovery Contract und Systemzustand ab.

## Recovery Scope

Der Wechsel zu NovaDOS bedeutet nicht automatisch eine vollständige Systemwiederherstellung.

Auch Offline Recovery folgt dem Prinzip des kleinsten sicheren Recovery Scope.

Beispiele:

```text
Repair Boot Metadata
Repair Filesystem
Restore System Component
Rollback System Transaction
Restore Checkpoint
Replace Corrupted Driver
Verify System Integrity
```

Eine vollständige Neuinstallation ist nur eine mögliche letzte Recovery-Strategie.

## Recovery Context

Vor dem Übergang zu NovaDOS soll Nova.Resilience, soweit noch sicher möglich, einen minimalen Recovery Context persistieren.

Dieser kann beispielsweise enthalten:

```text
Failure Classification
Affected Failure Domain
Recovery Scope
Recovery Contract
Relevant Checkpoint
Pending Transaction
Integrity Evidence
Requested Recovery Action
Verification Requirements
```

NovaDOS darf diesen Context nicht ungeprüft als vertrauenswürdig behandeln.

## Übergabe

Der Bootmanager stellt den kontrollierten Übergang zwischen NovaOS und NovaDOS bereit.

```text
NovaOS
   ↓
Recovery Escalation
   ↓
Boot Manager
   ↓
NovaDOS
```

Der gewünschte Recovery-Modus kann über geschützte Boot-/Recovery-Metadaten übergeben werden.

## Unabhängigkeit

NovaDOS muss ausreichend unabhängig von den Komponenten sein, die es reparieren soll.

Insbesondere darf eine Recovery kritischer NovaOS-Komponenten nicht zwingend deren funktionsfähige Ausführung voraussetzen.

```text
Broken NovaOS Component
        ↓
Must not be required
        ↓
NovaDOS Recovery
```

Dadurch bleibt Recovery auch bei schwer beschädigten Systemkomponenten möglich.

## Minimalität

NovaDOS soll nur die Funktionen enthalten, die für Diagnose, Wartung und Offline Recovery erforderlich sind.

Ein kleinerer Recovery Stack reduziert:

- Abhängigkeiten,
- Fehlerfläche,
- Angriffsfläche,
- Boot-Komplexität,
- Wahrscheinlichkeit gemeinsamer Fehler mit NovaOS.

NovaDOS ist deshalb kein zweites vollständiges NovaOS.

## Storage und Dateisysteme

NovaDOS muss auf relevante System-Volumes und unterstützte Dateisysteme zugreifen können.

Offline Recovery kann beispielsweise umfassen:

```text
Filesystem Verification
Filesystem Repair
Volume Inspection
System File Restoration
Snapshot / Checkpoint Restore
Transaction Recovery
```

Schreibende Operationen dürfen nur erfolgen, wenn die dafür notwendigen Recovery- und Integrity-Bedingungen erfüllt sind.

## System-Transaktionen

Nicht abgeschlossene transaktionale Systemoperationen müssen durch NovaDOS analysierbar sein können.

```text
Interrupted Transaction
        ↓
NovaDOS
        ↓
Inspect Transaction State
        ↓
Complete / Rollback / Reconcile
```

NovaDOS darf eine unbekannte Transaktion nicht blind zurückrollen, wenn deren Operationssemantik dies nicht erlaubt.

## Semantic Checkpoints

NovaDOS kann Semantic Checkpoints für Offline Recovery verwenden.

```text
Checkpoint
    ↓
Restore
    ↓
Validate Invariants
    ↓
System Verification
```

Die Wiederherstellung eines Checkpoints gilt erst nach erfolgreicher Verification als abgeschlossen.

## Security Failures

Wurde Offline Recovery aufgrund eines Security- oder Integrity-Failures ausgelöst, muss NovaDOS den Quarantine- und Trust-Kontext berücksichtigen.

Ein kompromittierter Zustand darf nicht allein durch Neustart wieder als vertrauenswürdig gelten.

```text
Security Failure
      ↓
NovaDOS
      ↓
Offline Remediation
      ↓
Integrity / Trust Verification
      ↓
Boot Authorization
```

## Trust

NovaDOS selbst gehört zur vertrauenswürdigen Recovery-Basis von NovaOS.

Seine Integrität muss vor sicherheitskritischer Recovery ausreichend verifizierbar sein.

Eine kompromittierte Recovery-Umgebung darf nicht verwendet werden, um einen neuen vertrauenswürdigen Systemzustand zu attestieren.

## Secrets

Offline Recovery darf nicht automatisch vollständigen Zugriff auf alle Secrets erhalten.

Secret-Zugriff muss weiterhin nach notwendiger Authority und Recovery-Semantik erfolgen.

Wenn Credentials oder Secrets aufgrund eines Security Incidents als kompromittiert gelten, kann Recovery deren Rotation oder Invalidierung vorbereiten.

## Data Sovereignty

Offline Recovery bleibt an Data-Sovereignty- und Privacy-Regeln gebunden.

NovaDOS darf geschützte Daten nicht allein aufgrund seines Recovery-Status beliebig lesen, exportieren oder übertragen.

Recovery Authority ist keine universelle Datenzugriffsberechtigung.

## Netzwerk

NovaDOS soll grundlegende Offline Recovery ohne Netzwerk durchführen können.

Netzwerkzugriff kann optional für definierte Funktionen verwendet werden, beispielsweise:

```text
Trusted Recovery Source
Package Retrieval
Remote Administration
External Verification
```

Netzwerkabhängigkeit darf jedoch nicht Voraussetzung für grundlegende lokale Recovery sein.

## Recovery Verification

Nach Offline Recovery muss NovaDOS den resultierenden Zustand soweit möglich prüfen.

```text
Offline Recovery
      ↓
Integrity Verification
      ↓
State Verification
      ↓
Bootability Verification
      ↓
Ready for NovaOS
```

Eine erfolgreich ausgeführte Reparaturaktion allein reicht nicht aus.

## Übergang zurück zu NovaOS

NovaDOS darf den normalen NovaOS-Start erst freigeben, wenn die für den Recovery Scope erforderlichen Bedingungen erfüllt sind.

```text
Recovery
   ↓
Verify
   ↓
Boot permitted?
 ├── Yes → NovaOS
 └── No  → Remain in Recovery / Escalate
```

Bei nicht erfolgreicher Verification bleibt das System im Recovery-Zustand oder eskaliert auf eine alternative Recovery-Strategie.

## Wiederholte Bootfehler

NovaOS muss wiederholte fehlgeschlagene Bootversuche erkennen können.

```text
Boot
 ↓ fail
Boot
 ↓ fail
Boot
 ↓
NovaDOS Recovery
```

Damit werden unbegrenzte Boot Loops vermieden.

Die genaue Schwelle wird durch Policy beziehungsweise NPSPEC definiert.

## Manuelle Verwendung

NovaDOS kann zusätzlich manuell über den Bootmanager gestartet werden.

Manueller Start bedeutet jedoch nicht automatisch uneingeschränkte Recovery Authority.

Sicherheitskritische Operationen bleiben an die dafür vorgesehenen Berechtigungen und Policies gebunden.

## Disaster Recovery

NovaDOS kann Bestandteil deklarativer Disaster-Recovery-Pläne sein.

Beispiel:

```text
System Storage Failure
        ↓
Boot NovaDOS
        ↓
Restore System State
        ↓
Verify
        ↓
Boot NovaOS
        ↓
Continue Declarative Recovery
```

NovaDOS übernimmt dabei den Teil des Recovery Plans, der außerhalb des laufenden NovaOS ausgeführt werden muss.

## Explainability

Der Übergang zu NovaDOS muss erklärbar sein.

Nova.Resilience soll mindestens darstellen können:

```text
Why offline recovery was required
Which failure triggered it
Which scope is affected
Which recovery action is planned
Whether recovery succeeded
Why NovaOS boot was allowed or denied
```

## Normative Anforderungen

1. NovaOS MUSS NovaDOS als definierte Offline-Recovery-Umgebung verwenden.
2. NovaDOS MUSS Bestandteil der Nova.Resilience-Architektur sein.
3. Offline Recovery MUSS verwendet werden können, wenn sichere Online Recovery nicht mehr gewährleistet werden kann.
4. NovaDOS MUSS ausreichend unabhängig von den zu reparierenden NovaOS-Komponenten sein.
5. NovaDOS SOLL einen möglichst kleinen Recovery Stack besitzen.
6. Offline Recovery MUSS dem Prinzip des kleinsten sicheren Recovery Scope folgen.
7. Nova.Resilience SOLL vor Offline Recovery einen minimal notwendigen Recovery Context persistieren können.
8. NovaDOS MUSS übergebenen Recovery Context vor sicherheitsrelevanter Verwendung validieren.
9. Der Bootmanager MUSS einen kontrollierten Übergang zu NovaDOS ermöglichen.
10. NovaDOS MUSS relevante System-Volumes und unterstützte Dateisysteme für Recovery bearbeiten können.
11. Unterbrochene Systemtransaktionen MÜSSEN entsprechend ihrer Operationssemantik recoverbar sein können.
12. NovaDOS MUSS Semantic Checkpoints für Offline Recovery verwenden können.
13. Security- und Integrity-Failures DÜRFEN nicht durch bloßen Neustart als behoben gelten.
14. Die Integrität der NovaDOS-Recovery-Umgebung MUSS für sicherheitskritische Recovery verifizierbar sein.
15. Recovery Authority DARF nicht automatisch uneingeschränkten Zugriff auf Secrets oder geschützte Daten erzeugen.
16. Data-Sovereignty-, Privacy-, Trust- und Capability-Regeln MÜSSEN auch während Offline Recovery gelten.
17. Grundlegende lokale Recovery SOLL ohne Netzwerkverbindung möglich sein.
18. Netzwerkbasierte Recovery-Funktionen MÜSSEN explizit kontrollierbar sein.
19. Offline Recovery MUSS vor Rückkehr zu NovaOS verifiziert werden.
20. Fehlgeschlagene Verification DARF keinen normalen NovaOS-Start als erfolgreich wiederhergestellt markieren.
21. Wiederholte Bootfehler MÜSSEN eine Eskalation zu NovaDOS auslösen können.
22. Manueller NovaDOS-Start DARF Security- und Recovery-Policies nicht automatisch umgehen.
23. Deklarative Disaster-Recovery-Pläne MÜSSEN NovaDOS als Offline-Recovery-Schritt referenzieren können.
24. Übergang, Recovery-Aktionen, Verification und Rückkehr zu NovaOS MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Recovery bleibt auch bei schwer beschädigtem NovaOS möglich,
- Kernel-, Boot-, Storage- und Systemfehler können offline behandelt werden,
- NovaDOS wird sauber in Nova.Resilience integriert,
- Boot Loops können automatisch in einen Recovery-Pfad überführt werden,
- Offline Recovery bleibt an Trust-, Capability- und Sovereignty-Regeln gebunden,
- NovaDOS kann Teil deklarativer Disaster-Recovery-Pläne sein.

### Negative Konsequenzen

- NovaDOS wird Teil der vertrauenswürdigen Recovery-Basis und muss entsprechend abgesichert werden,
- Recovery-Formate und Metadaten müssen zwischen NovaOS, Bootmanager und NovaDOS kompatibel bleiben,
- bestimmte NovaOS-Funktionen müssen in minimaler Form auch für Offline Recovery verfügbar sein.

## Verworfene Alternativen

### Recovery ausschließlich innerhalb von NovaOS

Verworfen, da beschädigte Kernel-, Storage-, Boot- oder Recovery-Komponenten eine sichere Online Recovery unmöglich machen können.

### Vollständiges zweites NovaOS als Recovery-System

Verworfen, da dadurch Abhängigkeiten, Angriffsfläche und gemeinsame Fehlerursachen unnötig vergrößert würden.

### NovaDOS als unabhängiges Wartungssystem ohne Nova.Resilience-Integration

Verworfen, da dadurch Recovery Context, Contracts, Verification und deklarative Recovery nicht konsistent fortgeführt werden könnten.

### Automatische Neuinstallation bei schwerem Fehler

Verworfen, da häufig ein wesentlich kleinerer Recovery Scope ausreichend ist und vorhandener Zustand erhalten werden kann.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0003_Hierarchische_Failure_Domains`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-RESILIENCE-0009_Semantic_Checkpointing`
- `ADR-RESILIENCE-0010_Rollback_über_Systemgrenzen`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0014_Security_Failures_durch_Quarantine_statt_blindem_Restart`
- `ADR-RESILIENCE-0017_RTO_und_RPO_als_Recovery_Contracts`
- `ADR-RESILIENCE-0018_Deklarative_Disaster_Recovery`
- `ADR-RESILIENCE-0020_Explainable_Self_Healing`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-OFFLINE-RECOVERY-0001`
- `NPSPEC-RESILIENCE-NOVADOS-RECOVERY-0001`
- `NPSPEC-RESILIENCE-RECOVERY-CONTEXT-0001`
- `NPSPEC-RESILIENCE-OFFLINE-TRANSACTION-RECOVERY-0001`
- `NPSPEC-RESILIENCE-OFFLINE-VERIFICATION-0001`
- `NPSPEC-RESILIENCE-BOOT-FAILURE-ESCALATION-0001`
- `NPSPEC-RESILIENCE-NOVADOS-TRUST-0001`
- `NPSPEC-RESILIENCE-NOVADOS-RECOVERY-TEST-0001`

## Ergebnis

NovaDOS bildet die Offline-Recovery-Ebene von Nova.Resilience:

```text
NovaOS
   ↓
Failure
   ↓
Online Recovery possible?
   ├── Yes
   │     ↓
   │ Nova.Resilience
   │
   └── No
         ↓
   Preserve Recovery Context
         ↓
      Boot Manager
         ↓
       NovaDOS
         ↓
   Offline Recovery
         ↓
      Verification
      ┌──────┴──────┐
    Valid         Invalid
      ↓              ↓
  Boot NovaOS    Remain in Recovery
      ↓              ↓
   Restore         Escalate
```

Damit besitzt NovaOS eine unabhängige, minimale und kontrollierte Recovery-Ebene für Fehler, bei denen das laufende Betriebssystem selbst keine ausreichend sichere Grundlage für seine eigene Wiederherstellung mehr darstellt.