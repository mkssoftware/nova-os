# ADR-BOOT-0012 – Bootloader-Updates mit besonderem Schutz

## Status

Angenommen

## Kategorie

Boot / Bootloader / Updates / Security / Trust / Recovery / Resilienz

## Kontext

Der Bootloader gehört zu den sicherheitskritischsten Komponenten von NovaOS.

Ein fehlerhaftes Update kann dazu führen, dass:

- NovaOS nicht mehr startet,
- Recovery nicht mehr erreichbar ist,
- die Trusted Boot Chain unterbrochen wird,
- Secure Boot nicht mehr funktioniert,
- falsche Bootartefakte geladen werden,
- Bootmetadaten beschädigt werden.

Anders als normale Systemkomponenten kann ein defekter Bootloader verhindern, dass überhaupt noch höhere Recoverymechanismen erreicht werden.

Deshalb dürfen Bootloader-Updates nicht wie gewöhnliche Dateiupdates behandelt werden.

Sie benötigen strengere Regeln für:

- Verifikation,
- Staging,
- Aktivierung,
- Fallback,
- Kompatibilität,
- Recovery,
- Commit.

---

## Entscheidung

NovaOS behandelt Bootloader-Updates als **besonders geschützte transaktionale Systemoperationen**.

Ein neuer Bootloader wird grundsätzlich:

```text
Stage
  ↓
Verify
  ↓
Validate Compatibility
  ↓
Prepare Fallback
  ↓
Activate Atomically
  ↓
Boot Test
  ↓
Commit
```

Die bisherige funktionsfähige Bootloader-Generation bleibt erhalten, bis die neue Version erfolgreich verwendet wurde.

Die zentrale Regel lautet:

```text
Never replace the only known-working boot path
before the new one has proven bootable.
```

---

## Schutzklasse

Bootloader-Updates erhalten eine höhere Schutzklasse als normale Systemupdates.

Sie dürfen insbesondere nicht:

- ungeprüft installiert,
- direkt überschrieben,
- ohne Recoverypfad aktiviert,
- durch normale Anwendungen durchgeführt

werden.

Nur autorisierte Systemkomponenten mit expliziter Boot-Update-Capability dürfen den Vorgang ausführen.

---

## Generationenmodell

Bootloader sollen versioniert und generationenfähig verwaltet werden.

Konzeptionell:

```text
Bootloader Generation N
    KnownGood

Bootloader Generation N+1
    Candidate
```

Die konkrete physische Umsetzung kann sich nach Firmwaretyp unterscheiden.

---

## UEFI

Auf UEFI-Systemen kann die neue EFI-Bootloader-Version separat gestaged werden.

Beispiel:

```text
EFI/
 ├── nova-current.efi
 └── nova-candidate.efi
```

Die tatsächliche Benennung ist implementationsspezifisch.

Die Aktivierung erfolgt erst nach vollständiger Verifikation.

---

## BIOS

Auf BIOS-Systemen ist der verfügbare frühe Bootbereich stärker begrenzt.

Deshalb muss die BIOS-Implementierung einen separaten sicheren Updatepfad definieren.

Insbesondere darf ein kritischer früher Loader nicht ohne Recoverystrategie blind überschrieben werden.

Mögliche Mechanismen sind:

- redundante Bootbereiche,
- minimaler stabiler Stage-1-Loader,
- austauschbare spätere Bootstufen,
- bekannte Recovery-Sektoren.

Die konkrete physische Implementierung wird in NPSPECs festgelegt.

---

## Stabiler Minimal Loader

Wo technisch sinnvoll, soll ein möglichst kleiner und selten veränderter früher Loader verwendet werden.

Das Modell lautet:

```text
Firmware
   ↓
Minimal Stable Loader
   ↓
Selectable Bootloader Generation
```

Dadurch müssen komplexere Bootloader-Komponenten nicht zwangsläufig den kritischsten Einstiegscode ersetzen.

---

## Verifikation

Vor Aktivierung muss der neue Bootloader mindestens auf folgende Eigenschaften geprüft werden:

- Signatur,
- Integrität,
- Architektur,
- Firmwarekompatibilität,
- Boot-Protocol-Kompatibilität,
- unterstützte BootInfo-Version,
- Trust Policy,
- erforderliche Plattformfähigkeiten.

Ein syntaktisch gültiges Image reicht nicht aus.

---

## Trust

Bootloader-Updates sind Bestandteil der Trusted Boot Chain.

Ein Candidate muss von einem dafür autorisierten Signierer stammen.

Es gilt:

```text
Valid signature
≠
automatic update permission
```

Zusätzlich muss die aktive Boot Trust Policy die neue Generation autorisieren.

---

## Secure Boot

Auf UEFI-Systemen mit Secure Boot muss der neue EFI-Loader durch die relevante Secure-Boot-Policy autorisiert werden können.

NovaOS darf keinen Bootloader aktivieren, der den nächsten Neustart unter der aktuellen Firmwarepolicy vorhersehbar blockiert.

---

## Kompatibilitätsprüfung

Vor Aktivierung muss NovaOS prüfen, ob die neue Bootloader-Generation mit dem installierten System kompatibel ist.

Dabei können insbesondere relevant sein:

```text
Boot Protocol
BootInfo ABI
Kernel Image Format
Manifest Format
Trust Metadata
Recovery Format
```

Inkompatible Kombinationen dürfen nicht ohne passenden gemeinsamen Updateplan aktiviert werden.

---

## Atomare Aktivierung

Die Auswahl der aktiven Bootloader-Generation muss crash-konsistent erfolgen.

Ein Stromausfall während der Umschaltung darf nicht dazu führen, dass kein definierter Bootpfad mehr existiert.

Die Aktivierung muss entweder:

```text
old generation active
```

oder:

```text
new generation active
```

ergeben.

Ein undefinierter Zwischenzustand ist nicht zulässig.

---

## Candidate Boot

Nach Aktivierung wird die neue Bootloader-Generation zunächst als Candidate verwendet.

Sie gilt noch nicht automatisch als Known-Good.

Der Bootpfad muss mindestens soweit erfolgreich sein, dass ein definierter Boot Health Marker erreicht wird.

---

## Bootloader Health

Für Bootloader-Updates gelten eigene minimale Health-Kriterien.

Beispiele:

```text
Firmware Entry successful
Boot Core reached
Boot configuration readable
Kernel verified
Kernel handoff successful
```

Die genaue Commit-Grenze wird durch die Bootloader-Update-Spezifikation definiert.

---

## Commit

Erst nach erfolgreichem Candidate-Boot darf die neue Bootloader-Generation als Known-Good markiert werden.

Beispiel:

```text
Bootloader N+1
Candidate
    ↓
Successful boot path
    ↓
KnownGood
```

Die vorherige Generation darf erst danach gemäß Retention Policy freigegeben werden.

---

## Fallback

Schlägt die Candidate-Generation fehl, muss die vorherige Known-Good-Bootloader-Generation weiterhin erreichbar sein.

Konzeptionell:

```text
Bootloader N+1
    ↓ failure

Bootloader N
    ↓ fallback
```

Dieser Fallback muss möglichst ohne Abhängigkeit von der defekten Candidate-Version funktionieren.

---

## Recovery-Unabhängigkeit

Ein Bootloader-Fallback darf nicht vollständig davon abhängen, dass der fehlerhafte neue Bootloader selbst seine eigene Beschädigung korrekt erkennt.

Wo möglich, soll die Auswahl von Known-Good durch eine niedrigere oder separate vertrauenswürdige Ebene erfolgen.

---

## Recovery und NovaDOS

Wenn die reguläre Bootloader-Generation nicht funktioniert, soll ein autorisierter Recoverypfad erreichbar bleiben.

Dieser kann je nach Plattform zu:

- Recovery Bootloader,
- Known-Good Bootloader,
- NovaDOS,
- Firmware Boot Manager

führen.

NovaDOS kann Bootloader-Metadaten diagnostizieren und autorisierte Reparaturen durchführen.

---

## Bootloader und A/B-Systemgenerationen

Bootloader-Generation und NovaOS-Systemgeneration sind getrennte Zustände.

Beispiel:

```text
Bootloader Generation:
    7

System Generation:
    42
```

Ein Systemupdate darf nicht stillschweigend voraussetzen, dass beide Generationen identisch fortschreiten.

---

## Gemeinsame Update-Transaktionen

Falls eine neue Systemgeneration einen neuen Bootloader zwingend benötigt, müssen beide Komponenten als koordinierter Updateplan behandelt werden.

Beispiel:

```text
Stage Bootloader N+1
Stage System Generation 43
Verify both
Validate compatibility
Activate coordinated candidate
Boot
Health validate
Commit
```

Ein halb aktualisiertes inkompatibles Paar ist zu vermeiden.

---

## Boot Protocol Evolution

Neue Bootloader-Versionen müssen mindestens die für den Updateübergang erforderliche Boot-Protocol-Kompatibilität besitzen.

Eine inkompatible Änderung benötigt eine explizite Migrationsstrategie.

---

## Trust Root Änderungen

Bootloader-Updates dürfen nicht automatisch Trust Roots ersetzen.

Änderungen an:

- Boot Signing Keys,
- Root Keys,
- Revocation State

sind eigene besonders geschützte Trust-Operationen.

Sie können Teil eines koordinierten Updates sein, dürfen aber nicht implizit erfolgen.

---

## Anti-Rollback

Security Anti-Rollback kann auch Bootloader-Generationen betreffen.

Dabei gilt:

```text
No unauthorized downgrade
```

aber weiterhin:

```text
Authorized Known-Good fallback remains possible
```

Eine ältere Bootloader-Generation darf nur zurückverwendet werden, wenn sie noch durch die aktuelle Security Policy autorisiert ist.

---

## Bootmetadaten

Der Zustand der Bootloader-Generationen muss minimal und crash-konsistent gespeichert werden.

Konzeptionell:

```text
BootloaderState {
    activeGeneration
    candidateGeneration
    knownGoodGeneration
    attemptCount
    lastResult
}
```

Die konkrete Struktur ist implementationsspezifisch.

---

## Stromausfall

Bootloader-Updates müssen explizit gegen Stromausfall getestet werden.

Kritische Zeitpunkte sind insbesondere:

```text
during staging
during metadata update
during activation
during first candidate boot
during commit
```

Nach jedem dieser Fälle muss ein definierter Boot- oder Recoverypfad erhalten bleiben.

---

## Schreibschutz

Nicht aktive Bootloader-Generationen sollen während des normalen Betriebs gegen unbeabsichtigte Änderungen geschützt sein.

Schreibzugriff wird nur innerhalb einer autorisierten Bootloader-Update-Transaktion erlaubt.

---

## Firmwarevariablen

UEFI-Variablen oder vergleichbare Firmwarezustände dürfen für die Bootauswahl verwendet werden.

Ihre Änderung muss jedoch ebenfalls:

- validiert,
- kontrolliert,
- auditierbar

sein.

NovaOS darf Firmwarevariablen nicht als unbegrenzt zuverlässigen oder transaktionalen Speicher voraussetzen.

---

## Architekturintrospektion

NovaOS soll mindestens darstellen können:

```text
Bootloader:
    Generation 7

State:
    KnownGood

Firmware:
    UEFI

Fallback:
    Generation 6

Last Update:
    Successful
```

Bei einem Candidate:

```text
Generation 8
State: Candidate
PreviousKnownGood: 7
```

---

## Audit

Mindestens folgende Ereignisse müssen nachvollziehbar sein:

```text
bootloader update staged
signature verified
compatibility validated
candidate activated
candidate boot attempted
candidate committed
candidate failed
fallback selected
bootloader rollback performed
```

---

## Keine normale App-Autorität

Anwendungen dürfen Bootloaderdateien oder Bootloader-State nicht direkt verändern.

Änderungen erfolgen ausschließlich über einen kontrollierten Systemdienst beziehungsweise Recoverymechanismus.

---

## KI-Unabhängigkeit

Bootloader-Update-Entscheidungen müssen vollständig ohne KI funktionieren.

KI darf optional:

- Updatefehler erklären,
- Logs analysieren,
- Recoveryvorschläge machen.

KI darf nicht:

- Signaturprüfung umgehen,
- Kandidaten ohne Trust-Freigabe aktivieren,
- Known-Good willkürlich setzen,
- Anti-Rollback-Regeln überschreiben.

---

## Normative Anforderungen

1. NovaOS MUSS Bootloader-Updates als besonders geschützte Systemoperation behandeln.
2. Ein neuer Bootloader MUSS vor Aktivierung vollständig gestaged werden.
3. Ein Candidate MUSS vor Aktivierung auf Integrität und Signatur geprüft werden.
4. Die aktive Trust Policy MUSS den Candidate autorisieren.
5. Firmware-, Architektur- und Boot-Protocol-Kompatibilität MÜSSEN vor Aktivierung geprüft werden.
6. Secure-Boot-Kompatibilität MUSS auf entsprechenden UEFI-Systemen berücksichtigt werden.
7. Die bisherige Known-Good-Bootloader-Generation MUSS bis zum erfolgreichen Commit erhalten bleiben.
8. Die Aktivierung MUSS crash-konsistent erfolgen.
9. Ein Stromausfall während der Aktivierung DARF NICHT den einzigen gültigen Bootpfad zerstören.
10. Eine neue Bootloader-Generation MUSS zunächst als Candidate behandelt werden.
11. Ein Candidate DARF erst nach erfolgreicher Boot Health Validation zu Known-Good werden.
12. Fehlgeschlagene Candidates MÜSSEN auf eine autorisierte Known-Good-Generation zurückfallen können.
13. Der Fallback SOLL nicht vollständig vom fehlerhaften Candidate selbst abhängen.
14. BIOS- und UEFI-Pfade DÜRFEN unterschiedliche physische Updateverfahren verwenden.
15. Beide Firmwarepfade MÜSSEN dieselbe Schutzsemantik erfüllen.
16. Bootloader- und Systemgenerationen MÜSSEN getrennt versioniert werden können.
17. Inkompatible Bootloader-/System-Kombinationen DÜRFEN NICHT aktiviert werden.
18. Koordinierte Bootloader- und Systemupdates MÜSSEN als gemeinsame transaktionale Updatepläne modellierbar sein.
19. Trust-Root-Änderungen DÜRFEN NICHT implizit durch normale Bootloader-Updates erfolgen.
20. Security Anti-Rollback MUSS mit autorisiertem Known-Good-Fallback vereinbar sein.
21. Bootloader-State MUSS crash-konsistent gespeichert werden.
22. Nicht aktive Bootloader-Generationen SOLLEN vor unbeabsichtigten Schreibzugriffen geschützt werden.
23. Firmware-State-Änderungen MÜSSEN validiert und auditierbar sein.
24. NovaDOS MUSS autorisierte Bootloader-Diagnose und -Reparatur unterstützen können.
25. Bootloader-Update, Commit, Failure und Rollback MÜSSEN auditierbar sein.
26. Der aktuelle Bootloader-Generationenstatus MUSS introspektierbar sein.
27. Normale Anwendungen DÜRFEN Bootloader oder dessen Update-State NICHT direkt verändern.
28. Bootloader-Updates MÜSSEN ohne Netzwerk ausführbar sein, sofern alle benötigten Artefakte lokal vorliegen.
29. Bootloader-Update-Sicherheit MUSS vollständig ohne KI funktionieren.
30. KI DARF Trust-, Signatur-, Commit- oder Rollbackregeln NICHT überschreiben.

---

## Konsequenzen

### Positive Konsequenzen

- ein fehlerhaftes Bootloader-Update muss nicht automatisch zum unbootbaren System führen,
- Known-Good-Fallback bleibt erhalten,
- BIOS und UEFI können plattformspezifisch implementiert werden, ohne die Schutzsemantik zu ändern,
- Bootloader- und Systemupdates lassen sich kontrolliert koordinieren,
- Secure Boot und Verified Boot bleiben während Updates erhalten,
- Recovery und NovaDOS können Bootloaderfehler gezielt behandeln.

### Negative Konsequenzen

- Bootloader-Updates benötigen zusätzliche Generationen- und Statusverwaltung,
- BIOS-Systeme können komplexere Redundanzmechanismen benötigen,
- zusätzlicher Speicher für mindestens eine Fallback-Generation wird benötigt,
- Boot Protocol und Updatekompatibilität müssen langfristig gepflegt werden.

---

## Verworfene Alternativen

### Bootloader direkt überschreiben

Verworfen.

Ein unterbrochener oder fehlerhafter Schreibvorgang könnte den einzigen Bootpfad zerstören.

### Bootloader wie normale Systemdatei aktualisieren

Verworfen.

Die Ausfallwirkung und Vertrauensrolle des Bootloaders ist wesentlich höher.

### Commit unmittelbar nach Schreiben

Verworfen.

Die neue Version muss zunächst tatsächlich erfolgreich booten.

### Kein Bootloader-Fallback

Verworfen.

Dies würde Recovery bei fehlerhaften Bootloader-Updates unnötig erschweren.

### Nur Firmware-Recovery verwenden

Verworfen.

Firmwarefunktionen sind plattformabhängig und nicht überall ausreichend.

### Automatischer Trust-Root-Wechsel zusammen mit jedem Bootloader-Update

Verworfen.

Bootloadercode und Trust-Root-Verwaltung sind getrennte Sicherheitsentscheidungen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-BOOT-0001_Parallele_BIOS_und_UEFI_Bootunterstützung`
- `ADR-BOOT-0002_UEFI_bevorzugen_mit_BIOS_Fallback`
- `ADR-BOOT-0003_Mehrstufige_Bootarchitektur`
- `ADR-BOOT-0005_Vertrauenswürdige_Bootkette`
- `ADR-BOOT-0006_Secure_Measured_Verified_Boot_Integration`
- `ADR-BOOT-0008_Recovery_als_First_Class_Bootmodus`
- `ADR-BOOT-0009_A_B_Boot_und_Boot_Rollback`
- `ADR-BOOT-0010_Boot_Health_Milestones_vor_Commit`
- `ADR-BOOT-0011_NovaDOS_als_Offline_Recovery_Umgebung`
- `ADR-ARCH-0004_Transaktionale_Systemoperationen`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvierbare_Systemarchitektur`

---

## Zugehörige NPSPECs

- `NPSPEC-BOOT-BOOTLOADER-UPDATE-0001`
- `NPSPEC-BOOT-BOOTLOADER-GENERATION-0001`
- `NPSPEC-BOOT-BOOTLOADER-STATE-0001`
- `NPSPEC-BOOT-BOOTLOADER-CANDIDATE-0001`
- `NPSPEC-BOOT-BOOTLOADER-COMMIT-0001`
- `NPSPEC-BOOT-BOOTLOADER-FALLBACK-0001`
- `NPSPEC-BOOT-BOOTLOADER-ROLLBACK-0001`
- `NPSPEC-BOOT-BOOTLOADER-COMPATIBILITY-0001`
- `NPSPEC-BOOT-UEFI-UPDATE-0001`
- `NPSPEC-BOOT-BIOS-UPDATE-0001`
- `NPSPEC-BOOT-BOOTLOADER-RECOVERY-0001`
- `NPSPEC-BOOT-BOOTLOADER-UPDATE-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- erfolgreiches UEFI-Bootloader-Update,
- erfolgreiches BIOS-Bootloader-Update,
- ungültige Candidate-Signatur,
- inkompatibles Boot Protocol,
- Secure-Boot-inkompatibler EFI-Loader,
- Stromausfall während Staging,
- Stromausfall während Aktivierung,
- Stromausfall während Commit,
- Candidate startet erfolgreich,
- Candidate erreicht Boot Health nicht,
- Candidate ist vollständig unbootbar,
- automatischer Fallback auf Known-Good,
- beschädigte Bootloader-State-Metadaten,
- koordiniertes Bootloader-/Systemupdate,
- Security Anti-Rollback,
- autorisierter Known-Good-Rollback,
- NovaDOS-Reparatur eines fehlerhaften Bootloader-State,
- normale Anwendung versucht Bootloaderänderung.

---

## Ergebnis

Bootloader-Updates besitzen in NovaOS einen eigenen besonders geschützten Updatepfad.

Das Modell lautet:

```text
Known-Good Bootloader
        ↓
Stage Candidate
        ↓
Verify
        ↓
Compatibility Check
        ↓
Atomic Activation
        ↓
Candidate Boot
        │
    ┌───┴───┐
    ▼       ▼
 Success   Failure
    │       │
    ▼       ▼
 Commit   Fallback
```

Die bisherige funktionsfähige Bootloader-Generation bleibt erhalten, bis der neue Bootpfad erfolgreich bestätigt wurde.

Die zentrale Architekturregel lautet:

```text
Treat the bootloader
as infrastructure that enables recovery.

Update it transactionally.

Verify before activation.

Keep a known-good path
until the new path has proven itself.
```