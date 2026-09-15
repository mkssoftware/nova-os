# ADR-UPDATE-0016 – Bootloader Update Schutz

## Status

Angenommen

## Kategorie

Update / Boot / Recovery

## Kontext

Der Bootloader ist Teil der kritischen Startkette von NovaOS.

Ein fehlerhaftes oder unterbrochenes Bootloader-Update kann dazu führen, dass weder NovaOS noch die reguläre Recovery-Umgebung gestartet werden können.

Bootloader-Updates benötigen deshalb stärkere Schutzmechanismen als normale Softwareupdates.

## Entscheidung

NovaOS behandelt Bootloader-Updates als besonders geschützte Systemtransaktionen.

```text
Bootloader Update
       ↓
Preflight
       ↓
Prepare
       ↓
Verify
       ↓
Protected Write
       ↓
Boot Verification
       ↓
Commit
```

Die aktuell funktionsfähige Bootfähigkeit muss soweit technisch möglich bis zur erfolgreichen Verifikation erhalten bleiben.

## Update-Strategie

Wenn die Plattform es unterstützt, wird eine redundante Bootloader-Struktur bevorzugt.

```text
Bootloader A → Active
Bootloader B → Update Target
```

Die aktive Kopie wird nicht direkt überschrieben.

Nach erfolgreicher Prüfung kann B als neuer Bootloader aktiviert werden.

## Preflight

Vor dem Update werden mindestens geprüft:

```text
Platform Compatibility
Boot Mode
Target Version
Firmware Requirements
Power State
Package Integrity
Trust
Recovery Path
```

Ein fehlgeschlagener Preflight blockiert das Update.

## Power-Failure-Safety

Kritische Boot-Metadaten müssen power-failure-safe aktualisiert werden.

```text
Old Boot State
      ↓
Prepare New State
      ↓
Persist
      ↓
Atomic Activation
```

Ein Stromausfall darf soweit hardwareseitig möglich nicht beide Bootpfade gleichzeitig unbrauchbar machen.

## Verifikation

Vor der endgültigen Aktivierung muss der neue Bootloader geprüft werden.

Nach dem ersten erfolgreichen Start wird bestätigt, dass der neue Bootpfad funktioniert.

```text
New Bootloader
      ↓
Boot Attempt
      ↓
Health Milestone
   ┌──┴───┐
   ↓      ↓
Success  Failure
   ↓      ↓
Commit   Fallback
```

## Fallback

Schlägt der neue Bootpfad fehl, soll automatisch auf den letzten Known-Good-Bootloader zurückgefallen werden.

```text
New Bootloader Failed
        ↓
Known-Good Bootloader
        ↓
Recovery
```

## Trust

Bootloader-Updates müssen die strengsten für die Boot Chain geltenden Trust-Anforderungen erfüllen.

Content ID, Signatur, Provenance und Autorisierung werden vor dem Schreiben geprüft.

## Rollback Barrier

Falls eine Plattform keinen redundanten oder atomaren Bootloader-Updatepfad unterstützt, gilt der kritische Schreibvorgang als Rollback Barrier.

Das erhöhte Risiko muss vor Beginn bekannt sein und ein alternativer Recovery-Pfad muss soweit möglich existieren.

## Normative Anforderungen

1. Bootloader-Updates MÜSSEN als besonders geschützte Systemtransaktionen behandelt werden.
2. Vor jedem Update MUSS ein Preflight erfolgen.
3. Zielartefakt, Plattformkompatibilität und Trust MÜSSEN vor dem Schreiben geprüft werden.
4. Die aktive Known-Good-Bootfähigkeit SOLL bis zur erfolgreichen Verifikation erhalten bleiben.
5. Redundante Bootloader-Slots SOLLEN verwendet werden, wenn die Plattform dies unterstützt.
6. Kritische Boot-Metadaten MÜSSEN power-failure-safe aktualisiert werden.
7. Der neue Bootloader MUSS vor endgültigem Commit verifiziert werden.
8. Bei fehlgeschlagenem Boot SOLL automatisch auf einen Known-Good-Zustand zurückgefallen werden.
9. Nicht sicher rückrollbare Schreiboperationen MÜSSEN als Rollback Barrier behandelt werden.
10. Bootloader-Updates DÜRFEN normale Trust-, Verified-Boot- oder Recovery-Prüfungen nicht umgehen.

## Konsequenzen

### Positive Konsequenzen

- geringeres Risiko eines nicht mehr bootfähigen Systems,
- automatische Rückkehr zu Known-Good-Zuständen,
- Bootloader-Updates werden power-failure-safe,
- Boot-Trust und Update-Architektur bleiben verbunden.

### Negative Konsequenzen

- redundante Bootloader können zusätzlichen Speicher benötigen,
- Hardwareplattformen unterscheiden sich stark,
- manche Plattformen erlauben keinen vollständig sicheren Rollback.

## Abhängigkeiten

- `ADR-UPDATE-0002_Transactional_Update_Pipeline`
- `ADR-UPDATE-0003_Atomic_Updates`
- `ADR-UPDATE-0010_Package_Signing_Provenance_und_SBOM`
- `ADR-UPDATE-0015_Firmware_Update_Preflight`
- `ADR-TRANSACTION-0006_Transaction_Log_und_Power_Failure_Safety`
- `ADR-TRANSACTION-0008_Irreversible_Operationen_mit_Rollback_Barrier`
- `ADR-BOOT-0006_Verified_Boot`
- `ADR-BOOT-0007_Measured_Boot`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-BOOTLOADER-PROTECTION-0001`
- `NPSPEC-UPDATE-BOOTLOADER-ATOMIC-SWITCH-0001`
- `NPSPEC-UPDATE-BOOTLOADER-FALLBACK-0001`

## Ergebnis

NovaOS überschreibt seine letzte funktionsfähige Bootbasis nicht ungeschützt:

```text
Known-Good Bootloader
        ↓
Prepare New Bootloader
        ↓
Verify
        ↓
Protected Activation
        ↓
Boot Test
     ┌──┴───┐
     ↓      ↓
  Success  Failure
     ↓      ↓
   Commit  Fallback
```

Der zentrale Grundsatz lautet:

```text
Ein Bootloader-Update darf
die letzte bekannte
Bootfähigkeit nicht leichtfertig zerstören.
```