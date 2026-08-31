# ADR-BOOT-0008 – Recovery als First-Class-Bootmodus

## Status

Angenommen

## Kategorie

Boot / Recovery / Resilienz / Known-Good / NovaDOS / Self-Healing / Boot Policy

## Kontext

Recovery wird in klassischen Betriebssystemen häufig als nachträglich ergänzter Sonderfall behandelt.

Typische Modelle sind:

```text
Normal Boot
    ↓ failure
Recovery somehow
```

oder:

```text
separate recovery partition
separate recovery loader
special hidden key combination
```

Dadurch entstehen häufig getrennte Mechanismen für:

- normales Booten,
- Reparatur,
- Rollback,
- Diagnose,
- Offline-Wartung,
- Wiederherstellung.

Für NovaOS passt dieses Modell nicht zur vorgesehenen Architektur.

Recovery soll dieselben grundlegenden Mechanismen verwenden wie jeder andere Bootvorgang:

- Boot Planning,
- Verified Boot,
- Trust Policy,
- Kernel Loading,
- Resource Management,
- BootInfo,
- Kernel Handoff,
- Architecture Introspection.

Recovery ist deshalb kein Fehleranhängsel des normalen Bootpfads, sondern ein eigenständiger und vollständig modellierter Bootmodus.

---

## Entscheidung

NovaOS behandelt **Recovery als First-Class-Bootmodus**.

Recovery ist ein reguläres Bootziel innerhalb derselben Bootarchitektur wie Normal Boot, NovaDOS, Diagnostic Mode oder Safe Mode.

Das Grundmodell lautet:

```text
Boot Environment
      ↓
Boot Policy
      ↓
Boot Mode Selection
      │
      ├── Normal
      ├── Recovery
      ├── NovaDOS
      ├── Safe
      └── Diagnostic
      ↓
Boot Plan
      ↓
Verification
      ↓
Kernel / Environment Handoff
```

Recovery besitzt dabei eigene:

- Boot Policy,
- Trust Policy,
- Capability-Grenzen,
- Ressourcen,
- UI,
- Aktionen,
- Auditinformationen.

---

## Grundprinzip

Es gilt:

```text
Recovery is a boot mode,
not an exception path.
```

und:

```text
Normal boot and recovery
share mechanisms,
but not necessarily policy.
```

---

## Recovery als expliziter Bootmodus

Der aktive Bootmodus muss Teil des Boot Plans sein.

Konzeptionell:

```text
BootMode {
    Normal
    Recovery
    NovaDOS
    Safe
    Diagnostic
}
```

Recovery wird damit nicht durch versteckte Sonderlogik aktiviert, sondern explizit ausgewählt.

---

## Auswahl von Recovery

Recovery kann ausgelöst werden durch:

- explizite Benutzerauswahl,
- Bootmenü,
- wiederholtes Bootversagen,
- beschädigte aktuelle Systemgeneration,
- fehlgeschlagenen Health Check,
- Self-Healing Policy,
- Updatefehler,
- administrative Policy.

Die Ursache der Auswahl muss nachvollziehbar bleiben.

Beispiel:

```text
BootMode:
    Recovery

Reason:
    PreviousBootFailedRepeatedly
```

---

## Recovery Boot Plan

Recovery wird als normaler Boot Plan beschrieben.

Beispiel:

```text
BootPlan {
    mode = Recovery
    target = NovaRecovery
    systemGeneration = KnownGood
    trustPolicy = RecoveryPolicy
}
```

Damit kann dieselbe Boot Planning Infrastructure wie beim normalen Start verwendet werden.

---

## Gemeinsame Mechanismen

Recovery soll möglichst dieselben technischen Mechanismen verwenden wie Normal Boot.

Dazu gehören insbesondere:

- Boot Storage,
- Dateisystemzugriff,
- Boot Resource Manager,
- Trust Verification,
- Kernel Image Loading,
- Boot Logging,
- BootInfo,
- Kernel Handoff.

Es soll keine zweite vollständig getrennte Bootarchitektur entstehen.

---

## Getrennte Policy

Gemeinsame Mechanismen bedeuten nicht identische Policy.

Beispielsweise kann gelten:

```text
Normal Boot:
    target = current generation

Recovery:
    target = known-good generation
```

oder:

```text
Normal Boot:
    production services

Recovery:
    maintenance capabilities only
```

---

## Recovery Environment

NovaOS darf eine eigene Recovery Environment besitzen.

Diese kann Funktionen bereitstellen für:

- Systemdiagnose,
- Bootreparatur,
- Dateisystemprüfung,
- Rollback,
- Update-Reparatur,
- Trust-Store-Reparatur,
- Systemgenerationen,
- Backup/Wiederherstellung,
- Hardwarediagnose,
- Start von NovaDOS.

Die Recovery Environment bleibt möglichst klein und unabhängig vom normalen Desktop.

---

## NovaDOS

NovaDOS ist ein eigenständiges privilegiertes Offline-Wartungssystem und kann aus Recovery heraus gestartet oder direkt als Bootmodus ausgewählt werden.

Das Modell lautet:

```text
Recovery
   │
   ├── Automated Repair
   ├── Known-Good Rollback
   ├── Diagnostics
   └── NovaDOS
```

NovaDOS und Recovery sind nicht identisch.

Recovery beschreibt den Systemmodus.

NovaDOS ist eines der möglichen Wartungsziele innerhalb der Recovery-Architektur.

---

## Known-Good Integration

Recovery ist eng mit Known-Good-Systemgenerationen verbunden.

Beispiel:

```text
Current Generation
      ↓ failure
Known-Good Generation
      ↓ verify
Recovery / Rollback
```

Eine als Known-Good markierte Generation muss weiterhin die aktuelle Trust Policy erfüllen.

---

## Automatisches Recovery

NovaOS darf Recovery automatisch auswählen, wenn normale Bootversuche wiederholt fehlschlagen.

Beispiel:

```text
Normal Boot
    ↓ failure

Normal Boot retry
    ↓ failure

Recovery
```

Die genaue Schwelle ist Policy und nicht fest in den Bootmechanismus eingebaut.

---

## Boot-Loop-Schutz

NovaOS muss erkennen können, wenn ein System wiederholt vor erfolgreicher Initialisierung neu startet.

Dadurch soll ein endloser Zyklus wie:

```text
Boot
↓
Crash
↓
Boot
↓
Crash
```

vermieden werden.

Stattdessen kann nach definierter Policy Recovery aktiviert werden.

---

## Boot Success Marker

Die Bootarchitektur benötigt einen klar definierten Punkt, ab dem eine Systemgeneration als erfolgreich gestartet gelten kann.

Beispiel:

```text
Bootloader
    ↓
Kernel
    ↓
Critical System Initialization
    ↓
Health Confirmation
    ↓
Boot Successful
```

Ein bloßer Kernel Entry reicht nicht zwingend aus, um einen Boot als erfolgreich zu markieren.

---

## Recovery und transaktionale Updates

Recovery ist Bestandteil der transaktionalen Updatearchitektur.

Beispiel:

```text
Generation N
    ↓
Stage N+1
    ↓
Boot Candidate N+1
    ↓ failure
Recovery
    ↓
Return to N
```

Dadurch kann ein fehlerhaftes Update rückgängig gemacht werden, ohne die gesamte Installation zu zerstören.

---

## Self-Healing

Self-Healing darf Recovery als Ausführungsmodus verwenden.

Beispiel:

```text
Boot problem detected
      ↓
Recovery plan created
      ↓
Validate
      ↓
Repair
      ↓
Verify
      ↓
Normal boot
```

Self-Healing darf dabei keine Hard Security Requirements umgehen.

---

## Repair Plan

Reparaturen sollen möglichst als explizite Pläne modelliert werden.

Beispiel:

```text
RecoveryPlan {
    issue
    actions
    affectedResources
    rollbackStrategy
    verification
}
```

Damit werden Reparaturaktionen nachvollziehbar und möglichst transaktional.

---

## Keine unkontrollierte automatische Reparatur

Recovery darf nicht unter Unsicherheit beliebige irreversible Änderungen durchführen.

Bei unbekanntem Zustand gilt weiterhin Fail-Safe.

Kritische Aktionen können eine explizite Benutzer- oder Administratorfreigabe verlangen.

---

## Recovery Trust

Recovery besitzt hohe Systemautorität.

Deshalb muss die Recovery Environment vollständig in die Trusted Boot Chain integriert sein.

Es gilt nicht:

```text
Recovery
=
verification disabled
```

sondern:

```text
Recovery
=
different authorized boot policy
```

---

## Kein Security-Downgrade

Ein Fehler im normalen Boot darf nicht automatisch zu einem unsignierten oder ungeprüften Recovery-System führen.

Recovery-Artefakte müssen ebenfalls:

- identifiziert,
- verifiziert,
- autorisiert

werden.

---

## Recovery Capabilities

Recovery kann mehr technische Systemrechte als eine normale Anwendung benötigen, aber nicht zwangsläufig uneingeschränkte Autorität.

Beispiele für explizite Recovery Capabilities:

```text
InspectSystemGeneration
RepairBootConfiguration
VerifySystemArtifacts
RollbackGeneration
RepairFilesystem
ManageRecoveryKeys
LaunchNovaDOS
```

Least Authority bleibt erhalten.

---

## Verschlüsselte Systeme

Recovery muss Verschlüsselung explizit berücksichtigen.

Ein Recovery Boot darf nicht automatisch alle Benutzer- oder Unternehmensschlüssel erhalten.

Mögliche Freigabemodelle sind:

- Recovery Credential,
- Recovery Key,
- TPM Policy,
- Administrator Authorization.

---

## Recovery und TPM

Recovery erzeugt einen eigenen gemessenen Bootzustand.

Dadurch kann TPM-basiertes Secret Sealing unterscheiden zwischen:

```text
Normal Boot
```

und:

```text
Recovery Boot
```

Dies verhindert, dass Recovery automatisch auf alle Secrets des normalen Betriebs zugreifen kann.

---

## Recovery UI

Recovery darf eine eigene minimale grafische oder textuelle Oberfläche besitzen.

Sie soll insbesondere:

- aktuellen Zustand erklären,
- erkannte Probleme darstellen,
- vorgeschlagene Maßnahmen anzeigen,
- irreversible Aktionen kenntlich machen,
- Fortschritt und Ergebnis anzeigen.

Die UI selbst ist jedoch keine Security Authority.

---

## Headless Recovery

Recovery muss auch ohne grafische Oberfläche möglich sein.

Mindestens ein reduzierter:

```text
Text / Serial / Console
```

Pfad soll unterstützt werden können.

---

## Netzwerk

Recovery soll grundsätzlich lokal und offline funktionieren können.

Netzwerk darf optional für:

- Updateabruf,
- Remote-Diagnose,
- Enterprise-Recovery,
- zusätzliche Artefakte

verwendet werden.

Es darf keine zwingende Cloudabhängigkeit für grundlegende Recovery geben.

---

## Recovery Logging

Recovery-Aktionen müssen strukturiert protokolliert werden.

Beispiel:

```text
Recovery started
Reason: failed candidate generation

Action:
    rollback to generation 41

Verification:
    successful

Result:
    system bootable
```

---

## Architecture Introspection

Nach einem Recovery-Vorgang soll nachvollziehbar sein:

- warum Recovery aktiviert wurde,
- welcher Bootmodus verwendet wurde,
- welche Generation aktiv war,
- welche Aktionen durchgeführt wurden,
- welches Ergebnis erreicht wurde.

---

## Recovery State

Konzeptionell:

```text
RecoveryState {
    reason
    sourceGeneration
    targetGeneration
    actions
    status
}
```

Die konkrete Struktur wird separat spezifiziert.

---

## Benutzerentscheidung vs Automatik

Recovery kann sowohl:

```text
Automatic
```

als auch:

```text
Interactive
```

arbeiten.

Automatische Aktionen dürfen nur ausgeführt werden, wenn deren Recovery Policy dies eindeutig erlaubt.

---

## Deterministic Recovery

Bei identischem:

- Fehlerzustand,
- Recovery Policy,
- Systemzustand,
- verfügbarer Known-Good-Generation

muss die grundlegende Recoveryentscheidung deterministisch sein können.

KI darf keine Voraussetzung dafür sein.

---

## KI-Unterstützung

KI darf optional:

- Bootlogs analysieren,
- Fehlerursachen gruppieren,
- Reparaturvorschläge priorisieren,
- technische Diagnosen erklären.

KI darf jedoch keine Hard Security Policy umgehen oder irreversible Reparatur ohne entsprechend autorisierte Policy erzwingen.

---

## Fehlerbehandlung innerhalb von Recovery

Auch Recovery selbst kann fehlschlagen.

Beispiele:

```text
RecoveryImageInvalid
KnownGoodUnavailable
FilesystemRepairFailed
RollbackFailed
TrustFailure
StorageUnavailable
```

Ein Recoveryfehler muss explizit behandelt werden.

---

## Recovery Fallback

Wenn automatisches Recovery nicht erfolgreich ist, kann ein tieferer Wartungspfad angeboten werden.

Beispiel:

```text
Automatic Recovery
      ↓ failure
Interactive Recovery
      ↓
NovaDOS
```

Auch dieser Pfad bleibt Teil derselben Trust Architecture.

---

## Kein garantierter Reparaturerfolg

Recovery bedeutet nicht, dass jeder Zustand automatisch repariert werden kann.

Bei schwerem:

- Hardwaredefekt,
- vollständigem Storage-Verlust,
- zerstörtem Trust Root,
- fehlendem Recovery Credential

kann Recovery lediglich Diagnose und kontrollierte Wiederherstellungsoptionen bereitstellen.

---

## Normative Anforderungen

1. NovaOS MUSS Recovery als First-Class-Bootmodus behandeln.
2. Recovery MUSS als expliziter Bootmodus im Boot Plan darstellbar sein.
3. Recovery MUSS dieselben grundlegenden Bootmechanismen wie Normal Boot wiederverwenden können.
4. Recovery MUSS eine eigene Policy besitzen können.
5. Recovery DARF NICHT als ungeprüfter Sonderpfad implementiert werden.
6. Recovery-Artefakte MÜSSEN in die Trusted Boot Chain integriert sein.
7. Ein fehlgeschlagener Normal Boot DARF NICHT automatisch Sicherheitsprüfungen für Recovery abschalten.
8. Recovery MUSS Known-Good-Systemgenerationen verwenden können.
9. Wiederholte Bootfehler MÜSSEN automatisches Recovery auslösen können.
10. Die Schwelle für automatisches Recovery MUSS policygesteuert sein.
11. NovaOS MUSS Boot-Loops erkennen können.
12. Ein erfolgreicher Boot MUSS über einen expliziten Success State markierbar sein.
13. Recovery MUSS mit transaktionalen Systemupdates integrierbar sein.
14. Ein fehlgeschlagenes Candidate-System MUSS auf Known-Good zurückgeführt werden können.
15. Self-Healing DARF Recovery als Ausführungsmodus verwenden.
16. Kritische Recoveryaktionen SOLLEN als explizite Recovery Plans modelliert werden.
17. Irreversible automatische Reparaturen DÜRFEN NICHT bei unbekanntem Zustand ausgeführt werden.
18. Recovery MUSS Least-Authority-Prinzipien unterstützen.
19. Recovery MUSS verschlüsselte Systeme und separate Recovery Credentials unterstützen können.
20. Recovery MUSS einen vom Normal Boot unterscheidbaren Measurement State erzeugen können.
21. TPM-Sealing MUSS zwischen Normal- und Recovery-Boot unterscheiden können.
22. NovaDOS MUSS als eigenständiges Recovery-/Maintenance-Target integrierbar sein.
23. Recovery MUSS auch ohne grafische Oberfläche funktionieren können.
24. Grundlegende Recovery MUSS offline möglich sein.
25. Netzwerkzugriff DARF optional sein.
26. Recoveryaktionen MÜSSEN auditierbar sein.
27. Der Grund für Recovery MUSS introspektierbar sein.
28. Automatische und interaktive Recovery MÜSSEN unterscheidbar sein.
29. Recoveryentscheidungen MÜSSEN ohne KI korrekt funktionieren.
30. KI DARF Hard Security Policy und Trust Requirements NICHT überschreiben.
31. Recoveryfehler MÜSSEN typisiert behandelbar sein.
32. Ein tieferer Wartungspfad MUSS möglich sein, wenn automatisches Recovery scheitert.
33. Recovery DARF keinen garantierten Reparaturerfolg voraussetzen.
34. NovaOS MUSS Recovery als regulären Bestandteil seiner Boot- und Resilienzarchitektur behandeln.

---

## Konsequenzen

### Positive Konsequenzen

- Recovery verwendet keine separate Parallelarchitektur.
- Bootloader, Trust, Kernel Loader und BootInfo können wiederverwendet werden.
- Known-Good und transaktionale Updates lassen sich sauber integrieren.
- Boot-Loops können kontrolliert beendet werden.
- Recovery und Self-Healing werden planbar und introspektierbar.
- NovaDOS fügt sich als Wartungsziel in dasselbe Modell ein.
- Recovery bleibt auch auf verschlüsselten und TPM-geschützten Systemen kontrollierbar.
- spätere Recovery-Erweiterungen benötigen keinen Architekturbruch.

### Negative Konsequenzen

- Boot Policy und Boot State Machine werden etwas umfangreicher.
- Recovery-Artefakte müssen separat gepflegt und signiert werden.
- Recovery- und Secret-Release-Policies benötigen zusätzliche Tests.
- Known-Good-Status und Boot-Success-Erkennung müssen zuverlässig verwaltet werden.

---

## Verworfene Alternativen

### Recovery nur als Fehlerdialog des Bootloaders

Verworfen.

Damit fehlen ein vollständiges Bootziel, definierte Ressourcen und systematische Reparaturmechanismen.

### Vollständig separater Recovery-Bootloader

Verworfen.

Dadurch würden Kernel Loading, Verification und Boot Policy unnötig dupliziert.

### Recovery ohne Trust-Prüfung

Verworfen.

Ein privilegiertes Wartungssystem wäre sonst ein direkter Sicherheits-Bypass.

### Immer automatisch reparieren

Verworfen.

Nicht jeder Fehlerzustand ist sicher und eindeutig reparierbar.

### Ausschließlich Cloud-Recovery

Verworfen.

NovaOS muss grundlegende Wiederherstellung auch offline ermöglichen.

### NovaDOS und Recovery gleichsetzen

Verworfen.

Recovery ist ein Bootmodus und eine Systemstrategie; NovaDOS ist ein konkretes Wartungs- und Diagnoseziel.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-BOOT-0001_Parallele_BIOS_und_UEFI_Bootunterstützung`
- `ADR-BOOT-0002_UEFI_bevorzugen_mit_BIOS_Fallback`
- `ADR-BOOT-0003_Mehrstufige_Bootarchitektur`
- `ADR-BOOT-0005_Vertrauenswürdige_Bootkette`
- `ADR-BOOT-0006_Secure_Measured_Verified_Boot_Integration`
- `ADR-BOOT-0007_TPM_Integration_in_die_Bootkette`
- `ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage`
- `ADR-ARCH-0004_Transaktionale_Systemoperationen`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`
- `ADR-TRUST-0001_Nova_Trust`

---

## Zugehörige NPSPECs

- `NPSPEC-BOOT-RECOVERY-0001`
- `NPSPEC-BOOT-RECOVERY-MODE-0001`
- `NPSPEC-BOOT-RECOVERY-POLICY-0001`
- `NPSPEC-BOOT-RECOVERY-PLAN-0001`
- `NPSPEC-BOOT-RECOVERY-STATE-0001`
- `NPSPEC-BOOT-BOOT-SUCCESS-0001`
- `NPSPEC-BOOT-BOOT-FAILURE-TRACKING-0001`
- `NPSPEC-BOOT-BOOTLOOP-0001`
- `NPSPEC-BOOT-KNOWNGOOD-0001`
- `NPSPEC-BOOT-ROLLBACK-0001`
- `NPSPEC-BOOT-RECOVERY-TRUST-0001`
- `NPSPEC-BOOT-RECOVERY-MEASUREMENT-0001`
- `NPSPEC-BOOT-RECOVERY-UI-0001`
- `NPSPEC-BOOT-RECOVERY-LOG-0001`
- `NPSPEC-BOOT-NOVADOS-RECOVERY-0001`
- `NPSPEC-BOOT-RECOVERY-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- manuell ausgewählter Recovery Boot,
- automatisch ausgelöstes Recovery,
- wiederholter Normal-Boot-Fehler,
- Boot-Loop-Erkennung,
- erfolgreich markierter Normal Boot,
- fehlerhafte Candidate-Generation,
- Known-Good-Rollback,
- fehlende Known-Good-Generation,
- verifiziertes Recovery Image,
- manipuliertes Recovery Image,
- Recovery auf verschlüsseltem System,
- unterschiedlicher TPM Measurement State für Normal und Recovery,
- Self-Healing über Recovery,
- fehlgeschlagene automatische Reparatur,
- Wechsel zu interaktiver Recovery,
- Start von NovaDOS,
- Headless-/Text-Recovery,
- Offline-Recovery,
- Recovery Logging und Introspection.

---

## Ergebnis

Recovery ist in NovaOS kein nachträglicher Sonderfall, sondern ein regulärer Bestandteil der Bootarchitektur.

Das Modell lautet:

```text
Boot Policy
    ↓
Select Mode
    │
    ├── Normal
    ├── Recovery
    ├── NovaDOS
    ├── Safe
    └── Diagnostic
    ↓
Verified Boot Plan
    ↓
Controlled Execution
```

Recovery verwendet dieselben grundlegenden Mechanismen wie Normal Boot, erhält jedoch eine eigene Policy und einen klar begrenzten Wartungskontext.

Die zentrale Architekturregel lautet:

```text
Recovery is a normal boot decision
for an abnormal system state.

Reuse the boot architecture.

Change the policy,
not the trust boundary.
```