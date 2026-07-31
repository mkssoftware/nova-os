# ADR-NET-0010 – TLS Offloading

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-NET-0010 |
| Titel | TLS Offloading |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Netzwerkleistung und Transportverschlüsselung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-NET-0001, ADR-NET-0002, ADR-NET-0008, ADR-NET-0009, ADR-DEV-0010, ADR-SEC-0008 |
| Nachfolgende Spezifikation | NPSPEC-NET-TLS-OFFLOADING |

---

## 1. Kontext

TLS schützt Netzwerkverbindungen durch Verschlüsselung, Integritätsprüfung und Authentifizierung.

Bei vielen parallelen oder schnellen Verbindungen können TLS-Datensatzverarbeitung und Kryptografie einen erheblichen Teil der CPU-Leistung beanspruchen.

NovaOS soll daher kontrolliertes TLS Offloading unterstützen für:

- Webserver
- Netzwerkdienste
- VPN-nahe Dienste
- große verschlüsselte Datenübertragungen
- lokale Reverse Proxies
- Hochleistungsnetzwerkadapter
- eingebettete Systeme mit Kryptobeschleunigern

TLS Offloading darf die Sicherheitsgrenzen der TLS-Verbindung nicht abschwächen.

---

## 2. Problemstellung

Eine vollständige TLS-Implementierung im Kernel würde eine große und regelmäßig veränderte Protokolloberfläche in den privilegierten Bereich verlagern.

Gleichzeitig ist ein vollständig userspacebasierter Datenpfad bei hohen Datenraten möglicherweise ineffizient.

NovaOS benötigt deshalb ein Modell, das:

- Handshake und Zertifikatsprüfung im Userspace belässt,
- den symmetrischen Datensatzpfad optional beschleunigt,
- Schlüsselmaterial schützt,
- Software- und Hardware-Fallbacks unterstützt,
- Verbindungszustände eindeutig synchronisiert,
- keine unsicheren oder veralteten TLS-Versionen aktiviert.

---

## 3. Entscheidung

NovaOS implementiert TLS Offloading als optionale Beschleunigung des symmetrischen TLS-Datenpfads.

Es gilt:

- TLS-Handshake und Zertifikatsvalidierung bleiben im Userspace.
- Die TLS-Bibliothek handelt Sitzung, Cipher Suite und Schlüssel aus.
- Erst danach dürfen begrenzte Datenpfadschlüssel an einen autorisierten Offload-Kontext übergeben werden.
- Record-Verschlüsselung und -Entschlüsselung kann durch Kernel oder Hardware beschleunigt werden.
- Ohne sichere Offload-Unterstützung bleibt die Verbindung vollständig im Userspace.

TLS Offloading ist eine Optimierung und keine Voraussetzung für TLS.

---

## 4. Architektur

```text
TLS-Bibliothek im Userspace
     |
     | Handshake und Zertifikatsprüfung
     v
TLS-Sitzung aufgebaut
     |
     | begrenzter Offload-Kontext
     v
Kernel TLS Record Layer
     |
     +--> Software-Kryptografie
     |
     +--> Hardware- oder NIC-Offload
     |
     v
TCP Socket
```

Die Anwendung verwendet weiterhin einen gewöhnlichen Socket mit ausdrücklich aktiviertem TLS-Offload-Kontext.

---

## 5. Verantwortlichkeiten

| Komponente | Verantwortung |
|---|---|
| Anwendung | Auswahl des TLS-Dienstes und Fehlerbehandlung |
| TLS-Bibliothek | Handshake, Zertifikate, Cipher Suite, Schlüsselableitung |
| Key Store | Schutz langfristiger privater Schlüssel |
| TLS-Offload-Layer | Verarbeitung symmetrischer TLS Records |
| Socket Layer | Transport und Verbindungszustand |
| Netzwerktreiber | optionale Hardwarebeschleunigung |
| Audit-System | sicherheitsrelevante Konfigurationsereignisse |

Langfristige private Zertifikatsschlüssel werden niemals an den Netzwerktreiber übertragen.

---

## 6. Unterstützte TLS-Versionen

TLS Offloading unterstützt ausschließlich ausdrücklich freigegebene Protokollversionen.

Für die erste Implementierung gilt:

| Version | Status |
|---|---|
| TLS 1.3 | bevorzugt |
| TLS 1.2 | kontrollierte Kompatibilität |
| TLS 1.1 und älter | nicht unterstützt |
| SSL-Versionen | nicht unterstützt |

Neue Versionen benötigen eine aktualisierte und geprüfte Offload-Spezifikation.

Die Userspace-TLS-Bibliothek bleibt für die eigentliche Protokollaushandlung verantwortlich.

---

## 7. Cipher Suites

Offloading wird nur für unterstützte AEAD-Verfahren bereitgestellt.

Beispielsweise:

- AES-GCM
- ChaCha20-Poly1305

Nicht authentifizierte Verschlüsselungsverfahren und veraltete Cipher Suites werden nicht unterstützt.

Wenn eine ausgehandelte Cipher Suite nicht offloadfähig ist, verwendet die TLS-Bibliothek den normalen Userspace-Pfad.

Der Wunsch nach Offloading darf die Auswahl nicht auf eine schwächere Cipher Suite herabstufen.

---

## 8. Offload-Kontext

Ein TLS-Offload-Kontext ist ein capability-geschütztes Kernelobjekt.

```c
typedef struct tls_offload_info {
    uint64_t context_id;
    uint64_t socket_id;
    uint16_t tls_version;
    uint16_t cipher_suite;
    uint32_t direction_flags;
    uint32_t backend;
    uint64_t generation;
} tls_offload_info_t;
```

Der Kontext enthält zusätzlich geschützt:

- Schreib- und Leseschlüssel
- Nonce- oder IV-Zustände
- Sequenznummern
- Recordgrenzen
- Key-Update-Generation
- Backendzustand
- Fehlerstatus

Schlüsselmaterial darf nicht über normale Diagnose-APIs ausgegeben werden.

---

## 9. Aktivierung

```c
status_t tls_offload_enable(
    cap_handle_t socket_handle,
    const tls_offload_configuration_t *configuration,
    cap_handle_t *offload_handle
);
```

Voraussetzungen:

- Socket ist verbunden,
- TLS-Handshake wurde erfolgreich abgeschlossen,
- TLS-Version und Cipher Suite sind unterstützt,
- Anwendung besitzt die erforderlichen Rechte,
- Schlüssel und Sequenzzustände sind vollständig,
- Socket enthält noch keine widersprüchlichen Daten,
- Backend kann die geforderte Semantik bereitstellen.

Die Aktivierung erfolgt atomar pro Richtung.

---

## 10. Richtungsgetrennter Betrieb

Offloading kann getrennt aktiviert werden für:

- Senden
- Empfangen
- beide Richtungen

Dadurch sind auch Backends möglich, die nur einen Teil des Datenpfads beschleunigen.

Jede Richtung besitzt eigene:

- Schlüssel
- Sequenznummer
- Nonce-Zustände
- Key-Update-Generation
- Fehlerzustände

Schlüssel und Sequenznummern dürfen niemals zwischen den Richtungen verwechselt werden.

---

## 11. Record-Semantik

Der Offload-Layer verarbeitet vollständige TLS Records.

Er übernimmt:

- Recordgrenzen
- authentifizierte Verschlüsselung
- Entschlüsselung
- Authentifizierungsprüfung
- Sequenznummern
- Nonce-Bildung
- maximale Recordgröße
- Weitergabe von Klartext oder Ciphertext

Ungültig authentifizierte Records werden verworfen und führen zu einem sichtbaren TLS-Fehler.

Unauthentifizierter Klartext darf niemals an die Anwendung geliefert werden.

---

## 12. TCP-Integration

TLS Records und TCP-Segmente besitzen unterschiedliche Grenzen.

Der Offload-Layer muss deshalb:

- aufgeteilte Records zusammensetzen,
- mehrere Records in einem TCP-Segment verarbeiten,
- Retransmissions korrekt behandeln,
- Out-of-Order-Segmente nicht als neue TLS-Daten interpretieren,
- TCP-Abbruch und Half-Close beachten.

TCP bleibt für zuverlässige Byteübertragung zuständig. TLS verarbeitet den daraus rekonstruierten geordneten Bytestrom.

---

## 13. Kernel-Software-Offload

Wenn keine geeignete Hardware vorhanden ist, kann der Kernel den symmetrischen TLS-Datenpfad mit optimierten Kryptoimplementierungen verarbeiten.

Mögliche Vorteile:

- weniger Userspace-/Kernel-Kopien
- direkte Verbindung zum Socketpuffer
- Nutzung architekturspezifischer CPU-Instruktionen
- effizientere Sendepfadsegmentierung

Der Kernel implementiert dabei keinen vollständigen TLS-Handshake.

Komplexe Protokollentscheidungen bleiben in der TLS-Bibliothek.

---

## 14. Hardware- und NIC-Offload

Geeignete Netzwerkadapter oder Kryptobeschleuniger dürfen TLS Records verarbeiten.

Voraussetzungen:

- verifizierter Treiber,
- unterstützte TLS-Version und Cipher Suite,
- kontrollierte Schlüsselübergabe,
- eindeutige Verbindung zwischen Schlüssel und Socket,
- geschützter DMA-Kontext,
- Fehler- und Fallbackpfad,
- sichere Schlüsselentfernung.

Hardware-Offload-Ergebnisse werden nicht ungeprüft als korrekt behandelt, wenn das Gerät Fehler oder inkonsistente Zustände meldet.

---

## 15. Schlüsselverwaltung

Langfristige private Schlüssel verbleiben im geschützten Key Store oder in einem autorisierten Kryptodienst.

An den Offload-Kontext werden nur die für den laufenden symmetrischen Datenpfad erforderlichen Sitzungsschlüssel übergeben.

Schlüssel werden:

- nicht in gewöhnliche Prozesslogs geschrieben,
- nicht über normale Diagnose-APIs ausgegeben,
- nicht in Crash Dumps aufgenommen,
- bei Kontextende sicher verworfen,
- bei Key Update ersetzt,
- nicht zwischen Sockets wiederverwendet.

TPM oder Hardware-Key-Store bleiben optional.

---

## 16. Key Update

TLS 1.3 Key Updates werden durch die Userspace-TLS-Bibliothek koordiniert.

Der Ablauf ist:

1. neue Traffic Secrets ableiten,
2. neuen Offload-Zustand vorbereiten,
3. Recordgrenze bestimmen,
4. neue Schlüsselgeneration atomar aktivieren,
5. alte Schlüssel nach Abschluss ausstehender Records löschen.

Ein Key Update darf keine Recordverarbeitung mit gemischten Sequenz- oder Schlüsselzuständen erzeugen.

---

## 17. Zero-Copy

TLS Offloading kann mit Zero-Copy- und Scatter/Gather-I/O kombiniert werden.

Dabei gelten:

- Buffer werden über kontrollierte Memory Objects bereitgestellt,
- DMA erfolgt über das DMA Framework,
- Klartextbuffer bleiben nur autorisierten Prozessen zugänglich,
- Ciphertext- und Klartextbereiche werden eindeutig getrennt,
- Bufferlebenszeiten reichen bis zum bestätigten Abschluss,
- nicht mehr benötigte sensible Puffer werden bereinigt.

Zero-Copy darf keine Speicher-, Capability- oder Schlüsselgrenzen umgehen.

---

## 18. Fallback

NovaOS unterstützt einen sicheren Software-Fallback.

Fallback ist möglich bei:

- nicht unterstützter Cipher Suite,
- Hardwareausfall,
- Treiberneustart,
- Ressourcenknappheit,
- Key Update,
- nicht unterstützter TLS-Funktion.

Ein Wechsel während einer aktiven Sitzung ist nur zulässig, wenn Sequenznummern und Recordzustände eindeutig übertragen werden können.

Andernfalls wird die TLS-Verbindung sicher beendet, statt möglicherweise fehlerhaft fortgesetzt.

---

## 19. Fehlerbehandlung

Mögliche Fehler sind:

| Fehler | Reaktion |
|---|---|
| Authentifizierung eines Records schlägt fehl | Verbindung sicher beenden |
| Sequenzzustand widersprüchlich | Offload stoppen und Verbindung beenden |
| Hardware fällt aus | sicherer Fallback oder Verbindungsabbruch |
| Key Update schlägt fehl | keine Verwendung unklarer Schlüssel |
| Socket wird geschlossen | Offload-Kontext und Schlüssel freigeben |
| Gerät wird entfernt | DMA und Hardwarekontext widerrufen |
| Klartext-/Ciphertextgrenze unklar | Verarbeitung abbrechen |

Ein Offload-Fehler darf niemals zu unverschlüsselter Übertragung führen.

---

## 20. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `TLS_OFFLOAD_RIGHT_ENABLE` | Offload für einen Socket aktivieren |
| `TLS_OFFLOAD_RIGHT_SEND` | Sendepfad konfigurieren |
| `TLS_OFFLOAD_RIGHT_RECEIVE` | Empfangspfad konfigurieren |
| `TLS_OFFLOAD_RIGHT_KEY_UPDATE` | Schlüsselgeneration wechseln |
| `TLS_OFFLOAD_RIGHT_DISABLE` | Offload beenden |
| `TLS_OFFLOAD_RIGHT_INSPECT` | nicht geheime Statusdaten lesen |
| `TLS_OFFLOAD_RIGHT_ADMIN` | Backendrichtlinien verwalten |

Das Recht zum Lesen des Socketstatus erteilt keinen Zugriff auf TLS-Schlüssel.

---

## 21. Ressourcenbegrenzung

Begrenzt werden mindestens:

- Offload-Kontexte pro Prozess
- Offload-Kontexte pro Gerät
- gepinnte Buffer
- Hardware-Schlüsselslots
- ausstehende TLS Records
- Recordgröße
- Key-Update-Rate
- Fallbackversuche
- CPU- und DMA-Ressourcen

Wenn Ressourcen nicht verfügbar sind, bleibt der gewöhnliche Userspace-TLS-Pfad funktionsfähig.

---

## 22. Sicherheit

Für TLS Offloading gelten folgende Regeln:

- Handshake und Zertifikatsprüfung verbleiben im Userspace.
- Nur symmetrische Sitzungsschlüssel werden an Offload-Kontexte übergeben.
- Offloading darf keine schwächere Cipher Suite erzwingen.
- Unausthentifizierter Klartext wird niemals ausgeliefert.
- Sequenznummern und Nonces dürfen nicht wiederverwendet werden.
- Langfristige private Schlüssel werden nicht an Treiber übertragen.
- Offload-Fehler führen niemals zu unverschlüsseltem Fallback.
- JIT-, DMA- und Speicherpfade halten NX-, W^X- und Capability-Regeln ein.
- Schlüssel werden bei Ende oder Widerruf sicher entfernt.

---

## 23. Audit und Diagnose

Auditiert werden insbesondere:

- Aktivierung privilegierter Hardware-Offloads
- Wechsel zwischen Hardware- und Softwarebackend
- Authentifizierungsfehler
- fehlerhafte Key Updates
- Treiber- oder Hardwareausfälle
- Verwendung veralteter Kompatibilitätsmodi
- administrative Änderungen der Offload-Richtlinie

Diagnoseinformationen dürfen enthalten:

- TLS-Version
- Cipher Suite
- Backend
- verarbeitete Byte- und Recordanzahl
- Fallbackstatus
- Fehlerklasse
- Key-Update-Generation

Schlüssel, Nonces, Klartext und vollständige Zertifikate werden nicht protokolliert.

---

## 24. Datenschutz

TLS Offloading darf die durch TLS geschützten Inhalte nicht zusätzlichen Prozessen oder globalen Diagnosediensten zugänglich machen.

Klartext ist nur sichtbar für:

- die berechtigte Anwendung,
- den kontrollierten TLS-Datenpfad,
- ausdrücklich autorisierte lokale Sicherheitskomponenten.

Paketmitschnitt auf dem physischen Interface sieht grundsätzlich nur Ciphertext.

Ein Klartextmitschnitt benötigt eine gesonderte, sichtbare Debug- oder Anwendungsfreigabe und ist im Produktionsmodus standardmäßig deaktiviert.

---

## 25. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Offload erfolgreich aktiviert |
| `STATUS_TLS_NOT_ESTABLISHED` | TLS-Handshake ist nicht abgeschlossen |
| `STATUS_TLS_VERSION_UNSUPPORTED` | TLS-Version wird nicht unterstützt |
| `STATUS_TLS_CIPHER_UNSUPPORTED` | Cipher Suite ist nicht offloadfähig |
| `STATUS_TLS_RECORD_INVALID` | TLS Record ist ungültig |
| `STATUS_TLS_AUTH_FAILED` | Record-Authentifizierung fehlgeschlagen |
| `STATUS_TLS_SEQUENCE_ERROR` | Sequenzzustand ist inkonsistent |
| `STATUS_TLS_KEY_UPDATE_FAILED` | Schlüsselwechsel ist fehlgeschlagen |
| `STATUS_OFFLOAD_UNAVAILABLE` | kein geeignetes Backend verfügbar |
| `STATUS_HARDWARE_FAILED` | Hardware-Offload ist ausgefallen |
| `STATUS_ACCESS_DENIED` | erforderliche Capability fehlt |
| `STATUS_LIMIT_EXCEEDED` | Offload-Ressourcenlimit überschritten |

---

## 26. Alternative Ansätze

### 26.1 Vollständiger TLS-Stack im Kernel

**Vorteile:**

- enger Netzwerkpfad
- weniger Übergänge

**Nachteile:**

- große Kernel-Angriffsfläche
- Zertifikats- und Protokollkomplexität im Kernel
- häufige Sicherheitsupdates notwendig

**Entscheidung:** Abgelehnt.

### 26.2 Ausschließlich TLS im Userspace

**Vorteile:**

- starke Isolation
- einfache Aktualisierung

**Nachteile:**

- mögliche zusätzliche Kopien und Systemaufrufe
- begrenzte Hardware-Offload-Integration

**Entscheidung:** Bleibt der Standard- und Fallbackpfad. Optionales Record Offloading ergänzt ihn.

### 26.3 Vollständiges Vertrauen in NIC-Offload

**Vorteile:**

- maximale Entlastung der CPU

**Nachteile:**

- Schlüssel liegen im Gerät
- Treiber- und Firmwarefehler beeinflussen die Sicherheit
- eingeschränkte Diagnose und Portabilität

**Entscheidung:** Hardware-Offload ist optional und bleibt unter Kernel- und Capability-Kontrolle.

---

## 27. Konsequenzen

### Positive Konsequenzen

- geringere CPU-Last bei verschlüsselten Verbindungen
- weniger Kopieroperationen
- Nutzung moderner CPU- und NIC-Beschleunigung
- Userspace behält Handshake und Zertifikatslogik
- sicherer Software-Fallback
- keine Abhängigkeit von Offload-Hardware

### Negative Konsequenzen

- komplexe Synchronisation von Schlüsseln und Sequenznummern
- zusätzliche Kerneloberfläche für TLS Records
- Hardwarebackends benötigen treiberspezifische Unterstützung
- laufender Fallback ist nicht in jedem Zustand möglich

### Risiken

- Wiederverwendung von Nonces oder Sequenznummern
- Schlüsselverlust durch fehlerhafte Treiber
- abweichende VM-, Software- und Hardwaresemantik
- unverschlüsselter Verkehr bei falschem Fehlerpfad
- sensible Daten in Crash Dumps
- inkonsistente Key Updates

---

## 28. Implementierungsrichtlinien

- TLS Offloading bleibt optional.
- TLS-Handshake und Zertifikatsprüfung bleiben im Userspace.
- Nur freigegebene TLS-Versionen und AEAD-Verfahren werden unterstützt.
- Offloading darf die kryptografische Auswahl nicht abschwächen.
- Richtungen besitzen getrennte Schlüssel- und Sequenzzustände.
- Key Updates werden atomar an Recordgrenzen aktiviert.
- Hardware-Offload verwendet kontrollierte DMA- und Schlüsselschnittstellen.
- Fallback erfolgt nur bei eindeutig übertragbarem Zustand.
- Bei unklarem Zustand wird die Verbindung sicher beendet.
- Schlüssel und Klartext erscheinen nicht in normalen Logs oder Crash Dumps.

---

## 29. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- TLS 1.3 und freigegebene TLS-1.2-Konfigurationen
- unterstützte AEAD-Cipher Suites
- getrenntes Sende- und Empfangsoffloading
- geteilte und zusammengefasste TCP-Segmente
- Retransmissions
- ungültige Record-Tags
- Sequenznummern und Nonces
- Key Updates
- Software- und Hardwarebackend
- sicheren Fallback
- Treiberabsturz und Geräteentfernung
- Zero-Copy und Scatter/Gather
- Ressourcenlimits
- Schlüsselbereinigung
- Fuzzing des TLS-Record-Parsers

---

## 30. Verbindliche Invarianten

1. TLS Offloading ist eine optionale Leistungsoptimierung.
2. Handshake und Zertifikatsvalidierung verbleiben im Userspace.
3. Langfristige private Schlüssel werden nicht an Netzwerktreiber übergeben.
4. Offloading darf keine schwächere TLS-Version oder Cipher Suite erzwingen.
5. Sende- und Empfangsrichtung besitzen getrennte Schlüsselzustände.
6. Sequenznummern und Nonces dürfen nicht wiederverwendet werden.
7. Unausthentifizierter Klartext wird niemals an Anwendungen geliefert.
8. Offload-Fehler dürfen keinen unverschlüsselten Fallback erzeugen.
9. Bei nicht eindeutig übertragbarem Zustand wird die Verbindung beendet.
10. TLS-Schlüssel, Klartext und Nonces werden nicht standardmäßig protokolliert.

---

## 31. Zusammenfassung

NovaOS implementiert TLS Offloading als optionale Beschleunigung des symmetrischen TLS-Record-Datenpfads.

Handshake, Zertifikatsprüfung und Protokollaushandlung bleiben in einer aktualisierbaren Userspace-TLS-Bibliothek. Nach erfolgreichem Handshake können begrenzte Sitzungsschlüssel an einen capability-geschützten Kernel- oder Hardware-Offload-Kontext übergeben werden.

Ein vollständiger Userspace-Pfad bleibt immer verfügbar. Kann ein Offload-Zustand nicht eindeutig und sicher fortgesetzt werden, beendet NovaOS die Verbindung, statt auf unverschlüsselte oder kryptografisch unsichere Verarbeitung zurückzufallen.