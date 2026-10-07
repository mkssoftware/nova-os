# NPSPEC-TIME-PTP-0001 – Nova Precision Time Protocol

## Status

Angenommen

## Kategorie

Time / Precision Synchronization

## Zweck

NovaOS unterstützt PTP als hochpräzisen Provider für die Synchronisation von Systemen und Geräten innerhalb geeigneter Netzwerke.

PTP ergänzt die allgemeine Time-Synchronization-Architektur und bleibt von Clock Sources, Clock Domains und Wall-Clock-Policy getrennt.

## Grundprinzipien

```text
PTP ≠ Clock Source
PTP ≠ Monotonic Time
PTP ≠ Hardware Timestamping
PTP Master ≠ Automatically Trusted Source
Precision ≠ Accuracy
Synchronization ≠ Syntonization
```

## Architektur

```text
PTP Network
     ↓
PTP Provider
     ↓
Timestamp Processing
     ↓
Delay / Offset Estimation
     ↓
Clock Selection
     ↓
Time Synchronization
     ↓
Wall Clock / PTP Domain
```

## PTP Clock-Modell

NovaOS muss unterschiedliche PTP-Rollen abbilden können:

```text
Ordinary Clock
Boundary Clock
Transparent Clock
Grandmaster Clock
```

Die konkrete Unterstützung hängt vom Netzwerk- und Hardwareprovider ab.

## PTP Domains

Mehrere unabhängige PTP-Domains dürfen gleichzeitig existieren.

```text
PTP Domain 0
PTP Domain 1
PTP Domain N
```

Eine PTP-Domain ist nicht automatisch identisch mit einer Nova Clock Domain.

Die Zuordnung muss explizit erfolgen.

## Synchronisation

PTP bestimmt Offset und Laufzeit zwischen beteiligten Uhren anhand präziser Zeitstempel.

```text
Master
  ↓ Sync
Slave
  ↓
Offset Measurement
  ↓
Delay Measurement
  ↓
Clock Discipline
```

Die resultierende Zeitreferenz wird an die Nova-Time-Synchronisationsschicht übergeben.

## Hardware Timestamping

Wenn unterstützt, soll NovaOS Hardware Timestamping verwenden können:

```text
Network Hardware
      ↓
Hardware Timestamp
      ↓
PTP Provider
```

Dadurch werden variable Verzögerungen durch Software-Stack, Interrupts und Scheduling reduziert.

Software Timestamping bleibt als Fallback zulässig.

## Clock Selection

NovaOS muss PTP-Quellen anhand geeigneter Eigenschaften bewerten können:

```text
Priority
Clock Quality
Accuracy
Variance
Clock Class
Identity
Path Quality
Trust
```

Die Auswahl einer Grandmaster Clock darf nicht allein aufgrund ihrer Erreichbarkeit erfolgen.

## Delay Mechanismen

PTP Provider dürfen unterschiedliche Delay-Verfahren unterstützen:

```text
End-to-End
Peer-to-Peer
```

Die konkrete Auswahl hängt von Netzwerk, Profil und Providerfähigkeiten ab.

## Netzwerkänderungen

Änderungen der Topologie müssen behandelbar sein:

```text
Grandmaster Lost
      ↓
Reevaluation
      ↓
New Master
      ↓
Resynchronization
```

Der Wechsel darf monotone Clock Domains nicht beeinflussen.

## NTP-Koexistenz

PTP und NTP dürfen parallel verfügbar sein.

```text
PTP ─┐
     ├─→ Time Synchronization Policy
NTP ─┘
```

Die Policy entscheidet anhand von Qualität, Verfügbarkeit, Trust und Systemanforderungen über die verwendete Referenz.

## Sicherheit

PTP-Nachrichten sind externe Eingaben und müssen entsprechend validiert werden.

Ein kompromittierter oder fehlerhafter Grandmaster darf nicht ungeprüft sicherheitskritische Zeitentscheidungen kontrollieren.

PTP erzeugt keine zusätzliche Authority.

## Normative Anforderungen

1. NovaOS MUSS PTP als Time-Synchronization-Provider unterstützen können.
2. PTP DARF monotone Clock Domains nicht direkt verändern.
3. Mehrere PTP-Domains MÜSSEN unterstützt werden können.
4. PTP-Domain und Nova Clock Domain MÜSSEN getrennte Konzepte bleiben.
5. Hardware Timestamping SOLL verwendet werden können.
6. Software Timestamping MUSS als Fallback möglich sein.
7. Ordinary, Boundary und Transparent Clocks MÜSSEN architektonisch abbildbar sein.
8. Grandmaster-Auswahl MUSS Qualitätsmerkmale berücksichtigen.
9. End-to-End- und Peer-to-Peer-Delay-Mechanismen MÜSSEN unterstützt werden können.
10. Grandmaster-Wechsel MÜSSEN kontrolliert behandelbar sein.
11. PTP und NTP MÜSSEN parallel als Zeitreferenzen existieren können.
12. Domain, Clock Identity, Grandmaster, Offset, Delay, Timestamp-Modus und Synchronisationszustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-CLOCKSOURCE-0001`
- `NPSPEC-TIME-CLOCKDOMAIN-0001`
- `NPSPEC-TIME-CALIBRATION-0001`
- `NPSPEC-TIME-SYNCHRONIZATION-0001`
- `NPSPEC-TIME-NTP-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS integriert PTP als präzisen und hardwarebeschleunigbaren Zeit-Synchronisationsprovider. PTP-Domains, Grandmaster-Auswahl, Hardware Timestamping und Netzwerkänderungen bleiben kontrollierbar, während die allgemeine Nova-Time-Architektur über die tatsächliche Nutzung der gewonnenen Zeitreferenz entscheidet.