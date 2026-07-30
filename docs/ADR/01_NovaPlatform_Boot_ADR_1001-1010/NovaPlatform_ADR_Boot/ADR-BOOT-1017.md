# ADR-BOOT-1017: OpenType/TTF nicht direkt im Stage-1-Code (Teil 1)

* **Dokumenttyp:** Architecture Decision Record (ADR)
* **Dokument-ID:** ADR-BOOT-1017
* **Titel:** OpenType/TTF nicht direkt im Stage-1-Code
* **Version:** 1.0.0
* **Status:** Angenommen
* **Kategorie:** Boot Architecture / Typography
* **Entscheidungsdatum:** 2026-07-29
* **Gültig für:** Stage-1, Stage-2, Bootloader, Grafiksystem und Schriftdarstellung

---

# Referenzen

* ADR-BOOT-1013 – Stage-1 bleibt minimal
* ADR-BOOT-1016 – Framebuffer-Abstraktion statt direkter VBE-Nutzung
* ADR-BOOT-0004 – ELF als Kernel-Format
* NPSPEC-BOOTERROR-0001
* NPSPEC-GRAPHICS-0001 *(geplant)*

---

# 1. Kontext

NovaOS soll bereits während des Bootvorgangs eine moderne grafische Oberfläche besitzen.

Dazu gehören beispielsweise:

* Bootlogo
* Ladeanimation
* Bootmenü
* Recovery
* Diagnose
* Panic Screen

Alle diese Oberflächen benötigen Textdarstellung.

Langfristig soll NovaOS OpenType- und TrueType-Schriftarten verwenden.

OpenType-Schriftdateien bestehen jedoch aus einer Vielzahl standardisierter Tabellen und können TrueType- oder CFF-Konturen enthalten.

---

# 2. Problemstellung

Es muss entschieden werden, ob Stage-1 bereits einen vollständigen TrueType-/OpenType-Parser enthält.

Ein solcher Parser müsste unter anderem unterstützen:

* Tabellenverzeichnis
* Glyphen
* CMaps
* Metrics
* Hinting
* Kerning
* Rasterisierung
* Skalierung

Dies würde die erste Bootstufe erheblich vergrößern.

---

# 3. Entscheidung

Stage-1 enthält **keinen** OpenType- oder TrueType-Parser.

Stage-1 kennt weder:

* `.ttf`
* `.otf`
* `.ttc`
* Glyphen
* Unicode
* Hinting

Stage-1 besitzt keinerlei Schriftlogik.

---

# 4. Ziele

Die Entscheidung verfolgt folgende Ziele:

* minimale Stage-1
* geringe Binärgröße
* einfache Wartbarkeit
* klare Bootarchitektur
* reduzierte Fehlerfläche
* schnelle Initialisierung

---

# 5. Grundprinzip

Stage-1 übernimmt ausschließlich:

```text
Initialisierung

↓

Stage-2 laden

↓

Kontrolle übergeben
```

Nicht mehr.

---

# 6. Warum kein TTF?

Ein moderner Fontparser gehört zu den komplexesten Dateiparsern eines Betriebssystems.

OpenType-Dateien bestehen aus einem Tabellenverzeichnis sowie zahlreichen optionalen und obligatorischen Tabellen für Layout, Glyphen und Metadaten.

---

# 7. Komplexität

Ein vollständiger Parser müsste unter anderem verstehen:

* cmap
* glyf
* loca
* head
* hhea
* hmtx
* maxp
* name
* OS/2
* post

Je nach Font kommen weitere Tabellen hinzu.

---

# 8. Rasterisierung

Zusätzlich wäre erforderlich:

* Bézier-Kurven
* Konturfüllung
* Anti-Aliasing
* Hinting
* Skalierung

Dies widerspricht dem Minimalitätsprinzip von Stage-1.

---

# 9. Sicherheitsaspekt

Dateiformatparser gehören zu den häufigsten Ursachen für Speicherfehler.

Je größer der Parser,

* desto größer die Angriffsfläche,
* desto höher der Testaufwand,
* desto schwieriger die Wartung.

---

# 10. Stage-1 besitzt keine Fonts

Stage-1 enthält:

* keine TTF-Dateien
* keine OTF-Dateien
* keine Font Collections
* keine Unicode-Tabellen

---

# 11. Schriftdarstellung

Falls Stage-1 überhaupt Text ausgibt, erfolgt dies ausschließlich über:

* BIOS-Textmodus
* sehr kleine Bitmapfonts
* fest eingebettete Glyphen

---

# 12. Bitmapfonts

Stage-1 darf einen kleinen Bitmapfont verwenden.

Beispielsweise:

```text
8 × 8

8 × 16

16 × 16
```

Diese Fonts bestehen lediglich aus Pixelmustern.

---

# 13. Warum Bitmap?

Bitmapfonts besitzen entscheidende Vorteile:

* extrem klein
* sofort verwendbar
* keine Parser
* keine Kurven
* keine Skalierung
* deterministische Darstellung

---

# 14. Typischer Speicherbedarf

Ein ASCII-8×16-Font benötigt lediglich wenige Kilobyte.

Ein vollständiger OpenType-Font kann dagegen mehrere hundert Kilobyte oder sogar mehrere Megabyte groß sein.

---

# 15. Unicode

Stage-1 implementiert kein vollständiges Unicode-System.

Es unterstützt höchstens:

* ASCII
* begrenzte Zeichensätze
* Fehlercodes

---

# 16. Internationalisierung

Mehrsprachige Bootoberflächen gehören in spätere Bootstufen.

Nicht in Stage-1.

---

# 17. Hinting

TrueType-Hinting ist ein eigenes Instruktionssystem innerhalb des Fontformats und erhöht die Komplexität erheblich.

Stage-1 implementiert deshalb kein Hinting.

---

# 18. Kerning

Kerning wird nicht unterstützt.

---

# 19. Ligaturen

Ligaturen werden nicht unterstützt.

---

# 20. OpenType Layout

Features wie:

* GSUB
* GPOS
* komplexe Schriftformen

gehören ausschließlich in spätere Systemkomponenten.

---

# 21. Verantwortlichkeit

Die Verantwortung lautet:

```text
Stage-1

↓

keine Fonts

↓

Stage-2

↓

optionale Fontbibliothek

↓

Kernel

↓

vollständige Typografie
```

---

# 22. Architekturprinzip

Je früher eine Bootstufe ausgeführt wird,

desto kleiner

und deterministischer

muss sie bleiben.

Ein Fontparser verletzt dieses Prinzip.

---

# 23. Zusammenfassung Teil 1

Stage-1 enthält bewusst **keinen** OpenType- oder TrueType-Parser. Falls überhaupt Text ausgegeben wird, erfolgt dies ausschließlich über einen kleinen fest eingebetteten Bitmapfont oder den BIOS-Textmodus. Die Unterstützung moderner Vektorschriften beginnt erst in späteren Bootstufen, in denen ausreichend Speicher, Rechenleistung und Infrastruktur für sichere und vollständige Fontverarbeitung vorhanden sind.



# ADR-BOOT-1017: OpenType/TTF nicht direkt im Stage-1-Code (Teil 2)

---

# 24. Stage-2 übernimmt die Typografie

Ab Stage-2 darf NovaOS erstmals eine Fontbibliothek verwenden.

Diese Bibliothek darf unterstützen:

* TrueType
* OpenType
* Bitmapfonts
* zukünftige Fontformate

Stage-2 verfügt über erheblich mehr Speicher, CPU-Zeit und Diagnosemöglichkeiten als Stage-1.

---

# 25. Klare Verantwortlichkeiten

Die Bootarchitektur wird bewusst getrennt.

```text id="8drd3u"
Stage-1

↓

Bitmapfont (optional)

↓

Stage-2

↓

Font Engine

↓

Kernel

↓

Nova Typography
```

Dadurch bleibt jede Komponente klar abgegrenzt.

---

# 26. Font Engine

Die eigentliche Font Engine ist kein Bestandteil des Bootsektors.

Sie stellt später Funktionen bereit wie:

* Font laden
* Glyphen laden
* Rasterisieren
* Glyphen cachen
* Unicode unterstützen
* Skalieren
* Anti-Aliasing

---

# 27. Renderer

Die Font Engine zeichnet niemals direkt auf den Bildschirm.

Die Architektur lautet:

```text id="pwr4g6"
Font Engine

↓

Glyph Renderer

↓

Nova Graphics API

↓

Framebuffer
```

---

# 28. Renderer ist austauschbar

Die Fontbibliothek kennt den Framebuffer nicht.

Sie erzeugt lediglich Glyphen.

Dadurch kann später:

* Software Rendering
* GPU Rendering
* PDF Rendering
* Druckausgabe

denselben Fontcode verwenden.

---

# 29. Font Cache

Ein Font Cache gehört nicht in Stage-1.

Er wird erst in Stage-2 oder später eingeführt.

Aufgaben:

* häufig verwendete Glyphen speichern
* Rasterisierung vermeiden
* Performance verbessern

---

# 30. Unicode

Ab Stage-2 wird Unicode unterstützt.

Stage-1 bleibt unabhängig davon.

---

# 31. UTF-Unterstützung

Langfristig soll NovaOS unterstützen:

* UTF-8
* UTF-16
* UTF-32

Die Entscheidung über das interne Standardformat erfolgt separat.

---

# 32. Fontdateien

Fontdateien werden nicht Bestandteil des Stage-1-Binärimages.

Sie befinden sich später beispielsweise in:

```text id="gqj6m3"
/System/Fonts
```

oder einer vergleichbaren Systemstruktur.

---

# 33. Boot Fonts

Für den frühen Bootvorgang dürfen spezielle Bootfonts existieren.

Beispiele:

```text id="3j8l7v"
boot8x16.bin

boot16x16.bin
```

Diese sind keine TrueType-Dateien.

---

# 34. Fehlerbildschirm

Der frühe Fehlerbildschirm darf ausschließlich Bitmapfonts verwenden.

Der moderne grafische Bootfehlerbildschirm kann später auf der vollständigen Font Engine basieren.

---

# 35. Recovery

Auch das Recovery-System darf zunächst Bitmapfonts verwenden.

Spätere Versionen dürfen auf dieselbe Font Engine wie der Desktop zugreifen.

---

# 36. Konsistentes Schriftmodell

NovaOS soll langfristig dieselbe Fontbibliothek verwenden für:

* Bootmenü
* Installer
* Recovery
* Login
* Desktop
* Anwendungen

Dadurch entstehen keine unterschiedlichen Schriftimplementierungen.

---

# 37. Boot-Handoff

Stage-1 übergibt keinerlei Fontinformationen.

Das Boot-Handoff enthält insbesondere nicht:

* Fontdateien
* Glyphencaches
* Fonttabellen
* Unicodezustände

---

# 38. Speicherverbrauch

Stage-1 soll möglichst wenig Speicher belegen.

Ein Fontparser würde:

* zusätzlichen Code
* zusätzliche Tabellen
* zusätzliche Datenstrukturen
* zusätzliche Fehlerpfade

erfordern.

---

# 39. Wartbarkeit

Bitmapfonts bestehen aus statischen Daten.

TrueType-Parser bestehen aus mehreren tausend Zeilen Code.

Die Wartbarkeit unterscheidet sich entsprechend erheblich.

---

# 40. Testbarkeit

Bitmapfonts lassen sich vollständig testen.

Ein OpenType-Parser benötigt dagegen:

* hunderte Testfonts
* Grenzfalltests
* beschädigte Dateien
* Fuzzing
* Regressionstests

---

# 41. Sicherheitsmodell

Stage-1 verarbeitet keine komplexen externen Fontdateien.

Dadurch entfällt eine bedeutende Angriffsfläche.

---

# 42. Vertrauensmodell

Stage-1 vertraut ausschließlich:

* eingebetteten Bitmapdaten
* eigenem Maschinencode

Nicht jedoch externen Fontdateien.

---

# 43. Signaturen

Eine kryptographische Prüfung von Fontdateien gehört ausschließlich in spätere Bootstufen oder den Kernel.

---

# 44. Speicherverwaltung

TrueType benötigt dynamische Speicherverwaltung.

Stage-1 besitzt diese bewusst nicht.

---

# 45. Heap

Stage-1 verwendet keinen Heap.

Ein Fontparser darf daher dort nicht implementiert werden.

---

# 46. Dateisystem

Stage-1 besitzt keinen vollständigen Dateisystemzugriff.

Fontdateien könnten daher ohnehin nicht flexibel geladen werden.

---

# 47. Skalierung

Bitmapfonts besitzen feste Größen.

Skalierung erfolgt frühestens in Stage-2.

---

# 48. Anti-Aliasing

Anti-Aliasing gehört zur Font Engine.

Nicht zu Stage-1.

---

# 49. Subpixel Rendering

Subpixel Rendering ist Bestandteil späterer Grafikbibliotheken.

---

# 50. Vektorfonts

Alle Vektorfontformate gehören in spätere Komponenten.

Beispiele:

* TrueType
* OpenType
* CFF
* Variable Fonts

---

# 51. Variable Fonts

Variable Fonts erhöhen die Komplexität zusätzlich.

Sie werden ausschließlich durch die Font Engine verarbeitet.

---

# 52. Emoji

Emoji-Unterstützung gehört nicht in Stage-1.

---

# 53. Farbschriften

Color Fonts werden ausschließlich durch spätere Komponenten unterstützt.

---

# 54. Bootgeschwindigkeit

Durch den Verzicht auf Fontparser startet Stage-1 schneller.

---

# 55. Speicherlayout

Stage-1 bleibt klein genug, um ihre klar definierte Speicherorganisation beizubehalten.

---

# 56. Modularität

Die Font Engine bildet ein eigenes Modul.

Sie ist unabhängig von:

* Bootloader
* Kernel
* Renderer

---

# 57. API

Die Fontbibliothek stellt später beispielsweise Funktionen bereit wie:

```text id="fxyqk5"
load_font()

draw_text()

measure_text()

rasterize_glyph()
```

Stage-1 kennt keine dieser Funktionen.

---

# 58. Austauschbarkeit

Die Fontbibliothek kann später ersetzt oder erweitert werden.

Stage-1 bleibt davon unberührt.

---

# 59. Bootlogo

Das Bootlogo verwendet ausschließlich Grafiken.

Nicht Fontdateien.

---

# 60. Icons

Auch Icons werden unabhängig von der Fontbibliothek behandelt.

---

# 61. Architekturregel

Die Typografie gehört nicht in die erste Bootstufe.

Sie gehört in die Grafikbibliothek.

---

# 62. Qualitätsanforderungen

Stage-1 muss:

* klein
* deterministisch
* robust
* nachvollziehbar
* firmwareunabhängig

bleiben.

Ein Fontparser widerspricht diesen Zielen.

---

# 63. Alternativen

## Vollständiger TrueType-Parser

**Abgelehnt**

Gründe:

* große Codebasis
* hoher Speicherbedarf
* komplexe Fehlerfälle
* hohe Wartungskosten

---

## OpenType direkt in Stage-1

**Abgelehnt**

Gründe:

* überdimensioniert
* unnötig
* verletzt Minimalitätsprinzip

---

## Nur BIOS-Textmodus

**Teilweise geeignet**

Für frühe Fehlerausgaben ausreichend.

Für moderne Bootoberflächen jedoch nicht ausreichend.

---

## Bitmapfont

**Angenommen**

Bitmapfonts erfüllen sämtliche Anforderungen der frühen Bootphase.

---

# 64. Implementierungsstrategie

## Phase 1

* ASCII-Bitmapfont
* Fehlercodes
* einfache Textausgabe

## Phase 2

* Stage-2 Fontbibliothek

## Phase 3

* TrueType

## Phase 4

* OpenType

## Phase 5

* vollständige Typografie

---

# 65. Beispielstruktur

```text id="4qeq9m"
Stage-1

↓

Bitmap Font

↓

Stage-2

↓

TrueType Parser

↓

Glyph Cache

↓

Renderer

↓

Framebuffer
```

---

# 66. Dokumentation

Die Bitmapfonts müssen dokumentieren:

* Zeichenhöhe
* Zeichenbreite
* unterstützte Glyphen
* Kodierung
* Speicherlayout

---

# 67. Tests

Stage-1 muss mindestens testen:

* Bitmapdarstellung
* ASCII-Ausgabe
* Fehlertexte
* Glyphenpositionierung

Nicht jedoch TrueType-Dateien.

---

# 68. Regression

Änderungen an Bitmapfonts dürfen Stage-1 nicht vergrößern oder destabilisieren.

---

# 69. Build-System

Das Build-System erzeugt Bitmapfonts getrennt von TrueType-Dateien.

---

# 70. Fontpakete

Systemfonts werden später als normale Betriebssystemressourcen installiert.

Nicht als Bestandteil des Bootsektors.

---

# 71. Zukunftssicherheit

Neue Fontformate können ergänzt werden, ohne Stage-1 zu verändern.

---

# 72. Plattformunabhängigkeit

Die Fontbibliothek bleibt unabhängig von:

* BIOS
* UEFI
* x86
* ARM
* RISC-V

---

# 73. Konformitätskriterien

Eine Implementierung erfüllt dieses ADR, wenn:

* Stage-1 keinen TrueType-Parser enthält,
* Stage-1 keinen OpenType-Parser enthält,
* Stage-1 keine Unicode-Engine besitzt,
* Stage-1 ausschließlich Bitmapfonts oder BIOS-Text verwendet,
* moderne Fontunterstützung erst in Stage-2 beginnt,
* sämtliche Fontdateien außerhalb von Stage-1 gespeichert werden,
* und die vollständige Typografie durch eine eigene Fontbibliothek bereitgestellt wird.

---

# 74. Entscheidungsergebnis

NovaOS trennt die frühe Bootarchitektur konsequent von der späteren Typografie.

Stage-1 bleibt frei von komplexer Fontlogik.

Die Unterstützung moderner Schriftformate beginnt erst in einer späteren Bootstufe.

---

# 75. Zusammenfassung

Stage-1 ist ausschließlich für die minimale Initialisierung und das Laden der nächsten Bootstufe verantwortlich. Komplexe Dateiformate wie OpenType oder TrueType würden die erste Bootstufe unnötig vergrößern, die Fehleranfälligkeit erhöhen und dem Minimalitätsprinzip widersprechen.

Deshalb verwendet Stage-1 ausschließlich fest eingebettete Bitmapfonts oder den BIOS-Textmodus. Erst Stage-2 führt eine eigenständige Font Engine ein, die moderne Schriftformate, Unicode, Skalierung und spätere Erweiterungen unterstützt. Dadurch bleibt die Bootarchitektur schlank, wartbar und langfristig erweiterbar.
