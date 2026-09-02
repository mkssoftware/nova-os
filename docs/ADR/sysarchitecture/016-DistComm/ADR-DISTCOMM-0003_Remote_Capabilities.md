# ADR-DISTCOMM-0003 – Remote Capabilities

## Status

Angenommen

## Kategorie

Distributed Communication / Capabilities / Security / Nova.Call / Distributed Authority

## Kontext

NovaOS verwendet Capabilities als zentrale Grundlage für Autorisierung und kontrollierten Zugriff auf Systemfunktionen, Dienste und Ressourcen.

Mit `Nova.Call` können Dienste und Objekte unabhängig von ihrem aktuellen Ausführungsort angesprochen werden. Dadurch entsteht die Anforderung, Capabilities auch über System- und Netzwerkgrenzen hinweg sicher verwenden zu können.

Eine lokale Capability darf dabei nicht einfach als kopierbares Token oder globale Berechtigung behandelt werden. Remote-Kommunikation führt zusätzliche Trust-, Identity-, Revocation-, Delegation- und Netzwerkgrenzen ein.

NovaOS benötigt deshalb ein explizites Modell für **Remote Capabilities**.

---

## Entscheidung

NovaOS führt **Remote Capabilities als verteilte, kryptografisch beziehungsweise systemseitig verifizierbare Authority-Referenzen** ein.

Eine Remote Capability autorisiert einen definierten Zugriff auf eine entfernte Capability, einen Service oder ein Objekt.

```text
Caller
  ↓
Remote Capability
  ↓
Nova.Call
  ↓
Identity / Trust / Authority Validation
  ↓
Remote Capability Endpoint
  ↓
Authorized Operation
```

Remote Capabilities bleiben Bestandteil des allgemeinen NovaOS-Capability-Modells und bilden kein unabhängiges zweites Berechtigungssystem.

---

## Grundprinzip

```text
Reachability
    ≠
Identity
    ≠
Authority
```

und:

```text
Possession of a valid capability
grants only the authority explicitly represented by it.
```

---

## Remote Capability

Eine Remote Capability repräsentiert mindestens die logische Beziehung zwischen:

```text
Capability
 ├── Issuer
 ├── Subject / Holder
 ├── Target
 ├── Authority
 ├── Constraints
 └── Validity
```

Die konkrete Repräsentation, kryptografische Absicherung und Serialisierung werden in NPSPECs definiert.

Remote Capabilities dürfen keine unnötigen transport- oder standortspezifischen Details in ihre logische Identität einbetten.

---

## Trennung von Capability und Endpoint

Eine Remote Capability ist nicht identisch mit einer IP-Adresse, einem Port oder einem anderen Netzwerkendpunkt.

```text
Remote Capability
       ↓
Logical Target
       ↓
Resolution
       ↓
Current Endpoint
```

Dadurch kann sich der physische Standort eines Services ändern, ohne dass dadurch automatisch seine logische Authority geändert wird.

---

## Integration mit Nova.Call

`Nova.Call` verwendet Remote Capabilities als mögliche Authority für verteilte Aufrufe.

```text
Nova.Call
 ├── Target
 ├── Operation
 ├── Remote Capability
 └── Call Contract
```

Vor der Ausführung muss geprüft werden, ob die Capability die angeforderte Operation tatsächlich autorisiert.

Die erfolgreiche Auflösung eines Call Targets reicht dafür nicht aus.

---

## Least Authority

Remote Capabilities folgen dem Principle of Least Authority.

Eine Capability soll nur die minimal erforderlichen Rechte repräsentieren.

Beispielsweise:

```text
Capability A
    Read

Capability B
    Read + Append

Capability C
    Administrative Control
```

Die Existenz einer stärkeren Capability darf nicht dazu führen, dass schwächere Zugriffskontexte automatisch erweitert werden.

---

## Delegation

Remote Capabilities können kontrolliert delegierbar sein.

```text
Issuer
  ↓
Holder A
  ↓
Restricted Delegation
  ↓
Holder B
```

Delegation darf Authority nicht erweitern.

Es muss möglich sein, bei der Delegation zusätzliche Einschränkungen hinzuzufügen.

```text
Original Authority
        ↓
Delegation
        ↓
Equal or Reduced Authority
```

Eine delegierte Capability darf niemals mächtiger sein als ihre autorisierende Herkunft.

---

## Attenuation

NovaOS unterstützt das Konzept der Capability Attenuation.

Eine Capability kann bei Weitergabe eingeschränkt werden, beispielsweise hinsichtlich:

- erlaubter Operationen,
- Zielobjekte,
- Zeitfenster,
- Resource Budgets,
- Data-Sovereignty-Constraints,
- Trust Requirements,
- Call-Kontext.

Die konkrete Attenuation-Semantik wird in NPSPECs definiert.

---

## Revocation

Remote Capabilities müssen widerrufbar sein, wenn der zugrunde liegende Capability-Typ dies verlangt.

```text
Capability Issued
      ↓
Capability Active
      ↓
Revocation
      ↓
Capability Invalid
```

Revocation darf nicht voraussetzen, dass alle bereits ausgegebenen Capability-Repräsentationen physisch zurückgeholt werden können.

Die konkrete Revocation-Architektur wird separat spezifiziert.

---

## Identity und Trust

Remote Capability Validation muss die relevante Identität und Trust-Beziehung des entfernten Systems berücksichtigen können.

Eine gültige Capability für Service A darf nicht durch einen anderen Service B übernommen werden, nur weil dieser denselben Netzwerkendpunkt kontrollieren kann.

```text
Capability Target Identity
          =
Authenticated Remote Identity
```

Die genaue Bindung hängt vom jeweiligen Capability- und Trust-Contract ab.

---

## Transportunabhängigkeit

Remote Capabilities sind grundsätzlich unabhängig vom verwendeten Transport.

```text
Remote Capability
       ↓
Nova.Call
       ↓
TCP / QUIC / Overlay / Future Transport
```

Ein Transportwechsel, Multipath oder Connection Migration darf die Capability-Semantik nicht verändern.

---

## Data Sovereignty

Eine Remote Capability autorisiert nicht automatisch jeden möglichen Netzwerk- oder Ausführungsort.

Data-Sovereignty- und Information-Flow-Regeln bleiben zusätzliche Hard Constraints.

```text
Valid Capability
      +
Permitted Data Flow
      +
Permitted Location
      =
Authorized Remote Call
```

Eine Capability darf Sovereignty Policies nicht überschreiben.

---

## Capability Forwarding

Remote Capabilities dürfen nicht automatisch an weitere Systeme weitergegeben werden.

Die Weitergabe benötigt eine explizite Delegationsberechtigung.

```text
Holder A
   ↓
Remote Service B
   ↓
Service C
```

Service B darf die Capability nur an C weitergeben, wenn dies durch den Capability Contract erlaubt ist.

Damit wird unbeabsichtigte Authority Propagation verhindert.

---

## Structured Concurrency und Lifetime

Remote Capabilities können an einen definierten Ausführungs- oder Task-Kontext gebunden sein.

Beispielsweise kann eine Capability nur während der Lebensdauer einer:

- Execution Domain,
- Session,
- Task Group,
- Transaktion,
- Distributed Operation

gültig sein.

Das Ende des zugehörigen Lifetimes kann die weitere Nutzung der Capability ungültig machen.

---

## Fail-Safe Verhalten

Kann die Gültigkeit oder Authority einer Remote Capability nicht zuverlässig festgestellt werden, muss NovaOS fail-safe reagieren.

```text
Valid      → Evaluate Authority
Invalid    → Deny
Revoked    → Deny
Expired    → Deny
Unknown    → Deny / Revalidate
```

`Unknown` darf nicht als autorisiert interpretiert werden.

---

## Introspection und Audit

Remote-Capability-Nutzung muss kontrolliert introspektierbar und auditierbar sein.

NovaOS soll insbesondere Beziehungen darstellen können wie:

```text
Caller
  ↓
Remote Capability
  ↓
Issuer
  ↓
Target
  ↓
Authorized Operation
  ↓
Nova.Call
```

Dabei dürfen sensitive Capability-Geheimnisse nicht durch Introspection offengelegt werden.

Audit-Einträge sollen Capability Identity beziehungsweise sichere Referenzen verwenden, nicht notwendigerweise das eigentliche Capability-Material.

---

## Normative Anforderungen

1. NovaOS MUSS Remote Capabilities als Bestandteil des allgemeinen Capability-Modells unterstützen.
2. Remote Capabilities MÜSSEN eine explizite Authority für entfernte Ressourcen oder Dienste repräsentieren.
3. Netzwerk-Erreichbarkeit DARF keine implizite Authority erzeugen.
4. Remote Capabilities SOLLEN unabhängig von konkreten Netzwerkendpunkten identifiziert werden.
5. `Nova.Call` MUSS Remote Capabilities zur Autorisierung verteilter Calls verwenden können.
6. Eine Capability DARF ausschließlich die durch ihren Contract definierte Authority gewähren.
7. Remote Capabilities MÜSSEN Least Authority unterstützen.
8. Delegation DARF Authority NICHT erweitern.
9. Delegierte Capabilities MÜSSEN weiter eingeschränkt werden können.
10. Capability Forwarding MUSS explizit autorisiert sein.
11. Capability-Typen, die Revocation benötigen, MÜSSEN einen kontrollierten Widerruf unterstützen.
12. Remote Capability Validation MUSS relevante Identity- und Trust-Anforderungen berücksichtigen können.
13. Transportwechsel, Multipath und Connection Migration DÜRFEN die Capability-Semantik NICHT verändern.
14. Remote Capabilities DÜRFEN Security-, Trust-, Data-Sovereignty- oder Information-Flow-Policies NICHT überschreiben.
15. Nicht zuverlässig validierbare Remote Capabilities DÜRFEN NICHT als autorisiert behandelt werden.
16. Remote-Capability-Nutzung MUSS kontrolliert introspektierbar und auditierbar sein.
17. Introspection und Audit DÜRFEN geheimes Capability-Material NICHT unnötig offenlegen.

---

## Konsequenzen

### Positive Konsequenzen

- Capability-basierte Security funktioniert über Systemgrenzen hinweg,
- `Nova.Call` benötigt kein separates klassisches RPC-Berechtigungsmodell,
- Authority bleibt von Netzwerkadressen und physischen Standorten getrennt,
- sichere Delegation und Attenuation werden möglich,
- Services können Authority gezielt weitergeben, ohne globale Berechtigungen zu erzeugen,
- Remote Capabilities integrieren sich direkt in Nova.Trust, Data Sovereignty und Architecture Introspection.

### Negative Konsequenzen

- verteilte Capability-Validierung und Revocation erhöhen die Systemkomplexität,
- Capability Lifetime und Delegationsketten müssen eindeutig definiert werden,
- kryptografische beziehungsweise systemseitige Capability-Repräsentationen benötigen sorgfältiges Schlüssel- und Trust-Management.

---

## Verworfene Alternativen

### Netzwerkidentität als Authority

Verworfen.

IP-Adresse, Port, DNS-Name oder erreichbarer Service-Endpunkt stellen keine ausreichende Autorisierung dar.

### Globale Benutzerberechtigungen für Remote Services

Verworfen.

Dies würde zu grobkörniger Authority führen und das Least-Authority-Modell von NovaOS schwächen.

### Frei kopierbare Remote Access Tokens

Verworfen.

Unkontrollierte Kopierbarkeit würde Delegation, Attenuation, Revocation und Authority Propagation nur unzureichend abbilden.

### Separates Remote-Permission-System

Verworfen.

Lokale und entfernte Authority sollen auf demselben grundlegenden Capability-Modell basieren.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-IPC-0002_Capability_Based_IPC`
- `ADR-IPC-0004_Unified_Call_Abstraction`
- `ADR-IPC-0005_Lokale_und_entfernte_Aufrufe_unter_einem_Contract`
- `ADR-DISTCOMM-0001_Nova_Call_als_einheitliche_Distributed_Call_Abstraktion`
- `ADR-DISTCOMM-0002_Location_Transparency_ohne_Kosten_und_Fehler_zu_verbergen`
- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0006_Identity_Address_und_Route_strikt_trennen`
- `ADR-NETWORK-0009_Identity_und_Capability_Aware_Firewall`
- `ADR-NETWORK-0019_Connection_Migration`
- `ADR-NETWORK-0021_Data_Sovereignty_Aware_Routing`

---

## Zugehörige NPSPECs

- `NPSPEC-DISTCOMM-REMOTE-CAPABILITY-0001`
- `NPSPEC-DISTCOMM-CAPABILITY-REPRESENTATION-0001`
- `NPSPEC-DISTCOMM-CAPABILITY-DELEGATION-0001`
- `NPSPEC-DISTCOMM-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-DISTCOMM-CAPABILITY-REVOCATION-0001`
- `NPSPEC-DISTCOMM-CAPABILITY-VALIDATION-0001`
- `NPSPEC-DISTCOMM-CAPABILITY-AUDIT-0001`
- `NPSPEC-DISTCOMM-REMOTE-CAPABILITY-TEST-0001`

---

## Ergebnis

NovaOS erweitert sein Capability-Modell über lokale Systemgrenzen hinaus:

```text
Local Authority
      ↓
Remote Capability
      ↓
Nova.Call
      ↓
Identity + Trust Validation
      ↓
Capability Validation
      ↓
Policy + Sovereignty Validation
      ↓
Authorized Remote Operation
```

Damit basiert auch verteilte Kommunikation auf expliziter, minimaler und kontrolliert delegierbarer Authority, ohne Netzwerkadressen, Erreichbarkeit oder Service Discovery fälschlich mit Berechtigung gleichzusetzen.