# NPSPEC-TIME-MEDIA-0001 – Nova Media Time

## Status

Angenommen

## Kategorie

Time / Media

## Zweck

NovaOS definiert eine gemeinsame Zeitbasis für Audio-, Video-, Animation-, Streaming- und andere zeitabhängige Medienpipelines.

Media Time bleibt von Wall Clock, CPU-Zeit und tatsächlicher Ausführungsdauer getrennt und ermöglicht präzise Synchronisation mehrerer Medienströme.

## Grundprinzipien

```text
Media Time ≠ Wall Clock
Media Time ≠ CPU Time
Timestamp ≠ Presentation Time
Decode Time ≠ Presentation Time
Frame Number ≠ Time
Playback Rate ≠ Clock Frequency
```

## Modell

```text
MediaClock
├── MediaClockID
├── ClockDomainID
├── TimeBase
├── CurrentPosition
├── PlaybackRate
├── State
├── MasterClock
└── SynchronizationState
```

## Zeitbasis

Medienzeit muss unabhängig von einer festen Einheit darstellbar sein.

```text
Media Timestamp
      ↓
Time Base
      ↓
Media Time
```

Beispiele:

```text
1 / 48,000 s
1 / 90,000 s
1 / FrameRate
Nanoseconds
```

Umrechnungen müssen Überlauf und unnötigen Präzisionsverlust vermeiden.

## Presentation und Decode Time

NovaOS unterscheidet:

```text
PTS = Presentation Timestamp
DTS = Decode Timestamp
```

Die Dekodierreihenfolge darf von der Präsentationsreihenfolge abweichen.

## Master Clock

Eine Medienpipeline muss eine führende Zeitbasis bestimmen können:

```text
Audio Clock
Video Clock
External Clock
System Monotonic Clock
Device Clock
Network Media Clock
```

Andere Streams werden gegen diese Master Clock synchronisiert.

## Audio/Video-Synchronisation

```text
Master Clock
     ↓
Current Media Time
     ↓
Audio PTS ← Compare → Video PTS
     ↓
Synchronization Correction
```

Kleine Abweichungen dürfen durch kontrollierte Anpassungen ausgeglichen werden.

## Playback

Unterstützte Zustände:

```text
Stopped
Playing
Paused
Seeking
Buffering
Completed
```

`Paused` friert die sichtbare Media Time ein, ohne globale Clock Domains zu verändern.

## Playback Rate

Medienzeit darf skaliert werden:

```text
0.5×
1.0×
1.5×
2.0×
```

Die Playback Rate verändert die Abbildung zwischen zugrunde liegender Clock Domain und Media Time.

## Seeking

Ein Seek erzeugt eine kontrollierte Diskontinuität:

```text
Current Position
      ↓
Seek
      ↓
New Position
      ↓
Pipeline Resynchronization
```

Decoder, Buffer, Untertitel und abhängige Streams müssen anschließend neu synchronisierbar sein.

## Drift

Geräte können voneinander abweichende physische Taktquellen besitzen.

```text
Audio Device Clock
        ↕
Video / System Clock
        ↓
Drift Detection
        ↓
Correction
```

Korrekturen dürfen beispielsweise Resampling, Frame Scheduling oder kontrollierte Zeitbasisanpassungen verwenden.

## Timer und Deadlines

Medienausgabe darf Deadlines verwenden:

```text
PTS
 ↓
Presentation Deadline
 ↓
Decode / Render / Output
```

Execution Contracts können Latenz-, Deadline- und Ressourcenanforderungen definieren.

## Verteilte Medien

Netzwerk- oder geräteübergreifende Wiedergabe darf synchronisierte Zeitreferenzen wie PTP verwenden.

Unsicherheit zwischen Geräten muss dabei berücksichtigt werden.

## Normative Anforderungen

1. NovaOS MUSS eine eigenständige Media-Time-Abstraktion unterstützen.
2. Media Time MUSS von Wall Clock und CPU-Zeit getrennt bleiben.
3. Unterschiedliche Media Time Bases MÜSSEN unterstützt werden.
4. PTS und DTS MÜSSEN getrennt darstellbar sein.
5. Eine Medienpipeline MUSS eine Master Clock bestimmen können.
6. Mehrere Medienströme MÜSSEN gegen eine gemeinsame Master Clock synchronisierbar sein.
7. Playback Rate MUSS unabhängig von der physischen Clock-Frequenz modelliert werden.
8. Pause und Seek MÜSSEN kontrollierte Änderungen der Media Time ermöglichen.
9. Clock Drift zwischen Geräten MUSS erkennbar und kompensierbar sein können.
10. Media Deadlines MÜSSEN mit der allgemeinen Deadline-Infrastruktur integrierbar sein.
11. Verteilte Medienwiedergabe MUSS externe Synchronisationsreferenzen verwenden können.
12. MediaClockID, Zeitbasis, Position, Rate, Master Clock, Drift und Synchronisationszustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-MONOTONIC-0001`
- `NPSPEC-TIME-CLOCKDOMAIN-0001`
- `NPSPEC-TIME-DEADLINE-0001`
- `NPSPEC-TIME-SYNCHRONIZATION-0001`
- `NPSPEC-TIME-PTP-0001`
- `NPSPEC-TIME-DISTRIBUTED-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine gemeinsame Media-Time-Infrastruktur für Audio, Video, Animation und Streaming. Unterschiedliche Medienzeitbasen, PTS/DTS, Master Clocks, Playback Rate, Seeking und Drift-Korrektur können einheitlich behandelt werden, ohne Medienzeit mit Wall Clock oder physischer Ausführungszeit zu vermischen.