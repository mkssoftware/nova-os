# NPSPEC-BOOTDIALOG-0008
# Credential Dialog

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTDIALOG-0008 |
| Titel | Credential Dialog |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Dialog System |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTDIALOG-0001 bis NPSPEC-BOOTDIALOG-0007, NPSPEC-BOOTINPUT-0001 bis NPSPEC-BOOTINPUT-0007 |
| Zugehörige ADRs | ADR-BOOTDIALOG-0008, ADR-BOOTSEC-0001, ADR-BOOTINPUT-0006, ADR-BOOTACCESS-0001, ADR-BOOTSAFE-0003 |

---

# 1. Ziel

Diese Spezifikation definiert den **Credential Dialog** der NovaOS Boot UI.

Der Credential Dialog dient zur sicheren Eingabe von Authentifizierungsinformationen während des Bootvorgangs.

Das System unterstützt:

- Passwörter
- PINs
- Wiederherstellungsschlüssel
- Verschlüsselungspassphrasen
- TPM-Fallback-Eingaben
- Administratorbestätigungen
- Mehrfaktor-Erweiterungen
- zukünftige Authentifizierungsmethoden

Der Dialog legt besonderen Wert auf Sicherheit, Datenschutz und Benutzerfreundlichkeit.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Authentifizierungsdialoge.

Sie umfasst:

- Boot Manager
- Recovery
- Datenträgerverschlüsselung
- Systemwiederherstellung
- Wartungsmodus
- Administratorfunktionen

Nicht Bestandteil sind:

- Benutzeranmeldung nach dem Boot
- Netzwerkanmeldung
- Cloud-Authentifizierung

---

# 3. Architektur

```text
Authentication Request

↓

Credential Dialog

↓

Credential Validation

↓

Authentication Result

↓

Application
```

---

# 4. Grundprinzip

Der Credential Dialog fordert Authentifizierungsinformationen sicher vom Benutzer an.

Während der Eingabe:

- bleiben Daten ausschließlich im Arbeitsspeicher.
- werden Eingaben maskiert.
- werden keine sensiblen Informationen protokolliert.
- erfolgt keine Speicherung im Dialog selbst.

---

# 5. Aufgaben

Der Credential Dialog übernimmt:

- sichere Eingabe
- Maskierung
- Validierungsvorbereitung
- Fokusverwaltung
- Ergebnisrückgabe
- Fehlerdarstellung
- Accessibility
- Speicherbereinigung

---

# 6. Unterstützte Anmeldedaten

Unterstützt werden:

- Passwort
- PIN
- Passphrase
- Recovery Key
- Security Token
- zukünftige Erweiterungen

Die eigentliche Prüfung erfolgt außerhalb des Dialogs.

---

# 7. Dialogaufbau

Der Dialog besteht aus:

```text
Titel

↓

Beschreibung

↓

Credential Field

↓

Optionale Zusatzinformationen

↓

Schaltflächen
```

---

# 8. Eingabefeld

Das Eingabefeld unterstützt:

- Unicode
- lange Passphrasen
- Copy deaktiviert
- Paste optional
- Cursorsteuerung
- Auswahl

Standardmäßig wird die Eingabe maskiert.

---

# 9. Maskierung

Die Zeichen werden durch Platzhalter dargestellt.

Optional kann der Benutzer die Eingabe kurzzeitig sichtbar machen, sofern dies durch die Sicherheitsrichtlinie erlaubt ist.

---

# 10. Validierung

Der Dialog überprüft lokal:

- leere Eingaben
- Eingabelänge
- Zeichencodierung

Die eigentliche Authentifizierung erfolgt außerhalb des Dialogs.

---

# 11. Fehlversuche

Optional kann angezeigt werden:

- verbleibende Versuche
- Wartezeit
- Sperrstatus

Die Verwaltung erfolgt durch den Authentifizierungsdienst.

---

# 12. Schaltflächen

Standardmäßig:

- Unlock
- OK
- Cancel

Optional:

- Retry
- Recovery
- Shutdown

---

# 13. Navigation

Während der Dialog aktiv ist:

- Hintergrundnavigation blockiert
- Fokus ausschließlich innerhalb des Dialogs
- Back nur gemäß Sicherheitsrichtlinie

---

# 14. Darstellung

Der Credential Dialog verwendet:

- Glass Dialog
- modalen Dialog
- Safe Display Area
- optionales Sicherheitssymbol

Die Größe richtet sich nach Inhalt und Layoutregeln.

---

# 15. Motion

Standardanimation:

```text
Fade

+

Scale
```

Bei Reduced Motion:

```text
Fade
```

Animationen dürfen die Eingabe nicht verzögern.

---

# 16. Accessibility

Unterstützt werden:

- Screenreader
- High Contrast
- Reduced Motion
- skalierbare Schriftgrößen
- vollständige Tastaturbedienung

Maskierte Zeichen dürfen nicht vorgelesen werden.

---

# 17. Datenschutz

Der Dialog speichert:

- keine Passwörter
- keine PINs
- keine Passphrasen

Es erfolgt keine Protokollierung sensibler Eingaben.

---

# 18. Speicherverwaltung

Sensible Eingaben:

- werden in geschützten Puffern gespeichert.
- nach Abschluss sicher überschrieben.
- nach Schließen vollständig freigegeben.

Heap-Allokationen während der Eingabe sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Zeicheneingabe
- O(1)-Löschen
- deterministische Laufzeit
- keine Heap-Allokationen

Die Eingabe darf jederzeit verzögerungsfrei erfolgen.

---

# 20. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- leere Eingabe
- ungültige Zeichen
- Eingabe zu lang
- beschädigte Eingabedaten
- Nullpointer

Fehler werden ohne Offenlegung sensibler Informationen dargestellt.

---

# 21. Sicherheit

Das System verhindert:

- Klartextspeicherung
- Speicherlecks
- Protokollierung sensibler Daten
- Fokusverlust
- unbeabsichtigte Offenlegung von Eingaben

Alle sensiblen Speicherbereiche werden unmittelbar nach Verwendung sicher gelöscht.

---

# 22. API

```c
typedef enum
{
    NOVA_CREDENTIAL_PASSWORD,
    NOVA_CREDENTIAL_PIN,
    NOVA_CREDENTIAL_PASSPHRASE,
    NOVA_CREDENTIAL_RECOVERY_KEY
}
nova_credential_type_t;

nova_result_t
nova_credential_dialog_show(
    nova_credential_type_t type,
    const char* title);

const char*
nova_credential_dialog_value(void);

nova_result_t
nova_credential_dialog_clear(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Credential-Typ
- Fehlversuche
- Eingabelänge
- Öffnungszeit
- Schließzeit
- Speicherverbrauch
- Sperrstatus
- Fehleranzahl

Sensible Eingabedaten dürfen niemals Bestandteil der Diagnosedaten sein.

---

# 24. Testfälle

Zu testen sind:

- Passwort
- PIN
- Passphrase
- Recovery Key
- Unicode
- Fehlversuche
- Speicherbereinigung
- Accessibility
- Reduced Motion
- Recovery

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Eingaben maskiert dargestellt werden.
- sensible Daten ausschließlich temporär im Speicher gehalten werden.
- Speicher nach Abschluss sicher überschrieben wird.
- Fehlversuche korrekt verarbeitet werden.
- Accessibility vollständig unterstützt wird.
- keine Heap-Allokationen während der Eingabe stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTDIALOG-0008 – Authentifizierungsdialoge
- ADR-BOOTSEC-0001 – Sichere Anmeldeinformationen
- ADR-BOOTINPUT-0006 – Fokusverwaltung
- ADR-BOOTACCESS-0001 – Barrierefreie Navigation
- ADR-BOOTSAFE-0003 – Sichere Speicherbehandlung

## NPSPECs

- NPSPEC-BOOTDIALOG-0001 – Boot Dialog Architecture
- NPSPEC-BOOTDIALOG-0002 – Modal Dialog
- NPSPEC-BOOTDIALOG-0003 – Glass Dialog
- NPSPEC-BOOTINPUT-0001 – Input System
- NPSPEC-BOOTINPUT-0007 – Secure Text Input
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 27. Zusammenfassung

Der **Credential Dialog** stellt die standardisierte Oberfläche für sämtliche Authentifizierungsvorgänge innerhalb der NovaOS Bootumgebung bereit. Er unterstützt verschiedene Arten von Anmeldedaten, gewährleistet eine sichere Eingabe durch Maskierung und geschützte Speicherbereiche und verhindert die Offenlegung sensibler Informationen. Durch statische Speicherverwaltung, vollständige Accessibility-Unterstützung und Integration in das Boot-Dialogsystem entsteht eine sichere und konsistente Authentifizierungslösung für BIOS- und UEFI-Systeme.