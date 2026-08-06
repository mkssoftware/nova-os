# NPSPEC-BOOTMANAGER-UI-0010
# Network Boot Interface

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTMANAGER-UI-0010 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Manager UI |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager / PXE / iPXE / HTTP Boot |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTMANAGER-UI-0001, NPSPEC-BOOTCONTROL-LIST-0001, NPSPEC-BOOTCONTROL-TILE-0001, NPSPEC-BOOTCONTROL-PROGRESS-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTNETWORK-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die **Network Boot Interface** des NovaOS Boot Managers.

Die Oberfläche ermöglicht das Erkennen, Konfigurieren und Starten von Netzwerk-Bootquellen. Sie unterstützt klassische PXE-Umgebungen ebenso wie moderne UEFI HTTP-Boot- und iPXE-basierte Infrastrukturen und stellt sämtliche Netzwerk-Bootfunktionen über eine einheitliche Benutzeroberfläche bereit. Moderne UEFI-Systeme unterstützen neben PXE zunehmend auch HTTP Boot als nativen Bootmechanismus. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- PXE Boot
- iPXE
- UEFI HTTP Boot
- IPv4
- IPv6
- DHCP
- Netzwerk-Bootserver
- zukünftige Netzwerk-Bootprotokolle

---

# 3. Ziele

Die Network Boot Interface muss:

- verfügbare Netzwerk-Bootquellen erkennen
- Netzwerkstatus anzeigen
- Bootserver auswählen können
- vollständig per Tastatur bedienbar sein
- deterministisch arbeiten
- modular erweiterbar sein

---

# 4. Bildschirmaufbau

```text
+--------------------------------------------------------------+

Zurück

Network Boot

---------------------------------------------------------------

Netzwerkadapter

---------------------------------------------------------------

Gefundene Bootserver

---------------------------------------------------------------

Serverdetails

---------------------------------------------------------------

Verbindungsstatus

---------------------------------------------------------------

[ Verbinden ]

[ Booten ]

[ Aktualisieren ]

[ Zurück ]

+--------------------------------------------------------------+
```

---

# 5. Komponenten

Die Oberfläche besteht aus:

- Header
- Netzwerkadapterliste
- Bootserverliste
- Detailbereich
- Statusbereich
- Aktionsbereich

Alle Komponenten verwenden Standard-Boot Controls.

---

# 6. Header

Der Header enthält:

- Zurücknavigation
- Netzwerk-Icon
- Titel
- Anzahl erkannter Bootserver

---

# 7. Netzwerkadapter

Für jeden Adapter werden dargestellt:

- Adaptername
- MAC-Adresse
- Verbindungsgeschwindigkeit
- IPv4-Adresse
- IPv6-Adresse
- DHCP-Status
- Linkstatus

Der aktive Adapter wird hervorgehoben.

---

# 8. Bootserverliste

Für jeden gefundenen Server werden angezeigt:

- Servername
- IP-Adresse
- Bootprotokoll
- Antwortzeit
- Status
- Authentifizierungsstatus (optional)

Server werden als Boot Option Tiles dargestellt.

---

# 9. Unterstützte Bootprotokolle

Mindestens folgende Verfahren werden unterstützt:

- PXE
- iPXE
- UEFI HTTP Boot
- TFTP Boot
- HTTP Boot
- HTTPS Boot (optional)

iPXE erweitert klassisches PXE unter anderem um HTTP-Boot, Skriptsteuerung und weitere Netzwerkprotokolle. HTTP Boot ist Bestandteil moderner UEFI-Systeme. :contentReference[oaicite:1]{index=1}

---

# 10. Detailbereich

Beim Markieren eines Servers werden angezeigt:

- Hostname
- IP-Adresse
- unterstützte Bootprotokolle
- Firmwaremodus
- Bootmenü verfügbar
- Signaturstatus
- Zertifikatsstatus (optional)
- letzte Erreichbarkeit

---

# 11. Statusbereich

Der Statusbereich zeigt:

- Netzwerkstatus
- DHCP erfolgreich
- Bootserver gefunden
- Verbindung aktiv
- Downloadstatus
- Authentifizierungsstatus
- Fehlerstatus

Statusinformationen werden als Status Badges dargestellt.

---

# 12. Aktionen

Standardaktionen:

- Verbinden
- Netzwerk-Boot starten
- Netzwerk aktualisieren
- Zurück

Optional:

- Adapter auswählen
- Netzwerkeinstellungen
- DHCP erneuern
- Bootskript anzeigen
- Zertifikate anzeigen

---

# 13. Navigation

Unterstützt werden:

- Tastatur
- Maus
- Touch

Die Navigation erfolgt gemäß Boot Input Architecture.

---

# 14. Fokus

Der Fokus muss:

- jederzeit sichtbar sein
- zwischen Adapter- und Serverliste wechseln können
- weich animiert werden

---

# 15. Animationen

Unterstützt werden:

- Listenanimationen
- Fokusanimationen
- Statusanimationen
- Fortschrittsanimationen
- Dialoganimationen

Alle Animationen verwenden Motion Tokens.

---

# 16. Themeintegration

Unterstützt werden:

- Dark Theme
- Light Theme
- High Contrast Theme

Alle Farben stammen ausschließlich aus Design Tokens.

---

# 17. Accessibility

Die Oberfläche unterstützt:

- Screenreader
- Tastatursteuerung
- hohe Kontraste
- große Schriftgrößen
- reduzierte Animationen

Alle Netzwerkinformationen besitzen textuelle Alternativen.

---

# 18. Fehlerfälle

Zu behandeln sind:

- kein Netzwerkadapter gefunden
- DHCP fehlgeschlagen
- kein Bootserver erreichbar
- Netzwerktimeout
- Authentifizierungsfehler
- ungültiges Bootimage
- Downloadfehler

Fehler werden eindeutig dargestellt.

---

# 19. Sicherheit

Vor dem Netzwerkstart werden geprüft:

- Integrität des Bootimages
- Serverauthentifizierung (falls verfügbar)
- Signaturstatus
- Netzwerkverbindung
- Kompatibilität des Bootprotokolls

Nicht vertrauenswürdige Bootquellen dürfen entsprechend der Sicherheitsrichtlinie blockiert oder mit einer Warnung versehen werden. Netzwerk-Boot sollte nach Möglichkeit signierte Bootkomponenten und Integritätsprüfungen unterstützen. :contentReference[oaicite:2]{index=2}

---

# 20. Speicherverwaltung

Die Oberfläche verwendet:

- Adapter Pool
- Server Pool
- Resource Cache
- Theme Cache

Heap-Allokationen während der Darstellung sind verboten.

---

# 21. Performance

Anforderungen:

- Initialisierung < 200 ms
- O(1)-Adapter Lookup
- O(1)-Server Lookup
- O(1)-Theme Lookup
- deterministische Renderzeit

Serverlisten dürfen asynchron aktualisiert werden, ohne die Benutzeroberfläche zu blockieren.

---

# 22. API

```c
typedef enum
{
    NOVA_NETBOOT_PXE,
    NOVA_NETBOOT_IPXE,
    NOVA_NETBOOT_HTTP,
    NOVA_NETBOOT_HTTPS
}
nova_netboot_protocol_t;

typedef struct
{
    uint32_t server_id;
    nova_netboot_protocol_t protocol;
    bool reachable;
    bool authenticated;
}
nova_netboot_server_t;

nova_result_t
nova_netboot_open(void);

nova_result_t
nova_netboot_scan(void);

nova_result_t
nova_netboot_connect(
    uint32_t server_id);

nova_result_t
nova_netboot_boot(
    uint32_t server_id);

const nova_netboot_server_t*
nova_netboot_server(
    uint32_t server_id);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- erkannte Netzwerkadapter
- erkannte Bootserver
- verwendetes Bootprotokoll
- Antwortzeiten
- Netzwerkfehler
- Benutzeraktionen
- Ladezeit

---

# 24. Testfälle

Zu testen sind:

- PXE Boot
- iPXE Boot
- UEFI HTTP Boot
- IPv4
- IPv6
- DHCP
- Themewechsel
- Accessibility
- Serverausfall
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche unterstützten Netzwerk-Bootserver erkannt werden.
- PXE, iPXE und UEFI HTTP Boot unterstützt werden.
- Netzwerkstatus und Serverinformationen korrekt dargestellt werden.
- Dark-, Light- und High-Contrast-Themes unterstützt werden.
- Netzwerkfehler eindeutig visualisiert werden.
- Integritäts- und Sicherheitsprüfungen vor dem Bootvorgang durchgeführt werden.
- keine Heap-Allokationen während der Darstellung stattfinden.
- sämtliche Animationen ausschließlich Motion Tokens verwenden.

---

# 26. Referenzen

## ADRs

- ADR-BOOTMANAGER-UI-0010 – Network Boot Interface
- ADR-BOOTNETWORK-0001 – Network Boot Framework
- ADR-BOOTDESIGN-0001 – Boot Design Language

## NPSPECs

- NPSPEC-BOOTMANAGER-UI-0001 – Main Boot Selection Screen
- NPSPEC-BOOTCONTROL-LIST-0001 – List
- NPSPEC-BOOTCONTROL-TILE-0001 – Boot Option Tile
- NPSPEC-BOOTCONTROL-PROGRESS-0001 – Progress Indicator
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTMOTION-0001 – Motion Engine
- NPSPEC-BOOTNETWORK-0001 – Network Boot Framework

---

# 27. Zusammenfassung

Die **Network Boot Interface** stellt die zentrale Benutzeroberfläche für sämtliche netzwerkbasierten Bootverfahren des NovaOS Boot Managers bereit. Sie unterstützt PXE, iPXE sowie UEFI HTTP Boot und bietet eine einheitliche Oberfläche zur Auswahl von Netzwerkadaptern, Bootservern und Bootquellen. Durch Integration in die NovaOS Design Language, Unterstützung moderner Netzwerk-Bootstandards, Sicherheitsprüfungen und vollständige Theme- sowie Accessibility-Unterstützung ermöglicht sie einen sicheren, konsistenten und leistungsfähigen Netzwerkstart bereits vor dem Laden des Betriebssystems. :contentReference[oaicite:3]{index=3}