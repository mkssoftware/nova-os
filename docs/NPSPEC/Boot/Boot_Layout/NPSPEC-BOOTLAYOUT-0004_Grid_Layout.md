# NPSPEC-BOOTLAYOUT-0004
# Grid Layout

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTLAYOUT-0004 |
| Titel | Grid Layout |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Layout |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTLAYOUT-0001 bis NPSPEC-BOOTLAYOUT-0003 |
| Zugehörige ADRs | ADR-BOOTLAYOUT-0003, ADR-BOOTLAYOUT-0005, ADR-BOOTCONTROL-0005, ADR-BOOTDESIGN-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das **Grid Layout** der NovaOS Boot UI.

Das Grid Layout ermöglicht die strukturierte Anordnung von UI-Elementen innerhalb eines tabellenähnlichen Rasters.

Es unterstützt:

- Zeilen
- Spalten
- Auto-Größen
- feste Größen
- prozentuale Größen
- Stretch
- Zellenzusammenführung
- Alignment innerhalb einzelner Zellen

Das Grid Layout wird für komplexe Dialoge, Formulare und Informationsübersichten verwendet.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Grid-Container.

Sie umfasst:

- Einstellungsdialoge
- Recovery-Dialoge
- Informationsseiten
- Diagnoseansichten
- Formulare
- Tabellen
- Statusanzeigen

Nicht Bestandteil sind:

- Stack Layout
- Overlay Layout
- Absolute Layout

---

# 3. Architektur

```text
Grid Container

↓

Measure Pass

↓

Zeilen berechnen

↓

Spalten berechnen

↓

Zellen anordnen

↓

Layout Tree

↓

Rendering
```

---

# 4. Grundprinzip

Ein Grid besteht aus:

- Zeilen
- Spalten
- Zellen

Jedes Kindobjekt wird genau einer Zelle zugeordnet.

Optional kann ein Objekt mehrere Zeilen oder Spalten belegen.

---

# 5. Aufgaben

Das Grid Layout übernimmt:

- Zeilenberechnung
- Spaltenberechnung
- Zellgrößen
- Positionierung
- Alignment
- Spacing
- Zellzusammenführung

---

# 6. Zeilen

Unterstützte Zeilentypen:

- Auto
- Fixed
- Star
- Percentage

Definition:

| Typ | Beschreibung |
|---|---|
| Auto | Größe nach Inhalt |
| Fixed | feste Größe |
| Star | proportionaler Rest |
| Percentage | prozentual |

---

# 7. Spalten

Unterstützte Spaltentypen:

- Auto
- Fixed
- Star
- Percentage

Die Berechnung erfolgt analog zu den Zeilen.

---

# 8. Zellposition

Jedes Element definiert:

- Zeile
- Spalte

Optional:

- RowSpan
- ColumnSpan

---

# 9. Zellzusammenführung

Ein Objekt darf mehrere Zellen belegen.

Beispiel:

```text
+-----+-----+
|     Header |
+-----+-----+
| A   |  B  |
+-----+-----+
```

Die Zusammenführung darf benachbarte Bereiche nicht überschneiden.

---

# 10. Größenberechnung

Die Berechnung erfolgt in drei Schritten:

1. Auto-Größen bestimmen
2. Feste Größen reservieren
3. Restfläche auf Star- und Prozentbereiche verteilen

---

# 11. Alignment

Innerhalb jeder Zelle werden unterstützt:

Horizontal:

- Left
- Center
- Right
- Stretch

Vertikal:

- Top
- Center
- Bottom
- Stretch

---

# 12. Padding

Jede Zelle kann eigenes Padding besitzen.

Padding beeinflusst ausschließlich den Innenbereich der Zelle.

---

# 13. Margin

Jedes Element besitzt zusätzlich Margin.

Margin wirkt innerhalb der zugewiesenen Zelle.

---

# 14. Spacing

Zwischen Zeilen und Spalten können feste Abstände definiert werden.

Standardwerte stammen aus den Design Tokens.

---

# 15. Reihenfolge

Die Zeichenreihenfolge ergibt sich aus:

1. Zeile
2. Spalte
3. Z-Index

Die logische Reihenfolge bleibt unabhängig davon erhalten.

---

# 16. Sichtbarkeit

Unsichtbare Elemente:

- belegen keine Fläche,
- beeinflussen jedoch weiterhin die Grid-Struktur, sofern dies konfiguriert ist.

Optional kann ein ausgeblendetes Element vollständig aus der Layoutberechnung ausgeschlossen werden.

---

# 17. Größenänderungen

Eine Neuberechnung erfolgt bei:

- Änderung der Containergröße
- Änderung von Zeilen
- Änderung von Spalten
- Änderung der Inhalte
- Änderung der Skalierung

---

# 18. Accessibility

Bei aktivierter Accessibility dürfen:

- Zeilenhöhen
- Spaltenabstände
- Padding
- Touchflächen

automatisch vergrößert werden.

---

# 19. Skalierung

Alle berechneten Werte werden anschließend durch das Resolution and Scaling System skaliert.

Die Proportionen des Grids bleiben erhalten.

---

# 20. Speicherverwaltung

Jeder Grid-Container besitzt:

- Zeilenliste
- Spaltenliste
- Zellzuordnung
- Layoutinformationen

Heap-Allokationen während des Layoutdurchlaufs sind verboten.

---

# 21. Performance

Anforderungen:

- deterministische Berechnung
- O(r + c + n)
- keine rekursiven Layoutschleifen
- keine Heap-Allokationen

Dabei stehen:

- r = Anzahl Zeilen
- c = Anzahl Spalten
- n = Anzahl Kindobjekte

---

# 22. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Zeilenindexe
- ungültige Spaltenindexe
- ungültige Spans
- beschädigte Grid-Struktur
- überlappende Zellen

Im Fehlerfall wird das betroffene Element nicht dargestellt.

---

# 23. Sicherheit

Das System verhindert:

- doppelte Zellbelegung
- zyklische Grid-Strukturen
- Nullpointer
- Speicherüberläufe
- ungültige Zellkoordinaten

---

# 24. API

```c
typedef enum
{
    NOVA_GRID_AUTO,
    NOVA_GRID_FIXED,
    NOVA_GRID_STAR,
    NOVA_GRID_PERCENT
}
nova_grid_unit_t;

nova_result_t
nova_grid_measure(
    nova_grid_t* grid);

nova_result_t
nova_grid_arrange(
    nova_grid_t* grid);

nova_result_t
nova_grid_update(
    nova_grid_t* grid);
```

---

# 25. Diagnosedaten

Zu erfassen sind:

- Anzahl Zeilen
- Anzahl Spalten
- Anzahl Zellen
- RowSpan
- ColumnSpan
- Layoutzeit
- Invalidierungen
- Fehleranzahl

---

# 26. Testfälle

Zu testen sind:

- Auto-Zeilen
- Fixed-Zeilen
- Star-Größen
- Prozentgrößen
- RowSpan
- ColumnSpan
- Alignment
- Padding
- Accessibility
- Auflösungswechsel

---

# 27. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Auto-, Fixed-, Star- und Prozentgrößen vollständig unterstützt werden.
- RowSpan und ColumnSpan korrekt funktionieren.
- Alignment innerhalb jeder Zelle korrekt berechnet wird.
- Grid-Strukturen deterministisch berechnet werden.
- Accessibility-Anpassungen berücksichtigt werden.
- keine Heap-Allokationen während des Layoutdurchlaufs stattfinden.
- BIOS- und UEFI-Systeme identische Grid-Layouts erzeugen.

---

# 28. Referenzen

## ADRs

- ADR-BOOTLAYOUT-0003 – Containerbasiertes Layoutsystem
- ADR-BOOTLAYOUT-0005 – Grid-basierte Dialoglayouts
- ADR-BOOTCONTROL-0005 – Wiederverwendbare Komponenten statt fest gezeichneter Oberflächen
- ADR-BOOTDESIGN-0002 – Gemeinsame Design-Tokens

## NPSPECs

- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTLAYOUT-0002 – Alignment and Spacing
- NPSPEC-BOOTLAYOUT-0003 – Stack Layout
- NPSPEC-BOOTUI-0006 – Resolution and Scaling System

---

# 29. Zusammenfassung

Das **Grid Layout** stellt ein leistungsfähiges Rasterlayout für komplexe Benutzeroberflächen der NovaOS Boot UI bereit. Durch die Unterstützung von Auto-, Fixed-, Star- und Prozentgrößen sowie Zellzusammenführungen, Alignment und skalierbaren Abständen eignet es sich für Dialoge, Formulare und Diagnoseansichten. Die deterministische Berechnung, feste Speicherverwaltung und vollständige Integration in das Layout- und Skalierungssystem gewährleisten konsistente Ergebnisse auf BIOS- und UEFI-Systemen.