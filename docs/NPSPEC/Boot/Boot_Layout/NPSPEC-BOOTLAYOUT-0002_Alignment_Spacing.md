# NPSPEC-BOOTLAYOUT-0002
# Alignment and Spacing

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTLAYOUT-0002 |
| Titel | Alignment and Spacing |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Layout |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTUI-0006 |
| Zugehörige ADRs | ADR-BOOTLAYOUT-0002, ADR-BOOTLAYOUT-0004, ADR-BOOTDESIGN-0002, ADR-BOOTDESIGN-0003, ADR-BOOTACCESS-0003 |

---

# 1. Ziel

Diese Spezifikation definiert das vollständige **Alignment- und Spacing-System** der NovaOS Boot UI.

Sie legt verbindlich fest:

- horizontale Ausrichtung
- vertikale Ausrichtung
- Innenabstände (Padding)
- Außenabstände (Margin)
- Abstände zwischen Elementen (Spacing)
- automatische Zentrierung
- gleichmäßige Verteilung
- Baseline-Ausrichtung

Dadurch entsteht auf sämtlichen Bootoberflächen ein konsistentes Layout mit identischen Abständen und einer einheitlichen visuellen Struktur.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Layoutobjekte.

Sie umfasst:

- Buttons
- Listen
- Panels
- Dialoge
- Menüs
- Statusleisten
- Textfelder
- Container
- Recovery UI

Nicht Bestandteil sind:

- Rendering
- Animationen
- Eingabesystem

---

# 3. Architektur

```text
UI Tree

↓

Layout Engine

↓

Alignment Engine

↓

Spacing Engine

↓

Layout Tree

↓

Rendering
```

---

# 4. Grundprinzip

Kein UI-Element besitzt fest kodierte Positionen.

Alle Positionen ergeben sich ausschließlich aus:

- Alignment
- Margin
- Padding
- Containerregeln
- verfügbaren Platz

---

# 5. Aufgaben

Das Alignment-System übernimmt:

- horizontale Ausrichtung
- vertikale Ausrichtung
- Zentrierung
- Stretch
- Verteilung
- Abstandsberechnung
- Baseline-Ausrichtung

---

# 6. Horizontales Alignment

Unterstützt werden:

- Left
- Center
- Right
- Stretch

Definition:

| Modus | Beschreibung |
|---|---|
| Left | linksbündig |
| Center | zentriert |
| Right | rechtsbündig |
| Stretch | gesamte Breite |

---

# 7. Vertikales Alignment

Unterstützt werden:

- Top
- Center
- Bottom
- Stretch

Definition:

| Modus | Beschreibung |
|---|---|
| Top | oben |
| Center | mittig |
| Bottom | unten |
| Stretch | gesamte Höhe |

---

# 8. Padding

Padding definiert den Innenabstand eines Containers.

Unterstützt werden:

- Top
- Right
- Bottom
- Left

Padding verkleinert den verfügbaren Inhaltsbereich.

---

# 9. Margin

Margin definiert den Außenabstand eines UI-Elements.

Margins verändern niemals die Größe des Elements.

Sie beeinflussen ausschließlich dessen Position.

---

# 10. Spacing

Spacing beschreibt den Abstand zwischen benachbarten Elementen innerhalb eines Containers.

Es existieren keine impliziten Standardabstände.

Jeder Container definiert sein eigenes Spacing.

---

# 11. Standard-Abstände

Empfohlene Design Tokens:

| Token | Wert |
|---|---:|
| XS | 4 DLU |
| S | 8 DLU |
| M | 12 DLU |
| L | 16 DLU |
| XL | 24 DLU |
| XXL | 32 DLU |

Direkte Pixelwerte im UI-Code sind unzulässig. Sämtliche Abstände werden in DLU angegeben.

---

# 12. Zentrierung

Unterstützt werden:

- horizontal
- vertikal
- vollständig

Die Zentrierung erfolgt relativ zum verfügbaren Containerbereich.

---

# 13. Stretch

Stretch vergrößert ein Objekt bis zur maximal verfügbaren Fläche.

Minimal- und Maximalgrößen bleiben dabei erhalten.

---

# 14. Baseline Alignment

Textelemente können entlang einer gemeinsamen Grundlinie ausgerichtet werden.

Dies verbessert die Lesbarkeit innerhalb komplexer Formulare.

---

# 15. Gleichmäßige Verteilung

Container dürfen Elemente gleichmäßig verteilen.

Unterstützt werden:

- Equal Space
- Space Around
- Space Between
- Space Evenly

---

# 16. Automatische Größenanpassung

Alignment verändert niemals:

- Minimalgröße
- Maximalgröße
- intrinsische Größe

Die Größenberechnung erfolgt ausschließlich im Measure Pass.

---

# 17. Containerregeln

Jeder Containertyp definiert:

- Standard Alignment
- Standard Spacing
- Padding
- Stretch-Verhalten

Kindobjekte dürfen diese Werte überschreiben.

---

# 18. Skalierung

Alle Abstände werden nach der Berechnung durch das Resolution and Scaling System skaliert.

Das Verhältnis der Abstände bleibt unabhängig von der Auflösung identisch.

---

# 19. Accessibility

Bei aktivierter Barrierefreiheit dürfen:

- Padding
- Margin
- Spacing

automatisch vergrößert werden.

Dadurch entstehen größere Klick- und Touchflächen.

---

# 20. Layout Invalidierung

Eine Neuberechnung erfolgt bei Änderungen von:

- Alignment
- Padding
- Margin
- Spacing
- Skalierungsfaktor
- Containergröße

---

# 21. Speicherverwaltung

Alignmentdaten werden Bestandteil des Layoutobjekts.

Separate Alignmentobjekte existieren nicht.

Heap-Allokationen während des Layoutdurchlaufs sind verboten.

---

# 22. Performance

Anforderungen:

- O(n)-Traversal
- deterministische Berechnung
- keine rekursiven Layoutschleifen
- keine Heap-Allokationen

---

# 23. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiges Alignment
- negative Paddingwerte
- ungültige Marginwerte
- beschädigte Layoutdaten
- ungültige Containerregeln

Im Fehlerfall werden Standardwerte verwendet.

---

# 24. Sicherheit

Das System verhindert:

- überlappende Standardlayouts
- ungültige Koordinaten
- Speicherüberläufe
- Nullpointer
- rekursive Alignmentberechnungen

---

# 25. API

```c
typedef enum
{
    NOVA_ALIGN_LEFT,
    NOVA_ALIGN_CENTER,
    NOVA_ALIGN_RIGHT,
    NOVA_ALIGN_STRETCH
}
nova_horizontal_alignment_t;

typedef enum
{
    NOVA_ALIGN_TOP,
    NOVA_ALIGN_CENTER_VERTICAL,
    NOVA_ALIGN_BOTTOM,
    NOVA_ALIGN_STRETCH_VERTICAL
}
nova_vertical_alignment_t;

nova_result_t
nova_layout_apply_alignment(
    nova_layout_node_t* node);

nova_result_t
nova_layout_apply_spacing(
    nova_layout_node_t* node);
```

---

# 26. Diagnosedaten

Zu erfassen sind:

- Alignmentänderungen
- Paddingwerte
- Marginwerte
- Spacingwerte
- Layoutinvalidierungen
- Skalierungsfaktor
- Containergrößen
- Fehleranzahl

---

# 27. Testfälle

Zu testen sind:

- Left Alignment
- Center Alignment
- Right Alignment
- Stretch
- Padding
- Margin
- Spacing
- Baseline Alignment
- Accessibility Scaling
- Auflösungswechsel

---

# 28. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Positionierungen ausschließlich über Alignment erfolgen.
- Padding und Margin korrekt getrennt verarbeitet werden.
- Design Tokens für alle Standardabstände verwendet werden.
- automatische Skalierung korrekt funktioniert.
- Accessibility-Anpassungen berücksichtigt werden.
- keine Heap-Allokationen während des Layoutdurchlaufs stattfinden.
- BIOS- und UEFI-Systeme identische Layoutergebnisse erzeugen.

---

# 29. Referenzen

## ADRs

- ADR-BOOTLAYOUT-0002 – Drei-Phasen-Layoutberechnung
- ADR-BOOTLAYOUT-0004 – Einheitliches Alignment-Modell
- ADR-BOOTDESIGN-0002 – Gemeinsame Design-Tokens
- ADR-BOOTDESIGN-0003 – Adaptives Glass-Design
- ADR-BOOTACCESS-0003 – Skalierbare Schrift- und Bedienelementgrößen

## NPSPECs

- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTUI-0006 – Resolution and Scaling System
- NPSPEC-BOOTDESIGN-0002 – Design Tokens
- NPSPEC-ACCESSIBILITY-0001 – Accessibility Framework

---

# 30. Zusammenfassung

Das **Alignment and Spacing System** definiert sämtliche Regeln zur Ausrichtung und Abstandsberechnung innerhalb der NovaOS Bootoberfläche. Durch einheitliche Alignment-Modelle, Design Tokens für Abstände sowie eine klare Trennung von Padding, Margin und Spacing entstehen konsistente und skalierbare Layouts. Die vollständige Integration in die Layout Engine und das Resolution-and-Scaling-System gewährleistet identische Ergebnisse auf BIOS- und UEFI-Systemen sowie eine optimale Anpassung an unterschiedliche Bildschirmgrößen und Accessibility-Anforderungen.
