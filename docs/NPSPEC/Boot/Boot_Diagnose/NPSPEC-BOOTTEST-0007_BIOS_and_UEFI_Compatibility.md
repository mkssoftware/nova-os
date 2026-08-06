# NPSPEC-BOOTTEST-0007
# BIOS and UEFI Compatibility Tests

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTTEST-0007 |
| Titel | BIOS and UEFI Compatibility Tests |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Testing |
| Priorität | Kritisch |
| Gültigkeitsbereich | Gesamte NovaOS Bootplattform |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTTEST-0001, NPSPEC-BOOT-0001, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTPERF-0004 |

---

# 1. Ziel

Diese Spezifikation definiert die **BIOS and UEFI Compatibility Tests** der NovaOS-Bootplattform.

Die Tests stellen sicher, dass sämtliche Komponenten des NovaOS-Bootsystems sowohl auf klassischen BIOS-Systemen als auch auf modernen UEFI-Systemen identisch funktionieren. Ziel ist eine vollständig kompatible, deterministische und reproduzierbare Bootumgebung unabhängig vom Firmwaretyp.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Bootloader Stage 1
- Bootloader Stage 2
- Boot Manager
- Rendering Engine
- Resource Manager
- Input System
- Recovery
- Self-Healing
- Installer
- Kernel Loader

---

# 3. Ziele

Die Tests müssen:

- BIOS vollständig unterstützen
- UEFI vollständig unterstützen
- identisches Verhalten sicherstellen
- Firmwareunterschiede validieren
- reproduzierbare Ergebnisse liefern
- vollständig automatisierbar sein

---

# 4. Testarchitektur

Das Testsystem besteht aus:

- Firmware Test Runner
- BIOS Adapter
- UEFI Adapter
- Hardware Simulator
- Diagnostics Interface
- Report Generator

Alle Firmware-spezifischen Unterschiede werden durch definierte Abstraktionsschichten behandelt.

---

# 5. Testplattformen

Mindestens folgende Plattformen werden unterstützt:

### BIOS

- Legacy BIOS
- CSM
- SeaBIOS

### UEFI

- UEFI Class 2
- UEFI Class 3
- OVMF
- Secure Boot (optional)
- TPM (optional)

---

# 6. Testablauf

Standardablauf:

1. Firmware initialisieren
2. Bootloader starten
3. Grafikausgabe prüfen
4. Eingabesystem prüfen
5. Ressourcen laden
6. Boot Manager starten
7. Kernelübergabe validieren
8. Ergebnisse speichern

---

# 7. BIOS-Tests

Zu prüfen sind:

- Real Mode
- Protected Mode Übergang
- Speicherlayout
- VESA Initialisierung
- Interrupts
- Tastatur
- Massenspeicher
- Boot Services

---

# 8. UEFI-Tests

Zu prüfen sind:

- GOP
- Boot Services
- Runtime Services
- Memory Map
- Protocol Discovery
- Handle Database
- Eingabegeräte
- ExitBootServices()

---

# 9. Grafiktests

Unterstützte Backends:

- VESA BIOS Extensions
- UEFI GOP

Zu validieren sind:

- Auflösung
- Farbtiefe
- Framebuffer
- Pixelgenauigkeit
- Double Buffering

---

# 10. Speicherverwaltung

Zu prüfen sind:

- BIOS Memory Map
- UEFI Memory Map
- reservierter Speicher
- freier Speicher
- Speicherübergabe
- Speicherattribute

---

# 11. Eingabetests

Geprüft werden:

- Tastatur
- Maus
- Touch (UEFI)
- Wiederholungen
- Scan Codes
- Unicode

---

# 12. Ressourcen

Zu testen sind:

- Assetpakete
- PNG
- SVG
- Fonts
- Themes
- Animationen

Alle Ressourcen müssen unter BIOS und UEFI identisch geladen werden.

---

# 13. Kernelübergabe

Zu validieren sind:

- Bootinformationen
- Framebuffer
- Speicherkarte
- ACPI
- SMBIOS
- CPU-Informationen
- Kernelparameter

---

# 14. Fehlerbehandlung

Zu erkennen sind:

- Firmwarefehler
- GOP-Fehler
- VESA-Fehler
- ExitBootServices-Fehler
- Speicherfehler
- Ressourcenfehler

---

# 15. Performance

Für beide Firmwarearten werden gemessen:

- Bootdauer
- UI Initialisierung
- Renderzeit
- Ressourcenladezeit
- Kernelübergabe

---

# 16. Regression

Automatisch auszuführen bei Änderungen an:

- Bootloader
- Firmwareadapter
- Grafiktreiber
- Speicherverwaltung
- Kernel Loader

---

# 17. Virtuelle Maschinen

Unterstützt werden:

- QEMU
- OVMF
- VirtualBox
- VMware
- Hyper-V

Virtuelle Plattformen ergänzen, ersetzen jedoch keine Tests auf realer Hardware.

---

# 18. Hardwaretests

Empfohlene Referenzhardware:

- Legacy BIOS Mainboard
- modernes UEFI Mainboard
- Notebook
- Desktop
- ARM64 (zukünftig)

---

# 19. Sicherheit

Zu prüfen sind:

- Secure Boot
- Signaturprüfung
- TPM-Unterstützung
- Integritätsprüfungen
- Recoverymechanismen

Sicherheitsfunktionen dürfen durch Firmwareunterschiede nicht beeinflusst werden.

---

# 20. Speicherverwaltung

Das Testsystem verwendet:

- Firmware Buffer
- Screenshot Buffer
- Diagnostics Buffer
- Report Buffer

Alle Speicherbereiche werden vor Testbeginn reserviert.

Heap-Allokationen während einzelner Firmwaretests sind nicht zulässig.

---

# 21. API

```c
typedef enum
{
    NOVA_FIRMWARE_BIOS,
    NOVA_FIRMWARE_UEFI
}
nova_firmware_type_t;

typedef struct
{
    nova_firmware_type_t firmware;
    bool graphics_ok;
    bool input_ok;
    bool resources_ok;
    bool kernel_handoff_ok;
    uint64_t boot_time_us;
}
nova_firmware_test_result_t;

nova_result_t
nova_firmware_test_initialize(void);

nova_result_t
nova_firmware_test_execute(
    nova_firmware_type_t firmware);

const nova_firmware_test_result_t*
nova_firmware_test_results(void);

nova_result_t
nova_firmware_test_generate_report(void);
```

---

# 22. Diagnosedaten

Zu protokollieren sind:

- Firmwaretyp
- Plattform
- Bootdauer
- Speicherlayout
- Grafikmodus
- Eingabesystem
- Ressourcenstatus
- Kernelübergabe
- Fehler
- Performancewerte

---

# 23. Testfälle

Zu testen sind:

- Legacy BIOS
- SeaBIOS
- OVMF
- UEFI GOP
- VESA
- Secure Boot
- ExitBootServices()
- Kernelübergabe
- Recovery
- Installer

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Funktionen unter BIOS und UEFI identisch arbeiten.
- Grafik-, Eingabe- und Ressourcenverwaltung auf beiden Firmwaretypen erfolgreich validiert werden.
- Kernelübergaben reproduzierbar funktionieren.
- Firmwareunterschiede korrekt abstrahiert werden.
- Regressionen automatisch erkannt werden.
- vollständige Diagnose- und Testberichte erzeugt werden.
- alle Firmwaretests automatisiert ausführbar sind.
- sämtliche Ergebnisse über die Test-API verfügbar sind.

---

# 25. Referenzen

## ADRs

- ADR-BOOTTEST-0007 – BIOS and UEFI Compatibility Tests
- ADR-BOOT-0001 – BIOS- und UEFI-Unterstützung
- ADR-BOOTRENDER-0001 – Rendering Engine

## NPSPECs

- NPSPEC-BOOTTEST-0001 – Boot UI Test Architecture
- NPSPEC-BOOT-0001 – BIOS and UEFI Support
- NPSPEC-BOOTRENDER-0001 – Rendering Engine
- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTPERF-0004 – Startup-Time Budget

---

# 26. Zusammenfassung

Die **BIOS and UEFI Compatibility Tests** definieren die standardisierte Teststrategie zur Validierung der NovaOS-Bootplattform auf klassischen BIOS- und modernen UEFI-Systemen. Sie überprüfen Bootloader, Grafik, Speicherverwaltung, Eingabesystem, Ressourcenverwaltung und Kernelübergabe auf identisches Verhalten über unterschiedliche Firmwarearchitekturen hinweg. Durch automatisierte Testabläufe, reproduzierbare Ergebnisse und umfassende Diagnosefunktionen wird eine konsistente und langfristig stabile Bootumgebung auf allen unterstützten Plattformen sichergestellt.