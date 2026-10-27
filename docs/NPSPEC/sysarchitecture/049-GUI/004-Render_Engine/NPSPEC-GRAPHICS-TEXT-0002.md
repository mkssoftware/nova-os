# NPSPEC-GRAPHICS-TEXT-0002 – Nova Font Management

**Status:** Angenommen  
**Bereich:** Graphics / Text / Font Management  
**System:** NovaOS

## 1. Zweck

Nova Font Management definiert die zentrale Verwaltung von Schriftarten innerhalb von NovaOS und des portablen Nova Framework.

Ziel ist eine sichere, effiziente und einheitliche Bereitstellung von Schriftressourcen für Nova UI, NovaStudio, Systemprogramme und grafische Anwendungen.

Schriftverwaltung, Textgestaltung und Glyphenrasterisierung bleiben voneinander getrennt.

## 2. Architektur

Das Font Management besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Font Manager | Zentrale Verwaltung von Schriftressourcen |
| Font Loader | Laden und Initialisieren von Schriftdateien |
| Font Parser | Validierung und Interpretation von Schriftformaten |
| Font Registry | Registrierung verfügbarer Schriftarten |
| Font Resolver | Auswahl geeigneter Schriftarten |
| Font Fallback Manager | Ersatz fehlender Schriftzeichen |
| Font Cache | Wiederverwendung geladener Schriftressourcen |

Die Komponenten stellen ihre Funktionen über definierte Schnittstellen des Nova Framework bereit.

## 3. Schriftformate

Das System unterstützt mindestens:

- TrueType (TTF).
- OpenType (OTF).
- TrueType Collections (TTC).
- OpenType Collections (OTC).
- Variable Fonts.

Weitere Formate können durch austauschbare Font-Parser ergänzt werden.

Schriftdateien MÜSSEN vor ihrer Verwendung validiert werden.

Nicht unterstützte Formate werden kontrolliert zurückgewiesen.

## 4. Font-Modell

Eine Schriftressource besteht aus:

- Eindeutiger Font-ID.
- Schriftfamilie.
- Schriftschnitt.
- Schriftstärke.
- Schriftbreite.
- Schriftstil.
- Unterstützten Unicode-Bereichen.
- Font-Metriken.
- Optionalen Variationsachsen.
- Referenz auf die zugrunde liegenden Font-Daten.

Es wird zwischen `FontFace` und `FontInstance` unterschieden.

**FontFace** beschreibt die geladene Schrift einschließlich ihrer Eigenschaften und Glyphendaten.

**FontInstance** beschreibt eine konkrete Verwendung mit Schriftgröße, Variationswerten und weiteren Darstellungsparametern.

Mehrere FontInstances können dieselbe FontFace verwenden.

## 5. Font Registry

Die Font Registry verwaltet verfügbare Schriftarten.

Unterschieden werden:

- Systemschriften.
- Benutzerinstallierte Schriften.
- Anwendungsspezifische Schriften.
- Temporär geladene Schriften.

Schriftressourcen können registriert, abgefragt und kontrolliert entfernt werden.

Die Registry MUSS Schriftfamilien und verfügbare Schriftschnitte ermitteln können.

Anwendungsspezifische Schriften DÜRFEN ohne globale Systeminstallation verwendet werden.

## 6. Font Resolution und Fallback

Der Font Resolver wählt anhand der angeforderten Eigenschaften eine geeignete Schrift.

Berücksichtigt werden:

- Schriftfamilie.
- Schriftstärke.
- Schriftstil.
- Schriftbreite.
- Unicode-Abdeckung.
- Sprach- und Schriftanforderungen.

Kann eine Schrift die benötigten Zeichen nicht darstellen, wird eine geeignete Fallback-Schrift ausgewählt.

Fallback MUSS auf Ebene von Unicode-Graphemclustern beziehungsweise zusammengehörigen Shaping-Einheiten erfolgen können.

Zusammengehörige Zeichen DÜRFEN nicht unbeabsichtigt auf inkompatible Schriftarten verteilt werden.

Die Fallback-Reihenfolge MUSS deterministisch sein.

## 7. Font-Metriken

Das System stellt typografische Metriken bereit:

- Ascender.
- Descender.
- Line Gap.
- Units per Em.
- Glyph Advance.
- Glyph Bounding Box.
- Kerning-Informationen.
- Vertikale Metriken, sofern vorhanden.

Die Metriken werden der Nova Text Shaping Engine und der Nova Text Layout Engine bereitgestellt.

Die Berechnung der endgültigen Textpositionen erfolgt nicht im Font Manager.

## 8. Variable Fonts

Variable Fonts unterstützen mehrere Schriftschnitte innerhalb einer Schriftressource.

Das System ermöglicht:

- Abfrage verfügbarer Variationsachsen.
- Auswahl definierter Achsenwerte.
- Verwendung benannter Instanzen.
- Kombination unterstützter Variationsparameter.

Variationswerte MÜSSEN gegen die gültigen Wertebereiche geprüft werden.

Unterschiedliche FontInstances DÜRFEN dieselben zugrunde liegenden Font-Daten verwenden.

## 9. Ressourcenverwaltung und Caching

Font Management unterstützt:

- Gemeinsame Nutzung geladener FontFaces.
- Referenzbasierte Ressourcenverwaltung.
- Caching von Font-Metadaten.
- Bedarfsgerechtes Laden von Font-Daten.
- Freigabe ungenutzter Ressourcen.
- Wiederverwendung identischer FontInstances.

Schriftressourcen unterliegen den zentralen Ressourcenlimits.

Glyphenraster und Glyph Atlases werden durch die Nova Text Rendering Engine verwaltet.

Der Font Manager DARF keine unnötigen Kopien identischer Font-Daten erzeugen.

## 10. Sicherheit und Isolation

Schriftdateien gelten als nicht vertrauenswürdige Eingaben.

Festlegungen:

- Dateistrukturen und Tabellenreferenzen MÜSSEN validiert werden.
- Speichergrenzen und Größenberechnungen MÜSSEN überprüft werden.
- Fehlerhafte Fonts DÜRFEN keine unkontrollierten Speicherzugriffe verursachen.
- Anwendungsschriften DÜRFEN keine Systemberechtigungen erweitern.
- Schriftressourcen werden entsprechend Nova.Trust und den geltenden Zugriffsrechten bereitgestellt.

Der Font Parser SOLL in einer isolierten Ausführungsumgebung betrieben werden.

## 11. Integration

- **Nova Text Rendering Engine:** Verwendung geladener Schriftressourcen.
- **Nova Text Shaping:** Glyphenzuordnung und typografische Eigenschaften.
- **Nova Text Layout:** Verwendung von Font-Metriken.
- **Nova UI:** Einheitliche Schriftverwendung in Controls und Oberflächen.
- **Nova Framework:** Portable Font-Management-Schnittstellen.
- **Nova.Trust:** Berechtigungen und Ressourcenisolation.

Das Font Management besitzt keine direkte Abhängigkeit von GPU, Window Manager oder Compositor.

## 12. Normative Anforderungen

- **GRAPHICS-001:** NovaOS MUSS eine zentrale Schriftverwaltung bereitstellen.
- **GRAPHICS-002:** TrueType und OpenType MÜSSEN unterstützt werden.
- **GRAPHICS-003:** FontFaces und FontInstances MÜSSEN getrennt verwaltet werden.
- **GRAPHICS-004:** Schriftfamilien und Schriftschnitte MÜSSEN abfragbar sein.
- **GRAPHICS-005:** System-, Benutzer- und Anwendungsschriften MÜSSEN unterstützt werden.
- **GRAPHICS-006:** Font Fallback MUSS verfügbar sein.
- **GRAPHICS-007:** Die Fallback-Auswahl MUSS deterministisch erfolgen.
- **GRAPHICS-008:** Unicode-Abdeckung und Font-Metriken MÜSSEN abfragbar sein.
- **GRAPHICS-009:** Variable Fonts MÜSSEN unterstützt werden.
- **GRAPHICS-010:** Geladene FontFaces MÜSSEN gemeinsam verwendet werden können.
- **GRAPHICS-011:** Schriftdateien MÜSSEN vor ihrer Verwendung validiert werden.
- **GRAPHICS-012:** Schriftressourcen MÜSSEN kontrolliert freigegeben werden.
- **GRAPHICS-013:** Anwendungsschriften DÜRFEN keine zusätzlichen Systemberechtigungen erhalten.
- **GRAPHICS-014:** Das Font Management MUSS unabhängig vom Rendering-Backend funktionieren.
- **GRAPHICS-015:** Die Schnittstellen MÜSSEN Bestandteil des portablen Nova Framework sein.
- **GRAPHICS-016:** Die Schriftverwaltung MUSS ohne Netzwerk- oder KI-Abhängigkeit funktionieren.

## 13. Fehlerbehandlung

- Fehlerhafte oder unvollständige Schriftdateien werden zurückgewiesen.
- Nicht unterstützte Font-Formate werden eindeutig gemeldet.
- Ungültige Variationswerte werden zurückgewiesen.
- Fehlende Schriftfamilien werden durch geeignete Fallback-Schriften ersetzt.
- Fehlgeschlagene Ladevorgänge DÜRFEN bestehende Schriftressourcen nicht beschädigen.
- Nicht verfügbare Schriftressourcen werden den betroffenen Clients gemeldet.

## 14. Ergebnis

NovaOS erhält eine einheitliche, sichere und portable Verwaltung von Schriftarten.

Schriften können systemweit oder anwendungsspezifisch geladen, ausgewählt, gemeinsam genutzt und wieder freigegeben werden.

Die Architektur ermöglicht eine zuverlässige Schriftversorgung für mehrsprachige Benutzeroberflächen und grafische Anwendungen.

**Grundsatz:** Schriftressourcen werden zentral und sicher verwaltet. Ihre typografische Verarbeitung und grafische Darstellung erfolgen durch spezialisierte, unabhängige Komponenten.