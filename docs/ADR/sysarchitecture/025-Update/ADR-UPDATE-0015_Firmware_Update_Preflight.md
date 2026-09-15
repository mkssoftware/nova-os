# ADR-UPDATE-0015 – Firmware Update Preflight

## Status

Angenommen

## Kategorie

Update / Firmware / Hardware Safety

## Kontext

Firmware-Updates unterscheiden sich von normalen Softwareupdates, da ein Fehler Hardware unbenutzbar machen oder einen manuellen Recovery-Vorgang erforderlich machen kann.

Vor einem Firmware-Update muss NovaOS deshalb prüfen, ob Gerät, Firmware, Energieversorgung und Recovery-Voraussetzungen einen sicheren Updatevorgang erlauben.

## Entscheidung

NovaOS führt vor jedem Firmware-Update einen verpflichtenden **Firmware Update Preflight** durch.

```text
Firmware Update
      ↓
Preflight
      ↓
Safe?
 ┌────┴────┐
 ↓         ↓
Yes        No
 ↓         ↓
Update    Block
```

Ohne erfolgreichen Preflight darf das Firmware-Update nicht gestartet werden.

## Preflight

Der Preflight prüft mindestens:

```text
Device Identity
Hardware Revision
Current Firmware
Target Firmware
Compatibility
Power State
Update Method
Recovery Capability
Package Integrity
Trust
```

Gerätespezifische Provider können zusätzliche Prüfungen definieren.

## Geräteidentität

Vor dem Update muss eindeutig festgestellt werden, für welches Gerät und welche Hardware-Revision die Firmware bestimmt ist.

```text
Target Device
     ↓
Device Identity
     ↓
Firmware Compatibility
```

Firmware darf nicht allein anhand eines ähnlichen Gerätenamens ausgewählt werden.

## Energieversorgung

NovaOS muss prüfen, ob während des Updates eine ausreichend stabile Energieversorgung erwartet werden kann.

Bei mobilen Geräten können beispielsweise erforderlich sein:

```text
External Power
Minimum Battery Level
Stable Power State
```

Sind die Anforderungen nicht erfüllt, wird das Update verschoben oder blockiert.

## Recovery

Vor dem Update muss bekannt sein, welche Recovery-Möglichkeiten das Gerät besitzt.

Beispiele:

```text
Dual Firmware Bank
Bootloader Recovery
Vendor Recovery Mode
Hardware Recovery
```

Fehlt eine sichere automatische Recovery-Möglichkeit, muss das erhöhte Risiko berücksichtigt werden.

## Updatefähigkeit

Der Preflight bestimmt die zulässige Update-Methode.

```text
Live Update
Restart Required
Reboot Required
Offline Update
```

Firmware darf nur über einen für das konkrete Gerät vorgesehenen Updatepfad geschrieben werden.

## Integrität und Trust

Vor dem Firmware-Update müssen Zielartefakt und relevante Trust-Evidenz geprüft werden.

```text
Firmware
   ↓
Content ID
   ↓
Signature / Provenance
   ↓
Trust Policy
```

Ein technisch kompatibles, aber nicht ausreichend vertrauenswürdiges Firmware-Artefakt darf nicht installiert werden.

## Transaction

Nach erfolgreichem Preflight wird das Firmware-Update als kontrollierte Systemtransaktion ausgeführt.

```text
Preflight
   ↓
Prepare
   ↓
Firmware Update
   ↓
Verify
   ↓
Commit
```

Irreversible Schreiboperationen werden als Rollback Barrier behandelt.

## Verification

Nach dem Update muss NovaOS soweit technisch möglich prüfen:

```text
Firmware Version
Device Health
Device Availability
Expected Function
```

Ein erfolgreich abgeschlossener Schreibvorgang allein gilt nicht als erfolgreiche Aktualisierung.

## Normative Anforderungen

1. Firmware-Updates MÜSSEN einen Preflight durchlaufen.
2. Gerät und Hardware-Revision MÜSSEN vor dem Update eindeutig bestimmt werden.
3. Die Ziel-Firmware MUSS mit dem Gerät kompatibel sein.
4. Energieanforderungen MÜSSEN vor Beginn erfüllt sein.
5. Recovery-Möglichkeiten MÜSSEN vor dem Update bestimmt werden.
6. Firmware-Artefakte MÜSSEN vor Verwendung auf Integrität und Trust geprüft werden.
7. Firmware DARF nur über einen unterstützten Updatepfad geschrieben werden.
8. Irreversible Firmware-Schreiboperationen MÜSSEN als Rollback Barrier behandelt werden.
9. Ein fehlgeschlagener Preflight MUSS das Update blockieren.
10. Nach dem Update MUSS soweit möglich eine Verification erfolgen.

## Konsequenzen

### Positive Konsequenzen

- geringeres Risiko unbrauchbarer Hardware,
- falsche Firmware wird frühzeitig erkannt,
- Energie- und Recovery-Risiken werden vor dem Schreiben geprüft,
- Firmware-Updates werden in die NovaOS-Trust- und Transaction-Architektur integriert.

### Negative Konsequenzen

- Firmware-Updates benötigen gerätespezifische Metadaten,
- nicht alle Geräte bieten sichere Recovery-Mechanismen,
- Preflight-Prüfungen können Updates verzögern.

## Abhängigkeiten

- `ADR-UPDATE-0002_Transactional_Update_Pipeline`
- `ADR-UPDATE-0003_Atomic_Updates`
- `ADR-UPDATE-0006_Content_Addressed_Packages`
- `ADR-UPDATE-0010_Package_Signing_Provenance_und_SBOM`
- `ADR-TRANSACTION-0008_Irreversible_Operationen_mit_Rollback_Barrier`
- `ADR-TRUST-0004_Trust_Anchors_nach_Zweck`

## Zugehörige NPSPECs

- `NPSPEC-UPDATE-FIRMWARE-PREFLIGHT-0001`
- `NPSPEC-UPDATE-FIRMWARE-COMPATIBILITY-0001`
- `NPSPEC-UPDATE-FIRMWARE-RECOVERY-0001`

## Ergebnis

Firmware wird erst geschrieben, wenn NovaOS den Updatevorgang als ausreichend sicher bewertet hat:

```text
Identify
   ↓
Check Compatibility
   ↓
Check Power
   ↓
Check Recovery
   ↓
Verify Firmware
   ↓
Update
   ↓
Verify Device
```

Der zentrale Grundsatz lautet:

```text
Erst prüfen,
dann Firmware verändern.
```