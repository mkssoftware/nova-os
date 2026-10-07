# NPSPEC-POWER-HYBRIDSLEEP-0001 – Nova Hybrid Sleep

## Status

Angenommen

## Kategorie

Power / Hybrid Sleep

## Zweck

NovaOS definiert Hybrid Sleep als Kombination aus Suspend und Hibernate.

Vor dem Wechsel in den Suspend-Zustand wird ein gültiges Hibernate Image erzeugt. Solange die Energieversorgung erhalten bleibt, kann NovaOS schnell aus dem Suspend fortsetzen. Bei Energieverlust kann der Systemzustand aus dem persistenten Hibernate Image wiederhergestellt werden.

## Grundprinzipien

```text
Hybrid Sleep = Hibernate Protection + Suspend Resume
Hybrid Sleep ≠ Hibernate Only
Hybrid Sleep ≠ Suspend Only
Hibernate Image ≠ Backup
Wake ≠ Successful Resume
Power Loss ≠ Session Loss
Stored State ≠ Trusted State
```

## Modell

```text
HybridSleepContext
├── HybridSleepID
├── HibernateImageID
├── SuspendMode
├── WakeSources[]
├── IntegrityState
├── PlatformState
└── ResumePath
```

## Ablauf

```text
Hybrid Sleep Request
        ↓
Validate
        ↓
Quiesce System
        ↓
Create Hibernate Image
        ↓
Verify Image
        ↓
Prepare Suspend
        ↓
Suspend Devices
        ↓
Platform Suspend
```

Der Suspend darf erst erfolgen, nachdem das Hibernate Image erfolgreich persistiert und verifiziert wurde.

## Resume-Pfade

Hybrid Sleep besitzt zwei mögliche Wiederaufnahmewege.

### Suspend Resume

Bleibt der Systemzustand im Arbeitsspeicher erhalten:

```text
Wake Event
    ↓
Platform Resume
    ↓
Restore Devices
    ↓
Verify
    ↓
Running
```

Das Hibernate Image wird für diesen Resume-Pfad nicht benötigt.

### Hibernate Resume

Geht der flüchtige Zustand durch Energieverlust verloren:

```text
Power On
   ↓
Boot
   ↓
Hibernate Image Discovery
   ↓
Validation
   ↓
Restore State
   ↓
Verify
   ↓
Running
```

Damit bleibt die Sitzung auch nach vollständigem Energieverlust wiederherstellbar.

## Hibernate Image

Das erzeugte Image unterliegt vollständig den Anforderungen von `NPSPEC-POWER-HIBERNATE-0001`.

Es muss insbesondere geschützt werden durch:

```text
Integrity
Confidentiality
Authenticity
Compatibility Validation
Replay Protection
```

## Konsistenz

Suspend-Zustand und Hibernate Image repräsentieren denselben logischen Systemzustand.

NovaOS muss verhindern, dass ein veraltetes Hibernate Image nach späteren Änderungen des laufenden Systems wiederhergestellt wird.

Nach erfolgreichem Suspend Resume muss das Image deshalb entsprechend invalidiert oder aktualisiert werden.

## Geräte

Geräte werden wie beim normalen Suspend in geeignete Zustände überführt.

Beim Resume können Geräte:

```text
Restored
Reinitialized
Reconfigured
```

werden.

Der konkrete Resume-Pfad darf keine inkonsistenten Geräte- oder Treiberzustände erzeugen.

## Fehlerbehandlung

Schlägt die Erstellung oder Verifikation des Hibernate Images fehl, darf NovaOS Hybrid Sleep nicht als erfolgreich vorbereitet behandeln.

Je nach Policy kann:

```text
Abort
Fallback to Suspend
Fallback to Hibernate
```

verwendet werden.

Ein solcher Fallback muss explizit erkennbar sein.

## Sicherheit

Hybrid Sleep darf weder Suspend- noch Hibernate-Sicherheitsanforderungen abschwächen.

Insbesondere bleiben:

```text
Authentication
Encryption
Capability State
Trust State
Session Policy
```

über beide Resume-Pfade konsistent geschützt.

## Normative Anforderungen

1. NovaOS MUSS Hybrid Sleep als Kombination aus Suspend und Hibernate modellieren.
2. Hybrid Sleep MUSS von reinem Suspend und reinem Hibernate unterscheidbar bleiben.
3. Vor Suspend MUSS ein gültiges Hibernate Image erzeugt werden.
4. Das Hibernate Image MUSS vor dem Suspend verifiziert werden.
5. Bei erhaltenem Arbeitsspeicher SOLL der schnelle Suspend-Resume-Pfad verwendet werden können.
6. Bei Verlust des flüchtigen Zustands MUSS ein Hibernate Resume möglich sein.
7. Beide Resume-Pfade MÜSSEN denselben logischen Ausgangszustand wiederherstellen.
8. Veraltete Hibernate Images DÜRFEN nicht wiederhergestellt werden.
9. Nach erfolgreichem Resume MUSS das verbleibende Hibernate Image sicher behandelt werden.
10. Fehler bei der Vorbereitung MÜSSEN einen kontrollierten Abbruch oder expliziten Fallback ermöglichen.
11. Hybrid Sleep DARF bestehende Sicherheits-, Trust- oder Verschlüsselungsgrenzen nicht abschwächen.
12. HybridSleepID, Imagezustand, Suspend-Modus, Resume-Pfad und Fehlerzustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-POWER-SUSPEND-0001`
- `NPSPEC-POWER-HIBERNATE-0001`
- `NPSPEC-POWER-DEVICE-0001`
- `NPSPEC-POWER-STORAGE-0001`
- `NPSPEC-SYSTEM-BOOT-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS verbindet mit Hybrid Sleep die schnelle Wiederaufnahme von Suspend mit der Ausfallsicherheit von Hibernate. Bei normalem Wake kann direkt aus dem Arbeitsspeicher fortgesetzt werden, während bei Energieverlust das zuvor verifizierte Hibernate Image die Wiederherstellung derselben Sitzung ermöglicht.