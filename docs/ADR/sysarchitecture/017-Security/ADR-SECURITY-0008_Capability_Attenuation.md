# ADR-SECURITY-0008 – Capability Attenuation

## Status

Angenommen

## Kategorie

Security / Authorization / Capabilities

## Kontext

Capabilities können zwischen Komponenten, Execution Domains und verteilten Diensten weitergegeben werden.

Dabei darf eine Komponente nicht gezwungen sein, ihre vollständige Authority weiterzugeben, wenn der Empfänger nur einen Teil davon benötigt.

Beispiel:

```text
Original Capability:
Read + Write + Delete

Required by Service:
Read
```

Die Weitergabe der vollständigen Capability würde gegen Least Privilege verstoßen.

---

## Entscheidung

NovaOS unterstützt **Capability Attenuation** als grundlegende Eigenschaft des Capability-Modells.

Aus einer bestehenden Capability kann eine stärker eingeschränkte Capability abgeleitet werden.

Dabei gilt:

```text
Authority(Derived) ⊆ Authority(Original)
```

Attenuation darf Authority ausschließlich erhalten oder reduzieren, niemals erweitern.

---

## Grundprinzip

```text
Authority can be reduced,
never amplified through attenuation.
```

---

## Einschränkbare Eigenschaften

Eine Capability kann unter anderem eingeschränkt werden hinsichtlich:

- erlaubter Operationen,
- Zielressourcen,
- Gültigkeitsdauer,
- Resource Budgets,
- Nutzungskontext,
- Delegierbarkeit,
- Delegationstiefe,
- Data-Sovereignty-Anforderungen,
- Trust Requirements.

Beispiel:

```text
Original:
Read + Write
All Documents
24 Hours

Derived:
Read
Document X
10 Minutes
```

---

## Monotone Authority

Jede Attenuation muss monoton sein.

```text
Capability A
    ↓ attenuation
Capability B
    ↓ attenuation
Capability C
```

Dabei gilt:

```text
Authority(C) ⊆ Authority(B) ⊆ Authority(A)
```

Keine Stufe darf zuvor entfernte Authority wiederherstellen.

---

## Delegation

Capability Attenuation ist der bevorzugte Mechanismus zur sicheren Delegation begrenzter Authority.

```text
Caller
  │ Full Capability
  ↓
Attenuation
  │ Restricted Capability
  ↓
Service
```

Dadurch muss ein Caller einem aufgerufenen Dienst nicht mehr Authority übertragen, als dieser für die konkrete Aufgabe benötigt.

---

## Unveränderliche Constraints

Bereits bestehende Einschränkungen einer Capability dürfen durch Attenuation nicht abgeschwächt werden.

Dies betrifft insbesondere:

- Security Constraints,
- Data Sovereignty,
- Trust Requirements,
- Gültigkeitsgrenzen,
- Delegationsbeschränkungen.

Eine abgeleitete Capability darf nur gleich starke oder strengere Constraints besitzen.

---

## Capability Chains

NovaOS muss attenuierte Capability-Ketten unterstützen können.

Die resultierende Authority ergibt sich aus allen wirksamen Einschränkungen der Kette.

```text
Original Capability
       ↓
Restriction A
       ↓
Restriction B
       ↓
Effective Capability
```

Die konkrete Repräsentation und Validierung solcher Ketten wird in NPSPECs definiert.

---

## Lokale und entfernte Capabilities

Attenuation verwendet für lokale und entfernte Capabilities dieselbe Semantik.

Ein Wechsel zwischen:

```text
Local IPC
Remote Nova.Call
Service Migration
```

darf die Authority einer attenuierten Capability nicht verändern.

---

## Revocation und Lifetime

Eine abgeleitete Capability darf nicht länger gültig sein als ihre Authority-Quelle.

```text
Lifetime(Derived) ≤ Lifetime(Original)
```

Revocation der zugrunde liegenden Authority muss entsprechend dem jeweiligen Capability-Typ auch abhängige Capabilities ungültig machen können.

---

## Audit und Introspection

NovaOS muss nachvollziehen können, dass Authority durch Attenuation entstanden ist.

Dabei sollen insbesondere:

```text
Original Authority
        ↓
Applied Restrictions
        ↓
Effective Authority
```

analysierbar sein, ohne geheimes Capability-Material offenzulegen.

---

## Normative Anforderungen

1. NovaOS MUSS Capability Attenuation unterstützen.
2. Attenuation DARF Authority niemals erweitern.
3. Für jede abgeleitete Capability MUSS `Authority(Derived) ⊆ Authority(Original)` gelten.
4. Operationen, Targets, Lifetime und andere Capability-Constraints MÜSSEN einschränkbar sein.
5. Bereits bestehende Constraints DÜRFEN durch Attenuation NICHT abgeschwächt werden.
6. Entfernte Authority DARF durch weitere Attenuation NICHT wiederhergestellt werden.
7. Attenuation MUSS mit Capability Delegation kombinierbar sein.
8. Abgeleitete Capabilities DÜRFEN nicht länger gültig sein als ihre zugrunde liegende Authority.
9. Security-, Trust- und Sovereignty-Grenzen DÜRFEN durch Attenuation NICHT abgeschwächt werden.
10. Attenuation MUSS für lokale und entfernte Capabilities dieselbe Authority-Semantik besitzen.
11. Attenuation MUSS mit Capability Revocation integrierbar sein.
12. Capability-Attenuation MUSS kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- konsequente Umsetzung von Least Privilege,
- sichere Capability Delegation,
- kleinere Authority-Sets für Dienste und Subsysteme,
- geringerer Blast Radius kompromittierter Komponenten,
- flexible Bildung aufgabenspezifischer Capabilities.

### Negative Konsequenzen

- Capability-Validierung wird komplexer,
- lange Attenuation Chains können zusätzlichen Verwaltungsaufwand erzeugen,
- Constraints benötigen eindeutig definierte Vererbungs- und Kombinationsregeln.

---

## Verworfene Alternativen

### Capabilities nur vollständig weitergeben

Verworfen.

Dies würde unnötig breite Authority übertragen.

### Attenuation mit späterer Authority-Erweiterung

Verworfen.

Dadurch wäre die Sicherheitsgarantie der Ableitung nicht mehr monoton.

### Für jede Einschränkung neue globale Capability anfordern

Verworfen.

Dies würde unnötige Abhängigkeit von zentraler Capability-Issuance erzeugen.

---

## Abhängigkeiten

- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0003_Least_Privilege_und_Privilege_Separation`
- `ADR-DISTCOMM-0003_Remote_Capabilities`
- `ADR-DISTCOMM-0004_Capability_Delegation_über_Netzwerkgrenzen`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-CONSTRAINT-0001`
- `NPSPEC-SECURITY-CAPABILITY-CHAIN-0001`
- `NPSPEC-SECURITY-CAPABILITY-ATTENUATION-TEST-0001`

---

## Ergebnis

Capability Attenuation ermöglicht NovaOS, vorhandene Authority sicher zu verkleinern:

```text
Broad Capability
       ↓
Attenuation
       ↓
Task-specific Capability
       ↓
Least Authority
```

Authority kann dadurch gezielt weitergegeben und eingeschränkt werden, ohne dass aus einer bestehenden Capability jemals zusätzliche Authority entstehen kann.