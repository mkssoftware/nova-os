# ADR-STORAGE-0020 – Globales System nur mit erhöhter Berechtigung

## Status

Angenommen

## Kategorie

Storage / System Integrity / Permissions / Application Isolation

## Kontext

NovaOS erlaubt Anwendungen über den privaten Application-`SYS`-Namespace, eigene Systemabhängigkeiten bereitzustellen, ohne den globalen Systembereich zu verändern.

Damit besteht für normale Anwendungen grundsätzlich kein technischer Grund, Bibliotheken oder andere Abhängigkeiten direkt im globalen System-Namespace zu installieren.

Änderungen am globalen System betreffen jedoch potenziell alle Anwendungen und können Sicherheit, Stabilität, Bootfähigkeit und Systemintegrität beeinflussen.

Der globale Systembereich muss deshalb eine besonders geschützte Storage-Domäne darstellen.

---

## Entscheidung

Schreibende Änderungen am globalen System-Namespace erfordern in NovaOS eine **explizite erhöhte Berechtigung**.

```text
Write Request
     ↓
Global System?
 ├── No  → Normal Authority Model
 └── Yes → Elevated System Authority
```

Normale Application Permissions reichen für Änderungen am globalen System nicht aus.

---

## Grundprinzip

```text
Application-private changes are local.

System-wide changes require system authority.
```

---

## Geschützter Systembereich

Der globale Systembereich umfasst die systemweit gültigen Komponenten und Ressourcen von NovaOS.

Dazu können insbesondere gehören:

- Systembibliotheken,
- Systemdienste,
- Runtime-Komponenten,
- Treiber,
- systemweite Capabilities,
- systemkritische Konfigurationen.

Lesender Zugriff kann abhängig von der jeweiligen Ressource normal zulässig sein.

Schreibender oder strukturell verändernder Zugriff benötigt dagegen entsprechende System Authority.

---

## Application SYS als Standardweg

Anwendungen sollen private Abhängigkeiten grundsätzlich in ihrem eigenen `SYS`-Namespace bereitstellen.

```text
Application Dependency
        ↓
Application SYS
```

statt:

```text
Application Dependency
        ↓
Global System
```

Eine Anwendung darf eine globale Installation nicht allein deshalb verlangen, weil sie eine bestimmte Library-Version benötigt.

---

## Erhöhte Berechtigung

Eine Änderung am globalen System muss als explizite privilegierte Systemoperation behandelt werden.

```text
Request
   ↓
Authority Check
   ↓
Policy Validation
   ↓
Authorized System Operation
```

Die konkrete Benutzerinteraktion, Authentifizierung und Vergabe der Berechtigung wird durch die Security- und Permission-Spezifikationen definiert.

---

## Transaktionale Änderungen

Autorisierte Änderungen am globalen System sollen über Transactional Storage beziehungsweise transaktionale Systemoperationen erfolgen.

```text
Current System
      ↓
Authorized Transaction
      ↓
New System State
```

Fehler dürfen nicht unnötig zu teilweise aktualisierten Systemzuständen führen.

Systemkritische Änderungen können zusätzlich mit Snapshots und Rollback abgesichert werden.

---

## Keine implizite Privilegienerhöhung

Eine Anwendung erhält keine System Authority durch:

- ihren Installationsort,
- ihren Namen,
- ihren Publisher,
- ihren `SYS`-Overlay,
- einen System-Namespace-Pfad,
- eine normale Application Capability.

```text
Namespace Position ≠ Authority
```

System Authority muss ausdrücklich vorhanden sein.

---

## Systemdienste und Installer

Autorisierte Systemdienste dürfen globale Änderungen im Auftrag eines Benutzers oder einer anderen autorisierten Instanz durchführen.

Dabei bleibt der eigentliche schreibende Zugriff bei der privilegierten Systemkomponente.

```text
Application
     ↓
System Change Request
     ↓
Privileged System Service
     ↓
Global System
```

Dies verhindert, dass Anwendungen selbst dauerhaft mit weitreichenden Systemrechten ausgeführt werden müssen.

---

## Security und Trust

Erhöhte Berechtigung ersetzt keine weiteren Sicherheitsprüfungen.

Eine globale Systemänderung kann zusätzlich Anforderungen an:

- Code Identity,
- Signature,
- Provenance,
- Trust,
- Capability,
- Data Sovereignty

besitzen.

Nova.Trust und das Permission-System bleiben damit Bestandteil der Entscheidung.

---

## Recovery

Änderungen an boot- oder systemkritischen Komponenten sollen mit den vorhandenen Snapshot-, Rollback- und Recovery-Mechanismen kombinierbar sein.

Eine erfolgreiche Autorisierung allein garantiert nicht, dass eine Änderung technisch sicher durchgeführt werden kann.

---

## Normative Anforderungen

1. NovaOS MUSS den globalen Systembereich als besonders geschützte Storage-Domäne behandeln.
2. Schreibende Änderungen am globalen System MÜSSEN erhöhte System Authority erfordern.
3. Normale Application Permissions DÜRFEN globale Systemänderungen NICHT automatisch erlauben.
4. Anwendungen SOLLEN private Abhängigkeiten über ihren Application-`SYS`-Namespace bereitstellen.
5. Ein Application-`SYS`-Overlay DARF KEINE globale System Authority verleihen.
6. Namespace-Position, Dateiname oder Installationsort DÜRFEN NICHT als Berechtigungsnachweis dienen.
7. Globale Systemänderungen SOLLEN über autorisierte Systemdienste ausgeführt werden können.
8. Anwendungen MÜSSEN dadurch globale Änderungen anfordern können, ohne selbst dauerhaft System Authority zu besitzen.
9. Globale Systemänderungen SOLLEN transaktional ausgeführt werden.
10. Systemkritische Änderungen SOLLEN mit Snapshot und Rollback kombinierbar sein.
11. Erhöhte Berechtigung DARF zusätzliche Security-, Trust- oder Data-Sovereignty-Prüfungen NICHT umgehen.
12. Nicht autorisierte Änderungen am globalen System MÜSSEN nach Fail-Safe-Default abgelehnt werden.
13. Globale Änderungen MÜSSEN auditierbar sein.
14. Die tatsächlich verwendete Authority MUSS kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Schutz der globalen Systemintegrität,
- Anwendungen benötigen seltener weitreichende Rechte,
- private Dependencies bleiben von globalen Komponenten getrennt,
- kontrollierbare Systeminstallationen und Updates,
- bessere Grundlage für Audit, Recovery und Self-Healing.

### Negative Konsequenzen

- globale Installationen benötigen zusätzliche Autorisierung,
- privilegierte Systemdienste müssen besonders geschützt werden,
- systemweite Änderungen besitzen einen höheren administrativen Aufwand.

---

## Verworfene Alternativen

### Anwendungen dürfen `/System` direkt verändern

Verworfen.

Dies würde Isolation, Systemintegrität und Updatefähigkeit erheblich schwächen.

### Installierte Anwendungen erhalten automatisch Systemrechte

Verworfen.

Installation und System Authority sind getrennte Sicherheitsentscheidungen.

### Globales System vollständig unveränderlich

Verworfen.

NovaOS muss kontrollierte Updates, Treiberinstallationen und andere legitime Systemänderungen ermöglichen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0004_Transactional_Storage`
- `ADR-STORAGE-0013_Transaktionale_Dateioperationen`
- `ADR-STORAGE-0014_Snapshots_und_Rollback`
- `ADR-STORAGE-0018_Per_Application_System_Overlay`
- `ADR-STORAGE-0019_Private_Systemabhängigkeiten_im_Application_SYS_Namespace`
- `ADR-ARCH-0004_Transactionale_Systemoperationen`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-SYSTEM-AUTHORITY-0001`
- `NPSPEC-STORAGE-SYSTEM-WRITE-0001`
- `NPSPEC-STORAGE-SYSTEM-CHANGE-0001`
- `NPSPEC-STORAGE-SYSTEM-TRANSACTION-0001`
- `NPSPEC-STORAGE-SYSTEM-AUDIT-0001`
- `NPSPEC-STORAGE-SYSTEM-AUTHORITY-TEST-0001`

---

## Ergebnis

NovaOS trennt normale Anwendungsänderungen klar von systemweiten Änderungen:

```text
Application
 ├── Private Dependencies → Application SYS
 │
 └── Global Change
          ↓
   Elevated Authority
          ↓
   Transactional Change
          ↓
      Global System
```

Damit bleibt das globale System geschützt, ohne legitime systemweite Änderungen grundsätzlich zu verhindern.

Die zentrale Architekturregel lautet:

```text
Private by default.

Global only with explicit authority.
```