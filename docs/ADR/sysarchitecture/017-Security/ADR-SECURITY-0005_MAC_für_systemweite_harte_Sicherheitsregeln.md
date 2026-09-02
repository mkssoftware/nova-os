# ADR-SECURITY-0005 – MAC für systemweite harte Sicherheitsregeln

## Status

Angenommen

## Kategorie

Security / Authorization / Mandatory Access Control / Policy

## Kontext

Capability-Based Security bildet das zentrale Autorisierungsmodell von NovaOS. DAC wird für klassische Berechtigungs- und Kompatibilitätsszenarien unterstützt.

Bestimmte Sicherheitsregeln dürfen jedoch weder durch den Besitzer einer Ressource noch durch delegierte Capabilities oder administrative Anwendungslogik frei verändert oder umgangen werden.

Dazu gehören insbesondere systemweite Vorgaben zu:

- Systemintegrität,
- Isolation,
- Schutz kritischer Systemressourcen,
- Datenklassifikation,
- Information Flow,
- Data Sovereignty,
- besonders geschützten Execution Domains.

NovaOS benötigt deshalb eine Mandatory-Access-Control-Ebene für verbindliche systemweite Sicherheitsregeln.

---

## Entscheidung

NovaOS unterstützt **Mandatory Access Control (MAC)** für systemweite harte Sicherheitsregeln.

MAC ist dabei kein konkurrierendes allgemeines Autorisierungsmodell, sondern eine zusätzliche verbindliche Policy-Ebene über der capability-basierten Authority.

```text
Capability Authority
        +
MAC Policy
        ↓
Effective Authorization
```

Eine Capability bedeutet:

```text
Subject has authority to request operation
```

MAC bestimmt zusätzlich:

```text
System policy permits operation
```

Beide Bedingungen müssen erfüllt sein.

---

## Grundprinzip

```text
Capability grants authority.
MAC defines boundaries authority cannot cross.
```

---

## Verhältnis zu Capabilities

Capabilities bleiben das primäre Mittel zur Vergabe konkreter Authority.

MAC kann diese Authority weiter einschränken, aber nicht erweitern.

```text
Capability allows
MAC allows
    → Allowed

Capability allows
MAC denies
    → Denied

Capability missing
MAC allows
    → Denied
```

Damit kann MAC niemals eine fehlende Capability ersetzen.

---

## Verhältnis zu DAC

DAC beschreibt durch Eigentümer oder kompatible Berechtigungsmodelle kontrollierbare Zugriffsregeln.

MAC beschreibt dagegen systemweit verbindliche Regeln.

```text
DAC
    → discretionary policy

MAC
    → mandatory policy
```

Eine erfolgreiche DAC-Prüfung darf eine MAC-Regel nicht überschreiben.

---

## MAC-Policies

MAC-Policies können unter anderem auf folgenden Eigenschaften basieren:

- Subject Identity,
- Execution Domain,
- Capability Type,
- Object Type,
- Semantic Type,
- Data Classification,
- Trust Domain,
- Security Label,
- Information-Flow-Klasse,
- Systemzustand.

Die konkrete Policy-Sprache, Label-Struktur und Evaluierungslogik werden in NPSPECs definiert.

---

## Systemintegrität

MAC kann besonders kritische Systemressourcen zusätzlich schützen.

Beispiele:

```text
Kernel Components
Boot Configuration
Trust Anchors
Security Policies
Global /System
Audit Infrastructure
```

Selbst eine grundsätzlich passende Capability darf nur verwendet werden, wenn die verbindliche MAC-Policy die Operation erlaubt.

---

## Information Flow und Data Sovereignty

MAC kann zur Durchsetzung harter Information-Flow- und Data-Sovereignty-Regeln verwendet werden.

Beispiel:

```text
Protected Data
      ↓
Capability allows transfer
      ↓
MAC / Sovereignty Policy
      ↓
Destination permitted?
```

Damit können systemweite Datenflussgrenzen unabhängig von der einzelnen Anwendung durchgesetzt werden.

---

## Execution Domains

MAC-Regeln können an Execution Domains gebunden werden.

Beispielsweise können:

```text
System Domain
Driver Domain
Application Domain
Compatibility Domain
Recovery Domain
```

unterschiedliche verbindliche Sicherheitsgrenzen besitzen.

Eine Capability darf diese Domain-Grenzen nicht automatisch aufheben.

---

## Policy Administration

MAC-Policies dürfen nicht von beliebigen Ressourceneigentümern oder Anwendungen verändert werden.

Änderungen benötigen explizite Security-Policy-Authority.

```text
Modify MAC Policy
        ↓
Dedicated Capability
        ↓
Policy Validation
        ↓
Transactional Update
```

Auch Security-Administratoren besitzen keine implizite universelle Umgehungsmöglichkeit.

---

## Fail-Safe Verhalten

Kann eine erforderliche MAC-Policy nicht zuverlässig bestimmt oder ausgewertet werden, gilt:

```text
Unknown
   ≠
Allowed
```

Für harte Sicherheitsregeln muss NovaOS fail-safe reagieren.

Fehler in einer Optimierungs-, Verwaltungs- oder Adaptionskomponente dürfen nicht automatisch zu einer Lockerung der MAC-Regeln führen.

---

## Adaptive Systeme

Adaptive Systeme dürfen MAC-Entscheidungen beobachten oder Optimierungsvorschläge erzeugen.

Sie dürfen harte MAC-Policies jedoch nicht selbständig abschwächen.

```text
Adaptive Recommendation
        ≠
Security Authority
```

Änderungen harter Policies benötigen den normalen autorisierten Policy-Prozess.

---

## Legacy Compatibility

Legacy-Systeme können eigene MAC-Modelle besitzen.

Diese dürfen innerhalb geeigneter Compatibility Domains emuliert oder auf NovaOS-Policies abgebildet werden.

Sie ersetzen jedoch nicht die systemweiten nativen NovaOS-MAC-Regeln.

---

## Audit und Introspection

MAC-Entscheidungen müssen kontrolliert nachvollziehbar sein.

Beispielsweise:

```text
Subject
 ├── Requested Operation
 ├── Target
 ├── Capability
 ├── Applicable MAC Policy
 └── Decision
```

NovaOS soll insbesondere unterscheiden können, ob eine Operation wegen fehlender Capability oder aufgrund einer MAC-Regel abgelehnt wurde.

---

## Normative Anforderungen

1. NovaOS MUSS MAC für systemweite harte Sicherheitsregeln unterstützen.
2. Capability-Based Security MUSS das primäre native Authority-Modell bleiben.
3. MAC DARF Authority nur einschränken und NICHT eigenständig erweitern.
4. Eine MAC-Erlaubnis DARF eine fehlende Capability NICHT ersetzen.
5. DAC-Regeln DÜRFEN MAC-Regeln NICHT überschreiben.
6. MAC MUSS auf systemkritische Ressourcen und Execution Domains anwendbar sein.
7. MAC MUSS mit Data Classification und Information-Flow-Policies integrierbar sein.
8. MAC MUSS mit Data-Sovereignty-Regeln integrierbar sein.
9. Capability Delegation DARF bestehende MAC-Grenzen NICHT abschwächen.
10. Änderungen an MAC-Policies MÜSSEN explizite Security-Policy-Authority erfordern.
11. MAC-Policies SOLLEN transaktional und validiert aktualisierbar sein.
12. Native Komponenten DÜRFEN keine allgemeine MAC-Bypass-Authority besitzen.
13. Unbekannte oder nicht auswertbare harte MAC-Regeln MÜSSEN fail-safe behandelt werden.
14. Adaptive Systeme DÜRFEN harte MAC-Policies NICHT selbständig abschwächen.
15. Legacy-MAC-Modelle DÜRFEN innerhalb kontrollierter Compatibility Domains unterstützt werden.
16. MAC-Entscheidungen MÜSSEN kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- harte Sicherheitsgrenzen bleiben unabhängig von Ressourceneigentümern bestehen,
- Capabilities können systemweit durch verbindliche Policies begrenzt werden,
- starke Grundlage für Information Flow und Data Sovereignty,
- zusätzlicher Schutz kritischer Systemressourcen,
- klare Trennung zwischen Authority und systemweiter Sicherheitsgrenze.

### Negative Konsequenzen

- Autorisierungsentscheidungen benötigen eine zusätzliche Policy-Ebene,
- fehlerhafte MAC-Policies können legitime Systemoperationen blockieren,
- Policy-Design und Debugging werden komplexer.

---

## Verworfene Alternativen

### Nur Capability-Based Security

Verworfen als alleinige Policy-Ebene.

Capabilities modellieren Authority sehr präzise, ersetzen jedoch nicht alle systemweiten verbindlichen Sicherheitsregeln.

### MAC als primäres Autorisierungsmodell

Verworfen.

MAC eignet sich für harte Systemgrenzen, soll aber nicht die flexible capability-basierte Authority ersetzen.

### DAC für systemweite Sicherheitsregeln

Verworfen.

Discretionary Policies können durch Ressourceneigentümer verändert werden und eignen sich daher nicht für unverhandelbare Systemgrenzen.

### Universeller MAC-Bypass für Administratoren

Verworfen.

Dies würde erneut ein Root-ähnliches universelles Privileg erzeugen.

---

## Abhängigkeiten

- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0002_Kein_universelles_Root_Privileg`
- `ADR-SECURITY-0003_Least_Privilege_und_Privilege_Separation`
- `ADR-SECURITY-0004_DAC_als_Kompatibilitätsmodell`
- `ADR-NETWORK-0021_Data_Sovereignty_Aware_Routing`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-MAC-0001`
- `NPSPEC-SECURITY-MAC-POLICY-0001`
- `NPSPEC-SECURITY-MAC-LABEL-0001`
- `NPSPEC-SECURITY-MAC-ENFORCEMENT-0001`
- `NPSPEC-SECURITY-MAC-TEST-0001`

---

## Ergebnis

NovaOS kombiniert flexible capability-basierte Authority mit verbindlichen systemweiten Sicherheitsgrenzen:

```text
Requested Operation
        ↓
Capability Check
        ↓
DAC / Compatibility Policy
        ↓
MAC Hard Policy
        ↓
Other Hard Constraints
        ↓
Allowed / Denied
```

Capabilities bestimmen, **welche Authority eine Komponente besitzt**.

MAC bestimmt, **welche systemweiten Grenzen diese Authority niemals überschreiten darf**.

Damit ergänzt MAC das Capability-Modell gezielt dort, wo NovaOS unverhandelbare Sicherheitsregeln systemweit durchsetzen muss.