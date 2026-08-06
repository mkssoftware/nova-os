# NPSPEC-BOOTINPUT-0010
# Keyboard Shortcuts

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTINPUT-0010 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Input System |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTINPUT-0002, NPSPEC-BOOTINPUT-0003, NPSPEC-BOOTINPUT-0008, NPSPEC-BOOTNAV-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Keyboard Shortcut System** der NovaOS-Bootoberfläche.

Das Shortcut-System stellt eine einheitliche Infrastruktur für Tastenkombinationen bereit. Es ermöglicht den schnellen Zugriff auf Funktionen des Boot Managers, der Recovery-Umgebung und der Diagnosewerkzeuge, ohne die Navigation über Menüs oder Maus zu erfordern.

Alle Tastenkombinationen werden zentral verwaltet und unabhängig von BIOS- oder UEFI-spezifischen Tastencodes verarbeitet.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Boot Manager
- Recovery
- Installer
- Diagnosemodule
- Einstellungsdialoge
- Kontextmenüs
- Overlays

---

# 3. Ziele

Das Shortcut-System muss:

- hardwareunabhängig arbeiten
- deterministisch sein
- zentral konfigurierbar sein
- kontextabhängige Shortcuts unterstützen
- globale Shortcuts unterstützen
- Accessibility integrieren

---

# 4. Architektur

```text
Keyboard

↓

Keyboard HAL

↓

Unified Input Events

↓

Shortcut Manager

↓

Command Resolver

↓

Application Action
```

---

# 5. Komponenten

Das Shortcut-System besteht aus:

- Shortcut Manager
- Shortcut Registry
- Context Resolver
- Command Dispatcher
- Conflict Resolver
- Accessibility Bridge

---

# 6. Grundprinzip

Ein Shortcut besteht aus:

- einer oder mehreren Tasten
- optionalen Modifiertasten
- einem Zielkontext
- einer auszuführenden Aktion

Ein Shortcut löst niemals direkt UI-Logik aus, sondern immer einen Befehl (Command).

---

# 7. Shortcut-Typen

Unterstützt werden:

### Globale Shortcuts

Im gesamten Boot Manager verfügbar.

### Kontextbezogene Shortcuts

Nur innerhalb eines bestimmten Focus Scope aktiv.

### Dialog-Shortcuts

Nur innerhalb eines geöffneten Dialogs aktiv.

### Entwickler-Shortcuts

Nur im Debug- oder Entwicklermodus verfügbar.

---

# 8. Tastenkombinationen

Unterstützt werden:

- Einzeltasten
- Modifier + Taste
- Mehrfachtastenkombinationen

Beispiele:

```text
F1

↓

Hilfe
```

```text
Ctrl + R

↓

Recovery starten
```

```text
Alt + Enter

↓

Details anzeigen
```

---

# 9. Standard-Shortcuts

Empfohlene Standardbelegung:

| Shortcut | Aktion |
|----------|--------|
| F1 | Hilfe |
| F5 | Recovery |
| F8 | Erweiterte Bootoptionen |
| F10 | Einstellungen |
| Esc | Zurück |
| Enter | Ausführen |
| Tab | Nächstes Control |
| Shift + Tab | Vorheriges Control |
| Ctrl + L | Protokolle öffnen |
| Ctrl + D | Diagnose starten |

Die tatsächliche Belegung erfolgt über die zentrale Konfiguration.

---

# 10. Shortcut-Auflösung

Die Auswertung erfolgt in folgender Reihenfolge:

1. Dialog-Shortcut
2. Kontext-Shortcut
3. Globaler Shortcut
4. Standardverhalten

Der spezifischste Shortcut besitzt immer Vorrang.

---

# 11. Konfliktbehandlung

Mehrdeutige Tastenkombinationen sind nicht zulässig.

Bei Konflikten gilt:

- höher priorisierter Kontext gewinnt
- gleichrangige Konflikte sind Konfigurationsfehler

Konflikte werden beim Systemstart validiert.

---

# 12. Focus-Integration

Shortcuts können:

- unabhängig vom Fokus arbeiten
- fokusabhängig arbeiten

Die Zuordnung erfolgt pro Shortcut.

---

# 13. Accessibility

Das Shortcut-System unterstützt:

- alternative Tastenkombinationen
- Sticky Keys
- Slow Keys
- Screen Reader Hinweise

Alle Shortcuts müssen ohne gleichzeitiges Drücken mehrerer Tasten nutzbar konfigurierbar sein.

---

# 14. Sicherheit

Das System verhindert:

- doppelte Shortcut-Registrierungen
- rekursive Shortcut-Aufrufe
- ungültige Tastenkombinationen
- nicht autorisierte Entwickler-Shortcuts
- Konflikte zwischen globalen und lokalen Shortcuts

Alle Shortcuts werden beim Registrieren validiert.

---

# 15. Speicherverwaltung

Das Shortcut-System verwendet:

- statische Shortcut-Tabellen
- feste Command-Referenzen
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 16. Performance

Anforderungen:

- O(1)-Lookup über Hash- oder Indexstruktur
- O(1)-Command-Auflösung
- deterministische Laufzeit

Die Anzahl registrierter Shortcuts darf die Eingabelatenz nicht beeinflussen.

---

# 17. API

```c
typedef struct
{
    uint32_t modifiers;
    nova_key_t key;
}
nova_shortcut_t;

typedef enum
{
    NOVA_SHORTCUT_GLOBAL,
    NOVA_SHORTCUT_CONTEXT,
    NOVA_SHORTCUT_DIALOG,
    NOVA_SHORTCUT_DEVELOPER
}
nova_shortcut_scope_t;

nova_result_t
nova_shortcut_register(
    const nova_shortcut_t* shortcut,
    nova_shortcut_scope_t scope,
    nova_command_id_t command);

nova_result_t
nova_shortcut_unregister(
    const nova_shortcut_t* shortcut);

bool
nova_shortcut_execute(
    const nova_keyboard_event_t* event);
```

---

# 18. Diagnosedaten

Zu erfassen sind:

- Shortcut
- Scope
- Zielbefehl
- Ausführungsanzahl
- Konflikte
- Fehlgeschlagene Aufrufe
- Ausführungszeit

---

# 19. Testfälle

Zu testen sind:

- Globale Shortcuts
- Kontext-Shortcuts
- Dialog-Shortcuts
- Entwickler-Shortcuts
- Konfliktbehandlung
- Focus-Integration
- Accessibility
- Layoutwechsel
- Fehlerfälle

---

# 20. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- globale und kontextabhängige Shortcuts korrekt unterstützt werden.
- Konflikte zuverlässig erkannt werden.
- Shortcut-Auflösung deterministisch erfolgt.
- Commands statt direkter UI-Logik ausgeführt werden.
- Accessibility vollständig integriert ist.
- keine dynamischen Speicherallokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 21. Referenzen

## ADRs

- ADR-BOOTINPUT-0010 – Keyboard Shortcuts
- ADR-BOOTINPUT-0003 – Keyboard Input
- ADR-BOOTNAV-0001 – Navigation Model
- ADR-BOOTACCESS-0001 – Accessibility

## NPSPECs

- NPSPEC-BOOTINPUT-0001 – Boot Input Architecture
- NPSPEC-BOOTINPUT-0002 – Unified Input Event Model
- NPSPEC-BOOTINPUT-0003 – Keyboard Input
- NPSPEC-BOOTINPUT-0008 – Focus Management
- NPSPEC-BOOTNAV-0001 – Boot Navigation Model
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 22. Zusammenfassung

Das **Keyboard Shortcut System** definiert die standardisierte Verarbeitung von Tastenkombinationen innerhalb der NovaOS-Bootoberfläche. Es unterstützt globale, kontextbezogene, dialogabhängige und entwicklerspezifische Shortcuts, löst diese deterministisch über eine zentrale Command-Infrastruktur auf und integriert sich vollständig in das Fokus-, Eingabe- und Accessibility-System. Durch zentrale Verwaltung, Konfliktprüfung und statische Speicherstrukturen entsteht ein leistungsfähiges und konsistentes Shortcut-System für BIOS- und UEFI-basierte Bootumgebungen.