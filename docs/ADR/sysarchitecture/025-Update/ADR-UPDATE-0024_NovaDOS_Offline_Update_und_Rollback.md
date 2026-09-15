# ADR-UPDATE-0024 – NovaDOS Offline Update und Rollback

## Status

Angenommen

## Kategorie

Update / Recovery / NovaDOS

## Kontext

Das normale NovaOS kann so beschädigt sein, dass die reguläre Update- und Recovery-Infrastruktur nicht mehr zuverlässig ausgeführt werden kann.

Für solche Fälle benötigt NovaOS einen unabhängigen Offline-Pfad zum Aktualisieren und Zurücksetzen kritischer Systemzustände.

## Entscheidung

NovaDOS unterstützt **Offline Update und Rollback** für NovaOS.

```text
Boot Manager
     ↓
NovaDOS
     ↓
Inspect System
     ↓
Update / Rollback
     ↓
Verify
     ↓
Reboot
```

NovaDOS arbeitet dabei unabhängig von der laufenden NovaOS-Systembasis.

## Offline Update

NovaDOS kann einen neuen Systemzustand vorbereiten und aktivieren.

```text
Current Generation
       ↓
NovaDOS
       ↓
Verify Update
       ↓
Prepare New Generation
       ↓
Atomic Switch
```

Die regulären Integritäts- und Trust-Anforderungen bleiben bestehen.

## Offline Rollback

NovaDOS kann auf einen vorhandenen zulässigen Known-Good-Zustand zurücksetzen.

```text
Broken State
     ↓
NovaDOS
     ↓
Select Known-Good
     ↓
Verify
     ↓
Rollback
```

Anti-Rollback-Grenzen müssen dabei berücksichtigt werden.

## Verifikation

Vor Update oder Rollback prüft NovaDOS mindestens:

```text
Content Integrity
System State
Target Compatibility
Trust Evidence
Anti-Rollback State
```

Ein beschädigter oder nicht zulässiger Zielzustand darf nicht aktiviert werden.

## A/B und Generationen

NovaDOS muss vorhandene A/B-Systembasen und Systemgenerationen erkennen können.

```text
Generation A → Known-Good
Generation B → Failed
```

Dadurch kann NovaDOS einen gültigen Zustand auswählen, ohne das beschädigte NovaOS starten zu müssen.

## Power-Failure-Safety

Offline-Operationen müssen dieselben Anforderungen an transaktionale und power-loss-safe Zustandswechsel erfüllen wie reguläre Updates.

Ein Stromausfall während NovaDOS-Recovery darf keinen undefinierten Generation State erzeugen.

## Benutzerkontrolle

Kritische Offline-Operationen müssen klar anzeigen:

```text
Current State
Target State
Operation
Recovery Impact
```

Destruktive oder irreversible Schritte benötigen eine explizite Bestätigung.

## Normative Anforderungen

1. NovaDOS MUSS NovaOS offline aktualisieren und zurücksetzen können.
2. NovaDOS MUSS Systemgenerationen und A/B-Zustände erkennen können.
3. Update- und Rollback-Ziele MÜSSEN vor Aktivierung verifiziert werden.
4. NovaDOS MUSS Content-, Trust- und Anti-Rollback-Regeln berücksichtigen.
5. Offline-Updates MÜSSEN transaktional und power-loss-safe erfolgen.
6. Unvollständige Systemgenerationen DÜRFEN nicht aktiviert werden.
7. Rollback DARF keine gültige Anti-Rollback-Grenze unterschreiten.
8. Kritische oder irreversible Operationen MÜSSEN explizit bestätigt werden.
9. NovaDOS DARF die reguläre NovaOS-Systembasis für seine Kern-Recovery-Funktion nicht benötigen.

## Konsequenzen

### Positive Konsequenzen

- Updates und Rollbacks bleiben auch bei beschädigtem NovaOS möglich.
- Recovery ist unabhängig vom laufenden Hauptsystem.
- A/B- und Generation-Recovery kann offline durchgeführt werden.
- schwerwiegende Updatefehler benötigen keine Neuinstallation.

### Negative Konsequenzen

- NovaDOS benötigt eigene Update- und Storage-Unterstützung.
- Recovery-relevante Trust- und Metadaten müssen offline verfügbar sein.

## Abhängigkeiten

- `ADR-UPDATE-0004_A_B_Systembasis`
- `ADR-UPDATE-0005_Snapshot_Based_Rollback`
- `ADR-UPDATE-0020_Anti_Rollback_Minimum_Version`
- `ADR-UPDATE-0022_Boot_Health_vor_Update_Commit`
- `ADR-UPDATE-0023_Power_Loss_Safe_Generation_Switch`
- `ADR-RESILIENCE-0022_NovaDOS_für_Offline_Recovery`
- `ADR-TRANSACTION-0006_Transaction_Log_und_Power_Failure_Safety`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-NOVADOS-OFFLINE-0001`
- `NPSPEC-UPDATE-NOVADOS-ROLLBACK-0001`
- `NPSPEC-UPDATE-NOVADOS-VERIFICATION-0001`

## Ergebnis

NovaDOS bildet den unabhängigen Offline-Pfad für Update und Rollback:

```text
NovaOS unavailable
       ↓
NovaDOS
       ↓
Inspect + Verify
       ↓
Update / Rollback
       ↓
Atomic Generation Switch
       ↓
Reboot
```

Der zentrale Grundsatz lautet:

```text
NovaOS muss reparierbar bleiben,
auch wenn NovaOS selbst
nicht mehr gestartet werden kann.
```