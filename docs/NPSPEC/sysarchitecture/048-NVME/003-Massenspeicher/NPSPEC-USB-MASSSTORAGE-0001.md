# NPSPEC-USB-MASSSTORAGE-0001 – USB Mass Storage

## Status

Angenommen

## Kategorie

USB / Massenspeicher / Storage

## Zweck

Diese Spezifikation definiert USB-Massenspeicher als Blockgeräte im NovaOS-Storage-Stack.

## Unterstützte Transporte

NovaOS MUSS USB BOT unterstützen und SOLL UASP unterstützen, sobald Streams und asynchrone Queues verfügbar sind.

## Architektur

```text
USB Interface
  -> Mass Storage Class Driver
  -> SCSI Translation
  -> Block Device
  -> VFS / NovaFS
```

## Anforderungen

- Gerätekapazität MUSS über SCSI Inquiry/Read Capacity ermittelt werden.
- Lese- und Schreiboperationen MÜSSEN in Block-I/O übersetzt werden.
- Wechselmedien MÜSSEN Medienwechsel signalisieren.

## Akzeptanzkriterien

- Ein USB-Stick erscheint als Blockgerät.
- Fehlerhafte SCSI-Antworten werden kontrolliert behandelt.
- Disconnect während I/O bricht alle ausstehenden Requests ab.

