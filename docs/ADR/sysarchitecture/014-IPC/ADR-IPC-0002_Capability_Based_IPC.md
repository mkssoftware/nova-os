# ADR-IPC-0002 – Capability-Based IPC

## Status

Angenommen

## Kategorie

IPC / Capabilities / Security / Authority / Isolation

## Kontext

`ADR-IPC-0001` definiert Typed IPC als natives Kommunikationsmodell von NovaOS.

Typkompatibilität allein darf jedoch keine Kommunikation autorisieren. Ein Prozess könnte einen IPC Contract kennen, ohne berechtigt zu sein, den entsprechenden Dienst oder Endpunkt zu verwenden.

Klassische globale Endpunktnamen oder frei adressierbare Service-IDs würden die Kenntnis einer Adresse mit Zugriffsberechtigung vermischen.

NovaOS benötigt deshalb eine IPC-Architektur, bei der Kommunikation auf expliziter Authority basiert.

---

## Entscheidung

NovaOS verwendet **Capability-Based IPC**.

Eine Execution Domain darf einen geschützten IPC-Endpunkt nur verwenden, wenn sie eine gültige Capability für diesen Endpunkt beziehungsweise die entsprechende Operation besitzt.

```text
IPC Capability
      +
Typed IPC Request
      ↓
Authority Validation
      ↓
IPC Endpoint
```

Die Kenntnis von Name, Identity oder IPC Contract allein gewährt keinen Zugriff.

---

## Grundprinzip

```text
Knowing an endpoint is not authority.

Holding the capability is authority.
```

---

## IPC Capabilities

Eine IPC Capability repräsentiert explizite Authority zur Kommunikation mit einem bestimmten IPC-Endpunkt oder Service.

Sie kann auf bestimmte Rechte begrenzt sein, beispielsweise:

```text
Connect
Call
Send
Receive
Delegate
Manage
```

Die konkrete Rechtekodierung wird in den NPSPECs definiert.

---

## Endpoint Identity und Authority

Endpoint Identity und IPC Capability bleiben getrennte Konzepte.

```text
Endpoint Identity
→ welcher Endpunkt?

IPC Capability
→ darf dieser Endpunkt verwendet werden?
```

Ein Endpunkt darf deshalb auffindbar oder introspektierbar sein, ohne dadurch automatisch nutzbar zu werden.

---

## Least Privilege

Capabilities sollen nur die tatsächlich benötigten IPC-Rechte enthalten.

Eine Anwendung, die ausschließlich eine bestimmte Operation benötigt, soll keine umfassende Management- oder Delegation-Authority erhalten.

Damit wird Least Privilege auch über Prozess- und Servicegrenzen hinweg durchgesetzt.

---

## Capability Transfer

IPC Capabilities dürfen kontrolliert zwischen Execution Domains übertragen werden.

```text
Domain A
   ↓
Delegated Capability
   ↓
Domain B
```

Eine Delegation darf keine stärkere Authority erzeugen, als die delegierende Domain besitzt.

Nicht delegierbare Capabilities müssen unterstützt werden können.

---

## Capability Revocation

IPC Authority muss widerrufbar sein, wenn der zugrunde liegende Zugriff nicht mehr zulässig ist.

Gründe können beispielsweise sein:

- Service Shutdown,
- Execution-Domain-Ende,
- Security Policy Change,
- Trust Revocation,
- expliziter Authority-Entzug.

Widerrufene Capabilities dürfen keine neuen autorisierten IPC-Operationen ermöglichen.

---

## Service Discovery

Service Discovery und Service Authority bleiben getrennt.

```text
Service Discovery
      ↓
Endpoint Identity

Authority Grant
      ↓
IPC Capability
```

Damit kann NovaOS Dienste auffindbar machen, ohne allen Suchenden Zugriff auf diese Dienste zu gewähren.

---

## Typed IPC Integration

Capability-Based IPC ergänzt Typed IPC.

Eine gültige Kommunikation benötigt grundsätzlich beide Dimensionen:

```text
Type Compatibility
        +
Valid Authority
        ↓
Allowed IPC
```

Eine Capability macht inkompatible Nachrichten nicht gültig.

Umgekehrt erzeugt ein kompatibler Typ keine Zugriffsberechtigung.

---

## Shared Objects und Zero-Copy

Werden über IPC Shared Buffers, Memory Objects oder andere Ressourcen übertragen, müssen deren Authorities ebenfalls explizit behandelt werden.

Eine IPC Capability für einen Service gewährt nicht automatisch Zugriff auf alle durch diesen Service referenzierten Speicherobjekte.

Damit bleiben IPC Authority und Memory/Object Authority sauber getrennt.

---

## Nova.Trust

Die Vergabe von IPC Capabilities kann von Nova.Trust und System Policies abhängig sein.

```text
Identity
   ↓
Trust / Permission
   ↓
Capability Grant
   ↓
IPC Access
```

Besonders privilegierte Systemdienste können entsprechend strengere Anforderungen an Capability Grants stellen.

---

## Fail-Safe Default

Kann die erforderliche IPC Authority nicht eindeutig nachgewiesen werden, wird die Operation verweigert.

```text
Missing / Invalid / Unknown Capability
                ↓
              Deny
```

Es gibt keinen permissiven Fallback auf bloße Endpoint-Kenntnis.

---

## Normative Anforderungen

1. NovaOS MUSS geschützte IPC-Kommunikation über explizite Capabilities autorisieren.
2. Endpoint Identity und IPC Authority MÜSSEN getrennte Konzepte bleiben.
3. Die Kenntnis eines Endpunkts oder IPC Contracts DARF keine Authority verleihen.
4. IPC Capabilities MÜSSEN auf bestimmte Endpunkte oder Services begrenzbar sein.
5. IPC Capabilities MÜSSEN unterschiedliche Rechte ausdrücken können.
6. Capability Grants MÜSSEN dem Least-Privilege-Prinzip folgen.
7. Capabilities DÜRFEN kontrolliert delegierbar sein.
8. Delegation DARF keine stärkere Authority erzeugen.
9. Nicht delegierbare Capabilities MÜSSEN unterstützt werden können.
10. IPC Capabilities MÜSSEN kontrolliert widerrufbar sein.
11. Service Discovery und Service Authority MÜSSEN getrennt bleiben.
12. Typed IPC Compatibility und IPC Authority MÜSSEN unabhängig geprüft werden.
13. IPC Authority DARF NICHT automatisch Authority über übertragene Shared Objects oder Buffer gewähren.
14. Fehlende oder ungültige IPC Authority MUSS nach dem Fail-Safe-Prinzip zur Verweigerung führen.
15. Capability Grants, Delegationen und Revocations MÜSSEN kontrolliert auditier- und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- konsequentes Least Privilege über IPC-Grenzen,
- keine implizite Authority durch globale Endpunktnamen,
- sichere Delegation von Servicezugriff,
- klare Trennung zwischen Discovery und Berechtigung,
- gute Integration mit Nova.Trust und Driver Capabilities,
- Grundlage für sichere komponierbare Dienste.

### Negative Konsequenzen

- Capability-Lifecycles müssen verwaltet werden,
- Delegation und Revocation benötigen definierte Semantik,
- Compatibility APIs müssen klassische namensbasierte IPC-Modelle auf Capabilities abbilden.

---

## Verworfene Alternativen

### Endpunktname als Zugriffsberechtigung

Verworfen.

Die Kenntnis eines Namens darf keine Security Authority darstellen.

### Globale IPC-Berechtigungen pro Anwendung

Verworfen.

Authority soll auf konkrete Services, Endpunkte und erforderliche Rechte begrenzbar sein.

### Typed IPC ohne separates Authority-Modell

Verworfen.

Typkompatibilität sagt nichts darüber aus, ob Kommunikation erlaubt ist.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-IPC-0001_Typed_IPC`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-MEMORY-0008_Shared_Memory_über_explizite_Shared_Objects`
- `ADR-DRIVER-0004_Driver_Capability_Model`

---

## Zugehörige NPSPECs

- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-IPC-CAPABILITY-GRANT-0001`
- `NPSPEC-IPC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-IPC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-IPC-SERVICE-DISCOVERY-0001`
- `NPSPEC-IPC-CAPABILITY-TEST-0001`

---

## Ergebnis

IPC-Zugriff basiert in NovaOS auf expliziter Authority:

```text
Endpoint Identity
        +
Typed IPC Contract
        +
IPC Capability
        ↓
Authorized Communication
```

Damit wird die Fähigkeit zur Kommunikation nicht aus Namen, Adressen oder Typkenntnis abgeleitet, sondern ausdrücklich und begrenzt über Capabilities vergeben.