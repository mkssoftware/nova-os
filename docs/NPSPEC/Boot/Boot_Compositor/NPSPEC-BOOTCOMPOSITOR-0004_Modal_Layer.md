# NPSPEC-BOOTCOMPOSITOR-0004
# Modal Layer

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTCOMPOSITOR-0004 |
| Titel | Modal Layer |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Compositor |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTCOMPOSITOR-0001, NPSPEC-BOOTCOMPOSITOR-0002, NPSPEC-BOOTCOMPOSITOR-0003, NPSPEC-BOOTINPUT-0005 |
| Zugehörige ADRs | ADR-BOOTDIALOG-0001, ADR-BOOTDIALOG-0002, ADR-BOOTDIALOG-0004, ADR-BOOTNAV-0002, ADR-BOOTSAFE-0005 |

---

# 1. Ziel

Diese Spezifikation definiert den **Modal Layer** des NovaOS Boot Compositors.

Der Modal Layer stellt sicher, dass modale Benutzeroberflächen den Eingabefokus exklusiv erhalten und sämtliche Interaktionen mit darunterliegenden Ebenen unterbunden werden.

Er bildet die Grundlage für:

- Bestätigungsdialoge
- Sicherheitsabfragen
- Recovery-Dialoge
- Passwortabfragen
- Verschlüsselungsdialoge
- Formatierungsdialoge
- Warnmeldungen
- Kritische Systemaktionen

Der Modal Layer ist vollständig in das Compositor-, Eingabe- und Navigationssystem integriert.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für alle modalen Oberflächen des Bootmanagers.

Sie umfasst:

- Modale Dialoge
- Modale Assistenten
- Sicherheitsfenster
- Sperrbildschirme
- Kritische Warnungen

Nicht Bestandteil sind:

- Normale Fenster
- Popups
- Tooltips
- Benachrichtigungen

---

# 3. Architektur

```text
Desktop Layer
        │
        ▼
Dialog Layer
        │
        ▼
Modal Layer
        │
        ▼
Cursor Layer
        │
        ▼
Framebuffer
```

---

# 4. Grundprinzip

Ist mindestens ein Modal Layer aktiv, besitzt ausschließlich dessen Inhalt Eingabefokus.

Alle darunterliegenden Layer bleiben sichtbar, dürfen jedoch keine Eingaben empfangen.

---

# 5. Aufgaben

Der Modal Layer übernimmt:

- Fokusbindung
- Eingabesperre
- Priorisierung
- Hintergrundverdunklung
- Glass-Darstellung
- Animation
- Lebenszyklusverwaltung
- Integration in den Navigations-Stack

---

# 6. Layerstruktur

```c
typedef struct
{
    uint32_t id;

    bool visible;

    bool modal;

    bool blocks_input;

    float opacity;

    nova_surface_t* surface;
} nova_modal_layer_t;
```

---

# 7. Z-Reihenfolge

Standard:

| Ebene | Z |
|---|---:|
| Desktop | 100 |
| Dialog | 300 |
| Overlay | 500 |
| Modal | 600 |
| Cursor | 900 |
| Debug | 1000 |

Ein Modal Layer liegt immer oberhalb normaler Dialoge.

---

# 8. Sichtbarkeit

Ein Modal Layer wird verarbeitet wenn:

- sichtbar
- gültige Surface vorhanden
- Deckkraft > 0

Unsichtbare Modal Layer werden vollständig ignoriert.

---

# 9. Eingabesperre

Während ein Modal Layer aktiv ist:

- werden Mausereignisse blockiert,
- werden Tastaturereignisse blockiert,
- werden Touch-Ereignisse blockiert,

sofern sie nicht an den Modal Layer selbst gerichtet sind.

---

# 10. Fokusbindung

Der Fokus darf den Modal Layer nicht verlassen.

Tabulator-, Pfeil- oder Mausnavigation außerhalb des Modal Layers ist unzulässig.

---

# 11. Hintergrundverdunklung

Ein Modal Layer erzeugt standardmäßig eine halbtransparente Hintergrundfläche.

Eigenschaften:

- Alpha-Blending
- optional animiert
- konfigurierbare Deckkraft

Die Verdunklung gehört zum Modal Layer.

---

# 12. Background Blur

Optional darf ein Modal Layer einen Background Blur verwenden.

Der Blur verarbeitet ausschließlich darunterliegende Layer.

Der Modal Layer selbst wird nicht verwischt.

---

# 13. Glass Design

Modale Dialoge bestehen typischerweise aus:

- Background Blur
- Gradient
- halbtransparenter Fläche
- Schatten
- Glow
- Rahmen

Alle Effekte werden vor dem finalen Compositing berechnet.

---

# 14. Animationen

Unterstützt werden:

- Fade-In
- Fade-Out
- Scale-In
- Scale-Out
- Slide-In
- Slide-Out

Animationen dürfen den Eingabefokus nicht verzögern.

---

# 15. Navigation

Der Modal Layer wird in den Navigations-Stack eingetragen.

Beim Schließen wird automatisch der vorherige Zustand wiederhergestellt.

---

# 16. Mehrere Modal Layer

Mehrere modale Ebenen sind zulässig.

Es gilt:

- Last In
- First Out (LIFO)

Nur der oberste Modal Layer erhält Eingaben.

---

# 17. Schließen

Ein Modal Layer darf geschlossen werden durch:

- Benutzeraktion
- erfolgreiche Aktion
- Fehlerbehandlung
- Navigation
- Systemereignis

Destruktive Dialoge dürfen nicht ohne explizite Bestätigung geschlossen werden.

---

# 18. Clipping

Jeder Modal Layer besitzt einen eigenen Clippingbereich.

Alle Inhalte außerhalb dieses Bereichs werden verworfen.

---

# 19. Dirty Regions

Verändert sich der Modal Layer, wird ausschließlich dessen Bereich invalidiert.

Die darunterliegenden Layer bleiben unverändert.

---

# 20. Speicherverwaltung

Für jeden Modal Layer existiert genau eine Surface.

Temporäre Effektflächen werden zentral verwaltet.

Heap-Allokationen während des Render-Hotpaths sind verboten.

---

# 21. Performance

Optimierungen:

- Dirty Regions
- Blur Cache
- Shadow Cache
- frühes Verwerfen verdeckter Inhalte

Das Öffnen eines Modal Layers darf den Bootprozess nicht merklich verzögern.

---

# 22. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- doppelte Modal Layer
- fehlende Surface
- ungültige Fokusreferenz
- beschädigte Blur Surface
- ungültiger Navigationszustand

Fehler dürfen den Bootprozess nicht blockieren.

---

# 23. Sicherheit

Das System verhindert:

- Fokusverlust
- Eingaben außerhalb des Modal Layers
- rekursive Modaldialoge ohne Begrenzung
- Nullpointer
- ungültige Layerreferenzen

---

# 24. API

```c
nova_result_t
nova_modal_create(
    nova_modal_layer_t* modal);

nova_result_t
nova_modal_show(
    nova_modal_layer_t* modal);

nova_result_t
nova_modal_hide(
    nova_modal_layer_t* modal);

nova_result_t
nova_modal_destroy(
    nova_modal_layer_t* modal);

bool
nova_modal_is_active(void);
```

---

# 25. Diagnosedaten

Zu erfassen sind:

- Anzahl aktiver Modal Layer
- Fokuswechsel
- blockierte Eingaben
- Renderzeit
- Blur-Nutzung
- Animationen
- Speicherverbrauch
- Navigationswechsel

---

# 26. Testfälle

Zu testen sind:

- Modal Layer öffnen
- Modal Layer schließen
- mehrere modale Ebenen
- Fokusbindung
- Hintergrundverdunklung
- Background Blur
- Navigation
- Animationen
- Dirty Regions
- Fehlerfall

---

# 27. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Eingaben ausschließlich an den obersten Modal Layer weitergeleitet werden.
- Fokus den Modal Layer nicht verlassen kann.
- Hintergrundverdunklung und Blur korrekt funktionieren.
- mehrere modale Ebenen im LIFO-Prinzip verwaltet werden.
- Dirty Regions vollständig unterstützt werden.
- keine Heap-Allokationen im Render-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten zeigen.

---

# 28. Referenzen

## ADRs

- ADR-BOOTDIALOG-0001 – Modale Overlay-Dialoge für Untermenüs
- ADR-BOOTDIALOG-0002 – Dialoge als eigene Compositor-Ebene
- ADR-BOOTDIALOG-0004 – Fokusbindung innerhalb modaler Dialoge
- ADR-BOOTNAV-0002 – Hierarchischer Navigations-Stack
- ADR-BOOTSAFE-0005 – Bestätigungsdialoge für destruktive Aktionen

## NPSPECs

- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor
- NPSPEC-BOOTCOMPOSITOR-0002 – Compositor Layer Model
- NPSPEC-BOOTCOMPOSITOR-0003 – Overlay Layer
- NPSPEC-BOOTINPUT-0005 – Eingabefokus und Fokusnavigation
- NPSPEC-BOOTRENDER-0016 – Background Blur

---

# 29. Zusammenfassung

Der **Modal Layer** stellt sicher, dass kritische Dialoge und Systemabfragen im NovaOS Bootmanager stets den exklusiven Eingabefokus erhalten. Durch die Kombination aus eigener Compositor-Ebene, Fokusbindung, Hintergrundverdunklung, Glass-Design, Background Blur und Integration in den Navigations-Stack entsteht eine sichere und konsistente Benutzerführung, die auf BIOS- und UEFI-Systemen identisch funktioniert.