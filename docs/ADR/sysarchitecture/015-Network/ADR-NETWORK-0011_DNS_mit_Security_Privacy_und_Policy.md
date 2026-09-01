# ADR-NETWORK-0011 – DNS mit Security, Privacy und Policy

## Status

Angenommen

## Kategorie

Network / DNS / Name Resolution / Security / Privacy / Policy

## Kontext

DNS bleibt für die Interoperabilität mit bestehenden Internet- und Netzwerkdiensten eine zentrale Infrastruktur zur Auflösung von Namen in Netzwerkadressen und weitere DNS-Daten.

NovaOS trennt jedoch gemäß `ADR-NETWORK-0006` Identity, Address und Route. Ein DNS-Name ist deshalb weder automatisch eine vertrauenswürdige Network Identity noch eine Communication Authority.

Gleichzeitig kann DNS sensible Informationen über verwendete Dienste und Kommunikationsziele offenlegen. Die Namensauflösung muss daher in die Security-, Privacy-, Data-Sovereignty- und Policy-Architektur von NovaOS integriert werden.

---

## Entscheidung

NovaOS behandelt DNS als **policygesteuerten, sicherheits- und datenschutzbewussten Resolution Provider** innerhalb der Netzwerkarchitektur.

```text
Name / Service
      ↓
Resolution Policy
      ↓
DNS Provider
      ↓
Validated Result
      ↓
Addresses / Service Data
      ↓
NetworkFlow
```

DNS bleibt vollständig interoperabel mit bestehenden DNS-Infrastrukturen, wird jedoch nicht als ungeprüfte globale Namensquelle behandelt.

---

## Grundprinzip

```text
DNS resolves names.

It does not establish
identity, trust or authority.
```

---

## DNS und Network Identity

DNS-Namen und Network Identities bleiben getrennte Konzepte.

```text
DNS Name
   ↓
Resolution
   ↓
Address

≠

Trusted Network Identity
```

Ein DNS-Ergebnis darf daher nicht allein als Beweis für die Identität oder Vertrauenswürdigkeit eines Kommunikationspartners gelten.

Authentisierung und Trust werden über die dafür vorgesehenen Security- und Nova.Trust-Mechanismen hergestellt.

---

## Security

NovaOS unterstützt die Validierung authentisierbarer DNS-Daten, sofern die verwendete DNS-Infrastruktur dies ermöglicht.

Manipulierte, ungültige oder nicht ausreichend vertrauenswürdige Resolution-Ergebnisse müssen entsprechend der geltenden Security Policy behandelt werden.

Dabei gilt:

```text
Resolution Success
        ≠
Security Authorization
```

Auch ein erfolgreich validiertes DNS-Ergebnis erzeugt keine Communication Authority.

---

## Privacy

DNS-Auflösungen können Rückschlüsse auf Anwendungen, Dienste und Nutzeraktivitäten ermöglichen.

NovaOS behandelt DNS-Privacy deshalb als explizite Systemeigenschaft.

Abhängig von Policy und verfügbarer Infrastruktur können verschlüsselte oder anderweitig geschützte Resolution Provider verwendet werden.

Die konkrete Unterstützung beispielsweise für verschlüsselte DNS-Transporte wird in NPSPECs festgelegt.

---

## Resolution Policy

Die Auswahl des DNS Providers erfolgt policybasiert.

```text
Resolution Request
       ↓
Hard Constraints
       ↓
Eligible DNS Providers
       ↓
Privacy / Performance Policy
       ↓
Selected Provider
```

Hard Constraints können insbesondere Security-, Trust- und Data-Sovereignty-Anforderungen enthalten.

Soft Policies können beispielsweise Latenz, Verfügbarkeit oder Cache-Nutzung optimieren.

---

## Split DNS und Resolution Domains

NovaOS unterstützt unterschiedliche Resolution-Kontexte.

Beispielsweise können:

- öffentliche Namen,
- Unternehmensdienste,
- lokale Dienste,
- VPN-Domänen,
- isolierte Execution Domains

unterschiedliche Resolution Provider benötigen.

```text
Resolution Context
 ├── Public DNS
 ├── Enterprise DNS
 ├── VPN DNS
 └── Local Resolution
```

Ein globaler DNS Provider darf nicht automatisch für jeden Resolution Context verwendet werden.

---

## Data Sovereignty

DNS-Anfragen unterliegen Data-Sovereignty-Policies.

Eine Resolution darf nicht über einen Provider oder Netzwerkpfad erfolgen, der für den betreffenden Kontext unzulässig ist.

```text
DNS Request
     ↓
Sovereignty Policy
     ↓
Permitted Providers
     ↓
Resolution
```

Damit wird verhindert, dass bereits die Namensauflösung Daten über unzulässige Netzwerkgrenzen überträgt.

---

## DNS Caching

NovaOS darf DNS-Ergebnisse entsprechend ihrer gültigen Lebensdauer cachen.

Caching ist Bestandteil der Resolution-Architektur und muss Security-, Privacy- und Isolationseigenschaften des jeweiligen Resolution Context erhalten.

Caches unterschiedlicher Trust- oder Isolation Domains dürfen nicht unkontrolliert miteinander vermischt werden.

Details zu TTL, Invalidierung und Cache-Strukturen werden in NPSPECs definiert.

---

## NetworkFlow Integration

DNS Resolution kann Bestandteil der Vorbereitung eines `NetworkFlow` sein.

```text
Service / Name
      ↓
DNS Resolution
      ↓
Eligible Addresses
      ↓
Policy-Based Routing
      ↓
NetworkFlow Path
```

Das DNS-Ergebnis bestimmt dabei mögliche Adressen, nicht automatisch die endgültige Route oder den Transportprovider.

Damit bleibt die Trennung von Identity, Address und Route erhalten.

---

## Fail-Safe Defaults

Kann eine für den jeweiligen Contract erforderliche Security-, Privacy- oder Sovereignty-Eigenschaft nicht erfüllt werden, darf NovaOS nicht stillschweigend auf einen schwächeren DNS Provider zurückfallen.

```text
Required Secure Resolution
          ↓
No Eligible Provider
          ↓
Explicit Failure
```

Ein unsicherer Fallback ist nur zulässig, wenn der geltende Contract beziehungsweise die Policy dies ausdrücklich erlaubt.

---

## Introspection

DNS Resolution muss kontrolliert introspektierbar sein.

NovaOS soll für autorisierte Diagnose insbesondere nachvollziehen können:

```text
Resolution Request
       ↓
Resolution Context
       ↓
Selected Provider
       ↓
Security / Privacy Mode
       ↓
Result
```

Sensible DNS-Historien oder Nutzeraktivitäten dürfen dabei nicht unnötig offengelegt werden.

---

## Normative Anforderungen

1. NovaOS MUSS DNS als nativen Resolution Provider unterstützen.
2. DNS-Namen DÜRFEN NICHT mit vertrauenswürdiger Network Identity gleichgesetzt werden.
3. Ein erfolgreiches DNS-Ergebnis DARF keine Communication Authority erzeugen.
4. DNS Resolution MUSS in die Security- und Policy-Architektur integriert sein.
5. NovaOS MUSS authentisierbare DNS-Daten validieren können, sofern dies vom verwendeten Verfahren unterstützt wird.
6. DNS-Privacy MUSS als explizite Systemeigenschaft behandelt werden.
7. DNS Provider MÜSSEN policybasiert auswählbar sein.
8. Hard Constraints MÜSSEN vor Performance- oder anderen Optimierungszielen ausgewertet werden.
9. NovaOS MUSS unterschiedliche Resolution Contexts unterstützen können.
10. Split-DNS-Szenarien MÜSSEN ohne globale Umleitung sämtlicher DNS-Anfragen möglich sein.
11. Data-Sovereignty-Policies MÜSSEN die Auswahl von DNS Provider und Netzwerkpfad begrenzen können.
12. DNS-Caches MÜSSEN die relevanten Trust-, Security- und Isolation Domains berücksichtigen.
13. Ein erforderliches Security- oder Privacy-Niveau DARF NICHT stillschweigend unterschritten werden.
14. DNS-Ergebnisse MÜSSEN als Address- beziehungsweise Service-Resolution und nicht als Routingentscheidung behandelt werden.
15. DNS-Provider-Auswahl und Resolution-Ergebnis MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- DNS bleibt vollständig mit bestehenden Netzwerken interoperabel,
- Security und Privacy werden systemweit berücksichtigt,
- Split DNS und VPN-/Unternehmensszenarien lassen sich sauber modellieren,
- Data Sovereignty gilt bereits für Namensauflösung,
- DNS wird nicht fälschlich mit Identity oder Trust gleichgesetzt.

### Negative Konsequenzen

- Resolution benötigt zusätzlichen Policy-Kontext,
- mehrere DNS Provider und Resolution Domains erhöhen die Komplexität,
- sichere und private Resolution kann abhängig von der Infrastruktur zusätzliche Kosten verursachen.

---

## Verworfene Alternativen

### Ein globaler DNS Resolver für das gesamte System

Verworfen.

Unterschiedliche Execution-, Trust-, VPN- und Sovereignty-Kontexte können unterschiedliche Resolver benötigen.

### DNS-Name als Network Identity

Verworfen.

DNS stellt Namensauflösung bereit, aber keine allgemeine NovaOS-Identity- oder Authority-Garantie.

### Automatischer Fallback auf unverschlüsseltes DNS

Verworfen.

Ein solcher Fallback könnte explizite Security- oder Privacy-Anforderungen unbemerkt verletzen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0006_Identity_Address_und_Route_strikt_trennen`
- `ADR-NETWORK-0008_Policy_Based_Routing`
- `ADR-NETWORK-0009_Identity_und_Capability_Aware_Firewall`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-DNS-0001`
- `NPSPEC-NETWORK-DNS-SECURITY-0001`
- `NPSPEC-NETWORK-DNS-PRIVACY-0001`
- `NPSPEC-NETWORK-DNS-POLICY-0001`
- `NPSPEC-NETWORK-DNS-CACHE-0001`
- `NPSPEC-NETWORK-DNS-RESOLUTION-CONTEXT-0001`
- `NPSPEC-NETWORK-DNS-TEST-0001`

---

## Ergebnis

DNS wird in NovaOS als kontrollierte Resolution-Funktion und nicht als Identitäts- oder Vertrauenssystem behandelt:

```text
Name
 ↓
Security / Privacy / Sovereignty Policy
 ↓
DNS Resolution
 ↓
Address Candidates
 ↓
Routing Policy
 ↓
NetworkFlow
```

Damit bleibt NovaOS mit bestehender DNS-Infrastruktur kompatibel, während Namensauflösung gleichzeitig in Security, Privacy, Data Sovereignty und das native Netzwerkmodell integriert wird.