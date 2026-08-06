# NPSPEC-BOOTINPUT-0008
# Focus Management

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTINPUT-0008 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Input System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTINPUT-0002, NPSPEC-BOOTINPUT-0006, NPSPEC-BOOTNAV-0001, NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Focus Management System** der NovaOS Boot UI.

Das Focus Management verwaltet den aktiven Eingabefokus innerhalb der Bootoberfläche und stellt sicher, dass Tastatur-, Controller- und andere fokusbasierte Eingaben jederzeit deterministisch an genau ein Control weitergeleitet werden.

Das System bildet die Grundlage für Navigation, Accessibility und Benutzerinteraktion.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Boot Manager
- Recovery
- Installer
- Diagnosemodule
- sämtliche UI-Controls
- Dialoge
- Overlays

---

# 3. Ziele

Das Focus-System muss:

- genau einen aktiven Fokus besitzen
- deterministisch arbeiten
- Navigation unterstützen
- Accessibility integrieren
- Dialoge berücksichtigen
- hardwareunabhängig sein

---

# 4. Architektur

```text
Input System

↓

Focus Manager

├── Focus Tree

├── Focus Scope

├── Navigation Engine

└── Dispatcher

↓

Focused Control
```

---

# 5. Komponenten

Das Focus-System besteht aus:

- Focus Manager
- Focus Registry
- Focus Scope Manager
- Navigation Resolver
- Default Focus Resolver
- Accessibility Bridge

---

# 6. Grundprinzip

Zu jedem Zeitpunkt besitzt das System genau **ein aktives fokussiertes Control** innerhalb des aktuellen Focus Scope.

Alle fokusbasierten Eingaben werden ausschließlich an dieses Control weitergeleitet.

Pointer-Ereignisse folgen weiterhin dem Hit Testing bzw. Pointer Capture.

---

# 7. Fokuszustände

Unterstützt werden:

```text
Unfocused

Focused

Focus Visible

Disabled

Hidden
```

Optional:

```text
Pending Focus
```

---

# 8. Focus Scope

Ein Focus Scope definiert einen eigenständigen Fokusbereich.

Beispiele:

- Hauptfenster
- Dialog
- Kontextmenü
- Overlay

Innerhalb eines Focus Scope existiert immer nur ein aktiver Fokus.

---

# 9. Fokuswechsel

Ein Fokuswechsel erfolgt durch:

- Tastatur
- Controller
- Maus
- Touch
- Programmsteuerung
- Dialogwechsel

Der Wechsel erfolgt atomar.

---

# 10. Default Focus

Jeder Focus Scope besitzt genau ein Standard-Control.

Beim Öffnen eines neuen Focus Scope erhält dieses automatisch den Fokus.

Existiert kein Standard-Control, wird das erste fokussierbare Element verwendet.

---

# 11. Fokusnavigation

Unterstützt werden:

- Vorwärts
- Rückwärts
- Links
- Rechts
- Oben
- Unten

Die Navigation erfolgt über das Navigation Framework.

---

# 12. Fokusregeln

Ein Control kann nur fokussiert werden, wenn:

- sichtbar
- aktiviert
- fokussierbar
- innerhalb des aktiven Focus Scope

Nicht sichtbare oder deaktivierte Controls dürfen niemals Fokus erhalten.

---

# 13. Fokusreihenfolge

Die Reihenfolge basiert standardmäßig auf:

1. expliziten Navigationsbeziehungen
2. Layout-Reihenfolge
3. Dokumentreihenfolge

Die Reihenfolge muss deterministisch sein.

---

# 14. Fokusdarstellung

Die Darstellung erfolgt ausschließlich über das Theme.

Unterstützt werden:

- Focus Ring
- Glow
- Rahmen
- Skalierung
- Animation

Alle Effekte werden durch Styles definiert.

---

# 15. Dialoge

Beim Öffnen eines Dialogs:

```text
Alter Fokus

↓

gespeichert

↓

Dialog-Fokus

↓

Dialog schließen

↓

Alter Fokus wiederherstellen
```

Dialoge besitzen einen eigenen Focus Scope.

---

# 16. Accessibility

Das Focus-System unterstützt:

- Screen Reader
- Focus Announcement
- Focus Highlight
- vereinfachte Navigation
- alternative Fokusreihenfolgen

Jeder Fokuswechsel erzeugt ein Accessibility-Ereignis.

---

# 17. Sicherheit

Das System verhindert:

- mehrere aktive Fokusse
- Fokus auf zerstörte Controls
- rekursive Fokuswechsel
- Fokus außerhalb eines Focus Scope
- ungültige Fokusreferenzen

Alle Fokusänderungen werden validiert.

---

# 18. Speicherverwaltung

Das Focus-System verwendet:

- statische Focus-Tabellen
- Objektpools
- feste Focus Scopes

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Fokuswechsel
- O(1)-Lookup
- O(1)-Navigation
- deterministische Laufzeit

Die Laufzeit darf unabhängig von der Anzahl der Controls konstant bleiben.

---

# 20. API

```c
typedef struct nova_focus_scope nova_focus_scope_t;

nova_result_t
nova_focus_initialize(void);

nova_result_t
nova_focus_set(
    nova_control_t* control);

nova_control_t*
nova_focus_get(void);

nova_result_t
nova_focus_next(void);

nova_result_t
nova_focus_previous(void);

nova_result_t
nova_focus_enter_scope(
    nova_focus_scope_t* scope);

nova_result_t
nova_focus_leave_scope(void);
```

---

# 21. Diagnosedaten

Zu erfassen sind:

- aktueller Fokus
- Focus Scope
- Fokuswechsel
- Navigationsrichtung
- Wiederherstellungen
- Fehler
- Latenz

---

# 22. Testfälle

Zu testen sind:

- Fokuswechsel
- Dialoge
- Overlays
- Focus Scope
- Tastatur
- Controller
- Accessibility
- Wiederherstellung
- Navigation
- Fehlerfälle

---

# 23. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- zu jedem Zeitpunkt genau ein aktives fokussiertes Control existiert.
- Focus Scopes korrekt verwaltet werden.
- Dialoge den vorherigen Fokus wiederherstellen.
- ausschließlich fokussierbare Controls Fokus erhalten.
- Accessibility vollständig integriert ist.
- Fokuswechsel deterministisch erfolgen.
- keine dynamischen Speicherallokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 24. Referenzen

## ADRs

- ADR-BOOTINPUT-0008 – Focus Management
- ADR-BOOTNAV-0001 – Navigation Model
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0001 – Performance Budget

## NPSPECs

- NPSPEC-BOOTINPUT-0001 – Boot Input Architecture
- NPSPEC-BOOTINPUT-0002 – Unified Input Event Model
- NPSPEC-BOOTINPUT-0006 – Pointer Model
- NPSPEC-BOOTNAV-0001 – Boot Navigation Model
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 25. Zusammenfassung

Das **Focus Management System** definiert die zentrale Verwaltung des Eingabefokus innerhalb der NovaOS-Bootoberfläche. Es stellt sicher, dass zu jedem Zeitpunkt genau ein aktives Control innerhalb des aktuellen Focus Scope existiert, verwaltet deterministische Fokuswechsel, unterstützt Dialoge, Navigation und Accessibility und trennt fokusbasierte Eingaben konsequent von Pointer-Ereignissen. Durch feste Datenstrukturen, klare Fokusregeln und hardwareunabhängige Verarbeitung bildet es die Grundlage für eine konsistente Benutzerinteraktion auf BIOS- und UEFI-basierten Systemen.