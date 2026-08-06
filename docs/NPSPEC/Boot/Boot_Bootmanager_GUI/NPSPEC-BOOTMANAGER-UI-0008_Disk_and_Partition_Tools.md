# NPSPEC-BOOTMANAGER-UI-0008
# Disk and Partition Tools

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0008 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager / Diagnostics / Recovery |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTMANAGER-UI-0001, NPSPEC-BOOTCONTROL-LIST-0001, NPSPEC-BOOTCONTROL-TREEVIEW-0001, NPSPEC-BOOTCONTROL-DIALOG-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTDISK-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die **Disk and Partition Tools** des NovaOS Boot Managers.

Die Oberfläche stellt Diagnose-, Analyse- und Wartungsfunktionen für Datenträger und Partitionen bereit. Sie ermöglicht das Anzeigen von Datenträgerinformationen, das Prüfen von Partitionen sowie ausgewählte Verwaltungsfunktionen innerhalb der Bootumgebung.

Die Benutzeroberfläche dient ausschließlich der Systemverwaltung vor dem Start des Betriebssystems.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- HDD
- SSD
- NVMe
- USB-Datenträger
- SATA
- SAS
- RAID (Lesemodus)
- GPT
- MBR
- EFI-Systempartitionen
- zukünftige Speichermedien

---

# 3. Ziele

Die Disk and Partition Tools müssen:

- alle Datenträger übersichtlich darstellen
- Partitionen visualisieren
- Diagnosefunktionen bereitstellen
- vollständig per Tastatur bedienbar sein
- deterministisch arbeiten
- modular erweiterbar sein

---

# 4. Bildschirmaufbau

```text
+---------------------------------------------------------------+

Zurück

Disk and Partition Tools

---------------------------------------------------------------

Datenträgerliste

---------------------------------------------------------------

Partitionsübersicht

---------------------------------------------------------------

Datenträgerinformationen

---------------------------------------------------------------

Aktionen

[ Prüfen ]

[ Details ]

[ Reparieren ]

[ Aktualisieren ]

[ Zurück ]

+---------------------------------------------------------------+
```

---

# 5. Komponenten

Der Bildschirm besteht aus:

- Header
- Datenträgerliste
- Partitionsübersicht
- Informationsbereich
- Aktionsbereich
- Statusbereich

Alle Komponenten verwenden Standard-Boot Controls.

---

# 6. Header

Der Header enthält:

- Zurücknavigation
- Datenträger-Icon
- Titel
- Anzahl erkannter Datenträger

---

# 7. Datenträgerliste

Für jeden Datenträger werden angezeigt:

- Laufwerksname
- Typ
- Modell
- Kapazität
- Anschlussart
- Gesundheitsstatus
- Bootfähigkeit

Jeder Datenträger wird als Tile dargestellt.

---

# 8. Partitionsübersicht

Nach Auswahl eines Datenträgers werden dargestellt:

- Partitionen
- Dateisystem
- Größe
- Belegter Speicher
- Freier Speicher
- GPT-/MBR-Typ
- EFI-Markierung
- Boot-Flag
- Integritätsstatus

Die Darstellung erfolgt zusätzlich als grafische Partitionsleiste.

---

# 9. Informationsbereich

Folgende Informationen werden angezeigt:

- Modell
- Hersteller
- Seriennummer
- Firmwareversion
- Blockgröße
- Sektorgröße
- SMART-Unterstützung
- Partitionstabellentyp
- Bootmodus
- UUID
- Dateisysteminformationen

---

# 10. Aktionen

Standardaktionen:

- Datenträger prüfen
- Partition prüfen
- Details anzeigen
- Reparieren
- Neu scannen
- Zurück

Optional:

- SMART anzeigen
- GPT validieren
- MBR validieren
- EFI prüfen
- Bootsektor analysieren

---

# 11. Statusbereich

Der Statusbereich zeigt:

- Gesundheitsstatus
- Integritätsstatus
- SMART-Status
- Prüfergebnisse
- erkannte Fehler
- Warnungen

Statusinformationen werden über Status Badges dargestellt.

---

# 12. Navigation

Unterstützt werden:

- Tastatur
- Maus
- Touch

Datenträger und Partitionen können unabhängig voneinander fokussiert werden.

---

# 13. Fokus

Der Fokus muss:

- jederzeit eindeutig sichtbar sein
- zwischen Datenträger- und Partitionsliste wechseln können
- weich animiert werden

---

# 14. Animationen

Unterstützt werden:

- Auswahlanimationen
- Fokusanimationen
- Listenanimationen
- Dialoganimationen
- Statusanimationen

Alle Bewegungen verwenden Motion Tokens.

---

# 15. Themeintegration

Unterstützt werden:

- Dark Theme
- Light Theme
- High Contrast Theme

Alle Farben stammen aus Design Tokens.

---

# 16. Accessibility

Die Disk and Partition Tools unterstützen:

- Screenreader
- Tastatursteuerung
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen

Die grafische Partitionsübersicht besitzt immer eine textuelle Alternative.

---

# 17. Fehlerfälle

Zu behandeln sind:

- Datenträger nicht erreichbar
- beschädigte Partitionstabelle
- ungültige GPT
- beschädigter MBR
- SMART-Warnung
- Lesefehler
- unbekanntes Dateisystem

Fehler werden deutlich hervorgehoben.

---

# 18. Sicherheit

Vor jeder Aktion werden geprüft:

- Schreibschutz
- Integrität
- Datenträgerstatus
- Berechtigungen
- unterstützte Operation

Kritische Änderungen verlangen eine ausdrückliche Bestätigung.

---

# 19. Speicherverwaltung

Die Oberfläche verwendet:

- Disk Pool
- Partition Pool
- Resource Cache
- Theme Cache

Heap-Allokationen während der Darstellung sind verboten.

---

# 20. Performance

Anforderungen:

- Initialisierung < 150 ms
- O(1)-Datenträger Lookup
- O(1)-Partitions Lookup
- O(1)-Theme Lookup
- deterministische Renderzeit

Datenträgerinformationen werden vor der Darstellung vollständig erfasst.

---

# 21. API

```c
typedef struct
{
    uint32_t disk_id;
    uint64_t capacity;
    uint32_t partition_count;
    bool bootable;
    bool healthy;
}
nova_disk_descriptor_t;

typedef struct
{
    uint32_t partition_id;
    uint64_t size;
    uint64_t used;
    bool mounted;
    bool boot_partition;
}
nova_partition_descriptor_t;

nova_result_t
nova_disk_tools_open(void);

nova_result_t
nova_disk_scan(void);

const nova_disk_descriptor_t*
nova_disk_get(
    uint32_t disk_id);

const nova_partition_descriptor_t*
nova_partition_get(
    uint32_t partition_id);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Anzahl Datenträger
- Anzahl Partitionen
- SMART-Status
- Integritätsstatus
- erkannte Fehler
- Benutzeraktionen
- Ladezeit

---

# 23. Testfälle

Zu testen sind:

- GPT-Datenträger
- MBR-Datenträger
- EFI-Systempartition
- beschädigte GPT
- beschädigter MBR
- SMART-Warnungen
- Themewechsel
- Accessibility
- Datenträger-Rescan
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Datenträger korrekt erkannt und dargestellt werden.
- Partitionen sowohl grafisch als auch textuell angezeigt werden.
- Diagnosefunktionen für Datenträger und Partitionen verfügbar sind.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- GPT-, MBR- und EFI-Informationen korrekt dargestellt werden.
- beschädigte Datenträger eindeutig gekennzeichnet werden.
- keine Heap-Allokationen während der Darstellung stattfinden.
- sämtliche Animationen ausschließlich Motion Tokens verwenden.

---

# 25. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0008 – Disk and Partition Tools
- ADR-BOOTDISK-0001 – Disk Management Framework
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTMANAGER-UI-0001 – Main Boot Selection Screen
- NPSPEC-BOOTCONTROL-LIST-0001 – List
- NPSPEC-BOOTCONTROL-TREEVIEW-0001 – Tree View
- NPSPEC-BOOTDIALOG-0001 – Dialog
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTDISK-0001 – Disk Management Framework

---

# 26. Zusammenfassung

Die **Disk and Partition Tools** bilden die zentrale Verwaltungsoberfläche für Datenträger und Partitionen innerhalb des NovaOS Boot Managers. Sie kombinieren eine grafische Partitionsdarstellung, detaillierte Datenträgerinformationen sowie Diagnose- und Wartungsfunktionen in einer modernen, konsistenten Benutzeroberfläche. Durch vollständige Integration in die NovaOS Design Language, das Theme-System und die Motion Engine ermöglichen sie eine sichere und effiziente Analyse von Datenträgern bereits vor dem Start des Betriebssystems.