# ADR-SECURITY-0004 – DAC als Kompatibilitätsmodell

## Status

Angenommen

## Kategorie

Security / Authorization / Compatibility / DAC

## Kontext

Discretionary Access Control (DAC) ist ein etabliertes Autorisierungsmodell klassischer Betriebssysteme. Ressourcen besitzen Eigentümer, und Zugriffe werden typischerweise über Benutzer, Gruppen, ACLs oder vergleichbare Berechtigungen gesteuert.

Beispiele sind:

```text
Owner
Group
Read
Write
Execute
ACL
```

Viele bestehende Anwendungen, Dateisysteme und Kompatibilitätsumgebungen erwarten diese Semantik.

NovaOS verwendet jedoch Capability-Based Security als natives Autorisierungsmodell. DAC soll deshalb unterstützt werden, ohne eine zweite gleichrangige Sicherheitsarchitektur zu schaffen.

---

## Entscheidung

NovaOS unterstützt **DAC als Kompatibilitätsmodell**, nicht als primäres natives Autorisierungsmodell.

```text
Legacy DAC
    ↓
Compatibility / Policy Mapping
    ↓
NovaOS Authority
    ↓
Capability Enforcement
```

DAC-Regeln können verwendet werden, um festzustellen, welche Authority einem Legacy- oder Compatibility-Kontext bereitgestellt wird.

Die letztliche native Durchsetzung geschützter Operationen bleibt capability-basiert.

---

## Grundprinzip

```text
DAC is compatibility policy.
Capabilities are native authority.
```

---

## DAC-Modell

NovaOS kann klassische DAC-Konzepte darstellen, insbesondere:

- Benutzer,
- Gruppen,
- Eigentümer,
- Read/Write/Execute-Rechte,
- Access Control Lists,
- vererbte Berechtigungen.

Diese Semantik wird dort bereitgestellt, wo Anwendungen, Protokolle oder Dateisysteme sie benötigen.

Native NovaOS-Komponenten sollen nicht von DAC als grundlegender Authority-Struktur abhängig sein.

---

## Mapping auf Capabilities

Eine erfolgreiche DAC-Prüfung kann zur Bereitstellung einer entsprechend begrenzten Capability führen.

Beispiel:

```text
User A
  ↓
DAC: Read allowed
  ↓
Read Capability
  ↓
Object X
```

Eine DAC-Regel erzeugt dabei keine universelle Authority, sondern nur die für den konkreten Zugriff erforderliche Berechtigung.

---

## DAC und Dateisysteme

DAC ist insbesondere für klassische Dateisystemsemantik relevant.

NovaOS muss dadurch Berechtigungsmodelle bestehender Dateisysteme und Anwendungen abbilden können, ohne das native Storage- und Capability-Modell darauf zu reduzieren.

```text
Legacy File Permission
        ↓
VFS / Compatibility Layer
        ↓
Capability-authorized Storage Operation
```

---

## Keine Umgehung nativer Policies

Eine erfolgreiche DAC-Prüfung reicht nicht aus, wenn andere verbindliche NovaOS-Policies die Operation verbieten.

Konzeptionell:

```text
DAC Permission
      +
Capability Authority
      +
Security Constraints
      +
Data Sovereignty
      =
Allowed Operation
```

DAC darf insbesondere keine:

- Capability-Grenzen,
- Isolation Domains,
- Trust Requirements,
- Information-Flow-Regeln,
- Data-Sovereignty-Policies

umgehen.

---

## Eigentümer ist kein universeller Administrator

Das DAC-Konzept des Resource Owners darf nicht mit universeller Authority gleichgesetzt werden.

Ein Eigentümer kann gemäß Policy bestimmte Berechtigungen verwalten, erhält dadurch aber keine allgemeine Systemauthority.

```text
Own Object X
    ≠
Administrative Authority over System
```

Dies entspricht dem Verzicht auf ein universelles Root-Privileg.

---

## Compatibility Domains

Legacy-Anwendungen können innerhalb einer Compatibility Domain eine weitgehend klassische DAC-Sicht erhalten.

```text
Legacy Application
       ↓
Compatibility Domain
       ↓
DAC Semantics
       ↓
Capability Mapping
       ↓
Native NovaOS Resources
```

Die Compatibility Domain bildet dabei eine Sicherheitsgrenze zwischen Legacy-Semantik und nativer NovaOS-Authority.

---

## DAC und Least Privilege

Aus DAC-Regeln abgeleitete Authority muss dem Least-Privilege-Prinzip folgen.

Beispielsweise soll:

```text
Read access to File X
```

nicht zu:

```text
Read access to entire Volume
```

werden.

Das Mapping muss die ursprüngliche Berechtigungsgrenze möglichst präzise erhalten.

---

## Native APIs

Native NovaOS-APIs dürfen nicht voraussetzen, dass Benutzer-ID, Gruppen-ID oder Eigentümerschaft die primäre Authority darstellen.

Native APIs verwenden Capabilities.

DAC-Informationen können als Policy- oder Compatibility-Kontext berücksichtigt werden.

---

## Audit und Introspection

Bei DAC-basierten Compatibility-Zugriffen soll nachvollziehbar sein, wie die resultierende Authority entstanden ist.

Beispiel:

```text
Legacy User
    ↓
DAC Rule
    ↓
Mapped Capability
    ↓
Authorized Operation
```

Damit bleibt auch bei Legacy-Zugriffen die tatsächliche Authority introspektierbar.

---

## Normative Anforderungen

1. NovaOS MUSS DAC als Kompatibilitätsmodell unterstützen können.
2. DAC DARF NICHT das primäre native Autorisierungsmodell von NovaOS sein.
3. Native geschützte Operationen MÜSSEN weiterhin capability-basiert autorisierbar sein.
4. Benutzer-, Gruppen-, Eigentümer- und ACL-Semantik MÜSSEN für geeignete Compatibility-Szenarien abbildbar sein.
5. DAC-Berechtigungen MÜSSEN auf begrenzte native Authority abbildbar sein.
6. DAC DARF Capability-, Trust-, Isolation-, Sovereignty- oder Information-Flow-Grenzen NICHT umgehen.
7. Eigentümerschaft DARF KEINE universelle Systemauthority erzeugen.
8. Legacy-DAC-Semantik SOLL innerhalb kontrollierter Compatibility Domains ausführbar sein.
9. Aus DAC abgeleitete Authority MUSS dem Least-Privilege-Prinzip folgen.
10. Native NovaOS-APIs SOLLEN nicht von DAC-Identitäten als primärer Authority abhängig sein.
11. DAC-Mappings MÜSSEN kontrolliert auditierbar und introspektierbar sein.
12. Nicht eindeutig autorisierbare DAC-Zugriffe MÜSSEN nach Fail-Safe Defaults abgelehnt werden.

---

## Konsequenzen

### Positive Konsequenzen

- gute Kompatibilität mit bestehenden Anwendungen und Dateisystemen,
- klassische Benutzer-, Gruppen- und ACL-Semantik bleibt nutzbar,
- kein zweites gleichrangiges natives Autorisierungsmodell,
- Legacy-Zugriffe bleiben in das Capability-Modell integrierbar,
- Least Privilege und Fail-Safe Defaults bleiben erhalten.

### Negative Konsequenzen

- DAC-zu-Capability-Mapping erzeugt zusätzliche Komplexität,
- bestimmte Legacy-Semantiken lassen sich möglicherweise nicht verlustfrei abbilden,
- Compatibility Domains benötigen eigene Policy- und Mapping-Regeln.

---

## Verworfene Alternativen

### DAC als primäres NovaOS-Autorisierungsmodell

Verworfen.

DAC ist für das native objekt-, service-, capability- und distributed-orientierte Sicherheitsmodell von NovaOS zu grob.

### DAC vollständig entfernen

Verworfen.

Dies würde die Kompatibilität mit bestehenden Anwendungen, Dateisystemen und Protokollen unnötig einschränken.

### DAC und Capabilities als zwei unabhängige Autorisierungsmodelle

Verworfen.

Zwei gleichrangige Authority-Systeme würden Sicherheitsentscheidungen unnötig komplex und schwer introspektierbar machen.

---

## Abhängigkeiten

- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0002_Kein_universelles_Root_Privileg`
- `ADR-SECURITY-0003_Least_Privilege_und_Privilege_Separation`
- `ADR-STORAGE-0001_VFS_als_Kompatibilitäts_und_Projektionsschicht`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-DAC-0001`
- `NPSPEC-SECURITY-DAC-MAPPING-0001`
- `NPSPEC-SECURITY-DAC-ACL-0001`
- `NPSPEC-SECURITY-DAC-COMPATIBILITY-0001`
- `NPSPEC-SECURITY-DAC-TEST-0001`

---

## Ergebnis

NovaOS behält DAC dort bei, wo klassische Software und Dateisysteme diese Semantik benötigen:

```text
DAC
 ↓
Compatibility Policy
 ↓
Capability Mapping
 ↓
Native Authority Enforcement
```

Damit bleibt bestehende DAC-basierte Software kompatibel, während Capability-Based Security das einheitliche native Autorisierungsfundament von NovaOS bleibt.