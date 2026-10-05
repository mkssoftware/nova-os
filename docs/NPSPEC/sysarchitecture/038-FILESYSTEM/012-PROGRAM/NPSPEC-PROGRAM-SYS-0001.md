# NPSPEC-PROGRAM-SYS-0001 – Nova Program SYS

## Status

Angenommen

## Kategorie

Program / SYS / Dependencies

## Zweck

NovaOS definiert `SYS` als privaten Systembereich eines klassischen Programms.

Ein Programm kann darin eigene Laufzeiten, Bibliotheken und Systemabhängigkeiten mitführen, ohne das globale `/System` zu verändern.

## Grundprinzipien

```text
Program SYS ≠ Global /System
Private Dependency ≠ Global Dependency
SYS Overlay ≠ Physical Copy
SYS Access ≠ Global System Authority
```

`SYS` gehört vollständig zum jeweiligen Program Package.

## Struktur

Der private Bereich liegt logisch unter:

```text
/Apps/<Program>/SYS/
├── Libraries/
├── Runtime/
└── Dependencies/
```

Weitere programmspezifische Bereiche dürfen ergänzt werden.

## SYS Overlay

Beim Start eines Programms kann NovaOS dessen privaten `SYS`-Bereich in den für dieses Programm sichtbaren System-Namespace projizieren.

```text
Global /System
      +
/Apps/<Program>/SYS
      ↓
Effective /System
```

Das Programm kann seine privaten Abhängigkeiten dadurch so verwenden, als wären sie Teil seiner Systemumgebung.

Physisch bleiben sie jedoch im Program Package.

## Isolation

Jedes Programm besitzt seinen eigenen `SYS`-Kontext.

```text
Program A → SYS A
Program B → SYS B
```

Private Abhängigkeiten eines Programms dürfen nicht automatisch für andere Programme sichtbar oder wirksam werden.

## Auflösung

Die Auflösung muss deterministisch erfolgen.

```text
Dependency Request
       ↓
Program SYS Overlay
       ↓
Global System
       ↓
Resolved Component
```

Welche Quelle Vorrang besitzt, wird durch die NovaOS-Overlay- und Dependency-Policy bestimmt.

Eine private Komponente darf eine sicherheitskritische globale Komponente nicht unkontrolliert ersetzen.

## Updates

Private Abhängigkeiten können gemeinsam mit dem Program Package installiert, aktualisiert und entfernt werden.

Dadurch dürfen unterschiedliche Programme unterschiedliche Versionen derselben Abhängigkeit verwenden.

```text
Program A → Runtime 1
Program B → Runtime 2
```

ohne die globale Systemumgebung gegenseitig zu verändern.

## Globales System

Benötigt ein Programm tatsächlich eine Änderung an `/System`, ist dies keine `SYS`-Operation.

Eine solche Änderung benötigt eine separate, explizite Systemberechtigung.

## Sicherheit

Der `SYS`-Overlay erzeugt keine zusätzliche Authority.

Private Komponenten laufen innerhalb des Sicherheitskontexts des jeweiligen Programms und unterliegen dessen Capabilities und Policies.

## Normative Anforderungen

1. Jedes Program Package DARF einen privaten `SYS`-Bereich besitzen.
2. `SYS` MUSS physisch vom globalen `/System` getrennt bleiben.
3. Private `SYS`-Komponenten DÜRFEN ausschließlich im jeweiligen Programmkontext eingeblendet werden.
4. Unterschiedliche Programme DÜRFEN unterschiedliche Versionen derselben Abhängigkeit verwenden.
5. Die Overlay-Auflösung MUSS deterministisch sein.
6. Private Abhängigkeiten DÜRFEN andere Programme nicht automatisch beeinflussen.
7. Ein `SYS`-Overlay DARF keine globale Systemänderung erzeugen.
8. Änderungen am globalen `/System` MÜSSEN separat autorisiert werden.
9. Sicherheitskritische globale Komponenten DÜRFEN nicht unkontrolliert überschrieben werden.
10. Das Entfernen eines Programms SOLL dessen private `SYS`-Abhängigkeiten vollständig entfernen können.

## Abhängigkeiten

- `NPSPEC-PROGRAM-PACKAGE-0001`
- `NPSPEC-PROGRAM-MANIFEST-0001`
- `NPSPEC-PROGRAM-LAYOUT-0001`
- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-USERSPACE-PERMISSION-0001`

## Ergebnis

NovaOS ermöglicht klassischen Programmen vollständig private Systemabhängigkeiten über einen programmspezifischen `SYS`-Overlay. Dadurch können Programme ihre benötigten Laufzeiten und Bibliotheken unabhängig voneinander mitführen, ohne das globale `/System` zu verändern.