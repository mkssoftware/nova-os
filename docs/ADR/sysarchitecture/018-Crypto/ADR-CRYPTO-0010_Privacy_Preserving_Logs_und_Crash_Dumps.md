# ADR-CRYPTO-0010 – Privacy-Preserving Logs und Crash Dumps

## Status

Angenommen

## Kategorie

Cryptography / Privacy / Logging / Diagnostics / Data Protection

## Kontext

Logs und Crash Dumps sind für Diagnose, Self-Healing, Fehleranalyse und Architecture Introspection notwendig.

Gleichzeitig können sie sensible Informationen enthalten, beispielsweise:

- personenbezogene Daten,
- Dateinamen und Pfade,
- Speicherinhalte,
- Secrets und Tokens,
- kryptographisches Material,
- Netzwerkdaten,
- Benutzerinhalte,
- Capability-Informationen,
- interne Systemzustände.

Insbesondere vollständige Memory Dumps können Daten enthalten, die mit dem eigentlichen Fehler nichts zu tun haben.

NovaOS benötigt deshalb eine Diagnosearchitektur, die möglichst hohe technische Aussagekraft mit Datenminimierung und Schutz sensibler Informationen verbindet.

---

## Entscheidung

NovaOS behandelt **Privacy-Preserving Logging und Crash Dumps als systemweite Sicherheitsanforderung**.

```text
Diagnostic Data
      ↓
Classification
      ↓
Minimization / Redaction
      ↓
Protected Diagnostic Object
      ↓
Authorized Analysis
```

Diagnosedaten werden nicht automatisch als unkritische Systemdaten betrachtet.

---

## Grundprinzip

```text
Collect what is necessary,
not everything that is available.
```

und:

```text
Diagnostics must not become a data leak.
```

---

## Datenminimierung

Logs sollen bevorzugt strukturierte technische Informationen enthalten, anstatt beliebige Speicher- oder Nutzdaten zu kopieren.

Bevorzugt werden beispielsweise:

```text
Event Identity
Component Identity
Error Code
State
Timestamp
Causation ID
Resource State
Relevant Metrics
```

Payloads und Benutzerinhalte werden nur aufgenommen, wenn dies für den jeweiligen Diagnosezweck erforderlich und erlaubt ist.

---

## Sensitive Data Classification

Logging- und Crash-Dump-Systeme müssen Semantic Types, Information-Flow-Labels und andere vorhandene Klassifikationen berücksichtigen können.

```text
Data
 ↓
Classification
 ├── Normal
 ├── Sensitive
 ├── Secret
 └── Non-Exportable
```

Besonders geschützte Daten dürfen nicht allein deshalb in Diagnoseobjekte gelangen, weil ein Fehler aufgetreten ist.

---

## Secrets

Secret-Material soll grundsätzlich nicht in Logs oder Crash Dumps aufgenommen werden.

Dies betrifft insbesondere:

- private Schlüssel,
- Session Secrets,
- Tokens,
- Credentials,
- Capability Secrets,
- Secure-Randomness-Zustände.

```text
Secret
   ↓
Diagnostic Boundary
   ↓
Redact / Omit
```

Hardwaregeschützte oder nicht exportierbare Secrets dürfen durch Diagnosefunktionen nicht exportierbar werden.

---

## Crash Dumps

NovaOS unterstützt unterschiedliche Crash-Dump-Stufen.

Konzeptionell können diese beispielsweise reichen von:

```text
Minimal Diagnostic State
        ↓
Structured Crash Context
        ↓
Selective Memory Regions
        ↓
Full Memory Dump
```

Je sensitiver und umfangreicher der Dump, desto stärker müssen Autorisierung und Schutz sein.

Ein Full Memory Dump ist kein uneingeschränkter Standardmodus.

---

## Schutz gespeicherter Diagnosedaten

Schützenswerte Logs und Crash Dumps müssen als geschützte Storage Objects behandelt werden können.

```text
Diagnostic Object
      ↓
Authenticated Encryption
      ↓
Protected Storage
```

Dabei können Per-Object Encryption, eigene Key Contexts, Retention Policies und Secure Erase verwendet werden.

---

## Export

Das Recht, Diagnosedaten lokal zu erzeugen oder zu analysieren, impliziert nicht automatisch das Recht, diese zu exportieren.

```text
Local Diagnostic Use
        ≠
Diagnostic Export
```

Export an Entwickler, Supportsysteme, Cloud-Dienste oder andere Geräte muss separat autorisiert werden und unterliegt Information Flow Control und Data Sovereignty.

---

## Pseudonymisierung und Redaction

Wo die Diagnosefunktion erhalten bleibt, sollen identifizierende oder sensitive Informationen reduziert werden können.

Beispiele:

```text
User Identity → Pseudonymous ID
Memory Address → Normalized Reference
File Path → Redacted Path
Network Endpoint → Reduced Representation
Secret → Removed
```

Die konkrete Redaction- und Pseudonymisierungssemantik wird in NPSPECs definiert.

---

## Causation und Introspection

Privacy-Preserving Logging darf die systemweite Nachvollziehbarkeit nicht unnötig zerstören.

NovaOS soll deshalb bevorzugt technische Referenzen wie:

```text
Trace ID
Causation ID
Operation ID
Execution Domain ID
Component ID
```

verwenden, statt vollständige Nutzdaten in Logs zu replizieren.

---

## Retention und Secure Erase

Diagnosedaten dürfen nicht unbegrenzt gespeichert werden, sofern dies nicht ausdrücklich erforderlich ist.

Retention Policies bestimmen Lebensdauer und Löschung.

```text
Diagnostic Object
      ↓
Retention Policy
      ↓
Expire
      ↓
Secure Erase
```

Für verschlüsselte Diagnoseobjekte kann Cryptographic Erasure verwendet werden.

---

## Fail-Safe Verhalten

Kann eine erforderliche Privacy Policy bei der Erzeugung eines Diagnoseobjekts nicht zuverlässig angewendet werden, darf NovaOS sensible Daten nicht vorsorglich vollständig erfassen.

```text
Privacy Classification Unknown
          ↓
Minimize / Omit Sensitive Data
```

Diagnosekomfort darf harte Security- oder Privacy-Grenzen nicht umgehen.

---

## Normative Anforderungen

1. NovaOS MUSS Logs und Crash Dumps als potenziell sensitive Daten behandeln.
2. Diagnoseerfassung MUSS dem Prinzip der Datenminimierung folgen.
3. Logging MUSS vorhandene Semantic Types und Information-Flow-Klassifikationen berücksichtigen können.
4. Secrets SOLLEN grundsätzlich aus Logs und Crash Dumps ausgeschlossen werden.
5. Diagnosefunktionen DÜRFEN nicht exportierbare Secrets NICHT exportierbar machen.
6. NovaOS MUSS unterschiedliche Crash-Dump-Stufen unterstützen können.
7. Full Memory Dumps DÜRFEN NICHT der uneingeschränkte Standardmodus sein.
8. Sensitive Diagnosedaten MÜSSEN verschlüsselbar und integritätsgeschützt sein.
9. Diagnosedaten MÜSSEN eigene Retention Policies besitzen können.
10. Lokale Diagnose-Authority DARF NICHT automatisch Export-Authority implizieren.
11. Export MUSS Information Flow Control und Data Sovereignty berücksichtigen.
12. Redaction und Pseudonymisierung MÜSSEN unterstützt werden können.
13. Causation und Tracing SOLLEN ohne unnötige Replikation von Nutzdaten möglich sein.
14. Diagnosedaten MÜSSEN kontrolliert löschbar sein.
15. Privacy- oder Security-Anforderungen DÜRFEN für Diagnosezwecke NICHT stillschweigend abgeschwächt werden.

---

## Konsequenzen

### Positive Konsequenzen

- geringeres Risiko sensibler Datenlecks durch Diagnosesysteme,
- Logs bleiben für Self-Healing und Fehleranalyse nutzbar,
- kontrollierter Umgang mit Crash Dumps,
- Integration mit IFC, Data Sovereignty und Per-Object Encryption,
- klare Trennung zwischen Diagnose und Datenexport.

### Negative Konsequenzen

- Redaction kann bestimmte Fehleranalysen erschweren,
- sichere Crash-Dump-Erzeugung benötigt zusätzliche Systemlogik,
- vollständige Debug-Informationen benötigen gegebenenfalls explizit erhöhte Authority.

---

## Verworfene Alternativen

### Vollständige Logs für maximale Diagnosefähigkeit

Verworfen.

Dies würde unnötige Kopien sensibler Informationen erzeugen.

### Full Memory Dump bei jedem Crash

Verworfen.

Speicher kann große Mengen sicherheitskritischer und personenbezogener Daten enthalten.

### Diagnose-Daten grundsätzlich als vertrauenswürdig behandeln

Verworfen.

Logs und Dumps sind selbst schützenswerte Datenobjekte und potenzielle Angriffsziele.

### Automatischer Upload von Crash Dumps

Verworfen.

Lokale Diagnoseberechtigung impliziert keine Berechtigung zur externen Datenübertragung.

---

## Abhängigkeiten

- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-CRYPTO-0003_Per_Object_Encryption`
- `ADR-CRYPTO-0006_Secure_Erase_als_Key_Destruction_wo_möglich`
- `ADR-CRYPTO-0007_Authenticated_Encryption_für_schützenswerte_Objekte`
- `ADR-SECURITY-0015_Information_Flow_Control`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-SECURITY-0018_Fail_Closed_für_Sicherheitsentscheidungen`
- `ADR-SECURITY-0019_Secret_Use_und_Secret_Export_getrennt`

---

## Zugehörige NPSPECs

- `NPSPEC-CRYPTO-PRIVACY-LOGGING-0001`
- `NPSPEC-CRYPTO-CRASH-DUMP-PROTECTION-0001`
- `NPSPEC-CRYPTO-DIAGNOSTIC-REDACTION-0001`
- `NPSPEC-CRYPTO-DIAGNOSTIC-CLASSIFICATION-0001`
- `NPSPEC-CRYPTO-DIAGNOSTIC-EXPORT-0001`
- `NPSPEC-CRYPTO-DIAGNOSTIC-RETENTION-0001`
- `NPSPEC-CRYPTO-PRIVACY-DIAGNOSTICS-TEST-0001`

---

## Ergebnis

NovaOS gestaltet Diagnosefunktionen nach dem Prinzip:

```text
System Failure
      ↓
Relevant Diagnostic Data
      ↓
Minimize + Classify + Redact
      ↓
Protect
      ↓
Authorized Analysis
      ↓
Retention / Secure Erase
```

Logs und Crash Dumps bleiben damit leistungsfähige Werkzeuge für Diagnose, Self-Healing und Architecture Introspection, ohne automatisch zu unkontrollierten Kopien sensibler System- und Benutzerdaten zu werden.