# NPSPEC-NVME-NAMESPACE-0001 – NVMe Namespace

## Status

Angenommen

## Kategorie

NVMe / Namespace / Blockgerät

## Zweck

Diese Spezifikation definiert NVMe Namespaces als Blockgeräte im NovaOS-Storage-Stack.

## Namespace Discovery

Der Treiber MUSS aktive Namespaces erkennen und per Identify Namespace beschreiben.

## Eigenschaften

Ein Namespace MUSS melden:

- Namespace ID,
- Größe,
- LBA-Format,
- Blockgröße,
- Write Protect Status,
- volatile write cache state,
- Multipath-/Controllerbindung, sofern vorhanden.

## Anforderungen

- Ungültige oder inaktive Namespaces DÜRFEN nicht als Blockgeräte erscheinen.
- Formatwechsel erfordert Neuvalidierung.
- Blockgröße MUSS mit Storage-Block-Stack kompatibel sein.

## Akzeptanzkriterien

- Namespace 1 kann als Blockgerät registriert werden.
- LBA-Größe wird korrekt übernommen.
- Namespace-Fehler isolieren nur betroffene Blockgeräte.

