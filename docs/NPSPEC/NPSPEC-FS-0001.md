
# NPSPEC-FS-0001 – Legacy Filesystem Layout

## Status

Ersetzt

## Ersetzt durch

- `docs/NPSPEC/sysarchitecture/038-FILESYSTEM/001-FILESYSTEM/NPSPEC-FILESYSTEM-NAMESPACE-0002.md`
- `docs/NPSPEC/sysarchitecture/038-FILESYSTEM/003-USERSPACE/NPSPEC-USERSPACE-LAYOUT-0001.md`
- `docs/NPSPEC/sysarchitecture/038-FILESYSTEM/007-SYSTEM/NPSPEC-SYSTEM-LAYOUT-0001.md`

## Hinweis

Diese ältere Skizze beschreibt eine einfache, kleingeschriebene Pfadstruktur. Sie ist nicht mehr maßgeblich, weil die angenommenen Filesystem-NPSPECs einen stabilen globalen Namespace mit getrennten Identitäten, Projections, lokalisierten Anzeigenamen und klaren System-/Benutzer-/Volume-Bereichen definieren.

Die folgende Struktur bleibt nur als historische Referenz erhalten.

```text
/
├── boot
|   ├── boot1.bin
|   ├── boot2.bin
|   └── bootmng.bin
├── system
|   ├── config.cfg
|   └── kernel
|       ├── kernel.bin
|       └── save_kernel.bin
├── recovery
|   ├── recover.bin
|   ├── memtest.bin
|   ├── crypt.bin
|   └── sysrecover.img
├── application
|   ├── explorer
|   ├── rechner
|   ├── browser
|   └── einstellungen
├── devices
|   ├── usb
|   ├── bluetooth
|   ├── wlan
|   └── midi
├── media
|   └── usb
├── user
|   ├── matze
|   |   ├── desktop
|   |   ├── bilder
|   |   ├── dokumente
|   |   ├── downloads
|   |   ├── musik
|   |   └── videos
|   └── mia
|       ├── desktop
|       ├── bilder
|       ├── dokumente
|       ├── downloads
|       ├── musik
|       └── videos
└── volumes
    ├── hdd0
    └── disc0
```
