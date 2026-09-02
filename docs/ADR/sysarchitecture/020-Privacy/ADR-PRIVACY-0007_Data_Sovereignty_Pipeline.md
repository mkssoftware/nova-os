# ADR-PRIVACY-0007 – Data Sovereignty Pipeline

## Status

Angenommen

## Kategorie

Privacy Architecture / Data Sovereignty / Data Pipeline / Information Flow

## Kontext

Data Sovereignty ist in NovaOS bereits als Hard Constraint definiert.

Daten können Anforderungen besitzen bezüglich:

- geografischer Region,
- administrativer Domäne,
- Provider,
- Trust Domain,
- Speicherort,
- Verarbeitungsort,
- Netzwerktransit,
- Remote Execution.

Eine einzelne Prüfung beim Öffnen oder Übertragen eines Objekts reicht jedoch nicht aus.

Daten durchlaufen häufig komplexe Verarbeitungsketten:

```text
Storage
   ↓
Application
   ↓
Capability
   ↓
GPU / NPU
   ↓
Remote Service
   ↓
Result Storage
```

Jede Stufe kann den tatsächlichen Speicher-, Verarbeitungs- oder Übertragungsort verändern.

NovaOS benötigt deshalb eine durchgängige Data-Sovereignty-Prüfung über den gesamten Datenpfad.

---

## Entscheidung

NovaOS führt eine **Data Sovereignty Pipeline** ein.

Sovereignty Constraints werden entlang des vollständigen Daten- und Verarbeitungspfades erhalten und vor relevanten Übergängen geprüft.

```text
Data
  ↓
Semantic Labels + Sovereignty Context
  ↓
Pipeline Planning
  ↓
Sovereignty Validation
  ↓
Eligible Processing Path
  ↓
Execution
  ↓
Continuous Revalidation
```

Data Sovereignty ist dabei ein Hard Constraint der Pipeline-Planung.

---

## Grundprinzip

```text
Data may only travel where it is permitted.

Data may only be processed where it is permitted.
```

Technische Erreichbarkeit oder funktionale Eignung eines Providers bedeutet nicht, dass Daten dorthin übertragen werden dürfen.

---

## Sovereignty Context

Schützenswerte Daten können einen Sovereignty Context besitzen.

Dieser kann Anforderungen enthalten wie:

```text
Allowed Regions
Forbidden Regions
Allowed Administrative Domains
Allowed Providers
Required Trust Domains
Storage Restrictions
Processing Restrictions
Transit Restrictions
Replication Restrictions
```

Die konkrete Repräsentation wird in NPSPECs definiert.

---

## Pipeline Planning

Vor der Ausführung muss NovaOS die geplanten Verarbeitungsschritte gegen die geltenden Sovereignty Constraints prüfen.

```text
Requested Pipeline
       ↓
Candidate Providers
       ↓
Candidate Locations
       ↓
Candidate Data Paths
       ↓
Sovereignty Filtering
       ↓
Eligible Pipeline
```

Erst danach dürfen Performance, Energieverbrauch, Kosten oder andere Soft Optimization Goals berücksichtigt werden.

---

## End-to-End-Prüfung

Sovereignty muss für alle relevanten Pipeline-Stufen gelten.

Beispiel:

```text
EU Storage
    ↓
EU Application
    ↓
EU Capability Provider
    ↓
Non-EU Remote AI Provider
```

Ist der letzte Schritt nicht zulässig, ist die gesamte geplante Pipeline in dieser Form nicht zulässig.

NovaOS muss einen anderen Provider oder eine andere Ausführungsstrategie wählen oder die Operation ablehnen.

---

## Verarbeitung und Speicherung

Sovereignty unterscheidet mindestens zwischen:

```text
Storage Location
Processing Location
Transit Location
```

Eine zulässige Speicherregion bedeutet nicht automatisch, dass Verarbeitung oder Netzwerktransit außerhalb dieser Region erlaubt sind.

Diese Eigenschaften müssen getrennt policyfähig sein.

---

## Capability Resolution

Capability Resolution muss Sovereignty Constraints berücksichtigen.

```text
Capability Requirement
        ↓
Providers
        ↓
Security / Trust
        ↓
Sovereignty
        ↓
Eligible Providers
        ↓
Soft Optimization
```

Ein schnellerer Provider darf nicht gewählt werden, wenn dessen Ausführung die Sovereignty Policy verletzt.

---

## Remote Execution

Vor Remote Execution muss ein Sovereignty Preflight stattfinden.

Dabei sind mindestens relevant:

```text
Remote Service
Execution Location
Administrative Domain
Provider
Trust Domain
Network Path
Relevant Dependencies
```

Die Prüfung erfolgt vor der Übertragung geschützter Daten.

---

## Netzwerkpfad

Nicht nur das Ziel, sondern auch relevante Transitpfade können Sovereignty Constraints unterliegen.

```text
Source
  ↓
Transit A
  ↓
Transit B
  ↓
Destination
```

Ein zulässiges Ziel macht einen verbotenen Transitpfad nicht automatisch zulässig.

VPN oder Verschlüsselung ändern die geografische oder administrative Lage eines Datenpfades nicht.

---

## Abgeleitete Daten

Sovereignty Constraints müssen bei abgeleiteten Daten berücksichtigt werden.

```text
Protected Input
      ↓
Processing
      ↓
Derived Output
```

Ein Ergebnis verliert seine Sovereignty-Anforderungen nicht automatisch durch Transformation.

Semantic Data Labels, Provenance und Information Flow Control bestimmen, welche Anforderungen weitergeführt oder neu abgeleitet werden.

---

## Pipeline Composition

Bei Capability Composition muss die Sovereignty-Prüfung die gesamte Composition berücksichtigen.

```text
Capability A
     ↓
Capability B
     ↓
Capability C
```

Jeder Provider und jeder relevante Datenübergang muss zulässig sein.

NovaOS darf die Pipeline global planen, um eine vollständig konforme Kombination zu finden.

---

## Dynamische Änderungen

Eine ursprünglich zulässige Pipeline kann während der Ausführung ungültig werden.

Beispiele:

```text
Provider Migration
Route Change
Service Failover
Connection Migration
Remote Dependency Change
Trust Change
Location Change
```

Relevante Änderungen müssen eine erneute Sovereignty-Prüfung auslösen.

---

## Failover und Replanning

Wenn ein Pipeline-Schritt nicht mehr zulässig oder verfügbar ist, darf NovaOS neu planen.

```text
Current Provider unavailable
        ↓
Replanning
        ↓
Sovereignty Filtering
        ↓
Alternative Provider
```

Hard Constraints dürfen dabei nicht abgeschwächt werden.

Ist keine zulässige Alternative vorhanden:

```text
Reject
Wait
Local Execution
Explicitly Allowed Degradation
```

Ein stiller Wechsel auf einen nicht konformen Provider ist verboten.

---

## Unknown State

Kann eine relevante Location-, Provider- oder Transit-Eigenschaft nicht zuverlässig bestimmt werden, gilt:

```text
Unknown ≠ Allowed
```

Die Policy kann zusätzliche Auflösung oder Validierung verlangen.

Für harte Sovereignty Constraints darf Unsicherheit nicht automatisch als Konformität interpretiert werden.

---

## Data Minimization und Selective Disclosure

Wenn eine vollständige Pipeline aufgrund von Sovereignty Constraints nicht zulässig ist, können Data Minimization oder Selective Disclosure eine zulässige Alternative ermöglichen.

```text
Protected Data
      ↓
Minimization / Selective Disclosure
      ↓
Reduced Data
      ↓
Sovereignty Evaluation
      ↓
Remote Processing
```

Dies ist nur zulässig, wenn die resultierenden Daten tatsächlich einer entsprechenden Policy unterliegen.

---

## Encryption

Verschlüsselung schützt Daten während Speicherung oder Übertragung, ersetzt aber keine Sovereignty-Prüfung.

```text
Encrypted
    ≠
Sovereignty Compliant
```

Eine Policy kann Verschlüsselung zusätzlich verlangen, aber ein verbotener Speicher-, Verarbeitungs- oder Transitort wird durch Verschlüsselung nicht automatisch zulässig.

---

## KI und adaptive Systeme

KI-basierte Planung darf innerhalb der Menge souveränitätskonformer Alternativen optimieren.

```text
Sovereignty-Compliant Candidates
        ↓
Adaptive Optimization
        ↓
Selected Pipeline
```

Prediction Error kann zukünftige Provider- oder Pfadwahl verbessern.

KI darf jedoch keine Sovereignty Constraints abschwächen oder umgehen.

---

## Introspection und Audit

NovaOS muss relevante Sovereignty-Entscheidungen nachvollziehbar machen können.

Beispielsweise:

```text
Data Identity
Semantic Labels
Sovereignty Policy
Pipeline
Providers
Processing Locations
Storage Locations
Transit Context
Rejected Candidates
Selected Path
Revalidation Events
Decision Reason
```

Geschützte Daten selbst dürfen dadurch nicht unnötig offengelegt werden.

---

## Normative Anforderungen

1. NovaOS MUSS Data Sovereignty über vollständige Daten- und Verarbeitungspipelines unterstützen.
2. Sovereignty MUSS als Hard Constraint vor Soft Optimization ausgewertet werden.
3. Sovereignty Constraints MÜSSEN mit Daten und relevanten abgeleiteten Daten weitergeführt werden können.
4. Storage-, Processing- und Transit-Constraints MÜSSEN getrennt ausdrückbar sein.
5. Capability Resolution MUSS Sovereignty Constraints berücksichtigen.
6. Capability Composition MUSS die Sovereignty der gesamten Pipeline berücksichtigen.
7. Remote Execution MUSS vor geschützter Datenübertragung einen Sovereignty Preflight durchführen.
8. Relevante Netzwerktransitpfade MÜSSEN in Sovereignty-Entscheidungen einbezogen werden können.
9. Provider-, Location-, Route- und Dependency-Änderungen MÜSSEN eine Revalidierung auslösen können.
10. Failover und Replanning DÜRFEN Sovereignty Constraints NICHT abschwächen.
11. Fehlende sicherheitsrelevante Sovereignty-Informationen DÜRFEN NICHT automatisch als zulässig gelten.
12. Verschlüsselung DARF NICHT als Ersatz für Sovereignty Compliance behandelt werden.
13. Data Minimization und Selective Disclosure MÜSSEN als policykonforme Alternativen integrierbar sein.
14. KI- und Adaptionssysteme DÜRFEN ausschließlich innerhalb souveränitätskonformer Alternativen optimieren.
15. Sovereignty-Entscheidungen MÜSSEN kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Data Sovereignty gilt durchgängig über komplexe Pipelines,
- lokale und entfernte Verarbeitung verwenden dasselbe Policy-Modell,
- Provider-Auswahl kann geografische und administrative Grenzen berücksichtigen,
- Failover und Migration bleiben souveränitätskonform,
- Capability Composition kann vollständige konforme Verarbeitungspfade planen.

### Negative Konsequenzen

- Provider und Netzwerkpfade benötigen geeignete Sovereignty-Metadaten,
- dynamische Änderungen können Revalidierung und Replanning verursachen,
- bestimmte Optimierungen oder Remote Provider können aufgrund harter Constraints ausgeschlossen werden.

---

## Verworfene Alternativen

### Sovereignty nur beim Speichern prüfen

Verworfen.

Daten können während Verarbeitung und Übertragung andere Domänen durchlaufen.

### Nur das endgültige Ziel prüfen

Verworfen.

Auch Verarbeitung, Transit und Zwischenstufen können Sovereignty Constraints verletzen.

### Verschlüsselte Daten dürfen überall verarbeitet werden

Verworfen.

Verschlüsselung ersetzt keine geografischen oder administrativen Vorgaben.

### Bei Provider-Ausfall beliebigen Fallback verwenden

Verworfen.

Failover darf Hard Constraints nicht umgehen.

---

## Abhängigkeiten

- `ADR-PRIVACY-0001_Data_Minimization`
- `ADR-PRIVACY-0003_Semantic_Data_Labels`
- `ADR-PRIVACY-0005_Selective_Disclosure`
- `ADR-SECURITY-0015_Information_Flow_Control`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-NETWORK-0021_Data_Sovereignty_Aware_Routing`
- `ADR-DISTCOMM-0011_Sovereignty_Preflight_vor_Remote_Execution`
- `ADR-CAPABILITY-0005_Capability_Composition`
- `ADR-CAPABILITY-0006_Capability_Negotiation`

---

## Zugehörige NPSPECs

- `NPSPEC-PRIVACY-SOVEREIGNTY-PIPELINE-0001`
- `NPSPEC-PRIVACY-SOVEREIGNTY-CONTEXT-0001`
- `NPSPEC-PRIVACY-SOVEREIGNTY-PLANNING-0001`
- `NPSPEC-PRIVACY-SOVEREIGNTY-PROPAGATION-0001`
- `NPSPEC-PRIVACY-SOVEREIGNTY-REVALIDATION-0001`
- `NPSPEC-PRIVACY-SOVEREIGNTY-AUDIT-0001`
- `NPSPEC-PRIVACY-SOVEREIGNTY-PIPELINE-TEST-0001`

---

## Ergebnis

NovaOS behandelt Data Sovereignty nicht als einzelne Prüfung an einer Systemgrenze, sondern als Eigenschaft des vollständigen Datenpfades:

```text
Data + Sovereignty Context
          ↓
Pipeline Planning
          ↓
Provider + Location + Transit Validation
          ↓
Eligible Pipeline
          ↓
Execution
          ↓
Continuous Revalidation
```

Damit dürfen Daten nur durch Speicher-, Verarbeitungs- und Kommunikationspfade fließen, die ihre Sovereignty Constraints über die gesamte Pipeline hinweg erfüllen.