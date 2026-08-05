# NPSPEC-BOOTNAV-0002
# Navigation Stack

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTNAV-0002 |
| Titel | Navigation Stack |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Navigation |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTNAV-0001 |
| Zugehörige ADRs | ADR-BOOTNAV-0002, ADR-BOOTSTATE-0001, ADR-BOOTMEM-0003, ADR-BOOTPERF-0002, ADR-BOOTSAFE-0001 |

---

# 1. Ziel

Diese Spezifikation definiert den **Navigation Stack** der NovaOS Boot UI.

Der Navigation Stack verwaltet den vollständigen Navigationsverlauf sämtlicher Bootoberflächen.

Er ermöglicht:

- Vorwärtsnavigation
- Rücknavigation
- Zustandswiederherstellung
- Dialogverwaltung
- Recovery-Navigation
- Navigation Reset
- Replace Navigation
- Navigation History

Der Stack stellt sicher, dass jede Navigation deterministisch reproduzierbar ist.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Bootoberflächen.

Sie umfasst:

- Bootmenü
- Recovery
- Diagnose
- Einstellungen
- Passwortdialoge
- Backup
- Wiederherstellung
- Fehlerdialoge

Nicht Bestandteil sind:

- Rendering
- Animationen
- Eingabesystem

---

# 3. Architektur

```text
Navigation Controller

↓

Navigation Stack

↓

Current View

↓

Navigation History

↓

Layout Engine

↓

Rendering
```

---

# 4. Grundprinzip

Der Navigation Stack arbeitet nach dem **LIFO-Prinzip (Last In – First Out)**.

Die aktuell sichtbare Ansicht befindet sich immer auf der obersten Stackposition.

Neue Ansichten werden auf den Stack gelegt.

Bei einer Rücknavigation wird ausschließlich der oberste Eintrag entfernt. Dieses Verhalten entspricht dem klassischen Back-Stack-Prinzip moderner Navigationssysteme. :contentReference[oaicite:0]{index=0}

---

# 5. Aufgaben

Der Navigation Stack übernimmt:

- Speicherung der Navigation
- Wiederherstellung
- Push
- Pop
- Replace
- Reset
- Dialogverwaltung
- Recovery Stack

---

# 6. Stackstruktur

```text
Top

↓

Filesystem Check

↓

Recovery

↓

Boot Menu

↓

Root
```

Der Root-Eintrag verbleibt dauerhaft im Stack.

---

# 7. Stackeintrag

Jeder Stackeintrag enthält:

```c
typedef struct
{
    nova_view_id_t view;

    nova_focus_id_t focus;

    uint32_t scroll_position;

    uint32_t flags;

    void* state;

} nova_navigation_entry_t;
```

---

# 8. Push

Beim Öffnen einer neuen Ansicht:

```text
Current View

↓

Push

↓

Neue View
```

Die bisherige Ansicht bleibt vollständig erhalten.

---

# 9. Pop

Beim Zurückkehren:

```text
Top View

↓

Pop

↓

Vorherige View
```

Die vorherige Ansicht wird vollständig wiederhergestellt.

---

# 10. Replace

Replace ersetzt den obersten Stackeintrag.

```text
View A

↓

Replace

↓

View B
```

Die Stacktiefe bleibt unverändert.

---

# 11. Reset

Reset entfernt sämtliche Stackeinträge bis auf:

```text
Root
```

Anschließend wird die definierte Startansicht geöffnet.

---

# 12. Stackgröße

Die maximale Stacktiefe ist konfigurierbar.

Standard:

```text
64 Navigationseinträge
```

Die Größe wird statisch reserviert.

---

# 13. Navigation History

Jeder Stackeintrag speichert:

- aktive View
- Fokus
- Scrollposition
- Parameter
- View-Zustand

Dadurch kann eine Ansicht vollständig wiederhergestellt werden.

---

# 14. Dialog Stack

Dialoge besitzen einen eigenen Unterstack.

```text
Main Stack

↓

Dialog Stack
```

Nach dem Schließen wird automatisch zur vorherigen Ansicht zurückgekehrt.

---

# 15. Recovery Stack

Recovery verwendet einen separaten Navigationszweig.

Beim Verlassen wird automatisch der ursprüngliche Hauptstack aktiviert.

---

# 16. Fokus

Beim Pop wird automatisch wiederhergestellt:

- letzter Fokus
- Scrollposition
- aktive Auswahl

Der Benutzer kehrt exakt an den vorherigen Zustand zurück.

---

# 17. Parameter

Navigationseinträge dürfen Parameter speichern.

Beispiele:

- Dateipfad
- Datenträger
- Recoverymodus
- Diagnoseoption
- Bootprofil

---

# 18. Layoutintegration

Nach jeder Stackänderung erfolgen:

- Layout Invalidierung
- Focus Update
- Render Invalidierung

Nur betroffene Ansichten werden neu aufgebaut.

---

# 19. Accessibility

Der Stack speichert zusätzlich:

- Accessibility-Fokus
- Screenreader-Position
- Auswahlstatus

Dadurch bleibt die Navigation vollständig barrierefrei.

---

# 20. Speicherverwaltung

Der Navigation Stack verwendet:

- statisches Array
- festen Objektpool
- keine dynamischen Listen

Heap-Allokationen sind verboten.

---

# 21. Performance

Anforderungen:

- O(1)-Push
- O(1)-Pop
- O(1)-Peek
- deterministische Laufzeit
- keine Heap-Allokationen

---

# 22. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- Stack Overflow
- Stack Underflow
- beschädigter Eintrag
- ungültige View-ID
- Nullpointer

Im Fehlerfall erfolgt automatisch die Rückkehr zur Root View.

---

# 23. Sicherheit

Das System verhindert:

- doppelte Root Views
- rekursive Stackeinträge
- beschädigte History
- inkonsistente Zustände
- ungültige Stacktiefe

---

# 24. API

```c
nova_result_t
nova_navigation_stack_initialize(void);

nova_result_t
nova_navigation_push(
    nova_view_id_t view);

nova_result_t
nova_navigation_pop(void);

nova_result_t
nova_navigation_replace(
    nova_view_id_t view);

nova_result_t
nova_navigation_reset(void);

const nova_navigation_entry_t*
nova_navigation_peek(void);
```

---

# 25. Diagnosedaten

Zu erfassen sind:

- Stacktiefe
- Push-Operationen
- Pop-Operationen
- Replace-Operationen
- Reset-Operationen
- Recoverywechsel
- Speicherverbrauch
- Fehleranzahl

---

# 26. Testfälle

Zu testen sind:

- Push
- Pop
- Replace
- Reset
- maximale Stacktiefe
- Dialog Stack
- Recovery Stack
- Fokuswiederherstellung
- Accessibility
- Fehlerfälle

---

# 27. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Push-, Pop-, Replace- und Reset-Operationen vollständig unterstützt werden.
- sämtliche Navigationszustände korrekt wiederhergestellt werden.
- Dialoge und Recovery eigene Navigationskontexte besitzen.
- Stackoperationen deterministisch arbeiten.
- keine Heap-Allokationen während der Navigation stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 28. Referenzen

## ADRs

- ADR-BOOTNAV-0002 – Navigation Stack
- ADR-BOOTSTATE-0001 – Globales Zustandsmodell
- ADR-BOOTMEM-0003 – Statische Speicherverwaltung
- ADR-BOOTPERF-0002 – Deterministische Laufzeiten
- ADR-BOOTSAFE-0001 – Sicherer Rückfallzustand

## NPSPECs

- NPSPEC-BOOTNAV-0001 – Boot Navigation Model
- NPSPEC-BOOTMOTION-0007 – Transition System
- NPSPEC-BOOTMOTION-0012 – Interruptible Animations
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTDIALOG-0001 – Dialog System

---

# 29. Zusammenfassung

Der **Navigation Stack** verwaltet den vollständigen Navigationsverlauf der NovaOS Bootoberfläche. Durch ein statisches, deterministisches LIFO-Modell mit Push-, Pop-, Replace- und Reset-Operationen können sämtliche Ansichten einschließlich Fokus, Scrollposition und Kontext zuverlässig wiederhergestellt werden. Die feste Speicherverwaltung, getrennte Dialog- und Recovery-Stacks sowie O(1)-Operationen gewährleisten ein reproduzierbares und sicheres Navigationsverhalten auf BIOS- und UEFI-Systemen. Moderne Navigationsarchitekturen verwenden ebenfalls einen zentralen Back Stack, bei dem neue Ziele auf den Stack gelegt und Rücknavigationen durch Entfernen des obersten Eintrags erfolgen. :contentReference[oaicite:1]{index=1}