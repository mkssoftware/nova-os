# NPSPEC-BOOTMANAGER-UI-0009
# Encryption and Unlock Interface

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0009 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager / Secure Boot / Disk Encryption |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-PASSWORD-0001, NPSPEC-BOOTCONTROL-TEXTFIELD-0001, NPSPEC-BOOTCONTROL-DIALOG-0001, NPSPEC-BOOTCONTROL-PROGRESS-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTSECURITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die **Encryption and Unlock Interface** des NovaOS Boot Managers.

Die Oberfläche dient dem sicheren Entsperren verschlüsselter Datenträger, Systempartitionen und Boot-Volumes vor dem Start des Betriebssystems. Sie stellt sämtliche unterstützten Entsperrverfahren über eine einheitliche Benutzeroberfläche bereit und integriert sich vollständig in die NovaOS Design Language.

Die Benutzeroberfläche darf keinerlei kryptographische Implementierungsdetails offenlegen.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Systempartitionen
- Bootpartitionen
- Datenpartitionen
- Vollverschlüsselung
- TPM-geschützte Volumes
- Smartcard-basierte Freigabe
- Hardware-Token
- zukünftige Entsperrverfahren

Mehrere Entsperrmethoden können parallel unterstützt werden. :contentReference[oaicite:0]{index=0}

---

# 3. Ziele

Die Encryption and Unlock Interface muss:

- sichere Entsperrung ermöglichen
- mehrere Authentifizierungsmethoden unterstützen
- hohe Benutzerfreundlichkeit bieten
- vollständig per Tastatur bedienbar sein
- deterministisch arbeiten
- modular erweiterbar sein

---

# 4. Bildschirmaufbau

```text
+---------------------------------------------------------------+

NovaOS Logo

Unlock Encrypted System

---------------------------------------------------------------

Datenträger

Status

---------------------------------------------------------------

Authentifizierung

---------------------------------------------------------------

Passwortfeld

(optional)

Hardware-Token

(optional)

TPM Status

---------------------------------------------------------------

Fortschritt

---------------------------------------------------------------

[ Entsperren ]

[ Erweiterte Optionen ]

[ Herunterfahren ]

+---------------------------------------------------------------+
```

---

# 5. Komponenten

Die Oberfläche besteht aus:

- Header
- Datenträgerinformationen
- Authentifizierungsbereich
- Statusbereich
- Fortschrittsbereich
- Aktionsbereich

Alle Komponenten verwenden Standard-Boot Controls.

---

# 6. Header

Der Header enthält:

- NovaOS Logo
- Titel
- Beschreibung
- optional Organisationslogo

---

# 7. Datenträgerinformationen

Folgende Informationen werden dargestellt:

- Datenträgername
- Volume-Name
- Verschlüsselungsstatus
- Dateisystem
- Größe
- UUID
- Integritätsstatus

Geheime Schlüssel werden niemals angezeigt.

---

# 8. Authentifizierungsmethoden

Unterstützt werden:

- Passwort
- Wiederherstellungsschlüssel
- TPM
- Smartcard
- PKCS#11-Token
- FIDO2-Token
- Netzwerkfreigabe (optional)
- Schlüsseldatei (optional)

Mehrere Verfahren dürfen kombiniert werden. :contentReference[oaicite:1]{index=1}

---

# 9. Passwortfeld

Das Passwortfeld verwendet:

- Password Control
- Ein-/Ausblenden
- Caps-Lock-Erkennung
- sichere Zeicheneingabe
- Zwischenablage deaktiviert

Passwörter dürfen niemals im Klartext gespeichert werden.

---

# 10. Hardwarestatus

Der Statusbereich zeigt:

- TPM erkannt
- Smartcard erkannt
- USB-Token erkannt
- Netzwerk verfügbar
- automatisches Entsperren möglich

Nicht verfügbare Verfahren werden deaktiviert.

---

# 11. Fortschrittsbereich

Während des Entsperrens werden angezeigt:

- Fortschritt
- aktueller Schritt
- Integritätsprüfung
- Schlüsselprüfung
- Volume-Status

Der Fortschritt verwendet den Progress Indicator.

---

# 12. Aktionen

Standardaktionen:

- Entsperren
- Erweiterte Optionen
- Herunterfahren

Optional:

- Wiederherstellungsschlüssel verwenden
- Schlüsseldatei auswählen
- Diagnose anzeigen

---

# 13. Navigation

Unterstützt werden:

- Tastatur
- Maus
- Touch

Die Eingabereihenfolge folgt einer festen Tabulatorstruktur.

---

# 14. Fokus

Der Fokus muss:

- jederzeit sichtbar sein
- Passwortfeld priorisieren
- weich animiert werden

Beim Öffnen erhält das Passwortfeld automatisch den Fokus.

---

# 15. Animationen

Unterstützt werden:

- Fade-In
- Fokusanimationen
- Fortschrittsanimationen
- Erfolgsanimation
- Fehleranimation

Alle Bewegungen verwenden Motion Tokens.

---

# 16. Themeintegration

Unterstützt werden:

- Dark Theme
- Light Theme
- High Contrast Theme

Alle Farben stammen aus Design Tokens.

---

# 17. Accessibility

Die Oberfläche unterstützt:

- Screenreader
- Tastatursteuerung
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen

Kennworteingaben werden nicht vorgelesen.

---

# 18. Fehlerfälle

Zu behandeln sind:

- falsches Passwort
- beschädigtes Volume
- TPM nicht verfügbar
- Smartcard entfernt
- Tokenfehler
- Integritätsprüfung fehlgeschlagen
- Schlüssel ungültig

Fehler werden eindeutig dargestellt, ohne sicherheitsrelevante Details preiszugeben.

---

# 19. Sicherheit

Das System muss:

- Passwörter unmittelbar nach Verwendung aus dem Speicher löschen
- Fehlversuche protokollieren
- Timeouts unterstützen
- Brute-Force-Schutz ermöglichen
- Klartextschlüssel verhindern
- Speicherseiten mit geheimen Daten nach Nutzung überschreiben

Temporäre Schlüssel und Passphrasen sind nach erfolgreicher oder fehlgeschlagener Authentifizierung sicher zu löschen. :contentReference[oaicite:2]{index=2}

---

# 20. Speicherverwaltung

Die Oberfläche verwendet:

- Dialog Pool
- Resource Cache
- Theme Cache
- Secure Memory Buffer

Heap-Allokationen während der Darstellung sind verboten.

Sensible Speicherbereiche müssen nach Gebrauch überschrieben werden.

---

# 21. Performance

Anforderungen:

- Initialisierung < 100 ms
- O(1)-Theme Lookup
- O(1)-Dialog Lookup
- O(1)-Status Lookup
- deterministische Darstellung

Die Benutzeroberfläche bleibt während kryptographischer Operationen responsiv.

---

# 22. API

```c
typedef enum
{
    NOVA_UNLOCK_PASSWORD,
    NOVA_UNLOCK_TPM,
    NOVA_UNLOCK_SMARTCARD,
    NOVA_UNLOCK_FIDO2,
    NOVA_UNLOCK_KEYFILE,
    NOVA_UNLOCK_RECOVERY_KEY
}
nova_unlock_method_t;

typedef struct
{
    bool volume_locked;
    bool tpm_available;
    bool smartcard_available;
    bool token_available;
}
nova_unlock_status_t;

nova_result_t
nova_unlock_open(void);

nova_result_t
nova_unlock_execute(
    nova_unlock_method_t method);

const nova_unlock_status_t*
nova_unlock_status(void);

nova_result_t
nova_unlock_close(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- verwendete Entsperrmethode
- Anzahl Fehlversuche
- Integritätsstatus
- Volume-ID
- Theme
- Ladezeit

Kennwörter, Schlüssel oder Passphrasen dürfen niemals protokolliert werden.

---

# 24. Testfälle

Zu testen sind:

- Passwortentsperrung
- TPM
- Smartcard
- FIDO2
- Wiederherstellungsschlüssel
- beschädigtes Volume
- Themewechsel
- Accessibility
- Fehlversuche
- Timeout

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche unterstützten Entsperrmethoden korrekt dargestellt werden.
- Passwörter ausschließlich über das Password Control eingegeben werden.
- sensible Daten nach der Verwendung sicher aus dem Speicher entfernt werden.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- Fehlversuche sicher behandelt werden.
- Integritätsprüfungen vor dem Entsperren durchgeführt werden.
- keine Heap-Allokationen während der Darstellung stattfinden.
- sämtliche Animationen ausschließlich Motion Tokens verwenden.

---

# 26. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0009 – Encryption and Unlock Interface
- ADR-BOOTSECURITY-0001 – Secure Boot Security Framework
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTCONTROL-PASSWORD-0001 – Password Field
- NPSPEC-BOOTCONTROL-TEXTFIELD-0001 – Text Field
- NPSPEC-BOOTCONTROL-DIALOG-0001 – Dialog
- NPSPEC-BOOTCONTROL-PROGRESS-0001 – Progress Indicator
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTSECURITY-0001 – Security Framework

---

# 27. Zusammenfassung

Die **Encryption and Unlock Interface** stellt die zentrale Benutzeroberfläche für das sichere Entsperren verschlüsselter Datenträger innerhalb des NovaOS Boot Managers bereit. Sie unterstützt verschiedene Authentifizierungsverfahren wie Passwort, TPM und Hardware-Token, integriert Integritätsprüfungen vor der Freigabe und schützt sensible Informationen durch sichere Speicherbehandlung und konsequente Trennung von Benutzeroberfläche und kryptographischer Implementierung. Durch die vollständige Integration in die NovaOS Design Language, das Theme-System und die Motion Engine entsteht eine konsistente, sichere und benutzerfreundliche Entsperroberfläche.