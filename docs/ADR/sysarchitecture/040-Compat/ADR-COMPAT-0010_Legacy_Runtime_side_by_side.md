# ADR-COMPAT-0010 – Legacy Runtime side-by-side

## Status

Angenommen

## Kategorie

Compatibility / Runtime / Versioning / Isolation

## Kontext

Legacy-Anwendungen können unterschiedliche oder sogar inkompatible Versionen derselben Runtime, Bibliothek oder Compatibility-Komponente benötigen.

Beispiele:

```text
Runtime v1
Runtime v2
Legacy Framework
Alte Systembibliothek
Spezifische Win32-/Linux-Komponente
```

Eine einzige globale Runtime-Version würde zu Versionskonflikten führen und könnte verhindern, dass ältere und neuere Anwendungen gleichzeitig funktionieren.

## Entscheidung

NovaOS unterstützt **mehrere Legacy-Runtime-Versionen side-by-side**.

```text
Application A → Runtime v1
Application B → Runtime v2
Application C → Runtime v3
```

Die benötigte Runtime wird dem jeweiligen Execution Context zugeordnet und nicht als globale Systemversion vorausgesetzt.

## Runtime Identity

Jede Runtime besitzt eine eindeutige Identität und Version.

```text
Runtime ID
Version
Compatibility Personality
Architecture
Dependencies
Trust State
```

Anwendungen können dadurch gezielt an eine kompatible Runtime gebunden werden.

## Isolation

Runtime-Versionen werden logisch voneinander getrennt.

```text
Application
    ↓
Runtime Environment
    ↓
Compatibility Domain
    ↓
NovaOS
```

Eine Anwendung darf nicht unbeabsichtigt Bibliotheken oder Zustand einer anderen Runtime-Version übernehmen.

## Dependency Resolution

Abhängigkeiten werden innerhalb des jeweiligen Runtime-Kontexts aufgelöst.

```text
Application
   ↓
Required Runtime
   ↓
Required Libraries
```

Systemweite Überschreibungen gemeinsam genutzter Legacy-Bibliotheken sollen vermieden werden.

## Gemeinsame Komponenten

Identische unveränderliche Komponenten dürfen intern gemeinsam genutzt werden, sofern dadurch keine semantische Kopplung entsteht.

```text
Runtime A ─┐
           ├─→ Shared Immutable Component
Runtime B ─┘
```

Die gemeinsame Nutzung ist eine Optimierung und darf die logische Isolation nicht verändern.

## Auswahl

Die passende Runtime kann anhand von:

```text
Executable Metadata
Compatibility Metadata
Declared Dependencies
Runtime Version
Architecture
```

bestimmt werden.

Fehlt eine kompatible Runtime, darf NovaOS eine andere Compatibility-Stufe verwenden oder die Ausführung definiert ablehnen.

## Updates

Eine neue Runtime-Version ersetzt ältere Versionen nicht automatisch.

```text
Runtime v1
Runtime v2
Runtime v3
```

können parallel vorhanden bleiben, solange sie benötigt und nach geltender Security Policy zulässig sind.

Unsichere oder nicht mehr erlaubte Runtime-Versionen können deaktiviert oder entfernt werden.

## Normative Anforderungen

1. NovaOS MUSS mehrere Legacy-Runtime-Versionen parallel unterstützen können.
2. Runtime-Versionen MÜSSEN eindeutig identifizierbar sein.
3. Anwendungen SOLLEN explizit einer kompatiblen Runtime zugeordnet werden.
4. Runtime-Abhängigkeiten MÜSSEN innerhalb des jeweiligen Execution Context auflösbar sein.
5. Eine Runtime-Version DARF eine andere nicht unbeabsichtigt überschreiben.
6. Gemeinsame Komponenten DÜRFEN nur verwendet werden, wenn die semantische Isolation erhalten bleibt.
7. Runtime-Updates DÜRFEN ältere benötigte Versionen nicht automatisch ersetzen.
8. Legacy-Runtimes MÜSSEN den aktuellen NovaOS-Sicherheitsregeln unterliegen.
9. Nicht mehr zulässige Runtime-Versionen MÜSSEN deaktivierbar oder entfernbar sein.

## Konsequenzen

### Positiv

- unterschiedliche Legacy-Anwendungen können gleichzeitig funktionieren,
- Versionskonflikte werden reduziert,
- Updates zerstören nicht automatisch ältere Software,
- Compatibility Domains bleiben reproduzierbar.

### Negativ

- mehrere Runtime-Versionen benötigen zusätzlichen Speicher,
- Security Updates müssen gegebenenfalls für mehrere Versionen gepflegt werden,
- Dependency Resolution wird komplexer.

## Abhängigkeiten

- `ADR-COMPAT-0003_Compatibility_Personalities`
- `ADR-COMPAT-0007_Execution_Compatibility_Ladder`
- `ADR-COMPAT-0008_Legacy_Software_in_Compatibility_Domains`
- `ADR-ARCH-0019_Evolvierbar`
- `ADR-OBJECT-0002`

## Ergebnis

NovaOS folgt für Legacy-Runtimes dem Prinzip:

```text
One System
   ↓
Multiple Runtime Versions
   ↓
Isolated per Application / Domain
```

Legacy-Anwendungen können damit ihre jeweils benötigte Runtime verwenden, ohne eine globale Runtime-Version für das gesamte System zu erzwingen.