# ADR-SECURITY-0010 – Capability Revocation

## Status

Angenommen

## Kategorie

Security / Authorization / Capabilities

## Kontext

Capabilities können in NovaOS über längere Zeit bestehen, delegiert, attenuiert oder an entfernte Komponenten übertragen werden.

Authority muss jedoch wieder entzogen werden können, beispielsweise wenn:

- eine Berechtigung zurückgezogen wird,
- ein Benutzer oder Dienst deaktiviert wird,
- eine Session endet,
- eine Komponente kompromittiert wurde,
- sich Security- oder Trust-Policies ändern,
- eine delegierte Authority nicht mehr gültig sein soll.

Alle verteilten Capability-Kopien physisch zurückholen zu müssen, wäre insbesondere in verteilten Systemen nicht zuverlässig möglich.

---

## Entscheidung

NovaOS unterstützt **Capability Revocation** als grundlegende Eigenschaft des Capability-Modells.

```text
Capability
    ↓
Valid
    ↓
Revocation
    ↓
Revoked
    ↓
Authority unavailable
```

Revocation entzieht bestehende Authority, ohne die physische Vernichtung aller Capability-Repräsentationen vorauszusetzen.

---

## Grundprinzip

```text
Possession does not guarantee continued authority.
```

Eine Capability muss zum Zeitpunkt ihrer Verwendung noch gültige Authority repräsentieren.

---

## Revocation-Modell

Capabilities können abhängig von ihrem Typ unterschiedliche Revocation-Mechanismen verwenden.

NovaOS darf beispielsweise unterstützen:

- direkte Revocation,
- Revocation über Authority-Objekte,
- Generationen oder Epochen,
- zeitlich begrenzte Capabilities,
- Revocation Domains.

Die konkrete Implementierung wird in NPSPECs definiert.

Die Architektur schreibt keinen einzelnen universellen Mechanismus für alle Capability-Typen vor.

---

## Delegierte Capabilities

Revocation muss mit Capability Delegation integriert sein.

```text
Original Capability
       ↓
Delegation
       ↓
Derived Capability
```

Wird die zugrunde liegende Authority widerrufen, müssen davon abhängige Delegationen entsprechend ihrer definierten Revocation-Semantik ebenfalls ungültig werden können.

Revocation darf nicht durch vorherige Delegation umgangen werden.

---

## Attenuierte Capabilities

Attenuation erzeugt keine unabhängige neue Authority.

```text
Source Authority
      ↓
Attenuation
      ↓
Restricted Authority
```

Wird die zugrunde liegende Authority widerrufen, darf eine daraus abgeleitete Capability nicht weiterhin dieselbe Authority ausüben.

---

## Revocation Domains

Zusammengehörige Capabilities können einer gemeinsamen Revocation Domain zugeordnet werden.

Dadurch können beispielsweise:

```text
Session
Task Group
Application Instance
Transaction
Service Instance
```

ihre zugehörige Authority gemeinsam verlieren.

Dies unterstützt Structured Concurrency und hierarchische Lebenszyklen.

---

## Lifetime und Revocation

Zeitliche Begrenzung und Revocation ergänzen sich.

```text
Capability Validity =
    Lifetime
    +
    Revocation State
```

Eine abgelaufene Capability ist unabhängig vom Revocation-Zustand ungültig.

Kurzlebige Capabilities können den Bedarf für aufwendige aktive Revocation reduzieren.

---

## Lokale und entfernte Capabilities

Die Revocation-Semantik muss für lokale und Remote Capabilities konsistent bleiben.

```text
Local Capability
IPC Capability
Remote Capability
```

Ein Wechsel des Transportes, Netzwerkpfades oder Ausführungsortes darf widerrufene Authority nicht wiederherstellen.

---

## Fail-Safe Verhalten

Kann der erforderliche Revocation-Zustand nicht zuverlässig bestimmt werden, gilt für sicherheitskritische Authority:

```text
Unknown
    ≠
Valid
```

Abhängig vom Capability-Typ muss NovaOS die Operation:

```text
Deny
```

oder eine erneute Validierung verlangen.

Verfügbarkeit darf nicht automatisch Vorrang vor Security erhalten.

---

## Laufende Operationen

Revocation einer Capability bedeutet nicht automatisch, dass bereits ausgeführte Effekte rückgängig gemacht werden können.

NovaOS muss zwischen:

```text
Authority for future operations
```

und:

```text
Already committed effects
```

unterscheiden.

Wie laufende Operationen, Streams oder Transaktionen auf Revocation reagieren, wird durch deren Contracts bestimmt.

---

## Resource Authority

Revocation muss auch Authority über Ressourcen entziehen können.

Beispiele:

- Device Access,
- Network Access,
- Storage Access,
- Shared Objects,
- administrative Operationen.

Dabei müssen Ressourcen und Handles nach definierten Lifecycle-Regeln freigegeben oder unzugänglich gemacht werden können.

---

## Audit und Introspection

Revocation muss kontrolliert nachvollziehbar sein.

NovaOS soll insbesondere erkennen können:

```text
Capability
 ├── Authority Source
 ├── Revocation Domain
 ├── Current State
 └── Dependent Delegations
```

Geheimes Capability-Material darf dabei nicht offengelegt werden.

---

## Normative Anforderungen

1. NovaOS MUSS Capability Revocation unterstützen.
2. Revocation DARF NICHT voraussetzen, dass alle Capability-Repräsentationen physisch zurückgeholt werden.
3. NovaOS DARF unterschiedliche Revocation-Mechanismen für unterschiedliche Capability-Typen verwenden.
4. Delegierte Capabilities MÜSSEN in die Revocation-Semantik ihrer zugrunde liegenden Authority integrierbar sein.
5. Attenuation DARF Revocation NICHT umgehen.
6. NovaOS MUSS Revocation Domains unterstützen können.
7. Capability Lifetime und Revocation MÜSSEN gemeinsam berücksichtigt werden.
8. Lokale und entfernte Capabilities MÜSSEN konsistente Revocation-Semantik besitzen.
9. Transport- oder Location-Wechsel DÜRFEN widerrufene Authority NICHT wiederherstellen.
10. Unbekannter Revocation-Zustand MUSS für sicherheitskritische Authority fail-safe behandelt werden.
11. Revocation MUSS von bereits abgeschlossenen Effekten unterschieden werden.
12. Laufende Operationen MÜSSEN eine definierte Revocation-Semantik besitzen können.
13. Revocation MUSS mit Resource Lifecycle und Structured Concurrency integrierbar sein.
14. Revocation-Ereignisse MÜSSEN kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Authority kann zuverlässig wieder entzogen werden,
- Delegation erzeugt keine dauerhaft unkontrollierbare Authority,
- kompromittierte oder veraltete Capabilities können invalidiert werden,
- Sessions und Task Groups können Authority gemeinsam verwalten,
- lokale und verteilte Capabilities folgen demselben Sicherheitsmodell.

### Negative Konsequenzen

- Revocation erhöht die Capability-Validierungskomplexität,
- verteilte Revocation kann zusätzliche Zustands- und Kommunikationskosten verursachen,
- starke Revocation-Garantien können mit Performance und Verfügbarkeit konkurrieren.

---

## Verworfene Alternativen

### Nicht widerrufbare Capabilities

Verworfen als allgemeines Modell.

Langfristige oder delegierte Authority könnte sonst nicht zuverlässig entzogen werden.

### Ausschließlich kurze Ablaufzeiten

Verworfen.

Lifetime reduziert das Revocation-Problem, ersetzt aber nicht den sofortigen Entzug kritischer Authority.

### Physisches Zurückholen aller Capabilities

Verworfen.

Dies ist insbesondere bei Delegation und verteilten Systemen nicht zuverlässig durchsetzbar.

---

## Abhängigkeiten

- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0008_Capability_Attenuation`
- `ADR-SECURITY-0009_Capability_Delegation`
- `ADR-DISTCOMM-0003_Remote_Capabilities`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-CAPABILITY-REVOCATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-REVOCATION-DOMAIN-0001`
- `NPSPEC-SECURITY-CAPABILITY-LIFETIME-0001`
- `NPSPEC-SECURITY-CAPABILITY-REVOCATION-TEST-0001`

---

## Ergebnis

Capability Revocation stellt sicher, dass einmal vergebene Authority nicht dauerhaft bestehen muss.

```text
Authority
   ↓
Capability
   ↓
Delegation / Attenuation
   ↓
Revocation
   ↓
Authority no longer valid
```

Damit können Capabilities in NovaOS sicher delegiert und verteilt werden, während die zugrunde liegende Authority weiterhin kontrolliert entziehbar bleibt.