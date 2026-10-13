# NPSPEC-USB-SCSI-0001 – SCSI über USB

## Status

Angenommen

## Kategorie

USB / SCSI / Storage

## Zweck

Diese Spezifikation definiert die SCSI-Kommandoschicht für USB-Massenspeicher.

## Mindestkommandos

NovaOS MUSS mindestens unterstützen:

- INQUIRY,
- TEST UNIT READY,
- REQUEST SENSE,
- READ CAPACITY,
- READ(10),
- WRITE(10),
- SYNCHRONIZE CACHE.

## Sense Handling

SCSI Sense-Daten MÜSSEN in NovaOS-Storage-Fehler übersetzt werden. Unit Attention MUSS für Wechselmedien berücksichtigt werden.

## Akzeptanzkriterien

- Kapazität und Blockgröße können ermittelt werden.
- Read/Write-Fehler erzeugen differenzierte Statuscodes.
- REQUEST SENSE wird nach Check Condition verwendet.

