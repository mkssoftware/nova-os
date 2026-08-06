# NPSPEC-BOOTRESOURCE-0005
# Resource Compression

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRESOURCE-0005 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Resource System |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTRESOURCE-0002, NPSPEC-BOOTRESOURCE-0003, NPSPEC-BOOTRESOURCE-0004, NPSPEC-BOOTFS-0001, NPSPEC-BOOTMEMORY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Resource Compression System** der NovaOS-Bootoberfläche.

Das System legt fest, wie Ressourcen innerhalb von Boot Asset Packages komprimiert, gespeichert und beim Laden wieder dekomprimiert werden. Ziel ist es, die Größe der Bootressourcen zu reduzieren, die Bootmedien effizienter zu nutzen und gleichzeitig kurze Ladezeiten sicherzustellen.

Die Komprimierung erfolgt transparent für alle Komponenten oberhalb des Resource Loaders.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Bilder
- Icons
- Schriftarten
- Themes
- Sprachpakete
- Animationen
- Cursor
- Binärressourcen

---

# 3. Ziele

Das Kompressionssystem muss:

- transparente Dekomprimierung ermöglichen
- mehrere Verfahren unterstützen
- deterministisch arbeiten
- Integrität erhalten
- schnellen Zugriff ermöglichen
- zukünftige Verfahren erweiterbar machen

---

# 4. Architektur

```text
Asset Package

↓

Resource Index

↓

Compression Resolver

↓

Decompressor

↓

Validator

↓

Resource Loader

↓

Resource Manager
```

---

# 5. Komponenten

Das System besteht aus:

- Compression Resolver
- Decompressor
- Compression Registry
- Integrity Checker
- Resource Loader Adapter
- Performance Monitor

---

# 6. Grundprinzip

Der Ablauf erfolgt wie folgt:

```text
Komprimierte Ressource

↓

Verfahren bestimmen

↓

Dekomprimieren

↓

Integrität prüfen

↓

Resource Loader

↓

Registry
```

Die Dekomprimierung erfolgt ausschließlich während des Ladevorgangs.

---

# 7. Unterstützte Verfahren

Pflicht:

- keine Komprimierung
- LZ4

Optional:

- Zstandard (Zstd)

Weitere Verfahren können zukünftig ergänzt werden.

---

# 8. Ressourcenspezifische Komprimierung

Die Wahl des Verfahrens erfolgt pro Ressource.

Beispiele:

| Ressourcentyp | Empfohlenes Verfahren |
|--------------|-----------------------|
| Bilder | LZ4 |
| Icons | LZ4 |
| Fonts | LZ4 |
| Sprachpakete | Zstandard |
| Themes | LZ4 |
| Binärdaten | Zstandard |

---

# 9. Ressourcenindex

Der Resource Index speichert:

- Komprimierungsverfahren
- Originalgröße
- Komprimierte Größe
- Prüfsumme

Der Loader entscheidet anhand dieser Informationen über die Dekomprimierung.

---

# 10. Dekomprimierung

Die Dekomprimierung erfolgt:

```text
Index

↓

Komprimierungsverfahren

↓

Dekomprimierung

↓

Integritätsprüfung

↓

Registrierung
```

Es wird ausschließlich in temporäre Arbeitsbereiche entpackt.

---

# 11. Integrität

Vor und nach der Dekomprimierung werden geprüft:

- Prüfsumme
- Originalgröße
- Ressourcentyp
- Versionsnummer

Fehlerhafte Ressourcen werden verworfen.

---

# 12. Speicherverwaltung

Die Dekomprimierung verwendet:

- feste Arbeitsbereiche
- statische Puffer
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 13. Lazy Loading

Komprimierte Ressourcen werden erst beim ersten Zugriff dekomprimiert.

Nicht verwendete Ressourcen verbleiben komprimiert im Asset Package.

---

# 14. Cacheintegration

Nach erfolgreicher Dekomprimierung:

```text
Resource

↓

Registry

↓

Cache

↓

Weitere Zugriffe
```

Eine Ressource wird nur einmal dekomprimiert.

---

# 15. Fehlerbehandlung

Kann eine Ressource nicht dekomprimiert werden:

```text
Dekomprimierung

↓

Fehler

↓

Fallback Resource

↓

Weiterarbeiten
```

Der Boot Manager bleibt funktionsfähig.

---

# 16. Sicherheit

Das System verhindert:

- manipulierte Kompressionsdaten
- ungültige Größenangaben
- Buffer Overflows
- beschädigte Datenströme
- Integerüberläufe

Alle Eingaben werden vollständig validiert.

---

# 17. Performance

Anforderungen:

- O(n)-Dekomprimierung
- O(1)-Verfahrensbestimmung
- deterministische Laufzeit

Das gewählte Verfahren darf die Bootzeit nur minimal beeinflussen.

---

# 18. API

```c
typedef enum
{
    NOVA_COMPRESSION_NONE,
    NOVA_COMPRESSION_LZ4,
    NOVA_COMPRESSION_ZSTD
}
nova_compression_type_t;

nova_result_t
nova_compression_initialize(void);

nova_result_t
nova_resource_decompress(
    nova_compression_type_t type,
    const void* source,
    size_t source_size,
    void* destination,
    size_t destination_size);

bool
nova_compression_supported(
    nova_compression_type_t type);
```

---

# 19. Diagnosedaten

Zu erfassen sind:

- Komprimierungsverfahren
- Originalgröße
- Komprimierte Größe
- Kompressionsverhältnis
- Dekomprimierungszeit
- Integritätsfehler
- Speicherverbrauch

---

# 20. Testfälle

Zu testen sind:

- unkomprimierte Ressourcen
- LZ4
- Zstandard
- beschädigte Daten
- ungültige Größen
- Cache-Wiederverwendung
- Lazy Loading
- Integritätsprüfung
- Fehlerfälle

---

# 21. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- alle unterstützten Kompressionsverfahren korrekt verarbeitet werden.
- Ressourcen transparent dekomprimiert werden.
- Integritätsprüfungen vor der Registrierung erfolgen.
- jede Ressource höchstens einmal dekomprimiert wird.
- Lazy Loading vollständig unterstützt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 22. Referenzen

## ADRs

- ADR-BOOTRESOURCE-0005 – Resource Compression
- ADR-BOOTRESOURCE-0002 – Boot Asset Package
- ADR-BOOTRESOURCE-0004 – Resource Loading
- ADR-BOOTMEMORY-0001 – Memory Management

## NPSPECs

- NPSPEC-BOOTRESOURCE-0002 – Boot Asset Package
- NPSPEC-BOOTRESOURCE-0003 – Resource Index
- NPSPEC-BOOTRESOURCE-0004 – Resource Loading
- NPSPEC-BOOTFS-0001 – Boot File System
- NPSPEC-BOOTMEMORY-0001 – Boot Memory Management

---

# 23. Zusammenfassung

Das **Resource Compression System** definiert die standardisierte Komprimierung und Dekomprimierung von Ressourcen innerhalb der NovaOS-Bootoberfläche. Es unterstützt mehrere Kompressionsverfahren, integriert sich nahtlos in den Resource Loader und ermöglicht bedarfsgesteuertes Laden komprimierter Ressourcen. Durch Integritätsprüfungen, deterministische Verarbeitung, statische Speicherverwaltung und transparente Cache-Integration stellt das System eine effiziente und sichere Nutzung von Bootressourcen auf BIOS- und UEFI-basierten Systemen sicher.