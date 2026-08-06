# NPSPEC-BOOTRESOURCE-0004
# Resource Loading

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRESOURCE-0004 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Resource System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTRESOURCE-0002, NPSPEC-BOOTRESOURCE-0003, NPSPEC-BOOTFS-0001, NPSPEC-BOOTMEMORY-0001, NPSPEC-BOOTSECURITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Resource Loading System** der NovaOS-Bootoberfläche.

Das System übernimmt das Auffinden, Validieren, Laden und Registrieren sämtlicher Ressourcen innerhalb des Boot Managers. Es arbeitet eng mit dem Resource Manager, dem Resource Index und den Boot Asset Packages zusammen und stellt sicher, dass Ressourcen nur bei Bedarf geladen werden.

Das Laden erfolgt vollständig deterministisch und unabhängig von BIOS oder UEFI.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Bilder
- Icons
- Fonts
- Themes
- Animationen
- Sprachpakete
- Cursor
- Binärressourcen
- zukünftige Ressourcentypen

---

# 3. Ziele

Das Resource Loading System muss:

- Lazy Loading unterstützen
- Integrität prüfen
- Ressourcen eindeutig identifizieren
- mehrfaches Laden verhindern
- deterministisch arbeiten
- zukünftige Ressourcentypen unterstützen

---

# 4. Architektur

```text
Resource Request

↓

Resource Manager

↓

Resource Index

↓

Asset Package

↓

Resource Loader

↓

Validator

↓

Registry

↓

Cache

↓

Anwendung
```

---

# 5. Komponenten

Das System besteht aus:

- Resource Loader
- Package Reader
- Resource Validator
- Resource Registry
- Cache Manager
- Loader Dispatcher
- Error Handler

---

# 6. Grundprinzip

Der Ladevorgang erfolgt in mehreren Schritten:

```text
Resource-ID

↓

Lookup

↓

Package öffnen

↓

Integrität prüfen

↓

Laden

↓

Dekodieren

↓

Registrieren

↓

Cache
```

Nur erfolgreich validierte Ressourcen dürfen verwendet werden.

---

# 7. Ladearten

Unterstützt werden:

### Lazy Loading

Ressourcen werden erst beim ersten Zugriff geladen.

### Preloading

Ressourcen werden bereits beim Start geladen.

### Background Loading (optional)

Nicht kritische Ressourcen können im Hintergrund vorbereitet werden.

---

# 8. Resource Lookup

Jeder Ladevorgang beginnt mit:

```text
Resource-ID

↓

Resource Index

↓

Descriptor
```

Direkte Dateipfade dürfen nicht verwendet werden.

---

# 9. Integritätsprüfung

Vor dem Laden werden geprüft:

- Paketheader
- Indexeintrag
- Prüfsumme
- Versionsnummer
- Ressourcentyp
- Größenangaben

Ungültige Ressourcen werden verworfen.

---

# 10. Dekodierung

Nach erfolgreichem Laden erfolgt optional:

- Dekomprimierung
- Formatprüfung
- Headerprüfung
- interne Konvertierung

Das Ergebnis wird an den Resource Manager übergeben.

---

# 11. Registrierung

Nach erfolgreicher Dekodierung:

```text
Resource

↓

Registry

↓

Cache

↓

Reference Count = 1
```

Eine Ressource wird niemals mehrfach registriert.

---

# 12. Cache-Integration

Bereits geladene Ressourcen werden direkt aus dem Cache geliefert.

Ein erneuter Dateizugriff findet nicht statt.

---

# 13. Fehlerbehandlung

Kann eine Ressource nicht geladen werden:

```text
Laden

↓

Fehler

↓

Fallback

↓

Default Resource
```

Das System bleibt weiterhin funktionsfähig.

---

# 14. Abhängigkeiten

Eine Ressource kann weitere Ressourcen benötigen.

Beispiel:

```text
Theme

↓

Icons

↓

Fonts

↓

Images
```

Abhängigkeiten werden automatisch geladen.

Zyklische Abhängigkeiten sind unzulässig.

---

# 15. Prioritäten

Unterstützt werden:

- Kritisch
- Hoch
- Normal
- Niedrig

Kritische Ressourcen werden bevorzugt geladen.

---

# 16. Sicherheit

Das System verhindert:

- doppelte Ladevorgänge
- beschädigte Ressourcen
- manipulierte Pakete
- ungültige Offsets
- rekursive Abhängigkeiten
- Integerüberläufe

Alle Ressourcen werden vor der Registrierung validiert.

---

# 17. Speicherverwaltung

Das Resource Loading System verwendet:

- statische Loaderstrukturen
- Objektpools
- feste Puffer

Heap-Allokationen während des laufenden Betriebs sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Resource Lookup
- O(1)-Cache Lookup
- O(n)-Dekodierung
- deterministische Laufzeit

Bereits geladene Ressourcen dürfen niemals erneut eingelesen werden.

---

# 19. API

```c
typedef enum
{
    NOVA_LOAD_LAZY,
    NOVA_LOAD_PRELOAD,
    NOVA_LOAD_BACKGROUND
}
nova_resource_load_mode_t;

nova_result_t
nova_resource_loader_initialize(void);

nova_result_t
nova_resource_load(
    uint64_t resource_id,
    nova_resource_load_mode_t mode,
    nova_resource_t** resource);

bool
nova_resource_loaded(
    uint64_t resource_id);

nova_result_t
nova_resource_unload(
    uint64_t resource_id);
```

---

# 20. Diagnosedaten

Zu erfassen sind:

- Resource-ID
- Ladezeit
- Ladeart
- Cache-Hits
- Cache-Misses
- Integritätsfehler
- Dekomprimierungszeit
- Speicherverbrauch

---

# 21. Testfälle

Zu testen sind:

- Lazy Loading
- Preloading
- Cache-Wiederverwendung
- beschädigte Ressourcen
- ungültige Prüfsummen
- Ressourcenketten
- Fallback-Ressourcen
- mehrfaches Laden
- Fehlerfälle

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Ressourcen ausschließlich über den Resource Loader geladen werden.
- Lazy Loading und Preloading korrekt funktionieren.
- Integritätsprüfungen vor jeder Registrierung durchgeführt werden.
- bereits geladene Ressourcen aus dem Cache verwendet werden.
- rekursive Abhängigkeiten erkannt werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 23. Referenzen

## ADRs

- ADR-BOOTRESOURCE-0004 – Resource Loading
- ADR-BOOTRESOURCE-0001 – Boot Resource Manager
- ADR-BOOTRESOURCE-0002 – Boot Asset Package
- ADR-BOOTSECURITY-0001 – Resource Validation

## NPSPECs

- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTRESOURCE-0002 – Boot Asset Package
- NPSPEC-BOOTRESOURCE-0003 – Resource Index
- NPSPEC-BOOTFS-0001 – Boot File System
- NPSPEC-BOOTMEMORY-0001 – Boot Memory Management
- NPSPEC-BOOTSECURITY-0001 – Boot Security

---

# 24. Zusammenfassung

Das **Resource Loading System** definiert den standardisierten Ladeprozess sämtlicher Ressourcen innerhalb der NovaOS-Bootoberfläche. Es nutzt den Resource Index zur eindeutigen Identifikation, validiert jede Ressource vor der Verwendung und integriert sie anschließend in Registry und Cache. Durch Unterstützung von Lazy Loading, Preloading, automatischer Abhängigkeitsauflösung, Integritätsprüfungen und statischer Speicherverwaltung stellt das System eine performante, sichere und deterministische Ressourcenbereitstellung für BIOS- und UEFI-basierte Bootumgebungen sicher.