# ADR-CAPABILITY-0008 – Capability Attenuation

## Status

Angenommen

## Kategorie

Capability Architecture / Authority / Least Privilege

## Kontext

NovaOS-Capabilities können Authority besitzen, die für eine konkrete Weitergabe oder Teiloperation umfangreicher ist als notwendig.

Beispielsweise kann eine Anwendung besitzen:

```text
Storage.ReadWrite(/Documents/*)
```

Ein aufgerufener Dienst benötigt jedoch möglicherweise ausschließlich:

```text
Storage.Read(/Documents/Image.nova)
```

Das vollständige Weiterreichen der ursprünglichen Authority würde Least Privilege verletzen.

NovaOS benötigt deshalb einen Mechanismus, mit dem Capabilities gezielt eingeschränkt werden können.

---

## Entscheidung

NovaOS unterstützt **Capability Attenuation als native Operation zur irreversiblen Einschränkung einer Capability innerhalb einer Ableitungskette**.

Es gilt:

```text
Authority(Derived)
    ⊆
Authority(Source)
```

Attenuation kann Authority reduzieren, aber niemals erweitern.

---

## Grundprinzip

```text
Reduce authority before sharing it.
```

---

## Attenuation

Aus einer bestehenden Capability kann eine stärker eingeschränkte Capability abgeleitet werden.

```text
Source Capability
        ↓
Attenuation
        ↓
Derived Capability
```

Beispiel:

```text
Storage.ReadWrite(/Documents/*)
        ↓
Storage.Read(/Documents/Image.nova)
```

Die abgeleitete Capability besitzt nur die verbleibende Authority.

---

## Einschränkbare Eigenschaften

Attenuation kann unter anderem beschränken:

```text
Operations
Targets
Resource Scope
Lifetime
Resource Budget
Call Count
Delegation Rights
Delegation Depth
Trust Requirements
Data Sovereignty
Execution Context
```

Die konkreten Constraint-Typen werden in NPSPECs definiert.

---

## Monotonie

Attenuation ist innerhalb einer Ableitungskette monoton.

```text
Capability A
    ↓
Capability B
    ↓
Capability C
```

Es muss gelten:

```text
Authority(C)
⊆ Authority(B)
⊆ Authority(A)
```

Eine später abgeleitete Capability darf zuvor entfernte Authority nicht wiederherstellen.

---

## Attenuation und Delegation

Attenuation und Delegation sind getrennte, aber eng verbundene Operationen.

```text
Source Capability
       ↓
Attenuation
       ↓
Restricted Capability
       ↓
Delegation
       ↓
Delegate
```

NovaOS soll Authority möglichst vor einer Delegation auf den tatsächlich benötigten Umfang reduzieren.

---

## Operation Attenuation

Eine Capability kann auf bestimmte Operationen reduziert werden.

```text
Read + Write + Delete
        ↓
Attenuation
        ↓
Read
```

Die entfernten Operationen können durch weitere Ableitung nicht wieder hinzugefügt werden.

---

## Target Attenuation

Authority kann auf kleinere Zielbereiche beschränkt werden.

```text
/Documents/*
      ↓
/Documents/Project/*
      ↓
/Documents/Project/Image.nova
```

Damit können Dienste Zugriff auf einzelne Ressourcen erhalten, ohne Zugriff auf den gesamten ursprünglichen Bereich zu bekommen.

---

## Zeitliche Attenuation

Capabilities können auf kürzere Gültigkeitszeiträume beschränkt werden.

```text
Source Lifetime
      ↓
Derived Lifetime
```

Dabei gilt:

```text
Lifetime(Derived)
≤ Lifetime(Source)
```

Leases und Epochs können hierfür verwendet werden.

---

## Resource Attenuation

Capability Attenuation kann Resource Budgets reduzieren.

```text
Source Budget
CPU:     1000 ms
Memory:  512 MiB
Network: 100 MiB

        ↓

Derived Budget
CPU:     100 ms
Memory:  64 MiB
Network: 10 MiB
```

Ableitung darf Ressourcenrechte nicht vervielfachen.

---

## Delegation Attenuation

Auch die weitere Delegierbarkeit kann eingeschränkt werden.

Beispielsweise:

```text
Delegatable
     ↓
Restricted Delegation
     ↓
Non-Delegatable
```

Eine entfernte Delegationsberechtigung darf nicht durch weitere Ableitung wiederhergestellt werden.

---

## Hard Constraints

Attenuation darf bestehende Hard Constraints nur beibehalten oder verschärfen.

Dies betrifft insbesondere:

```text
Security
Trust
Information Flow
Data Sovereignty
Resource Limits
Delegation Limits
```

Beispiel:

```text
Allowed Region: EU
        ↓
Attenuation
        ↓
Allowed Region: Germany
```

zulässig.

Dagegen:

```text
Allowed Region: EU
        ↓
Allowed Region: Worldwide
```

unzulässig.

---

## Capability Composition

Bei Capability Composition kann Authority für einzelne Teiloperationen attenuiert werden.

```text
Composite Capability
       │
       ├── Capability A → Authority A
       ├── Capability B → Authority B
       └── Capability C → Authority C
```

Jede Teil-Capability erhält dadurch nur die Authority, die für ihre konkrete Aufgabe notwendig ist.

---

## Revocation und Lifecycle

Eine attenuierte Capability bleibt von ihrer ursprünglichen Authority abhängig.

```text
Source Revoked
      ↓
Derived Capability
      ↓
Invalid
```

Attenuation darf Revocation, Leases oder Epochs der Quelle nicht umgehen.

---

## Lokale und entfernte Capabilities

Attenuation verwendet für lokale und entfernte Capabilities dieselben Authority-Grundregeln.

```text
Local
IPC
Nova.Call
Remote
```

Ein Wechsel des Transport- oder Ausführungsortes darf eine attenuierte Authority nicht erweitern.

---

## Introspection und Audit

NovaOS muss kontrolliert nachvollziehen können:

```text
Source Capability
Original Authority
Applied Restrictions
Derived Authority
Delegation State
Lifetime
Revocation Dependency
```

Geheimes Capability-Material darf dabei nicht offengelegt werden.

---

## Normative Anforderungen

1. NovaOS MUSS Capability Attenuation unterstützen.
2. Eine attenuierte Capability DARF niemals mehr Authority als ihre Quelle besitzen.
3. Attenuation MUSS monoton nicht-erweiternd sein.
4. Entfernte Authority DARF innerhalb derselben Ableitungskette NICHT wiederhergestellt werden.
5. Operationen und Targets MÜSSEN einschränkbar sein.
6. Lebensdauer und Resource Budgets MÜSSEN einschränkbar sein können.
7. Delegierbarkeit MUSS attenuierbar sein.
8. Security-, Trust-, Information-Flow- und Sovereignty-Constraints DÜRFEN nur erhalten oder verschärft werden.
9. Attenuation MUSS mit Capability Delegation kombinierbar sein.
10. Capability Composition MUSS attenuierte Authority für Teiloperationen verwenden können.
11. Attenuierte Capabilities DÜRFEN Revocation, Leases oder Epochs ihrer Quelle NICHT umgehen.
12. Lokale und entfernte Capabilities MÜSSEN dieselben grundlegenden Attenuation-Regeln verwenden können.
13. Attenuation und effektive Authority MÜSSEN kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- konsequentes Least Privilege,
- sichere Delegation,
- präzise Authority für Capability Composition,
- geringere Auswirkungen kompromittierter Komponenten,
- kontrollierbare Resource- und Sovereignty-Grenzen.

### Negative Konsequenzen

- Capability Constraints werden komplexer,
- Ableitungsketten müssen korrekt validiert werden,
- Authority-Vergleiche benötigen klar definierte Semantik.

---

## Verworfene Alternativen

### Capabilities nur vollständig weitergeben

Verworfen.

Dies würde unnötige Authority übertragen.

### Attenuation darf später rückgängig gemacht werden

Verworfen.

Eine abgeleitete Capability könnte sonst Authority wiederherstellen, die ihr bewusst entzogen wurde.

### Einschränkungen nur auf Operationen

Verworfen.

Least Privilege betrifft auch Targets, Lebensdauer, Ressourcen, Delegation, Trust und Data Sovereignty.

### Neue eingeschränkte Capability unabhängig von der Quelle erzeugen

Verworfen.

Die Ableitung muss nachvollziehbar an die ursprüngliche Authority und deren Lifecycle gebunden bleiben.

---

## Abhängigkeiten

- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-CAPABILITY-0005_Capability_Composition`
- `ADR-CAPABILITY-0007_Capability_Delegation`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0003_Least_Privilege_und_Privilege_Separation`
- `ADR-SECURITY-0008_Capability_Attenuation`
- `ADR-SECURITY-0010_Capability_Revocation`
- `ADR-SECURITY-0011_Capability_Leases_und_Epochs`

---

## Zugehörige NPSPECs

- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-CONSTRAINTS-0001`
- `NPSPEC-CAPABILITY-AUTHORITY-SUBSET-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-LIFECYCLE-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-TEST-0001`

---

## Ergebnis

NovaOS kann vorhandene Authority gezielt auf den für eine konkrete Aufgabe benötigten Umfang reduzieren:

```text
Broad Authority
      ↓
Attenuation
      ↓
Scoped Authority
      ↓
Delegation / Execution
```

Dabei gilt verbindlich:

```text
Authority(Derived)
    ⊆
Authority(Source)
```

Capability Attenuation bildet damit die zentrale Grundlage, um Least Privilege auch bei Capability Delegation, Composition und verteilter Ausführung konsequent aufrechtzuerhalten.