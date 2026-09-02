# ADR-SECURITY-0015 – Information Flow Control

## Status

Angenommen

## Kategorie

Security / Information Flow / Data Sovereignty

## Kontext

Klassische Zugriffskontrolle entscheidet, ob eine Komponente auf eine Ressource zugreifen darf.

Nach einem erlaubten Zugriff muss NovaOS jedoch weiterhin kontrollieren können, wohin geschützte Informationen anschließend übertragen, verarbeitet oder gespeichert werden dürfen.

```text
Access Control:
Who may access data?

Information Flow Control:
Where may the data flow afterwards?
```

Dies ist insbesondere für Data Sovereignty, KI-Agenten, verteilte Verarbeitung und unterschiedlich vertrauenswürdige Execution Domains erforderlich.

---

## Entscheidung

NovaOS integriert **Information Flow Control (IFC)** als systemweite Sicherheitsarchitektur.

Schutzanforderungen werden mit Daten und daraus abgeleiteten Informationsflüssen verbunden.

```text
Data
 ↓
Information-Flow Policy
 ↓
Processing
 ↓
Transfer / Derivation / Storage
 ↓
Policy Enforcement
```

Eine vorhandene Capability zum Lesen oder Übertragen von Daten hebt deren Information-Flow-Regeln nicht auf.

---

## Grundprinzip

```text
Authority to access data
≠
Authority to move data anywhere.
```

---

## Informationsklassifikation

Daten können Sicherheits- und Flusseigenschaften besitzen.

Beispielsweise:

```text
Public
Internal
Confidential
Restricted
```

Zusätzlich können semantische Eigenschaften wie:

- Herkunft,
- Eigentümer,
- Trust Domain,
- erlaubte Empfänger,
- geografische Grenzen,
- Verarbeitungsvorgaben

Teil des Information-Flow-Kontexts sein.

Die konkrete Label- und Policy-Semantik wird in NPSPECs definiert.

---

## Flow Enforcement

IFC muss an relevanten Datenbewegungsgrenzen durchsetzbar sein.

Dazu gehören insbesondere:

- IPC,
- `Nova.Call`,
- Storage,
- Netzwerk,
- Shared Objects,
- Export,
- Remote Execution.

```text
Source
  ↓
Flow Policy
  ↓
Destination
  ↓
Allow / Deny
```

---

## Ableitungen

Aus geschützten Daten erzeugte Informationen dürfen ihre Schutzanforderungen nicht automatisch verlieren.

```text
Protected Input
      ↓
Processing
      ↓
Derived Output
      ↓
Inherited / Derived Flow Policy
```

Die konkreten Regeln für Policy-Vererbung, Kombination und Declassification werden in NPSPECs definiert.

---

## Declassification

Eine kontrollierte Abschwächung von Information-Flow-Regeln muss möglich sein, darf aber nur über explizit autorisierte Mechanismen erfolgen.

```text
Protected Data
      ↓
Authorized Declassification
      ↓
Reduced Restrictions
```

Normale Datenverarbeitung darf nicht implizit als Declassification wirken.

---

## Data Sovereignty

Data Sovereignty wird als spezialisierte harte Information-Flow-Anforderung integriert.

Beispielsweise kann festgelegt werden:

```text
Data may be processed:
EU only
```

Diese Einschränkung muss auch bei:

- Remote Execution,
- Service Discovery,
- Routing,
- Migration,
- Storage-Auswahl

erhalten bleiben.

---

## Capabilities und IFC

Capabilities und IFC beantworten unterschiedliche Fragen:

```text
Capability:
May this subject perform the operation?

IFC:
May this information flow to that destination?
```

Eine Operation ist nur zulässig, wenn beide Anforderungen erfüllt sind.

---

## KI-Agenten

KI-Agenten unterliegen denselben Information-Flow-Regeln wie andere Komponenten.

Ein Agent darf Daten nicht aufgrund eigener Planung an einen nicht autorisierten Dienst, Speicherort oder Empfänger übertragen.

Adaptive Entscheidungen dürfen harte Flow-Policies nicht abschwächen.

---

## Audit und Introspection

NovaOS muss relevante Informationsflüsse nachvollziehbar machen können.

```text
Data Source
    ↓
Classification / Policy
    ↓
Processing
    ↓
Destination
    ↓
Decision
```

Dabei dürfen geschützte Daten selbst nicht unnötig im Audit-System offengelegt werden.

---

## Normative Anforderungen

1. NovaOS MUSS systemweites Information Flow Control unterstützen.
2. Information-Flow-Policies MÜSSEN unabhängig von Capability Authority durchsetzbar sein.
3. Eine Capability DARF bestehende Information-Flow-Regeln NICHT automatisch aufheben.
4. Daten MÜSSEN mit Flow-relevanten Sicherheitsinformationen verknüpfbar sein.
5. IFC MUSS über IPC-, Storage-, Netzwerk- und Distributed-Communication-Grenzen durchsetzbar sein.
6. Abgeleitete Daten DÜRFEN ihre Schutzanforderungen NICHT automatisch verlieren.
7. Policy-Kombination und Policy-Vererbung MÜSSEN eindeutig definierbar sein.
8. Declassification MUSS explizit autorisiert werden.
9. Data Sovereignty MUSS als harte Information-Flow-Anforderung integrierbar sein.
10. Remote Execution und Migration DÜRFEN Information-Flow-Grenzen NICHT umgehen.
11. KI-Agenten MÜSSEN denselben IFC-Regeln wie andere Komponenten unterliegen.
12. Adaptive Systeme DÜRFEN harte Flow-Policies NICHT selbständig abschwächen.
13. Unbekannte sicherheitskritische Flow-Zustände MÜSSEN fail-safe behandelt werden.
14. Relevante Information-Flow-Entscheidungen MÜSSEN kontrolliert auditierbar und introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Schutz bleibt auch nach erlaubtem Datenzugriff erhalten,
- systemweite Grundlage für Data Sovereignty,
- sichere verteilte und KI-gestützte Verarbeitung,
- kontrollierte Weitergabe und Ableitung geschützter Informationen,
- Capabilities und Datenschutz ergänzen sich sauber.

### Negative Konsequenzen

- Datenflüsse benötigen zusätzliche Metadaten und Policyentscheidungen,
- Ableitungen und Policy-Kombinationen erhöhen die Komplexität,
- Legacy-Anwendungen benötigen gegebenenfalls Compatibility-Regeln.

---

## Verworfene Alternativen

### Zugriffskontrolle allein

Verworfen.

Nach einem erlaubten Lesezugriff könnte der weitere Informationsfluss nicht kontrolliert werden.

### IFC nur für Netzwerkverkehr

Verworfen.

Informationen können auch über Storage, IPC, Shared Memory oder lokale Verarbeitung Grenzen überschreiten.

### Automatisches Entfernen von Policies nach Verarbeitung

Verworfen.

Dies würde Schutzanforderungen durch einfache Datenableitung umgehen.

---

## Abhängigkeiten

- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0009_Unified_Data_Object_Pipelining`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0005_MAC_für_systemweite_harte_Sicherheitsregeln`
- `ADR-SECURITY-0006_ABAC_als_zentrale_Policyentscheidung`
- `ADR-NETWORK-0021_Data_Sovereignty_Aware_Routing`
- `ADR-DISTCOMM-0011_Sovereignty_Preflight_vor_Remote_Execution`

---

## Zugehörige NPSPECs

- `NPSPEC-SECURITY-IFC-0001`
- `NPSPEC-SECURITY-IFC-LABEL-0001`
- `NPSPEC-SECURITY-IFC-PROPAGATION-0001`
- `NPSPEC-SECURITY-IFC-DECLASSIFICATION-0001`
- `NPSPEC-SECURITY-IFC-ENFORCEMENT-0001`
- `NPSPEC-SECURITY-IFC-TEST-0001`

---

## Ergebnis

NovaOS schützt nicht nur den Zugriff auf Informationen, sondern auch deren weiteren Fluss:

```text
Data
 ↓
Access Authority
 ↓
Information-Flow Policy
 ↓
Processing / Movement
 ↓
Permitted Destination
```

Capabilities kontrollieren den Zugriff auf Ressourcen.

Information Flow Control kontrolliert, wohin daraus gewonnene Informationen gelangen dürfen.

Damit bildet IFC die systemweite Grundlage für Data Sovereignty und kontrollierte Informationsverarbeitung in NovaOS.