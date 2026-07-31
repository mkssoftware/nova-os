# ADR-NET-0006 – Firewall

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-NET-0006 |
| Titel | Firewall |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Netzwerksicherheit und Paketfilterung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-NET-0001, ADR-NET-0002, ADR-NET-0003, ADR-NET-0004, ADR-NET-0005, ADR-SEC-0008 |
| Nachfolgende Spezifikation | NPSPEC-NET-FIREWALL |

---

## 1. Kontext

NovaOS benötigt eine integrierte Firewall zur Kontrolle lokaler und weitergeleiteter Netzwerkkommunikation.

Die Firewall muss Schutz bieten für:

- eingehende Verbindungen
- ausgehende Verbindungen
- lokale Systemdienste
- Anwendungen und Sandboxen
- IPv4 und IPv6
- virtuelle Netzwerke
- VPN-Verbindungen
- Netzwerkweiterleitung
- unterschiedliche Netzwerkprofile

Firewallregeln sollen nicht nur Adressen und Ports, sondern auch den Sicherheitskontext einer Anwendung berücksichtigen können.

---

## 2. Problemstellung

Netzwerkpakete stammen aus nicht vertrauenswürdigen Quellen. Gleichzeitig können kompromittierte Anwendungen versuchen, unerlaubte ausgehende Verbindungen aufzubauen.

Die Firewall muss daher:

- Pakete früh und effizient filtern,
- Verbindungszustände berücksichtigen,
- IPv4 und IPv6 gleichwertig behandeln,
- Regeln atomar aktualisieren,
- pro Netzwerk-Namespace arbeiten,
- Anwendungen eindeutig identifizieren,
- Ressourcenerschöpfung verhindern,
- nachvollziehbare Entscheidungen ermöglichen.

---

## 3. Entscheidung

NovaOS implementiert eine zustandsbehaftete, capability-verwaltete Firewall als festen Bestandteil des Netzwerk-Stacks.

Die Architektur wird aufgeteilt in:

- einen schnellen Filterpfad im Netzwerk-Stack,
- einen privilegierten Userspace-Dienst für Regeln und Profile,
- versionierte Regelgenerationen,
- einen begrenzten Connection Tracker,
- lokale Audit- und Diagnoseschnittstellen.

Regeländerungen werden vollständig validiert, kompiliert und anschließend atomar veröffentlicht.

---

## 4. Architektur

```text
Firewall-Dienst
      |
      v
Regeln validieren und kompilieren
      |
      v
Atomare Regelgeneration
      |
      v
Paketfilter im Netzwerk-Stack
      |
      v
ALLOW, DROP oder REJECT
```

Der Userspace-Dienst verarbeitet keine einzelnen Pakete im normalen Datenpfad.

Dadurch bleibt die Filterung auch dann aktiv, wenn der Verwaltungsdienst neu gestartet wird.

---

## 5. Filterpunkte

NovaOS definiert folgende Filterpunkte:

| Filterpunkt | Bedeutung |
|---|---|
| `INGRESS` | Paket wurde von einem Interface empfangen |
| `LOCAL_INPUT` | Paket soll lokal zugestellt werden |
| `FORWARD` | Paket soll weitergeleitet werden |
| `LOCAL_OUTPUT` | Paket wurde lokal erzeugt |
| `EGRESS` | Paket verlässt ein Interface |
| `SOCKET_BIND` | Anwendung möchte Adresse oder Port binden |
| `SOCKET_CONNECT` | Anwendung möchte eine Verbindung aufbauen |
| `SOCKET_LISTEN` | Anwendung möchte eingehende Verbindungen annehmen |

Socket-Filterpunkte besitzen zuverlässigere Prozess- und Anwendungsinformationen als reine Paketfilterpunkte.

---

## 6. Standardrichtlinie

NovaOS verwendet folgende sichere Standardrichtlinie:

| Verkehr | Standard |
|---|---|
| Loopback | erlaubt |
| Ausgehend | nach Anwendungs- und Netzwerkprofil |
| Eingehend, bestehende Verbindung | erlaubt |
| Eingehend, neue Verbindung | verweigert |
| Weiterleitung | verweigert |
| notwendiges ICMP/ICMPv6 | kontrolliert erlaubt |
| Raw Traffic | verweigert ohne besondere Capability |

Systemdienste müssen eingehende Verbindungen ausdrücklich registrieren und freigeben.

Die Firewall darf für IPv6 notwendiges ICMPv6 nicht pauschal blockieren.

---

## 7. Regelmodell

Eine Regel enthält mindestens:

```c
typedef struct firewall_rule {
    uint32_t structure_size;
    uint32_t version;
    uint64_t rule_id;
    uint32_t priority;
    uint32_t action;
    uint32_t direction;
    uint32_t protocol;
    uint32_t connection_states;
    uint64_t flags;
} firewall_rule_t;
```

Zusätzliche Match-Felder werden als versionierte, typisierte Bedingungen gespeichert.

Unbekannte Bedingungen führen nicht zu einer teilweise wirksamen Regel.

---

## 8. Regelbedingungen

Regeln können mindestens folgende Merkmale prüfen:

- IPv4 oder IPv6
- Quell- und Zielpräfix
- Transportprotokoll
- Quell- und Zielport
- Eingangs- und Ausgangsinterface
- Netzwerk-Namespace
- Verbindungszustand
- Benutzer
- Session
- Anwendung
- Sicherheitsdomäne
- Dienstidentität
- ICMP- oder ICMPv6-Typ
- Traffic Class
- Netzwerkprofil
- Zeit- oder Administrationsprofil, sofern ausdrücklich aktiviert

Paketinhalte werden in der Basis-Firewall nicht als allgemeines Match-Feld verwendet.

---

## 9. Aktionen

NovaOS unterstützt mindestens:

| Aktion | Bedeutung |
|---|---|
| `ALLOW` | Verkehr erlauben |
| `DROP` | Paket ohne Antwort verwerfen |
| `REJECT` | Paket verwerfen und geeigneten Fehler senden |
| `LOG` | begrenztes Diagnoseereignis erzeugen und weiter auswerten |
| `RATE_LIMIT` | Verkehr nach definierter Rate begrenzen |
| `MARK` | internes Routing- oder Filtermerkmal setzen |
| `REDIRECT` | an einen autorisierten lokalen Dienst umleiten |
| `CONTINUE` | Regelauswertung fortsetzen |

`REDIRECT` und `MARK` benötigen besondere administrative Rechte.

---

## 10. Regelauswertung

Regeln werden deterministisch in einer kanonischen Reihenfolge ausgewertet.

Grundsätzlich gilt:

1. systemkritische Schutzregeln,
2. Namespace-Regeln,
3. Netzwerkprofil,
4. Anwendungs- und Dienstrichtlinien,
5. benutzerdefinierte Regeln,
6. Standardrichtlinie.

Innerhalb einer Ebene entscheidet die numerische Priorität.

Eine Regel darf Sicherheitsrichtlinien einer höheren Ebene nicht abschwächen.

---

## 11. Zustandsbehaftete Filterung

Der Connection Tracker erkennt mindestens:

| Zustand | Bedeutung |
|---|---|
| `NEW` | neue Verbindung oder neuer Datenfluss |
| `ESTABLISHED` | bestätigte bestehende Verbindung |
| `RELATED` | zu einer erlaubten Verbindung gehörend |
| `INVALID` | keinem gültigen Zustand zuordenbar |
| `CLOSED` | Verbindung wurde beendet |

Für TCP wird die Protokollzustandsmaschine berücksichtigt.

UDP verwendet zeitlich begrenzte Flow-Einträge und wird dadurch nicht als tatsächlich verbindungsorientiert dargestellt.

---

## 12. Connection Tracking

Ein Connection-Tracking-Eintrag enthält mindestens:

- Adressfamilie
- Protokoll
- Quell- und Zieladresse
- Quell- und Zielport
- Netzwerk-Namespace
- Richtung
- Zustand
- letzte Aktivität
- Sicherheits- und Regelgeneration
- Ablaufzeit

Alle Tabellen sind begrenzt.

Bei Überlastung werden neue Einträge kontrolliert abgelehnt oder nach einer definierten Strategie verworfen.

---

## 13. Anwendungsidentität

Ausgehende und lokale Socketoperationen können nach Anwendungsidentität gefiltert werden.

Die Identität stammt aus dem Kernel-Sicherheitskontext und nicht aus Angaben der Anwendung.

Verwendet werden können:

- signierte Anwendungs-ID
- Paket- oder Service-ID
- Sicherheitsdomäne
- Benutzer und Session
- Job Object
- Systemdienst-Identität

Nach dem Übertragen einer Socket-Capability bleibt der ursprüngliche Sicherheitskontext nachvollziehbar. Eine Delegation darf Firewallrichtlinien nicht unbeabsichtigt umgehen.

---

## 14. Netzwerkprofile

Firewallregeln können an ein Netzwerkprofil gebunden sein.

Vorgesehene Profile sind:

| Profil | Verwendung |
|---|---|
| Public | nicht vertrauenswürdiges Netzwerk |
| Private | kontrolliertes lokales Netzwerk |
| Domain | administrativ verwaltetes Netzwerk |
| VPN | Verkehr über einen VPN-Kontext |
| Restricted | besonders eingeschränkte Verbindung |
| Offline | kein externer Netzwerkverkehr |

Ein Netzwerk wird nicht allein aufgrund seines Namens oder WLAN-SSIDs als vertrauenswürdig eingestuft.

Der Benutzer oder eine administrative Richtlinie bestätigt das Profil.

---

## 15. IPv4 und IPv6

Firewallregeln können:

- protokollunabhängig,
- nur für IPv4,
- nur für IPv6

definiert werden.

NovaOS versucht nicht, IPv4-Regeln automatisch unsichtbar in IPv6-Regeln umzuwandeln, wenn die Semantik abweicht.

Die Benutzeroberfläche soll jedoch standardmäßig beide Protokollfamilien berücksichtigen, damit keine unbeabsichtigte Schutzlücke entsteht.

---

## 16. ICMP und ICMPv6

ICMP wird typbezogen gefiltert.

Notwendige Funktionen wie:

- IPv4 Fragmentation Needed
- IPv6 Packet Too Big
- IPv6 Neighbor Discovery
- IPv6 Router Discovery
- Time Exceeded
- ausgewählte Erreichbarkeitsfehler

dürfen nicht pauschal blockiert werden.

Echo-Anfragen und Diagnoseverkehr können getrennt eingeschränkt und rate-limitiert werden.

---

## 17. Fragmentierte Pakete

Die Firewall prüft Fragmente so früh wie möglich.

Dabei gilt:

- das erste Fragment muss vollständige notwendige Headerdaten enthalten,
- überlappende Fragmente werden verworfen,
- Fragment-Reassembly ist streng begrenzt,
- nicht initiale Fragmente werden einem validierten Fragmentkontext zugeordnet,
- Fragmente dürfen Portregeln nicht umgehen,
- `INVALID`-Fragmente werden standardmäßig verworfen.

Die Firewall arbeitet mit den IPv4- und IPv6-Reassembly-Modulen zusammen.

---

## 18. Regelverwaltung

```c
status_t firewall_transaction_begin(
    cap_handle_t firewall_namespace,
    cap_handle_t *transaction_handle
);

status_t firewall_rule_add(
    cap_handle_t transaction_handle,
    const firewall_rule_t *rule
);

status_t firewall_transaction_commit(
    cap_handle_t transaction_handle
);
```

Eine neue Regelmenge wird:

1. vollständig eingelesen,
2. syntaktisch validiert,
3. auf Konflikte und Limits geprüft,
4. in eine interne Darstellung kompiliert,
5. getestet,
6. atomar veröffentlicht.

Bei einem Fehler bleibt die vorherige Regelgeneration aktiv.

---

## 19. Regelgenerationen

Jede veröffentlichte Regelmenge besitzt eine eindeutige Generation.

Laufende Paketverarbeitung sieht entweder:

- vollständig die alte Generation oder
- vollständig die neue Generation.

Alte Generationen werden erst freigegeben, wenn keine CPU sie mehr verwendet.

Connection-Tracking-Einträge können bei sicherheitsrelevanten Regeländerungen neu bewertet oder kontrolliert verworfen werden.

---

## 20. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `FIREWALL_RIGHT_INSPECT` | Regeln und Status lesen |
| `FIREWALL_RIGHT_ADD` | Regeln hinzufügen |
| `FIREWALL_RIGHT_REMOVE` | Regeln entfernen |
| `FIREWALL_RIGHT_PROFILE` | Netzwerkprofile verwalten |
| `FIREWALL_RIGHT_CONNECTIONS` | Connection Tracking untersuchen |
| `FIREWALL_RIGHT_LOGGING` | Diagnose- und Auditregeln verwalten |
| `FIREWALL_RIGHT_REDIRECT` | Umleitungen konfigurieren |
| `FIREWALL_RIGHT_ADMIN` | Firewall-Namespace vollständig verwalten |

Gewöhnliche Anwendungen erhalten keine administrativen Firewallrechte.

---

## 21. Pro-Namespace-Isolation

Jeder Netzwerk-Namespace besitzt einen eigenen Firewallkontext.

Zusätzlich können globale Systemregeln gelten, die alle Namespaces einschränken.

Ein untergeordneter Namespace darf:

- globale Verbote nicht aufheben,
- eigene Regeln verschärfen,
- nur ausdrücklich delegierte Freigaben verwenden,
- keine Pakete in andere Namespaces umleiten, sofern dies nicht genehmigt wurde.

Damit können Anwendungen und Container eigene eingeschränkte Netzwerkansichten erhalten.

---

## 22. Rate Limiting

Regeln können Verkehr nach Token-Bucket- oder vergleichbaren Verfahren begrenzen.

Rate Limits können gelten pro:

- Quelladresse
- Zieladresse
- Port
- Anwendung
- Verbindung
- Interface
- Namespace
- Regel

Die dafür verwendeten Zustände sind begrenzt und müssen gegen absichtlich erzeugte hohe Kardinalität geschützt werden.

Kritischer Kontrollverkehr benötigt gesonderte Grenzwerte.

---

## 23. Logging und Audit

Firewall-Logging erfasst standardmäßig nur notwendige Metadaten:

- Zeitstempel
- Regel-ID
- Aktion
- Adressfamilie
- Protokoll
- gekürzte oder geschützte Adressinformationen
- Ports
- Interface
- Anwendungs- oder Dienstidentität
- Netzwerkprofil

Paketnutzdaten werden nicht protokolliert.

Identische Ereignisse werden zusammengefasst und rate-limitiert.

Auditiert werden insbesondere administrative Regeländerungen und Versuche, geschützte Regeln zu umgehen.

---

## 24. Notfall- und Recovery-Regeln

NovaOS enthält eine minimale, schreibgeschützte Notfallregelmenge.

Sie wird verwendet, wenn:

- der Firewall-Dienst ausfällt,
- die Regelkonfiguration beschädigt ist,
- ein Update nicht validiert werden kann,
- Recovery aktiv ist.

Die Notfallrichtlinie erlaubt nur notwendige lokale Systemkommunikation und ausdrücklich definierte Recovery-Verbindungen.

Ein Ausfall des Verwaltungsdienstes darf die Firewall nicht automatisch vollständig öffnen.

---

## 25. Performance

Der schnelle Filterpfad muss:

- ohne Userspace-Wechsel pro Paket arbeiten,
- vorkompilierte Regeln verwenden,
- gemeinsame globale Sperren vermeiden,
- CPU-lokale Statistiken unterstützen,
- verbindungsbezogene Ergebnisse cachen,
- IPv4 und IPv6 effizient behandeln.

Optimierungen dürfen die deterministische Regelreihenfolge oder Sicherheitsprüfung nicht verändern.

Ein optionaler JIT-Compiler darf erst nach sicherer Verifikation der internen Regelrepräsentation verwendet werden.

---

## 26. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_FILTERED` | Verbindung oder Paket wurde blockiert |
| `STATUS_FIREWALL_RULE_INVALID` | Regel ist syntaktisch ungültig |
| `STATUS_FIREWALL_CONFLICT` | Regeln oder Prioritäten widersprechen sich |
| `STATUS_ACCESS_DENIED` | erforderliches Firewallrecht fehlt |
| `STATUS_LIMIT_EXCEEDED` | Regel- oder Zustandstabellenlimit überschritten |
| `STATUS_GENERATION_CHANGED` | Regelgeneration wurde parallel geändert |
| `STATUS_POLICY_LOCKED` | geschützte Regel darf nicht verändert werden |
| `STATUS_NOT_SUPPORTED` | Match oder Aktion wird nicht unterstützt |
| `STATUS_CONNECTION_STATE_INVALID` | Paket gehört zu keinem gültigen Zustand |

---

## 27. Alternative Ansätze

### 27.1 Zustandslose Firewall

**Vorteile:**

- einfach und ressourcensparend

**Nachteile:**

- schwächere Verbindungssemantik,
- kompliziertere Rückverkehrsregeln.

**Entscheidung:** Zustandslose Regeln bleiben möglich, die Standardfirewall arbeitet zustandsbehaftet.

### 27.2 Firewall vollständig im Userspace

**Vorteile:**

- starke Isolation und einfache Updates

**Nachteile:**

- zusätzlicher Paketkopier- und IPC-Aufwand,
- Ausfall des Dienstes könnte den Datenpfad beeinträchtigen.

**Entscheidung:** Verwaltung im Userspace, schneller Filterpfad im Netzwerk-Stack.

### 27.3 Nur Adressen und Ports filtern

**Vorteile:**

- einfache Regeln

**Nachteile:**

- keine Anwendungs- und Sicherheitsdomänenregeln,
- ungeeignet für lokale Datenschutzkontrolle.

**Entscheidung:** NovaOS unterstützt zusätzlich kernelbestätigte Anwendungsidentitäten.

---

## 28. Konsequenzen

### Positive Konsequenzen

- integrierter Schutz für eingehenden und ausgehenden Verkehr
- zustandsbehaftete Verbindungsprüfung
- anwendungsbezogene Regeln
- getrennte Firewallkontexte pro Namespace
- atomare Regeländerungen
- sichere Fallback-Regeln
- gemeinsame Unterstützung für IPv4 und IPv6

### Negative Konsequenzen

- Connection Tracking benötigt Speicher und Rechenzeit
- komplexe Regeln können schwer nachvollziehbar werden
- Anwendungsdelegation erfordert klare Identitätsregeln
- ICMPv6 benötigt differenzierte statt pauschaler Filterung

### Risiken

- Zustandstabellenüberlastung
- fehlerhafte Regelprioritäten
- unbeabsichtigte IPv6-Schutzlücken
- zu umfangreiches Logging
- Firewallumgehung durch Socket-Delegation
- falsche Fragmentbehandlung

---

## 29. Implementierungsrichtlinien

- Eingehende neue Verbindungen sind standardmäßig blockiert.
- Forwarding ist standardmäßig blockiert.
- Regeländerungen werden transaktional und atomar veröffentlicht.
- Anwendungen werden ausschließlich über kernelbestätigte Identitäten erkannt.
- Globale Schutzregeln können durch Namespaces nicht abgeschwächt werden.
- IPv4 und IPv6 müssen gemeinsam in Standardregeln berücksichtigt werden.
- Notwendiges ICMPv6 wird nicht pauschal blockiert.
- Fragmente dürfen Transportregeln nicht umgehen.
- Connection-Tracking- und Loggingzustände sind begrenzt.
- Ein Ausfall des Firewall-Dienstes darf keinen Fail-open-Zustand erzeugen.

---

## 30. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- IPv4- und IPv6-Regeln
- eingehenden und ausgehenden Verkehr
- TCP-, UDP- und ICMP-Zustände
- Anwendungs- und Benutzerregeln
- Netzwerkprofile
- Namespace-Isolation
- atomare Regelaktualisierung
- Fragmentierung
- Connection-Tracking-Limits
- Rate Limiting
- Socket-Delegation
- Firewall-Dienstausfall
- Notfallregeln
- Mehrkernverarbeitung
- Fuzzing der Regel- und Paketfilterparser

---

## 31. Verbindliche Invarianten

1. Die NovaOS-Firewall ist standardmäßig aktiviert.
2. Neue eingehende Verbindungen sind ohne Freigabe blockiert.
3. Paketweiterleitung ist ohne Freigabe blockiert.
4. Firewallregeln gelten getrennt pro Netzwerk-Namespace und unter globalen Systemregeln.
5. Regeländerungen werden nur vollständig und atomar veröffentlicht.
6. Anwendungsidentitäten stammen aus dem Kernel-Sicherheitskontext.
7. IPv4 und IPv6 werden gleichwertig gefiltert.
8. Notwendiges ICMPv6 darf nicht pauschal blockiert werden.
9. Paketfragmente dürfen Firewallregeln nicht umgehen.
10. Firewall-Nutzdaten werden nicht standardmäßig protokolliert.

---

## 32. Zusammenfassung

NovaOS implementiert eine zustandsbehaftete, capability-verwaltete Firewall als festen Bestandteil des Netzwerk-Stacks.

Der schnelle Filterpfad läuft kernelnah, während ein privilegierter Userspace-Dienst Regeln, Profile und atomare Regelgenerationen verwaltet. Die Firewall kann nach Adressen, Protokollen, Interfaces, Verbindungszuständen und kernelbestätigten Anwendungsidentitäten filtern.

Sichere Standardregeln, getrennte Namespace-Kontexte und ein Fail-closed-Notfallregelwerk sorgen dafür, dass ein Konfigurations- oder Dienstausfall das System nicht ungeschützt öffnet.