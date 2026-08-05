# NPSPEC-BOOTDIALOG-0011
# Dialog Result and Cancellation

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTDIALOG-0011 |
| Titel | Dialog Result and Cancellation |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Dialog System |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTDIALOG-0001 bis NPSPEC-BOOTDIALOG-0010, NPSPEC-BOOTNAV-0004 |
| Zugehörige ADRs | ADR-BOOTDIALOG-0011, ADR-BOOTNAV-0004, ADR-BOOTSAFE-0002, ADR-BOOTSTATE-0001, ADR-BOOTINPUT-0006 |

---

# 1. Ziel

Diese Spezifikation definiert die Verarbeitung von **Dialogergebnissen (Results)** und **Abbrüchen (Cancellation)** innerhalb der NovaOS Boot UI.

Sie stellt sicher, dass jeder Dialog genau ein eindeutiges Ergebnis liefert oder kontrolliert abgebrochen wird.

Das System unterstützt:

- eindeutige Dialogergebnisse
- Benutzerabbruch
- Systemabbruch
- Timeout-Abbruch
- Wiederherstellung des Dialogkontexts
- Ergebnisvalidierung
- Fehlerbehandlung
- Rückgabe an den Aufrufer

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Dialoge.

Sie umfasst:

- Informationsdialoge
- Bestätigungsdialoge
- Warnungsdialoge
- Fehlerdialoge
- Fortschrittsdialoge
- Credential Dialoge
- Recovery Dialoge

Nicht Bestandteil sind:

- Seitennavigation
- Hintergrundprozesse
- Rendering

---

# 3. Architektur

```text
Application

↓

Dialog Manager

↓

Dialog

↓

User/System Event

↓

Result Manager

↓

Application
```

---

# 4. Grundprinzip

Jeder Dialog endet genau einmal.

Das Ende erfolgt durch:

- Benutzerentscheidung
- Systemabbruch
- Timeout
- Fehler
- erfolgreiche Ausführung

Nach Abschluss wird genau ein Resultat an den Aufrufer zurückgegeben.

---

# 5. Aufgaben

Das Result-System übernimmt:

- Ergebnisverwaltung
- Abbruchverwaltung
- Ergebnisvalidierung
- Rückgabe
- Statusänderungen
- Kontextbereinigung
- Ressourcenfreigabe
- Diagnose

---

# 6. Dialogzustände

```text
Created

↓

Opening

↓

Active

↓

Closing

↓

Completed
```

Während **Completed** darf kein weiteres Ergebnis erzeugt werden.

---

# 7. Standardergebnisse

Unterstützt werden:

```text
OK

Cancel

Yes

No

Retry

Ignore

Continue

Abort

Close

Timeout

Error
```

Eigene Ergebnisse können ergänzt werden.

---

# 8. Benutzerabbruch

Ein Benutzerabbruch kann ausgelöst werden durch:

- Cancel
- Escape
- Back
- Close

Ob ein Abbruch zulässig ist, entscheidet der Dialogtyp.

---

# 9. Systemabbruch

Ein Dialog kann beendet werden durch:

- Shutdown
- Neustart
- Recoverywechsel
- schwerwiegenden Fehler
- Wechsel des Bootzustands

Der Aufrufer erhält das Ergebnis:

```text
Abort
```

---

# 10. Timeout

Optional können Dialoge automatisch beendet werden.

Mögliche Ergebnisse:

```text
Timeout
```

Timeouts sind standardmäßig deaktiviert.

---

# 11. Ergebnisvalidierung

Vor der Rückgabe wird geprüft:

- genau ein Ergebnis vorhanden
- gültiger Ergebniswert
- vollständiger Abschluss
- gültiger Dialogzustand

Ungültige Ergebnisse werden verworfen.

---

# 12. Rückgabe

Der Dialog liefert genau:

- ein Result
- optionale Zusatzdaten

Nach der Rückgabe wird der Dialog geschlossen.

---

# 13. Zusatzdaten

Optional können zurückgegeben werden:

- ausgewählte Option
- Eingabetext
- Fehlercode
- Statusinformationen

Sensible Daten dürfen nur zurückgegeben werden, wenn dies ausdrücklich vorgesehen ist.

---

# 14. Navigation

Nach erfolgreichem Abschluss:

- Dialog schließen
- Fokus wiederherstellen
- Navigation fortsetzen

Die Navigation wird erst nach vollständiger Bereinigung fortgesetzt.

---

# 15. Fokus

Nach Abschluss:

- Fokus des Dialogs freigeben
- vorherigen Fokus wiederherstellen
- Accessibility-Fokus aktualisieren

---

# 16. Fehler

Tritt während der Verarbeitung ein Fehler auf:

```text
Dialog

↓

Error Result

↓

Error Dialog
```

Das ursprüngliche Ergebnis wird verworfen.

---

# 17. Accessibility

Unterstützt werden:

- Screenreader
- High Contrast
- Reduced Motion
- Tastaturbedienung

Dialogabschlüsse müssen eindeutig angekündigt werden.

---

# 18. Speicherverwaltung

Beim Abschluss werden:

- Dialogobjekte freigegeben
- Eingabepuffer gelöscht
- Fokusinformationen entfernt
- temporäre Daten überschrieben

Heap-Allokationen während der Ergebnisverarbeitung sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Ergebnisrückgabe
- O(1)-Abbruch
- deterministische Laufzeit
- keine Heap-Allokationen

---

# 20. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- mehrfaches Ergebnis
- ungültiges Ergebnis
- fehlender Dialog
- beschädigter Status
- Nullpointer

Im Fehlerfall wird automatisch:

```text
Error
```

zurückgegeben.

---

# 21. Sicherheit

Das System verhindert:

- doppelte Ergebnisrückgaben
- Ergebnisänderungen nach Abschluss
- unvollständige Bereinigung
- Fokusverlust
- inkonsistente Dialogzustände

Jeder Dialog darf nur einmal abgeschlossen werden.

---

# 22. API

```c
typedef enum
{
    NOVA_DIALOG_RESULT_OK,
    NOVA_DIALOG_RESULT_CANCEL,
    NOVA_DIALOG_RESULT_YES,
    NOVA_DIALOG_RESULT_NO,
    NOVA_DIALOG_RESULT_RETRY,
    NOVA_DIALOG_RESULT_IGNORE,
    NOVA_DIALOG_RESULT_CONTINUE,
    NOVA_DIALOG_RESULT_ABORT,
    NOVA_DIALOG_RESULT_TIMEOUT,
    NOVA_DIALOG_RESULT_ERROR
}
nova_dialog_result_t;

nova_result_t
nova_dialog_finish(
    nova_dialog_t* dialog,
    nova_dialog_result_t result);

nova_dialog_result_t
nova_dialog_result(
    const nova_dialog_t* dialog);

bool
nova_dialog_completed(
    const nova_dialog_t* dialog);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Dialog-ID
- Ergebnis
- Abschlusszeit
- Abbruchgrund
- Timeout
- Fehler
- Speicherverbrauch
- Fehleranzahl

---

# 24. Testfälle

Zu testen sind:

- OK
- Cancel
- Retry
- Abort
- Timeout
- Error
- mehrfaches Ergebnis
- Fokuswiederherstellung
- Accessibility
- Recovery

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- jeder Dialog genau ein Ergebnis liefert.
- Benutzer- und Systemabbrüche eindeutig verarbeitet werden.
- Timeout optional unterstützt wird.
- Ressourcen nach Abschluss vollständig freigegeben werden.
- Fokus korrekt wiederhergestellt wird.
- keine Heap-Allokationen während der Ergebnisverarbeitung stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTDIALOG-0011 – Dialogergebnisse und Abbruch
- ADR-BOOTNAV-0004 – Back Navigation
- ADR-BOOTSAFE-0002 – Sichere Dialogabschlüsse
- ADR-BOOTSTATE-0001 – Globales Zustandsmodell
- ADR-BOOTINPUT-0006 – Fokusverwaltung

## NPSPECs

- NPSPEC-BOOTDIALOG-0001 – Boot Dialog Architecture
- NPSPEC-BOOTDIALOG-0002 – Modal Dialog
- NPSPEC-BOOTDIALOG-0009 – Dialog Focus Management
- NPSPEC-BOOTDIALOG-0010 – Dialog Animation
- NPSPEC-BOOTNAV-0004 – Back Navigation
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 27. Zusammenfassung

Das **Dialog Result and Cancellation System** definiert den vollständigen Abschluss aller Dialoge innerhalb der NovaOS Bootoberfläche. Jeder Dialog liefert genau ein gültiges Ergebnis oder wird kontrolliert abgebrochen. Benutzer- und Systemabbrüche, optionale Timeouts, Fokuswiederherstellung sowie die sichere Freigabe aller Ressourcen erfolgen deterministisch und nachvollziehbar. Dadurch entsteht ein konsistentes und fehlertolerantes Dialogabschlussmodell für BIOS- und UEFI-basierte Bootumgebungen.