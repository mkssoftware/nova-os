# NPSPEC-BOOTLAYOUT-0007
# Responsive Boot Layout

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTLAYOUT-0007 |
| Titel | Responsive Boot Layout |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Layout |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTLAYOUT-0001 bis NPSPEC-BOOTLAYOUT-0006, NPSPEC-BOOTUI-0006 |
| Zugehörige ADRs | ADR-BOOTLAYOUT-0009, ADR-BOOTDESIGN-0002, ADR-BOOTDESIGN-0004, ADR-BOOTACCESS-0003, ADR-BOOTDISPLAY-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das **Responsive Boot Layout System** der NovaOS Boot UI.

Das System stellt sicher, dass sämtliche Bootoberflächen unabhängig von:

- Bildschirmauflösung
- Seitenverhältnis
- DPI
- BIOS
- UEFI
- Monitorgröße
- Virtualisierung

identisch funktionieren.

Es werden keine separaten Layouts für unterschiedliche Auflösungen entwickelt.

Alle Layouts entstehen aus denselben Layoutregeln.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche grafischen Bootoberflächen.

Sie umfasst:

- Hauptmenü
- Recovery
- Diagnose
- Einstellungen
- Dialoge
- Listen
- Benachrichtigungen
- Passwortdialoge

Nicht Bestandteil sind:

- Rendering
- Eingabesystem
- Animationen

---

# 3. Architektur

```text
Display Information

↓

Resolution Manager

↓

Scaling Engine

↓

Responsive Layout Engine

↓

Layout Tree

↓

Rendering
```

---

# 4. Grundprinzip

Es existiert genau **ein einziges Layoutmodell**.

Dieses Layout wird dynamisch an:

- verfügbare Fläche
- Skalierungsfaktor
- Seitenverhältnis

angepasst.

Auflösungsspezifische Layoutdateien sind unzulässig.

---

# 5. Aufgaben

Das Responsive Layout übernimmt:

- Größenanpassung
- Positionierung
- Skalierung
- Containeranpassung
- Breakpoints
- Mindestgrößen
- Dialogzentrierung
- Bildschirmanpassung

---

# 6. Unterstützte Auflösungen

Mindestens:

| Auflösung | Unterstützung |
|---|---|
| 640×480 | Ja |
| 800×600 | Ja |
| 1024×768 | Ja |
| 1280×720 | Ja |
| 1280×800 | Ja |
| 1366×768 | Ja |
| 1600×900 | Ja |
| 1920×1080 | Ja |
| 2560×1440 | Ja |
| 3840×2160 | Ja |

Alle höheren Auflösungen werden automatisch unterstützt.

---

# 7. Seitenverhältnisse

Unterstützt werden:

- 4:3
- 5:4
- 16:9
- 16:10
- 21:9
- Ultrawide

Die Layoutlogik bleibt identisch.

---

# 8. Skalierungsstufen

Unterstützt werden:

- 100 %
- 125 %
- 150 %
- 175 %
- 200 %
- automatisch

Die Skalierung erfolgt über das Resolution and Scaling System.

---

# 9. Breakpoints

Responsive Breakpoints definieren ausschließlich Layoutverhalten.

Sie erzeugen keine separaten Layoutdateien.

Empfohlene Klassen:

| Klasse | Breite |
|---|---:|
| Compact | < 800 px |
| Standard | 800–1279 px |
| Wide | ≥ 1280 px |

---

# 10. Dialoge

Dialoge werden:

- automatisch zentriert
- innerhalb der Bildschirmgrenzen gehalten
- bei Bedarf verkleinert

Sie dürfen niemals außerhalb des sichtbaren Bereichs liegen.

---

# 11. Listen

Listen passen automatisch an:

- Breite
- Zeilenhöhe
- Schriftgröße
- Scrollbereich

an.

---

# 12. Buttons

Buttons besitzen:

- Mindestbreite
- Mindesthöhe
- maximale Breite

Große Bildschirme erzeugen keine übergroßen Buttons.

---

# 13. Panels

Panels dürfen:

- wachsen
- schrumpfen
- nebeneinander
- untereinander

angeordnet werden.

Die Entscheidung erfolgt automatisch durch die Layout Engine.

---

# 14. Zentrierung

Folgende Bereiche bleiben grundsätzlich zentriert:

- Dialoge
- Recovery
- Passwortdialoge
- Ladeanzeigen

---

# 15. Freier Platz

Zusätzlicher Platz wird bevorzugt verwendet für:

- größere Abstände
- größere Inhaltsbereiche
- zusätzliche Randbereiche

Nicht für übergroße Controls.

---

# 16. Kleine Bildschirme

Bei geringer Auflösung:

- Abstände reduzieren
- Panels stapeln
- Dialoggrößen verkleinern

Die Bedienbarkeit muss vollständig erhalten bleiben.

---

# 17. Große Bildschirme

Bei großen Auflösungen:

- größere Innenabstände
- breitere Dialoge
- größere Inhaltsbereiche

Die Informationsdichte bleibt konstant.

---

# 18. Accessibility

Accessibility beeinflusst:

- Mindestgrößen
- Schriftgrößen
- Touchflächen
- Dialogbreiten

Responsive Regeln bleiben weiterhin aktiv.

---

# 19. Layout Invalidierung

Eine Neuberechnung erfolgt bei:

- Auflösungswechsel
- DPI-Wechsel
- Skalierungsänderung
- Displaywechsel
- Accessibility-Anpassung

---

# 20. Speicherverwaltung

Responsive Informationen werden Bestandteil des Layout Trees.

Separate Layoutkopien werden nicht erzeugt.

Heap-Allokationen während der Layoutberechnung sind verboten.

---

# 21. Performance

Anforderungen:

- deterministische Berechnung
- O(n)-Traversal
- keine rekursiven Layoutschleifen
- keine Heap-Allokationen

---

# 22. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Bildschirmgröße
- ungültiger Skalierungsfaktor
- beschädigte Responsive-Regeln
- fehlerhafte Breakpoints
- Layout außerhalb des Bildschirms

Im Fehlerfall wird auf das Standardlayout zurückgegriffen.

---

# 23. Sicherheit

Das System verhindert:

- abgeschnittene Dialoge
- unbedienbare Controls
- negative Layoutgrößen
- Nullpointer
- rekursive Responsive-Berechnungen

---

# 24. API

```c
typedef enum
{
    NOVA_LAYOUT_COMPACT,
    NOVA_LAYOUT_STANDARD,
    NOVA_LAYOUT_WIDE
}
nova_layout_class_t;

nova_result_t
nova_responsive_update(
    const nova_display_info_t* display);

nova_layout_class_t
nova_responsive_class(void);

nova_result_t
nova_responsive_apply(
    nova_layout_tree_t* tree);
```

---

# 25. Diagnosedaten

Zu erfassen sind:

- Bildschirmauflösung
- DPI
- Layoutklasse
- Skalierungsfaktor
- Breakpointwechsel
- Layoutinvalidierungen
- Berechnungszeit
- Fehleranzahl

---

# 26. Testfälle

Zu testen sind:

- 640×480
- 800×600
- 1024×768
- 1920×1080
- 4K
- Ultrawide
- DPI-Wechsel
- Accessibility
- Fenstergrößenänderung
- Recovery Mode

---

# 27. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Layouts ohne auflösungsspezifische Varianten funktionieren.
- alle unterstützten Seitenverhältnisse korrekt dargestellt werden.
- Dialoge jederzeit vollständig sichtbar bleiben.
- Breakpoints ausschließlich das Layoutverhalten beeinflussen.
- Accessibility-Regeln berücksichtigt werden.
- keine Heap-Allokationen während der Layoutberechnung stattfinden.
- BIOS- und UEFI-Systeme identische Layouts erzeugen.

---

# 28. Referenzen

## ADRs

- ADR-BOOTLAYOUT-0009 – Responsive Layout ohne separate Layoutdateien
- ADR-BOOTDESIGN-0002 – Gemeinsame Design-Tokens
- ADR-BOOTDESIGN-0004 – Adaptive Oberflächen
- ADR-BOOTACCESS-0003 – Skalierbare Schrift- und Bedienelementgrößen
- ADR-BOOTDISPLAY-0002 – Einheitliche Auflösungsverwaltung

## NPSPECs

- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTLAYOUT-0002 – Alignment and Spacing
- NPSPEC-BOOTLAYOUT-0006 – Constraint-Based Sizing
- NPSPEC-BOOTUI-0006 – Resolution and Scaling System
- NPSPEC-ACCESSIBILITY-0001 – Accessibility Framework

---

# 29. Zusammenfassung

Das **Responsive Boot Layout** sorgt dafür, dass sämtliche Benutzeroberflächen des NovaOS-Bootmanagers unabhängig von Auflösung, Seitenverhältnis oder DPI konsistent dargestellt werden. Durch ein einziges adaptives Layoutmodell, Constraint-basierte Größenberechnung und automatische Skalierung entstehen robuste, auflösungsunabhängige Oberflächen ohne separate Layoutvarianten. Die vollständige Integration in das Layout-, Scaling- und Accessibility-System gewährleistet identisches Verhalten auf BIOS- und UEFI-Plattformen.