# ADR-STORAGE-0019 – Private Systemabhängigkeiten im Application SYS Namespace

## Status

Angenommen

## Kategorie

Storage / Applications / Dependencies / Namespace / System Overlay

## Kontext

`ADR-STORAGE-0018` definiert einen Per-Application System Overlay. Anwendungen können dadurch eine eigene Sicht auf den System-Namespace erhalten, ohne den globalen Systemzustand zu verändern.

Für private Systemabhängigkeiten muss zusätzlich festgelegt werden, wo diese logisch und physisch liegen.

Eine Anwendung soll Bibliotheken, Runtime-Komponenten, Ressourcen oder andere zulässige Systemabhängigkeiten mitführen können, ohne diese global installieren zu müssen.

Gleichzeitig sollen diese Komponenten aus Sicht der Anwendung dort erscheinen, wo auch entsprechende globale Systemkomponenten erwartet werden.

---

## Entscheidung

Private Systemabhängigkeiten einer Anwendung werden **physisch im Application Storage gespeichert und logisch in deren `SYS`-Namespace eingeblendet**.

```text
Apps:/
 └── ExampleApp/
      └── SYS/
           ├── Lib/
           ├── Runtime/
           └── Resources/
```

Innerhalb des Application Context werden diese Ressourcen als Bestandteil des effektiven System-Namespace aufgelöst:

```text
Application SYS
      ↓
Private Dependency
      ↓
Global System Fallback
```

Der `SYS`-Bereich einer Anwendung ist damit kein globaler Systemordner, sondern eine private Namespace-Ebene dieser Anwendung.

---

## Grundprinzip

```text
Application-private dependency
→ Application-private storage
→ System-compatible namespace view
```

---

## Physische Eigentümerschaft

Private Systemabhängigkeiten bleiben Bestandteil der Anwendung.

```text
Application
 ├── Program
 ├── Resources
 └── SYS
      └── Private Dependencies
```

Sie gehören nicht zum globalen `/System` und besitzen grundsätzlich denselben Lifecycle wie die Anwendung beziehungsweise das zugehörige Anwendungspaket.

Damit können Installation, Update und Entfernung einer Anwendung ihre privaten Abhängigkeiten gemeinsam verwalten.

---

## Logischer SYS-Namespace

Der Application `SYS` Namespace bildet die private Systemebene einer Anwendung.

Eine Ressource kann dadurch logisch beispielsweise unter:

```text
SYS:/Lib/Nova/Example
```

verfügbar sein, obwohl sie physisch innerhalb des Application Storage liegt.

Der tatsächliche Storage-Ort wird dadurch nicht zur Namespace-Identität.

---

## Namespace-Auflösung

Für überlagerbare Systemressourcen gilt grundsätzlich:

```text
Lookup SYS Resource
        ↓
Application SYS
        ↓
Found?
 ├── Yes → Private Resource
 └── No  → Global System Resource
```

Damit kann eine Anwendung gezielt einzelne Abhängigkeiten privat bereitstellen und alle übrigen Komponenten aus dem globalen System beziehen.

---

## Versionsisolation

Private Systemabhängigkeiten ermöglichen parallele Versionen derselben Komponente.

```text
Global System
└── Nova.Library v4

Application A SYS
└── Nova.Library v2

Application B SYS
└── Nova.Library v3
```

Jede Anwendung erhält innerhalb ihres eigenen Execution Context die für sie aufgelöste Version.

Die privaten Versionen beeinflussen andere Anwendungen nicht.

---

## Dependency Identity

Die Identität einer Abhängigkeit darf nicht ausschließlich aus ihrem physischen Pfad entstehen.

Abhängigkeiten sollen über stabile Komponenten-, Capability- oder Ressourcenidentitäten identifizierbar bleiben.

Dadurch können Namespace-Auflösung, Versionierung, Trust und Dependency Management unabhängig von der konkreten Storage-Struktur arbeiten.

---

## Systemgeschützte Komponenten

Nicht jede Ressource des globalen Systems darf privat ersetzt werden.

System Contracts definieren, welche Komponenten:

```text
Overlayable
```

und welche:

```text
System Protected
```

sind.

Kernel-, Security- oder andere kritische Systemkomponenten dürfen dadurch von normalen Anwendungen nicht über den privaten `SYS`-Namespace ersetzt werden.

---

## Updates und Entfernung

Private Systemabhängigkeiten folgen grundsätzlich dem Lifecycle der Anwendung.

```text
Application Install
      ↓
Private SYS Installed

Application Update
      ↓
Private SYS Updated

Application Removal
      ↓
Private SYS Removed
```

Gemeinsam genutzte globale Komponenten bleiben davon unberührt.

Installation und Update sollen über transaktionale Storage-Operationen erfolgen.

---

## Security und Trust

Eine private Systemabhängigkeit erhält durch ihre Position im `SYS`-Namespace keine Systemprivilegien.

```text
SYS Namespace
     ≠
System Authority
```

Code Identity, Signaturen, Provenance, Permissions und Capabilities werden weiterhin über die dafür vorgesehenen Security- und Nova.Trust-Mechanismen bewertet.

Die Herkunft einer geladenen Komponente muss eindeutig bestimmbar bleiben.

---

## Resource Economy

Private Abhängigkeiten können dazu führen, dass mehrere Anwendungen unterschiedliche Versionen derselben Komponente speichern.

CAS und andere zulässige Storage-Mechanismen dürfen identische immutable Inhalte physisch gemeinsam nutzen, sofern Security-, Trust- und Data-Sovereignty-Grenzen dies erlauben.

Die logische Eigentümerschaft der Anwendungen bleibt davon getrennt.

---

## Normative Anforderungen

1. Private Systemabhängigkeiten MÜSSEN physisch dem jeweiligen Application Storage zugeordnet sein.
2. Anwendungen MÜSSEN dafür einen privaten `SYS`-Namespace verwenden können.
3. Private `SYS`-Ressourcen DÜRFEN logisch wie Systemressourcen aufgelöst werden.
4. Nicht privat bereitgestellte Ressourcen SOLLEN auf den globalen System-Namespace zurückfallen.
5. Private Systemabhängigkeiten DÜRFEN den globalen `/System`-Bereich NICHT verändern.
6. Der private `SYS`-Namespace einer Anwendung DARF andere Anwendungen NICHT beeinflussen.
7. Mehrere Anwendungen MÜSSEN unterschiedliche Versionen derselben überlagerbaren Abhängigkeit verwenden können.
8. Abhängigkeitsidentitäten SOLLEN unabhängig vom physischen Storage-Pfad sein.
9. System Contracts MÜSSEN nicht überlagerbare Systemkomponenten definieren können.
10. Private `SYS`-Komponenten DÜRFEN durch ihre Namespace-Position KEINE zusätzlichen Privilegien erhalten.
11. Private Abhängigkeiten SOLLEN gemeinsam mit der Anwendung installierbar, aktualisierbar und entfernbar sein.
12. Installation und Update privater Abhängigkeiten SOLLEN transaktional erfolgen.
13. Physische Content-Wiederverwendung DARF die logische Anwendungsisolation NICHT aufheben.
14. Die tatsächliche Herkunft einer aufgelösten `SYS`-Komponente MUSS kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Anwendungen können benötigte Dependency-Versionen selbst mitführen,
- keine globalen Versionskonflikte,
- sauberer Application Lifecycle,
- globale Systeminstallation bleibt klein und kontrolliert,
- bestehende System-Namespace-Strukturen können weiterverwendet werden,
- CAS kann identische Inhalte trotzdem physisch deduplizieren.

### Negative Konsequenzen

- mehrere Dependency-Versionen können parallel existieren,
- Namespace-Auflösung benötigt einen Application Context,
- Dependency- und Security-Diagnose muss zwischen privaten und globalen Komponenten unterscheiden.

---

## Verworfene Alternativen

### Private Abhängigkeiten global installieren

Verworfen.

Dies würde Anwendungsversionen koppeln und globale Dependency-Konflikte erzeugen.

### Separater vollständig neuer Dependency-Namespace

Verworfen.

Anwendungen und Libraries sollen dieselben logischen Systempfade verwenden können, unabhängig davon, ob eine Komponente privat oder global bereitgestellt wird.

### Private SYS-Komponenten wie globale Systemkomponenten privilegieren

Verworfen.

Namespace und Authority müssen getrennte Architekturkonzepte bleiben.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-STORAGE-0003_Content_Addressed_Storage`
- `ADR-STORAGE-0013_Transaktionale_Dateioperationen`
- `ADR-STORAGE-0017_Stabile_localeunabhängige_Dateinamenidentität`
- `ADR-STORAGE-0018_Per_Application_System_Overlay`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvable_System_Architecture`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-APP-SYS-0001`
- `NPSPEC-STORAGE-APP-SYS-NAMESPACE-0001`
- `NPSPEC-STORAGE-APP-SYS-RESOLUTION-0001`
- `NPSPEC-STORAGE-APP-SYS-DEPENDENCY-0001`
- `NPSPEC-STORAGE-APP-SYS-SECURITY-0001`
- `NPSPEC-STORAGE-APP-SYS-TEST-0001`

---

## Ergebnis

Private Systemabhängigkeiten bleiben physisch Teil ihrer Anwendung, erscheinen für diese jedoch innerhalb des normalen System-Namespace:

```text
Application Storage
      ↓
Private SYS
      ↓
Effective SYS Namespace
      ↓
Global System Fallback
```

Damit verbindet NovaOS Dependency-Isolation mit einer einheitlichen System-Namespace-Semantik.

Die zentrale Architekturregel lautet:

```text
Private dependencies belong to the application.

Their namespace may look like the system.
```