# ADR-UPDATE-0020 – Anti-Rollback Minimum Version

## Status

Angenommen

## Kategorie

Update / Security / Rollback Protection

## Kontext

Rollback ist für Recovery wichtig. Gleichzeitig darf NovaOS nicht auf Softwarestände zurückgesetzt werden, die bekannte kritische Sicherheitsprobleme besitzen oder nicht mehr zulässig sind.

Ein Angreifer könnte sonst versuchen, eine korrekt signierte, aber veraltete Version zu installieren:

```text
Secure Version V5
      ↓
Rollback
      ↓
Vulnerable Version V2
```

Eine gültige Signatur allein verhindert diesen Downgrade nicht.

## Entscheidung

NovaOS unterstützt eine **Anti-Rollback Minimum Version** für sicherheitsrelevante Komponenten und Systemzustände.

```text
Installed Version: V5
Minimum Version:   V4

Target V4 → Allowed
Target V3 → Blocked
```

Die Minimum Version definiert die älteste noch zulässige Version.

## Minimum Version

Die Grenze kann für unterschiedliche Komponenten separat definiert werden.

```text
Bootloader → Minimum V5
Kernel     → Minimum V12
Firmware   → Minimum V7
```

Sie muss Bestandteil eines vertrauenswürdig verwalteten Security State sein.

## Rollback

Normale Recovery-Rollbacks bleiben möglich, solange das Ziel die Minimum Version erfüllt.

```text
Current V6
   ↓
Rollback V5
   ↓
V5 >= Minimum V4
   ↓
Allowed
```

Ein Snapshot oder Known-Good-State unterhalb der Grenze darf nicht regulär aktiviert werden.

## Erhöhung

Die Minimum Version darf kontrolliert angehoben werden.

```text
Minimum V4
    ↓
Security Update
    ↓
Minimum V5
```

Vor der Erhöhung muss sichergestellt sein, dass ein gültiger Recovery-Zustand oberhalb der neuen Grenze existiert.

## Speicherung

Die Minimum Version muss manipulationsgeschützt gespeichert werden.

Je nach Plattform können dafür beispielsweise verwendet werden:

```text
TPM
Secure Storage
Firmware Counter
Protected Boot Metadata
```

Die konkrete Implementierung ist plattformabhängig.

## Recovery

Recovery darf Anti-Rollback nicht stillschweigend umgehen.

Ist kein zulässiger Recovery-Zustand mehr vorhanden, muss NovaOS einen expliziten Recovery-Pfad verwenden, statt automatisch eine bekannte verwundbare Version zu starten.

## Normative Anforderungen

1. NovaOS MUSS Anti-Rollback-Grenzen für sicherheitskritische Komponenten unterstützen können.
2. Die Minimum Version MUSS manipulationsgeschützt gespeichert werden.
3. Versionen unterhalb der gültigen Minimum Version DÜRFEN nicht regulär aktiviert werden.
4. Signierte Altversionen DÜRFEN Anti-Rollback nicht umgehen.
5. Die Minimum Version DARF nur über einen autorisierten Vorgang erhöht werden.
6. Vor einer Erhöhung MUSS ein zulässiger Recovery-Zustand vorhanden sein.
7. Rollback und Recovery MÜSSEN die Minimum Version berücksichtigen.
8. Anti-Rollback MUSS mit Verified Boot und der Update Pipeline integrierbar sein.

## Konsequenzen

### Positive Konsequenzen

- bekannte verwundbare Altversionen können dauerhaft gesperrt werden,
- Downgrade-Angriffe werden erschwert,
- signierte, aber unsichere Altstände reichen nicht zur Aktivierung aus.

### Negative Konsequenzen

- zu früh gesetzte Grenzen können Recovery-Möglichkeiten einschränken,
- der Minimum-Version-State muss besonders geschützt werden.

## Abhängigkeiten

- `ADR-UPDATE-0004_A_B_Systembasis`
- `ADR-UPDATE-0005_Snapshot_Based_Rollback`
- `ADR-UPDATE-0016_Bootloader_Update_Schutz`
- `ADR-UPDATE-0019_Vulnerability_Aware_Updating`
- `ADR-BOOT-0006_Verified_Boot`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-ANTI-ROLLBACK-0001`
- `NPSPEC-UPDATE-MINIMUM-VERSION-0001`
- `NPSPEC-UPDATE-ANTI-ROLLBACK-RECOVERY-0001`

## Ergebnis

NovaOS erlaubt Recovery auf ältere Zustände, aber nicht unter eine definierte Sicherheitsgrenze:

```text
Target Version
      ↓
Check Minimum Version
   ┌──┴───┐
   ↓      ↓
Allowed  Too Old
   ↓      ↓
Boot     Block
```

Der zentrale Grundsatz lautet:

```text
Rollback bleibt möglich,
aber nicht zurück
in einen gesperrten Sicherheitszustand.
```