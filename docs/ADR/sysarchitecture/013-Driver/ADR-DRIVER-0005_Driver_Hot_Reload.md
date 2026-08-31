# ADR-DRIVER-0005 – Driver Hot Reload

## Status

Angenommen

## Kategorie

Driver / Hot Replacement / Lifecycle / Reliability / Live Evolution

## Kontext

Treiber gehören zu den Komponenten, die während der Lebensdauer eines Systems aktualisiert, ersetzt oder nach Fehlern neu gestartet werden müssen.

Ein klassisches Treibermodell erfordert dafür häufig einen Neustart des Betriebssystems oder zumindest die vollständige Deaktivierung des betroffenen Geräts.

NovaOS besitzt mit Hot Replacement / Live Evolution, Driver Isolation Domains und systemweiter Supervision bereits die architektonischen Grundlagen, um geeignete Treiber kontrolliert zur Laufzeit zu ersetzen.

Dabei darf Hot Reload jedoch weder laufende I/O-Operationen beschädigen noch Hardwarezustände, Ressourcen oder Security-Grenzen unkontrolliert übertragen.

---

## Entscheidung

NovaOS unterstützt **Driver Hot Reload als kontrollierten Driver-Lifecycle-Vorgang**.

Geeignete Treiber können ohne vollständigen Systemneustart durch eine neue Instanz ersetzt werden.

```text
Old Driver
    ↓
Quiesce
    ↓
State Handover
    ↓
New Driver
    ↓
Resume
```

Hot Reload ist nur zulässig, wenn Driver, Device und beteiligte Systemkomponenten einen sicheren Übergang unterstützen.

---

## Grundprinzip

```text
Replace the driver,
not the running system.
```

---

## Quiescing

Vor dem eigentlichen Austausch muss der alte Treiber in einen kontrollierten Zustand überführt werden.

Neue I/O Requests dürfen während des Übergangs zurückgehalten oder an eine geeignete neue Instanz weitergeleitet werden.

Bereits laufende Operationen müssen entsprechend ihrem Contract:

- abgeschlossen,
- abgebrochen oder
- kontrolliert übernommen

werden.

Die konkrete Übergangslogik wird in den NPSPECs definiert.

---

## Driver State

Treiberzustand darf nur übertragen werden, wenn er explizit als übertragbarer Driver State definiert ist.

```text
Runtime State
    ↓
Defined Transferable State
    ↓
Compatibility Validation
    ↓
New Driver
```

Interne Speicheradressen, temporäre Kernelzustände oder nicht definierte Implementierungsdetails dürfen nicht als stabile Hot-Reload-Schnittstelle betrachtet werden.

---

## Device State

Der physische Gerätezustand muss beim Driver Hot Reload berücksichtigt werden.

Je nach Device Contract kann ein Gerät:

- seinen Zustand beibehalten,
- kontrolliert neu initialisiert,
- teilweise zurückgesetzt oder
- vollständig zurückgesetzt

werden.

NovaOS darf nicht voraussetzen, dass jedes Gerät einen zustandserhaltenden Treiberwechsel unterstützt.

---

## Compatibility

Die neue Treiberinstanz muss mit dem vorhandenen Gerät, Driver Contract und gegebenenfalls dem übertragbaren Zustand kompatibel sein.

```text
Device Contract
      +
Driver Contract
      +
State Compatibility
      ↓
Hot Reload permitted
```

Bei nicht erfüllter Kompatibilität muss der Hot Reload abgelehnt oder ein kontrollierter Neustart des Device Stacks verwendet werden.

---

## Capabilities und Ressourcen

Driver Capabilities werden nicht unkontrolliert von der alten Instanz übernommen.

Die neue Instanz muss für die benötigten Capabilities und Ressourcen autorisiert werden.

Dazu gehören insbesondere:

- Device Assignment,
- MMIO,
- Interrupts,
- DMA/IOMMU,
- I/O-Ressourcen,
- Resource Budgets.

Der Übergang muss verhindern, dass alte und neue Treiberinstanz gleichzeitig unautorisiert dieselben exklusiven Ressourcen kontrollieren.

---

## Fehler und Rollback

Schlägt der Hot Reload fehl, muss NovaOS einen definierten Recovery-Pfad besitzen.

Abhängig vom Zustand können insbesondere folgende Ergebnisse zulässig sein:

```text
New Driver Active
Old Driver Restored
Device Reinitialized
Driver Disabled
```

Ein fehlgeschlagener Hot Reload darf nicht zu einem undefinierten Treiber- oder Gerätezustand führen.

---

## User- und Kernel-Mode-Treiber

User-Mode-Treiber sind aufgrund ihrer Isolation Domains besonders für Hot Reload geeignet.

Kernel-Mode-Treiber dürfen ebenfalls Hot Replacement unterstützen, unterliegen jedoch strengeren Anforderungen, da fehlerhafte Übergänge unmittelbar den Kernelzustand beeinflussen können.

Hot Reload ist daher eine deklarierte Driver Capability und keine automatische Eigenschaft jedes Treibers.

---

## Normative Anforderungen

1. NovaOS MUSS Driver Hot Reload für geeignete Treiber unterstützen können.
2. Hot Reload DARF nur erfolgen, wenn Driver und Device einen sicheren Übergang unterstützen.
3. Der alte Treiber MUSS vor dem Austausch kontrolliert quiesziert werden.
4. Laufende I/O Requests MÜSSEN während des Übergangs definiert behandelt werden.
5. Übertragbarer Driver State MUSS explizit definiert und versionierbar sein.
6. Implementierungsinterne Speicheradressen DÜRFEN NICHT als stabile State-Transfer-Schnittstelle verwendet werden.
7. Die neue Treiberinstanz MUSS vor Aktivierung auf Kompatibilität geprüft werden.
8. Device State MUSS während des Hot Reload kontrolliert behandelt werden.
9. Capabilities und Hardware-Ressourcen MÜSSEN kontrolliert auf die neue Instanz übertragen oder neu vergeben werden.
10. Hot Reload DARF Security-, Trust- oder Resource-Grenzen NICHT umgehen.
11. Ein fehlgeschlagener Hot Reload MUSS in einen definierten Recovery-Zustand führen.
12. User-Mode Driver Isolation Domains SOLLEN unabhängig hot-reloadbar sein.
13. Kernel-Mode Driver Hot Reload MUSS strengeren Sicherheits- und Konsistenzanforderungen unterliegen.
14. Hot-Reload-Fähigkeit MUSS über Driver Contracts deklarierbar sein.
15. Hot-Reload-Vorgänge MÜSSEN auditier- und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Treiberupdates ohne vollständigen Systemneustart,
- schnellere Recovery nach Treiberfehlern,
- geringere Downtime,
- Grundlage für Live Evolution der Treiberarchitektur,
- besonders effektive Nutzung isolierter User-Mode-Treiber.

### Negative Konsequenzen

- Driver State benötigt definierte Übergabesemantik,
- nicht jedes Gerät unterstützt zustandserhaltenden Austausch,
- Kernel-Mode Hot Reload bleibt sicherheitskritisch,
- I/O und Hardware-Ressourcen müssen während des Übergangs koordiniert werden.

---

## Verworfene Alternativen

### Treiberupdates immer nur nach Systemneustart

Verworfen.

Dies widerspricht dem NovaOS-Prinzip Hot Replacement / Live Evolution und erzeugt unnötige Downtime.

### Beliebigen Treiberzustand vollständig kopieren

Verworfen.

Implementierungsinterner Zustand ist keine stabile Schnittstelle zwischen Treiberversionen.

### Hot Reload für jeden Treiber erzwingen

Verworfen.

Bestimmte Geräte und Treiber können keinen sicheren Laufzeitaustausch garantieren.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0012_Hot_Replacement_Live_Evolution`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-DRIVER-0001_Hybrides_Kernel_und_User_Mode_Drivermodell`
- `ADR-DRIVER-0002_Driver_Sandboxing`
- `ADR-DRIVER-0003_Driver_Isolation_Domains`
- `ADR-DRIVER-0004_Driver_Capability_Model`
- `ADR-HAL-0007_Hotplug_über_deklarative_Device_Lifecycle_States`
- `ADR-IO-0001_Completion_Based_Asynchronous_IO`

---

## Zugehörige NPSPECs

- `NPSPEC-DRIVER-HOTRELOAD-0001`
- `NPSPEC-DRIVER-HOTRELOAD-STATE-0001`
- `NPSPEC-DRIVER-HOTRELOAD-QUIESCE-0001`
- `NPSPEC-DRIVER-HOTRELOAD-COMPAT-0001`
- `NPSPEC-DRIVER-HOTRELOAD-RECOVERY-0001`
- `NPSPEC-DRIVER-HOTRELOAD-TEST-0001`

---

## Ergebnis

NovaOS kann geeignete Treiber kontrolliert austauschen, ohne das gesamte System neu zu starten:

```text
Running Driver
      ↓
   Quiesce
      ↓
Validate + Transfer
      ↓
Replacement Driver
      ↓
    Resume
```

Driver Hot Reload wird damit zu einer kontrollierten Anwendung der systemweiten Hot-Replacement-Architektur, ohne Sicherheit, I/O-Konsistenz oder Hardwarezustand dem Laufzeitaustausch unterzuordnen.