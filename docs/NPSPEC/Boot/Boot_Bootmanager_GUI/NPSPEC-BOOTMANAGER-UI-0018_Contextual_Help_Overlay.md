# NPSPEC-BOOTMANAGER-UI-0018
# Contextual Help Overlay

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0018 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Mittel |
| Gültigkeitsbereich | NovaOS Boot Manager / Recovery / Self-Healing / Installer |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTMANAGER-UI-0001, NPSPEC-BOOTCONTROL-TOOLTIP-0001, NPSPEC-BOOTCONTROL-DIALOG-0001, NPSPEC-BOOTCONTROL-CARD-0001, NPSPEC-BOOTCONTROL-LABEL-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTACCESS-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Contextual Help Overlay** der NovaOS-Bootumgebung.

Das Overlay stellt kontextabhängige Hilfestellungen direkt innerhalb der Benutzeroberfläche bereit. Es erklärt Bedienelemente, Funktionen und mögliche Aktionen, ohne den Benutzer aus seinem aktuellen Arbeitskontext herauszuführen.

Das Hilfesystem ist vollständig lokal verfügbar und benötigt keine Netzwerkverbindung.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Boot Manager
- Recovery
- Installer
- Self-Healing
- Diagnosemodule
- sämtliche Bootdialoge
- zukünftige Bootmodule

---

# 3. Ziele

Das Contextual Help Overlay muss:

- kontextbezogene Hilfe bereitstellen
- den aktuellen Arbeitsablauf nicht unterbrechen
- vollständig per Tastatur bedienbar sein
- lokalisiert werden können
- deterministisch arbeiten
- modular erweiterbar sein

---

# 4. Bildschirmaufbau

```text
+--------------------------------------------------------------+

Aktuelle Oberfläche

--------------------------------------------------------------

? Boot Option

Kurze Erklärung

--------------------------------------------------------------

Tipps

• ...

• ...

• ...

--------------------------------------------------------------

Weitere Informationen

--------------------------------------------------------------

[ Schließen ]

[ Weitere Hilfe ]

+--------------------------------------------------------------+
```

---

# 5. Komponenten

Das Overlay besteht aus:

- Overlay-Hintergrund
- Hilfekarte
- Überschrift
- Beschreibung
- Tipps
- weiterführende Hinweise
- Aktionsbereich

Alle Komponenten verwenden Standard-Boot Controls.

---

# 6. Aktivierung

Das Overlay kann geöffnet werden durch:

- F1
- Hilfe-Schaltfläche
- Kontextmenü
- Touch-Geste
- Accessibility-Funktion
- API

---

# 7. Positionierung

Das Overlay wird:

- möglichst neben dem betreffenden Element angezeigt
- niemals außerhalb des sichtbaren Bereichs dargestellt
- automatisch repositioniert
- bei Bedarf zentriert

---

# 8. Inhalt

Ein Hilfeeintrag enthält:

- Titel
- Kurzbeschreibung
- Detailbeschreibung
- Bedienhinweise
- Tastenkombinationen
- Warnhinweise
- optionale Beispiele

---

# 9. Kontextbezug

Das Hilfesystem erkennt automatisch:

- aktuelles Fenster
- fokussiertes Element
- aktiven Dialog
- ausgewählte Funktion
- aktuellen Bootmodus

Es wird ausschließlich passende Hilfe angezeigt.

---

# 10. Hilfeebenen

Unterstützt werden:

- Kurzinfo
- Standardhilfe
- Erweiterte Hilfe
- Administratorhinweise
- Entwicklerhinweise (optional)

---

# 11. Navigation

Unterstützt werden:

- Tastatur
- Maus
- Touch

Standard-Tasten:

- F1 → Hilfe öffnen
- Esc → Hilfe schließen
- Tab → Fokus wechseln

---

# 12. Fokus

Beim Öffnen erhält:

- die Überschrift den Fokus.

Beim Schließen wird der ursprüngliche Fokus vollständig wiederhergestellt.

---

# 13. Animationen

Unterstützt werden:

- Fade-In
- Fade-Out
- Hintergrundunschärfe
- Fokusanimation
- Kartenanimation

Alle Animationen verwenden Motion Tokens.

---

# 14. Themeintegration

Unterstützt werden:

- Dark Theme
- Light Theme
- High Contrast Theme

Die Hilfekarte verwendet dieselben Materialdefinitionen wie Dialoge.

---

# 15. Accessibility

Das Overlay unterstützt:

- Screenreader
- Tastatursteuerung
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen

Hilfetexte müssen vollständig vorgelesen werden können.

---

# 16. Lokalisierung

Alle Hilfetexte werden über das NovaOS-Lokalisierungssystem geladen.

Unterstützt werden:

- Mehrsprachigkeit
- RTL-Sprachen
- Unicode
- UTF-8

---

# 17. Fehlerfälle

Zu behandeln sind:

- Hilfeeintrag fehlt
- Übersetzung fehlt
- Overlay kann nicht geöffnet werden
- ungültiger Kontext

In diesen Fällen wird eine allgemeine Hilfe angezeigt.

---

# 18. Sicherheit

Das Overlay darf:

- keine internen Debugdaten
- keine kryptographischen Informationen
- keine vertraulichen Diagnosedaten

anzeigen.

Administrator- und Entwicklerhinweise werden ausschließlich bei entsprechender Berechtigung angezeigt.

---

# 19. Speicherverwaltung

Die Oberfläche verwendet:

- Help Pool
- Dialog Pool
- Resource Cache
- Localization Cache
- Theme Cache

Heap-Allokationen während der Darstellung sind verboten.

---

# 20. Performance

Anforderungen:

- Öffnungszeit < 25 ms
- O(1)-Help Lookup
- O(1)-Localization Lookup
- O(1)-Theme Lookup
- deterministische Darstellung

Hilfeeinträge werden beim Systemstart indiziert.

---

# 21. API

```c
typedef struct
{
    uint32_t help_id;
    const char* title;
    const char* summary;
    const char* details;
}
nova_help_entry_t;

nova_result_t
nova_help_overlay_open(
    uint32_t help_id);

nova_result_t
nova_help_overlay_close(void);

const nova_help_entry_t*
nova_help_entry_get(
    uint32_t help_id);

bool
nova_help_available(
    uint32_t help_id);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- geöffneter Hilfeeintrag
- Kontext
- Sprache
- Öffnungsdauer
- Benutzeraktionen
- Theme

Es werden keine Inhalte der Hilfeeingaben protokolliert.

---

# 23. Testfälle

Zu testen sind:

- Hilfe über F1
- Hilfe über Button
- Overlay schließen
- Themewechsel
- Accessibility
- verschiedene Sprachen
- fehlender Hilfeeintrag
- Fokuswiederherstellung
- Touchsteuerung
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- für alle relevanten UI-Elemente kontextbezogene Hilfe verfügbar ist.
- das Overlay den Arbeitsablauf nicht unterbricht.
- sämtliche Texte lokalisiert werden können.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- der ursprüngliche Fokus nach dem Schließen wiederhergestellt wird.
- fehlende Hilfeeinträge korrekt behandelt werden.
- keine Heap-Allokationen während der Darstellung stattfinden.
- sämtliche Animationen ausschließlich Motion Tokens verwenden.

---

# 25. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0018 – Contextual Help Overlay
- ADR-BOOTACCESS-0001 – Accessibility Framework
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTMANAGER-UI-0001 – Main Boot Selection Screen
- NPSPEC-BOOTCONTROL-TOOLTIP-0001 – Tooltip
- NPSPEC-BOOTCONTROL-DIALOG-0001 – Dialog
- NPSPEC-BOOTCONTROL-CARD-0001 – Glass Card
- NPSPEC-BOOTCONTROL-LABEL-0001 – Label
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTACCESS-0001 – Accessibility Framework

---

# 26. Zusammenfassung

Das **Contextual Help Overlay** stellt ein vollständig integriertes, kontextsensitives Hilfesystem für die gesamte NovaOS-Bootumgebung bereit. Es bietet verständliche Erklärungen, Bedienhinweise und weiterführende Informationen direkt am jeweiligen UI-Element, ohne den aktuellen Arbeitsablauf zu unterbrechen. Durch die vollständige Integration in die NovaOS Design Language, das Lokalisierungs- und Accessibility-System sowie die Motion Engine entsteht eine konsistente und benutzerfreundliche Hilfeoberfläche für alle Bereiche des Boot Managers.