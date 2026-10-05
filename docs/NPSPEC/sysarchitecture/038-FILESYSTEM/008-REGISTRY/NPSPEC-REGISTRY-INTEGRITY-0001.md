# NPSPEC-REGISTRY-INTEGRITY-0001 – Nova Registry Integrity

## Status

Angenommen

## Kategorie

Registry / Integrity

## Zweck

NovaOS definiert die Integritätsprüfung seiner System-Registries.

Beschädigte, unvollständige, manipulierte oder veraltete Registry-Zustände müssen erkannt werden, bevor sie sicherheitskritische oder systemweite Entscheidungen beeinflussen.

## Grundprinzipien

```text
Integrity ≠ Trust
Integrity ≠ Authority
Valid Structure ≠ Trusted Content
Unknown ≠ Valid
Registry ≠ Source of Authority
```

## Integritätszustände

Registry-Zustände können mindestens klassifiziert werden als:

```text
Valid
Stale
Incomplete
Inconsistent
Corrupted
Unknown
```

`Unknown` darf nicht automatisch als `Valid` behandelt werden.

## Prüfmodell

Eine Integritätsprüfung kann umfassen:

```text
Registry Generation
Entry Structure
Entry Identity
Version
References
Dependencies
Checksums / Hashes
Transaction State
Required Metadata
```

Sicherheitskritische Registry-Typen können zusätzliche Prüfungen verlangen.

## Ablauf

```text
Registry State
     ↓
Integrity Check
     ↓
Validate Generation
     ↓
Validate Entries
     ↓
Validate References
     ↓
Valid / Invalid State
```

## Referenzintegrität

Beziehungen zwischen Registry-Einträgen müssen überprüfbar sein.

Beispiele:

```text
Capability → Provider
Service → Interface
Algorithm → Implementation
Codec → Type
Device → Driver
```

Fehlende oder ungültige Referenzen müssen erkannt werden.

## Generationen

Jede veröffentlichte Registry-Generation muss als zusammengehöriger Zustand überprüfbar sein.

```text
Generation N
├── Entries
├── References
├── Integrity Metadata
└── Commit State
```

Unvollständig veröffentlichte Generationen dürfen nicht als gültig verwendet werden.

## Laufende Prüfung

NovaOS darf Registry-Integrität prüfen:

```text
At Boot
After Update
After Recovery
On Access
Periodically
On Suspicious Change
```

Der Prüfzeitpunkt wird durch Registry-Typ, Schutzbedarf und System Policy bestimmt.

## Fehlerbehandlung

Bei erkannter Beschädigung kann NovaOS abhängig vom Umfang:

```text
Reject Entry
Disable Entry
Restore Previous Generation
Rebuild Registry
Enter Degraded Mode
Trigger Recovery
```

Die kleinste ausreichende Maßnahme soll bevorzugt werden.

## Rebuild

Ist eine Registry aus autoritativen Systeminformationen rekonstruierbar, darf sie neu aufgebaut werden.

Ein Rebuild darf keine Berechtigungen, Capability-Tokens oder Trust-Entscheidungen erfinden.

## Sicherheit

Manipulationen an sicherheitskritischen Registry-Daten müssen als Sicherheitsereignis behandelbar sein.

Integritätsprüfung bestätigt lediglich, dass Registry-Daten dem erwarteten Zustand entsprechen; sie ersetzt keine Trust- oder Authority-Prüfung.

## Normative Anforderungen

1. NovaOS MUSS die Integrität seiner System-Registries prüfen können.
2. Registry-Generationen MÜSSEN auf Vollständigkeit prüfbar sein.
3. Registry-Einträge MÜSSEN strukturell validierbar sein.
4. Referenzen zwischen Registry-Einträgen MÜSSEN überprüfbar sein.
5. `Unknown` DARF nicht automatisch als gültiger Integritätszustand gelten.
6. Unvollständige oder beschädigte Generationen DÜRFEN nicht als gültig veröffentlicht werden.
7. Integritätsfehler MÜSSEN kontrolliert isoliert oder repariert werden können.
8. Ein vorheriger gültiger Registry-Zustand MUSS wiederherstellbar sein können.
9. Rekonstruierbare Registries SOLLEN einen Rebuild unterstützen.
10. Rebuild DARF keine Authority oder Trust-Entscheidungen erzeugen.
11. Sicherheitskritische Manipulationen MÜSSEN als Sicherheitsereignis behandelbar sein.
12. Integritätszustand, Generation und erkannte Fehler MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-REGISTRY-0001`
- `NPSPEC-REGISTRY-UPDATE-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-SYSTEM-TRUST-0001`
- `NPSPEC-SYSTEM-RECOVERY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann beschädigte, inkonsistente oder unvollständige Registry-Zustände erkennen und kontrolliert behandeln. Nur validierte Registry-Generationen werden verwendet, während Reparatur, Rebuild oder Recovery ohne Erzeugung neuer Authority möglich bleiben.