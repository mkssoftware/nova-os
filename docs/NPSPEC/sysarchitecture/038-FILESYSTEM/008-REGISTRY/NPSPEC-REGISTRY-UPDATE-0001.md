# NPSPEC-REGISTRY-UPDATE-0001 – Nova Registry Update

## Status

Angenommen

## Kategorie

Registry / Update

## Zweck

NovaOS definiert den kontrollierten Änderungsprozess für System-Registries.

Registry-Einträge müssen erstellt, geändert, ersetzt oder entfernt werden können, ohne inkonsistente Zwischenzustände oder ungültige Referenzen sichtbar zu machen.

## Grundprinzipien

```text
Registry Update ≠ System Update
Registration ≠ Authority
Update ≠ Permission Grant
Staged ≠ Published
Published ≠ Trusted
Delete ≠ Immediate Destruction
```

## Update-Modell

Eine Registry-Änderung kann umfassen:

```text
Register
Modify
Replace
Disable
Enable
Remove
```

Betroffen sein können insbesondere:

```text
Capabilities
Types
Services
Algorithms
Devices
Codecs
System Components
```

## Ablauf

```text
Update Request
      ↓
Authorize
      ↓
Validate
      ↓
Check References
      ↓
Stage
      ↓
Commit
      ↓
Publish
      ↓
Verify
```

Ein neuer Registry-Zustand darf erst nach erfolgreichem Commit sichtbar werden.

## Transaktionen

Zusammengehörige Änderungen müssen als gemeinsame Registry-Transaktion ausführbar sein.

```text
Generation N
    ↓
Staged Changes
    ↓
Atomic Commit
    ↓
Generation N+1
```

Leser sehen entweder den vorherigen oder den neuen gültigen Zustand.

## Referenzen

Vor Änderung oder Entfernung eines Eintrags müssen relevante Abhängigkeiten berücksichtigt werden.

Beispiele:

```text
Capability → Provider
Service → Interface
Codec → Type
Algorithm → Implementation
Device → Driver
```

Eine Änderung darf keine unkontrollierten ungültigen Referenzen erzeugen.

## Versionierung

Inkompatible Änderungen müssen über neue Versionen oder Identitäten veröffentlicht werden.

Bestehende stabile Identitäten dürfen nicht stillschweigend mit einer inkompatiblen Bedeutung überschrieben werden.

## Entfernung

Ein verwendeter Registry-Eintrag kann zunächst deaktiviert oder als auslaufend markiert werden:

```text
Active
  ↓
Deprecated
  ↓
Disabled
  ↓
Removed
```

Bereits ausgegebene Authority wird durch das Entfernen eines Registry-Eintrags nicht automatisch widerrufen.

Capability-Revocation ist ein separater Sicherheitsvorgang.

## Parallelität

Gleichzeitige Registry-Änderungen müssen kontrolliert behandelt werden.

Versions- oder Generation-Prüfungen sollen verhindern, dass neuere Änderungen unbeabsichtigt durch veraltete Updates überschrieben werden.

## Fehlerbehandlung

Schlägt eine Registry-Aktualisierung fehl:

```text
Abort
  ↓
Discard Staged State
  ↓
Preserve Previous Generation
```

Ein teilweise veröffentlichter Registry-Zustand ist unzulässig.

## Sicherheit

Registry-Änderungen benötigen explizite Authority.

Besonders sicherheitsrelevante Einträge können zusätzlich Trust-, Integritäts- oder Policy-Prüfungen erfordern.

## Normative Anforderungen

1. Registry-Änderungen MÜSSEN explizit autorisiert sein.
2. Änderungen MÜSSEN vor Veröffentlichung validiert werden.
3. Zusammengehörige Änderungen MÜSSEN transaktional ausführbar sein.
4. Teilweise veröffentlichte Registry-Zustände DÜRFEN nicht entstehen.
5. Registry-Generationen MÜSSEN unterscheidbar sein.
6. Abhängigkeiten und Referenzen MÜSSEN vor kritischen Änderungen berücksichtigt werden.
7. Inkompatible Bedeutungsänderungen DÜRFEN nicht stillschweigend bestehende Versionen ersetzen.
8. Entfernung eines Registry-Eintrags DARF nicht automatisch bestehende Authority widerrufen.
9. Fehlgeschlagene Updates MÜSSEN den vorherigen gültigen Zustand erhalten.
10. Gleichzeitige Änderungen MÜSSEN konfliktkontrolliert verarbeitet werden.
11. Registry-Updates DÜRFEN keine impliziten Berechtigungen erzeugen.
12. Änderungen, Generationen und Fehler MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-REGISTRY-0001`
- `NPSPEC-REGISTRY-QUERY-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-SYSTEM-POLICY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann Registry-Inhalte atomar, versioniert und kontrolliert verändern. Leser sehen ausschließlich gültige Registry-Generationen, während fehlerhafte oder konkurrierende Änderungen keinen inkonsistenten Systemzustand erzeugen.