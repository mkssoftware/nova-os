# NPSPEC-BOOTDIALOG-0002
# Modal Dialog

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTDIALOG-0002 |
| Titel | Modal Dialog |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Dialog System |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTDIALOG-0001, NPSPEC-BOOTCOMPOSITOR-0004, NPSPEC-BOOTNAV-0004 |
| Zugehörige ADRs | ADR-BOOTDIALOG-0002, ADR-BOOTCOMPOSITOR-0004, ADR-BOOTINPUT-0006, ADR-BOOTACCESS-0001, ADR-BOOTSAFE-0002 |

---

# 1. Ziel

Diese Spezifikation definiert den **Modal Dialog** der NovaOS Boot UI.

Ein modaler Dialog blockiert sämtliche Interaktionen mit der darunterliegenden Benutzeroberfläche, bis der Dialog abgeschlossen oder geschlossen wurde.

Das System unterstützt:

- Bestätigungsdialoge
- Passwortdialoge
- Sicherheitsdialoge
- Fehlermeldungen
- Warnungen
- Recovery-Dialoge
- kritische Systemabfragen

Modale Dialoge besitzen höchste Priorität innerhalb der Benutzeroberfläche.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für alle modalen Dialoge.

Sie umfasst:

- Boot Manager
- Recovery
- Diagnose
- Backup
- Wiederherstellung
- Verschlüsselung
- Systemeinstellungen

Nicht Bestandteil sind:

- nichtmodale Dialoge
- normale Seiten
- Benachrichtigungen

---

# 3. Architektur

```text
Application

↓

Dialog Manager

↓

Modal Dialog

↓

Modal Layer

↓

Backdrop

↓

Rendering
```

---

# 4. Grundprinzip

Ein modaler Dialog sperrt sämtliche Interaktionen außerhalb seines eigenen Dialogfensters.

Während der Laufzeit:

- ist genau ein modaler Dialog aktiv.
- besitzt ausschließlich dieser Dialog den Fokus.
- werden alle Eingaben ausschließlich an diesen Dialog weitergeleitet.

---

# 5. Aufgaben

Der modale Dialog übernimmt:

- Eingabesperre
- Fokusverwaltung
- Dialogsteuerung
- Ergebnisverwaltung
- Sicherheitsabfragen
- Navigation Blockierung
- Backdrop-Steuerung
- Lebenszyklusverwaltung

---

# 6. Eigenschaften

Ein modaler Dialog besitzt:

- Dialog-ID
- Root View
- Dialogtyp
- Priorität
- Fokuskontext
- Ergebnisstatus
- Lebenszyklus

---

# 7. Dialoglebenszyklus

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

Zwischenzustände sind nicht zulässig.

---

# 8. Modal Layer

Jeder modale Dialog wird im **Modal Layer** dargestellt.

Dieser Layer liegt oberhalb:

- Content
- Overlay
- Popups

Nur Debug-Overlays dürfen optional darüber dargestellt werden.

---

# 9. Backdrop

Beim Öffnen wird automatisch ein Backdrop aktiviert.

Eigenschaften:

- Hintergrund abdunkeln
- Eingaben blockieren
- Fokus auf Dialog lenken

Der Backdrop wird automatisch mit dem Dialog entfernt.

---

# 10. Fokus

Während der gesamten Laufzeit gilt:

- Fokus bleibt innerhalb des Dialogs.
- Fokus darf den Dialog nicht verlassen.
- Fokuszyklen werden automatisch geschlossen.

Dies entspricht einem **Focus Trap**.

---

# 11. Navigation

Während ein modaler Dialog aktiv ist:

- wird keine Seitennavigation ausgeführt.
- wird keine Recovery-Navigation ausgeführt.
- werden keine Unterseiten geöffnet.

Navigation wird erst nach Schließen des Dialogs fortgesetzt.

---

# 12. Back Navigation

Ein Back-Ereignis bewirkt:

- Dialog schließen

oder

- definierte Standardaktion

Die eigentliche Seitennavigation wird nicht ausgeführt.

---

# 13. Dialogergebnis

Standardergebnisse:

- OK
- Cancel
- Yes
- No
- Retry
- Ignore
- Abort

Eigene Ergebnisse können ergänzt werden.

---

# 14. Sicherheitsdialoge

Kritische Dialoge dürfen:

- Schließen verhindern
- Bestätigung verlangen
- Passwort erfordern
- Warnhinweise anzeigen

Ein versehentliches Schließen muss ausgeschlossen sein.

---

# 15. Layout

Dialoge werden:

- zentriert
- innerhalb der Safe Display Area dargestellt
- automatisch skaliert

Die maximale Größe ist durch die Safe Area begrenzt.

---

# 16. Motion

Standardanimation:

```text
Fade

+

Scale

+

Backdrop Fade
```

Bei aktiviertem Reduced Motion:

```text
Fade
```

---

# 17. Accessibility

Modale Dialoge unterstützen:

- vollständige Tastatursteuerung
- Screenreader
- High Contrast
- Reduced Motion
- skalierbare Schriftgrößen

Der Dialogtitel muss als primäres Accessibility-Element verfügbar sein.

---

# 18. Speicherverwaltung

Modale Dialoge verwenden:

- statische Dialogobjekte
- feste Speicherpools
- feste Fokusobjekte

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Öffnen
- O(1)-Schließen
- deterministische Laufzeit
- keine Heap-Allokationen

Die Darstellung muss innerhalb des Animation Performance Budgets erfolgen.

---

# 20. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- zweiter modaler Dialog
- beschädigter Fokus
- fehlender Backdrop
- ungültiger Lebenszyklus
- Nullpointer

Im Fehlerfall wird der Dialog kontrolliert geschlossen und der vorherige Zustand wiederhergestellt.

---

# 21. Sicherheit

Das System verhindert:

- mehrere gleichzeitige modale Dialoge
- Fokusverlust
- Navigation außerhalb des Dialogs
- rekursive Modaldialoge
- inkonsistente Zustände

---

# 22. API

```c
typedef struct nova_modal_dialog nova_modal_dialog_t;

nova_result_t
nova_modal_dialog_create(
    nova_dialog_type_t type,
    nova_modal_dialog_t** dialog);

nova_result_t
nova_modal_dialog_show(
    nova_modal_dialog_t* dialog);

nova_result_t
nova_modal_dialog_close(
    nova_modal_dialog_t* dialog);

bool
nova_modal_dialog_active(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Dialogtyp
- Öffnungszeit
- Schließzeit
- Fokuswechsel
- Backdropstatus
- Dialogergebnis
- Speicherverbrauch
- Fehleranzahl

---

# 24. Testfälle

Zu testen sind:

- Informationsdialog
- Passwortdialog
- Sicherheitsdialog
- Fehlermeldung
- Recovery-Dialog
- Fokusfalle
- Back Navigation
- Reduced Motion
- Accessibility
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- stets höchstens ein modaler Dialog gleichzeitig aktiv ist.
- sämtliche Eingaben ausschließlich an den aktiven Dialog weitergeleitet werden.
- Navigation während eines modalen Dialogs blockiert wird.
- Fokus den Dialog nicht verlassen kann.
- Backdrop und Animationen korrekt dargestellt werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTDIALOG-0002 – Modale Dialoge
- ADR-BOOTCOMPOSITOR-0004 – Modal Layer
- ADR-BOOTINPUT-0006 – Fokusfalle für modale Oberflächen
- ADR-BOOTACCESS-0001 – Barrierefreie Navigation
- ADR-BOOTSAFE-0002 – Sichere Bestätigungsdialoge

## NPSPECs

- NPSPEC-BOOTDIALOG-0001 – Boot Dialog Architecture
- NPSPEC-BOOTNAV-0004 – Back Navigation
- NPSPEC-BOOTLAYOUT-0005 – Overlay Layout
- NPSPEC-BOOTCOMPOSITOR-0004 – Modal Layer
- NPSPEC-BOOTMOTION-0008 – Dialog Enter and Exit Motion
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 27. Zusammenfassung

Der **Modal Dialog** stellt die zentrale Komponente für alle sicherheitsrelevanten und bestätigungspflichtigen Benutzerinteraktionen der NovaOS Bootoberfläche dar. Durch einen exklusiven Fokus, einen separaten Modal Layer, automatische Backdrop-Verwaltung und vollständige Blockierung der Hintergrundnavigation gewährleistet das System eine konsistente und sichere Bedienung. Die deterministische Zustandsverwaltung, statische Speicherverwaltung und vollständige Integration in Navigation, Motion und Accessibility sorgen für identisches Verhalten auf BIOS- und UEFI-Systemen.