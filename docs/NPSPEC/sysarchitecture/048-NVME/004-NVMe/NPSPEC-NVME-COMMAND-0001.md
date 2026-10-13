# NPSPEC-NVME-COMMAND-0001 – NVMe Commands

## Status

Angenommen

## Kategorie

NVMe / Command / Protokoll

## Zweck

Diese Spezifikation definiert die Kommandodarstellung und Statusverarbeitung für NVMe.

## Command-Typen

NovaOS MUSS mindestens unterstützen:

- Identify,
- Get Features,
- Set Features,
- Create I/O Completion Queue,
- Create I/O Submission Queue,
- Delete I/O Queues,
- Read,
- Write,
- Flush.

## Command-Objekt

Ein Command MUSS Opcode, Namespace ID, PRP/SGL-Information, Command ID, Timeout und Completion-Ziel enthalten.

## Status

NVMe Status Codes MÜSSEN in NovaOS-Storage-Statuscodes übersetzt werden. Retryable und fatal Errors MÜSSEN unterscheidbar bleiben.

## Akzeptanzkriterien

- Command IDs werden pro Queue eindeutig vergeben.
- Fehlerhafte Statuscodes erzeugen Diagnose.
- Timeout kann Command/Controller-Recovery auslösen.

