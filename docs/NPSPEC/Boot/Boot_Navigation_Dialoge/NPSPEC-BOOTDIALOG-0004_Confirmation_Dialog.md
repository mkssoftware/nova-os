# NPSPEC-BOOTDIALOG-0004
# Confirmation Dialog

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTDIALOG-0004 |
| Titel | Confirmation Dialog |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Dialog System |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTDIALOG-0001 bis NPSPEC-BOOTDIALOG-0003, NPSPEC-BOOTNAV-0004 |
| Zugehörige ADRs | ADR-BOOTDIALOG-0004, ADR-BOOTSAFE-0002, ADR-BOOTACCESS-0001, ADR-BOOTMOTION-0008, ADR-BOOTINPUT-0006 |

---

# 1. Ziel

Diese Spezifikation definiert den **Confirmation Dialog** der NovaOS Boot UI.

Der Confirmation Dialog dient zur Bestätigung sicherheitskritischer oder irreversibler Aktionen.

Das System unterstützt:

- Bestätigungen
- Warnungen
- Löschvorgänge
- Formatierungen
- Wiederherstellungen
- Neustarts
- Herunterfahren
- sicherheitsrelevante Aktionen

Kein kritischer Vorgang darf ohne ausdrückliche Bestätigung des Benutzers ausgeführt werden.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Bestätigungsdialoge.

Sie umfasst:

- Boot Manager
- Recovery
- Diagnose
- Backup
- Wiederherstellung
- Datenträgerverwaltung
- Systemeinstellungen

Nicht Bestandteil sind:

- Informationsdialoge
- Progressdialoge
- Benachrichtigungen

---

# 3. Architektur

```text
Application

↓

Confirmation Request

↓

Dialog Manager

↓

Confirmation Dialog

↓

User Decision

↓

Application
```

---

# 4. Grundprinzip

Ein Confirmation Dialog wird immer vor einer Aktion angezeigt, deren Ausführung:

- Daten verändern kann
- nicht rückgängig gemacht werden kann
- Sicherheitsauswirkungen besitzt
- den Bootvorgang beeinflusst

Ohne Benutzerentscheidung erfolgt keine Ausführung.

---

# 5. Aufgaben

Der Confirmation Dialog übernimmt:

- Benutzerbestätigung
- Warnhinweise
- Entscheidungsverwaltung
- Ergebnisrückgabe
- Fokussteuerung
- Navigation Blockierung
- Sicherheitsprüfung
- Dialogvalidierung

---

# 6. Dialogaufbau

Der Dialog besteht aus:

```text
Titel

↓

Beschreibung

↓

Warnhinweis

↓

Optionale Zusatzinformationen

↓

Schaltflächen
```

Alle Elemente werden automatisch an den Inhalt angepasst.

---

# 7. Standard-Schaltflächen

Unterstützt werden:

- OK
- Cancel
- Yes
- No
- Continue
- Abort
- Retry
- Ignore

Standardmäßig erhält die sichere Option den Eingabefokus.

---

# 8. Standardverhalten

Der Dialog:

- blockiert die Navigation
- blockiert Hintergrundeingaben
- besitzt einen Modal Layer
- wartet auf eine eindeutige Benutzerentscheidung

---

# 9. Warnstufen

Unterstützte Warnstufen:

| Stufe | Bedeutung |
|---|---|
| Information | normale Bestätigung |
| Warning | mögliche Auswirkungen |
| Critical | irreversible Aktion |
| Security | sicherheitskritische Aktion |

Die Warnstufe beeinflusst Symbolik, Farben und Standardaktion.

---

# 10. Standardaktion

Die Standardaktion richtet sich nach dem Risiko.

Beispiele:

| Aktion | Standard |
|---|---|
| Informationen | OK |
| Warnung | Cancel |
| Formatierung | Cancel |
| Datenträger löschen | Cancel |
| Wiederherstellung | Continue |

Die sichere Option besitzt stets den Standardfokus.

---

# 11. Sicherheitsbestätigung

Bei besonders kritischen Aktionen können zusätzliche Bestätigungen erforderlich sein.

Beispiele:

- erneute Bestätigung
- Eingabe eines Kennworts
- Eingabe eines Bestätigungstextes

Die Anforderungen werden durch die aufrufende Anwendung definiert.

---

# 12. Dialogergebnis

Mögliche Rückgabewerte:

```text
OK

Cancel

Yes

No

Continue

Abort

Retry

Ignore
```

Es wird genau ein Ergebnis zurückgegeben.

---

# 13. Navigation

Während der Dialog aktiv ist:

- keine Seitennavigation
- keine Recovery-Navigation
- keine Hintergrundinteraktion

Back schließt den Dialog nur dann, wenn dies explizit erlaubt ist.

---

# 14. Fokus

Beim Öffnen:

- erhält die sichere Standardaktion den Fokus.

Tabulator- und Pfeiltasten bleiben innerhalb des Dialogs eingeschlossen.

---

# 15. Darstellung

Der Confirmation Dialog verwendet standardmäßig:

- Glass Dialog
- Modal Layer
- Backdrop
- Warnsymbol
- zentrierte Darstellung

Die Größe richtet sich nach Inhalt und Safe Display Area.

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

Der Dialog unterstützt:

- Screenreader
- vollständige Tastatursteuerung
- High Contrast
- Reduced Motion
- skalierbare Schriftgrößen

Warnstufe und Dialogtitel müssen vollständig vorgelesen werden können.

---

# 18. Speicherverwaltung

Confirmation Dialoge verwenden:

- statische Dialogobjekte
- feste Buttonlisten
- feste Layoutobjekte

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Öffnen
- O(1)-Schließen
- deterministische Laufzeit
- keine Heap-Allokationen

---

# 20. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- fehlende Standardaktion
- ungültige Warnstufe
- fehlende Schaltflächen
- beschädigter Fokus
- Nullpointer

Im Fehlerfall wird der Dialog sicher beendet und die Aktion abgebrochen.

---

# 21. Sicherheit

Das System verhindert:

- automatische Bestätigungen
- fehlenden Benutzerentscheid
- versehentliche Standardauswahl kritischer Aktionen
- Fokusverlust
- Navigation außerhalb des Dialogs

Kritische Aktionen dürfen niemals ohne bestätigte Benutzerentscheidung ausgeführt werden.

---

# 22. API

```c
typedef enum
{
    NOVA_CONFIRM_INFORMATION,
    NOVA_CONFIRM_WARNING,
    NOVA_CONFIRM_CRITICAL,
    NOVA_CONFIRM_SECURITY
}
nova_confirmation_level_t;

nova_result_t
nova_confirmation_dialog_show(
    nova_confirmation_level_t level,
    const char* title,
    const char* message);

nova_dialog_result_t
nova_confirmation_dialog_result(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Warnstufe
- Dialogtitel
- Benutzerentscheidung
- Öffnungszeit
- Schließzeit
- Fokuswechsel
- Speicherverbrauch
- Fehleranzahl

---

# 24. Testfälle

Zu testen sind:

- Informationsbestätigung
- Warnung
- kritische Aktion
- Sicherheitsdialog
- Passwortbestätigung
- Back Navigation
- High Contrast
- Reduced Motion
- Accessibility
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- jede kritische Aktion eine Benutzerbestätigung erfordert.
- die sichere Option standardmäßig fokussiert wird.
- modale Sperre während der gesamten Laufzeit aktiv bleibt.
- genau ein gültiges Dialogergebnis zurückgegeben wird.
- Accessibility vollständig unterstützt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTDIALOG-0004 – Bestätigungsdialoge
- ADR-BOOTSAFE-0002 – Sichere Bestätigungsabläufe
- ADR-BOOTACCESS-0001 – Barrierefreie Navigation
- ADR-BOOTMOTION-0008 – Dialog Enter and Exit Motion
- ADR-BOOTINPUT-0006 – Fokusverwaltung für Dialoge

## NPSPECs

- NPSPEC-BOOTDIALOG-0001 – Boot Dialog Architecture
- NPSPEC-BOOTDIALOG-0002 – Modal Dialog
- NPSPEC-BOOTDIALOG-0003 – Glass Dialog
- NPSPEC-BOOTNAV-0004 – Back Navigation
- NPSPEC-BOOTMOTION-0008 – Dialog Enter and Exit Motion
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 27. Zusammenfassung

Der **Confirmation Dialog** stellt sicher, dass sämtliche sicherheitskritischen oder irreversiblen Aktionen innerhalb der NovaOS Bootoberfläche erst nach einer eindeutigen Benutzerbestätigung ausgeführt werden. Durch modales Verhalten, sichere Standardauswahl, klare Warnstufen und vollständige Integration in Navigation, Motion und Accessibility entsteht ein konsistentes und fehlertolerantes Bestätigungssystem für BIOS- und UEFI-basierte Bootumgebungen.