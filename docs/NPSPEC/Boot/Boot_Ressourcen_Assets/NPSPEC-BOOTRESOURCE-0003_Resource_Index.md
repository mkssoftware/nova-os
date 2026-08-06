# NPSPEC-BOOTRESOURCE-0003
# Resource Index

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRESOURCE-0003 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Resource System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTRESOURCE-0002, NPSPEC-BOOTFS-0001, NPSPEC-BOOTMEMORY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert den **Resource Index** der NovaOS-Bootoberfläche.

Der Resource Index ist die zentrale Nachschlagetabelle innerhalb eines Boot Asset Packages sowie des Boot Resource Managers. Er ermöglicht den direkten Zugriff auf Ressourcen anhand ihrer eindeutigen Resource-ID, ohne das gesamte Paket durchsuchen zu müssen.

Der Index bildet die Grundlage für schnelles Laden, Lazy Loading, Integritätsprüfungen und Referenzverwaltung.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Boot Asset Packages
- Boot Resource Manager
- Theme-System
- Font-System
- Bildverwaltung
- Lokalisierung
- Animationen
- sämtliche registrierten Ressourcentypen

---

# 3. Ziele

Der Resource Index muss:

- O(1)-Lookup ermöglichen
- deterministisch arbeiten
- eindeutige Resource-IDs verwalten
- Integritätsinformationen enthalten
- Lazy Loading unterstützen
- erweiterbar sein

---

# 4. Architektur

```text
Boot Asset Package

↓

Package Header

↓

Resource Index

↓

Resource Descriptor

↓

Resource Loader

↓

Resource Manager

↓

Subsysteme
```

---

# 5. Komponenten

Der Resource Index besteht aus:

- Index Header
- Resource Table
- Lookup Table
- Hash Table
- Offset Table
- Validation Engine

---

# 6. Grundprinzip

Alle Ressourcen werden ausschließlich über ihre Resource-ID gefunden.

Ablauf:

```text
Resource-ID

↓

Lookup

↓

Indexeintrag

↓

Offset

↓

Ressource laden
```

Ein lineares Durchsuchen des Pakets ist nicht zulässig.

---

# 7. Indexstruktur

Der Index besteht aus:

```text
Index Header

↓

Resource Entries

↓

Hash Lookup

↓

Offset Table
```

Alle Bereiche besitzen feste Offsets.

---

# 8. Resource Descriptor

Jeder Indexeintrag enthält mindestens:

- Resource-ID
- Ressourcentyp
- Paket-ID
- Dateioffset
- Größe
- Komprimierungsverfahren
- Prüfsumme
- Versionsnummer
- Status

Alle Einträge besitzen eine feste Struktur.

---

# 9. Resource-ID

Eigenschaften:

- eindeutig
- unveränderlich
- 32-Bit oder 64-Bit
- systemweit eindeutig innerhalb des Boot Managers

Die ID dient als primärer Schlüssel.

---

# 10. Hash Lookup

Der Index verwendet eine Hash-basierte Lookup-Struktur.

Eigenschaften:

- deterministisch
- kollisionsarm
- konstante Zugriffszeit

Hash-Kollisionen werden über definierte Kollisionsstrategien behandelt.

---

# 11. Offset-Verwaltung

Jede Ressource besitzt:

- Startoffset
- Endoffset
- Größe

Offsets beziehen sich auf das Boot Asset Package.

Überlappende Offsets sind unzulässig.

---

# 12. Komprimierung

Der Index enthält Informationen über:

- keine Komprimierung
- LZ4
- Zstandard

Der Loader entscheidet anhand des Indexeintrags über die Dekomprimierung.

---

# 13. Integrität

Jeder Eintrag besitzt:

- Prüfsumme
- Versionsnummer
- Validierungsstatus

Vor dem Laden wird die Integrität überprüft.

---

# 14. Lazy Loading

Der Resource Index ermöglicht:

```text
Lookup

↓

Offset

↓

Nur benötigte Ressource laden
```

Nicht verwendete Ressourcen verbleiben auf dem Datenträger.

---

# 15. Erweiterbarkeit

Neue Ressourcentypen können ergänzt werden, ohne die Indexstruktur zu ändern.

Nicht unterstützte Typen werden ignoriert oder entsprechend der Systemrichtlinie behandelt.

---

# 16. Sicherheit

Das System verhindert:

- doppelte Resource-IDs
- ungültige Offsets
- überlappende Bereiche
- beschädigte Indexeinträge
- manipulierte Prüfsummen
- Integerüberläufe

Alle Einträge werden beim Öffnen eines Asset Packages validiert.

---

# 17. Speicherverwaltung

Der Resource Index verwendet:

- statische Lookup-Tabellen
- feste Descriptor-Puffer
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Lookup
- O(1)-Hash-Zugriff
- O(1)-Offset-Ermittlung
- O(n)-Indexinitialisierung

Der Index wird ausschließlich einmal beim Öffnen eines Asset Packages aufgebaut.

---

# 19. API

```c
typedef struct
{
    uint64_t resource_id;
    uint32_t resource_type;
    uint64_t offset;
    uint64_t size;
    uint32_t compression;
    uint32_t checksum;
    uint32_t version;
}
nova_resource_descriptor_t;

nova_result_t
nova_resource_index_initialize(void);

const nova_resource_descriptor_t*
nova_resource_index_lookup(
    uint64_t resource_id);

bool
nova_resource_index_validate(void);

uint32_t
nova_resource_index_count(void);
```

---

# 20. Diagnosedaten

Zu erfassen sind:

- Anzahl Indexeinträge
- Hash-Kollisionen
- Lookup-Zeit
- Integritätsfehler
- ungültige Offsets
- doppelte Resource-IDs
- Speicherverbrauch

---

# 21. Testfälle

Zu testen sind:

- eindeutige Resource-IDs
- Hash Lookup
- Offset-Ermittlung
- beschädigte Einträge
- doppelte IDs
- Integritätsprüfung
- komprimierte Ressourcen
- Lazy Loading
- Fehlerfälle

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- jede Ressource eindeutig über ihre Resource-ID gefunden wird.
- Lookup-Operationen deterministisch und performant erfolgen.
- Integritätsprüfungen vor dem Laden durchgeführt werden.
- Lazy Loading vollständig unterstützt wird.
- beschädigte oder doppelte Indexeinträge erkannt werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 23. Referenzen

## ADRs

- ADR-BOOTRESOURCE-0003 – Resource Index
- ADR-BOOTRESOURCE-0001 – Boot Resource Manager
- ADR-BOOTRESOURCE-0002 – Boot Asset Package
- ADR-BOOTMEMORY-0001 – Memory Management

## NPSPECs

- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTRESOURCE-0002 – Boot Asset Package
- NPSPEC-BOOTFS-0001 – Boot File System
- NPSPEC-BOOTMEMORY-0001 – Boot Memory Management

---

# 24. Zusammenfassung

Der **Resource Index** definiert die zentrale Nachschlagetabelle für sämtliche Ressourcen innerhalb der NovaOS-Bootoberfläche. Er verwaltet eindeutige Resource-IDs, Offsets, Größen, Versionen und Integritätsinformationen und ermöglicht dadurch einen direkten, deterministischen Zugriff auf Ressourcen ohne lineares Durchsuchen von Asset Packages. Durch Hash-basierten Lookup, Unterstützung für Lazy Loading, statische Speicherverwaltung und umfassende Validierungsmechanismen bildet der Resource Index die Grundlage für ein performantes und sicheres Ressourcenmanagement auf BIOS- und UEFI-basierten Systemen.