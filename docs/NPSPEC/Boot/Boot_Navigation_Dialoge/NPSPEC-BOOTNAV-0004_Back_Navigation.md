# NPSPEC-BOOTNAV-0004
# Back Navigation

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTNAV-0004 |
| Titel | Back Navigation |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Navigation |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTNAV-0001 bis NPSPEC-BOOTNAV-0003 |
| Zugehörige ADRs | ADR-BOOTNAV-0002, ADR-BOOTNAV-0004, ADR-BOOTSTATE-0001, ADR-BOOTINPUT-0005, ADR-BOOTSAFE-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Back Navigation System** der NovaOS Boot UI.

Das System beschreibt sämtliche Rückwärtsnavigationen innerhalb des Bootmanagers.

Es unterstützt:

- Zurück zur vorherigen Ansicht
- Schließen von Dialogen
- Verlassen von Untermenüs
- Wiederherstellung des vorherigen Zustands
- Wiederherstellung des Fokus
- Wiederherstellung der Scrollposition
- Rückkehr aus Recovery-Modi

Die Back Navigation basiert vollständig auf dem Navigation Stack.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Bootoberflächen.

Sie umfasst:

- Bootmenü
- Einstellungen
- Recovery
- Diagnose
- Passwortdialoge
- Backup
- Wiederherstellung
- Fehlerdialoge

Nicht Bestandteil sind:

- Rendering
- Animationen
- Layout

---

# 3. Architektur

```text
Back Event

↓

Navigation Controller

↓

Navigation Stack

↓

Previous View

↓

Layout Engine

↓

Rendering
```

---

# 4. Grundprinzip

Eine Rücknavigation entfernt grundsätzlich den obersten Navigationseintrag.

Anschließend wird der vorherige Eintrag vollständig wiederhergestellt.

Die Wiederherstellung umfasst:

- Ansicht
- Fokus
- Scrollposition
- Navigationskontext
- temporären Zustand

---

# 5. Aufgaben

Das Back-System übernimmt:

- Pop
- Fokuswiederherstellung
- Kontextwiederherstellung
- Dialogschließung
- Recovery-Rückkehr
- Stackvalidierung
- Zustandsprüfung

---

# 6. Navigation

```text
Root

↓

Boot Menu

↓

Settings

↓

Appearance
```

Back ergibt:

```text
Appearance

↓

Settings
```

---

# 7. Dialoge

Ist ein Dialog geöffnet:

```text
Dialog

↓

Back

↓

Dialog schließen
```

Die darunterliegende Ansicht bleibt unverändert erhalten.

Dialoge besitzen Vorrang vor normalen Seiten.

---

# 8. Recovery

Innerhalb des Recovery-Zweiges erfolgt Back ausschließlich innerhalb des Recovery-Stacks.

Erst nach Verlassen des Recovery-Stacks erfolgt die Rückkehr zum Hauptmenü.

---

# 9. Fokus

Nach jeder Rücknavigation wird wiederhergestellt:

- letzter Fokus
- zuletzt ausgewähltes Element
- Scrollposition

Der Benutzer kehrt exakt an die vorherige Position zurück.

---

# 10. Zustandswiederherstellung

Folgende Informationen werden übernommen:

- Parameter
- temporäre Werte
- Eingaben
- Auswahl
- Navigationskontext

Es erfolgt keine Neuinitialisierung der View.

---

# 11. Root View

Die Root View besitzt keine Rücknavigation.

Ein Back-Ereignis auf Root führt standardmäßig zu:

- keiner Aktion
- oder einem konfigurierbaren Verhalten

---

# 12. Modale Dialoge

Bei geöffnetem modalen Dialog:

- wird ausschließlich der Dialog geschlossen.
- darunterliegende Ansichten bleiben unverändert.

---

# 13. Replace Navigation

Wurde eine Ansicht durch **Replace** geöffnet, existiert kein zusätzlicher Stackeintrag.

Back führt daher direkt zur vorherigen verbleibenden Ansicht.

---

# 14. Navigation History

Die History bleibt nach einem Back konsistent.

Entfernte Einträge werden vollständig freigegeben.

---

# 15. Navigation Events

Folgende Ereignisse lösen Back aus:

- Escape
- Zurück-Schaltfläche
- Mausaktion
- Touch-Geste
- API
- automatischer Dialogabschluss

Alle Ereignisse werden zentral verarbeitet.

---

# 16. Transition

Der Ablauf:

```text
Back

↓

Leave Animation

↓

Pop

↓

Restore State

↓

Enter Animation
```

Die Transition erfolgt atomar.

---

# 17. Accessibility

Back funktioniert unabhängig vom Eingabegerät.

Unterstützt werden:

- Tastatur
- Maus
- Touch
- Screenreader
- alternative Eingabegeräte

---

# 18. Layoutintegration

Nach einem Back erfolgen:

- Layout Invalidierung
- Focus Update
- Render Invalidierung

Nur die wieder aktivierte Ansicht wird aktualisiert.

---

# 19. Speicherverwaltung

Back verwendet ausschließlich:

- Navigation Stack
- feste Objektpools
- statische Listen

Heap-Allokationen sind verboten.

---

# 20. Performance

Anforderungen:

- O(1)-Pop
- deterministische Laufzeit
- keine rekursiven Navigationen
- keine Heap-Allokationen

---

# 21. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- leerer Stack
- beschädigter Stack
- ungültige Zielansicht
- fehlender Fokus
- Nullpointer

Im Fehlerfall erfolgt automatisch die Rückkehr zur Root View.

---

# 22. Sicherheit

Das System verhindert:

- Back unter Root
- doppelte Pop-Operationen
- rekursive Rücknavigation
- inkonsistente Navigation
- beschädigte Zustandswiederherstellung

---

# 23. API

```c
nova_result_t
nova_navigation_back(void);

bool
nova_navigation_can_go_back(void);

const nova_navigation_entry_t*
nova_navigation_previous(void);

nova_result_t
nova_navigation_restore_state(
    const nova_navigation_entry_t* entry);
```

---

# 24. Diagnosedaten

Zu erfassen sind:

- Back-Operationen
- Stacktiefe
- Fokuswiederherstellungen
- Dialogschließungen
- Recovery-Rückkehr
- Navigationsdauer
- Speicherverbrauch
- Fehleranzahl

---

# 25. Testfälle

Zu testen sind:

- Back aus Untermenü
- Back aus Dialog
- Back aus Recovery
- Root Navigation
- Fokuswiederherstellung
- Scrollposition
- Replace Navigation
- Accessibility
- Recovery Stack
- Fehlerfälle

---

# 26. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Rücknavigationen ausschließlich über den Navigation Stack erfolgen.
- Fokus und Scrollposition korrekt wiederhergestellt werden.
- Dialoge Vorrang vor Seiten besitzen.
- Root Views keine ungültige Rücknavigation erlauben.
- Recovery Navigation korrekt funktioniert.
- keine Heap-Allokationen während der Navigation stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 27. Referenzen

## ADRs

- ADR-BOOTNAV-0002 – Navigation Stack
- ADR-BOOTNAV-0004 – Einheitliches Back-Verhalten
- ADR-BOOTSTATE-0001 – Globales Zustandsmodell
- ADR-BOOTINPUT-0005 – Eingabefokus und Fokusnavigation
- ADR-BOOTSAFE-0001 – Sicherer Rückfallzustand

## NPSPECs

- NPSPEC-BOOTNAV-0001 – Boot Navigation Model
- NPSPEC-BOOTNAV-0002 – Navigation Stack
- NPSPEC-BOOTNAV-0003 – Page and View Model
- NPSPEC-BOOTDIALOG-0001 – Dialog System
- NPSPEC-BOOTMOTION-0007 – Transition System

---

# 28. Zusammenfassung

Das **Back Navigation System** definiert sämtliche Rückwärtsnavigationen innerhalb der NovaOS Bootoberfläche. Es basiert vollständig auf dem Navigation Stack und stellt sicher, dass Ansichten einschließlich Fokus, Scrollposition und Kontext deterministisch wiederhergestellt werden. Dialoge besitzen Vorrang vor normalen Seiten, Recovery-Bereiche verwenden eigene Navigationszweige, und sämtliche Operationen erfolgen mit statischer Speicherverwaltung und O(1)-Stackzugriffen. Dadurch entsteht ein konsistentes und reproduzierbares Navigationsverhalten auf BIOS- und UEFI-Systemen.