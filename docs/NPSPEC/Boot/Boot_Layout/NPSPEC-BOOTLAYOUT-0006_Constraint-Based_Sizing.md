# NPSPEC-BOOTLAYOUT-0006
# Constraint-Based Sizing

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTLAYOUT-0006 |
| Titel | Constraint-Based Sizing |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Layout |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTLAYOUT-0001 bis NPSPEC-BOOTLAYOUT-0005, NPSPEC-BOOTUI-0006 |
| Zugehörige ADRs | ADR-BOOTLAYOUT-0007, ADR-BOOTLAYOUT-0008, ADR-BOOTDESIGN-0002, ADR-BOOTACCESS-0003, ADR-BOOTPERF-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das **Constraint-Based Sizing System** der NovaOS Boot UI.

Das System berechnet Größen aller UI-Elemente anhand deklarativer Größenregeln (Constraints) in DLU anstelle physischer Pixelwerte.

Es unterstützt:

- Mindestgrößen
- Maximalgrößen
- bevorzugte Größen
- automatische Größen
- Prozentgrößen
- relative Größen
- Seitenverhältnisse
- Inhaltsgrößen
- Stretch-Verhalten

Dadurch passt sich die Bootoberfläche automatisch an unterschiedliche Bildschirmauflösungen, DPI-Werte und Schriftgrößen an.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche UI-Elemente.

Sie umfasst:

- Buttons
- Dialoge
- Listen
- Panels
- Menüs
- Icons
- Statusleisten
- Eingabefelder
- Container

Nicht Bestandteil sind:

- Rendering
- Animationen
- Compositor

---

# 3. Architektur

```text
UI Element

↓

Constraint Engine

↓

Measure Pass

↓

Constraint Solver

↓

Arrange Pass

↓

Layout Tree

↓

Rendering
```

---

# 4. Grundprinzip

Jedes UI-Element beschreibt seine gewünschte Größe ausschließlich über Constraints.

Absolute Pixelgrößen sind innerhalb der Layoutlogik unzulässig. Feste logische Größen werden ausschließlich in DLU angegeben.

Die endgültige Größe wird erst während des Layoutdurchlaufs bestimmt.

---

# 5. Aufgaben

Das Constraint-System übernimmt:

- Größenberechnung
- Größenvalidierung
- Konfliktauflösung
- Seitenverhältnis
- Stretch
- Auto-Größe
- DPI-Anpassung
- Accessibility-Anpassung

---

# 6. Constraintmodell

Jedes Element besitzt folgende Größenparameter:

```c
typedef struct
{
    uint32_t min_width;
    uint32_t min_height;

    uint32_t preferred_width;
    uint32_t preferred_height;

    uint32_t max_width;
    uint32_t max_height;

} nova_size_constraints_t;
```

---

# 7. Größenarten

Unterstützt werden:

- Auto
- Fixed
- Percentage
- Stretch
- Content
- Remaining Space
- Aspect Ratio

---

# 8. Auto-Größe

Die Größe ergibt sich automatisch aus:

- Inhalt
- Padding
- Mindestgröße
- Maximalgröße

---

# 9. Feste Größe

Eine feste Größe darf verwendet werden für:

- Logos
- Icons
- Cursor
- definierte UI-Symbole

Alle übrigen Elemente sollen bevorzugt adaptive Größen verwenden.

---

# 10. Prozentgrößen

Größen können relativ zum Elterncontainer angegeben werden.

Beispiele:

```text
50 %

75 %

100 %
```

Die Berechnung erfolgt nach Abzug von Padding und Margin.

---

# 11. Stretch

Stretch verwendet den verbleibenden freien Platz innerhalb des Containers.

Mehrere Stretch-Elemente teilen den verfügbaren Bereich proportional auf.

---

# 12. Inhaltsgröße

Die Inhaltsgröße wird bestimmt durch:

- Text
- Icon
- Bild
- Kindobjekte

Anschließend werden Padding und Constraints angewendet.

---

# 13. Mindestgröße

Die Mindestgröße darf niemals unterschritten werden.

Auch bei kleinen Bildschirmauflösungen bleibt das Element vollständig bedienbar.

---

# 14. Maximalgröße

Die Maximalgröße verhindert unverhältnismäßig große Bedienelemente auf hochauflösenden Displays.

---

# 15. Bevorzugte Größe

Die bevorzugte Größe wird verwendet, wenn ausreichend Platz vorhanden ist.

Sie besitzt Vorrang vor Stretch.

---

# 16. Seitenverhältnis

Elemente können ein festes Seitenverhältnis definieren.

Beispiele:

```text
1:1

4:3

16:9
```

Das Seitenverhältnis wird während der Größenberechnung berücksichtigt.

---

# 17. Constraint-Prioritäten

Bei Konflikten gilt folgende Reihenfolge:

1. Mindestgröße
2. Maximalgröße
3. Seitenverhältnis
4. bevorzugte Größe
5. Stretch
6. Prozentgröße

---

# 18. Konfliktauflösung

Kann nicht jeder Constraint erfüllt werden, wird automatisch der Constraint mit der niedrigeren Priorität angepasst.

Dabei bleibt das Layout jederzeit gültig.

---

# 19. Accessibility

Bei aktivierter Accessibility dürfen automatisch erhöht werden:

- Mindestgrößen
- Touchflächen
- Buttonhöhen
- Dialoggrößen

Die relativen Proportionen bleiben erhalten.

---

# 20. Skalierung

Alle Größen werden abschließend durch das Resolution and Scaling System skaliert.

Dadurch entstehen identische Proportionen auf BIOS- und UEFI-Systemen unabhängig von der nativen Auflösung.

---

# 21. Layout Invalidierung

Eine Neuberechnung erfolgt bei:

- Änderung eines Constraints
- Änderung der Containergröße
- DPI-Wechsel
- Schriftgrößenänderung
- Accessibility-Anpassung

---

# 22. Speicherverwaltung

Constraintdaten sind Bestandteil des Layoutobjekts.

Separate Constraintobjekte werden nicht dynamisch erzeugt.

Heap-Allokationen während der Layoutberechnung sind verboten.

---

# 23. Performance

Anforderungen:

- deterministische Berechnung
- O(n)-Constraint-Auswertung
- keine rekursiven Constraint-Schleifen
- keine Heap-Allokationen

---

# 24. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- Mindestgröße größer als Maximalgröße
- ungültige Prozentwerte
- negatives Seitenverhältnis
- beschädigte Constraintdaten
- widersprüchliche Größenregeln

Im Fehlerfall werden Standard-Constraints verwendet.

---

# 25. Sicherheit

Das System verhindert:

- unendliche Constraint-Schleifen
- zyklische Größenabhängigkeiten
- Nullpointer
- Speicherüberläufe
- ungültige Layoutgrößen

---

# 26. API

```c
typedef enum
{
    NOVA_SIZE_AUTO,
    NOVA_SIZE_FIXED,
    NOVA_SIZE_PERCENT,
    NOVA_SIZE_STRETCH,
    NOVA_SIZE_CONTENT
}
nova_size_mode_t;

nova_result_t
nova_constraints_measure(
    nova_layout_node_t* node);

nova_result_t
nova_constraints_resolve(
    nova_layout_node_t* node);

nova_result_t
nova_constraints_validate(
    nova_layout_node_t* node);
```

---

# 27. Diagnosedaten

Zu erfassen sind:

- Constraintverletzungen
- Mindestgrößen
- Maximalgrößen
- Stretch-Anteile
- Prozentwerte
- Seitenverhältnisse
- Layoutinvalidierungen
- Fehleranzahl

---

# 28. Testfälle

Zu testen sind:

- Auto-Größe
- Fixed-Größe
- Prozentgrößen
- Stretch
- Seitenverhältnis
- Mindestgröße
- Maximalgröße
- Accessibility
- DPI-Wechsel
- Recovery Mode

---

# 29. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Größen ausschließlich über Constraints berechnet werden.
- Mindest- und Maximalgrößen korrekt eingehalten werden.
- Prozent- und Stretchgrößen zuverlässig funktionieren.
- Seitenverhältnisse korrekt berücksichtigt werden.
- Accessibility-Anpassungen automatisch erfolgen.
- keine Heap-Allokationen während der Layoutberechnung stattfinden.
- BIOS- und UEFI-Systeme identische Layoutgrößen erzeugen.

---

# 30. Referenzen

## ADRs

- ADR-BOOTLAYOUT-0007 – Constraint-basiertes Layoutsystem
- ADR-BOOTLAYOUT-0008 – Priorisierte Constraint-Auswertung
- ADR-BOOTDESIGN-0002 – Gemeinsame Design-Tokens
- ADR-BOOTACCESS-0003 – Skalierbare Schrift- und Bedienelementgrößen
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath

## NPSPECs

- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTLAYOUT-0002 – Alignment and Spacing
- NPSPEC-BOOTLAYOUT-0003 – Stack Layout
- NPSPEC-BOOTLAYOUT-0004 – Grid Layout
- NPSPEC-BOOTLAYOUT-0005 – Overlay Layout
- NPSPEC-BOOTUI-0006 – Resolution and Scaling System
- NPSPEC-ACCESSIBILITY-0001 – Accessibility Framework

---

# 31. Zusammenfassung

Das **Constraint-Based Sizing System** definiert die adaptive Größenberechnung sämtlicher UI-Elemente der NovaOS Bootoberfläche. Alle festen logischen Größen und Constraints werden in DLU angegeben; physische Pixelwerte entstehen erst an der Rendering-Grenze. Mindest- und Maximalgrößen, Prozentwerte, Stretch-Regeln und Seitenverhältnisse ermöglichen eine deterministische, auflösungsunabhängige Darstellung auf BIOS- und UEFI-Systemen.
