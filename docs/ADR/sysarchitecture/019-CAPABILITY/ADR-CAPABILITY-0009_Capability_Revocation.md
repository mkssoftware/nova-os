# ADR-CAPABILITY-0009 – Capability Revocation

## Status

Angenommen

## Kategorie

Capability Architecture / Authority / Revocation / Lifecycle

## Kontext

Capabilities können zeitlich begrenzte oder dauerhaft widerrufbare Authority repräsentieren.

Eine einmal ausgegebene Capability darf nicht zwangsläufig bis zum Ende ihrer ursprünglich vorgesehenen Lebensdauer gültig bleiben.

Widerruf kann erforderlich werden durch:

- Benutzerentscheidung,
- Session-Ende,
- Prozess- oder Execution-Domain-Ende,
- Sicherheitsvorfälle,
- Policy-Änderungen,
- Geräteentfernung,
- Service-Abschaltung,
- Capability-Provider-Wechsel,
- Entzug administrativer Authority.

NovaOS benötigt deshalb einen systemweiten Revocation-Mechanismus.

---

## Entscheidung

NovaOS unterstützt **Capability Revocation als fundamentalen Bestandteil des Capability-Lifecycles**.

```text
Capability
    ↓
Valid
    ↓
Revocation
    ↓
Invalid
```

Eine Capability kann widerrufen werden, ohne sämtliche existierenden Repräsentationen dieser Capability physisch zurückholen zu müssen.

---

## Grundprinzip

```text
Authority can be granted,
and authority can be withdrawn.
```

Revocation betrifft die weitere Verwendbarkeit der Authority.

Bereits vollständig abgeschlossene und irreversible Effekte werden dadurch nicht rückgängig gemacht.

---

## Revocation State

Capabilities müssen einen eindeutig bestimmbaren Gültigkeitszustand besitzen können.

Konzeptionell:

```text
Valid
Revoked
Expired
Invalid
Unknown
```

Für sicherheitskritische Operationen gilt:

```text
Unknown ≠ Valid
```

Kann der Revocation-Zustand nicht zuverlässig bestimmt werden, muss die Authority erneut validiert oder die Operation abgelehnt werden.

---

## Revocation Domains

Mehrere Capabilities können einer gemeinsamen Revocation Domain zugeordnet werden.

Beispiele:

```text
Session
Application Instance
Execution Domain
Task Group
Transaction
Service Instance
Device Context
```

```text
Revocation Domain
      │
      ├── Capability A
      ├── Capability B
      └── Capability C
```

Das Invalidieren der Domain kann alle davon abhängigen Capabilities ungültig machen.

---

## Delegierte und attenuierte Capabilities

Abgeleitete Capabilities bleiben von ihrer Authority-Quelle abhängig.

```text
Source Capability
       ↓
Attenuation
       ↓
Delegation
       ↓
Derived Capability
```

Wird die zugrunde liegende Authority widerrufen, darf eine daraus abgeleitete Capability den Widerruf nicht umgehen.

---

## Leases und Epochs

Revocation kann mit Leases und Epochs kombiniert werden.

```text
Capability
    +
Lease
    +
Epoch
    +
Revocation State
    ↓
Effective Validity
```

Epoch-Wechsel ermöglichen insbesondere das effiziente Invalidieren ganzer Capability-Generationen oder Revocation Domains.

Nicht jede Capability muss denselben Revocation-Mechanismus verwenden.

---

## Laufende Operationen

Revocation einer Capability und Abbruch einer bereits gestarteten Operation sind unterschiedliche Vorgänge.

```text
Capability Revoked
        ↓
Future Use → Denied
Running Operation
        ↓
Contract-defined Handling
```

Je nach Operation kann eine laufende Ausführung:

- abgebrochen,
- kontrolliert beendet,
- zurückgerollt,
- bis zu einem sicheren Commit-Punkt fortgeführt

werden.

Die konkrete Semantik wird durch Capability- und Execution Contract bestimmt.

---

## Capability Composition

Bei zusammengesetzten Capabilities muss Revocation entlang abhängiger Authority berücksichtigt werden.

```text
Composite Capability
      ↓
A → B → C
```

Wird eine erforderliche Authority während der Ausführung ungültig, darf die Composition nicht mit nicht mehr autorisierten Teiloperationen fortfahren.

Structured Concurrency und Cancellation können zur kontrollierten Beendigung verwendet werden.

---

## Lokale und entfernte Capabilities

Revocation verwendet für lokale und entfernte Capabilities dasselbe grundlegende Authority-Modell.

Remote Revocation kann zusätzliche Mechanismen für:

- verteilte Validierung,
- Caching,
- Epoch-Synchronisation,
- Erreichbarkeitsfehler

benötigen.

Ein Netzwerkfehler darf jedoch nicht automatisch als gültige Authority interpretiert werden.

---

## Provider und Capability Lifecycle

Das Entfernen eines Capability Providers ist nicht automatisch identisch mit Authority Revocation.

```text
Provider unavailable
        ≠
Authority revoked
```

Die Capability kann weiterhin gültige Authority darstellen, obwohl aktuell kein ausführbarer Provider verfügbar ist.

Umgekehrt kann Authority widerrufen sein, obwohl ein Provider weiterhin vorhanden ist.

---

## Introspection und Audit

NovaOS muss Revocation kontrolliert nachvollziehbar machen können.

```text
Capability Identity
Authority Source
Revocation Domain
Revocation State
Epoch
Lease
Delegation Dependencies
Revocation Reason
```

Geheimes Capability-Material darf dabei nicht offengelegt werden.

---

## Normative Anforderungen

1. NovaOS MUSS Capability Revocation systemweit unterstützen.
2. Authority MUSS widerrufbar sein können, ohne sämtliche Capability-Repräsentationen physisch zurückholen zu müssen.
3. Revocation Domains MÜSSEN unterstützt werden können.
4. Delegierte und attenuierte Capabilities DÜRFEN Revocation ihrer Authority-Quelle NICHT umgehen.
5. Revocation MUSS mit Leases und Epochs kombinierbar sein.
6. Nicht jede Capability MUSS denselben technischen Revocation-Mechanismus verwenden.
7. Für sicherheitskritische Entscheidungen DARF ein unbekannter Revocation-Zustand NICHT als gültig behandelt werden.
8. Revocation MUSS zukünftige nicht autorisierte Verwendung verhindern.
9. Das Verhalten bereits laufender Operationen MUSS durch deren Contract definiert werden können.
10. Capability Composition MUSS Revocation abhängiger Authority berücksichtigen.
11. Lokale und entfernte Capabilities MÜSSEN dieselben grundlegenden Revocation-Semantiken verwenden.
12. Provider-Verfügbarkeit und Authority Revocation MÜSSEN getrennte Zustände bleiben.
13. Revocation MUSS mit Structured Concurrency und Cancellation integrierbar sein.
14. Revocation-Zustand und relevante Abhängigkeiten MÜSSEN kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Authority kann nachträglich zuverlässig entzogen werden,
- Sessions und Execution Domains können ganze Capability-Gruppen invalidieren,
- delegierte Authority bleibt kontrollierbar,
- Sicherheitsvorfälle können gezielt eingegrenzt werden,
- lokale und verteilte Capabilities verwenden ein gemeinsames Lifecycle-Modell.

### Negative Konsequenzen

- Revocation-Validierung erzeugt zusätzlichen Verwaltungsaufwand,
- verteilte Revocation benötigt kontrollierte Konsistenzmechanismen,
- laufende Operationen benötigen definierte Revocation-Semantik.

---

## Verworfene Alternativen

### Capabilities sind nach Ausgabe unwiderruflich

Verworfen.

Dies würde langfristig bestehende Authority erzeugen, die nicht zuverlässig zurückgenommen werden kann.

### Revocation ausschließlich durch kurze Lebensdauer

Verworfen.

Leases begrenzen Authority zeitlich, ersetzen aber keinen sofortigen Widerruf.

### Alle Capability-Objekte physisch zurückholen

Verworfen.

Dies ist insbesondere bei Delegation und verteilter Kommunikation nicht zuverlässig möglich.

### Provider-Entfernung als Revocation verwenden

Verworfen.

Verfügbarkeit und Authority sind unterschiedliche Systemzustände.

---

## Abhängigkeiten

- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-CAPABILITY-0007_Capability_Delegation`
- `ADR-CAPABILITY-0008_Capability_Attenuation`
- `ADR-ARCH-0010_Structured_Concurrency`
- `ADR-SECURITY-0010_Capability_Revocation`
- `ADR-SECURITY-0011_Capability_Leases_und_Epochs`
- `ADR-SECURITY-0018_Fail_Closed_für_Sicherheitsentscheidungen`

---

## Zugehörige NPSPECs

- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-DOMAIN-0001`
- `NPSPEC-CAPABILITY-REVOCATION-DEPENDENCY-0001`
- `NPSPEC-CAPABILITY-REVOCATION-RUNTIME-0001`
- `NPSPEC-CAPABILITY-REVOCATION-REMOTE-0001`
- `NPSPEC-CAPABILITY-REVOCATION-TEST-0001`

---

## Ergebnis

NovaOS behandelt den Entzug von Authority als regulären Bestandteil des Capability-Lifecycles:

```text
Authority
    ↓
Capability
    ↓
Delegation / Attenuation
    ↓
Active Use
    ↓
Revocation
    ↓
Authority Invalid
```

Damit können ausgegebene, delegierte und zusammengesetzte Capabilities kontrolliert ungültig gemacht werden, ohne auf das physische Zurückholen sämtlicher Capability-Repräsentationen angewiesen zu sein.