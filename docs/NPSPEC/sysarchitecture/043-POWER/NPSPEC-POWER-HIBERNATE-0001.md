# NPSPEC-POWER-HIBERNATE-0001 – Nova System Hibernate

## Status

Angenommen

## Kategorie

Power / Hibernate

## Zweck

NovaOS definiert Hibernate als kontrollierten Systemzustand, bei dem der für die Wiederaufnahme erforderliche Laufzeitzustand persistent gespeichert und das System anschließend vollständig oder nahezu vollständig abgeschaltet wird.

Hibernate ermöglicht die Wiederherstellung einer vorherigen Sitzung ohne dauerhaft mit Energie versorgten Arbeitsspeicher.

## Grundprinzipien

```text
Hibernate ≠ Suspend
Hibernate ≠ Shutdown
Hibernate ≠ Snapshot Backup
Hibernate Image ≠ Normal User Data
Resume ≠ Normal Boot
Stored State ≠ Trusted State
Power Loss ≠ State Loss
```

## Modell

```text
HibernateContext
├── HibernateID
├── ImageID
├── SystemIdentity
├── KernelVersion
├── MemoryState
├── DeviceState
├── IntegrityMetadata
├── EncryptionState
└── ResumeState
```

## Hibernate-Ablauf

```text
Hibernate Request
       ↓
Validate
       ↓
Quiesce System
       ↓
Freeze Tasks
       ↓
Flush I/O
       ↓
Suspend Devices
       ↓
Capture State
       ↓
Write Hibernate Image
       ↓
Verify Image
       ↓
Power Off
```

Das System darf erst abgeschaltet werden, nachdem das Hibernate Image erfolgreich persistiert und validiert wurde.

## Hibernate Image

Das Hibernate Image enthält ausschließlich den für eine Wiederaufnahme erforderlichen Systemzustand.

Es kann insbesondere enthalten:

```text
Memory State
Kernel State
Process State
Task State
Selected Device State
Resume Metadata
```

Temporäre oder rekonstruierbare Daten dürfen ausgelassen werden, sofern ihre Wiederherstellung garantiert möglich ist.

## Persistenz

Das Hibernate Image muss auf einem geeigneten persistenten Storage-Ziel gespeichert werden.

```text
Memory State
     ↓
Hibernate Writer
     ↓
Persistent Storage
```

Speicherort und physische Repräsentation dürfen vom Hibernate-Modell abstrahiert bleiben.

## Sicherheit

Ein Hibernate Image kann sensible Inhalte des Arbeitsspeichers enthalten.

Daher muss NovaOS dessen Schutz berücksichtigen:

```text
Confidentiality
Integrity
Authenticity
Anti-Tampering
Secure Erasure
```

Bei aktivierter Systemverschlüsselung darf Hibernate deren Sicherheitsmodell nicht umgehen.

## Resume

```text
Boot
 ↓
Hibernate Image Discovery
 ↓
Identity / Compatibility Check
 ↓
Integrity Verification
 ↓
Restore Memory State
 ↓
Restore Devices
 ↓
Restore Tasks
 ↓
Verify
 ↓
Running System
```

Ein vorhandenes Hibernate Image darf nicht automatisch als gültig betrachtet werden.

## Kompatibilität

Vor Resume müssen mindestens relevante Änderungen an:

```text
Kernel
System Components
Hardware
Firmware
Memory Layout
Security State
```

berücksichtigt werden.

Ein inkompatibles Image muss verworfen oder über einen sicheren normalen Boot umgangen werden können.

## Geräte

Nicht jeder Gerätezustand muss vollständig gespeichert werden.

Geräte dürfen beim Resume neu initialisiert und anschließend mit dem wiederhergestellten Systemzustand synchronisiert werden.

## Fehlerbehandlung

Ist das Hibernate Image:

```text
Missing
Incomplete
Corrupted
Invalid
Incompatible
Untrusted
```

darf kein unsicherer Resume durchgeführt werden.

Fallback:

```text
Resume unavailable
      ↓
Invalidate Hibernate Image
      ↓
Normal Boot / Recovery
```

## Image Lifecycle

Ein erfolgreich verwendetes Hibernate Image darf nicht unbegrenzt erneut verwendet werden.

NovaOS muss Replay und veraltete Images verhindern können.

Nach erfolgreichem Resume oder bewusster Verwerfung muss das Image sicher invalidiert werden.

## Normative Anforderungen

1. NovaOS MUSS Hibernate von Suspend und Shutdown trennen.
2. Hibernate MUSS den erforderlichen Laufzeitzustand persistent speichern können.
3. Das System DARF erst nach erfolgreicher Persistierung und Verifikation abgeschaltet werden.
4. Hibernate Images MÜSSEN Integritätsschutz unterstützen.
5. Sensible Hibernate-Daten MÜSSEN vertraulich geschützt werden können.
6. Hibernate DARF bestehende Systemverschlüsselung nicht umgehen.
7. Resume MUSS Image-Identität, Integrität und Kompatibilität prüfen.
8. Ungültige oder inkompatible Images DÜRFEN nicht wiederhergestellt werden.
9. Geräte DÜRFEN beim Resume neu initialisiert werden.
10. Fehlgeschlagener Resume MUSS einen sicheren normalen Boot oder Recovery ermöglichen.
11. Veraltete oder bereits konsumierte Images MÜSSEN gegen Replay geschützt und invalidierbar sein.
12. Hibernate Image, Zustand, Validierung, Resume-Ergebnis und Fehler MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-POWER-SUSPEND-0001`
- `NPSPEC-POWER-DEVICE-0001`
- `NPSPEC-POWER-STORAGE-0001`
- `NPSPEC-FILESYSTEM-TRANSACTION-0001`
- `NPSPEC-SYSTEM-BOOT-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann den laufenden Systemzustand sicher persistent speichern, das System abschalten und diesen Zustand bei einem späteren Start wiederherstellen. Hibernate Images werden als sicherheitskritische, validierte Zustandsartefakte behandelt und dürfen nur bei bestätigter Integrität, Vertrauenswürdigkeit und Kompatibilität für einen Resume verwendet werden.