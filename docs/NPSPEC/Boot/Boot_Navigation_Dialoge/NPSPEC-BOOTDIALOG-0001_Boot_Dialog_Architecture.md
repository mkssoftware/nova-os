# NPSPEC-BOOTDIALOG-0001
# Boot Dialog Architecture

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTDIALOG-0001 |
| Titel | Boot Dialog Architecture |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Dialog System |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTNAV-0001 bis NPSPEC-BOOTNAV-0005, NPSPEC-BOOTLAYOUT-0001 bis NPSPEC-BOOTLAYOUT-0010, NPSPEC-BOOTCOMPOSITOR-0001 bis NPSPEC-BOOTCOMPOSITOR-0010 |
| Zugehörige ADRs | ADR-BOOTDIALOG-0001, ADR-BOOTCOMPOSITOR-0004, ADR-BOOTNAV-0003, ADR-BOOTMOTION-0008, ADR-BOOTACCESS-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die vollständige **Boot Dialog Architecture** der NovaOS Boot UI.

Das Dialogsystem stellt eine einheitliche Architektur für sämtliche Dialoge während des Bootvorgangs bereit.

Es unterstützt:

- modale Dialoge
- nichtmodale Dialoge
- Bestätigungsdialoge
- Eingabedialoge
- Fehlermeldungen
- Fortschrittsdialoge
- Recovery-Dialoge
- Sicherheitsdialoge

Alle Dialoge verwenden dieselbe Infrastruktur und dasselbe Lebenszyklusmodell.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Dialoge der Bootoberfläche.

Sie umfasst:

- Boot Manager
- Recovery
- Diagnose
- Backup
- Wiederherstellung
- Verschlüsselung
- Einstellungen
- Wartung

Nicht Bestandteil sind:

- normale Seiten
- Rendering Engine
- Layout Engine

---

# 3. Architektur

```text
Application

↓

Dialog Manager

↓

Dialog Controller

↓

Dialog

↓

Layout Engine

↓

Compositor

↓

Rendering
```

---

# 4. Grundprinzip

Ein Dialog ist eine eigenständige UI-Komponente.

Jeder Dialog besitzt:

- eigene Root View
- eigenes Layout
- eigenen Fokus
- eigenen Lebenszyklus

Dialoge werden unabhängig von normalen Seiten verwaltet.

---

# 5. Aufgaben

Die Dialogarchitektur übernimmt:

- Dialogerstellung
- Aktivierung
- Fokusverwaltung
- Eingabeverarbeitung
- Dialogschließung
- Ergebnisverwaltung
- Ressourcenverwaltung
- Navigationseinbindung

---

# 6. Dialogarten

Unterstützt werden:

- Information
- Warning
- Error
- Confirmation
- Progress
- Password
- Recovery
- Custom

---

# 7. Dialoghierarchie

```text
Dialog Manager

↓

Dialog

↓

Root View

↓

Content

↓

Controls
```

Jeder Dialog besitzt genau eine Root View.

---

# 8. Dialoglebenszyklus

Jeder Dialog durchläuft:

```text
Created

↓

Initialized

↓

Opening

↓

Visible

↓

Active

↓

Closing

↓

Destroyed
```

Ein Überspringen einzelner Zustände ist unzulässig.

---

# 9. Modale Dialoge

Modale Dialoge:

- blockieren sämtliche Eingaben außerhalb des Dialogs
- besitzen höchste Fokuspriorität
- sperren die darunterliegende Oberfläche

Es darf immer nur ein modaler Dialog gleichzeitig aktiv sein.

---

# 10. Nichtmodale Dialoge

Nichtmodale Dialoge:

- blockieren keine Eingaben
- können parallel zu anderen Ansichten existieren
- besitzen eine niedrigere Priorität

---

# 11. Dialog Stack

Alle aktiven Dialoge werden über einen separaten Dialog Stack verwaltet.

```text
Dialog 2

↓

Dialog 1
```

Der oberste Eintrag besitzt den Fokus.

---

# 12. Dialogpositionierung

Standardmäßig werden Dialoge:

- horizontal zentriert
- vertikal zentriert

Innerhalb der Safe Display Area positioniert.

---

# 13. Dialoggröße

Die Größe ergibt sich aus:

- Inhalt
- Constraints
- Mindestgrößen
- maximal verfügbarer Fläche

Dialoge dürfen niemals größer als die Safe Display Area werden.

---

# 14. Hintergrund

Bei modalen Dialogen wird ein Backdrop aktiviert.

Der Backdrop:

- dimmt den Hintergrund
- blockiert Eingaben
- wird durch den Compositor erzeugt

---

# 15. Fokus

Beim Öffnen:

- erhält das erste fokussierbare Element den Fokus.

Beim Schließen:

- wird der vorherige Fokus vollständig wiederhergestellt.

---

# 16. Navigation

Dialoge besitzen einen eigenen Navigationskontext.

Ein Back-Ereignis schließt zunächst den obersten Dialog.

Erst danach wird die normale Navigation fortgesetzt.

---

# 17. Dialogergebnis

Jeder Dialog liefert genau ein Ergebnis.

Beispiele:

- OK
- Cancel
- Yes
- No
- Retry
- Ignore
- Abort

Optional können zusätzliche Rückgabewerte definiert werden.

---

# 18. Eingaben

Unterstützt werden:

- Tastatur
- Maus
- Touch
- Controller
- Screenreader

Alle Eingaben werden ausschließlich an den aktiven Dialog weitergeleitet.

---

# 19. Motion

Dialoge verwenden standardmäßig:

- Fade
- leichte Skalierung
- Fokusanimation

Bei aktiviertem Reduced Motion wird ausschließlich eine Fade-Animation verwendet.

---

# 20. Accessibility

Jeder Dialog unterstützt:

- Screenreader
- skalierbare Schriftgrößen
- High Contrast
- Reduced Motion
- vollständige Tastaturbedienung

Die Fokusreihenfolge muss eindeutig definiert sein.

---

# 21. Speicherverwaltung

Dialoge verwenden:

- statische Objektpools
- feste Layoutobjekte
- feste Dialoglisten

Heap-Allokationen während der Laufzeit sind verboten.

---

# 22. Performance

Anforderungen:

- O(1)-Dialogöffnung
- O(1)-Dialogschließung
- deterministische Laufzeit
- keine Heap-Allokationen

Dialoge dürfen das Animation Performance Budget nicht überschreiten.

---

# 23. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- fehlende Root View
- doppelter Dialog
- beschädigter Dialog Stack
- ungültiger Lebenszyklus
- Nullpointer

Im Fehlerfall wird der Dialog kontrolliert geschlossen.

---

# 24. Sicherheit

Das System verhindert:

- mehrere aktive modale Dialoge
- rekursive Dialoge
- Fokusverlust
- inkonsistente Dialogzustände
- beschädigte Dialoghierarchien

---

# 25. API

```c
typedef enum
{
    NOVA_DIALOG_INFORMATION,
    NOVA_DIALOG_WARNING,
    NOVA_DIALOG_ERROR,
    NOVA_DIALOG_CONFIRMATION,
    NOVA_DIALOG_PROGRESS,
    NOVA_DIALOG_PASSWORD,
    NOVA_DIALOG_RECOVERY,
    NOVA_DIALOG_CUSTOM
}
nova_dialog_type_t;

nova_result_t
nova_dialog_create(
    nova_dialog_type_t type,
    nova_dialog_t** dialog);

nova_result_t
nova_dialog_show(
    nova_dialog_t* dialog);

nova_result_t
nova_dialog_close(
    nova_dialog_t* dialog);

nova_dialog_result_t
nova_dialog_result(
    const nova_dialog_t* dialog);
```

---

# 26. Diagnosedaten

Zu erfassen sind:

- Dialogtyp
- Lebenszyklus
- Dialog Stack
- Fokuswechsel
- Öffnungszeit
- Schließzeit
- Speicherverbrauch
- Fehleranzahl

---

# 27. Testfälle

Zu testen sind:

- Informationsdialog
- Fehlerdialog
- Passwortdialog
- Progressdialog
- modaler Dialog
- nichtmodaler Dialog
- Dialog Stack
- Accessibility
- Reduced Motion
- Recovery Dialog

---

# 28. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Dialoge dieselbe Architektur verwenden.
- modale Dialoge Eingaben zuverlässig blockieren.
- Dialoge einen eigenen Navigationskontext besitzen.
- Fokus nach dem Schließen korrekt wiederhergestellt wird.
- Accessibility vollständig unterstützt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 29. Referenzen

## ADRs

- ADR-BOOTDIALOG-0001 – Einheitliche Dialogarchitektur
- ADR-BOOTCOMPOSITOR-0004 – Modal Layer
- ADR-BOOTNAV-0003 – Page- und View-Modell
- ADR-BOOTMOTION-0008 – Dialog Enter and Exit Motion
- ADR-BOOTACCESS-0001 – Barrierefreie Navigation

## NPSPECs

- NPSPEC-BOOTNAV-0003 – Page and View Model
- NPSPEC-BOOTNAV-0004 – Back Navigation
- NPSPEC-BOOTNAV-0005 – Navigation Transitions
- NPSPEC-BOOTLAYOUT-0005 – Overlay Layout
- NPSPEC-BOOTMOTION-0008 – Dialog Enter and Exit Motion
- NPSPEC-BOOTCOMPOSITOR-0004 – Modal Layer

---

# 30. Zusammenfassung

Die **Boot Dialog Architecture** definiert eine einheitliche Infrastruktur für sämtliche Dialoge der NovaOS Bootoberfläche. Jeder Dialog besitzt eine eigene Root View, einen eigenen Navigations- und Fokuskontext sowie einen klar definierten Lebenszyklus. Durch einen separaten Dialog Stack, deterministische Zustandsübergänge, vollständige Accessibility-Unterstützung und statische Speicherverwaltung entsteht ein konsistentes, sicheres und performant arbeitendes Dialogsystem für BIOS- und UEFI-basierte Bootumgebungen.