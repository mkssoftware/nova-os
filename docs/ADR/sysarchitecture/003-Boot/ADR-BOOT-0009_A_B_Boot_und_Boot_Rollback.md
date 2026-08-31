# ADR-BOOT-0009 – A/B-Boot und Boot-Rollback

## Status

Angenommen

## Kategorie

Boot / Recovery / Updates / Rollback / Known-Good / Resilienz / Transaktionale Systemoperationen

## Kontext

NovaOS soll Systemupdates durchführen können, ohne dass ein fehlerhaftes oder unvollständiges Update das System dauerhaft unbootbar macht.

Ein klassisches In-Place-Update verändert die aktive Installation direkt:

```text
Active System
     ↓
Modify Files
     ↓
Modify Boot State
     ↓
Restart
```

Scheitert dieser Vorgang während des Schreibens oder stellt sich die neue Version erst nach dem Neustart als fehlerhaft heraus, kann ein inkonsistenter Systemzustand entstehen.

Dies widerspricht den NovaOS-Prinzipien:

- transaktionale Systemoperationen,
- Fail-Safe Defaults,
- Known-Good Recovery,
- Self-Healing,
- vertrauenswürdige Bootkette,
- explizite Systemgenerationen.

NovaOS benötigt deshalb einen Bootmechanismus, bei dem eine neue Systemgeneration vorbereitet und verifiziert werden kann, ohne die aktuell funktionierende Generation sofort zu zerstören.

---

## Entscheidung

NovaOS unterstützt **A/B-Boot mit transaktionaler Aktivierung und automatischem Boot-Rollback**.

Mindestens zwei bootfähige Systemgenerationen können parallel existieren:

```text
Slot A
Slot B
```

Dabei ist jeweils eine Generation:

```text
Active
```

während eine andere beispielsweise:

```text
Inactive
Candidate
Known-Good
Fallback
```

sein kann.

Ein Update wird grundsätzlich auf eine nicht aktive Generation vorbereitet.

Erst nach vollständigem Staging und erfolgreicher Verifikation wird diese Generation als neuer Boot Candidate aktiviert.

Die bisherige funktionierende Generation bleibt erhalten, bis die neue Generation erfolgreich gestartet und bestätigt wurde.

---

## Grundmodell

```text
Current Known-Good
      Slot A
        │
        │ Update
        ▼
   Prepare Slot B
        │
        ▼
      Verify
        │
        ▼
Mark B as Candidate
        │
        ▼
      Reboot
        │
        ▼
   Boot Slot B
        │
   ┌────┴────┐
   │         │
Success    Failure
   │         │
   ▼         ▼
Commit B   Rollback A
   │
   ▼
B becomes
Known-Good
```

Die zentrale Regel lautet:

```text
Never destroy the known-good system
before the candidate has proven itself.
```

---

## Logische Slots statt physischer Festlegung

`A` und `B` sind **logische Bootslots**.

Die ADR schreibt nicht vor, dass zwingend zwei feste physische Partitionen existieren müssen.

Eine Implementierung darf beispielsweise verwenden:

- getrennte Partitionen,
- Btrfs-Subvolumes,
- immutable Systemimages,
- Snapshots,
- Content-addressed Generationen,
- andere atomar aktivierbare Storage-Mechanismen.

Entscheidend ist die Semantik:

```text
Active Generation
+
Independent Candidate Generation
+
Atomic Boot Selection
+
Known-Good Fallback
```

Damit bleibt die Architektur unabhängig vom konkreten Dateisystem.

---

## Boot Slot

Ein Boot Slot beschreibt eine bootfähige Systemgeneration.

Konzeptionell:

```text
BootSlot {
    id
    generation
    state
    manifest
    kernel
    systemRoot
    trustState
    bootAttempts
}
```

Die konkrete Datenstruktur wird durch die zugehörigen NPSPECs definiert.

---

## Slot-Zustände

Mindestens folgende logische Zustände müssen darstellbar sein:

```text
Inactive
Staging
Candidate
Booting
KnownGood
Failed
Invalid
```

Ein Slot kann zusätzlich als aktueller:

```text
Active
```

Bootpfad referenziert werden.

`Active` und `KnownGood` sind dabei nicht zwingend identisch.

Eine Candidate-Generation kann aktiv gebootet werden, ohne bereits Known-Good zu sein.

---

## Systemgenerationen

Jeder bootfähige Systemzustand erhält eine eindeutige Generation.

Beispiel:

```text
Slot A:
    Generation 41
    KnownGood

Slot B:
    Generation 42
    Candidate
```

Die Generation ist Bestandteil von:

- Boot Planning,
- Verified Boot,
- Measured Boot,
- Recovery,
- Updateverwaltung,
- Architecture Introspection.

---

## Updateablauf

Ein reguläres A/B-Update folgt konzeptionell:

```text
1. Select inactive slot
2. Stage new generation
3. Verify generation
4. Prepare boot metadata
5. Mark candidate
6. Atomically select candidate
7. Reboot
8. Boot candidate
9. Validate system health
10. Commit or rollback
```

Bis Schritt 9 bleibt die vorherige Known-Good-Generation erhalten.

---

## Staging

Während des Staging wird die neue Generation vollständig vorbereitet.

Die aktive Generation darf dabei nicht inkonsistent verändert werden.

Ein Stromausfall während:

```text
Staging
```

darf deshalb nicht die bisherige Known-Good-Generation zerstören.

Ein unvollständiger Slot wird beim nächsten Boot als:

```text
Incomplete
Invalid
```

beziehungsweise äquivalent behandelt und nicht automatisch gestartet.

---

## Verifikation vor Aktivierung

Eine Candidate-Generation darf erst aktiviert werden, wenn ihre erforderlichen Bootartefakte erfolgreich verifiziert wurden.

Dies umfasst entsprechend der aktiven Policy insbesondere:

- Boot Manifest,
- Kernel,
- erforderliche Module,
- Systemgeneration,
- Signaturen,
- Integrität,
- Trust Policy.

Ein fehlerhafter Candidate darf die Known-Good-Generation nicht ersetzen.

---

## Atomare Bootauswahl

Der Wechsel:

```text
Slot A → Slot B
```

muss atomar beziehungsweise crash-konsistent erfolgen.

Ein Stromausfall während der Aktivierung darf keinen Zustand erzeugen, bei dem weder A noch B eindeutig bootfähig ausgewählt werden kann.

Bootmetadaten müssen daher transaktional aktualisiert werden.

---

## Candidate Boot

Nach Aktivierung wird der Candidate beim nächsten Boot bevorzugt gestartet.

Beispiel:

```text
PreferredBootSlot = B
PreviousKnownGood = A
```

Der Candidate bleibt jedoch zunächst unbestätigt.

---

## Boot Attempt Tracking

NovaOS verfolgt Bootversuche eines Candidates.

Beispiel:

```text
Generation 42
State: Candidate
Attempts: 1
```

Damit kann erkannt werden, wenn eine neue Generation wiederholt nicht erfolgreich startet.

---

## Boot Success

Ein Candidate wird nicht allein deshalb Known-Good, weil der Bootloader den Kernel erfolgreich gestartet hat.

Die erfolgreiche Generation muss einen definierten Systemzustand erreichen.

Konzeptionell:

```text
Bootloader
    ↓
Kernel Entry
    ↓
Critical Services
    ↓
System Health Validation
    ↓
Boot Success
```

Erst danach darf:

```text
Candidate
    ↓
KnownGood
```

erfolgen.

---

## Commit

Nach erfolgreicher Health Validation wird die neue Generation committed.

Beispiel:

```text
Slot B:
    Candidate
        ↓
    KnownGood
```

Anschließend darf die vorherige Generation:

- als Fallback erhalten bleiben,
- später freigegeben,
- archiviert,
- oder durch eine zukünftige Generation ersetzt

werden.

Die konkrete Retention Policy ist nicht Bestandteil dieser ADR.

---

## Fehlgeschlagener Candidate

Scheitert der Candidate vor erfolgreicher Bestätigung, bleibt die vorherige Known-Good-Generation gültig.

Beispiel:

```text
Boot B
   ↓
Failure
   ↓
Mark B Failed
   ↓
Select A
   ↓
Boot Known-Good A
```

---

## Automatischer Rollback

Nach einer definierten Anzahl fehlgeschlagener Candidate-Bootversuche muss NovaOS automatisch auf eine bekannte funktionierende Generation zurückfallen können.

Die genaue Anzahl der Versuche ist Policy.

Beispiel:

```text
Candidate B
Attempt 1 → Failed
Attempt 2 → Failed
        ↓
Rollback to A
```

Die Anzahl darf nicht als unveränderliche Bootloaderkonstante behandelt werden.

---

## Sofortiger Rollback

Bestimmte Fehler dürfen einen sofortigen Rollback auslösen.

Beispiele:

- ungültige Signatur,
- beschädigtes Boot Manifest,
- Kernel nicht ladbar,
- Required Boot Resource fehlt,
- Candidate explizit als ungültig markiert.

Ein weiterer Bootversuch wäre in solchen Fällen nicht sinnvoll.

---

## Rollback als Bootentscheidung

Rollback ist keine Dateikopie zurück auf einen alten Zustand.

Bevorzugt wird:

```text
change active generation reference
```

statt:

```text
copy old files back
```

Dadurch bleibt Rollback schnell, nachvollziehbar und crash-konsistent.

---

## Known-Good

Eine Known-Good-Generation ist eine Generation, die:

- erfolgreich verifiziert,
- erfolgreich gebootet,
- den definierten Health State erreicht

hat.

`Known-Good` bedeutet nicht, dass die Generation dauerhaft vertrauenswürdig bleibt.

Wird beispielsweise ihre Signatur, Integrität oder Trust Policy später ungültig, darf sie nicht allein wegen des alten Known-Good-Flags gestartet werden.

---

## Verified Boot

Jeder Slot wird unabhängig verifiziert.

Es gilt:

```text
Known-Good
≠
skip verification
```

Auch ein Rollback-Ziel muss beim tatsächlichen Boot die aktuelle erforderliche Trust Policy erfüllen.

---

## Measured Boot und TPM

Die aktive Generation muss in den Measurement State einfließen können.

Beispiel:

```text
Generation 41
```

und:

```text
Generation 42
```

erzeugen unterscheidbare Boot Evidence.

Dadurch können Attestation und TPM-Sealing erkennen, welche Generation tatsächlich gestartet wurde.

---

## TPM-Sealing und Updates

Wenn Secrets an Bootzustände gebunden sind, muss ein Update die neue Candidate-Generation kontrolliert autorisieren können.

Konzeptionell:

```text
Known-Good N
      OR
Candidate N+1
```

können während des Updateübergangs autorisierte Zustände sein.

Nach erfolgreichem Commit kann die Policy entsprechend weiterentwickelt werden.

---

## Anti-Rollback

A/B-Rollback und Security Anti-Rollback sind unterschiedliche Konzepte.

A/B-Rollback bedeutet:

```text
return to an authorized known-good generation
```

Security Anti-Rollback verhindert:

```text
unauthorized downgrade
```

Beide Mechanismen müssen zusammenarbeiten.

Ein älterer Slot darf nicht gestartet werden, wenn die Security Policy diese Generation inzwischen verbietet.

---

## Recovery Integration

Wenn weder Candidate noch vorherige Known-Good-Generation erfolgreich gestartet werden können, wechselt NovaOS in den Recovery-Modus gemäß `ADR-BOOT-0008`.

Beispiel:

```text
Candidate B
    ↓ failure

Known-Good A
    ↓ failure

Recovery
```

A/B-Boot ersetzt Recovery daher nicht.

Es reduziert lediglich die Anzahl der Situationen, in denen eine tiefere Recovery notwendig ist.

---

## NovaDOS

NovaDOS kann zur Diagnose oder Reparatur der Slot-Metadaten und Systemgenerationen verwendet werden.

Änderungen an A/B-Metadaten müssen dabei denselben Sicherheits- und Konsistenzregeln folgen.

---

## Self-Healing

Self-Healing darf:

- fehlerhafte Candidates deaktivieren,
- Known-Good auswählen,
- Bootmetadaten reparieren,
- eine neue Candidate-Generation vorbereiten.

Self-Healing darf jedoch keinen ungültigen Slot als vertrauenswürdig markieren oder Anti-Rollback-Regeln umgehen.

---

## Bootmetadaten

A/B-Boot benötigt kleine, besonders geschützte Bootmetadaten.

Konzeptionell:

```text
BootState {
    activeSlot
    candidateSlot
    knownGoodSlot
    generation
    attemptCount
    lastBootResult
}
```

Diese Metadaten müssen crash-konsistent und gegen beschädigte Zustände abgesichert sein.

---

## Redundante Bootmetadaten

Die konkrete Implementierung darf redundante Kopien, Generation Counter oder Checksummen verwenden.

Beispiel:

```text
BootState Copy 1
BootState Copy 2
Generation Counter
Checksum
```

Damit kann nach einem unterbrochenen Schreibvorgang der letzte gültige Zustand bestimmt werden.

---

## Boot State Ownership

Während der frühen Bootphase besitzt die Bootumgebung die Autorität über die Auswahl des Slots.

Nach Kernelstart darf die Systemverwaltung den Bootstatus aktualisieren, beispielsweise:

```text
MarkBootSuccessful()
```

Die Verantwortungsübergabe muss explizit definiert sein.

---

## Keine beliebige Anwendungskontrolle

Normale Anwendungen dürfen:

- Slot-Auswahl,
- Known-Good-State,
- Boot Attempt Counter,
- Rollback Policy

nicht direkt verändern.

Diese Operationen benötigen entsprechende System-Capabilities.

---

## Architekturintrospektion

NovaOS soll den A/B-Zustand strukturiert darstellen können.

Beispiel:

```text
Current:
    Slot B
    Generation 42
    Candidate

Fallback:
    Slot A
    Generation 41
    Known-Good

Boot Attempts:
    1
```

---

## Audit

Mindestens folgende Ereignisse sollen nachvollziehbar sein:

```text
candidate staged
candidate verified
candidate activated
candidate boot attempted
candidate failed
rollback selected
known-good booted
candidate committed
```

---

## Determinismus

Bei identischem:

- Boot State,
- Candidate State,
- Known-Good State,
- Trust State,
- Rollback Policy

muss die Slot-Auswahl deterministisch sein.

KI ist dafür nicht erforderlich.

---

## KI-Unterstützung

KI darf optional:

- Fehlerursachen analysieren,
- Updateprobleme erklären,
- Recoverymaßnahmen vorschlagen.

KI darf nicht:

- einen unverifizierten Candidate aktivieren,
- Known-Good ohne Nachweis setzen,
- Anti-Rollback umgehen,
- Boot Attempt Tracking manipulieren.

---

## Normative Anforderungen

1. NovaOS MUSS A/B-Boot beziehungsweise eine semantisch äquivalente Generationenarchitektur unterstützen.
2. A und B MÜSSEN als logische Slots behandelt werden; feste physische Partitionen DÜRFEN NICHT vorausgesetzt werden.
3. Eine neue Systemgeneration MUSS unabhängig von der aktiven Known-Good-Generation vorbereitet werden können.
4. Die aktive Known-Good-Generation DARF während des Candidate-Staging NICHT zerstört werden.
5. Unvollständig vorbereitete Candidates DÜRFEN NICHT automatisch aktiviert werden.
6. Candidates MÜSSEN vor Aktivierung gemäß Boot Trust Policy verifiziert werden.
7. Die Aktivierung eines Candidates MUSS atomar beziehungsweise crash-konsistent erfolgen.
8. NovaOS MUSS Systemgenerationen eindeutig identifizieren können.
9. Candidate, Known-Good, Failed und Invalid MÜSSEN unterscheidbare Zustände sein.
10. Ein Candidate DARF erst nach erfolgreicher Health Validation zu Known-Good werden.
11. Kernel Entry allein MUSS NICHT als vollständiger Booterfolg gelten.
12. Candidate-Bootversuche MÜSSEN gezählt beziehungsweise äquivalent verfolgt werden können.
13. Die maximal erlaubte Anzahl fehlgeschlagener Bootversuche MUSS policygesteuert sein.
14. NovaOS MUSS nach Candidate-Fehlern automatisch auf eine autorisierte Known-Good-Generation zurückfallen können.
15. Eindeutig ungültige Candidates MÜSSEN ohne unnötige Wiederholungsversuche deaktiviert werden können.
16. Rollback SOLL durch Generation-/Slot-Umschaltung statt durch Rückkopieren einzelner Systemdateien erfolgen.
17. Known-Good DARF die normale Verified-Boot-Prüfung NICHT umgehen.
18. Die aktive Generation MUSS in Measured Boot integrierbar sein.
19. TPM-Sealing MUSS Updateübergänge zwischen autorisierten Generationen unterstützen können.
20. A/B-Rollback MUSS mit Security Anti-Rollback kompatibel sein.
21. Ein durch Security Policy verbotener alter Slot DARF NICHT allein aufgrund seines Known-Good-Status gestartet werden.
22. Wenn kein autorisierter bootfähiger Slot verfügbar ist, MUSS Recovery erreichbar sein.
23. A/B-Boot MUSS in die transaktionale Updatearchitektur integrierbar sein.
24. Self-Healing DARF Slot-Auswahl und Rollback nur innerhalb der geltenden Trust Policy durchführen.
25. Bootmetadaten MÜSSEN crash-konsistent gespeichert werden.
26. Beschädigte Bootmetadaten MÜSSEN erkennbar sein.
27. Die Autorität zum Ändern von Slot- und Known-Good-Zuständen MUSS capabilitybasiert geschützt sein.
28. Slot-Auswahl, Candidate-Aktivierung, Commit und Rollback MÜSSEN auditierbar sein.
29. Der aktuelle A/B- und Generationenzustand MUSS introspektierbar sein.
30. Die grundlegende Rollbackentscheidung MUSS ohne KI korrekt funktionieren.
31. KI DARF Verified Boot, Anti-Rollback oder Known-Good-Regeln NICHT überschreiben.
32. A/B-Boot MUSS auch bei Stromausfall während Staging oder Boot-State-Aktualisierung einen definierten Recoverypfad behalten.

---

## Konsequenzen

### Positive Konsequenzen

- Updates verändern nicht unmittelbar die funktionierende Systemgeneration.
- fehlerhafte Updates können automatisch zurückgerollt werden.
- Stromausfälle während des Update-Staging gefährden die aktive Generation deutlich weniger.
- Rollback kann durch schnelle Generationenumschaltung erfolgen.
- Known-Good, Recovery und Self-Healing greifen auf dasselbe Modell zurück.
- Btrfs-Snapshots oder zukünftige immutable Systemimages können dieselbe A/B-Semantik implementieren.
- Verified Boot, Measured Boot und TPM-Sealing können generationenbezogen arbeiten.

### Negative Konsequenzen

- mindestens eine zusätzliche bootfähige Generation benötigt Speicherplatz.
- Boot State und Generation Lifecycle müssen zuverlässig verwaltet werden.
- Updates müssen mit TPM-Sealing und Anti-Rollback koordiniert werden.
- die Definition eines zuverlässigen Boot-Success-/Health-Punkts ist erforderlich.

---

## Verworfene Alternativen

### Ausschließlich In-Place-Updates

Verworfen.

Ein fehlgeschlagenes Update könnte die einzige bootfähige Systemgeneration beschädigen.

### Zwingend zwei feste Systempartitionen

Verworfen.

Dies würde NovaOS unnötig an ein bestimmtes Storage-Layout binden.

### Candidate sofort nach Kernel Entry als Known-Good markieren

Verworfen.

Viele kritische Fehler treten erst während der weiteren Systeminitialisierung auf.

### Rollback durch Rückkopieren alter Dateien

Als primärer Mechanismus verworfen.

Eine Generationenumschaltung ist konsistenter und leichter transaktional abzusichern.

### Known-Good ohne erneute Trust-Prüfung starten

Verworfen.

Known-Good ist ein Health-State und kein dauerhafter kryptografischer Vertrauensnachweis.

### Unbegrenzte Candidate-Bootversuche

Verworfen.

Dies würde zu Boot-Loops führen und automatisches Recovery verhindern.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-BOOT-0003_Mehrstufige_Bootarchitektur`
- `ADR-BOOT-0005_Vertrauenswürdige_Bootkette`
- `ADR-BOOT-0006_Secure_Measured_Verified_Boot_Integration`
- `ADR-BOOT-0007_TPM_Integration_in_die_Bootkette`
- `ADR-BOOT-0008_Recovery_als_First_Class_Bootmodus`
- `ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage`
- `ADR-ARCH-0004_Transaktionale_Systemoperationen`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`
- `ADR-TRUST-0001_Nova_Trust`

---

## Zugehörige NPSPECs

- `NPSPEC-BOOT-AB-0001`
- `NPSPEC-BOOT-SLOT-0001`
- `NPSPEC-BOOT-GENERATION-0001`
- `NPSPEC-BOOT-CANDIDATE-0001`
- `NPSPEC-BOOT-BOOTSTATE-0001`
- `NPSPEC-BOOT-BOOT-SUCCESS-0001`
- `NPSPEC-BOOT-ATTEMPT-0001`
- `NPSPEC-BOOT-KNOWNGOOD-0001`
- `NPSPEC-BOOT-ROLLBACK-0001`
- `NPSPEC-BOOT-ANTIROLLBACK-0001`
- `NPSPEC-BOOT-UPDATE-HANDOFF-0001`
- `NPSPEC-BOOT-AB-RECOVERY-0001`
- `NPSPEC-BOOT-AB-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- Update von Slot A auf Slot B,
- Update von Slot B auf Slot A,
- Candidate-Staging bei laufender Known-Good-Generation,
- Stromausfall während des Staging,
- Stromausfall während der Candidate-Aktivierung,
- beschädigter Candidate,
- ungültige Candidate-Signatur,
- erfolgreicher Candidate-Boot,
- Candidate erreicht Health State,
- Commit zu Known-Good,
- Kernel startet, Health State wird aber nicht erreicht,
- wiederholte Candidate-Bootfehler,
- automatischer Rollback,
- sofortiger Rollback bei ungültigem Candidate,
- Rollback auf Known-Good,
- Known-Good mit inzwischen ungültiger Trust Policy,
- Anti-Rollback gegen unautorisierten Downgrade,
- autorisierter Known-Good-Rollback,
- TPM-Sealing während eines Generationenwechsels,
- beschädigte Bootmetadaten,
- Recovery wenn beide Slots nicht bootfähig sind,
- NovaDOS-Diagnose des Slot-Zustands.

---

## Ergebnis

NovaOS verwendet A/B-Boot als transaktionales Generationenmodell für sichere Systemupdates und schnelle Boot-Rollbacks.

Das Kernmodell lautet:

```text
Known-Good A
     ↓
Stage B
     ↓
Verify B
     ↓
Boot Candidate B
     │
 ┌───┴───┐
 ▼       ▼
Success Failure
 │       │
 ▼       ▼
Commit  Rollback
 B       A
```

Dabei sind A und B keine zwingenden Partitionen, sondern logische bootfähige Systemgenerationen.

Die zentrale Architekturregel lautet:

```text
Stage separately.

Verify before activation.

Keep the known-good generation.

Commit only after health confirmation.

Rollback by switching generations,
not by reconstructing the past.
```