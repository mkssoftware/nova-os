# ADR-UPDATE-0013 – Live Service, Driver und Component Updates

## Status

Angenommen

## Kategorie

Update / Live Update / Availability

## Kontext

Nicht jede Aktualisierung von NovaOS soll einen Neustart des gesamten Systems erfordern.

Insbesondere Services, Treiber und austauschbare Systemkomponenten sollen aktualisiert werden können, während das System weiterläuft.

Dabei dürfen laufende Operationen, Zustände und Abhängigkeiten nicht unkontrolliert unterbrochen werden.

## Entscheidung

NovaOS unterstützt **Live Updates** für Komponenten, deren Lifecycle- und State-Contracts einen sicheren Austausch erlauben.

```text
Component V1
     ↓
Prepare V2
     ↓
Verify V2
     ↓
State Transfer
     ↓
Atomic Switch
     ↓
Component V2
```

Ein Live Update ist nur zulässig, wenn der neue Provider die erforderlichen Contracts erfüllt.

## Updatefähigkeit

Komponenten deklarieren ihre Live-Update-Fähigkeit.

```text
LiveUpdate:
    Supported
    RestartRequired
    RebootRequired
```

NovaOS entscheidet dadurch nicht allein anhand des Komponententyps über die Update-Methode.

## Services

Ein Service kann parallel zur alten Version vorbereitet werden.

```text
Service V1 → Active
Service V2 → Prepared
```

Nach erfolgreicher Verifikation werden neue Requests auf V2 umgeleitet.

Laufende Operationen von V1 können kontrolliert beendet werden.

## Treiber

Treiber dürfen nur live aktualisiert werden, wenn Hardwarezustand, Ressourcen und laufende I/O-Operationen sicher übergeben werden können.

```text
Driver V1
    ↓
Quiesce
    ↓
Transfer / Reinitialize
    ↓
Driver V2
```

Ist dies nicht sicher möglich, wird ein Neustart oder Reboot verlangt.

## Komponenten

Capability Provider und andere austauschbare Komponenten verwenden denselben grundlegenden Mechanismus:

```text
Old Provider
     ↓
Prepare New Provider
     ↓
Contract Validation
     ↓
Atomic Provider Switch
```

## State Transfer

Zustandsbehaftete Komponenten müssen ihre Übergabesemantik explizit definieren.

Mögliche Strategien:

```text
Stateless Switch
State Transfer
Shared Versioned State
Drain and Restart
```

Ein implizites Kopieren unbekannten internen Zustands ist nicht zulässig.

## Quiescing

Vor einem Austausch kann eine Komponente in einen ruhenden Zustand versetzt werden.

```text
Running
   ↓
Quiescing
   ↓
Quiescent
   ↓
Switch
```

Neue Operationen werden dabei gestoppt oder bereits an die neue Version geleitet.

## Rollback

Die alte Version bleibt bis zur erfolgreichen Verifikation verfügbar, sofern dies technisch möglich ist.

```text
V1
 ↓
Switch V2
 ↓
Verification Failure
 ↓
Rollback V1
```

Nach einer irreversiblen Zustandsänderung gelten die definierten Compensation- oder Recovery-Regeln.

## Kernel-nahe Komponenten

Nicht sicher austauschbare Kernel- oder Hardwarekomponenten werden nicht zwangsweise live aktualisiert.

```text
Safe Live Update?
   ├── Yes → Live Switch
   └── No  → Restart / Reboot
```

Verfügbarkeit darf nicht über Systemintegrität gestellt werden.

## Normative Anforderungen

1. NovaOS MUSS Live Updates für geeignete Services, Treiber und Komponenten unterstützen können.
2. Komponenten MÜSSEN ihre Live-Update-Fähigkeit deklarieren.
3. Neue Versionen MÜSSEN vor dem Switch vorbereitet und verifiziert werden.
4. Der Versionswechsel MUSS logisch atomar erfolgen.
5. Zustandsbehaftete Komponenten MÜSSEN ihre State-Transfer-Semantik deklarieren.
6. Laufende Operationen MÜSSEN kontrolliert abgeschlossen, übertragen oder abgebrochen werden.
7. Treiber DÜRFEN nur live ersetzt werden, wenn Hardware- und I/O-Zustand sicher behandelt werden können.
8. Unsichere Live Updates MÜSSEN auf Restart oder Reboot zurückfallen.
9. Die alte Version SOLL bis zur erfolgreichen Verification rollbackfähig bleiben.
10. Live Updates MÜSSEN die bestehenden Capability-, Security-, Trust- und Execution-Contracts erhalten.

## Konsequenzen

### Positive Konsequenzen

- weniger notwendige Systemneustarts,
- höhere Verfügbarkeit,
- Services und Provider können transparent ausgetauscht werden,
- Updates können mit laufenden Workloads koordiniert werden.

### Negative Konsequenzen

- zustandsbehaftete Komponenten benötigen explizite Übergabeprotokolle,
- Treiber-Live-Updates sind nicht für jede Hardware möglich,
- alte und neue Version können kurzfristig parallel Ressourcen benötigen.

## Abhängigkeiten

- `ADR-UPDATE-0002_Transactional_Update_Pipeline`
- `ADR-UPDATE-0003_Atomic_Updates`
- `ADR-UPDATE-0008_Side_By_Side_Versionen`
- `ADR-UPDATE-0009_Capability_Based_Dependency_Resolution`
- `ADR-TRANSACTION-0007_Rollback_und_Compensation`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-ARCH-0012_Hot_Replacement`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-LIVE-UPDATE-0001`
- `NPSPEC-UPDATE-LIVE-SERVICE-0001`
- `NPSPEC-UPDATE-LIVE-DRIVER-0001`
- `NPSPEC-UPDATE-STATE-TRANSFER-0001`

## Ergebnis

NovaOS aktualisiert geeignete laufende Komponenten durch kontrollierten Austausch:

```text
Old Version
     ↓
Prepare New Version
     ↓
Verify
     ↓
Quiesce / State Transfer
     ↓
Atomic Switch
     ↓
Verify
```

Kann ein sicherer Live-Wechsel nicht garantiert werden, verwendet NovaOS stattdessen einen kontrollierten Restart oder Reboot.