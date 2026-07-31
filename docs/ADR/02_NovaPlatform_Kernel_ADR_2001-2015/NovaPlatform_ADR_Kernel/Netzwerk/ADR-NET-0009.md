# ADR-NET-0009 – VPN

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-NET-0009 |
| Titel | Virtual Private Network |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Netzwerkvirtualisierung und sichere Tunnel |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-NET-0001, ADR-NET-0002, ADR-NET-0005, ADR-NET-0006, ADR-NET-0008, ADR-SEC-0008 |
| Nachfolgende Spezifikation | NPSPEC-NET-VPN |

---

## 1. Kontext

NovaOS benötigt eine integrierte VPN-Infrastruktur für verschlüsselte Verbindungen über nicht vertrauenswürdige Netzwerke.

Typische Anwendungsfälle sind:

- Zugriff auf Unternehmensnetze
- sichere Verbindung in öffentlichen Netzwerken
- Site-to-Site-Tunnel
- Remote-Administration
- Zugriff auf private Dienste
- anwendungsbezogene Tunnel
- getrennte Netzwerkumgebungen
- Datenschutzprofile

NovaOS soll keine eigene, ungeprüfte VPN-Kryptografie oder ein proprietäres Sicherheitsprotokoll erfinden.

---

## 2. Problemstellung

Ein VPN besteht nicht nur aus Verschlüsselung. Es beeinflusst ebenfalls:

- Routing
- DNS-Auflösung
- Firewall
- Netzwerk-Namespaces
- Quelladressauswahl
- MTU und Fragmentierung
- Authentifizierung
- Schlüsselverwaltung
- Verbindungswiederherstellung

Fehler können zu DNS-Leaks, Routing-Leaks oder unverschlüsseltem Fallback-Verkehr führen.

---

## 3. Entscheidung

NovaOS implementiert ein protokollunabhängiges VPN Framework.

Das Framework stellt bereit:

- virtuelle Tunnelinterfaces
- capability-geschützte VPN-Profile
- Routing- und Firewallintegration
- DNS-Kontexte
- Schlüssel- und Identitätsanbindung
- Verbindungszustände
- Kill-Switch-Richtlinien
- Split- und Full-Tunnel-Unterstützung
- versionierte VPN-Provider-Schnittstellen

Konkrete VPN-Protokolle werden durch isolierte Provider implementiert. Etablierte, öffentlich geprüfte Protokolle und Kryptografie werden bevorzugt.

---

## 4. Architektur

```text
Anwendung
    |
    v
Routing und Firewall
    |
    v
Virtuelles VPN-Interface
    |
    v
VPN Provider
    |
    v
Physisches Netzwerkinterface
    |
    v
VPN-Gegenstelle
```

Anwendungen kommunizieren über gewöhnliche Sockets. Sie müssen die interne Tunnelimplementierung nicht kennen.

---

## 5. VPN Provider

Ein VPN Provider implementiert ein bestimmtes Tunnelprotokoll.

Mögliche Provider sind:

- WireGuard-kompatibler Provider
- IKEv2/IPsec-Provider
- TLS-basierter VPN-Provider
- Unternehmens- oder gerätespezifischer Provider

Provider laufen grundsätzlich in isolierten Userspace-Diensten.

Kernelunterstützung ist nur für kleine, klar abgegrenzte und leistungskritische Datenpfade zulässig.

---

## 6. Keine eigene Kryptografie

NovaOS definiert keine neuen kryptografischen Algorithmen.

VPN Provider verwenden:

- etablierte kryptografische Verfahren
- versionierte Krypto-Schnittstellen
- sichere Zufallsquellen
- geprüfte Schlüsselableitung
- authentifizierte Verschlüsselung
- aktuelle Protokollparameter

Veraltete Algorithmen und unsichere Konfigurationen werden standardmäßig abgelehnt.

Kompatibilitätsmodi müssen ausdrücklich aktiviert und sichtbar gekennzeichnet werden.

---

## 7. VPN-Profil

Ein VPN-Profil enthält mindestens:

```c
typedef struct vpn_profile_info {
    uint64_t profile_id;
    uint32_t provider_type;
    uint32_t tunnel_mode;
    uint32_t flags;
    uint32_t state;
    uint64_t owner_principal;
} vpn_profile_info_t;
```

Zusätzlich können gespeichert werden:

- Gegenstellen
- erlaubte Netzwerkpräfixe
- Authentifizierungsmethode
- DNS-Konfiguration
- Routingmodus
- Kill-Switch-Richtlinie
- Netzwerkprofil
- automatische Verbindungsbedingungen
- Wiederverbindungsstrategie

Geheime Schlüssel werden nicht direkt im gewöhnlichen Profil gespeichert.

---

## 8. Tunnelmodi

NovaOS unterstützt:

| Modus | Bedeutung |
|---|---|
| Full Tunnel | gesamter vorgesehener Verkehr läuft über VPN |
| Split Tunnel | nur ausgewählte Ziele laufen über VPN |
| Application Tunnel | nur bestimmte Anwendungen verwenden den Tunnel |
| Service Tunnel | Tunnel ist an einen Systemdienst gebunden |
| Site-to-Site | Verbindung zwischen Netzwerkbereichen |
| On-Demand | Tunnel wird bei definierten Bedingungen aufgebaut |

Die tatsächlich nutzbaren Modi hängen vom Provider und den Sicherheitsrichtlinien ab.

---

## 9. Virtuelles Interface

Jede aktive VPN-Verbindung erzeugt ein virtuelles Netzwerkinterface.

Das Interface besitzt:

- eigene Interface-ID
- MTU
- Netzwerkadressen
- Routingkontext
- Firewallkontext
- QoS-Zuordnung
- Verbindungszustand
- Providerbindung

Das virtuelle Interface wird wie andere Netzwerkschnittstellen in Routing und Socket API integriert.

VPN-Schlüssel und interne Kryptozustände werden nicht als Interfaceattribute offengelegt.

---

## 10. Verbindungszustände

```text
DISCONNECTED
     |
     v
CONNECTING
     |
     v
AUTHENTICATING
     |
     v
CONFIGURING
     |
     v
CONNECTED
     |
     +--> RECONNECTING
     |
     v
DISCONNECTING
     |
     v
DISCONNECTED
```

Ein Fehler führt in den Zustand `FAILED`, sofern keine Wiederverbindung versucht wird.

Der Zustand `CONNECTED` wird erst gemeldet, nachdem Tunnel, Routing, Firewall und DNS-Kontext vollständig aktiviert wurden.

---

## 11. Verbindungsaufbau

Der Aufbau erfolgt transaktional:

1. Profil und Berechtigungen prüfen.
2. Provider starten oder kontaktieren.
3. Gegenstelle auflösen.
4. sichere Sitzung aufbauen.
5. Tunneladressen und MTU festlegen.
6. virtuelles Interface aktivieren.
7. Routinggeneration vorbereiten.
8. Firewall und Kill Switch aktivieren.
9. DNS-Kontext aktivieren.
10. Konfiguration atomar veröffentlichen.

Schlägt ein Schritt fehl, werden vorbereitete Änderungen vollständig zurückgenommen oder der Verkehr bleibt sicher blockiert.

---

## 12. Bootstrapping

Die VPN-Gegenstelle muss vor Aktivierung des Tunnels über das physische Netzwerk erreichbar sein.

Dafür erzeugt NovaOS eine geschützte Bootstrap-Route zur Gegenstelle.

Diese Route:

- gilt nur für den VPN-Transport,
- ist an das physische Interface gebunden,
- darf nicht für gewöhnlichen Anwendungsverkehr verwendet werden,
- wird bei Adressänderungen aktualisiert,
- verhindert eine Routingschleife durch den eigenen Tunnel.

---

## 13. Full Tunnel

Bei einem Full Tunnel werden Default Routes für IPv4 und IPv6 über das virtuelle Interface geleitet.

Dabei müssen mindestens kontrolliert werden:

- IPv4-Verkehr
- IPv6-Verkehr
- DNS
- lokale Netzwerke
- Multicast
- neue Interfaces
- Fallbackrouten

Ein nur für IPv4 konfigurierter VPN-Tunnel darf IPv6-Verkehr nicht unbemerkt am Tunnel vorbeileiten.

Die Richtlinie muss IPv6 entweder tunneln oder ausdrücklich blockieren.

---

## 14. Split Tunnel

Ein Split Tunnel leitet nur definierte Ziele oder Anwendungen über das VPN.

Auswahlkriterien können sein:

- Zielpräfix
- Anwendung
- Benutzer oder Session
- Dienstidentität
- Netzwerk-Namespace
- DNS-Domäne mit aufgelösten Zieladressen
- Sicherheitsdomäne

Split-Tunnel-Regeln werden als Policy Routing umgesetzt.

Überlappende Präfixe und widersprüchliche Regeln müssen vor Aktivierung erkannt werden.

---

## 15. Application Tunnel

Ein Application Tunnel verwendet einen eigenen oder eingeschränkten Netzwerk-Namespace.

Die Anwendung erhält:

- nur die vorgesehenen Routen,
- den zugehörigen DNS-Kontext,
- passende Firewallregeln,
- eine Capability auf den Tunnelkontext.

Andere Anwendungen dürfen den Tunnel nicht automatisch verwenden.

Eine Anwendung kann den Tunnel nicht durch selbst erzeugte Routen oder Interfacebindungen umgehen.

---

## 16. Kill Switch

Der Kill Switch verhindert unverschlüsselten Fallback-Verkehr.

Unterstützte Modi:

| Modus | Verhalten |
|---|---|
| Off | Verkehr darf auf normale Routen zurückfallen |
| While Connected | nur während aktiver VPN-Sitzung erzwingen |
| Always | geschützter Verkehr bleibt ohne VPN blockiert |
| Application | nur definierte Anwendungen blockieren |
| Full System | sämtlichen externen Verkehr ohne VPN blockieren |

Kill-Switch-Regeln werden vor dem Verbindungsaufbau vorbereitet und erst nach sicherer Tunnelaktivierung gelockert.

Ein Ausfall des VPN Providers darf keinen Fail-open-Zustand erzeugen.

---

## 17. DNS-Integration

Jede VPN-Verbindung kann einen eigenen Resolverkontext bereitstellen.

Dieser kann enthalten:

- VPN-DNS-Server
- Suchdomänen
- Split-DNS-Regeln
- DNSSEC-Richtlinien
- interne Namenszonen
- Cache-Trennung

DNS-Anfragen für interne VPN-Domänen dürfen nicht an öffentliche Resolver gesendet werden.

Beim Trennen des VPN werden die VPN-spezifischen DNS-Caches invalidiert oder isoliert behalten, ohne sie in andere Netzwerkkontexte zu übernehmen.

---

## 18. Routing-Integration

VPN-Routen werden in einer transaktionalen Routinggeneration aktiviert.

Der VPN Manager prüft:

- Bootstrap-Route
- Tunnelpräfixe
- Default Routes
- Split-Tunnel-Regeln
- Quelladressauswahl
- Routenprioritäten
- rekursive Next Hops
- Überschneidungen mit anderen VPNs

Mehrere VPN-Verbindungen können parallel existieren, benötigen aber eindeutig auflösbare Policy-Regeln.

---

## 19. Firewall-Integration

Die Firewall kontrolliert:

- Transportverbindung zur VPN-Gegenstelle
- Verkehr in das virtuelle Interface
- Verkehr aus dem virtuellen Interface
- Kill-Switch-Regeln
- Forwarding
- anwendungsbezogene Tunnelrechte
- eingehende VPN-Verbindungen

Die Tatsache, dass ein Paket aus einem VPN stammt, macht es nicht automatisch vertrauenswürdig.

VPN-Verkehr durchläuft weiterhin die relevanten Firewallregeln.

---

## 20. QoS und MTU

Ein VPN erzeugt zusätzlichen Header- und Kryptografieaufwand.

Der VPN Provider meldet eine sichere Tunnel-MTU.

NovaOS berücksichtigt:

- Transportprotokoll
- Verschlüsselungsoverhead
- äußere Path MTU
- Fragmentierungsregeln
- TCP MSS-Anpassung
- QoS-Klassifizierung

VPN-Kontrollverkehr darf priorisiert werden, bleibt aber rate-limitiert.

Innere DSCP-Werte werden nicht ungeprüft in das äußere Netzwerk übernommen.

---

## 21. Authentifizierung und Schlüssel

Unterstützte Authentifizierungsformen können sein:

- öffentliche Schlüssel
- Clientzertifikate
- Benutzeranmeldung
- Sicherheitstoken
- gerätegebundene Schlüssel
- Pre-Shared Keys für eingeschränkte Anwendungsfälle

Private Schlüssel werden über den geschützten NovaOS Key Store referenziert.

Das VPN-Profil enthält nur eine Schlüsselreferenz oder Capability, niemals den privaten Schlüssel im Klartext.

TPM-Unterstützung ist optional.

---

## 22. Schlüsselwechsel

VPN Provider müssen sichere Schlüsselrotation unterstützen, sofern das Protokoll dies vorsieht.

Ein Schlüsselwechsel soll:

- laufende Verbindungen möglichst erhalten,
- alte Schlüssel zeitnah verwerfen,
- Replay-Schutz beibehalten,
- fehlgeschlagene Aushandlungen sicher behandeln,
- keine unverschlüsselte Übergangsphase erzeugen.

Abgelaufene oder widerrufene Schlüssel dürfen nicht automatisch weiterverwendet werden.

---

## 23. Wiederverbindung

Bei Netzwerkwechsel oder Verbindungsverlust kann der VPN Manager eine Wiederverbindung versuchen.

Die Strategie verwendet:

- begrenzte Anzahl schneller Versuche
- exponentielles Backoff
- Netzwerkwechselereignisse
- Provider-spezifische Sessionfortsetzung
- feste maximale Wiederholungsrate
- klare Benutzer- und Dienstinformation

Der Kill Switch bleibt während der Wiederverbindung entsprechend seiner Richtlinie aktiv.

---

## 24. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `VPN_RIGHT_CONNECT` | VPN-Verbindung aufbauen |
| `VPN_RIGHT_DISCONNECT` | Verbindung trennen |
| `VPN_RIGHT_USE` | Tunnel für Netzwerkverkehr verwenden |
| `VPN_RIGHT_CONFIGURE` | Profil ändern |
| `VPN_RIGHT_SECRETS` | Schlüsselreferenzen verwalten |
| `VPN_RIGHT_ROUTING` | Tunnelrouten konfigurieren |
| `VPN_RIGHT_KILL_SWITCH` | Kill-Switch-Richtlinien verändern |
| `VPN_RIGHT_INSPECT` | Status und Diagnose lesen |
| `VPN_RIGHT_ADMIN` | systemweite VPN-Verwaltung |

Anwendungen erhalten nur die Rechte für ausdrücklich freigegebene VPN-Profile.

---

## 25. Ressourcenbegrenzung

Begrenzt werden mindestens:

- aktive VPN-Verbindungen
- Tunnelinterfaces
- Routing- und Firewallregeln
- Kryptografiesitzungen
- Paketwarteschlangen
- Wiederverbindungsversuche
- Schlüsselreferenzen
- DNS-Kontexte
- Speicher und CPU-Zeit pro Provider

Ein fehlerhafter Provider darf keine unbegrenzte Wiederverbindungs- oder Paketwarteschleife erzeugen.

---

## 26. Sicherheit und Datenschutz

Für VPN gelten folgende Regeln:

- nur etablierte Kryptografie und Protokolle werden verwendet,
- private Schlüssel bleiben im geschützten Key Store,
- Tunnelstatus wird erst nach vollständiger Konfiguration als verbunden gemeldet,
- Kill-Switch-Regeln arbeiten Fail Closed,
- IPv4, IPv6 und DNS werden gemeinsam berücksichtigt,
- VPN-Gegenstellen werden authentifiziert,
- eingehender VPN-Verkehr bleibt firewallpflichtig,
- Provider laufen bevorzugt isoliert im Userspace,
- Protokoll- und Paketparser werden fuzz-getestet,
- geheime Schlüssel erscheinen weder in Audit noch Diagnose.

---

## 27. Audit und Diagnose

Auditiert werden insbesondere:

- Aufbau und Trennung administrativer VPNs
- Authentifizierungsfehler
- Zertifikats- und Schlüsselprobleme
- Änderungen des Kill Switch
- Routing- oder DNS-Leak-Verhinderung
- wiederholte Providerabstürze
- Änderungen privilegierter VPN-Profile
- Verwendung veralteter Kompatibilitätsmodi

Lokale Diagnose kann anzeigen:

- Verbindungszustand
- Provider und Protokollversion
- Gegenstelle in datenschutzgerechter Darstellung
- Tunnel-MTU
- übertragene Datenmengen
- aktive Routen
- DNS-Kontext
- letzte Fehlerursache

Nutzdaten und private Schlüssel werden nicht protokolliert.

---

## 28. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | VPN ist erfolgreich verbunden |
| `STATUS_VPN_AUTH_FAILED` | Authentifizierung fehlgeschlagen |
| `STATUS_VPN_PEER_UNREACHABLE` | Gegenstelle ist nicht erreichbar |
| `STATUS_VPN_HANDSHAKE_FAILED` | sichere Sitzung konnte nicht aufgebaut werden |
| `STATUS_VPN_ROUTE_CONFLICT` | Routingregeln widersprechen sich |
| `STATUS_VPN_DNS_FAILED` | DNS-Kontext konnte nicht aktiviert werden |
| `STATUS_VPN_KILL_SWITCH_ACTIVE` | Verkehr wird absichtlich blockiert |
| `STATUS_VPN_PROVIDER_FAILED` | VPN Provider ist ausgefallen |
| `STATUS_KEY_REVOKED` | verwendeter Schlüssel wurde widerrufen |
| `STATUS_POLICY_DENIED` | Sicherheitsrichtlinie verbietet das Profil |
| `STATUS_LIMIT_EXCEEDED` | VPN-Ressourcenlimit überschritten |

---

## 29. Alternative Ansätze

### 29.1 Eigenes NovaOS-VPN-Protokoll

**Vorteile:**

- vollständige Kontrolle
- genaue Anpassung an NovaOS

**Nachteile:**

- ungeprüfte Kryptografie und Protokollfehler
- keine vorhandene Infrastruktur
- hohes Sicherheitsrisiko

**Entscheidung:** Abgelehnt.

### 29.2 VPN vollständig im Kernel

**Vorteile:**

- geringer IPC-Aufwand
- potenziell hoher Durchsatz

**Nachteile:**

- große Kernel-Angriffsfläche
- Parser- und Kryptofehler gefährden das System

**Entscheidung:** Provider laufen bevorzugt im Userspace. Nur klar begrenzte Datenpfade dürfen kernelnah optimiert werden.

### 29.3 VPN ohne eigene DNS- und Firewallintegration

**Vorteile:**

- einfachere Implementierung

**Nachteile:**

- DNS- und Routing-Leaks
- unsicherer Fallback bei Verbindungsverlust

**Entscheidung:** Abgelehnt.

---

## 30. Konsequenzen

### Positive Konsequenzen

- protokollunabhängige VPN-Integration
- sichere Full-, Split- und Application-Tunnel
- integrierter Kill Switch
- getrennte DNS-Kontexte
- atomare Routing- und Firewalländerungen
- isolierte VPN Provider
- keine eigene ungeprüfte Kryptografie

### Negative Konsequenzen

- komplexe Koordination mehrerer Netzwerkkomponenten
- Userspace-Verschlüsselung kann zusätzlichen Overhead erzeugen
- mehrere parallele VPNs benötigen eindeutige Policy-Regeln
- Netzwerkwechsel können Reservierungen und Sitzungen unterbrechen

### Risiken

- DNS- oder IPv6-Leaks
- Fail-open bei fehlerhaftem Kill Switch
- Routingschleifen
- kompromittierter Provider
- unsichere Altkonfigurationen
- Verlust oder Widerruf von Schlüsseln
- fehlerhafte MTU-Ermittlung

---

## 31. Implementierungsrichtlinien

- NovaOS entwickelt kein eigenes VPN-Kryptoprotokoll.
- VPN Provider laufen grundsätzlich isoliert im Userspace.
- Tunnel werden als virtuelle Netzwerkinterfaces dargestellt.
- VPN-Aktivierung umfasst Routing, Firewall und DNS als eine Transaktion.
- `CONNECTED` wird erst nach vollständiger Aktivierung gemeldet.
- Der Kill Switch verwendet Fail-closed-Semantik.
- Full Tunnel berücksichtigt IPv4, IPv6 und DNS gemeinsam.
- Private Schlüssel werden nur über Key-Store-Referenzen verwendet.
- Bootstrap-Routen sind auf den VPN-Transport beschränkt.
- Eingehender VPN-Verkehr bleibt durch die Firewall kontrolliert.

---

## 32. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Full-, Split- und Application-Tunnel
- IPv4 und IPv6
- Kill Switch
- DNS- und Split-DNS-Regeln
- Routingtransaktionen
- Bootstrap-Routen
- mehrere parallele VPNs
- Providerabsturz
- Netzwerk- und Interfacewechsel
- Wiederverbindung
- Schlüsselrotation und Widerruf
- MTU und Fragmentierung
- Firewall- und QoS-Integration
- DNS-, IPv4- und IPv6-Leak-Tests
- Fuzzing von VPN-Protokollnachrichten

---

## 33. Verbindliche Invarianten

1. NovaOS verwendet keine selbst erfundene VPN-Kryptografie.
2. Konkrete VPN-Protokolle werden über versionierte Provider implementiert.
3. Jeder aktive Tunnel besitzt ein virtuelles Netzwerkinterface.
4. VPN-Aktivierung umfasst Routing, Firewall und DNS atomar.
5. `CONNECTED` wird erst nach vollständiger Tunnelkonfiguration gemeldet.
6. Kill-Switch-Regeln dürfen bei Provider- oder Verbindungsfehlern nicht Fail Open arbeiten.
7. Full Tunnel berücksichtigt IPv4, IPv6 und DNS.
8. Private Schlüssel werden nicht im gewöhnlichen VPN-Profil gespeichert.
9. VPN-Verkehr bleibt den Firewall- und Capability-Regeln unterworfen.
10. VPN-Nutzdaten und geheime Schlüssel werden nicht standardmäßig protokolliert.

---

## 34. Zusammenfassung

NovaOS implementiert ein protokollunabhängiges VPN Framework mit isolierten Provider-Diensten und virtuellen Netzwerkinterfaces.

Full-, Split- und Application-Tunnel werden über atomare Routing-, Firewall- und DNS-Konfigurationen bereitgestellt. Ein integrierter Kill Switch verhindert unverschlüsselten Fallback-Verkehr und berücksichtigt IPv4, IPv6 sowie DNS gemeinsam.

NovaOS setzt auf etablierte, geprüfte VPN-Protokolle und Kryptografie, anstatt ein eigenes Sicherheitsprotokoll zu entwickeln.