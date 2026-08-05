# NPSPEC-BOOTDIALOG-0009
# Dialog Focus Management

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTDIALOG-0009 |
| Titel | Dialog Focus Management |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Dialog System |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTDIALOG-0001 bis NPSPEC-BOOTDIALOG-0008, NPSPEC-BOOTINPUT-0005, NPSPEC-BOOTNAV-0004 |
| Zugehörige ADRs | ADR-BOOTDIALOG-0009, ADR-BOOTINPUT-0005, ADR-BOOTINPUT-0006, ADR-BOOTACCESS-0001, ADR-BOOTNAV-0004 |

---

# 1. Ziel

Diese Spezifikation definiert das **Dialog Focus Management** der NovaOS Boot UI.

Das Focus Management stellt sicher, dass sämtliche Dialoge jederzeit einen konsistenten, vorhersehbaren und barrierefreien Eingabefokus besitzen.

Das System unterstützt:

- automatischen Initialfokus
- Focus Trap
- Fokuswiederherstellung
- Tastaturnavigation
- Controller-Navigation
- Mausfokus
- Accessibility-Fokus
- Fokusprioritäten

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Dialoge.

Sie umfasst:

- modale Dialoge
- nichtmodale Dialoge
- Passwortdialoge
- Fehlerdialoge
- Warnungsdialoge
- Recovery-Dialoge
- Fortschrittsdialoge

Nicht Bestandteil sind:

- Seitennavigation
- Rendering
- Layoutberechnung

---

# 3. Architektur

```text
Input

↓

Focus Manager

↓

Dialog Focus Controller

↓

Focused Control

↓

Input Dispatch
```

---

# 4. Grundprinzip

Zu jedem Zeitpunkt besitzt genau **ein** Steuerelement den Eingabefokus.

Während ein Dialog aktiv ist:

- verbleibt der Fokus vollständig innerhalb des Dialogs.
- dürfen Hintergrundelemente keinen Fokus erhalten.
- werden sämtliche Eingaben ausschließlich an das fokussierte Element weitergeleitet.

---

# 5. Aufgaben

Das Focus Management übernimmt:

- Initialfokus
- Fokuswechsel
- Focus Trap
- Fokuswiederherstellung
- Tab-Reihenfolge
- Controllernavigation
- Accessibility-Fokus
- Fokusvalidierung

---

# 6. Initialfokus

Beim Öffnen eines Dialogs erhält automatisch:

1. explizit definierter Standardfokus
2. empfohlene sichere Aktion
3. erstes fokussierbares Element

den Eingabefokus.

---

# 7. Focus Trap

Während der Dialog aktiv ist:

```text
Tab

↓

Element 1

↓

Element 2

↓

Element 3

↓

Element 1
```

Der Fokus darf den Dialog niemals verlassen.

---

# 8. Fokusreihenfolge

Die Reihenfolge wird explizit definiert.

Standard:

- oben nach unten
- links nach rechts

Eigene Fokusreihenfolgen sind zulässig.

---

# 9. Fokuswechsel

Ein Fokuswechsel erfolgt durch:

- Tab
- Shift+Tab
- Pfeiltasten
- Controller
- Maus
- Touch

Jeder Wechsel wird zentral durch den Focus Manager verarbeitet.

---

# 10. Fokuswiederherstellung

Beim Schließen eines Dialogs werden wiederhergestellt:

- zuvor fokussiertes Steuerelement
- Accessibility-Fokus
- Navigation Context

Die Wiederherstellung erfolgt automatisch.

---

# 11. Verschachtelte Dialoge

Bei mehreren Dialogen:

```text
Dialog 2

↓

Dialog 1

↓

Page
```

Nur der oberste Dialog besitzt den Fokus.

Nach dem Schließen kehrt der Fokus automatisch zum darunterliegenden Dialog zurück.

---

# 12. Ungültiger Fokus

Wird das aktuelle Element entfernt:

- wird automatisch das nächste gültige Element fokussiert.
- existiert keines, erhält der Dialogcontainer den Fokus.

---

# 13. Maus

Ein Mausklick auf ein fokussierbares Element:

- aktiviert das Element
- setzt den Fokus
- aktualisiert den Accessibility-Fokus

---

# 14. Touch

Eine Touch-Auswahl:

- aktiviert das Ziel
- setzt den Fokus
- löst bei Bedarf die Aktion aus

---

# 15. Controller

Controllernavigation erfolgt über:

- Richtungstasten
- Bestätigen
- Zurück

Die Fokusreihenfolge bleibt identisch zur Tastatursteuerung.

---

# 16. Accessibility

Unterstützt werden:

- Screenreader
- High Contrast
- Reduced Motion
- Tastaturnavigation
- alternative Eingabegeräte

Jeder Fokuswechsel muss eindeutig angekündigt werden.

---

# 17. Navigation

Ein Back-Ereignis:

- schließt den Dialog
- stellt anschließend den vorherigen Fokus vollständig wieder her

Während eines Dialogs findet keine Hintergrundnavigation statt.

---

# 18. Darstellung

Das fokussierte Element besitzt:

- sichtbaren Fokusrahmen
- ausreichenden Kontrast
- eindeutige Hervorhebung

Die Darstellung erfolgt unabhängig vom Theme.

---

# 19. Speicherverwaltung

Das Focus Management verwendet:

- statische Fokusobjekte
- feste Fokuslisten
- statische Dialogreferenzen

Heap-Allokationen während der Laufzeit sind verboten.

---

# 20. Performance

Anforderungen:

- O(1)-Fokuswechsel
- O(1)-Fokusabfrage
- deterministische Laufzeit
- keine Heap-Allokationen

Fokuswechsel dürfen die Darstellung nicht verzögern.

---

# 21. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiges Fokuselement
- fehlender Fokus
- rekursive Fokuswechsel
- beschädigte Fokusliste
- Nullpointer

Im Fehlerfall erhält der Dialogcontainer den Fokus.

---

# 22. Sicherheit

Das System verhindert:

- Fokus außerhalb des Dialogs
- mehrere aktive Fokuselemente
- Fokusverlust
- rekursive Fokuswechsel
- inkonsistente Fokuszustände

---

# 23. API

```c
typedef struct nova_focus_node nova_focus_node_t;

nova_result_t
nova_dialog_focus_initialize(void);

nova_result_t
nova_dialog_focus_set(
    nova_focus_node_t* node);

nova_focus_node_t*
nova_dialog_focus_current(void);

nova_result_t
nova_dialog_focus_restore(void);
```

---

# 24. Diagnosedaten

Zu erfassen sind:

- aktuelles Fokuselement
- Fokuswechsel
- Focus Trap Aktivierungen
- Accessibility-Fokus
- Dialog-ID
- Controllernavigation
- Speicherverbrauch
- Fehleranzahl

---

# 25. Testfälle

Zu testen sind:

- Initialfokus
- Focus Trap
- Tab-Navigation
- Shift+Tab
- Controllersteuerung
- Mausfokus
- Touch
- Fokuswiederherstellung
- Accessibility
- verschachtelte Dialoge

---

# 26. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- immer genau ein aktives Fokuselement existiert.
- der Fokus den Dialog nicht verlassen kann.
- Initialfokus automatisch gesetzt wird.
- Fokus nach dem Schließen korrekt wiederhergestellt wird.
- Accessibility vollständig unterstützt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Fokusverhalten erzeugen.

---

# 27. Referenzen

## ADRs

- ADR-BOOTDIALOG-0009 – Dialog Focus Management
- ADR-BOOTINPUT-0005 – Fokusnavigation
- ADR-BOOTINPUT-0006 – Focus Trap
- ADR-BOOTACCESS-0001 – Barrierefreie Navigation
- ADR-BOOTNAV-0004 – Back Navigation

## NPSPECs

- NPSPEC-BOOTDIALOG-0001 – Boot Dialog Architecture
- NPSPEC-BOOTDIALOG-0002 – Modal Dialog
- NPSPEC-BOOTNAV-0004 – Back Navigation
- NPSPEC-BOOTINPUT-0005 – Focus Management
- NPSPEC-BOOTINPUT-0006 – Keyboard Navigation
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 28. Zusammenfassung

Das **Dialog Focus Management** definiert die vollständige Verwaltung des Eingabefokus innerhalb sämtlicher Dialoge der NovaOS Bootoberfläche. Es gewährleistet einen eindeutigen Initialfokus, eine geschlossene Focus Trap, konsistente Fokuswiederherstellung sowie vollständige Unterstützung für Tastatur-, Maus-, Touch- und Controller-Eingaben. Durch deterministische Fokuswechsel, statische Speicherverwaltung und vollständige Accessibility-Integration entsteht ein robustes und reproduzierbares Fokusmodell für BIOS- und UEFI-basierte Bootumgebungen.