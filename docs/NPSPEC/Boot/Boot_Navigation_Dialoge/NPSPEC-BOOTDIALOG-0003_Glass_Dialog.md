# NPSPEC-BOOTDIALOG-0003
# Glass Dialog

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTDIALOG-0003 |
| Titel | Glass Dialog |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Dialog System |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTDIALOG-0001, NPSPEC-BOOTDIALOG-0002, NPSPEC-BOOTCOMPOSITOR-0006, NPSPEC-BOOTCOMPOSITOR-0008 |
| Zugehörige ADRs | ADR-BOOTDIALOG-0003, ADR-BOOTCOMPOSITOR-0006, ADR-BOOTDESIGN-0003, ADR-BOOTRENDER-0016, ADR-BOOTACCESS-0002 |

---

# 1. Ziel

Diese Spezifikation definiert den **Glass Dialog** der NovaOS Boot UI.

Der Glass Dialog stellt das standardmäßige Dialogdesign der Bootoberfläche dar und verwendet ein transparentes, glasähnliches Material mit Hintergrundunschärfe.

Das System unterstützt:

- Glasmaterial
- Hintergrundunschärfe
- Transparenz
- Lichtreflexionen
- Schatten
- abgerundete Ecken
- automatische Fallbacks
- adaptives Erscheinungsbild

Der Glass Dialog bildet die primäre Designsprache der NovaOS Boot UI.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für alle Standarddialoge.

Sie umfasst:

- Informationsdialoge
- Bestätigungsdialoge
- Passwortdialoge
- Recovery-Dialoge
- Fortschrittsdialoge
- Systemeinstellungen

Nicht Bestandteil sind:

- Fehlermodus ohne Grafik
- Textmodus
- Classic-Fallback

---

# 3. Architektur

```text
Dialog Manager

↓

Glass Dialog

↓

Glass Material

↓

Backdrop Blur

↓

Compositor

↓

Framebuffer
```

---

# 4. Grundprinzip

Der Glass Dialog verwendet ein halbtransparentes Material.

Der Hintergrund bleibt sichtbar, wird jedoch weichgezeichnet und abgedunkelt.

Die Lesbarkeit des Dialoginhalts besitzt stets Vorrang vor visuellen Effekten.

---

# 5. Aufgaben

Der Glass Dialog übernimmt:

- Darstellung
- Materialdefinition
- Blur-Steuerung
- Transparenz
- Schatten
- Farbgebung
- Lichtakzente
- Fallbackumschaltung

---

# 6. Aufbau

Der Dialog besteht aus:

```text
Shadow

↓

Glass Surface

↓

Border Highlight

↓

Content

↓

Controls
```

Alle Ebenen werden durch den Compositor zusammengesetzt.

---

# 7. Glass Material

Eigenschaften:

- halbtransparent
- Hintergrundunschärfe
- leicht getönt
- weiche Lichtreflexion
- dezente Innenbeleuchtung

Das Material wird zentral über den Glass Material Renderer definiert.

---

# 8. Transparenz

Empfohlene Standardwerte:

| Eigenschaft | Wert |
|---|---:|
| Deckkraft | 82 % |
| Hintergrundtransparenz | 18 % |
| Border Deckkraft | 35 % |

Diese Werte können durch Themes angepasst werden.

---

# 9. Hintergrundunschärfe

Die Hintergrundunschärfe wird ausschließlich auf den Bereich hinter dem Dialog angewendet.

Blur darf niemals den Dialoginhalt beeinflussen.

Der Blur-Radius richtet sich nach dem Rendering Quality Profile.

---

# 10. Rahmen

Der Dialog besitzt:

- dünnen Glasrahmen
- obere Lichtkante
- leichte Innenkontur

Rahmenbreiten werden über Design Tokens definiert.

---

# 11. Schatten

Jeder Glass Dialog besitzt:

- weichen Außenschatten
- optionalen Kontaktschatten

Schatten dienen ausschließlich der Tiefenwirkung.

---

# 12. Abgerundete Ecken

Standard:

```text
Corner Radius

12 DLU
```

Die Rundung wird vollständig durch den Rounded Geometry Renderer erzeugt.

---

# 13. Farbgebung

Das Material verwendet:

- neutrale Grautöne
- leichte Farbtönung
- adaptive Akzentfarben

Die Farben stammen aus dem Boot Theme.

---

# 14. Layout

Dialoge werden:

- zentriert
- innerhalb der Safe Display Area dargestellt
- automatisch skaliert

Die maximale Größe ist begrenzt.

---

# 15. Motion

Standardanimation:

```text
Fade

+

Scale

+

Glass Fade
```

Die Materialeigenschaften werden synchron zur Dialoganimation eingeblendet.

---

# 16. Fallback

Ist Blur nicht verfügbar:

```text
Glass

↓

Transparente Fläche

↓

Kein Blur

↓

Normale Darstellung
```

Kann Transparenz ebenfalls nicht verwendet werden:

```text
Glass

↓

Opaque Surface
```

Der Dialog bleibt vollständig funktionsfähig.

---

# 17. Accessibility

Glass Dialoge unterstützen:

- High Contrast
- Reduced Motion
- größere Schriftgrößen
- höhere Kontraste

Bei aktiviertem High Contrast kann Transparenz vollständig deaktiviert werden.

---

# 18. Speicherverwaltung

Glass-Materialien verwenden:

- statische Materialobjekte
- feste Blur-Puffer
- feste Shaderparameter

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Materialwechsel
- deterministische Blur-Berechnung
- keine Heap-Allokationen

Sinkt das Performance Budget, wird zuerst die Blurqualität reduziert.

---

# 20. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- Blur nicht verfügbar
- Transparenz nicht verfügbar
- ungültiges Material
- beschädigte Theme-Daten
- Nullpointer

Im Fehlerfall wird automatisch auf den definierten Fallback gewechselt.

---

# 21. Sicherheit

Das System verhindert:

- unlesbaren Dialoginhalt
- fehlende Kontraste
- beschädigte Materialzustände
- rekursive Materialinitialisierung
- inkonsistente Transparenzwerte

---

# 22. API

```c
typedef struct
{
    float opacity;

    float blur_radius;

    float border_opacity;

    float shadow_strength;

} nova_glass_style_t;

nova_result_t
nova_glass_dialog_initialize(void);

nova_result_t
nova_glass_dialog_apply(
    nova_dialog_t* dialog);

bool
nova_glass_dialog_supported(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Blurstatus
- Transparenzstatus
- Materialtyp
- Fallbackstatus
- Renderzeit
- Speicherverbrauch
- Theme
- Fehleranzahl

---

# 24. Testfälle

Zu testen sind:

- Glass Material
- Blur
- Transparenz
- Schatten
- Corner Radius
- High Contrast
- Reduced Motion
- Fallback
- BIOS
- UEFI

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Glass Dialoge standardmäßig verwendet werden.
- Hintergrundunschärfe ausschließlich hinter dem Dialog angewendet wird.
- Transparenz und Schatten korrekt dargestellt werden.
- bei fehlender Unterstützung automatisch auf definierte Fallbacks gewechselt wird.
- Accessibility-Modi vollständig unterstützt werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme ein konsistentes Erscheinungsbild erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTDIALOG-0003 – Glass-Design für Dialoge
- ADR-BOOTCOMPOSITOR-0006 – Glass Material
- ADR-BOOTDESIGN-0003 – Nova Glass Design Language
- ADR-BOOTRENDER-0016 – Background Blur
- ADR-BOOTACCESS-0002 – Reduced Motion und High Contrast

## NPSPECs

- NPSPEC-BOOTDIALOG-0001 – Boot Dialog Architecture
- NPSPEC-BOOTDIALOG-0002 – Modal Dialog
- NPSPEC-BOOTCOMPOSITOR-0006 – Glass Material
- NPSPEC-BOOTCOMPOSITOR-0008 – Transparency Fallback
- NPSPEC-BOOTRENDER-0014 – Rounded Geometry
- NPSPEC-BOOTRENDER-0016 – Background Blur

---

# 27. Zusammenfassung

Der **Glass Dialog** definiert das standardmäßige Erscheinungsbild sämtlicher Dialoge der NovaOS Bootoberfläche. Durch halbtransparente Glasmaterialien, Hintergrundunschärfe, weiche Schatten und abgerundete Geometrien entsteht eine moderne und konsistente Benutzeroberfläche. Automatische Fallbackmechanismen gewährleisten eine identische Funktionalität auch auf Hardware ohne Blur- oder Transparenzunterstützung. Die enge Integration mit Compositor, Rendering, Motion und Accessibility sorgt für ein hochwertiges und gleichzeitig robustes Dialogsystem auf BIOS- und UEFI-Plattformen.