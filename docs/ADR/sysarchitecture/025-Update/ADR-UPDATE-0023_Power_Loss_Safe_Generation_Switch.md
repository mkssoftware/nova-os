# ADR-UPDATE-0023 – Power-Loss-Safe Generation Switch

## Status

Angenommen

## Kategorie

Update / Atomicity / Power-Failure Safety

## Kontext

NovaOS verwendet versionierte und unveränderliche Systemzustände.

Nach Vorbereitung einer neuen Generation muss die aktive Generation gewechselt werden. Ein Stromausfall während dieses Wechsels darf keinen undefinierten oder nicht bootfähigen Zustand erzeugen.

## Entscheidung

Der Wechsel zwischen Systemgenerationen erfolgt **atomar und power-loss-safe**.

```text
Generation N
     ↓
Prepare N+1
     ↓
Persist + Verify
     ↓
Atomic Generation Switch
     ↓
Generation N+1
```

Die neue Generation darf erst aktiviert werden, wenn alle dafür erforderlichen Daten persistent und verifiziert sind.

## Generation Pointer

NovaOS verwendet eine kleine persistente Referenz auf die aktive Generation.

```text
Active Generation
       ↓
Generation N
```

Beim Update wird zunächst die neue Generation vollständig vorbereitet.

Erst danach wird die aktive Referenz atomar auf die neue Generation umgestellt.

## Power Loss

Ein Stromausfall darf nur zu einem eindeutig bestimmbaren Zustand führen:

```text
Before Switch → Generation N

After Switch  → Generation N+1
```

Ein teilweise aktivierter Mischzustand ist nicht zulässig.

## Reihenfolge

Die erforderliche Persistenzreihenfolge lautet:

```text
Write Generation N+1
        ↓
Flush
        ↓
Verify
        ↓
Persist Metadata
        ↓
Atomic Switch
        ↓
Flush
```

Die konkrete Storage-Implementierung wird durch die entsprechenden NPSPECs definiert.

## Recovery

Nach einem unerwarteten Stromverlust muss NovaOS feststellen können, welche Generation vollständig committed wurde.

```text
Boot
 ↓
Read Generation State
 ↓
Validate
 ↓
Select Valid Generation
```

Eine unvollständig vorbereitete Generation darf nicht aktiviert werden.

## Boot Health

Der Generation Switch bedeutet noch nicht, dass die neue Generation dauerhaft als Known-Good gilt.

```text
Generation Switch
       ↓
Boot Candidate
       ↓
Boot Health
       ↓
Final Commit
```

Schlägt der Boot Health Check fehl, kann auf die vorherige zulässige Generation zurückgefallen werden.

## Normative Anforderungen

1. Generation Switches MÜSSEN atomar erfolgen.
2. Die neue Generation MUSS vor Aktivierung vollständig persistent sein.
3. Die neue Generation MUSS vor dem Switch verifiziert werden.
4. Ein Stromausfall DARF keinen undefinierten Mischzustand erzeugen.
5. Nach einem Stromausfall MUSS die gültige aktive Generation eindeutig bestimmbar sein.
6. Unvollständige Generationen DÜRFEN nicht aktiviert werden.
7. Der vorherige Recovery-Zustand MUSS bis zum erfolgreichen Boot Health Commit erhalten bleiben.
8. Generation Switch und Commit-Metadaten MÜSSEN power-loss-safe gespeichert werden.

## Konsequenzen

### Positive Konsequenzen

- Stromausfälle während Updates beschädigen nicht den aktiven Systemzustand.
- Systemgenerationen bleiben eindeutig.
- Recovery und A/B-Fallback werden zuverlässiger.
- Updates benötigen keine In-Place-Mutation der aktiven Basis.

### Negative Konsequenzen

- Storage benötigt atomare beziehungsweise entsprechend abgesicherte Metadatenoperationen.
- Persistenzreihenfolge und Flush-Semantik müssen exakt definiert werden.

## Abhängigkeiten

- `ADR-UPDATE-0002_Transactional_Update_Pipeline`
- `ADR-UPDATE-0003_Atomic_Updates`
- `ADR-UPDATE-0004_A_B_Systembasis`
- `ADR-UPDATE-0007_Immutable_System_Basis`
- `ADR-UPDATE-0022_Boot_Health_vor_Update_Commit`
- `ADR-TRANSACTION-0006_Transaction_Log_und_Power_Failure_Safety`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-GENERATION-SWITCH-0001`
- `NPSPEC-UPDATE-GENERATION-METADATA-0001`
- `NPSPEC-UPDATE-GENERATION-RECOVERY-0001`

## Ergebnis

NovaOS aktiviert neue Systemgenerationen erst nach vollständiger Persistierung und Verifikation:

```text
Generation N
     ↓
Build N+1
     ↓
Persist
     ↓
Verify
     ↓
Atomic Switch
     ↓
Boot Health
     ↓
Commit
```

Der zentrale Grundsatz lautet:

```text
Nach einem Stromausfall
existiert immer eine eindeutig
bestimmbare gültige Generation.
```