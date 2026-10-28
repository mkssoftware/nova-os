# NPSPEC-INPUT-DATA-0003 – Nova Data Transfer Formats

**Status:** Angenommen  
**Bereich:** Input / Data / Transfer Formats  
**System:** NovaOS

## 1. Zweck

Nova Data Transfer Formats definiert ein einheitliches Formatmodell für den Datenaustausch zwischen Anwendungen, Capabilities und Systemkomponenten.

Ziel ist die Übertragung von Daten unter Erhaltung ihrer Struktur, ihres Datentyps und ihrer semantischen Bedeutung.

Das Formatmodell bildet die gemeinsame Grundlage für Clipboard, Drag and Drop und weitere systeminterne Datenübertragungen.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Data Format Registry | Verwaltung registrierter Datenformate |
| Data Format Descriptor | Beschreibung eines Datenformats |
| Format Negotiator | Aushandlung kompatibler Formate |
| Format Converter | Kontrollierte Formatkonvertierung |
| Data Transfer Validator | Prüfung von Daten und Metadaten |

Die Architektur ist unabhängig vom Übertragungsweg, der Benutzeroberfläche und dem verwendeten Speicherbackend.

## 3. Formatmodell

Jedes Datenformat besitzt einen eindeutigen Formatdeskriptor.

Dieser enthält mindestens:

- Formatkennung.
- Formatversion.
- Datentyp.
- Strukturdefinition beziehungsweise Formatvertrag.
- Kodierung, sofern erforderlich.
- Optional semantischen Typ.
- Optional unterstützte Konvertierungen.

Formatkennungen MÜSSEN eindeutig und versionsfähig sein.

Registrierte Formate DÜRFEN keine ausführbaren Funktionen voraussetzen.

## 4. Datenkategorien

NovaOS unterstützt mindestens folgende Kategorien:

| Kategorie | Beschreibung |
|---|---|
| Text | Unicode-Text |
| Rich Text | Formatierter Text |
| Image | Raster- und Vektorgrafiken |
| Audio | Audiodaten |
| Video | Videodaten |
| File Reference | Datei- und Verzeichnisreferenzen |
| Structured Data | Strukturierte Objekte und Datensätze |
| Binary Data | Beliebige binäre Daten |
| Semantic Data | Daten mit definierter fachlicher Bedeutung |
| Custom Data | Anwendungsspezifische Formate |

Weitere Kategorien DÜRFEN registriert werden.

Unbekannte Formate MÜSSEN als opake Daten übertragen werden können, sofern die Übertragung zulässig ist.

## 5. Semantische Datentypen

NovaOS unterscheidet zwischen Datenformat und semantischer Bedeutung.

Beispiele:

- Eine Zahl kann eine Länge, Temperatur oder Geschwindigkeit darstellen.
- Eine Tabelle kann Messwerte, Finanzdaten oder Koordinaten enthalten.
- Eine Audiodatei kann Musik, Sprache oder Messsignale repräsentieren.
- Eine Punktliste kann geografische Positionen oder CAD-Geometrie beschreiben.

Semantische Typen DÜRFEN zusätzliche Informationen enthalten:

- Einheit.
- Koordinatensystem.
- Wertebereich.
- Genauigkeit.
- Fachliche Struktur.
- Herkunft.

Die semantische Bedeutung MUSS bei kompatiblen Übertragungen erhalten bleiben können.

Eine Konvertierung DARF inkompatible semantische Typen nicht stillschweigend gleichsetzen.

## 6. Mehrfachrepräsentationen

Ein logisches Datenobjekt kann mehrere Formate bereitstellen.

Beispiel einer Tabelle:

- Native strukturierte Tabellendaten.
- CSV.
- HTML.
- Unicode-Klartext.

Alle Repräsentationen gehören zum selben logischen Datenobjekt.

Der Empfänger kann das für ihn geeignetste Format auswählen.

Die ursprüngliche strukturierte Repräsentation SOLL bevorzugt werden, wenn Quelle und Ziel sie unterstützen.

## 7. Formatverhandlung

Die Formatverhandlung erfolgt anhand der angebotenen und unterstützten Formate.

Dabei gilt:

1. Quelle veröffentlicht verfügbare Formate.
2. Ziel ermittelt kompatible Formate.
3. Direkte Formatübereinstimmungen werden bevorzugt.
4. Falls erforderlich, werden zulässige Konvertierungen geprüft.
5. Das Ziel wählt eine geeignete Repräsentation.
6. Die Datenübertragung wird angefordert.

Die Verhandlung MUSS deterministisch erfolgen.

Automatische Konvertierungen DÜRFEN nur über registrierte und autorisierte Konverter erfolgen.

## 8. Formatkonvertierung

NovaOS unterstützt kontrollierte Konvertierungen zwischen kompatiblen Formaten.

Dabei gilt:

- Verlustfreie Konvertierungen SOLLEN bevorzugt werden.
- Verlustbehaftete Konvertierungen MÜSSEN erkennbar sein.
- Semantische Informationen SOLLEN erhalten bleiben.
- Einheitenumrechnungen MÜSSEN mathematisch korrekt erfolgen.
- Nicht unterstützte Konvertierungen werden abgelehnt.
- Originaldaten bleiben unverändert.

Konvertierungen können durch das Nova Framework oder registrierte Capabilities bereitgestellt werden.

Eine Formatkonvertierung DARF keine zusätzlichen Berechtigungen erzeugen.

## 9. Datenreferenzen und Zero-Copy

Nova Data Transfer Formats definiert die Formatbeschreibung unabhängig von der tatsächlichen Datenhaltung.

Unterstützte Bereitstellungsformen:

- Direkte Daten.
- Speicherreferenzen.
- Datei- und Objekt-Referenzen.
- Streams.
- Verzögert erzeugte Daten.

Zero-Copy-Übertragungen SOLLEN verwendet werden, wenn sie technisch möglich und sicher sind.

Referenzen MÜSSEN hinsichtlich Lebensdauer, Zugriff und Gültigkeit überprüfbar sein.

## 10. Sicherheit und Validierung

Das Formatmodell unterliegt Nova.Trust.

Festlegungen:

- Datenformate MÜSSEN vor ihrer Verarbeitung validiert werden.
- Unbekannte Formate DÜRFEN nicht automatisch interpretiert oder ausgeführt werden.
- Formatdeskriptoren DÜRFEN keine Berechtigungen verleihen.
- Datenreferenzen DÜRFEN keine Sicherheitsgrenzen umgehen.
- Konverter MÜSSEN innerhalb ihrer autorisierten Capabilities arbeiten.
- Größenangaben und Strukturinformationen MÜSSEN auf Plausibilität geprüft werden.
- Fehlerhafte Daten DÜRFEN andere Prozesse nicht beeinträchtigen.

## 11. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine zentrale Registrierung von Datenformaten bereitstellen.
- **INPUT-002:** Datenformate MÜSSEN eindeutig identifizierbar und versionsfähig sein.
- **INPUT-003:** Strukturierte, binäre und textbasierte Daten MÜSSEN unterstützt werden.
- **INPUT-004:** Anwendungsspezifische Formate MÜSSEN registrierbar sein.
- **INPUT-005:** Semantische Datentypen MÜSSEN unterstützt werden können.
- **INPUT-006:** Ein Datenobjekt MUSS mehrere Repräsentationen anbieten können.
- **INPUT-007:** Quelle und Ziel MÜSSEN kompatible Formate aushandeln können.
- **INPUT-008:** Formatverhandlungen MÜSSEN deterministisch erfolgen.
- **INPUT-009:** Automatische Konvertierungen MÜSSEN kontrolliert und autorisiert sein.
- **INPUT-010:** Verlustbehaftete Konvertierungen MÜSSEN erkennbar sein.
- **INPUT-011:** Datenformate MÜSSEN unabhängig vom Übertragungsweg definiert sein.
- **INPUT-012:** Referenzbasierte und streambasierte Datenübertragungen MÜSSEN unterstützt werden können.
- **INPUT-013:** Unbekannte Formate DÜRFEN nicht automatisch ausgeführt werden.
- **INPUT-014:** Datenübertragungen MÜSSEN die Sicherheitsrichtlinien von Nova.Trust einhalten.
- **INPUT-015:** Nova Data Transfer Formats MUSS unabhängig von Nova UI funktionieren.
- **INPUT-016:** Das Formatmodell MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 12. Integration

- **Nova Clipboard Management:** Formatbasierter Datenaustausch über die Zwischenablage.
- **Nova Drag and Drop:** Formatverhandlung bei Ziehen und Ablegen.
- **Nova Framework:** Datentypen, Konvertierungen und Validierung.
- **Nova Capability System:** Austausch semantischer Daten.
- **Nova IPC:** Prozessübergreifende Datenübertragung.
- **Nova Memory Management:** Speicherreferenzen und Zero-Copy.
- **Nova VFS:** Datei- und Objekt-Referenzen.
- **NovaFile:** Strukturierte Datencontainer.
- **Nova UI:** Darstellung und Verarbeitung übertragener Daten.
- **Nova.Trust:** Berechtigungen und Isolation.

## 13. Ergebnis

NovaOS erhält ein einheitliches, erweiterbares und semantisch orientiertes Datenformatmodell.

Anwendungen und Capabilities können Daten austauschen, ohne ihre Struktur oder Bedeutung unnötig zu verlieren. Kompatible Formate werden direkt übertragen, während erforderliche Konvertierungen kontrolliert erfolgen.

**Grundsatz:** NovaOS überträgt Daten möglichst in ihrer ursprünglichen Struktur und Bedeutung. Eine vereinfachte Darstellung ist eine mögliche Repräsentation, nicht der eigentliche Datentyp.