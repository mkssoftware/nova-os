# ADR-STORAGE-0018 – Per-Application System Overlay

## Status

Angenommen

## Kategorie

Storage / Application Isolation / System Namespace / Dependencies / VFS

## Kontext

Anwendungen benötigen teilweise Systemkomponenten, Bibliotheken, Ressourcen oder Laufzeitabhängigkeiten, deren Version von der global installierten Systemversion abweicht.

Eine direkte Installation solcher privaten Abhängigkeiten in den globalen `/System`-Bereich würde:

- Anwendungen unnötig miteinander koppeln,
- Versionskonflikte erzeugen,
- Systemupdates erschweren,
- die Integrität des globalen Systems gefährden.

Eine vollständige Kopie des gesamten Systembereichs pro Anwendung wäre dagegen ineffizient.

NovaOS benötigt deshalb einen Mechanismus, mit dem Anwendungen private Systemabhängigkeiten verwenden können, ohne den globalen Systemzustand verändern zu müssen.

---

## Entscheidung

NovaOS unterstützt ein **Per-Application System Overlay**.

Jede Anwendung kann einen privaten `SYS`-Overlay besitzen, der logisch über den globalen System-Namespace gelegt wird.

```text
Application View

        SYS:/
          ↓
 ┌─────────────────┐
 │ Application SYS │
 ├─────────────────┤
 │ Global System   │
 └─────────────────┘
```

Private Komponenten erscheinen der Anwendung logisch im System-Namespace, bleiben physisch jedoch Bestandteil der Anwendung.

---

## Grundprinzip

```text
Private system dependencies
without modifying the global system.
```

---

## Overlay-Modell

Der effektive System-Namespace einer Anwendung entsteht aus mindestens zwei Ebenen:

```text
Application SYS Overlay
          ↓
Global System Namespace
```

Ist eine Ressource im Application Overlay vorhanden, kann sie innerhalb der dafür vorgesehenen Regeln die entsprechende globale Ressource für diese Anwendung überlagern.

Nicht überlagerte Ressourcen werden aus dem globalen System bereitgestellt.

---

## Physische Speicherung

Private Systemabhängigkeiten bleiben physisch im Storage-Bereich der jeweiligen Anwendung.

Beispiel:

```text
Apps:/
 └── ExampleApp/
      ├── Application
      └── SYS/
           └── Lib/
```

Aus Sicht der Anwendung kann eine Ressource dagegen beispielsweise als:

```text
SYS:/Lib/Example
```

erscheinen.

Der logische Namespace und der physische Storage-Ort sind damit bewusst getrennt.

---

## Globaler Systembereich

Das Application Overlay verändert den globalen Systemzustand nicht.

```text
Application Overlay
      ↓
Visible only in Application Context

Global System
      ↓
Unchanged
```

Eine Anwendung darf durch das Bereitstellen eines privaten Overlays keine globale Systemkomponente für andere Anwendungen ersetzen.

---

## Systemintegrität

Bestimmte Systemkomponenten dürfen nicht durch Application Overlays ersetzbar sein.

Insbesondere sicherheits- oder korrektheitskritische Komponenten können als:

```text
Non-Overridable
```

definiert werden.

Welche Systembereiche überlagerbar sind, wird durch System Contracts und Security Policy bestimmt.

---

## Abhängigkeiten und Versionierung

Application Overlays ermöglichen Anwendungen, gezielt eigene kompatible Versionen von Abhängigkeiten mitzuführen.

Dadurch kann beispielsweise:

```text
App A → Library v2
App B → Library v3
System → Library v4
```

parallel existieren, ohne dass die Anwendungen gegenseitig ihren Systemzustand verändern.

Dies unterstützt langfristige Anwendungskompatibilität und kontrollierte Systementwicklung.

---

## Updates

Ein globales Systemupdate darf private Application Overlays nicht automatisch verändern.

Umgekehrt darf ein Anwendungsupdate seinen eigenen Overlay aktualisieren, ohne dadurch den globalen Systemzustand zu verändern.

Damit bleiben:

```text
System Lifecycle
```

und:

```text
Application Lifecycle
```

weitgehend voneinander entkoppelt.

---

## Security und Trust

Application Overlays unterliegen denselben Security-, Trust- und Data-Sovereignty-Regeln wie andere Anwendungskomponenten.

Eine private Komponente erhält keine höheren Rechte, nur weil sie innerhalb des Application Context unter `SYS:/` erscheint.

```text
Namespace Position ≠ Privilege
```

Die Herkunft der tatsächlich verwendeten Komponente muss für Trust- und Audit-Systeme unterscheidbar bleiben.

---

## Änderungen am globalen System

Benötigt eine Anwendung tatsächlich eine Änderung des globalen Systembereichs, muss dies als separate privilegierte Systemoperation behandelt werden.

```text
Application Request
       ↓
Explicit System Authority
       ↓
Transactional System Operation
       ↓
Global System Change
```

Das Vorhandensein eines Application Overlays erteilt keine solche Berechtigung.

---

## Architecture Introspection

NovaOS muss den effektiven System-Namespace einer Anwendung kontrolliert introspektierbar machen.

Dabei muss erkennbar sein, ob eine verwendete Ressource aus:

- dem globalen System,
- dem Application Overlay

stammt.

Dies ist insbesondere für Debugging, Security, Dependency Analysis und Updates relevant.

---

## Normative Anforderungen

1. NovaOS MUSS Per-Application System Overlays unterstützen.
2. Anwendungen DÜRFEN private Systemabhängigkeiten innerhalb ihres eigenen Storage-Bereichs bereitstellen.
3. Private Abhängigkeiten DÜRFEN logisch im System-Namespace der jeweiligen Anwendung erscheinen.
4. Application Overlays DÜRFEN den globalen Systemzustand NICHT verändern.
5. Ein Overlay einer Anwendung DARF NICHT den System-Namespace anderer Anwendungen beeinflussen.
6. Nicht überlagerte Ressourcen SOLLEN aus dem globalen System-Namespace aufgelöst werden.
7. System Contracts MÜSSEN Bereiche als nicht überlagerbar definieren können.
8. Security-kritische Systemkomponenten DÜRFEN NICHT allein durch ein Application Overlay ersetzbar werden.
9. Namespace-Position DARF keine zusätzlichen Privilegien verleihen.
10. Application Overlay und globaler Systembereich MÜSSEN getrennte Lebenszyklen besitzen können.
11. Systemupdates DÜRFEN private Application Overlays NICHT unkontrolliert verändern.
12. Anwendungsupdates DÜRFEN ihren eigenen Overlay unabhängig aktualisieren.
13. Änderungen am globalen System MÜSSEN eine separate explizite Authority erfordern.
14. Die Herkunft einer effektiv verwendeten Systemressource MUSS kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- weniger Dependency- und Versionskonflikte,
- Anwendungen können private Systemabhängigkeiten mitführen,
- globale Systemintegrität bleibt erhalten,
- Anwendungen und System können unabhängig aktualisiert werden,
- ältere Anwendungen können länger kompatibel bleiben,
- keine vollständige Systemkopie pro Anwendung erforderlich.

### Negative Konsequenzen

- Namespace-Auflösung wird komplexer,
- mehrere Versionen derselben Bibliothek können Storage verbrauchen,
- Debugging muss die Herkunft einer Komponente berücksichtigen,
- Overlay-Regeln müssen klar zwischen erlaubten und geschützten Systembereichen unterscheiden.

---

## Verworfene Alternativen

### Jede Abhängigkeit global installieren

Verworfen.

Dies erzeugt Versionskonflikte und koppelt Anwendungen unnötig an den globalen Systemzustand.

### Vollständige Systemkopie pro Anwendung

Verworfen.

Dies würde unnötig Storage verbrauchen und Updates erschweren.

### Anwendung darf beliebige Systemkomponenten überlagern

Verworfen.

Sicherheits- und korrektheitskritische Systemkomponenten müssen vor unkontrolliertem Override geschützt bleiben.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0001_VFS_als_Kompatibilitäts_und_Projektionsschicht`
- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-STORAGE-0012_Named_Volumes_statt_Laufwerksbuchstaben`
- `ADR-STORAGE-0013_Transaktionale_Dateioperationen`
- `ADR-STORAGE-0017_Stabile_localeunabhängige_Dateinamenidentität`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvable_System_Architecture`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-SYS-OVERLAY-0001`
- `NPSPEC-STORAGE-SYS-OVERLAY-RESOLUTION-0001`
- `NPSPEC-STORAGE-SYS-OVERLAY-POLICY-0001`
- `NPSPEC-STORAGE-SYS-OVERLAY-SECURITY-0001`
- `NPSPEC-STORAGE-SYS-OVERLAY-UPDATE-0001`
- `NPSPEC-STORAGE-SYS-OVERLAY-TEST-0001`

---

## Ergebnis

Jede Anwendung kann eine eigene kontrollierte Sicht auf den System-Namespace besitzen:

```text
                 Application
                      ↓
               Effective SYS:/
                 /          \
       Private Overlay    Global System
              ↓
       Application Folder
```

Private Abhängigkeiten bleiben damit Bestandteil der Anwendung, obwohl sie für diese logisch wie Systemkomponenten erscheinen.

Die zentrale Architekturregel lautet:

```text
Overlay the system view.

Do not modify the system.
```