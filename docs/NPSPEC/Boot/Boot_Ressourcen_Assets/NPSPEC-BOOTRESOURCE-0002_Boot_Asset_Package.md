# NPSPEC-BOOTRESOURCE-0002
# Boot Asset Package

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRESOURCE-0002 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Resource System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTFS-0001, NPSPEC-BOOTSECURITY-0001, NPSPEC-BOOTTHEME-0001, NPSPEC-BOOTLOCALIZATION-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Boot Asset Package (BAP)**-Format der NovaOS-Bootoberfläche.

Ein Boot Asset Package fasst mehrere Ressourcen in einer einzigen logischen Einheit zusammen. Dadurch können Bilder, Icons, Schriftarten, Themes, Animationen, Sprachpakete und weitere Ressourcen gemeinsam verteilt, versioniert und geladen werden.

Das BAP-Format ist das bevorzugte Austausch- und Installationsformat für Bootressourcen.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Theme-Pakete
- Icon-Pakete
- Schriftpakete
- Sprachpakete
- Bildsammlungen
- Animationen
- Cursor
- kombinierte Ressourcenpakete

---

# 3. Ziele

Das Asset-Package-System muss:

- mehrere Ressourcen bündeln
- Versionierung unterstützen
- Integritätsprüfungen ermöglichen
- Erweiterbarkeit gewährleisten
- deterministisch arbeiten
- schnelles Laden ermöglichen

---

# 4. Architektur

```text
Boot Asset Package

↓

Package Reader

↓

Package Validator

↓

Manifest Parser

↓

Resource Loader

↓

Resource Manager

↓

UI
```

---

# 5. Komponenten

Das Boot Asset Package besteht aus:

- Package Header
- Manifest
- Ressourcenverzeichnis
- Ressourcendaten
- Prüfsummen
- optionale Signatur

---

# 6. Grundprinzip

Der Ladevorgang erfolgt wie folgt:

```text
Paket

↓

Header prüfen

↓

Manifest lesen

↓

Integrität prüfen

↓

Ressourcen registrieren

↓

Laden bei Bedarf
```

Die einzelnen Ressourcen werden nicht zwangsläufig sofort in den Arbeitsspeicher geladen.

---

# 7. Paketstruktur

Ein Boot Asset Package besitzt folgende Struktur:

```text
Package Header

Manifest

Resource Index

Resource Data

Checksums

Digital Signature (optional)
```

Alle Bereiche besitzen eine definierte Reihenfolge.

---

# 8. Package Header

Der Header enthält mindestens:

- Magic Number
- Formatversion
- Paket-ID
- Paketname
- Paketversion
- Anzahl Ressourcen
- Offset zum Manifest

Der Header besitzt eine feste Größe.

---

# 9. Manifest

Das Manifest beschreibt:

- enthaltene Ressourcen
- Ressourcentypen
- Abhängigkeiten
- Themeinformationen
- Sprachinformationen
- Mindestversionen

## Verbindliches BAP-1-Wire-Format

Alle Integer sind Little Endian. Der Header ist exakt 64 Byte: `NOVABAP\0`
(0..7), Version u16 (8), Headergröße u16=64 (10), Gesamtgröße u32 (12),
Package-ID u64 (16), Flags u32 (24), Resource-Anzahl u32 (28), Index-Offset/
Größe u32 (32/36), Daten-Offset/Größe u32 (40/44), Manifest-Offset/Größe u32
(48/52), CRC32 u32 (56; beim Rechnen null) und reserviert u32=0 (60).
Bereiche sind 8-Byte-ausgerichtet, vollständig innerhalb der Gesamtgröße und
überlappen nicht. Bit0=`SIGNED`, Bit1=`COMPRESSED_INDEX`; unbekannte Bits
sind abzulehnen. Ohne Signatur ist CRC32 verpflichtend.

Das Manifest wird vor allen anderen Daten eingelesen.

---

# 10. Resource Index

Der Index enthält für jede Ressource:

- Resource-ID
- Offset
- Größe
- Typ
- Komprimierungsverfahren
- Prüfsumme

Der Index ermöglicht direkten Zugriff ohne lineares Durchsuchen.

---

# 11. Ressourcentypen

Unterstützt werden:

- Images
- Icons
- Fonts
- Themes
- Localization
- Animationen
- Cursor
- Binärdaten

Neue Typen können ergänzt werden.

---

# 12. Versionierung

Jedes Paket besitzt:

- Hauptversion
- Nebenversion
- Buildnummer

Zusätzlich besitzt jede Ressource eine eigene Versionsnummer.

---

# 13. Kompression

Optional unterstützt:

- keine Kompression
- LZ4
- Zstandard (Zstd)

Die verwendete Komprimierung wird pro Ressource angegeben.

---

# 14. Integrität

Für jede Ressource wird eine Prüfsumme gespeichert.

Zusätzlich besitzt das gesamte Paket eine Gesamtprüfsumme.

Beschädigte Ressourcen dürfen nicht geladen werden.

---

# 15. Digitale Signatur

Optional kann ein Paket digital signiert werden.

Die Signatur schützt:

- Manifest
- Resource Index
- Ressourcendaten

Nicht signierte Pakete können abhängig von der Systemrichtlinie abgelehnt werden.

---

# 16. Themeintegration

Ein Theme verweist ausschließlich auf Ressourcen innerhalb eines oder mehrerer Asset Packages.

Direkte Dateipfade sind nicht zulässig.

---

# 17. Lokalisierung

Sprachpakete können Bestandteil eines Asset Packages sein.

Die gewünschte Sprache wird anhand der aktiven Locale ausgewählt.

Nicht benötigte Sprachressourcen müssen nicht geladen werden.

---

# 18. Sicherheit

Das System verhindert:

- manipulierte Header
- ungültige Offsets
- doppelte Resource-IDs
- beschädigte Manifeste
- ungültige Prüfsummen
- Integerüberläufe

Alle Paketbestandteile werden vor der Registrierung validiert.

---

# 19. Speicherverwaltung

Das Asset-Package-System verwendet:

- statische Headerstrukturen
- feste Indexpuffer
- Objektpools

Ressourcendaten werden bedarfsgesteuert geladen.

Heap-Allokationen während der Laufzeit sind verboten.

---

# 20. Performance

Anforderungen:

- O(1)-Indexzugriff
- O(1)-Resource-Lookup
- O(n)-Manifestverarbeitung
- deterministische Laufzeit

Das Öffnen eines Pakets darf nicht zum vollständigen Laden aller Ressourcen führen.

---

# 21. API

```c
typedef struct
{
    uint32_t package_id;
    uint32_t version_major;
    uint32_t version_minor;
    uint32_t resource_count;
}
nova_asset_package_t;

nova_result_t
nova_asset_package_initialize(void);

nova_result_t
nova_asset_package_open(
    const char* path,
    nova_asset_package_t** package);

nova_result_t
nova_asset_package_close(
    nova_asset_package_t* package);

const nova_resource_descriptor_t*
nova_asset_package_find_resource(
    const nova_asset_package_t* package,
    uint32_t resource_id);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Paket-ID
- Paketversion
- Ressourcenanzahl
- Ladezeit
- Integritätsfehler
- Prüfsummenfehler
- Signaturstatus
- Speicherverbrauch

---

# 23. Testfälle

Zu testen sind:

- gültige Pakete
- beschädigte Header
- beschädigte Manifeste
- falsche Prüfsummen
- verschiedene Ressourcentypen
- Themepakete
- Sprachpakete
- komprimierte Ressourcen
- signierte Pakete
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- mehrere Ressourcen in einem Paket verwaltet werden können.
- Header, Manifest und Index korrekt verarbeitet werden.
- beschädigte Pakete erkannt und abgelehnt werden.
- Ressourcen bedarfsgesteuert geladen werden.
- Versionierung und Integritätsprüfung vollständig funktionieren.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 25. Referenzen

## ADRs

- ADR-BOOTRESOURCE-0002 – Boot Asset Package
- ADR-BOOTRESOURCE-0001 – Boot Resource Manager
- ADR-BOOTSECURITY-0001 – Resource Validation
- ADR-BOOTTHEME-0001 – Theme System

## NPSPECs

- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTFS-0001 – Boot File System
- NPSPEC-BOOTSECURITY-0001 – Boot Security
- NPSPEC-BOOTTHEME-0001 – Theme System
- NPSPEC-BOOTLOCALIZATION-0001 – Localization System

---

# 26. Zusammenfassung

Das **Boot Asset Package (BAP)** definiert das standardisierte Paketformat für Ressourcen der NovaOS-Bootoberfläche. Es bündelt Bilder, Schriftarten, Themes, Sprachpakete, Animationen und weitere Ressourcen in einer versionierten, validierbaren und optional signierten Einheit. Durch Manifest, Ressourcenindex, Integritätsprüfungen und bedarfsgesteuertes Laden stellt das BAP-Format eine effiziente, sichere und erweiterbare Grundlage für die Verteilung und Nutzung von Bootressourcen auf BIOS- und UEFI-basierten Systemen bereit.
