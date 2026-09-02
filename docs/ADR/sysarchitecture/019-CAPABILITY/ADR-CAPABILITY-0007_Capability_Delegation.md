# ADR-CAPABILITY-0007 – Capability Delegation

## Status

Angenommen

## Kategorie

Capability Architecture / Delegation / Authority / Composition

## Kontext

NovaOS-Capabilities können von Anwendungen, Systemdiensten, Pipelines, Plugins, Treibern und KI-Agenten genutzt werden.

Bei zusammengesetzten Operationen muss ein Consumer häufig einen Teil seiner vorhandenen Authority an eine andere Komponente weitergeben.

Beispiel:

```text
Application
    ↓
Image Processing Service
    ↓
Storage Service
```

Der Image Processing Service benötigt möglicherweise Zugriff auf genau ein bestimmtes Bild, soll aber nicht automatisch sämtliche Storage-Rechte der Anwendung erhalten.

NovaOS benötigt deshalb explizite und begrenzbare Capability Delegation.

---

## Entscheidung

NovaOS unterstützt **Capability Delegation als explizite Übertragung begrenzter Authority zwischen Systemkomponenten**.

Für jede Delegation gilt:

```text
Authority(Delegated)
    ⊆
Authority(Source)
```

Eine Delegation darf vorhandene Authority erhalten oder reduzieren, niemals erweitern.

---

## Grundprinzip

```text
Delegate only the authority
required for the delegated operation.
```

Delegation ist kein implizites Weiterreichen des vollständigen Security Context.

---

## Delegation und funktionale Capabilities

Funktionale Capability und Security Authority bleiben getrennte Konzepte.

```text
Functional Capability
→ Was soll ausgeführt werden?

Delegated Authority
→ Was darf dabei im Namen des Callers getan werden?
```

Ein Service kann eine Capability implementieren, ohne automatisch Authority auf die Daten oder Ressourcen des Aufrufers zu besitzen.

---

## Explizite Delegation

Authority wird nicht automatisch über Capability-Aufrufe hinweg weitergegeben.

```text
Caller
 ├── Capability A
 ├── Capability B
 └── Capability C

Delegation
      ↓

Callee
 └── Capability B
```

Der Callee erhält nur die explizit delegierte Authority.

---

## Attenuation

Delegation soll mit Capability Attenuation kombiniert werden können.

Beispiel:

```text
Source:
Storage.ReadWrite(/Documents/*)

        ↓ Attenuation

Delegated:
Storage.Read(/Documents/Image.nova)
```

Mögliche Einschränkungen umfassen:

- Operationen,
- Zielobjekte,
- Lebensdauer,
- Resource Budgets,
- Call Context,
- Data Sovereignty,
- Trust Requirements,
- Delegationstiefe.

---

## Delegierbarkeit

Nicht jede Capability muss delegierbar sein.

Capabilities können konzeptionell als:

```text
Non-Delegatable
Restricted Delegation
Delegatable
```

klassifiziert werden.

Die Authority zur Nutzung einer Capability impliziert nicht automatisch die Authority, sie weiterzugeben.

---

## Delegation Chains

Delegation kann über mehrere Komponenten erfolgen.

```text
A
↓
B
↓
C
↓
D
```

Dabei muss die Authority monoton begrenzt bleiben:

```text
Authority(D)
⊆ Authority(C)
⊆ Authority(B)
⊆ Authority(A)
```

Keine Stufe darf entfernte Einschränkungen wieder hinzufügen oder erweitern.

---

## Capability Composition

Capability Composition kann Delegation benötigen.

```text
Composite Capability
        ↓
Capability A
        ↓ delegated authority
Capability B
        ↓ delegated authority
Capability C
```

Jede Teil-Capability erhält nur die für ihre konkrete Aufgabe notwendige Authority.

Damit darf eine Composition nicht automatisch die Authorities aller beteiligten Komponenten vereinigen.

---

## Confused-Deputy-Schutz

Services müssen zwischen eigener Authority und delegierter Caller Authority unterscheiden können.

```text
Service
 ├── Own Authority
 └── Delegated Caller Authority
```

Eine Operation im Auftrag eines Callers darf nicht unbeabsichtigt mit umfassenderen eigenen Rechten des Services ausgeführt werden.

---

## Lebensdauer

Delegierte Authority muss an einen geeigneten Lifecycle gebunden werden können.

Beispiele:

```text
Nova.Call
Task Group
Session
Transaction
Pipeline
Execution Domain
```

Nach Ablauf des Delegationskontexts soll die delegierte Authority nicht weiter verwendbar sein.

Leases, Epochs und Revocation können dafür verwendet werden.

---

## Lokale und entfernte Delegation

Dasselbe grundlegende Delegationsmodell gilt für:

```text
Local Call
IPC
Nova.Call
Remote Call
```

Bei Netzwerkgrenzen kommen zusätzliche Anforderungen für Remote Capabilities, Identität, Trust und kryptographische Validierung hinzu.

Die semantische Authority darf sich durch einen Wechsel des Transportwegs nicht verändern.

---

## Data Sovereignty und Information Flow

Delegation kann bestehende Hard Constraints nicht aufheben.

```text
Delegated Capability
        +
Information Flow Policy
        +
Data Sovereignty
        ↓
Effective Authority
```

Eine delegierte Capability darf keine Datenbewegung ermöglichen, die dem ursprünglichen Sicherheits- oder Sovereignty-Kontext widerspricht.

---

## Revocation

Delegierte Capabilities müssen mit dem Revocation-Modell integriert sein.

Wird die zugrunde liegende Authority widerrufen, dürfen daraus abgeleitete Delegationen den Widerruf nicht umgehen.

```text
Source Revoked
      ↓
Dependent Delegations
      ↓
Invalid
```

Die konkreten Revocation-Abhängigkeiten werden in den Security-NPSPECs definiert.

---

## Introspection und Audit

Delegationen müssen nachvollziehbar sein.

NovaOS soll kontrolliert darstellen können:

```text
Source Authority
Delegator
Delegate
Attenuation
Delegation Context
Lifetime
Delegation Chain
Effective Authority
Revocation State
```

Geheimes Capability-Material darf dabei nicht offengelegt werden.

---

## Normative Anforderungen

1. NovaOS MUSS explizite Capability Delegation unterstützen.
2. Delegierte Authority DARF die Authority der Quelle NICHT überschreiten.
3. Capability-Nutzung DARF NICHT automatisch Delegations-Authority implizieren.
4. Delegation MUSS mit Capability Attenuation kombinierbar sein.
5. Capabilities MÜSSEN als nicht, eingeschränkt oder vollständig delegierbar definierbar sein.
6. Delegation Chains MÜSSEN monoton nicht-erweiternd sein.
7. Delegierte Authority MUSS an einen definierten Lifecycle gebunden werden können.
8. Capability Composition DARF Authority NICHT automatisch vereinigen.
9. Services MÜSSEN eigene und delegierte Caller Authority unterscheiden können.
10. Delegation MUSS mit Revocation, Leases und Epochs integrierbar sein.
11. Lokale und entfernte Delegation SOLLEN dieselben grundlegenden Authority-Semantiken verwenden.
12. Delegation DARF Security-, Trust-, Information-Flow- oder Data-Sovereignty-Constraints NICHT abschwächen.
13. Resource Budgets DÜRFEN durch Delegation NICHT unkontrolliert vervielfacht werden.
14. Delegationsketten und effektive Authority MÜSSEN kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Least Privilege über Capability-Grenzen hinweg,
- sichere Capability Composition,
- Schutz vor Confused-Deputy-Problemen,
- kontrollierte Delegation an Plugins, Dienste und KI-Agenten,
- einheitliches Modell für lokale und verteilte Ausführung.

### Negative Konsequenzen

- Delegationsketten erhöhen die Authority-Management-Komplexität,
- Lifecycle und Revocation müssen über abgeleitete Capabilities verfolgt werden,
- Services müssen eigene und delegierte Authority sauber auseinanderhalten.

---

## Verworfene Alternativen

### Vollständigen Security Context automatisch weiterreichen

Verworfen.

Dies würde Least Privilege verletzen und unnötige Ambient Authority erzeugen.

### Delegierte Capability darf Authority erweitern

Verworfen.

Delegation darf keine neue Authority erzeugen.

### Jede Capability ist automatisch delegierbar

Verworfen.

Nutzung und Weitergabe von Authority sind unterschiedliche Berechtigungen.

### Composition vereinigt automatisch alle Authorities

Verworfen.

Jede Teiloperation soll nur die tatsächlich benötigte Authority erhalten.

---

## Abhängigkeiten

- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-CAPABILITY-0005_Capability_Composition`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0003_Least_Privilege_und_Privilege_Separation`
- `ADR-SECURITY-0008_Capability_Attenuation`
- `ADR-SECURITY-0009_Capability_Delegation`
- `ADR-SECURITY-0010_Capability_Revocation`
- `ADR-SECURITY-0011_Capability_Leases_und_Epochs`
- `ADR-DISTCOMM-0004_Capability_Delegation_über_Netzwerkgrenzen`

---

## Zugehörige NPSPECs

- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-CAPABILITY-DELEGATION-CONSTRAINTS-0001`
- `NPSPEC-CAPABILITY-DELEGATION-CHAIN-0001`
- `NPSPEC-CAPABILITY-DELEGATION-LIFECYCLE-0001`
- `NPSPEC-CAPABILITY-DELEGATION-AUDIT-0001`
- `NPSPEC-CAPABILITY-DELEGATION-TEST-0001`

---

## Ergebnis

NovaOS erlaubt Capabilities, Authority kontrolliert an andere Komponenten weiterzugeben:

```text
Source Authority
       ↓
Attenuation
       ↓
Explicit Delegation
       ↓
Scoped Delegated Authority
       ↓
Capability Execution
```

Dabei gilt systemweit:

```text
Authority(Delegated)
    ⊆
Authority(Source)
```

Capability Delegation ermöglicht damit komplexe Capability-Kompositionen und verteilte Ausführung, ohne Least Privilege durch implizite oder unbegrenzte Rechteweitergabe zu verletzen.