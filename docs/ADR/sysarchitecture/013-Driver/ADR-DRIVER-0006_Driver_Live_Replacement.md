# ADR-DRIVER-0006 – Driver Live Replacement

## Status

Angenommen

## Kategorie

Driver / Live Evolution / Replacement / Availability / Reliability

## Kontext

`ADR-DRIVER-0005` definiert Driver Hot Reload als kontrollierten Austausch eines Treibers zur Laufzeit.

Für NovaOS reicht jedoch ein einfaches Stoppen und erneutes Starten nicht in allen Fällen aus. Kritische Geräte und kontinuierliche I/O-Pipelines sollen möglichst weiterarbeiten können, während eine neue Treiberimplementierung vorbereitet und aktiviert wird.

NovaOS benötigt deshalb ein Live-Replacement-Modell, bei dem alte und neue Treiberinstanz während einer begrenzten Übergangsphase koordiniert existieren können.

---

## Entscheidung

NovaOS unterstützt **Driver Live Replacement** als erweiterte Form des Hot Replacement.

Die neue Treiberinstanz wird vorbereitet und validiert, bevor die aktive Kontrolle vom bisherigen Treiber übernommen wird.

```text
Old Driver
    ↓
Prepare New Driver
    ↓
Validate
    ↓
Controlled Handover
    ↓
New Driver Active
    ↓
Retire Old Driver
```

Der eigentliche Wechsel soll möglichst kurz und atomar aus Sicht abhängiger Systemkomponenten erfolgen.

---

## Grundprinzip

```text
Prepare before replacing.

Switch only when the replacement is ready.
```

---

## Abgrenzung zu Hot Reload

Hot Reload und Live Replacement sind verwandt, aber nicht identisch.

```text
Hot Reload
Old → Stop/Quiesce → Replace → Resume

Live Replacement
Old Active → New Prepared → Handover → New Active
```

Live Replacement reduziert die Zeit, in der kein funktionsfähiger Treiber verfügbar ist.

`ADR-DRIVER-0005` definiert die allgemeinen Regeln für State Transfer, Kompatibilität und Recovery, die auch für Live Replacement gelten.

---

## Parallel vorbereitete Instanzen

Während der Vorbereitung dürfen alte und neue Treiberinstanz gleichzeitig existieren.

Dies bedeutet jedoch nicht, dass beide uneingeschränkt dasselbe Gerät kontrollieren dürfen.

```text
Old Driver → Active Authority

New Driver → Preparation Authority
```

Exklusive Device Authority wird erst beim kontrollierten Handover übertragen.

---

## Kontrolliertes Handover

Der Wechsel muss eine klar definierte Übergabegrenze besitzen.

Dabei können insbesondere übertragen werden:

- Device Ownership,
- Driver State,
- I/O Queues,
- Shared Buffers,
- Interrupt-Zuweisungen,
- DMA-/IOMMU-Kontexte,
- Resource Reservations.

Welche Zustände tatsächlich übertragbar sind, bestimmt der jeweilige Driver- und Device-Contract.

---

## Laufende I/O-Operationen

Live Replacement soll laufende I/O-Workloads möglichst wenig unterbrechen.

Requests können abhängig vom Contract:

```text
Old Driver → complete existing requests

New Driver → accept new requests
```

oder über eine definierte Übergabesemantik übernommen werden.

Ein Request darf während des Übergangs weder verloren gehen noch unkontrolliert doppelt ausgeführt werden.

---

## Capability Transfer

Die neue Treiberinstanz erhält benötigte Capabilities kontrolliert.

Die Umschaltung exklusiver Authority erfolgt gemeinsam mit dem Driver Handover.

```text
Old Driver Authority
        ↓
Atomic Handover
        ↓
New Driver Authority
```

Es darf kein undefiniertes Zeitfenster entstehen, in dem beide Instanzen unkontrolliert dieselbe exklusive Hardware-Authority besitzen.

---

## Kompatibilität

Live Replacement setzt kompatible Driver Contracts voraus.

Die neue Version muss mindestens mit den für den laufenden Betrieb benötigten:

- Device Contracts,
- Driver Interfaces,
- State-Formaten,
- Security-Anforderungen

kompatibel sein.

Nicht kompatible Änderungen erfordern einen stärkeren Lifecycle-Übergang oder eine Device-Neuinitialisierung.

---

## Failure und Rollback

Die alte Treiberinstanz soll bis zum erfolgreichen Commit des Replacement-Vorgangs als möglicher Recovery-Pfad erhalten bleiben, sofern dies technisch möglich ist.

```text
Prepare New
    ↓
Validation
    ├── Failed → Keep Old
    └── Passed
          ↓
       Handover
          ↓
       Commit
```

Erst nach erfolgreicher Aktivierung und Verifikation darf die alte Instanz endgültig entfernt werden.

---

## Kernel- und User-Mode-Treiber

User-Mode Driver Isolation Domains sind das bevorzugte Einsatzgebiet für Live Replacement.

Kernel-Mode-Treiber dürfen Live Replacement unterstützen, wenn ihre Contracts und Kernel-Schnittstellen einen sicheren Austausch ermöglichen.

Die stärkeren Privilegien von Kernel-Mode-Treibern erfordern entsprechend strengere Validierungs- und Übergaberegeln.

---

## Normative Anforderungen

1. NovaOS MUSS Driver Live Replacement für geeignete Treiber unterstützen können.
2. Eine neue Treiberinstanz SOLL vor der Umschaltung vollständig vorbereitet und validiert werden.
3. Alte und neue Treiberinstanz DÜRFEN während einer kontrollierten Übergangsphase gleichzeitig existieren.
4. Gleichzeitige Existenz DARF NICHT automatisch gleichzeitige exklusive Device Authority bedeuten.
5. Der Wechsel der aktiven Treiberinstanz MUSS über eine definierte Handover-Grenze erfolgen.
6. Laufende I/O Requests DÜRFEN durch Live Replacement weder verloren gehen noch unkontrolliert doppelt ausgeführt werden.
7. Übertragbarer Zustand MUSS durch Driver- und Device-Contracts definiert sein.
8. Capabilities und exklusive Hardware-Ressourcen MÜSSEN kontrolliert übertragen werden.
9. Die neue Treiberinstanz MUSS vor Aktivierung kompatibel und ausreichend vertrauenswürdig sein.
10. Security-, Trust-, Data-Sovereignty- und Resource-Constraints DÜRFEN durch Live Replacement NICHT umgangen werden.
11. Die alte Instanz SOLL bis zum erfolgreichen Replacement Commit als Recovery-Möglichkeit erhalten bleiben, sofern technisch möglich.
12. Ein fehlgeschlagener Replacement-Vorgang MUSS einen definierten Recovery-Zustand besitzen.
13. User-Mode-Treiber SOLLEN bevorzugt für Live Replacement ausgelegt werden.
14. Live-Replacement-Vorgänge MÜSSEN auditier- und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- sehr geringe Treiber-Downtime,
- Updates während des laufenden Betriebs,
- sicherere Aktivierung neuer Treiberversionen,
- Rollback vor endgültigem Commit,
- Grundlage für langfristige Live Evolution von NovaOS.

### Negative Konsequenzen

- zeitweise existieren zwei Treiberinstanzen,
- State- und Authority-Handover müssen koordiniert werden,
- nicht jede Hardware unterstützt unterbrechungsarmen Austausch,
- höhere Anforderungen an Driver Contracts.

---

## Verworfene Alternativen

### Alten Treiber zuerst vollständig entfernen

Verworfen.

Dadurch entsteht unnötige Downtime und der direkte Rollback-Pfad geht verloren.

### Alte und neue Instanz gleichzeitig vollständig autorisieren

Verworfen.

Konkurrierende Kontrolle derselben Hardware könnte undefinierte Gerätezustände erzeugen.

### Live Replacement für jeden Treiber verpflichtend machen

Verworfen.

Nicht jedes Gerät oder Driver Interface unterstützt einen sicheren Live-Übergang.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0012_Hot_Replacement_Live_Evolution`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-DRIVER-0001_Hybrides_Kernel_und_User_Mode_Drivermodell`
- `ADR-DRIVER-0003_Driver_Isolation_Domains`
- `ADR-DRIVER-0004_Driver_Capability_Model`
- `ADR-DRIVER-0005_Driver_Hot_Reload`
- `ADR-HAL-0007_Hotplug_über_deklarative_Device_Lifecycle_States`
- `ADR-IO-0001_Completion_Based_Asynchronous_IO`

---

## Zugehörige NPSPECs

- `NPSPEC-DRIVER-LIVE-REPLACEMENT-0001`
- `NPSPEC-DRIVER-LIVE-HANDOVER-0001`
- `NPSPEC-DRIVER-LIVE-STATE-0001`
- `NPSPEC-DRIVER-LIVE-AUTHORITY-0001`
- `NPSPEC-DRIVER-LIVE-ROLLBACK-0001`
- `NPSPEC-DRIVER-LIVE-TEST-0001`

---

## Ergebnis

NovaOS kann eine neue Treiberinstanz vorbereiten, während die bisherige Instanz das Gerät weiterhin kontrolliert:

```text
Old Driver Active
        ↓
New Driver Prepared
        ↓
Validated Handover
        ↓
New Driver Active
        ↓
Old Driver Retired
```

Driver Live Replacement erweitert damit Hot Reload um einen vorbereiteten, kontrollierten und möglichst unterbrechungsarmen Übergang zwischen Treiberinstanzen.