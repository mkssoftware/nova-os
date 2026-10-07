# NPSPEC-TIME-NTP-0001 – Nova Network Time Protocol

## Status

Angenommen

## Kategorie

Time / Network Synchronization

## Zweck

NovaOS unterstützt NTP als Netzwerkprovider für die Synchronisation der Wall Clock mit externen Zeitreferenzen.

NTP ist ein Provider der allgemeinen Nova-Time-Synchronisation und darf weder Clock Sources noch monotone Clock Domains direkt verändern.

## Grundprinzipien

```text
NTP ≠ System Clock
NTP ≠ Clock Source
NTP ≠ Monotonic Time
NTP ≠ RTC
NTP Sample ≠ Trusted Time
Network Delay ≠ Clock Offset
Reachable Server ≠ Reliable Server
```

## Architektur

```text
NTP Servers
     ↓
NTP Provider
     ↓
Sample Validation
     ↓
Offset / Delay Estimation
     ↓
Reference Selection
     ↓
NPSPEC-TIME-SYNCHRONIZATION
     ↓
Wall Clock
```

NTP bestimmt nicht selbst, wie NovaOS eine Zeitkorrektur letztlich durchführt.

## Servermodell

```text
NTPPeer
├── PeerID
├── Address
├── Stratum
├── Reachability
├── RoundTripDelay
├── ClockOffset
├── Dispersion
├── Jitter
├── LastSample
└── State
```

NovaOS soll mehrere unabhängige NTP-Server verwenden können.

## Zeitmessung

Eine NTP-Abfrage verwendet die vier relevanten Zeitpunkte:

```text
T1 → Request sent
T2 → Request received by server
T3 → Response sent by server
T4 → Response received
```

Daraus können Netzwerkverzögerung und Clock Offset geschätzt werden.

## Auswahl

Mehrere Samples und Peers müssen bewertet werden anhand von:

```text
Reachability
Stratum
Delay
Dispersion
Jitter
Consistency
Sample Age
Historical Stability
Trust Policy
```

Ein einzelner erreichbarer Server darf nicht automatisch als beste Zeitquelle gelten.

## Ausreißer

NovaOS muss stark abweichende oder inkonsistente Samples erkennen und aus der Synchronisationsentscheidung ausschließen können.

```text
Samples
   ↓
Validation
   ↓
Outlier Rejection
   ↓
Candidate Set
   ↓
Reference Selection
```

## Synchronisation

Der NTP Provider liefert eine validierte Zeitreferenz an die allgemeine Synchronisationsschicht.

```text
NTP Reference
      ↓
Synchronization Policy
      ↓
Slew / Step / Holdover
```

Die Entscheidung zwischen Slew und Step erfolgt nicht im NTP-Protokollmodul selbst.

## Polling

Abfrageintervalle dürfen dynamisch angepasst werden.

```text
Unstable Clock → shorter interval
Stable Clock   → longer interval
```

Dabei müssen Netzwerk-, Energie- und Synchronisationsanforderungen berücksichtigt werden.

## Netzwerkverlust

Bei fehlender NTP-Verbindung:

```text
NTP unavailable
      ↓
Synchronization Holdover
      ↓
Local Clock + Drift Estimate
      ↓
NTP recovered
      ↓
Resynchronize
```

Der Ausfall von NTP darf monotone Zeitmessung nicht beeinträchtigen.

## Sicherheit

NTP-Daten müssen als externe Eingabe behandelt werden.

NovaOS soll authentisierte oder anderweitig vertrauenswürdig abgesicherte Zeitquellen unterstützen können.

Unplausible Zeitänderungen müssen erkannt und dürfen nicht ungeprüft übernommen werden.

NTP erzeugt keine zusätzliche Authority.

## Datenschutz

NTP-Anfragen können Informationen über Netzwerkaktivität und Systembetrieb offenlegen.

Serverauswahl und Kommunikationsverhalten müssen daher durch System- und Datenschutzrichtlinien steuerbar sein.

## Normative Anforderungen

1. NovaOS MUSS NTP als Time-Synchronization-Provider unterstützen können.
2. NTP DARF monotone Clock Domains nicht direkt verändern.
3. Mehrere NTP-Peers MÜSSEN unterstützt werden können.
4. Delay, Offset, Dispersion und Jitter MÜSSEN getrennt bewertet werden können.
5. Einzelne Samples DÜRFEN nicht ungeprüft übernommen werden.
6. Ausreißer und inkonsistente Quellen MÜSSEN erkennbar sein.
7. Peer-Auswahl MUSS mehrere Qualitätsmerkmale berücksichtigen.
8. Polling-Intervalle MÜSSEN dynamisch anpassbar sein können.
9. Netzwerkverlust MUSS einen Holdover-Betrieb ermöglichen.
10. Slew- und Step-Entscheidungen MÜSSEN durch die allgemeine Synchronisationsschicht erfolgen.
11. Sicherheits- und Datenschutzrichtlinien MÜSSEN auf NTP anwendbar sein.
12. Peer, Stratum, Delay, Offset, Jitter, Dispersion, Sample-Alter und Synchronisationszustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TIME-ARCH-0001`
- `NPSPEC-TIME-CLOCKSOURCE-0001`
- `NPSPEC-TIME-CALIBRATION-0001`
- `NPSPEC-TIME-SYNCHRONIZATION-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS integriert NTP als austauschbaren Netzwerk-Zeitprovider. Mehrere Server können bewertet, verglichen und gefiltert werden, bevor eine Zeitreferenz an die allgemeine Synchronisationsschicht übergeben wird. Netzwerkfehler oder fehlerhafte NTP-Quellen beeinträchtigen weder die monotone Zeitbasis noch die grundsätzliche Funktionsfähigkeit des Systems.