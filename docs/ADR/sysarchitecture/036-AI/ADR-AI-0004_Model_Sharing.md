# ADR-AI-0004 – Model Sharing

## Status

Angenommen

## Kategorie

AI / Runtime / Resource Sharing

## Kontext

AI-Modelle können große Mengen an RAM, VRAM oder NPU Memory benötigen.

Wenn jede Anwendung dieselben Modelle separat lädt, entstehen unnötige Duplikate:

```text
Application A → Model X → 4 GB

Application B → Model X → 4 GB

Application C → Model X → 4 GB
```

Obwohl alle drei Anwendungen dasselbe Modell verwenden, würden 12 GB Speicher benötigt.

NovaOS soll deshalb Modellinstanzen systemweit gemeinsam nutzen können, ohne dabei Isolation, Berechtigungen oder Datenschutz aufzuheben.

## Entscheidung

Die `Nova.AI Runtime` unterstützt **systemweites Model Sharing**.

```text
Application A ─┐
Application B ─┼→ Nova.AI Runtime → Shared Model Instance
Application C ─┘
```

Unveränderliche Modellbestandteile dürfen zwischen mehreren AI Executions geteilt werden.

Ausführungsspezifischer Zustand bleibt dagegen logisch isoliert.

## Shared Model Components

Typischerweise gemeinsam nutzbar sind:

```text
Model Weights
Model Structure
Tokenizer
Static Lookup Tables
Compiled Kernels
Runtime Code
Read-Only Metadata
```

Diese Daten sollen möglichst nur einmal physisch im Speicher vorhanden sein.

## Execution State

Ausführungsspezifischer Zustand darf nicht automatisch geteilt werden.

Beispiele:

```text
Prompt
Conversation State
KV Cache
Embeddings
Temporary Activations
Generated Output
User Context
Secrets
```

Das Grundmodell lautet daher:

```text
Shared Model
     +
Private Execution Context
     ↓
AI Execution
```

## Shared Model Identity

Eine gemeinsam verwendete Modellinstanz muss eindeutig identifizierbar sein.

Mindestens relevant sind:

```text
Model ID
Model Version
Variant
Runtime Adapter
Execution Provider
```

Nur kompatible Anforderungen dürfen dieselbe Instanz verwenden.

## Read-Only Sharing

Modellgewichte sollen während normaler Inferenz bevorzugt unveränderlich behandelt werden.

```text
Shared Weights
     ↓
Read Only
     ↓
Multiple Executions
```

Dadurch können mehrere Prozesse oder Capabilities dieselben physischen Speicherseiten verwenden.

## Copy-on-Write

Muss eine Ausführung einen normalerweise geteilten Zustand verändern, soll NovaOS nach Möglichkeit Copy-on-Write verwenden.

```text
Shared State
    ↓
Modification
    ↓
Private Copy
```

Die Änderung darf andere Executions nicht unbeabsichtigt beeinflussen.

## Provider Scope

Model Sharing kann providerabhängig sein.

Beispiele:

```text
CPU RAM
GPU VRAM
NPU Memory
Shared Accelerator Memory
```

Eine GPU-residente Modellinstanz ist nicht automatisch mit einer CPU- oder anderen GPU-Instanz identisch.

Die Runtime darf deshalb mehrere Residency Instances derselben Model Version verwalten.

## Cross-Process Sharing

Model Sharing soll nicht auf einen einzelnen Prozess beschränkt sein.

```text
Process A ─┐
Process B ─┼→ Nova.AI Runtime → Model Instance
Process C ─┘
```

Die AI Runtime kontrolliert Lebenszyklus, Referenzen und Isolation.

## Permissions

Das Recht, eine AI Capability zu verwenden, bedeutet nicht automatisch Zugriff auf Daten anderer Nutzer oder Executions.

```text
Shared Model
    ≠
Shared Permissions
    ≠
Shared Context
```

Authorization wird pro Execution bewertet.

## Security Domains

Model Sharing darf nur erfolgen, wenn die erforderliche Isolation zwischen Security Domains erhalten bleibt.

Falls sichere gemeinsame Nutzung nicht garantiert werden kann:

```text
Shared Instance
      ↓
Not Safe
      ↓
Separate Instance
```

Security besitzt Vorrang vor Speicherersparnis.

## User Isolation

Bei mehreren Nutzern dürfen private Zustände nicht über gemeinsam verwendete Modelle übertragen werden.

Insbesondere dürfen folgende Daten nicht unbeabsichtigt wiederverwendet werden:

```text
Conversation Context
Prompt History
KV Cache
Personal Embeddings
Temporary Files
Private Adaptation State
```

## Stateful Models

Nicht jedes Modell ist vollständig zustandslos.

Die Registry muss deshalb beschreiben können, welche Bestandteile:

```text
Immutable
Shareable
Execution Local
User Local
Mutable
Persistent
```

sind.

Die AI Runtime verwendet diese Informationen zur sicheren Instanziierung.

## Model Adaptation

Fine-Tuning, LoRA, Adapter oder andere Modellanpassungen werden als explizite zusätzliche Modellzustände behandelt.

Beispiel:

```text
Base Model
   ├── Shared Weights
   │
   ├── Adapter A
   │     ↓
   │   User A
   │
   └── Adapter B
         ↓
       Workload B
```

Das gemeinsame Base Model muss dafür nicht vollständig dupliziert werden.

## Shared Prefix und KV Cache

Zustandsbehaftete Caches dürfen nur geteilt werden, wenn ihre Semantik und Sicherheitsgrenzen dies ausdrücklich erlauben.

```text
Public Shared Prefix
        ↓
Shareable Cache

Private Conversation
        ↓
Private Cache
```

Standardmäßig werden nutzer- oder executionbezogene KV Caches nicht zwischen unabhängigen Security Contexts geteilt.

## Resource Accounting

Geteilte Ressourcen müssen korrekt verrechnet werden.

NovaOS unterscheidet:

```text
Shared Model Memory
Private Execution Memory
Temporary Compute Memory
Cache Memory
```

Shared Memory darf nicht vollständig jeder einzelnen Execution als exklusiver Verbrauch zugerechnet werden.

## Lifecycle

Eine Shared Model Instance bleibt geladen, solange sie benötigt oder aufgrund der Cache Policy resident gehalten wird.

```text
Load
  ↓
Reference Count > 0
  ↓
Shared Use
  ↓
Reference Count = 0
  ↓
Cache / Evict
```

Die endgültige Residency wird durch Model Scheduling und Caching bestimmt.

## Failure Isolation

Der Fehler einer Execution darf andere Nutzer derselben Modellinstanz möglichst nicht beeinflussen.

```text
Execution A Failure
       ↓
Terminate Context A

Shared Model
       ↓
Remain Available
```

Ist die gemeinsame Modellinstanz selbst beschädigt, muss sie verworfen und neu validiert oder geladen werden.

## Deterministic Mode

Model Sharing darf das fachliche Ergebnis einer deterministischen Ausführung nicht verändern.

Eine Execution muss unabhängig davon reproduzierbar bleiben, ob das Modell:

```text
Already Resident
Newly Loaded
Shared
Exclusively Loaded
```

war.

## Observability

Model Sharing muss beobachtbar sein.

Relevante Informationen sind:

```text
Model ID
Model Version
Instance ID
Provider
Shared Memory
Private Memory
Reference Count
Execution IDs
Cache State
Isolation Mode
```

Private Inputs oder Outputs dürfen dadurch nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS SOLL identische AI-Modelle systemweit gemeinsam nutzbar machen.
2. Model Weights SOLLEN während normaler Inferenz bevorzugt read-only geteilt werden.
3. Ausführungsspezifischer Zustand MUSS logisch von Shared Model State getrennt sein.
4. Private Prompts, KV Caches und User Context DÜRFEN nicht implizit zwischen Executions geteilt werden.
5. Shared Model Instances MÜSSEN eindeutig einer Model ID, Version und Variante zugeordnet sein.
6. Model Sharing DARF Authorization und Security Isolation nicht aufheben.
7. Unsicheres Sharing MUSS durch getrennte Instanzen ersetzt werden können.
8. Cross-Process Model Sharing SOLL unterstützt werden.
9. Provider-spezifische Residency Instances MÜSSEN getrennt verwaltbar sein.
10. Copy-on-Write SOLL für veränderliche geteilte Zustände verwendet werden können.
11. Stateful Models MÜSSEN ihre teilbaren und privaten Zustände beschreiben können.
12. Model Adapter und Fine-Tuning States SOLLEN vom gemeinsamen Base Model getrennt behandelbar sein.
13. Private KV Caches DÜRFEN standardmäßig nicht zwischen unabhängigen Security Contexts geteilt werden.
14. Shared Resources MÜSSEN von privaten Execution Resources unterscheidbar sein.
15. Resource Accounting MUSS Shared Memory korrekt berücksichtigen.
16. Der Fehler einer einzelnen Execution SOLL andere Nutzer derselben Modellinstanz nicht beeinflussen.
17. Beschädigte Shared Model Instances MÜSSEN invalidierbar sein.
18. Model Sharing DARF Determinism Requirements nicht verletzen.
19. Model Sharing MUSS mit Model Scheduling und Caching integriert sein.
20. Shared Model State MUSS mit Observability und Resource Tracing korrelierbar sein.

## Konsequenzen

### Positive Konsequenzen

- identische Modelle müssen nicht mehrfach vollständig geladen werden,
- RAM, VRAM und NPU Memory werden deutlich effizienter genutzt,
- Anwendungen können gemeinsame Modelle verwenden, ohne voneinander abhängig zu werden,
- große Base Models können mit mehreren kleinen Adaptern kombiniert werden,
- Ladezeiten können durch bereits residente Modelle reduziert werden.

### Negative Konsequenzen

- Shared State und Private State müssen strikt getrennt werden,
- zustandsbehaftete Modelle benötigen zusätzliche Metadaten,
- Fehler- und Security-Isolation werden komplexer,
- Resource Accounting muss gemeinsame Ressourcen korrekt aufteilen.

## Abhängigkeiten

- `ADR-AI-0001_Systemweite_AI_Runtime`
- `ADR-AI-0002_Model_Registry`
- `ADR-AI-0003_Model_Scheduling_und_Caching`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-EXECUTION-0004_Resource_Budgets`
- `ADR-SECURITY-0003_Least_Privilege`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`
- `ADR-AUTONOMY-0007_Self_Protection`

## Zugehörige NPSPECs

- `NPSPEC-AI-MODEL-SHARING-0001`
- `NPSPEC-AI-SHARED-MODEL-STATE-0001`
- `NPSPEC-AI-EXECUTION-STATE-ISOLATION-0001`
- `NPSPEC-AI-SHARED-MODEL-ACCOUNTING-0001`
- `NPSPEC-AI-MODEL-ADAPTER-SHARING-0001`

## Ergebnis

NovaOS trennt gemeinsame Modellressourcen konsequent von privaten Ausführungszuständen:

```text
                 Shared Model
              /       |       \
             /        |        \
            ↓         ↓         ↓
      Execution A Execution B Execution C
           ↓          ↓          ↓
      Private State Private State Private State
```

Der zentrale Grundsatz lautet:

```text
Teure Modellressourcen
werden gemeinsam genutzt.

Private Ausführungszustände
bleiben getrennt.

Shared Model
bedeutet niemals
Shared User Context.
```