# NPSPEC-BOOTLAYOUT-0003
# Stack Layout

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTLAYOUT-0003 |
| Titel | Stack Layout |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Layout |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTLAYOUT-0002 |
| Zugehörige ADRs | ADR-BOOTLAYOUT-0003, ADR-BOOTLAYOUT-0004, ADR-BOOTCONTROL-0005, ADR-BOOTDESIGN-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das **Stack Layout** der NovaOS Boot UI.

Das Stack Layout ist der am häufigsten verwendete Containertyp der Bootoberfläche und dient zur automatischen Anordnung mehrerer UI-Elemente entlang einer gemeinsamen Achse.

Es unterstützt:

- vertikale Stapel
- horizontale Stapel
- automatische Größenanpassung
- gleichmäßige Verteilung
- feste Abstände
- Alignment
- Stretch
- Verschachtelung

Das Stack Layout bildet die Grundlage für Menüs, Dialoge, Listen und Einstellungsseiten.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Stack-Container.

Sie umfasst:

- Bootmenüs
- Recovery-Menüs
- Listen
- Dialoginhalte
- Buttongruppen
- Formulare
- Seitenbereiche
- Informationsfelder

Nicht Bestandteil sind:

- Grid Layout
- Overlay Layout
- Absolute Layout

---

# 3. Architektur

```text
Stack Container

↓

Measure Pass

↓

Arrange Pass

↓

Alignment

↓

Spacing

↓

Layout Tree

↓

Rendering
```

---

# 4. Grundprinzip

Alle Kindobjekte werden entlang einer gemeinsamen Achse angeordnet.

Die Reihenfolge entspricht exakt der Reihenfolge innerhalb des UI Trees.

Ein Stack verändert niemals die Reihenfolge seiner Elemente.

---

# 5. Aufgaben

Das Stack Layout übernimmt:

- Positionierung
- Größenberechnung
- Spacing
- Alignment
- Stretch
- automatische Verteilung
- Containergrößenberechnung

---

# 6. Orientierung

Unterstützt werden:

- Vertical
- Horizontal

```text
Vertical

Item 1
Item 2
Item 3
```

```text
Horizontal

Item1 Item2 Item3
```

---

# 7. Größenmodell

Jedes Element besitzt:

- gewünschte Größe
- Minimalgröße
- Maximalgröße
- Stretch-Verhalten

Die endgültige Größe wird während des Arrange Pass berechnet.

---

# 8. Reihenfolge

Die Reihenfolge ist stabil.

Änderungen erfolgen ausschließlich durch:

- Einfügen
- Entfernen
- Sortierung

Animationen verändern niemals die Reihenfolge.

---

# 9. Spacing

Zwischen benachbarten Elementen wird ein definierter Abstand eingefügt.

Standard:

| Token | Abstand |
|---|---:|
| XS | 4 DLU |
| S | 8 DLU |
| M | 12 DLU |
| L | 16 DLU |
| XL | 24 DLU |

Die Werte stammen aus den Design Tokens.

---

# 10. Padding

Der Container besitzt:

- Top
- Right
- Bottom
- Left

Padding reduziert den verfügbaren Innenbereich.

---

# 11. Alignment

Unterstützt werden:

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

Die nicht verwendete Achse wird über das Alignment gesteuert.

---

# 12. Stretch

Elemente dürfen:

- horizontal
- vertikal

gestreckt werden.

Die Minimal- und Maximalgrößen bleiben dabei erhalten.

---

# 13. Gleichmäßige Verteilung

Optional können Elemente gleichmäßig verteilt werden.

Unterstützt werden:

- Equal
- Space Between
- Space Around
- Space Evenly

---

# 14. Sichtbarkeit

Unsichtbare Elemente:

- belegen keinen Platz,
- beeinflussen keine Abstände.

Sie bleiben jedoch Bestandteil des UI Trees.

---

# 15. Verschachtelung

Stack Layouts dürfen beliebig ineinander verschachtelt werden.

Beispiel:

```text
Vertical Stack

↓

Horizontal Stack

↓

Vertical Stack
```

Die Verschachtelungstiefe wird ausschließlich durch das Layoutsystem begrenzt.

---

# 16. Größenanpassung

Ändert sich:

- Containergröße
- Inhalt
- Schriftgröße
- Skalierungsfaktor

wird der Stack automatisch neu berechnet.

---

# 17. Layout Invalidierung

Ein neuer Layoutdurchlauf erfolgt bei:

- Hinzufügen eines Elements
- Entfernen eines Elements
- Größenänderung
- Änderung von Padding
- Änderung von Spacing
- Änderung des Alignments

---

# 18. Accessibility

Bei aktivierter Accessibility dürfen automatisch vergrößert werden:

- Padding
- Spacing
- Klickflächen

Die Reihenfolge der Elemente bleibt unverändert.

---

# 19. Skalierung

Alle berechneten Abstände werden anschließend durch das Resolution and Scaling System skaliert.

Das Verhältnis der Elemente bleibt erhalten.

---

# 20. Speicherverwaltung

Jeder Stack Container besitzt:

- feste Kindliste
- Layoutinformationen
- Alignmentinformationen

Heap-Allokationen während des Layoutdurchlaufs sind verboten.

---

# 21. Performance

Anforderungen:

- O(n)-Traversal
- deterministische Berechnung
- keine rekursiven Endlosschleifen
- keine Heap-Allokationen

---

# 22. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Orientierung
- beschädigte Kindliste
- ungültiges Alignment
- negative Größen
- fehlerhafte Paddingwerte

Im Fehlerfall wird der betroffene Container übersprungen.

---

# 23. Sicherheit

Das System verhindert:

- rekursive Layoutschleifen
- Nullpointer
- Speicherüberläufe
- doppelte Kindobjekte
- zyklische Containerstrukturen

---

# 24. API

```c
typedef enum
{
    NOVA_STACK_VERTICAL,
    NOVA_STACK_HORIZONTAL
}
nova_stack_orientation_t;

nova_result_t
nova_stack_measure(
    nova_stack_t* stack);

nova_result_t
nova_stack_arrange(
    nova_stack_t* stack);

nova_result_t
nova_stack_update(
    nova_stack_t* stack);
```

---

# 25. Diagnosedaten

Zu erfassen sind:

- Orientierung
- Anzahl Kindobjekte
- Layoutzeit
- Arrangezeit
- Padding
- Spacing
- Invalidierungen
- Fehleranzahl

---

# 26. Testfälle

Zu testen sind:

- Vertical Stack
- Horizontal Stack
- Stretch
- Alignment
- Padding
- Spacing
- Verschachtelung
- Accessibility
- Auflösungswechsel
- Recovery Mode

---

# 27. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- vertikale und horizontale Stack Layouts vollständig unterstützt werden.
- Reihenfolge und Hierarchie unverändert bleiben.
- Alignment und Spacing korrekt berechnet werden.
- verschachtelte Stack Layouts funktionieren.
- Accessibility-Anpassungen berücksichtigt werden.
- keine Heap-Allokationen während des Layoutdurchlaufs stattfinden.
- BIOS- und UEFI-Systeme identische Layouts erzeugen.

---

# 28. Referenzen

## ADRs

- ADR-BOOTLAYOUT-0003 – Containerbasiertes Layoutsystem
- ADR-BOOTLAYOUT-0004 – Einheitliches Alignment-Modell
- ADR-BOOTCONTROL-0005 – Wiederverwendbare Komponenten statt fest gezeichneter Oberflächen
- ADR-BOOTDESIGN-0002 – Gemeinsame Design-Tokens

## NPSPECs

- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTLAYOUT-0002 – Alignment and Spacing
- NPSPEC-BOOTUI-0006 – Resolution and Scaling System
- NPSPEC-BOOTCONTROL-0001 – Base Control Model

---

# 29. Zusammenfassung

Das **Stack Layout** bildet den zentralen Layoutcontainer der NovaOS Bootoberfläche. Es ordnet UI-Elemente automatisch entlang einer horizontalen oder vertikalen Achse an und übernimmt Größenberechnung, Alignment, Spacing sowie Stretch-Verhalten. Durch deterministische Layoutberechnung, Unterstützung beliebiger Verschachtelungen und vollständige Integration in das Skalierungs- und Accessibility-System entsteht eine robuste und konsistente Grundlage für Menüs, Dialoge und Listen auf BIOS- und UEFI-Systemen.
