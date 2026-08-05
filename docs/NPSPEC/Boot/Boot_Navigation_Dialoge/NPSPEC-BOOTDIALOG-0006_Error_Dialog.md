# NPSPEC-BOOTDIALOG-0006
# Error Dialog

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTDIALOG-0006 |
| Titel | Error Dialog |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Dialog System |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTDIALOG-0001 bis NPSPEC-BOOTDIALOG-0005, NPSPEC-BOOTNAV-0004 |
| Zugehörige ADRs | ADR-BOOTDIALOG-0006, ADR-BOOTSAFE-0003, ADR-BOOTDIAG-0001, ADR-BOOTACCESS-0001, ADR-BOOTMOTION-0008 |

---

# 1. Ziel

Diese Spezifikation definiert den **Error Dialog** der NovaOS Boot UI.

Der Error Dialog informiert den Benutzer über Fehlerzustände, die den Bootvorgang oder eine Systemfunktion beeinträchtigen.

Das System unterstützt:

- Bootfehler
- Dateisystemfehler
- Hardwarefehler
- Speicherfehler
- Recovery-Fehler
- Konfigurationsfehler
- Sicherheitsfehler
- interne Systemfehler

Der Dialog stellt verständliche Informationen bereit und bietet geeignete Handlungsmöglichkeiten.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Fehlerdialoge.

Sie umfasst:

- Boot Manager
- Recovery
- Diagnose
- Backup
- Wiederherstellung
- Verschlüsselung
- Systemkonfiguration

Nicht Bestandteil sind:

- Warnungen
- Informationsdialoge
- Progressdialoge

---

# 3. Architektur

```text
Application

↓

Error Detection

↓

Dialog Manager

↓

Error Dialog

↓

User Action

↓

Application
```

---

# 4. Grundprinzip

Ein Error Dialog wird angezeigt, wenn ein Fehler erkannt wurde, der:

- den aktuellen Vorgang verhindert,
- eine Benutzerentscheidung erfordert,
- oder diagnostische Informationen bereitstellen soll.

Der Dialog beschreibt den Fehler verständlich und trennt Benutzerinformationen von technischen Details.

---

# 5. Aufgaben

Der Error Dialog übernimmt:

- Fehleranzeige
- Fehlerklassifizierung
- Benutzerinformation
- Handlungsoptionen
- Diagnoseanzeige
- Fehlercodeausgabe
- Ergebnisverwaltung
- Protokollierung

---

# 6. Dialogaufbau

Der Dialog besteht aus:

```text
Fehlersymbol

↓

Titel

↓

Fehlerbeschreibung

↓

Empfohlene Maßnahme

↓

Optionale technische Details

↓

Schaltflächen
```

---

# 7. Fehlerklassen

Unterstützt werden:

| Klasse | Beschreibung |
|---|---|
| Information | nicht kritischer Fehler |
| Recoverable | Fehler kann behoben werden |
| Critical | Vorgang kann nicht fortgesetzt werden |
| Fatal | Bootvorgang muss beendet oder gewechselt werden |

---

# 8. Schaltflächen

Je nach Fehler werden unterstützt:

- OK
- Retry
- Ignore
- Continue
- Recovery
- Shutdown
- Restart
- Diagnostics

Nicht jede Kombination ist zulässig.

---

# 9. Fehlercode

Jeder Error Dialog besitzt einen eindeutigen Fehlercode.

Beispiel:

```text
BOOT-000123
```

Der Fehlercode dient ausschließlich der Diagnose und dem Support.

---

# 10. Technische Details

Optional können angezeigt werden:

- Fehlercode
- Modul
- Dateisystem
- Laufwerk
- Geräte-ID
- Zeitstempel
- Zusatzinformationen

Technische Informationen sind standardmäßig eingeklappt.

---

# 11. Empfohlene Maßnahmen

Der Dialog kann konkrete Maßnahmen empfehlen.

Beispiele:

- erneut versuchen
- Recovery starten
- Datenträger prüfen
- Neustart durchführen
- Diagnose öffnen

Die Empfehlungen richten sich nach der Fehlerklasse.

---

# 12. Navigation

Bei kritischen Fehlern:

- wird die Hintergrundnavigation blockiert.

Bei nicht kritischen Fehlern kann der Dialog optional nichtmodal dargestellt werden.

---

# 13. Benutzerentscheidung

Mögliche Ergebnisse:

```text
OK

Retry

Ignore

Recovery

Restart

Shutdown

Diagnostics
```

Genau ein Ergebnis wird zurückgegeben.

---

# 14. Fokus

Beim Öffnen erhält die empfohlene Standardaktion den Fokus.

Bei kritischen Fehlern wird niemals eine potenziell gefährliche Aktion als Standard gewählt.

---

# 15. Darstellung

Der Error Dialog verwendet:

- Glass Dialog
- Fehlersymbol
- Modal Layer (standardmäßig)
- Backdrop

Die Gestaltung richtet sich nach der Fehlerklasse.

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

Bei Reduced Motion:

```text
Fade
```

---

# 17. Accessibility

Unterstützt werden:

- Screenreader
- High Contrast
- Reduced Motion
- skalierbare Schriftgrößen
- vollständige Tastaturbedienung

Fehlertitel und empfohlene Maßnahme müssen eindeutig ausgegeben werden.

---

# 18. Speicherverwaltung

Error Dialoge verwenden:

- statische Dialogobjekte
- feste Symboltabellen
- feste Layoutstrukturen

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Erzeugung
- O(1)-Schließen
- deterministische Laufzeit
- keine Heap-Allokationen

---

# 20. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Fehlerklasse
- fehlender Fehlercode
- beschädigte Dialogdaten
- fehlendes Symbol
- Nullpointer

Im Fehlerfall wird ein generischer Error Dialog angezeigt.

---

# 21. Sicherheit

Das System verhindert:

- unvollständige Fehlermeldungen
- automatische Fortsetzung kritischer Fehler
- Fokusverlust
- Navigation außerhalb modaler Fehlerdialoge
- inkonsistente Fehlerzustände

Bei fatalen Fehlern darf keine Fortsetzung des betroffenen Vorgangs angeboten werden.

---

# 22. API

```c
typedef enum
{
    NOVA_ERROR_INFORMATION,
    NOVA_ERROR_RECOVERABLE,
    NOVA_ERROR_CRITICAL,
    NOVA_ERROR_FATAL
}
nova_error_level_t;

nova_result_t
nova_error_dialog_show(
    nova_error_level_t level,
    const char* error_code,
    const char* title,
    const char* message);

nova_dialog_result_t
nova_error_dialog_result(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Fehlerklasse
- Fehlercode
- Benutzerentscheidung
- Öffnungszeit
- Schließzeit
- Modul
- Speicherverbrauch
- Fehleranzahl

---

# 24. Testfälle

Zu testen sind:

- Information Error
- Recoverable Error
- Critical Error
- Fatal Error
- Retry
- Recovery
- Diagnostics
- Accessibility
- Reduced Motion
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- jede Fehlerklasse korrekt dargestellt wird.
- Fehlercodes eindeutig angezeigt werden.
- empfohlene Maßnahmen passend zur Fehlerklasse angeboten werden.
- kritische Fehler standardmäßig modal dargestellt werden.
- Accessibility vollständig unterstützt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTDIALOG-0006 – Einheitliche Fehlerdialoge
- ADR-BOOTSAFE-0003 – Fehlerbehandlung im Bootmanager
- ADR-BOOTDIAG-0001 – Diagnoseschnittstellen
- ADR-BOOTACCESS-0001 – Barrierefreie Navigation
- ADR-BOOTMOTION-0008 – Dialog Enter and Exit Motion

## NPSPECs

- NPSPEC-BOOTDIALOG-0001 – Boot Dialog Architecture
- NPSPEC-BOOTDIALOG-0002 – Modal Dialog
- NPSPEC-BOOTDIALOG-0003 – Glass Dialog
- NPSPEC-BOOTNAV-0004 – Back Navigation
- NPSPEC-BOOTMOTION-0008 – Dialog Enter and Exit Motion
- NPSPEC-BOOTDIAG-0001 – Boot Diagnostics Framework

---

# 27. Zusammenfassung

Der **Error Dialog** definiert die standardisierte Darstellung sämtlicher Fehlerzustände innerhalb der NovaOS Bootoberfläche. Durch eine klare Trennung zwischen benutzerverständlichen Informationen und technischen Diagnosedaten, eindeutige Fehlerklassen, sichere Standardaktionen und vollständige Integration in Navigation, Motion und Accessibility entsteht ein robustes Fehlermanagementsystem für BIOS- und UEFI-basierte Bootumgebungen. Kritische und fatale Fehler werden kontrolliert behandelt und ermöglichen dem Benutzer eine sichere Entscheidung über das weitere Vorgehen.