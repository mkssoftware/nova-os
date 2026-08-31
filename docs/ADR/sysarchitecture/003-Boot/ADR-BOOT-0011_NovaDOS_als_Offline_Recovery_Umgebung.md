# ADR-BOOT-0011 – NovaDOS als Offline-Recovery-Umgebung

## Status

Angenommen

## Kategorie

Boot / Recovery / NovaDOS / Offline-Wartung / Diagnose / Self-Healing / Resilienz

## Kontext

NovaOS benötigt neben dem normalen Systembetrieb eine robuste Wartungs- und Wiederherstellungsumgebung, die auch dann verfügbar bleibt, wenn das eigentliche NovaOS nicht mehr regulär startet.

Typische Fehlerfälle sind:

- beschädigte Systemgeneration,
- fehlerhafte Bootkonfiguration,
- defektes Update,
- beschädigtes Dateisystem,
- fehlerhafte Treiber,
- Probleme mit Bootloader oder Kernel,
- nicht erreichbarer grafischer Desktop,
- beschädigte Systemdienste,
- notwendige Offline-Reparaturen.

Eine Recovery-Umgebung, die vollständig vom normalen NovaOS abhängt, kann in genau diesen Situationen unbrauchbar werden.

Mit `ADR-BOOT-0008_Recovery_als_First_Class_Bootmodus` ist Recovery bereits als regulärer Bootmodus definiert.

NovaDOS soll innerhalb dieser Architektur die Rolle einer kleinen, eigenständigen und offline-fähigen Wartungsumgebung übernehmen.

---

## Entscheidung

NovaDOS wird als **eigenständige Offline-Recovery- und Maintenance-Umgebung von NovaOS** definiert.

NovaDOS ist kein normaler NovaOS-Benutzermodus und kein Ersatz für das reguläre Betriebssystem.

Es ist ein separates Bootziel mit eigener minimaler Runtime, das unabhängig von der normalen NovaOS-Systemgeneration arbeiten kann.

Das Grundmodell lautet:

```text
Nova Boot Environment
        ↓
Boot Mode Selection
        │
        ├── Normal NovaOS
        ├── Recovery
        ├── Safe Mode
        ├── Diagnostic Mode
        └── NovaDOS
                ↓
        Offline Maintenance
```

NovaDOS muss auch dann nutzbar bleiben, wenn die normale NovaOS-Systeminstallation nicht erfolgreich gestartet werden kann.

---

## Grundprinzip

Es gilt:

```text
NovaDOS is outside
the normal runtime,

but inside
the NovaOS trust architecture.
```

und:

```text
Recovery must not depend
on the system it is repairing.
```

---

## Rolle von NovaDOS

NovaDOS dient insbesondere für:

- Bootdiagnose,
- Reparatur von Bootmetadaten,
- Prüfung und Reparatur von Dateisystemen,
- Verwaltung von Systemgenerationen,
- A/B-Slot-Diagnose,
- Known-Good-Rollback,
- Integritätsprüfung,
- Backup und Wiederherstellung,
- Storage-Diagnose,
- grundlegende Hardwarediagnose,
- Recovery von Verschlüsselungs- und Trust-Konfigurationen,
- manuelle Wartungsarbeiten.

NovaDOS ist damit die tiefere Wartungsebene unterhalb der normalen NovaOS-Recovery-Oberfläche.

---

## Verhältnis zu Recovery

Recovery und NovaDOS sind nicht identisch.

```text
Recovery
    =
Bootmodus und Wiederherstellungsstrategie

NovaDOS
    =
konkrete Offline-Wartungsumgebung
```

Recovery kann NovaDOS als Wartungsziel starten.

NovaDOS kann jedoch auch direkt aus dem Bootmenü gestartet werden.

---

## Unabhängigkeit vom normalen NovaOS

NovaDOS darf nicht voraussetzen, dass:

- das normale NovaOS Root-Dateisystem funktioniert,
- normale NovaOS-Systemdienste laufen,
- die Desktopumgebung verfügbar ist,
- die aktuelle Systemgeneration intakt ist.

Notwendige NovaDOS-Komponenten müssen separat verfügbar sein.

---

## Minimale Architektur

NovaDOS soll bewusst klein gehalten werden.

Die Umgebung enthält nur die für Diagnose und Wartung erforderlichen Komponenten.

Dazu gehören mindestens:

- eigener Kernel beziehungsweise eigene minimale Runtime,
- Speicherverwaltung,
- Scheduler,
- grundlegende I/O-Infrastruktur,
- Storagezugriff,
- VFS,
- relevante Dateisystemtreiber,
- Shell,
- Recovery- und Diagnosewerkzeuge.

Nicht erforderliche Desktop- oder Anwendungsframeworks gehören nicht in die Baseline.

---

## Native Ausführung

NovaDOS soll als native Offline-Umgebung ausgeführt werden.

Es ist nicht lediglich:

- ein Shellprozess innerhalb von NovaOS,
- eine versteckte Desktopanwendung,
- ein Container innerhalb des beschädigten Systems.

Dadurch bleibt seine Funktionsfähigkeit von der normalen NovaOS-Runtime unabhängig.

---

## Bootziel

NovaDOS wird als reguläres Boot Target modelliert.

Konzeptionell:

```text
BootTarget {
    mode = NovaDOS
    image = NovaDOS
    trustPolicy = NovaDOSRecoveryPolicy
}
```

Es verwendet die gemeinsame NovaOS-Bootarchitektur für:

- Auswahl,
- Laden,
- Verifikation,
- BootInfo,
- Kernel Handoff.

---

## Trusted Boot

NovaDOS muss vollständig in die vertrauenswürdige Bootkette integriert sein.

Es gilt nicht:

```text
Recovery system
=
unsigned system
```

NovaDOS-Komponenten müssen entsprechend der Recovery Trust Policy:

- identifiziert,
- auf Integrität geprüft,
- verifiziert,
- autorisiert

werden.

---

## Eigene Trust Policy

NovaDOS darf eine eigene Trust Policy besitzen.

Beispielsweise kann ein eigener Recovery Signing Key vorgesehen werden.

Dies erlaubt eine Trennung zwischen:

```text
Production System Authority
```

und:

```text
Recovery Authority
```

Recovery Trust darf jedoch kein allgemeiner Bypass der NovaOS-Sicherheitsarchitektur sein.

---

## Measured Boot

NovaDOS erzeugt einen eigenen messbaren Bootzustand.

Ein TPM- beziehungsweise Attestation-System muss unterscheiden können zwischen:

```text
Normal NovaOS
```

und:

```text
NovaDOS
```

Dies ist insbesondere für Secret Sealing relevant.

---

## Zugriff auf verschlüsselte Daten

NovaDOS erhält nicht automatisch Zugriff auf alle verschlüsselten Benutzer- oder Systemdaten.

Freigabe kann abhängig sein von:

- Recovery Key,
- Benutzercredential,
- Administratorcredential,
- TPM Policy,
- Enterprise Recovery Policy.

Damit verhindert NovaOS, dass der Recovery-Modus zu einem automatischen Entschlüsselungs-Bypass wird.

---

## Offline-First

NovaDOS muss seine grundlegenden Recovery-Funktionen vollständig ohne Netzwerk ausführen können.

Das betrifft insbesondere:

- Dateisystemdiagnose,
- Bootreparatur,
- Slotverwaltung,
- Known-Good-Rollback,
- lokale Integritätsprüfung,
- Storage-Diagnose.

Netzwerk darf optional verfügbar sein.

---

## Netzwerk als Erweiterung

Optional kann NovaDOS Netzwerkfunktionen für:

- Download signierter Recovery-Artefakte,
- Update-Reparatur,
- Remote-Support,
- Enterprise-Recovery

bereitstellen.

Netzwerkzugriff darf jedoch nicht Voraussetzung für die Baseline-Funktionalität sein.

---

## Dateisystemzugriff

NovaDOS benötigt direkten Zugriff auf relevante NovaOS-Dateisysteme.

Die Baseline soll mindestens die für NovaOS-System- und Recovery-Speicher tatsächlich verwendeten Dateisysteme unterstützen.

Zusätzliche Dateisystemmodule können modular geladen werden.

---

## Storage

Für moderne Systeme müssen mindestens die für NovaOS benötigten Storagepfade unterstützt werden können.

Dazu gehören abhängig von Plattform und Entwicklungsstand:

```text
AHCI
NVMe
USB Storage
```

Weitere Provider können später ergänzt werden.

---

## VFS

NovaDOS verwendet eine eigene VFS-Abstraktion.

Recovery-Werkzeuge dürfen dadurch unabhängig vom konkreten Dateisystem arbeiten, soweit dessen Semantik dies zulässt.

---

## Volumes

NovaDOS übernimmt das NovaOS-Prinzip benannter beziehungsweise identifizierter Volumes statt klassischer Laufwerksbuchstaben.

Beispiele:

```text
System:/
Boot:/
Recovery:/
Data:/
```

Die tatsächliche Zuordnung erfolgt über erkannte Volume-Identitäten.

---

## Shell

Die primäre NovaDOS-Schnittstelle ist eine textbasierte Shell.

Sie soll:

- Befehle,
- Skripte,
- Diagnoseausgaben,
- interaktive Recovery

unterstützen.

Eine grafische Oberfläche ist für die Baseline nicht erforderlich.

---

## Shell als Wartungsschnittstelle

Die Shell bietet Zugriff auf Recovery-Capabilities und darf nicht lediglich rohe Hardwarefunktionen ohne Sicherheitsmodell exponieren.

Beispiele:

```text
boot status
boot rollback
disk check
system verify
generation list
generation activate
recovery log
```

Die genaue Syntax gehört in separate NovaDOS-Spezifikationen.

---

## Skripting

NovaDOS darf Wartungsskripte unterstützen.

Skripting muss für reproduzierbare und automatisierbare Recovery-Abläufe geeignet sein.

Skripte unterliegen denselben Berechtigungs- und Trustregeln wie interaktive Operationen.

---

## Recovery Capabilities

NovaDOS erhält explizite Wartungsfähigkeiten.

Beispiele:

```text
InspectBootState
RepairBootState
InspectStorage
RepairFilesystem
VerifySystemGeneration
ActivateKnownGood
ManageRecoveryConfiguration
```

Nicht jede NovaDOS-Komponente benötigt automatisch alle Fähigkeiten.

---

## A/B-Boot-Integration

NovaDOS muss den Zustand der in `ADR-BOOT-0009` definierten Bootslots diagnostizieren können.

Beispiel:

```text
Slot A
Generation 41
KnownGood

Slot B
Generation 42
Failed
```

Autorisierte Recoveryaktionen können unter anderem:

- Candidate deaktivieren,
- Known-Good aktivieren,
- Bootmetadaten reparieren

umfassen.

---

## Boot Health Integration

NovaDOS kann die in `ADR-BOOT-0010` gespeicherten Boot Health Milestones auswerten.

Beispiel:

```text
Last Attempt:
    Generation 42

Last Milestone:
    SystemRootReady

Failure:
    CriticalServicesReady
```

Damit kann die Fehlerphase gezielt diagnostiziert werden.

---

## Self-Healing

Automatisches Self-Healing kann bestimmte NovaDOS-Funktionen verwenden.

Dabei gilt jedoch:

```text
Automatic repair
requires explicit policy.
```

NovaDOS darf keine irreversiblen Änderungen allein aufgrund unsicherer Heuristik durchführen.

---

## Schreibzugriffe

Recovery-Werkzeuge sollen nach Möglichkeit zunächst diagnostisch arbeiten.

Schreibende beziehungsweise destruktive Aktionen müssen klar von reinen Leseoperationen getrennt sein.

Beispiele:

```text
inspect
verify
diagnose
```

gegen:

```text
repair
rollback
format
rewrite
```

Kritische Operationen können explizite Bestätigung oder zusätzliche Autorisierung verlangen.

---

## Transaktionale Reparaturen

Soweit technisch möglich, sollen Reparaturen transaktional erfolgen.

Konzeptionell:

```text
Inspect
   ↓
Plan
   ↓
Validate
   ↓
Apply
   ↓
Verify
   ↓
Commit
```

Bei Fehlern soll ein klar definierter Recovery- oder Rollbackpfad bestehen.

---

## Keine versteckten Automatismen

NovaDOS darf keine sicherheitskritischen Änderungen allein durch das Öffnen der Recovery-Umgebung ausführen.

Automatische Aktionen müssen durch Policy klar definiert sein.

---

## Diagnosemodus

NovaDOS soll einen rein diagnostischen Modus unterstützen können, in dem keine schreibenden Systemänderungen erlaubt sind.

Dies erleichtert:

- Forensik,
- Fehleranalyse,
- sichere Remote-Unterstützung.

---

## Hardwarediagnose

NovaDOS darf grundlegende Hardwaretests bereitstellen.

Beispiele:

- Speicherprüfung,
- Storageprüfung,
- Geräteerkennung,
- grundlegende CPU-/Plattformdiagnose.

Es soll jedoch kein vollständiger Ersatz für spezialisierte Hardwarediagnostik sein.

---

## Bootloader-Wartung

NovaDOS darf Werkzeuge für die Diagnose und Reparatur der NovaOS-Bootumgebung besitzen.

Dazu können gehören:

- Bootkonfiguration,
- Boot State,
- Systemgenerationen,
- Recovery-Metadaten,
- verifizierte Bootartefakte.

Firmwarekritische Änderungen bleiben besonders geschützt.

---

## Trust Store Recovery

NovaDOS darf eine kontrollierte Wiederherstellung beschädigter Trust-Metadaten unterstützen.

Dabei darf es Trust Roots nicht eigenmächtig ersetzen.

Änderungen erfordern eine autorisierte Recovery Policy beziehungsweise geeignete Recovery Credentials.

---

## Logs und Introspection

NovaDOS muss relevante Boot- und Recoveryinformationen lesen können.

Dazu gehören insbesondere:

- Boot Logs,
- Recovery Logs,
- Health Milestones,
- Slotzustände,
- Generationen,
- Trust-Fehler,
- Storagefehler.

---

## Eigene Logs

NovaDOS schreibt eigene strukturierte Recovery Logs.

Beispiel:

```text
NovaDOS started
Reason: manual recovery

Generation 42:
    verification failed

Generation 41:
    known-good

Action:
    activate generation 41

Result:
    successful
```

---

## Audit

Sicherheitsrelevante Aktionen müssen auditierbar sein.

Dazu gehören insbesondere:

- Rollback,
- Boot-State-Änderung,
- Trust-Konfigurationsänderung,
- Dateisystemreparatur,
- Recovery-Key-Nutzung,
- Formatierung,
- Löschung.

---

## Kein Login als Baseline

NovaDOS benötigt nicht zwingend einen normalen Mehrbenutzer-Login wie das reguläre NovaOS.

Zugriff auf sensitive Recoveryoperationen kann jedoch zusätzliche Credentials oder physische Autorisierung verlangen.

Die Recovery Security Policy entscheidet darüber.

---

## Isolation vom normalen System

NovaDOS darf Komponenten der beschädigten normalen Systemgeneration nicht automatisch als ausführbaren Code laden.

Daten dürfen untersucht werden.

Code aus der zu reparierenden Generation darf nur ausgeführt werden, wenn dies ausdrücklich verifiziert und autorisiert wurde.

---

## Kein Plugin-Autoload aus beschädigtem System

NovaDOS darf insbesondere keine normalen Treiber, Erweiterungen oder Plugins allein deshalb laden, weil sie auf der normalen Systempartition vorhanden sind.

Dies schützt die Recovery-Umgebung vor kompromittierten oder defekten Komponenten.

---

## Aktualisierung von NovaDOS

NovaDOS selbst muss aktualisierbar sein.

Updates der Recovery-Umgebung müssen:

- signiert,
- verifiziert,
- transaktional aktiviert

werden.

Eine funktionierende vorherige Recovery-Version sollte nach Möglichkeit als Fallback erhalten bleiben.

---

## Unabhängige Versionierung

NovaDOS darf unabhängig von der normalen NovaOS-Systemgeneration versioniert werden.

Beispiel:

```text
NovaOS Generation:
    42

NovaDOS Version:
    0.30
```

Kompatibilitätsanforderungen zwischen beiden müssen explizit definiert werden.

---

## Ressourcenbedarf

NovaDOS soll deutlich weniger Ressourcen als das vollständige NovaOS benötigen.

Dies verbessert die Nutzbarkeit:

- auf beschädigten Systemen,
- bei reduziertem Arbeitsspeicher,
- in virtuellen Maschinen,
- auf älteren Plattformen.

---

## Determinismus

Recoveryentscheidungen und Reparaturwerkzeuge sollen bei identischem Zustand deterministische Ergebnisse liefern, soweit die zugrunde liegende Hardware dies erlaubt.

Automatische Recovery darf nicht von probabilistischen KI-Entscheidungen abhängig sein.

---

## KI-Unterstützung

NovaDOS darf optional Assistenzfunktionen besitzen, beispielsweise:

- natürlichsprachliche Erklärung von Fehlern,
- Zusammenfassung von Logs,
- Vorschläge für Recoverybefehle.

Der Assistent darf nicht die eigentliche Recovery Authority sein.

Kritische Aktionen müssen weiterhin über deterministische Recovery-Mechanismen und explizite Berechtigungen erfolgen.

---

## Normative Anforderungen

1. NovaDOS MUSS als eigenständige Offline-Recovery-Umgebung von NovaOS behandelt werden.
2. NovaDOS MUSS als reguläres Boot Target startbar sein.
3. NovaDOS DARF NICHT vom erfolgreichen Start der normalen NovaOS-Systemgeneration abhängen.
4. NovaDOS MUSS in die gemeinsame NovaOS-Bootarchitektur integriert sein.
5. NovaDOS MUSS in die Trusted Boot Chain integriert sein.
6. NovaDOS MUSS eine eigene Recovery Trust Policy besitzen können.
7. NovaDOS MUSS einen vom normalen NovaOS unterscheidbaren Measured-Boot-Zustand erzeugen können.
8. NovaDOS DARF nicht automatisch Zugriff auf alle verschlüsselten Secrets erhalten.
9. Grundlegende NovaDOS-Recovery MUSS offline funktionieren.
10. Netzwerkfunktionen DÜRFEN optional ergänzt werden.
11. NovaDOS MUSS Storage- und Dateisystemzugriff für die relevanten NovaOS-Systemmedien bereitstellen können.
12. NovaDOS MUSS eine VFS-Abstraktion verwenden.
13. NovaDOS MUSS benannte beziehungsweise identifizierte Volumes unterstützen.
14. NovaDOS MUSS eine textbasierte Wartungsschnittstelle bereitstellen.
15. Recoveryoperationen MÜSSEN capabilitybasiert kontrollierbar sein.
16. NovaDOS MUSS A/B-Slot- und Generationenzustände diagnostizieren können.
17. NovaDOS MUSS autorisierten Known-Good-Rollback unterstützen können.
18. NovaDOS SOLL Boot Health Milestones auswerten können.
19. Schreibende und rein diagnostische Operationen MÜSSEN unterscheidbar sein.
20. Kritische destruktive Operationen MÜSSEN explizite Autorisierung unterstützen.
21. Reparaturen SOLLEN soweit möglich transaktional durchgeführt werden.
22. NovaDOS SOLL einen Read-Only-Diagnosemodus unterstützen.
23. NovaDOS DARF keine normalen Systemkomponenten aus einer beschädigten Generation ungeprüft ausführen.
24. Treiber oder Plugins aus der normalen Installation DÜRFEN NICHT ungeprüft automatisch geladen werden.
25. NovaDOS MUSS relevante Boot-, Trust-, Health- und Recoverylogs auswerten können.
26. NovaDOS MUSS eigene Recoveryaktionen protokollieren.
27. Sicherheitsrelevante NovaDOS-Aktionen MÜSSEN auditierbar sein.
28. NovaDOS MUSS unabhängig vom normalen NovaOS versionierbar sein.
29. NovaDOS-Updates MÜSSEN verifiziert und kontrolliert aktiviert werden.
30. NovaDOS SOLL einen möglichst kleinen Ressourcen-Footprint besitzen.
31. Recovery-Kernfunktionen MÜSSEN ohne KI korrekt funktionieren.
32. KI DARF keine Trust-, Verschlüsselungs- oder Recovery-Policy umgehen.
33. NovaDOS MUSS als tiefe Wartungsebene unterhalb der normalen NovaOS-Recovery-Architektur dienen.
34. NovaDOS DARF nicht zu einem allgemeinen Sicherheits-Bypass für das normale NovaOS werden.

---

## Konsequenzen

### Positive Konsequenzen

- NovaOS besitzt eine Recovery-Umgebung, die unabhängig vom normalen System funktioniert.
- beschädigte Systemgenerationen können offline untersucht und repariert werden.
- A/B-Boot, Known-Good und Boot Health lassen sich direkt diagnostizieren.
- Recovery bleibt auch ohne Netzwerk verfügbar.
- NovaDOS kann klein, robust und gezielt testbar gehalten werden.
- Trust- und Verschlüsselungsgrenzen bleiben auch im Recovery-Modus erhalten.
- dieselbe Umgebung kann für automatisierte und manuelle Wartung verwendet werden.

### Negative Konsequenzen

- NovaDOS benötigt eigene Kernel-/Runtime-, Treiber- und Dateisystemkomponenten.
- Recovery-Komponenten müssen separat gepflegt und getestet werden.
- zwischen NovaDOS und NovaOS entstehen versionierte Kompatibilitätsgrenzen.
- moderne Storage- und Dateisystemunterstützung erhöht den Umfang der Recovery-Umgebung.

---

## Verworfene Alternativen

### Recovery ausschließlich innerhalb des normalen NovaOS

Verworfen.

Ein beschädigtes NovaOS könnte seine eigene Recovery-Umgebung unbrauchbar machen.

### Vollständiges NovaOS als Recovery-System starten

Verworfen.

Dies würde unnötig viele Abhängigkeiten und Fehlerquellen in den Recoverypfad übernehmen.

### Unsignierte Wartungsumgebung

Verworfen.

Eine privilegierte Recovery-Umgebung darf die Trusted Boot Chain nicht umgehen.

### Automatischer Vollzugriff auf verschlüsselte Volumes

Verworfen.

Recovery darf keinen impliziten Entschlüsselungs-Bypass erzeugen.

### Ausschließlich Cloud-basierte Recovery

Verworfen.

Die grundlegende Wiederherstellung muss offline funktionieren.

### NovaDOS als bloße Bootloader-Shell

Verworfen.

Für Storage-, Dateisystem- und Systemreparatur wird eine eigenständige Wartungsruntime benötigt.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-BOOT-0003_Mehrstufige_Bootarchitektur`
- `ADR-BOOT-0005_Vertrauenswürdige_Bootkette`
- `ADR-BOOT-0006_Secure_Measured_Verified_Boot_Integration`
- `ADR-BOOT-0007_TPM_Integration_in_die_Bootkette`
- `ADR-BOOT-0008_Recovery_als_First_Class_Bootmodus`
- `ADR-BOOT-0009_A_B_Boot_und_Boot_Rollback`
- `ADR-BOOT-0010_Boot_Health_Milestones_vor_Commit`
- `ADR-ARCH-0004_Transaktionale_Systemoperationen`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`
- `ADR-TRUST-0001_Nova_Trust`

---

## Zugehörige NPSPECs

- `NPSPEC-NOVADOS-ARCH-0001`
- `NPSPEC-NOVADOS-BOOT-0001`
- `NPSPEC-NOVADOS-KERNEL-0001`
- `NPSPEC-NOVADOS-STORAGE-0001`
- `NPSPEC-NOVADOS-VFS-0001`
- `NPSPEC-NOVADOS-FILESYSTEM-0001`
- `NPSPEC-NOVADOS-SHELL-0001`
- `NPSPEC-NOVADOS-RECOVERY-0001`
- `NPSPEC-NOVADOS-CAPABILITY-0001`
- `NPSPEC-NOVADOS-TRUST-0001`
- `NPSPEC-NOVADOS-ENCRYPTION-RECOVERY-0001`
- `NPSPEC-NOVADOS-GENERATION-0001`
- `NPSPEC-NOVADOS-BOOTSTATE-0001`
- `NPSPEC-NOVADOS-DIAGNOSTIC-0001`
- `NPSPEC-NOVADOS-LOG-0001`
- `NPSPEC-NOVADOS-UPDATE-0001`
- `NPSPEC-NOVADOS-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- direkter Start von NovaDOS aus dem Bootmenü,
- Start von NovaDOS aus Recovery,
- normale NovaOS-Systemgeneration vollständig beschädigt,
- NovaDOS weiterhin bootfähig,
- ungültige NovaDOS-Signatur,
- Offline-Betrieb ohne Netzwerk,
- Erkennung von AHCI-/NVMe-/USB-Speicher,
- Zugriff auf relevante NovaOS-Dateisysteme,
- A/B-Slot-Diagnose,
- Known-Good-Rollback,
- Boot Health Log Analyse,
- Read-Only-Diagnosemodus,
- Dateisystemreparatur,
- beschädigte Bootmetadaten,
- verschlüsseltes Volume ohne Recovery Credential,
- autorisierte Recovery-Key-Nutzung,
- verhindert ausgeführter Code aus kompromittierter Systemgeneration,
- Recovery Logging,
- NovaDOS-Update,
- fehlgeschlagenes NovaDOS-Update mit weiterhin bootfähiger vorheriger Recovery-Version.

---

## Ergebnis

NovaDOS ist die eigenständige, kleine und offline-fähige Wartungsumgebung von NovaOS.

Das Architekturmodell lautet:

```text
Nova Boot Environment
        ↓
Recovery Decision
        ↓
NovaDOS
        ↓
Offline Diagnosis
        ↓
Repair / Rollback / Recovery
        ↓
Verify
        ↓
Return to normal boot
```

NovaDOS bleibt technisch vom normalen NovaOS getrennt, verwendet jedoch dieselben übergeordneten Trust-, Recovery- und Architekturprinzipien.

Die zentrale Architekturregel lautet:

```text
Keep recovery independent
from the system being repaired.

Keep it small.

Keep it trusted.

Keep it offline-capable.
``` 