# ADR-CAPABILITY-0013 – Dynamic Capabilities

## Status

Angenommen

## Kategorie

Capability Architecture / Dynamic Capabilities / Lifecycle

## Kontext

Die Menge verfügbarer NovaOS-Capabilities ist nicht statisch.

Capabilities können während des laufenden Systems entstehen, verschwinden oder ihre verfügbaren Provider ändern.

Ursachen sind beispielsweise:

- Installation oder Entfernung von Software,
- Start oder Ende eines Dienstes,
- Device Hotplug,
- Laden oder Ersetzen eines Treibers,
- Aktivierung eines Plugins,
- verfügbare Remote Services,
- temporäre Sessions,
- Hot Replacement,
- dynamisch erzeugte Capability Compositions.

NovaOS benötigt deshalb ein Capability-Modell, das Änderungen zur Laufzeit als normalen Systemzustand behandelt.

---

## Entscheidung

NovaOS unterstützt **Dynamic Capabilities als First-Class-Eigenschaft der Capability-Architektur**.

```text
System State
    ↓
Capability Availability Changes
    ↓
Capability Registry
    ↓
Discovery / Resolution
    ↓
Applications and Services
```

Capabilities und ihre Provider dürfen während des laufenden Systems hinzugefügt, aktualisiert, ersetzt oder entfernt werden.

Ein Neustart des Gesamtsystems darf dafür grundsätzlich nicht erforderlich sein.

---

## Grundprinzip

```text
Capability identity is stable.

Capability availability is dynamic.
```

Die Identität einer Capability wird nicht mit der aktuellen Existenz eines bestimmten Providers gleichgesetzt.

---

## Dynamische Provider

Eine Capability kann zu unterschiedlichen Zeitpunkten unterschiedliche Provider besitzen.

```text
Capability X
   │
   ├── Provider A
   ├── Provider B
   └── Provider C
```

Wird Provider B entfernt:

```text
Capability X
   │
   ├── Provider A
   └── Provider C
```

bleibt die Capability Identity unverändert.

---

## Capability Lifecycle

Provider durchlaufen einen expliziten Lifecycle.

Konzeptionell:

```text
Discovered
    ↓
Registered
    ↓
Available
    ↓
Unavailable / Replaced
    ↓
Removed
```

Die exakten Zustände und Übergänge werden in NPSPECs definiert.

---

## Dynamic Registration

Provider können Capabilities zur Laufzeit registrieren.

Typische Auslöser sind:

```text
Application Start
Service Start
Driver Load
Device Hotplug
Plugin Activation
Remote Service Discovery
```

Registrierungen müssen validiert werden, bevor sie für Resolution und Ausführung verwendet werden dürfen.

---

## Dynamic Removal

Das Entfernen eines Providers muss kontrolliert erfolgen.

```text
Provider Removal
      ↓
Registry Update
      ↓
New Resolution prohibited
      ↓
Existing Operations handled
      ↓
Provider removed
```

Neue Operationen dürfen nicht mehr an einen Provider gebunden werden, der sich im Removal-Prozess befindet.

Das Verhalten bereits laufender Operationen wird durch deren Contracts und Lifecycle-Regeln bestimmt.

---

## Dynamic Replacement

Dynamic Capabilities werden mit Hot Replacement und Live Evolution integriert.

```text
Provider V1
    ↓
Replacement
    ↓
Provider V2
```

Wenn Contracts kompatibel sind, kann ein neuer Provider bestehende Capability Identity weiter bedienen.

Eine neue Implementierung erzeugt nicht automatisch eine neue Capability Identity.

---

## Temporäre Capabilities

Capabilities oder Provider können bewusst nur für einen begrenzten Kontext existieren.

Beispiele:

```text
Session
Execution Domain
Application Instance
Task Group
Connected Device
Remote Connection
```

Ihre Sichtbarkeit und Lebensdauer werden an den jeweiligen Lifecycle gebunden.

---

## Dynamische Capability Composition

Auch eine zur Laufzeit erzeugte Composition kann als Capability bereitgestellt werden.

```text
Capability A
     +
Capability B
     +
Capability C
     ↓
Dynamic Composite Capability
```

Ihre Gültigkeit hängt von den zugrunde liegenden Capabilities, Contracts und Lifecycle-Bedingungen ab.

---

## Anwendungen

Capability-Based Applications dürfen nicht davon ausgehen, dass optionale Capabilities dauerhaft verfügbar bleiben.

```text
Capability appears
      ↓
Application Function enabled

Capability disappears
      ↓
Application adapts
```

Required Capabilities müssen entsprechend ihres Application Contracts behandelt werden.

Optionale Capabilities können dynamische Funktionen der Anwendung aktivieren oder deaktivieren.

---

## Registry und Discovery

Capability Registry und Discovery müssen Änderungen beobachtbar machen.

```text
Capability Added
Capability Changed
Provider Added
Provider Unavailable
Provider Replaced
Provider Removed
```

Consumer sollen Änderungen abonnieren können, ohne die Registry permanent aktiv abfragen zu müssen.

Die konkrete Event- und Subscription-Semantik wird in NPSPECs definiert.

---

## Resolution

Eine frühere Resolution ist keine Garantie für zukünftige Verfügbarkeit.

```text
Resolved Provider
      ↓
State changes
      ↓
Revalidation / Resolution
```

Bei relevanten Änderungen kann NovaOS eine erneute Resolution oder Negotiation durchführen.

Hard Constraints dürfen dabei nicht abgeschwächt werden.

---

## Security

Dynamische Registrierung erzeugt keine automatische Vertrauensstellung oder Authority.

```text
Registered
    ≠
Trusted
    ≠
Authorized
```

Provider müssen weiterhin Code Integrity, Trust, Sandbox-, Capability- und sonstige Security-Anforderungen erfüllen.

Nicht autorisierte Komponenten dürfen keine privilegierten Capability Provider einschleusen.

---

## Resource Economy

Dynamische Capabilities unterliegen der systemweiten Resource Economy.

Registrierung und Aktivierung eines Providers können Ressourcen benötigen wie:

```text
Memory
CPU
Device Resources
GPU / NPU
Network
Storage
Queues
```

Das dynamische Erzeugen von Capabilities darf nicht zu unkontrolliertem Ressourcenverbrauch führen.

---

## Introspection

NovaOS muss den dynamischen Capability-Zustand kontrolliert introspektierbar machen.

Beispielsweise:

```text
Capability Identity
Registered Providers
Provider Lifecycle State
Availability
Registration Source
Contract Version
Dependencies
Replacement State
```

Damit kann nachvollzogen werden, warum eine Capability aktuell verfügbar oder nicht verfügbar ist.

---

## Normative Anforderungen

1. NovaOS MUSS dynamische Capability Provider unterstützen.
2. Provider MÜSSEN zur Laufzeit registriert, aktualisiert, ersetzt und entfernt werden können.
3. Capability Identity MUSS von aktueller Provider-Verfügbarkeit getrennt bleiben.
4. Provider MÜSSEN einen expliziten Lifecycle besitzen.
5. Dynamische Registrierungen MÜSSEN vor ihrer Nutzung validiert werden.
6. Provider im Removal-Zustand DÜRFEN keine neuen Operationen erhalten.
7. Laufende Operationen MÜSSEN nach definierten Lifecycle- und Contract-Regeln behandelt werden.
8. Hot Replacement MUSS mit Dynamic Capabilities integrierbar sein.
9. Temporäre Capabilities MÜSSEN an definierte Lifecycles gebunden werden können.
10. Dynamische Capability Compositions MÜSSEN unterstützt werden können.
11. Registry und Discovery MÜSSEN relevante Capability-Änderungen beobachtbar machen.
12. Consumer MÜSSEN auf Capability-Änderungen reagieren können, ohne ausschließlich Polling verwenden zu müssen.
13. Provider-Änderungen MÜSSEN eine erneute Resolution oder Negotiation auslösen können.
14. Dynamische Registrierung DARF Security-, Trust- oder Authority-Prüfungen NICHT umgehen.
15. Dynamische Capabilities MÜSSEN in Resource Economy und Architecture Introspection integriert sein.

---

## Konsequenzen

### Positive Konsequenzen

- Hardware und Dienste können zur Laufzeit integriert werden,
- Anwendungen können sich dynamisch erweitern,
- Hot Replacement wird mit dem Capability-Modell vereinheitlicht,
- Remote- und temporäre Fähigkeiten lassen sich natürlich darstellen,
- Systemneustarts werden für viele Capability-Änderungen unnötig.

### Negative Konsequenzen

- Consumer müssen mit wechselnder Verfügbarkeit umgehen,
- Registry und Resolution benötigen konsistente Lifecycle-Semantik,
- laufende Operationen benötigen definierte Regeln für Provider Removal und Replacement.

---

## Verworfene Alternativen

### Capability-Bestand nur beim Systemstart bestimmen

Verworfen.

Dies würde Hotplug, dynamische Dienste und Live Replacement stark einschränken.

### Provider-Änderung erzeugt immer neue Capability Identity

Verworfen.

Capability Identity beschreibt die semantische Fähigkeit und nicht ihre konkrete Implementierung.

### Anwendungen pollen permanent die Capability Registry

Verworfen.

Capability-Änderungen sollen über ein beobachtbares Lifecycle-Modell verfügbar sein.

### Dynamische Provider automatisch vertrauen

Verworfen.

Dynamische Verfügbarkeit darf bestehende Security- und Trust-Grenzen nicht umgehen.

---

## Abhängigkeiten

- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-CAPABILITY-0003_Capability_Registry`
- `ADR-CAPABILITY-0004_Capability_Discovery`
- `ADR-CAPABILITY-0005_Capability_Composition`
- `ADR-CAPABILITY-0006_Capability_Negotiation`
- `ADR-CAPABILITY-0011_Capability_Based_Applications`
- `ADR-CAPABILITY-0012_Capability_Based_Drivers`
- `ADR-ARCH-0012_Hot_Replacement_Live_Evolution`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-HAL-0007_Hotplug_über_deklarative_Device_Lifecycle_States`

---

## Zugehörige NPSPECs

- `NPSPEC-CAPABILITY-DYNAMIC-0001`
- `NPSPEC-CAPABILITY-LIFECYCLE-0001`
- `NPSPEC-CAPABILITY-DYNAMIC-REGISTRATION-0001`
- `NPSPEC-CAPABILITY-DYNAMIC-REMOVAL-0001`
- `NPSPEC-CAPABILITY-DYNAMIC-REPLACEMENT-0001`
- `NPSPEC-CAPABILITY-CHANGE-NOTIFICATION-0001`
- `NPSPEC-CAPABILITY-DYNAMIC-TEST-0001`

---

## Ergebnis

NovaOS behandelt die Verfügbarkeit von Fähigkeiten als dynamischen Systemzustand:

```text
Capability Identity
        ↓
Dynamic Providers
        ↓
Registry Lifecycle
        ↓
Discovery / Negotiation / Resolution
        ↓
Current Executable Capability
```

Damit können Geräte, Dienste, Anwendungen, Plugins und entfernte Provider ihre Fähigkeiten zur Laufzeit in NovaOS einbringen oder wieder entfernen, während Capability Identity, Security und Contracts stabil und kontrollierbar bleiben.