# ADR-SECURITY-0009 – Capability Delegation

## Status

Angenommen

## Kategorie

Security / Authorization / Capabilities

## Kontext

NovaOS-Komponenten müssen Authority kontrolliert an andere Komponenten weitergeben können.

Dies ist insbesondere notwendig bei:

- IPC,
- `Nova.Call`,
- Systemdiensten,
- Child Execution Domains,
- Plugins,
- KI-Agenten,
- verteilten Diensten.

Eine solche Weitergabe darf weder implizit erfolgen noch zusätzliche Authority erzeugen.

---

## Entscheidung

NovaOS unterstützt **explizite Capability Delegation** als Bestandteil des nativen Capability-Modells.

```text
Capability Holder
       ↓
Authorized Delegation
       ↓
Target
       ↓
Delegated Capability
```

Dabei gilt grundsätzlich:

```text
Authority(Delegated) ⊆ Authority(Source)
```

Delegation darf Authority übertragen oder durch Attenuation reduzieren, niemals erweitern.

---

## Grundprinzip

```text
Authority is delegated explicitly,
never inherited implicitly.
```

---

## Delegierbarkeit

Capabilities können hinsichtlich ihrer Delegierbarkeit klassifiziert werden:

```text
Non-Delegatable
Restricted Delegation
Delegatable
```

Der Besitz einer Capability bedeutet nicht automatisch, dass sie weitergegeben werden darf.

Nutzung und Delegation sind getrennte Authorities.

---

## Delegation und Attenuation

Vor der Delegation kann eine Capability attenuiert werden.

```text
Original:
Read + Write + Delete

        ↓ Attenuation

Delegated:
Read
```

Capability Attenuation ist damit der bevorzugte Mechanismus zur Umsetzung von Least Privilege bei Delegationen.

---

## Delegation Chains

Capabilities können über mehrere autorisierte Stufen delegiert werden.

```text
A
↓
B
↓
C
```

Dabei muss über die gesamte Kette gelten:

```text
Authority(C) ⊆ Authority(B) ⊆ Authority(A)
```

Keine Delegationsstufe darf zuvor entfernte Authority wiederherstellen.

---

## Holder Binding

Delegierte Capabilities können an einen bestimmten Kontext gebunden werden, beispielsweise:

- Identity,
- Execution Domain,
- Service,
- Session,
- Task Group,
- Transaction.

Dadurch kann verhindert werden, dass eine delegierte Capability außerhalb ihres vorgesehenen Kontexts verwendet wird.

---

## Lifetime und Revocation

Die Gültigkeitsdauer einer delegierten Capability darf die ihrer Quelle nicht überschreiten.

```text
Lifetime(Delegated) ≤ Lifetime(Source)
```

Delegierte Capabilities müssen mit dem Revocation-Modell von NovaOS integrierbar sein.

Die Aufhebung zugrunde liegender Authority muss abhängige Delegationen entsprechend ihrer Semantik ungültig machen können.

---

## Lokale und entfernte Delegation

NovaOS verwendet dieselbe Authority-Semantik für:

```text
Local Delegation
IPC Delegation
Remote Delegation
```

Transport, Standort oder Netzwerkpfad dürfen die Bedeutung der delegierten Authority nicht verändern.

Für Netzwerkgrenzen gelten zusätzlich die Sicherheitsanforderungen für Remote Capabilities.

---

## Confused-Deputy-Schutz

Dienste müssen eigene Authority von delegierter Caller-Authority unterscheiden können.

```text
Service
 ├── Own Authority
 └── Delegated Authority
```

Eine Operation im Auftrag eines Callers soll nur die dafür vorgesehene delegierte Authority verwenden.

---

## Keine Policy-Umgehung

Delegation darf keine bestehenden:

- MAC-Regeln,
- ABAC-Policies,
- Trust Requirements,
- Data-Sovereignty-Regeln,
- Information-Flow-Regeln

abschwächen oder umgehen.

Eine Capability delegiert Authority, nicht automatisch Trust oder zusätzliche Policy-Rechte.

---

## Resource Delegation

Capabilities können mit Resource Budgets verbunden sein.

Bei Delegation darf daraus keine unkontrollierte Vervielfachung von Ressourcenrechten entstehen.

```text
Parent Budget
     ↓
Delegated Budget
     ↓
Bounded Resource Authority
```

Die konkreten Budgetregeln werden in den Resource- und Capability-NPSPECs definiert.

---

## Audit und Introspection

Delegationen müssen kontrolliert nachvollziehbar sein.

```text
Source Holder
     ↓
Source Capability
     ↓
Attenuation
     ↓
Delegation
     ↓
Target Holder
```

NovaOS muss Delegationsketten analysieren können, ohne geheimes Capability-Material offenzulegen.

---

## Normative Anforderungen

1. NovaOS MUSS explizite Capability Delegation unterstützen.
2. Capability-Nutzung DARF NICHT automatisch Delegationsauthority implizieren.
3. Capabilities MÜSSEN als nicht delegierbar, eingeschränkt delegierbar oder delegierbar definierbar sein.
4. Delegierte Authority DARF die Authority ihrer Quelle NICHT überschreiten.
5. Capability Delegation MUSS mit Capability Attenuation kombinierbar sein.
6. Delegationsketten MÜSSEN monoton nicht-erweiternd sein.
7. Delegierte Capabilities MÜSSEN an definierte Holder oder Kontexte bindbar sein.
8. Delegierte Capabilities DÜRFEN nicht länger gültig sein als ihre zugrunde liegende Authority.
9. Delegation MUSS mit Capability Revocation integrierbar sein.
10. Lokale und entfernte Delegation MÜSSEN dieselbe Authority-Semantik verwenden.
11. Dienste MÜSSEN eigene und delegierte Authority unterscheiden können.
12. Delegation DARF MAC-, ABAC-, Trust-, Sovereignty- oder Information-Flow-Grenzen NICHT abschwächen.
13. Resource Authority DARF durch Delegation NICHT unkontrolliert vervielfacht werden.
14. Delegationen MÜSSEN kontrolliert auditierbar und introspektierbar sein.
15. Ungültige oder nicht eindeutig validierbare Delegationen MÜSSEN fail-safe abgelehnt werden.

---

## Konsequenzen

### Positive Konsequenzen

- kontrollierte Authority-Weitergabe,
- konsequentes Least Privilege,
- sichere Service- und IPC-Komposition,
- einheitliches Modell für lokale und entfernte Delegation,
- Schutz vor impliziter Authority-Vererbung,
- Grundlage für sichere KI-Agenten und Plugins.

### Negative Konsequenzen

- Delegationsketten erhöhen die Validierungskomplexität,
- Revocation muss abhängige Delegationen berücksichtigen,
- Holder Binding und Attenuation benötigen eindeutige Semantik.

---

## Verworfene Alternativen

### Automatische Authority-Vererbung

Verworfen.

Sie erzeugt unnötige Ambient Authority.

### Besitz impliziert Delegationsrecht

Verworfen.

Authority verwenden und Authority weitergeben sind unterschiedliche Sicherheitsoperationen.

### Unbeschränkte Capability-Weitergabe

Verworfen.

Dies widerspricht Least Privilege und Capability Attenuation.

---

## Abhängigkeiten

- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0003_Least_Privilege_und_Privilege_Separation`
- `ADR-SECURITY-0008_Capability_Attenuation`
- `ADR-DISTCOMM-0003_Remote_Capabilities`
- `ADR-DISTCOMM-0004_Capability_Delegation_über_Netzwerkgrenzen`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-CAPABILITY-DELEGATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-DELEGATION-POLICY-0001`
- `NPSPEC-SECURITY-CAPABILITY-HOLDER-BINDING-0001`
- `NPSPEC-SECURITY-CAPABILITY-DELEGATION-CHAIN-0001`
- `NPSPEC-SECURITY-CAPABILITY-DELEGATION-TEST-0001`

---

## Ergebnis

NovaOS behandelt Capability Delegation als explizite, kontrollierte Authority-Übertragung:

```text
Source Authority
       ↓
Attenuation
       ↓
Delegation
       ↓
Restricted Authority
       ↓
Target
```

Delegation kann Authority weitergeben oder reduzieren, aber niemals erweitern.

Damit können Anwendungen, Dienste und verteilte Komponenten zusammenarbeiten, ohne unnötige oder implizite Privilegien weiterzugeben.