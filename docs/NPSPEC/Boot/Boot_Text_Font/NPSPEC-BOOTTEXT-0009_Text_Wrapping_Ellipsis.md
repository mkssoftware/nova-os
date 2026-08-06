# NPSPEC-BOOTTEXT-0009
# Text Wrapping and Ellipsis

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTTEXT-0009 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Text System |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTTEXT-0007, NPSPEC-BOOTTEXT-0008, NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTRENDER-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Text Wrapping and Ellipsis System** der NovaOS-Bootoberfläche.

Das System legt fest, wie Texte automatisch umgebrochen, abgeschnitten oder mit einer Auslassungsmarkierung versehen werden, wenn der verfügbare Darstellungsbereich nicht ausreicht. Ziel ist eine konsistente und vorhersehbare Darstellung unabhängig von Sprache, Schriftart oder Bildschirmauflösung.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Labels
- Buttons
- Listen
- Menüs
- Dialoge
- Tooltips
- Statusanzeigen
- Recovery
- Installer
- Diagnosemodule

---

# 3. Ziele

Das System muss:

- Unicode vollständig unterstützen
- Wortumbruch beherrschen
- Ellipsis unterstützen
- deterministisch arbeiten
- DPI-unabhängig sein
- Layout und Rendering trennen

---

# 4. Architektur

```text
UTF-8 Text

↓

Text Measurement

↓

Wrapping Engine

↓

Line Builder

↓

Ellipsis Engine

↓

Text Layout

↓

Renderer
```

---

# 5. Komponenten

Das System besteht aus:

- Wrapping Engine
- Word Break Engine
- Line Builder
- Ellipsis Engine
- Constraint Resolver
- Layout Generator

---

# 6. Grundprinzip

Der Ablauf erfolgt wie folgt:

```text
Text

↓

Messung

↓

Zeilenbildung

↓

Prüfung auf Grenzen

↓

Ellipsis

↓

Layout
```

Die Entscheidung erfolgt vollständig vor dem Rendering.

---

# 7. Wrapping-Modi

Unterstützt werden:

- No Wrap
- Word Wrap
- Character Wrap
- Clip

Der Modus wird pro Control festgelegt.

---

# 8. Wortumbruch

Standardmäßig erfolgt der Umbruch an Wortgrenzen.

Beispiel:

```text
NovaOS entwickelt

↓

NovaOS

entwickelt
```

Wörter werden nicht getrennt, solange genügend Platz vorhanden ist.

---

# 9. Zeichenumbruch

Optional kann der Umbruch zwischen einzelnen Zeichen erfolgen.

Dies ist insbesondere für:

- asiatische Schriftsysteme
- sehr lange Zeichenketten
- Dateinamen

geeignet.

---

# 10. Zeilenbegrenzung

Für jedes Layout können definiert werden:

- maximale Breite
- maximale Höhe
- maximale Zeilenanzahl

Wird eine Grenze überschritten, greift die Ellipsis-Engine.

---

# 11. Ellipsis

Unterstützt werden:

- End Ellipsis
- Middle Ellipsis
- Start Ellipsis

Standard:

```text
End Ellipsis

↓

NovaOS Boot Man…
```

Die Auslassung erfolgt ausschließlich an gültigen Zeichenpositionen.

---

# 12. Ellipsis-Zeichen

Standardmäßig wird verwendet:

```text
U+2026

…
```

Drei einzelne Punkte (`...`) dürfen nur verwendet werden, wenn das Zeichen nicht verfügbar ist.

---

# 13. Mehrzeilige Ellipsis

Optional unterstützt:

```text
NovaOS Boot

Manager

…

```

Die letzte sichtbare Zeile erhält die Ellipsis-Markierung.

---

# 14. Trunkierung

Alternativ zur Ellipsis kann ein Text abgeschnitten werden.

Modi:

- Clip
- Ellipsis
- Scroll (optional)

Das Verhalten wird pro Control konfiguriert.

---

# 15. Unicode-Unterstützung

Die Wrapping Engine berücksichtigt:

- Unicode-Wortgrenzen
- Combining Characters
- Surrogate-Paare
- Grapheme Cluster

Ein Grapheme Cluster darf niemals getrennt werden.

---

# 16. Themeintegration

Das Theme kann definieren:

- Standard-Wrapping-Modus
- Ellipsis-Stil
- Zeilenhöhe

Das Theme beeinflusst nicht die eigentliche Berechnungslogik.

---

# 17. Accessibility

Unterstützt werden:

- vollständiger Text für Screen Reader
- vergrößerte Schriftgrößen
- dynamischer Neuumbruch

Auch abgeschnittene Texte bleiben vollständig für Accessibility verfügbar.

---

# 18. Sicherheit

Das System verhindert:

- Trennung innerhalb eines Grapheme Clusters
- ungültige Unicode-Sequenzen
- negative Layoutgrößen
- rekursive Umbruchberechnungen
- Buffer Overflows

Alle Eingaben werden validiert.

---

# 19. Speicherverwaltung

Das System verwendet:

- statische Zeilenpuffer
- Layout-Pools
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 20. Performance

Anforderungen:

- O(n)-Umbruch
- O(n)-Ellipsis-Berechnung
- deterministische Laufzeit

Die Laufzeit wächst ausschließlich linear mit der Anzahl der Zeichen.

---

# 21. API

```c
typedef enum
{
    NOVA_WRAP_NONE,
    NOVA_WRAP_WORD,
    NOVA_WRAP_CHARACTER,
    NOVA_WRAP_CLIP
}
nova_wrap_mode_t;

typedef enum
{
    NOVA_ELLIPSIS_NONE,
    NOVA_ELLIPSIS_END,
    NOVA_ELLIPSIS_MIDDLE,
    NOVA_ELLIPSIS_START
}
nova_ellipsis_mode_t;

nova_result_t
nova_text_wrap(
    const char* utf8,
    const nova_text_constraints_t* constraints,
    nova_text_layout_t* layout);

nova_result_t
nova_text_apply_ellipsis(
    nova_text_layout_t* layout,
    nova_ellipsis_mode_t mode);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Zeilenanzahl
- Wrapping-Modus
- Ellipsis-Modus
- Trunkierungen
- Layoutgröße
- Berechnungszeit
- Fehler

---

# 23. Testfälle

Zu testen sind:

- Word Wrap
- Character Wrap
- No Wrap
- Clip
- End Ellipsis
- Middle Ellipsis
- Start Ellipsis
- Mehrzeilige Texte
- Unicode
- Grapheme Cluster
- Accessibility
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- alle Wrapping-Modi korrekt funktionieren.
- Ellipsis deterministisch angewendet wird.
- Unicode-Wortgrenzen berücksichtigt werden.
- Grapheme Cluster niemals getrennt werden.
- mehrzeilige Texte korrekt verarbeitet werden.
- Accessibility jederzeit den vollständigen Text bereitstellt.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 25. Referenzen

## ADRs

- ADR-BOOTTEXT-0009 – Text Wrapping and Ellipsis
- ADR-BOOTTEXT-0008 – Text Layout
- ADR-BOOTLAYOUT-0001 – Layout Engine
- ADR-BOOTACCESS-0001 – Accessibility

## NPSPECs

- NPSPEC-BOOTTEXT-0007 – Text Measurement
- NPSPEC-BOOTTEXT-0008 – Text Layout
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTRENDER-0001 – Boot Rendering Engine

---

# 26. Zusammenfassung

Das **Text Wrapping and Ellipsis System** definiert die standardisierte Verarbeitung von Zeilenumbrüchen und Textkürzungen innerhalb der NovaOS-Bootoberfläche. Es unterstützt Wort- und Zeichenumbruch, verschiedene Ellipsis-Modi sowie Unicode-konforme Trennregeln und erzeugt deterministische Layouts unabhängig von Auflösung oder Sprache. Durch vollständige Trennung von Layout und Rendering, statische Speicherverwaltung und Integration in das Accessibility-System gewährleistet es eine konsistente und benutzerfreundliche Textdarstellung auf BIOS- und UEFI-basierten Systemen.