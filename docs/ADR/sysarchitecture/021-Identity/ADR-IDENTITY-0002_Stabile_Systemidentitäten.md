# ADR-IDENTITY-0002 – Stabile Systemidentitäten

## Status

Angenommen

## Kategorie

Identity Architecture / System Identity / Lifecycle

## Kontext

NovaOS besitzt zahlreiche Entitäten, die über Neustarts, Migrationen oder den Austausch ihrer konkreten Implementierung hinweg eindeutig identifizierbar bleiben müssen.

Dazu gehören beispielsweise:

- Systemdienste,
- Anwendungen,
- Geräte,
- Capabilities,
- Benutzeridentitäten,
- Execution Domains,
- vertrauenswürdige Systemkomponenten.

Kurzlebige Kennungen wie Process IDs, Speicheradressen, Netzwerkadressen oder Runtime Handles sind dafür ungeeignet.

## Entscheidung

NovaOS führt **stabile Systemidentitäten** als dauerhafte logische Identitäten für langlebige Systementitäten ein.

```text
Stable Identity
      ↓
Current Instance
      ↓
Runtime Handles / Addresses / Processes
```

Die stabile Identity bleibt erhalten, solange die logisch gleiche Entität besteht.

## Grundprinzip

```text
Stable identity represents logical continuity,
not a particular runtime instance.
```

Ein Neustart, eine Migration oder ein Provider-Wechsel erzeugt daher nicht automatisch eine neue Identity.

## Identity und Instance

NovaOS unterscheidet zwischen stabiler Identity und konkreter Instanz.

```text
Service Identity
    ├── Instance A
    ├── Instance B
    └── Instance C
```

Mehrere Instanzen können derselben logischen Service Identity zugeordnet sein.

Wo eine konkrete Instanz sicherheitsrelevant ist, muss zusätzlich eine Instance Identity verwendet werden können.

## Lebensdauer

Stabile Identities können länger leben als ihre aktuelle Ausführungsinstanz.

```text
Service Instance A
       ↓ restart
Service Instance B

Stable Service Identity unchanged
```

Dies ermöglicht dauerhafte Referenzen, Policies, Capabilities und Audit-Beziehungen.

## Identity-Klassen

Das gemeinsame Identity-Modell muss unterschiedliche Systementitäten unterstützen können, beispielsweise:

```text
User Identity
Application Identity
Service Identity
Device Identity
Capability Provider Identity
System Component Identity
```

Die konkrete Semantik der jeweiligen Identity-Klasse wird durch spezialisierte Contracts definiert.

## Erzeugung und Eindeutigkeit

Stabile Systemidentitäten müssen so erzeugt werden, dass sie innerhalb ihres definierten Identity Scopes eindeutig sind.

Eine Identity darf nicht aus veränderlichen Eigenschaften wie:

```text
Display Name
Process ID
IP Address
Installationspfad
```

abgeleitet werden.

Die konkrete Identifier-Repräsentation wird in NPSPECs festgelegt.

## Identity Continuity

Die Übernahme einer bestehenden Identity muss kontrolliert erfolgen.

Ein beliebiger neuer Prozess oder Provider darf nicht allein durch Verwendung derselben Kennung behaupten, dieselbe Systementität zu sein.

```text
Identity Continuity
        =
Stable Identifier
        +
Verified Binding
```

Damit wird Identity-Spoofing bei Neustart, Migration oder Hot Replacement verhindert.

## Hot Replacement und Migration

Stabile Identities unterstützen NovaOS Live Evolution.

```text
Implementation V1
       ↓
Hot Replacement
       ↓
Implementation V2
       ↓
Same Logical Identity
```

Dies ist nur zulässig, wenn die Identity-Bindung und die erforderlichen Trust- und Security-Bedingungen erhalten bleiben.

## Identity und Authority

Eine stabile Identity erzeugt keine Authority.

```text
Stable Identity ≠ Capability
Stable Identity ≠ Trust
```

Sie dient als Referenz für Security-, Trust-, Policy- und Audit-Systeme.

Die tatsächliche Authority wird weiterhin durch Capabilities und Policies bestimmt.

## Persistenz

Persistente Systemidentitäten müssen über relevante Neustarts hinweg wiederherstellbar sein.

Temporäre Entitäten dürfen dagegen bewusst nur innerhalb eines begrenzten Lifecycles existieren.

Nicht jede Runtime-Entität benötigt deshalb automatisch eine persistente Identity.

## Normative Anforderungen

1. NovaOS MUSS stabile Systemidentitäten für langlebige logische Systementitäten unterstützen.
2. Stabile Identities MÜSSEN von Runtime-Instanzen und Runtime Handles getrennt sein.
3. Neustart, Migration oder Hot Replacement DÜRFEN eine stabile Identity nicht automatisch verändern.
4. Mehrere Instanzen MÜSSEN einer gemeinsamen logischen Identity zugeordnet werden können.
5. Konkrete Instanzen MÜSSEN bei Bedarf zusätzlich eindeutig identifizierbar sein.
6. Stabile Identities DÜRFEN NICHT von Process IDs, Adressen, Display Names oder Speicherorten abhängen.
7. Identity Continuity MUSS systemverifizierbar sein.
8. Eine neue Instanz DARF eine bestehende Identity NICHT ohne autorisierte Bindung übernehmen.
9. Persistente Identities MÜSSEN relevante Systemneustarts überstehen können.
10. Nicht langlebige Entitäten DÜRFEN lifecyclegebundene Identities verwenden.
11. Identity DARF keine automatische Authority oder Trust-Einstufung erzeugen.
12. Identity-Lifecycle und relevante Bindungen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- stabile Referenzen über Neustarts und Migrationen,
- sichere Unterstützung für Hot Replacement,
- klare Trennung zwischen logischer Entität und Instanz,
- dauerhafte Policies, Capabilities und Audit-Beziehungen,
- geeignet für lokale und verteilte Systemkomponenten.

### Negative Konsequenzen

- Identity Lifecycle und Bindings müssen dauerhaft verwaltet werden,
- Migration und Replacement benötigen eine verifizierte Identity-Übernahme.

## Verworfene Alternativen

### Runtime IDs als dauerhafte Identity

Verworfen, da diese kurzlebig und wiederverwendbar sein können.

### Bei jedem Neustart neue Identity

Verworfen, da dadurch die logische Kontinuität einer Systementität verloren geht.

### Identity aus Namen oder Speicherort ableiten

Verworfen, da beide veränderlich sind.

## Abhängigkeiten

- `ADR-IDENTITY-0001_Identity_getrennt_von_Display_Name_Credential_Process_und_Address`
- `ADR-ARCH-0012_Hot_Replacement_Live_Evolution`
- `ADR-ARCH-0019_Evolvable_System_Architecture`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`

## Zugehörige NPSPECs

- `NPSPEC-IDENTITY-STABLE-IDENTITY-0001`
- `NPSPEC-IDENTITY-INSTANCE-IDENTITY-0001`
- `NPSPEC-IDENTITY-CONTINUITY-0001`
- `NPSPEC-IDENTITY-LIFECYCLE-0001`
- `NPSPEC-IDENTITY-PERSISTENCE-0001`
- `NPSPEC-IDENTITY-STABLE-IDENTITY-TEST-0001`

## Ergebnis

NovaOS trennt die langlebige Identität einer Systementität von ihren kurzlebigen Ausführungsinstanzen:

```text
Stable System Identity
          │
    ┌─────┼─────┐
    ↓     ↓     ↓
Instance A  B   C
```

Damit bleiben Systementitäten über Neustarts, Migrationen und Live Replacement eindeutig referenzierbar, ohne Identity an veränderliche technische Eigenschaften zu koppeln.