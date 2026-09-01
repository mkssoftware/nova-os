# ADR-NETWORK-0009 – Identity- und Capability-Aware Firewall

## Status

Angenommen

## Kategorie

Network / Firewall / Security / Identity / Capabilities

## Kontext

Klassische Firewalls treffen Entscheidungen überwiegend anhand technischer Netzwerkmerkmale wie IP-Adressen, Ports, Protokollen und Interfaces.

Diese Informationen bleiben für Paketverarbeitung wichtig, beschreiben jedoch nicht zuverlässig, **welcher Dienst, welche Anwendung oder welche Execution Domain kommuniziert und mit welcher Authority dies geschieht**.

Durch die Trennung von Identity, Address und Route sowie `NetworkFlow` als native Netzwerkabstraktion kann NovaOS Firewallentscheidungen auf höherwertige Systeminformationen stützen.

---

## Entscheidung

NovaOS verwendet eine **Identity- und Capability-Aware Firewall** als native Netzwerksicherheitsarchitektur.

Firewallentscheidungen können neben klassischen Netzwerkmerkmalen insbesondere berücksichtigen:

- Network Identity,
- Execution Domain,
- Service Identity,
- Network Capabilities,
- Trust Context,
- `NetworkFlow`,
- Security Policy,
- Data-Sovereignty-Anforderungen.

```text
NetworkFlow
     ↓
Identity + Capability + Context
     ↓
Firewall Policy
     ↓
Allow / Deny
     ↓
Network Path
```

IP-Adresse und Port bleiben verfügbare Policy-Eigenschaften, sind jedoch nicht die alleinige Grundlage der Zugriffskontrolle.

---

## Grundprinzip

```text
Network reachability != communication authority
```

Die technische Erreichbarkeit eines Ziels erzeugt keine Berechtigung zur Kommunikation.

---

## Identity-Aware Filtering

Firewallregeln können sich auf stabile logische Identitäten beziehen.

Beispielsweise:

```text
Application Identity
Service Identity
Device Identity
Execution Domain Identity
Remote Trusted Identity
```

Dadurch müssen Sicherheitsregeln nicht unnötig an veränderliche IP-Adressen gebunden werden.

Die Firewall darf eine behauptete Identity nicht allein aufgrund einer Netzwerkadresse als authentisch behandeln.

---

## Capability-Aware Filtering

Eine Execution Domain benötigt für geschützte Netzwerkoperationen die entsprechende Capability beziehungsweise Authority.

Konzeptionell können Capabilities beispielsweise Kommunikation mit:

- bestimmten Services,
- bestimmten Netzwerkdomänen,
- bestimmten Protokollen,
- bestimmten Klassen externer Ziele

erlauben.

```text
Network Request
      ↓
Capability Check
      ↓
Firewall Policy
      ↓
Permitted NetworkFlow
```

Die genaue Capability-Struktur wird in den zugehörigen NPSPECs festgelegt.

---

## Least Privilege

Netzwerkzugriff wird nach dem Least-Privilege-Prinzip vergeben.

Eine Anwendung soll nicht automatisch uneingeschränkten Netzwerkzugriff erhalten, nur weil sie grundsätzlich Netzwerkkommunikation verwenden darf.

Capabilities und Firewall Policies können den zulässigen Kommunikationsraum begrenzen.

---

## Klassische Netzwerkregeln

Die Firewall unterstützt weiterhin klassische Kriterien wie:

- Source Address,
- Destination Address,
- Port,
- Transport Protocol,
- Interface,
- Route,
- Traffic Direction.

Diese dienen insbesondere Kompatibilität, Administration und Low-Level-Netzwerkschutz.

```text
Identity / Capability Policy
            +
Traditional Network Policy
            ↓
      Firewall Decision
```

Beide Ebenen ergänzen sich.

---

## NetworkFlow Integration

Firewallentscheidungen werden soweit möglich auf `NetworkFlow`-Ebene mit dem zugehörigen Security Context getroffen.

Dadurch können Regeln auf die logische Kommunikation statt ausschließlich auf einzelne Pakete angewendet werden.

Die paketbasierte Durchsetzung bleibt Bestandteil der Netzwerkimplementation.

---

## Nova.Trust

Remote Identities können mit `Nova.Trust` verbunden werden.

```text
Identity
   ↓
Signature / Authentication
   ↓
Provenance / Trust
   ↓
Capability
   ↓
Firewall Policy
```

Trust und Capability bleiben getrennt:

Eine vertrauenswürdige Identity besitzt nicht automatisch die Authority für jede Netzwerkkommunikation.

---

## Data Sovereignty

Data-Sovereignty-Regeln können Bestandteil der Firewallentscheidung sein.

Ein Flow kann beispielsweise verboten werden, wenn:

- das Ziel nicht zulässig ist,
- die Zielidentität nicht ausreichend vertrauenswürdig ist,
- der verwendete Netzwerkpfad gegen Sovereignty Constraints verstößt.

Die Firewall ergänzt damit Policy-Based Routing um eine explizite Durchsetzungsebene.

---

## Fail-Safe Defaults

Bei fehlender oder nicht eindeutig bestimmbarer Authority gilt für geschützte Netzwerkoperationen:

```text
Unknown Authority
      ↓
     Deny
```

Unbekannte Identity darf nicht automatisch als vertrauenswürdig behandelt werden.

Adaptive Systeme dürfen Firewallberechtigungen nicht eigenständig erweitern.

---

## Inbound und Outbound

Die Firewall gilt sowohl für eingehende als auch ausgehende Kommunikation.

```text
Inbound
Network → Firewall → Service

Outbound
Application → Firewall → Network
```

Damit können sowohl erreichbare Dienste als auch die Netzwerkfähigkeiten von Anwendungen kontrolliert werden.

---

## Introspection und Audit

Firewallentscheidungen müssen kontrolliert nachvollziehbar sein.

NovaOS soll insbesondere zuordnen können:

```text
NetworkFlow
    ↓
Identity
    ↓
Capability
    ↓
Applied Policy
    ↓
Decision
```

Relevante Sicherheitsentscheidungen können in das systemweite Audit- und Architecture-Introspection-Modell integriert werden.

Sicherheitskritische Informationen dürfen dabei nur autorisierten Komponenten offengelegt werden.

---

## Normative Anforderungen

1. NovaOS MUSS eine Identity- und Capability-Aware Firewall bereitstellen.
2. Firewallentscheidungen DÜRFEN NICHT ausschließlich von IP-Adressen und Ports abhängig sein müssen.
3. Die Firewall MUSS Network Identity und Execution Domain berücksichtigen können.
4. Die Firewall MUSS Network Capabilities beziehungsweise Communication Authority berücksichtigen können.
5. Die Kenntnis einer Adresse oder Identity DARF keine implizite Communication Authority erzeugen.
6. Eine Netzwerkadresse DARF NICHT allein als Nachweis einer vertrauenswürdigen Identity gelten.
7. Firewall Policies MÜSSEN klassische Adress-, Port-, Protokoll- und Interface-Regeln unterstützen können.
8. Identity- und klassische Netzwerkregeln MÜSSEN gemeinsam verwendbar sein.
9. Firewallentscheidungen SOLLEN mit `NetworkFlow` und dessen Security Context verknüpft sein.
10. Inbound- und Outbound-Kommunikation MÜSSEN kontrollierbar sein.
11. Data-Sovereignty- und Trust-Anforderungen MÜSSEN in Firewallentscheidungen einfließen können.
12. Fehlende oder ungültige erforderliche Authority MUSS zu einer sicheren Ablehnung führen.
13. Adaptive Optimierung DARF Firewallberechtigungen NICHT selbstständig erweitern.
14. Firewallentscheidungen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Sicherheitsregeln können Anwendungen und Services statt nur Adressen beschreiben,
- wechselnde IP-Adressen beeinträchtigen Identity-basierte Regeln weniger,
- Netzwerkzugriff folgt dem systemweiten Capability-Modell,
- Least Privilege kann auch für ausgehenden Netzwerkverkehr umgesetzt werden,
- Nova.Trust und Data Sovereignty lassen sich direkt integrieren.

### Negative Konsequenzen

- Firewallentscheidungen benötigen zusätzlichen Systemkontext,
- Identity und Capability müssen sicher bis zur Durchsetzungsebene propagiert werden,
- klassische Firewallwerkzeuge benötigen Projektionen auf das erweiterte Modell.

---

## Verworfene Alternativen

### Ausschließlich paketbasierte IP-/Port-Firewall

Verworfen.

Sie kann die logische Identity und Authority des verursachenden `NetworkFlow` nicht ausreichend abbilden.

### Netzwerkzugriff als globale Anwendungsberechtigung

Verworfen.

Eine einzige Berechtigung wie „darf Netzwerk verwenden“ ist für konsequentes Least Privilege zu grob.

### Trust automatisch als Communication Authority behandeln

Verworfen.

Vertrauen in eine Identity bedeutet nicht automatisch, dass jede Kommunikation mit dieser Identity zulässig ist.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0006_Identity_Address_und_Route_strikt_trennen`
- `ADR-NETWORK-0007_Multipath_Networking`
- `ADR-NETWORK-0008_Policy_Based_Routing`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-DRIVER-0004_Driver_Capability_Model`
- `ADR-IPC-0002_Capability_Based_IPC`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-FIREWALL-0001`
- `NPSPEC-NETWORK-FIREWALL-IDENTITY-0001`
- `NPSPEC-NETWORK-FIREWALL-CAPABILITY-0001`
- `NPSPEC-NETWORK-FIREWALL-POLICY-0001`
- `NPSPEC-NETWORK-FIREWALL-AUDIT-0001`
- `NPSPEC-NETWORK-FIREWALL-TEST-0001`

---

## Ergebnis

NovaOS verschiebt die Firewall von einer ausschließlich adressbasierten Paketfilterung zu einer systemweit integrierten Autorisierungsentscheidung:

```text
Who?
Identity
   +
May it?
Capability
   +
Where / How?
Address + Route
   ↓
Firewall Policy
   ↓
Allow / Deny
```

Damit kontrolliert NovaOS Netzwerkkommunikation nicht nur danach, **wohin Daten gesendet werden**, sondern auch danach, **wer kommuniziert und welche Authority dafür tatsächlich vorhanden ist**.