# NPSPEC-BOOTNAV-0001
# Boot Navigation Model

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTNAV-0001 |
| Titel | Boot Navigation Model |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Navigation |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTUI-0001 bis NPSPEC-BOOTUI-0010, NPSPEC-BOOTLAYOUT-0001 bis NPSPEC-BOOTLAYOUT-0010 |
| Zugehörige ADRs | ADR-BOOTNAV-0001, ADR-BOOTNAV-0002, ADR-BOOTSTATE-0001, ADR-BOOTINPUT-0005, ADR-BOOTACCESS-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das vollständige **Boot Navigation Model** der NovaOS Boot UI.

Das Modell beschreibt sämtliche Navigationsabläufe innerhalb der Bootoberfläche.

Es definiert:

- Navigationsstruktur
- Seitenhierarchie
- Zustandsmodell
- Navigation Stack
- Fokuswechsel
- Dialognavigation
- Rücknavigation
- Recovery-Navigation

Das Navigation Model bildet die Grundlage sämtlicher Benutzerinteraktionen.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Bootoberflächen.

Sie umfasst:

- Hauptmenü
- Einstellungen
- Recovery
- Diagnose
- Passwortdialoge
- Backup
- Wiederherstellung
- Bootoptionen

Nicht Bestandteil sind:

- Rendering
- Layout
- Animationen

---

# 3. Architektur

```text
Input Event

↓

Navigation Controller

↓

Navigation Model

↓

Navigation Stack

↓

Current View

↓

Layout Engine

↓

Rendering
```

---

# 4. Grundprinzip

Die Navigation basiert vollständig auf einem **zustandsorientierten Navigationsmodell**.

Zu jedem Zeitpunkt existiert genau:

- eine aktive Ansicht
- ein aktiver Fokus
- ein definierter Navigationszustand

Die Navigation wird als Zustandsgraph mit eindeutigen Übergängen modelliert. Dieses Prinzip entspricht modernen Navigation-Stacks, bei denen Navigation über einen kontrollierten Zustandswechsel erfolgt. :contentReference[oaicite:0]{index=0}

---

# 5. Aufgaben

Das Navigation Model übernimmt:

- Seitenwechsel
- Zustandsverwaltung
- Back Stack
- Fokusverwaltung
- Dialogsteuerung
- Navigation History
- Recovery Navigation
- Navigation Events

---

# 6. Navigation State

Jede Ansicht besitzt genau einen Zustand.

```text
Inactive

↓

Loading

↓

Active

↓

Suspended

↓

Closed
```

Ungültige Zustandswechsel sind unzulässig.

---

# 7. Navigation Tree

Alle Ansichten bilden einen hierarchischen Navigationsbaum.

```text
Root
├── Boot Menu
├── Settings
├── Recovery
├── Diagnostics
└── Shutdown
```

Jede Ansicht besitzt genau einen Elternknoten.

---

# 8. Navigation Stack

Beim Wechsel in eine neue Ansicht wird die aktuelle Ansicht auf den Navigation Stack gelegt.

```text
Root

↓

Boot Menu

↓

Recovery

↓

Filesystem Check
```

Bei einer Rücknavigation wird der oberste Stack-Eintrag entfernt.

Der Navigation Stack stellt den vollständigen Navigationsverlauf dar. :contentReference[oaicite:1]{index=1}

---

# 9. Root View

Die Root View besitzt keinen Elternknoten.

Sie bildet den Ursprung sämtlicher Navigation.

Sie darf niemals aus dem Stack entfernt werden.

---

# 10. Navigation Transition

Jeder Seitenwechsel besteht aus:

```text
Leave

↓

Deactivate

↓

Push / Pop

↓

Activate

↓

Enter
```

Die Transition erfolgt atomar.

---

# 11. Vorwärtsnavigation

Eine Vorwärtsnavigation:

- öffnet eine neue Ansicht
- erweitert den Stack
- übernimmt den Fokus

---

# 12. Rücknavigation

Eine Rücknavigation:

- entfernt die aktuelle Ansicht
- aktiviert den vorherigen Stackeintrag
- stellt Fokus und Scrollposition wieder her

Die Wiederherstellung des vorherigen Zustands erfolgt vollständig über den Navigation Stack. :contentReference[oaicite:2]{index=2}

---

# 13. Dialognavigation

Dialoge besitzen einen eigenen Navigationskontext.

Sie blockieren die Navigation der darunterliegenden Ansicht.

Nach dem Schließen wird automatisch zur vorherigen Ansicht zurückgekehrt.

---

# 14. Modale Navigation

Modale Ansichten:

- blockieren Eingaben
- blockieren Navigation
- besitzen höchste Priorität

Nur ein modaler Dialog darf gleichzeitig aktiv sein.

---

# 15. Fokusübernahme

Nach jedem Seitenwechsel erhält automatisch:

- das erste fokussierbare Element
- oder das zuletzt gespeicherte Element

den Eingabefokus.

---

# 16. Navigation Events

Folgende Ereignisse existieren:

- Navigate
- Back
- Cancel
- Confirm
- Open Dialog
- Close Dialog
- Replace View
- Reset Navigation

Alle Navigationen erfolgen ausschließlich über diese Ereignisse.

---

# 17. Navigation History

Der Navigation Stack speichert:

- aktuelle Ansicht
- Elternansicht
- Fokus
- Scrollposition
- Parameter

Damit kann jede Ansicht vollständig wiederhergestellt werden.

---

# 18. Recovery Navigation

Recovery-Ansichten bilden einen eigenen Navigationszweig.

Beim Verlassen des Recovery-Modus wird automatisch zum vorherigen Hauptzustand zurückgekehrt.

---

# 19. Fehlernavigation

Bei einem Fehler:

```text
Aktuelle Ansicht

↓

Fehlerdialog

↓

Benutzerentscheidung

↓

Vorherige Ansicht
```

Die Navigation bleibt konsistent.

---

# 20. Layoutintegration

Jeder Seitenwechsel erzeugt:

- Layout Invalidierung
- Render Invalidierung
- Focus Update

Nur die betroffenen Bereiche werden neu aufgebaut.

---

# 21. Accessibility

Die Navigation unterstützt:

- Tastatur
- Maus
- Touch
- Screenreader
- alternative Eingabegeräte

Die Navigationslogik bleibt unabhängig vom Eingabegerät.

---

# 22. Speicherverwaltung

Der Navigation Stack verwendet:

- feste Objektpools
- feste Stackgröße
- keine dynamischen Listen

Heap-Allokationen während der Navigation sind verboten.

---

# 23. Performance

Anforderungen:

- O(1)-Push
- O(1)-Pop
- deterministische Laufzeit
- keine Heap-Allokationen

Navigation darf niemals die Bildwiederholrate beeinflussen.

---

# 24. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiger Navigationszustand
- beschädigter Stack
- ungültige Zielansicht
- rekursive Navigation
- Nullpointer

Im Fehlerfall erfolgt die Rückkehr zur letzten gültigen Ansicht.

---

# 25. Sicherheit

Das System verhindert:

- doppelte Navigation
- Stacküberläufe
- ungültige Zustandswechsel
- rekursive Navigation
- inkonsistente Fokuszustände

---

# 26. API

```c
typedef enum
{
    NOVA_NAV_PUSH,
    NOVA_NAV_POP,
    NOVA_NAV_REPLACE,
    NOVA_NAV_RESET
}
nova_navigation_action_t;

nova_result_t
nova_navigation_initialize(void);

nova_result_t
nova_navigation_navigate(
    nova_view_id_t destination);

nova_result_t
nova_navigation_back(void);

const nova_view_t*
nova_navigation_current(void);
```

---

# 27. Diagnosedaten

Zu erfassen sind:

- aktuelle Ansicht
- Stacktiefe
- Navigation Events
- Fokuswechsel
- Dialogwechsel
- Recoverywechsel
- Navigationszeit
- Fehleranzahl

---

# 28. Testfälle

Zu testen sind:

- Vorwärtsnavigation
- Rücknavigation
- Dialognavigation
- modale Dialoge
- Recovery Navigation
- Navigation Reset
- Fokusübernahme
- Accessibility
- Stackwiederherstellung
- Fehlernavigation

---

# 29. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Navigation ausschließlich über das Navigation Model erfolgt.
- der Navigation Stack jederzeit konsistent bleibt.
- Vorwärts- und Rücknavigation deterministisch funktionieren.
- Dialoge einen eigenen Navigationskontext besitzen.
- Recovery-Ansichten korrekt integriert sind.
- keine Heap-Allokationen während der Navigation stattfinden.
- BIOS- und UEFI-Systeme identisches Navigationsverhalten erzeugen.

---

# 30. Referenzen

## ADRs

- ADR-BOOTNAV-0001 – Zustandsbasierte Navigation
- ADR-BOOTNAV-0002 – Navigation Stack
- ADR-BOOTSTATE-0001 – Globales Zustandsmodell
- ADR-BOOTINPUT-0005 – Eingabefokus und Fokusnavigation
- ADR-BOOTACCESS-0001 – Barrierefreie Navigation

## NPSPECs

- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTMOTION-0007 – Transition System
- NPSPEC-BOOTMOTION-0009 – Menu Navigation Transitions
- NPSPEC-BOOTMOTION-0010 – Focus and Selection Motion
- NPSPEC-BOOTDIALOG-0001 – Dialog System

---

# 31. Zusammenfassung

Das **Boot Navigation Model** definiert die vollständige Navigationsarchitektur der NovaOS Bootoberfläche. Es basiert auf einem zustandsorientierten Navigationsmodell mit einem deterministischen Navigation Stack, klar definierten Zustandsübergängen und einer zentralen Navigation Controller-Komponente. Dadurch entstehen reproduzierbare, sichere und konsistente Navigationsabläufe für BIOS- und UEFI-Systeme, einschließlich Recovery-, Dialog- und Fehlernavigation. Moderne UI-Frameworks verwenden ebenfalls einen zentral verwalteten Navigationszustand und einen Stack zur Steuerung von Vorwärts- und Rücknavigation. :contentReference[oaicite:3]{index=3}