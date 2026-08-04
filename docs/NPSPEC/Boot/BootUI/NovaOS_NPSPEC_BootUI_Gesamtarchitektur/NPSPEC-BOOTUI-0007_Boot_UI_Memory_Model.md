# NPSPEC-BOOTUI-0007
# Boot UI Memory Model

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTUI-0007 |
| Titel | Boot UI Memory Model |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTUI-0001 bis NPSPEC-BOOTUI-0006 |
| ADRs | ADR-BOOTPERF-0001, ADR-BOOTPERF-0002, ADR-BOOTSAFE-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das vollständige Speicherverwaltungsmodell der NovaOS Boot-UI.

Das Memory Model beschreibt:

- Speicheraufteilung
- Speicherpools
- Allokationsregeln
- Lebensdauer aller Speicherobjekte
- Speichergrenzen
- Speicherfreigabe
- Cacheverwaltung
- Fragmentierungsvermeidung
- Speicherdiagnose
- Recovery nach Speicherfehlern

Die Boot-UI arbeitet **ohne klassischen Heap-Allocator**. Stattdessen werden feste Speicherbereiche und spezialisierte Speicherpools verwendet. Dieses Vorgehen ist typisch für frühe Bootphasen, da dort die reguläre Speicherverwaltung des Betriebssystems noch nicht verfügbar ist. Linux verwendet beispielsweise während des frühen Bootvorgangs einen separaten Boot-Memory-Allocator (`memblock`), bevor der normale Seitenallocator aktiv wird. :contentReference[oaicite:0]{index=0}

---

# 2. Architekturziele

Das Speichermodell verfolgt folgende Ziele:

- deterministische Speicherverwaltung
- keine Speicherfragmentierung
- keine Speicherlecks
- konstante Allokationszeiten
- reproduzierbares Verhalten
- vorhersehbarer Speicherverbrauch
- minimale Heap-Nutzung
- vollständige Diagnosefähigkeit

---

# 3. Speicherarchitektur

```
                Physical Memory
                       │
            Boot UI Memory Manager
                       │
      ┌────────────────┼────────────────┐
 Permanent Pool     Runtime Pool     Frame Pool
      │                 │               │
  Resource Pool    Scene Pool     Render Pool
      │                 │               │
 Cache Pool      Animation Pool  Temporary Pool
```

---

# 4. Speicherbereiche

Die Runtime unterteilt den Speicher in mehrere logisch getrennte Bereiche.

| Bereich | Zweck |
|----------|------|
| Permanent Pool | dauerhaft gültige Daten |
| Runtime Pool | Laufzeitdaten |
| Scene Pool | UI-Objekte |
| Animation Pool | Animationen |
| Frame Pool | temporäre Daten |
| Render Pool | Rendering |
| Resource Cache | Icons, Fonts, Bilder |
| Diagnostics | Statistiken |

---

# 5. Permanent Pool

Enthält Daten, die während der gesamten Boot-UI existieren.

Beispiele

- Runtime Context
- Theme
- Konfiguration
- Design Tokens
- Font Tabellen
- SVG Metadaten

Eigenschaften

- niemals verschoben
- niemals fragmentiert
- nur beim Shutdown freigegeben

---

# 6. Runtime Pool

Beinhaltet

- Runtime States
- Event Queue
- Navigation
- Dialogverwaltung
- Scheduler
- Diagnostics

Lebensdauer

```
Runtime Start
      ↓
Runtime Ende
```

---

# 7. Scene Pool

Alle UI-Objekte werden hier erzeugt.

Beispiele

- Controls
- Listen
- Dialoge
- Panels
- Fenster
- Overlays

Lebensdauer

```
Create
   ↓
Visible
   ↓
Destroy
```

---

# 8. Animation Pool

Speichert

- Keyframes
- Easing
- Spring States
- Timelines
- Interpolationen

Animationen werden nach Abschluss sofort freigegeben.

---

# 9. Frame Pool

Der wichtigste Speicherbereich.

Enthält ausschließlich temporäre Daten.

Beispiele

- Render Queue
- Clipping
- Dirty Regions
- Transformationsmatrizen
- temporäre Listen

Nach jedem Frame:

```
Frame Pool
    ↓
  Reset
    ↓
   leer
```

Es findet **keine Einzel-Freigabe** statt.

---

# 10. Render Pool

Enthält

- Vertex Buffer
- Draw Commands
- Layer Informationen
- Blend Daten
- Offscreen Surfaces

---

# 11. Cache Pool

Zwischenspeichert

- Fonts
- Glyphen
- SVG
- PNG
- Blur Masken
- Layout Berechnungen

Cacheobjekte besitzen

```
ID
↓
Hash
↓
Reference Count
↓
Timestamp
```

---

# 12. Speicherhierarchie

```
Permanent
↓
Runtime
↓
Scene
↓
Animation
↓
Frame
```

Jede Ebene darf ausschließlich darunterliegende Ebenen referenzieren.

Zyklische Referenzen sind verboten.

---

# 13. Speicherbudget

| Bereich | Maximale Größe |
|----------|---------------:|
| Permanent | 8 MB |
| Runtime | 4 MB |
| Scene | 16 MB |
| Animation | 2 MB |
| Render | 16 MB |
| Frame | 8 MB |
| Cache | 8 MB |
| Diagnose | 2 MB |

---

# 14. Gesamtspeicherbudget

Maximal:

```
64 MB
```

Das Budget darf niemals überschritten werden.

Bei Überschreitung erfolgt automatische Degradation.

---

# 15. Speicherobjekte

Jedes Objekt besitzt

```cpp
MemoryID

Size

Owner

Pool

ReferenceCount

Flags

State
```

---

# 16. Allokationsregeln

Alle Allokationen erfolgen ausschließlich über den Memory Manager.

Direkte Aufrufe von

```
malloc()

calloc()

realloc()

free()
```

sind im Boot-UI-Code verboten.

---

# 17. Speicherlebensdauer

```
Allocate
↓
Initialize
↓
Use
↓
Release
↓
Pool Reset
```

Jede Allokation besitzt einen definierten Besitzer.

---

# 18. Alignment

Alle Allokationen werden mindestens auf

```
16 Byte
```

ausgerichtet.

Spezielle Renderdaten

```
64 Byte
```

Alignment.

---

# 19. Referenzzählung

Gemeinsam genutzte Ressourcen verwenden

```
Reference Count
```

Beispiel

```
Font
↓
Dialog A
↓
Dialog B
↓
Dialog C
↓
Reference = 3
```

Erst bei

```
Reference = 0
```

erfolgt Freigabe.

---

# 20. Fragmentierung

Fragmentierung soll vollständig vermieden werden.

Methoden

- feste Pools
- Arena-Allokatoren
- Stack-Allokatoren
- Frame Reset

Es werden keine allgemeinen Heap-Algorithmen verwendet.

---

# 21. Speicherfehler

Erkannt werden

- Overflow
- Underflow
- Double Free
- Invalid Pointer
- Pool Overflow
- Null Pointer
- Alignment Fehler

Bei Fehler

```
Recovery
↓
Fallback
↓
Weiterarbeiten
```

---

# 22. Recovery

```
Falls ein Pool erschöpft ist
↓
Animationen deaktivieren
↓
Caches verkleinern
↓
Glass reduzieren
↓
Blur deaktivieren
↓
weiterarbeiten
```

---

# 23. Speicherdiagnose

Die Runtime misst permanent

- Gesamtspeicher
- Poolbelegung
- Peak Usage
- Fragmentierung
- Cache Hits
- Cache Misses
- Allokationen
- Freigaben

---

# 24. Debugmodus

Im Debugmodus besitzt jede Allokation

```cpp
Datei

Zeile

Owner

Pool

Zeitstempel
```

Dadurch können Speicherfehler exakt lokalisiert werden.

---

# 25. UEFI-Spezifika

Unter UEFI werden Speicherbereiche ausschließlich über die Boot Services reserviert.

Die Graphics Abstraction und Runtime erhalten ausschließlich bereits reservierte Bereiche.

Direkte Speicherverwaltung durch einzelne UI-Komponenten ist verboten. UEFI stellt dafür standardisierte Speicherfunktionen wie `AllocatePages()`, `AllocatePool()` und `GetMemoryMap()` bereit. :contentReference[oaicite:1]{index=1}

---

# 26. BIOS-Spezifika

Unter BIOS verwendet der Bootmanager die zuvor reservierten Speicherbereiche.

Alle Pools werden beim Runtime-Start initialisiert.

Eine nachträgliche Vergrößerung einzelner Pools ist nicht vorgesehen.

---

# 27. Performance

Zielwerte

| Operation | Ziel |
|-----------|------|
| Pool Allocation | < 100 ns |
| Frame Reset | < 1 ms |
| Cache Lookup | O(1) |
| Referenzänderung | O(1) |

Alle Pool-Operationen besitzen konstante Laufzeiten.

---

# 28. API

```cpp
MemoryInitialize()

AllocatePermanent()

AllocateRuntime()

AllocateScene()

AllocateFrame()

AllocateRender()

AllocateCache()

Release()

ResetFramePool()

GetStatistics()

ValidatePointer()
```

---

# 29. Testanforderungen

Es müssen getestet werden

## Speicher

- maximale Auslastung
- minimale Auslastung
- Peak Usage

## Fehler

- Pool Overflow
- Invalid Pointer
- Double Free
- Alignment Fehler

## Performance

- Allokationszeit
- Reset-Zeit
- Cache Hit Rate

## Recovery

- Speicherknappheit
- deaktivierte Animationen
- deaktivierter Blur
- reduzierte Glass-Effekte

---

# 30. Referenzen

## ADR

- ADR-BOOTPERF-0001 – Festes Speicherbudget für die Boot-UI
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath
- ADR-BOOTSAFE-0002 – UI-Fehler dürfen den Bootprozess nicht blockieren

## NPSPEC

- NPSPEC-BOOTUI-0001 – Bootmanager UI Architecture
- NPSPEC-BOOTUI-0002 – Boot UI Runtime
- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager

---

# 31. Zusammenfassung

Das **Boot UI Memory Model** definiert eine deterministische Speicherarchitektur für die gesamte Bootoberfläche von NovaOS. Anstelle eines klassischen Heaps verwendet die Runtime spezialisierte Speicherpools für permanente Daten, Laufzeitobjekte, Szenen, Animationen, Rendering und temporäre Frame-Daten. Durch feste Speicherbudgets, Arena- und Frame-Allocator, Referenzzählung sowie automatische Degradationsmechanismen bleibt der Speicherverbrauch jederzeit vorhersehbar und reproduzierbar. Dieses Modell gewährleistet hohe Performance, verhindert Fragmentierung und ermöglicht auch unter Speicherknappheit einen sicheren Weiterbetrieb der Bootoberfläche.