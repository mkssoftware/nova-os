# NPSPEC-DRIVER-DMA-0001 – Driver DMA

## Status

Angenommen

## Kategorie

Driver / DMA / Sicherheit

## Zweck

Diese Spezifikation definiert gemeinsame DMA-Regeln für USB, NVMe und andere Hardwaretreiber.

## Anforderungen

- Treiber DÜRFEN keine ungeprüften physischen Adressen erzeugen.
- DMA Mapping MUSS über die Kernel-DMA-Schicht erfolgen.
- IOMMU MUSS genutzt werden, wenn verfügbar.
- Pufferlebenszeit MUSS mindestens bis Completion reichen.
- Unmap MUSS bei Erfolg, Fehler, Cancel und Reset erfolgen.

## Kohärenz

Cache-Kohärenz und Speicherbarrieren MÜSSEN durch DMA-APIs oder dokumentierte Treiberregeln sichergestellt werden.

## Akzeptanzkriterien

- NVMe PRP/SGL und USB Transfer Buffer nutzen dieselbe DMA-Abstraktion.
- DMA-Faults werden an den betroffenen Request gebunden.
- Kein Treiber erhält pauschalen Zugriff auf Kernel-Speicher.

