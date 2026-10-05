# NPSPEC-PROGRAM-SYSOVERLAY-0001 – Nova Program SYS Overlay

## Status

Angenommen

## Kategorie

Program / SYS / Overlay

## Zweck

NovaOS definiert den SYS Overlay als programmspezifische Projektion privater Systemabhängigkeiten in den effektiven `/System`-Namespace eines Programms.

Dadurch kann ein Programm private Bibliotheken, Laufzeiten und Abhängigkeiten wie Systemkomponenten verwenden, ohne das globale `/System` zu verändern.

## Grundprinzipien

```text
SYS Overlay ≠ Global Installation
SYS Overlay ≠ Copy
Effective /System ≠ Global /System
Private Component ≠ Global Component
Overlay Visibility ≠ Authority
```

Der Overlay existiert ausschließlich im Kontext des jeweiligen Programms.

## Modell

```text
Global /System
      +
/Apps/<Program>/SYS
      ↓
Program SYS Overlay
      ↓
Effective /System
```

Andere Programme und das globale System sehen weiterhin ihre jeweils eigene Systemansicht.

## Auflösung

Eine Anfrage innerhalb des Programms wird gegen dessen effektiven System-Namespace aufgelöst:

```text
Program Request
      ↓
Effective /System
      ↓
Overlay Resolution
      ├── Private SYS
      └── Global /System
```

Die Auflösungsreihenfolge muss deterministisch und durch Policy definiert sein.

## Kontextbindung

Der Overlay wird an den Sicherheits- und Ausführungskontext des Programms gebunden.

```text
Program A → Global System + SYS A
Program B → Global System + SYS B
```

Private Komponenten dürfen dadurch nicht unbeabsichtigt in andere Programmkontexte gelangen.

## Lebenszyklus

```text
Program Start
     ↓
Resolve SYS
     ↓
Validate Overlay
     ↓
Create Projection
     ↓
Program Execution
     ↓
Destroy Projection
```

Der Overlay darf dynamisch erzeugt und nach Ende des Programms wieder entfernt werden.

## Konflikte

Existiert eine private und globale Komponente mit gleicher logischer Position, entscheidet die definierte Overlay-Policy.

Sicherheitskritische oder ausdrücklich geschützte Systemkomponenten dürfen nicht durch private Komponenten unkontrolliert überschrieben werden.

Inkompatible oder mehrdeutige Auflösungen müssen erkannt werden.

## Updates

Updates eines Programms dürfen dessen privaten SYS-Inhalt und Overlay-Konfiguration ändern.

Andere Programme und das globale `/System` bleiben davon unberührt.

## Sicherheit

Der Overlay verändert ausschließlich die Sicht auf Ressourcen.

```text
Projection
    ↓
ObjectID
    ↓
Capability Check
    ↓
Authorized Handle
```

Eine durch den Overlay sichtbare Ressource erzeugt keine zusätzliche Authority.

## Normative Anforderungen

1. NovaOS MUSS programmspezifische SYS Overlays unterstützen können.
2. Der Overlay MUSS auf den jeweiligen Programmkontext begrenzt sein.
3. Private SYS-Komponenten MÜSSEN physisch vom globalen `/System` getrennt bleiben.
4. Der Overlay DARF keine Dateien in das globale `/System` kopieren.
5. Die Overlay-Auflösung MUSS deterministisch sein.
6. Unterschiedliche Programme MÜSSEN unterschiedliche SYS Overlays gleichzeitig verwenden können.
7. Private Komponenten DÜRFEN andere Programmkontexte nicht automatisch beeinflussen.
8. Geschützte Systemkomponenten DÜRFEN nicht unkontrolliert überschrieben werden.
9. Overlay-Sichtbarkeit DARF keine zusätzliche Authority erzeugen.
10. Der Overlay MUSS kontrolliert erstellt, aktualisiert und entfernt werden können.

## Abhängigkeiten

- `NPSPEC-PROGRAM-SYS-0001`
- `NPSPEC-PROGRAM-PACKAGE-0001`
- `NPSPEC-PROGRAM-MANIFEST-0001`
- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-USERSPACE-PERMISSION-0001`

## Ergebnis

NovaOS kann für jedes klassische Programm einen eigenen effektiven `/System`-Namespace erzeugen. Private Abhängigkeiten werden dabei ausschließlich als kontrollierter Overlay eingeblendet, sodass unterschiedliche Programme verschiedene Laufzeiten und Bibliotheken verwenden können, ohne das globale System gegenseitig zu verändern.