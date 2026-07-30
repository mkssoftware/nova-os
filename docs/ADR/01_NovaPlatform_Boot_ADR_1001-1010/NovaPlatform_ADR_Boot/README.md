# Nova Platform – Boot ADR Index

Diese Sammlung enthält die angenommenen Boot-Architecture-ADRs ADR-1001 bis ADR-1010.

## Dokumente

- [ADR-1001: Mehrstufige Nova-Bootarchitektur](ADR-1001-Bootarchitektur.md)
- [ADR-1002: BIOS/MBR als erste Referenzimplementierung](ADR-1002-BIOS-zuerst.md)
- [ADR-1003: UEFI als separates Firmware-Frontend](ADR-1003-UEFI-spaeter.md)
- [ADR-1004: Versionierte BootInfo-ABI](ADR-1004-BootInfo-ABI.md)
- [ADR-1005: Framebuffer-Initialisierung im Bootloader](ADR-1005-Grafikinitialisierung.md)
- [ADR-1006: Minimaler Asset Loader im Boot Manager](ADR-1006-Asset-Loader.md)
- [ADR-1007: Eingeschränktes Boot-Theme](ADR-1007-Theme-Loader.md)
- [ADR-1008: PNG nur in der Host-Asset-Pipeline](ADR-1008-PNG-Unterstuetzung.md)
- [ADR-1009: TTF wird buildseitig in Bitmap-Fonts konvertiert](ADR-1009-TTF-Unterstuetzung.md)
- [ADR-1010: Datengetriebenes Bootmenü mit sicherer Eingabe](ADR-1010-Bootmenue.md)

## Geltungsbereich

Diese Entscheidungen gelten für den Nova Boot Manager und dessen BIOS- und UEFI-Frontends. Der Bootpfad muss klein, validierbar und unabhängig vom Zustand des installierten NovaOS bleiben.
