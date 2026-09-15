# ADR-UPDATE-0004 – A/B-Systembasis

## Status

Angenommen

## Kategorie

Update Architecture / Boot / Recovery

## Kontext

Kritische NovaOS-Komponenten müssen aktualisiert werden können, ohne die letzte bekannte funktionsfähige Systembasis sofort zu überschreiben.

Dies betrifft insbesondere:

- Kernel,
- Boot-relevante Komponenten,
- grundlegende Systemdienste,
- kritische Treiber,
- zentrale Runtime-Komponenten,
- systemkritische Konfiguration.

Wird die aktive Systembasis direkt überschrieben, kann ein fehlerhaftes Update dazu führen, dass NovaOS nicht mehr regulär startet.

NovaOS benötigt deshalb für kritische Systemupdates eine A/B-fähige Systembasis.

## Entscheidung

NovaOS unterstützt zwei voneinander getrennte bootfähige Systembasen:

```text
System Base A
System Base B
```

Zu einem Zeitpunkt ist eine Basis aktiv, während die andere als Candidate für Updates vorbereitet werden kann.

```text
A = Active
B = Inactive

Update
  ↓
Prepare B
  ↓
Verify B
  ↓
Boot Target → B
  ↓
Reboot
  ↓
Verify B
```

Die aktive Basis wird während der Vorbereitung nicht überschrieben.

## Grundprinzip

```text
Never destroy the last known-good
system base while preparing its replacement.
```

Die A/B-Systembasis dient primär der sicheren Aktualisierung und Recovery kritischer Systemkomponenten.

## Rollen

Eine Systembasis kann verschiedene Rollen besitzen:

```text
Active
Inactive
Candidate
Booting
Verified
Known-Good
Failed
```

Die Rollen sind logische Zustände und nicht zwingend dauerhaft an `A` oder `B` gebunden.

Nach einem erfolgreichen Wechsel kann beispielsweise:

```text
B = Active / Known-Good
A = Inactive
```

gelten.

## Update-Ablauf

Ein typischer A/B-Update-Ablauf lautet:

```text
A = Known-Good
B = Inactive
      ↓
Prepare Update in B
      ↓
Verify B
      ↓
Set B as Candidate
      ↓
Atomic Boot Target Switch
      ↓
Reboot
      ↓
Boot B
      ↓
Health Verification
      ↓
Mark B Known-Good
```

Erst nach erfolgreicher Verification wird die neue Basis als stabil betrachtet.

## Inaktive Basis

Updates werden grundsätzlich auf der inaktiven Basis vorbereitet.

```text
Active A
   │
   └──── running system

Inactive B
   ↓
Update
   ↓
Candidate B
```

Dadurch bleibt die aktive Systembasis während Download, Installation und Vorbereitung unverändert.

## Boot Target

Die Auswahl der nächsten Systembasis muss über einen kleinen, power-failure-sicheren Boot-Control-Zustand erfolgen.

```text
Boot Control
    ↓
A or B
```

Der Wechsel des Boot Targets muss atomar beziehungsweise eindeutig recoverbar sein.

Ein Stromausfall während der Änderung darf keinen undefinierten Bootzustand erzeugen.

## Candidate

Eine vorbereitete Basis wird zunächst als `Candidate` behandelt.

```text
Inactive
   ↓
Prepared
   ↓
Candidate
```

`Candidate` bedeutet ausdrücklich nicht `Known-Good`.

Die Basis muss zunächst erfolgreich gestartet und verifiziert werden.

## Boot Attempt

Für einen Candidate muss NovaOS Boot Attempts begrenzen können.

```text
Candidate B
    ↓
Boot Attempt 1
    ↓
Failure
    ↓
Boot Attempt 2
    ↓
Failure
    ↓
Fallback A
```

Damit wird verhindert, dass ein defekter Candidate eine endlose Boot-Schleife erzeugt.

## Health Milestones

Ein erfolgreicher Kernelstart reicht nicht aus, um eine Basis als funktionsfähig zu markieren.

NovaOS verwendet definierte Health Milestones.

Beispiel:

```text
Bootloader
   ↓
Kernel operational
   ↓
Storage operational
   ↓
Core services operational
   ↓
Security services operational
   ↓
User environment operational
```

Welche Milestones erforderlich sind, wird durch den Update- und Recovery-Contract bestimmt.

## Known-Good

Eine Basis wird erst nach erfolgreicher Verification als `Known-Good` markiert.

```text
Candidate
   ↓
Boot
   ↓
Health Milestones
   ↓
Verification
   ↓
Known-Good
```

Bis dahin muss die vorherige Known-Good-Basis für Recovery verfügbar bleiben.

## Automatischer Fallback

Schlägt der Candidate fehl, kann NovaOS automatisch auf die vorherige Known-Good-Basis zurückfallen.

```text
Boot B
  ↓
Failure
  ↓
B = Failed
  ↓
Boot Target → A
  ↓
Boot A
```

Der Fallback muss selbst power-failure-safe sein.

## Boot-Control-Zustand

Der Boot-Control-Zustand muss mindestens Informationen über folgende Eigenschaften verwalten können:

```text
Active Base
Candidate Base
Known-Good Base
Boot Attempts
Verification State
Failure State
```

Die konkrete Struktur wird durch die Boot-Control-NPSPEC definiert.

## Trennung von System und Benutzerdaten

Die A/B-Systembasis darf nicht dazu führen, dass Benutzerdaten unnötig dupliziert werden.

```text
System A ─┐
          ├── Shared User Data
System B ─┘
```

Systembasis und persistente Benutzerdaten müssen logisch getrennt behandelt werden.

## Gemeinsame Daten

Daten, die von beiden Systembasen verwendet werden, benötigen kompatible Versionierungs- und Migrationsregeln.

Eine neue Systembasis darf gemeinsame Daten nicht irreversibel verändern, solange ein Fallback auf die vorherige Basis erforderlich ist.

## Datenmigration

Ist eine Datenmigration notwendig, muss geprüft werden, ob die vorherige Systembasis danach weiterhin mit den Daten arbeiten kann.

Geeignete Strategien sind beispielsweise:

```text
Backward-Compatible Migration
Versioned Data
Copy-on-Write
Dual-Format Transition
```

Ist die Migration irreversibel, muss eine Rollback Barrier verwendet werden.

## Systembasis und Softwarezustand

Eine A/B-Basis repräsentiert einen konkreten versionierten Systemzustand.

```text
Base A
 └── System State 41

Base B
 └── System State 42
```

Die Basis muss eindeutig dem zugehörigen Desired- und Software-State zugeordnet werden können.

## Integrität

Vor einem Boot muss die ausgewählte Systembasis entsprechend den geltenden Trust- und Boot-Regeln geprüft werden.

Dazu können gehören:

```text
Integrity
Signature
Provenance
Verified Boot
Measured Boot
Trust Policy
```

Eine beschädigte Basis darf nicht allein deshalb gestartet werden, weil sie als Candidate eingetragen ist.

## Secure Boot

Die A/B-Architektur muss mit Secure Boot und Verified Boot kompatibel sein.

Beide Systembasen müssen unabhängig überprüfbar sein.

```text
Boot Target
    ↓
Verify Selected Base
    ↓
Boot
```

Der Boot-Control-Zustand darf die Boot-Vertrauenskette nicht umgehen.

## Power Failure Safety

Ein Stromausfall kann während jeder Updatephase auftreten.

```text
Prepare B
   ↓
Power Failure
```

Die aktive Basis A bleibt davon unberührt.

Auch während:

```text
Boot Target Switch
```

muss der Zustand eindeutig rekonstruierbar bleiben.

Nach einem Neustart muss NovaOS feststellen können:

```text
A valid?
B valid?
Candidate?
Known-Good?
Boot attempt pending?
```

## Transaction Log

Die A/B-Update-Operation muss mit dem Transaction Log integriert sein.

Der Log dokumentiert den Update- und Aktivierungszustand, während der Boot-Control-Zustand die unmittelbar bootrelevante Auswahl enthält.

Beide müssen konsistent reconciliert werden können.

## NovaDOS

NovaDOS muss beide Systembasen offline analysieren können.

Mögliche Operationen sind:

```text
Inspect A
Inspect B
Verify A
Verify B
Select Known-Good
Reset Candidate
Repair Boot Control
```

NovaDOS darf eine beschädigte Basis nicht ungeprüft als Known-Good markieren.

## Manuelle Auswahl

Der Bootmanager kann eine autorisierte manuelle Auswahl einer Systembasis ermöglichen.

```text
Boot A
Boot B
Recovery
NovaDOS
```

Eine manuelle Auswahl verändert nicht automatisch den dauerhaften Known-Good-Status.

## Speicherbedarf

Eine A/B-Systembasis benötigt zusätzlichen Storage.

NovaOS soll deshalb nicht zwangsläufig jede Anwendung oder jeden Benutzerinhalt doppelt speichern.

A/B wird primär für den kritischen Systemzustand verwendet.

## Cleanup

Nach erfolgreicher Stabilisierung der neuen Basis kann die vorherige Basis für das nächste Update vorbereitet werden.

```text
B = Known-Good
A = Previous
      ↓
Retention satisfied
      ↓
A becomes update target
```

Notwendige Recovery-Daten dürfen nicht vorzeitig entfernt werden.

## Normative Anforderungen

1. NovaOS MUSS eine A/B-fähige Systembasis für kritische Systemupdates unterstützen.
2. Die aktive Systembasis DARF während der Vorbereitung ihres Ersatzes nicht überschrieben werden.
3. Eine inaktive Basis MUSS unabhängig aktualisierbar sein.
4. Eine vorbereitete Basis MUSS als Candidate von einer Known-Good-Basis unterscheidbar sein.
5. Der Boot Target Switch MUSS atomar oder eindeutig power-failure-safe recoverbar sein.
6. Ein Candidate DARF nicht allein durch erfolgreichen Bootstart als Known-Good gelten.
7. Kritische Health Milestones MÜSSEN vor Known-Good-Markierung überprüfbar sein.
8. Boot Attempts für einen Candidate MÜSSEN begrenzbar sein.
9. Nach wiederholtem Candidate-Fehler MUSS ein automatischer Fallback auf eine Known-Good-Basis möglich sein.
10. Der Fallback MUSS selbst power-failure-safe sein.
11. Der Boot-Control-Zustand MUSS Active, Candidate, Known-Good und Failure State unterscheiden können.
12. A/B-Systembasen MÜSSEN mit Secure Boot, Verified Boot und Measured Boot integrierbar sein.
13. Beide Systembasen MÜSSEN unabhängig auf Integrität prüfbar sein.
14. Persistente Benutzerdaten SOLLEN nicht unnötig zwischen A und B dupliziert werden.
15. Gemeinsame Daten MÜSSEN während des Fallback-Fensters kompatibel bleiben oder eine explizite Migrationsstrategie besitzen.
16. Irreversible Datenmigrationen MÜSSEN eine Rollback Barrier verwenden.
17. Jede Basis MUSS einem eindeutig identifizierbaren Softwarezustand zugeordnet werden können.
18. A/B-Updates MÜSSEN mit der Transactional Update Pipeline integriert sein.
19. Boot-Control- und Transaction-State MÜSSEN nach einem Crash reconciliierbar sein.
20. NovaDOS MUSS kritische A/B-Zustände offline analysieren und recovern können.
21. Eine manuelle Boot-Auswahl DARF nicht automatisch den Known-Good-Status verändern.
22. Die vorherige Known-Good-Basis DARF nicht entfernt oder überschrieben werden, solange sie für notwendige Recovery benötigt wird.

## Konsequenzen

### Positive Konsequenzen

- fehlerhafte Systemupdates zerstören nicht sofort die letzte funktionierende Basis,
- Updates können vollständig offline zur aktiven Basis vorbereitet werden,
- automatischer Boot-Fallback wird möglich,
- Power Failure während eines Updates ist wesentlich leichter recoverbar,
- Kernel- und systemkritische Updates erhalten eine klare Recovery-Strategie,
- NovaDOS kann beide Systemzustände unabhängig untersuchen.

### Negative Konsequenzen

- zusätzlicher Storage für zwei Systembasen,
- Boot-Control-State muss zuverlässig verwaltet werden,
- gemeinsame Daten benötigen Kompatibilitätsregeln,
- Datenmigrationen werden durch das Fallback-Fenster komplexer.

## Verworfene Alternativen

### Aktive Systembasis direkt überschreiben

Verworfen, da ein fehlgeschlagenes Update die letzte funktionsfähige Basis zerstören könnte.

### Vollständige Verdopplung aller Benutzer- und Anwendungsdaten

Verworfen, da A/B primär die kritische Systembasis schützen soll und vollständige Duplikation unnötig viel Storage verbraucht.

### Candidate nach erstem erfolgreichen Kernelstart als Known-Good markieren

Verworfen, da spätere systemkritische Komponenten weiterhin fehlschlagen können.

### Unbegrenzte Bootversuche des Candidate

Verworfen, da dies zu dauerhaften Boot-Schleifen führen kann.

### A/B ohne Transaction Integration

Verworfen, da Update-, Boot- und Recovery-Zustand dadurch auseinanderlaufen könnten.

## Abhängigkeiten

- `ADR-UPDATE-0001_Deklarativer_Softwarezustand`
- `ADR-UPDATE-0002_Transactional_Update_Pipeline`
- `ADR-UPDATE-0003_Atomic_Updates`
- `ADR-TRANSACTION-0006_Transaction_Log_und_Power_Failure_Safety`
- `ADR-TRANSACTION-0007_Rollback_und_Compensation`
- `ADR-TRANSACTION-0008_Irreversible_Operationen_mit_Rollback_Barrier`
- `ADR-BOOTCONTROL-0001_A_B_Boot_Control`
- `ADR-BOOT-0006_Verified_Boot`
- `ADR-BOOT-0007_Measured_Boot`
- `ADR-BOOT-0008_Secure_Boot`
- `ADR-BOOT-0010_Recovery_Boot`
- `ADR-BOOT-0011_Health_Milestones`
- `ADR-BOOT-0012_Bootloader_Update_Schutz`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0022_NovaDOS_für_Offline_Recovery`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-AB-BASE-0001`
- `NPSPEC-UPDATE-AB-STATE-0001`
- `NPSPEC-UPDATE-AB-BOOT-TARGET-0001`
- `NPSPEC-UPDATE-AB-CANDIDATE-0001`
- `NPSPEC-UPDATE-AB-HEALTH-0001`
- `NPSPEC-UPDATE-AB-FALLBACK-0001`
- `NPSPEC-UPDATE-AB-DATA-COMPATIBILITY-0001`
- `NPSPEC-UPDATE-AB-POWER-FAILURE-0001`
- `NPSPEC-UPDATE-AB-RECOVERY-0001`
- `NPSPEC-UPDATE-AB-TEST-0001`

## Ergebnis

NovaOS hält für kritische Systemupdates eine bekannte funktionsfähige Systembasis verfügbar, während die nächste Version getrennt vorbereitet wird:

```text
        System Base A
        Known-Good
             │
             │ remains intact
             │
        System Base B
          Candidate
             ↓
            Boot
             ↓
      Health Verification
        ┌────┴────┐
        ↓         ↓
      Valid     Failed
        ↓         ↓
   Known-Good   Fallback A
```

Damit wird nicht die gerade laufende Systembasis aktualisiert, sondern eine neue Basis vorbereitet, geprüft und anschließend kontrolliert aktiviert.

Der zentrale Grundsatz lautet:

```text
Die letzte bekannte funktionsfähige
Systembasis bleibt erhalten,
bis ihr Ersatz nachweislich funktioniert.
```