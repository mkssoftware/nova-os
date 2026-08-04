# NPSPEC-BOOTLAYOUT-0008
# Safe Display Areas

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTLAYOUT-0008 |
| Titel | Safe Display Areas |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Layout |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTLAYOUT-0001 bis NPSPEC-BOOTLAYOUT-0007, NPSPEC-BOOTUI-0006 |
| Zugehörige ADRs | ADR-BOOTDISPLAY-0003, ADR-BOOTLAYOUT-0010, ADR-BOOTDESIGN-0002, ADR-BOOTACCESS-0003, ADR-BOOTSAFE-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die **Safe Display Areas** der NovaOS Boot UI.

Das System stellt sicher, dass sämtliche wichtigen Benutzeroberflächenelemente unabhängig von Hardware, Firmware oder Bildschirmtyp vollständig sichtbar bleiben.

Es berücksichtigt insbesondere:

- Overscan
- BIOS-Grafikmodi
- UEFI GOP
- ungewöhnliche Seitenverhältnisse
- zukünftige Displaytypen
- virtuelle Maschinen
- Remote-Konsolen

Die Safe Display Area bildet den garantierten Bereich, in dem alle interaktiven Elemente dargestellt werden.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche grafischen Bootoberflächen.

Sie umfasst:

- Hauptmenü
- Recovery
- Diagnose
- Einstellungen
- Dialoge
- Statusleisten
- Benachrichtigungen
- Passwortdialoge

Nicht Bestandteil sind:

- Rendering
- Compositor
- Animationen

---

# 3. Architektur

```text
Display Detection

↓

Resolution Manager

↓

Safe Area Calculator

↓

Layout Engine

↓

Layout Tree

↓

Rendering
```

---

# 4. Grundprinzip

Die Layout Engine arbeitet niemals direkt mit der vollständigen Bildschirmfläche.

Zunächst wird eine Safe Display Area berechnet.

Alle interaktiven UI-Elemente müssen vollständig innerhalb dieser Fläche liegen.

Dekorative Hintergründe dürfen außerhalb der Safe Area dargestellt werden.

Dieses Vorgehen entspricht etablierten TV- und Safe-Area-Konzepten zur Vermeidung abgeschnittener Inhalte durch Overscan. :contentReference[oaicite:0]{index=0}

---

# 5. Aufgaben

Das System übernimmt:

- Overscan-Kompensation
- sichere Platzierung
- Bildschirmränder
- Layoutbegrenzung
- Zentrierung
- Skalierungsanpassung
- Displayvalidierung

---

# 6. Safe Area

Die Safe Area beschreibt:

```text
+--------------------------------------+
|############## Rand ##################|
|######################################|
|## +------------------------------+ ##|
|## |                              | ##|
|## |      Safe Display Area       | ##|
|## |                              | ##|
|## +------------------------------+ ##|
|######################################|
+--------------------------------------+
```

Der äußere Bereich darf ausschließlich für dekorative Inhalte verwendet werden.

---

# 7. Standardränder

Standardmäßig verwendet NovaOS:

| Bildschirmkante | Abstand |
|---|---:|
| Links | 5 % |
| Rechts | 5 % |
| Oben | 5 % |
| Unten | 5 % |

Diese Werte orientieren sich an bewährten Overscan-Sicherheitszonen für TV- und Firmware-Oberflächen. :contentReference[oaicite:1]{index=1}

---

# 8. Kritische Elemente

Folgende Elemente müssen vollständig innerhalb der Safe Area liegen:

- Buttons
- Text
- Icons
- Eingabefelder
- Listen
- Dialoge
- Fokusindikatoren
- Warnmeldungen

---

# 9. Dekorative Elemente

Außerhalb der Safe Area zulässig sind:

- Hintergrundbilder
- Farbverläufe
- Schatten
- Blurflächen
- Glass-Effekte
- Animationen ohne Interaktion

---

# 10. Zentrierung

Dialoge werden innerhalb der Safe Area zentriert.

Die Bildschirmmitte wird aus der Safe Area berechnet und nicht aus der nativen Bildschirmauflösung.

---

# 11. Overscan

Bei aktivem Overscan:

- werden Ränder automatisch berücksichtigt.
- werden keine Bedienelemente abgeschnitten.
- bleiben sämtliche Texte vollständig sichtbar.

Die Overscan-Kompensation erfolgt automatisch.

---

# 12. BIOS

Im BIOS-Modus können Grafikkarten ungenaue Bildränder liefern.

Die Safe Area kompensiert diese Unterschiede automatisch.

---

# 13. UEFI GOP

Bei GOP-Ausgabe entspricht die Safe Area normalerweise der vollständigen sichtbaren Fläche.

Optional kann dennoch eine Sicherheitsreserve aktiviert werden.

---

# 14. Virtuelle Maschinen

Virtuelle Displays besitzen häufig keine Overscan-Bereiche.

Die Safe Area bleibt dennoch aktiv, um identische Layouts auf allen Plattformen sicherzustellen.

---

# 15. Bildschirmwechsel

Bei Änderungen von:

- Auflösung
- Grafikmodus
- Skalierung

wird die Safe Area vollständig neu berechnet.

---

# 16. Responsive Layout

Alle Responsive-Regeln beziehen sich ausschließlich auf die Safe Area.

Die physische Bildschirmgröße spielt für die Layoutberechnung keine direkte Rolle.

---

# 17. Accessibility

Accessibility kann:

- Mindestabstände vergrößern
- Safe Area erweitern
- Dialoggrößen anpassen

Dadurch entstehen größere und leichter erreichbare Bedienelemente.

---

# 18. Skalierung

Die Safe Area wird vor der DPI-Skalierung berechnet.

Anschließend werden sämtliche Koordinaten gemeinsam skaliert.

---

# 19. Layout Invalidierung

Eine Neuberechnung erfolgt bei:

- Auflösungswechsel
- DPI-Wechsel
- Overscan-Änderung
- Displaywechsel
- Accessibility-Anpassung

---

# 20. Speicherverwaltung

Die Safe Area wird als Bestandteil der Displayinformationen gespeichert.

Es existiert stets genau eine aktive Safe Area.

Heap-Allokationen sind nicht zulässig.

---

# 21. Performance

Anforderungen:

- O(1)-Berechnung
- deterministische Ergebnisse
- keine rekursiven Berechnungen
- keine Heap-Allokationen

---

# 22. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Displaygröße
- negative Safe Area
- Safe Area außerhalb des Displays
- beschädigte Displaydaten
- ungültiger Skalierungsfaktor

Im Fehlerfall wird automatisch die vollständige Bildschirmfläche verwendet.

---

# 23. Sicherheit

Das System verhindert:

- abgeschnittene Buttons
- abgeschnittene Dialoge
- unlesbaren Text
- negative Koordinaten
- Layouts außerhalb des sichtbaren Bereichs

---

# 24. API

```c
typedef struct
{
    int32_t x;

    int32_t y;

    uint32_t width;

    uint32_t height;

} nova_safe_area_t;

nova_result_t
nova_safe_area_initialize(void);

const nova_safe_area_t*
nova_safe_area_get(void);

nova_result_t
nova_safe_area_update(
    const nova_display_info_t* display);
```

---

# 25. Diagnosedaten

Zu erfassen sind:

- Displayauflösung
- Safe-Area-Größe
- Overscan-Ränder
- DPI
- Layoutklasse
- Invalidierungen
- Berechnungszeit
- Fehleranzahl

---

# 26. Testfälle

Zu testen sind:

- BIOS 640×480
- BIOS 800×600
- UEFI 1920×1080
- UEFI 4K
- Overscan aktiv
- Overscan deaktiviert
- DPI-Wechsel
- Accessibility
- Virtuelle Maschine
- Recovery Mode

---

# 27. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche interaktiven Elemente vollständig innerhalb der Safe Display Area liegen.
- dekorative Elemente außerhalb der Safe Area dargestellt werden dürfen.
- Overscan automatisch kompensiert wird.
- Responsive Layouts ausschließlich auf Basis der Safe Area berechnet werden.
- Accessibility-Anpassungen korrekt berücksichtigt werden.
- keine Heap-Allokationen während der Berechnung stattfinden.
- BIOS- und UEFI-Systeme identische Layoutergebnisse erzeugen.

---

# 28. Referenzen

## ADRs

- ADR-BOOTDISPLAY-0003 – Sichere Darstellungsbereiche
- ADR-BOOTLAYOUT-0010 – Safe-Area-basierte Layoutberechnung
- ADR-BOOTDESIGN-0002 – Gemeinsame Design-Tokens
- ADR-BOOTACCESS-0003 – Skalierbare Schrift- und Bedienelementgrößen
- ADR-BOOTSAFE-0001 – Funktionaler Textmodus als letzter Fallback

## NPSPECs

- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTLAYOUT-0007 – Responsive Boot Layout
- NPSPEC-BOOTUI-0006 – Resolution and Scaling System
- NPSPEC-ACCESSIBILITY-0001 – Accessibility Framework

---

# 29. Zusammenfassung

Die **Safe Display Areas** definieren den garantiert sichtbaren Darstellungsbereich der NovaOS Bootoberfläche. Sämtliche interaktiven Elemente werden ausschließlich innerhalb dieser Fläche positioniert, während dekorative Inhalte bis an den Bildschirmrand reichen dürfen. Durch automatische Overscan-Kompensation, deterministische Berechnung und vollständige Integration in das Responsive- und Scaling-System entstehen robuste, hardwareunabhängige Layouts, die auf BIOS-, UEFI- und virtuellen Systemen konsistent dargestellt werden. Das Konzept orientiert sich an etablierten Safe-Zone- und Overscan-Richtlinien moderner TV- und UI-Plattformen. :contentReference[oaicite:2]{index=2}