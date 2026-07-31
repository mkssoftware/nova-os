# ADR-DEV-0003 – Driver Signing

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-DEV-0003 |
| Titel | Driver Signing |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Treibersicherheit und Softwarevertrauen |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-DEV-0001, ADR-DEV-0002, ADR-SEC-0001, ADR-SEC-0008, ADR-SEC-0010 |
| Nachfolgende Spezifikation | NPSPEC-DEV-DRIVER-SIGNING |

---

## 1. Kontext

Gerätetreiber besitzen weitreichende Zugriffsrechte auf Hardware, Speicher, Interrupts und DMA-Ressourcen. Besonders Kernel-Treiber können bei Manipulation oder Programmfehlern die Sicherheit des gesamten Systems gefährden.

NovaOS benötigt deshalb einen Mechanismus, mit dem Herkunft, Integrität und Freigabestatus eines Treiberpakets vor dessen Aktivierung überprüft werden.

---

## 2. Problemstellung

Das Betriebssystem muss sicherstellen, dass:

- Treiber seit ihrer Veröffentlichung nicht verändert wurden,
- der Herausgeber des Treibers überprüfbar ist,
- widerrufene oder kompromittierte Treiber blockiert werden können,
- Kernel- und Userspace-Treiber unterschiedlich streng behandelt werden,
- Offline-Systeme Treiber weiterhin prüfen können,
- Entwicklung eigener Treiber möglich bleibt,
- der Benutzer nicht unbemerkt in einen unsicheren Zustand gelangt.

Eine gültige Signatur beweist dabei nicht, dass ein Treiber fehlerfrei oder vertrauenswürdig programmiert wurde. Sie bestätigt lediglich dessen signierte Herkunft und Integrität.

---

## 3. Entscheidung

NovaOS verlangt digitale Signaturen für alle produktiv geladenen Treiberpakete.

Es gelten drei Vertrauensklassen:

| Vertrauensklasse | Verwendung |
|---|---|
| NovaOS System | Offizielle, mit NovaOS ausgelieferte Treiber |
| Verifizierter Hersteller | Durch anerkannte Hersteller oder Organisationen signierte Treiber |
| Lokale Entwicklung | Vom Gerätebesitzer ausdrücklich lokal autorisierte Treiber |

Nicht signierte Treiber werden im normalen Systemmodus nicht geladen.

---

## 4. Geltungsbereich

Die Signatur umfasst das vollständige Treiberpaket, insbesondere:

- ausführbare Treiberdateien
- Treibermanifest
- Matching-Regeln
- Ressourcenanforderungen
- ABI-Angaben
- Bibliotheken und Hilfsdateien
- Firmwaredateien, sofern enthalten
- Versions- und Herausgeberinformationen
- kryptografische Hashes aller Paketbestandteile

Eine Änderung an einem signierten Bestandteil macht die Paketsignatur ungültig.

---

## 5. Treiberpaket

Ein Treiberpaket besitzt eine eindeutig definierte Struktur:

```text
driver-package/
├── manifest.toml
├── binaries/
├── libraries/
├── firmware/
├── metadata/
└── signature/
    ├── package.sig
    └── certificate-chain.bin
```

Nicht vorhandene optionale Verzeichnisse werden ausgelassen.

Dateipfade, Reihenfolge, Hashverfahren und Serialisierung des Manifests müssen kanonisch definiert sein, damit dieselben Inhalte immer dieselbe Signaturgrundlage erzeugen.

---

## 6. Kryptografisches Modell

NovaOS verwendet moderne asymmetrische Signaturen.

Die erste Implementierung unterstützt mindestens:

- Ed25519 für NovaOS-native Signaturen
- ECDSA P-256 für kompatible Herstellerinfrastrukturen
- SHA-256 oder stärker für Datei- und Pakethashes

Veraltete oder unsichere Algorithmen wie MD5, SHA-1 oder RSA mit unzureichender Schlüssellänge werden nicht akzeptiert.

Die Liste zulässiger Algorithmen wird versioniert und kann durch Systemrichtlinien eingeschränkt werden.

---

## 7. Vertrauenskette

Eine Treibersignatur wird gegen einen vertrauenswürdigen Schlüssel oder eine Zertifikatskette geprüft.

```text
Vertrauensanker
      |
      v
Herausgeberzertifikat
      |
      v
Treibersignatur
      |
      v
Treiberpaket
```

Vertrauensanker werden im geschützten NovaOS Trust Store gespeichert.

Der Trust Store kann enthalten:

- NovaOS-Root-Schlüssel
- freigegebene Herstellerzertifikate
- lokale Administratorzertifikate
- lokal erzeugte Entwicklungsschlüssel
- Sperr- und Widerrufsinformationen

---

## 8. Prüfprozess

Vor der Treiberaktivierung führt NovaOS mindestens folgende Prüfungen durch:

1. Paketstruktur validieren.
2. Manifest vollständig prüfen.
3. Hashes aller Paketbestandteile berechnen.
4. Signatur kryptografisch verifizieren.
5. Zertifikats- oder Schlüsselvertrauen prüfen.
6. Widerrufsstatus prüfen.
7. Gültigkeitszeitraum bewerten.
8. Driver-ABI und Plattformkompatibilität prüfen.
9. Treiberrichtlinien und Berechtigungen prüfen.
10. Prüfergebnis an Device Manager und Audit übergeben.

Erst nach erfolgreicher Prüfung darf das Driver Framework den Treiber laden.

---

## 9. Kernel- und Userspace-Treiber

### 9.1 Kernel-Treiber

Kernel-Treiber benötigen:

- eine gültige Signatur,
- einen für Kernelcode freigegebenen Vertrauensanker,
- eine kompatible Kernel-ABI,
- eine passende Plattformfreigabe,
- eine erfolgreiche Widerrufsprüfung.

Lokal entwickelte Kernel-Treiber benötigen den ausdrücklich aktivierten Entwicklungsmodus.

### 9.2 Userspace-Treiber

Userspace-Treiber benötigen ebenfalls eine Signatur, dürfen jedoch von einer größeren Menge lokal autorisierter Herausgeber stammen.

Ihre Capabilities bleiben unabhängig von der Signatur auf die im Manifest genehmigten Ressourcen begrenzt.

---

## 10. Entwicklungsmodus

NovaOS unterstützt einen ausdrücklich aktivierbaren Driver Development Mode.

In diesem Modus dürfen lokal signierte Entwicklungstreiber geladen werden.

Voraussetzungen:

- bewusste administrative Aktivierung,
- lokale Schlüsselerzeugung oder Schlüsselregistrierung,
- sichtbare Sicherheitswarnung,
- Audit-Eintrag,
- Kennzeichnung des Systemzustands,
- keine automatische Übernahme in den Produktivmodus.

Vollständig unsignierte Treiber bleiben auch im Entwicklungsmodus standardmäßig blockiert. Ein zusätzlicher Unsigned-Testmodus kann ausschließlich für isolierte Testsysteme vorgesehen werden.

---

## 11. Lokale Signierung

Entwickler können eigene Treiber mit einem lokalen Schlüssel signieren.

Der private Schlüssel:

- darf nicht durch NovaOS exportiert oder offengelegt werden,
- soll in einem geschützten Schlüsselspeicher liegen,
- kann optional TPM-gestützt werden,
- muss widerrufbar sein,
- darf nur nach ausdrücklicher Benutzeraktion verwendet werden.

TPM-Unterstützung ist optional. Driver Signing muss auch auf Systemen ohne TPM funktionieren.

---

## 12. Widerruf

NovaOS unterstützt den Widerruf von:

- einzelnen Treiberpaketen,
- bestimmten Treiberversionen,
- Herausgeberzertifikaten,
- Signaturschlüsseln,
- besonders gefährlichen Binärhashes.

Widerrufsinformationen werden lokal gespeichert und können durch signierte Systemupdates aktualisiert werden.

Für Offline-Systeme gilt der zuletzt bekannte lokale Widerrufsstand.

Ein widerrufener Treiber darf beim nächsten Start oder Ladeversuch nicht aktiviert werden.

---

## 13. Bereits laufende Treiber

Wird ein bereits aktiver Treiber nachträglich als widerrufen erkannt, entscheidet die Sicherheitsrichtlinie abhängig vom Risiko:

| Risikostufe | Reaktion |
|---|---|
| Niedrig | Warnung und Blockierung beim nächsten Start |
| Mittel | Geordnete Treiberdeaktivierung |
| Hoch | Sofortiger Stopp, Geräteisolierung und Recovery-Hinweis |
| Kritisch | Sicheres Herunterfahren oder Wechsel in Recovery |

Ein sofortiges Entfernen ist nur zulässig, wenn dadurch keine größere Gefahr für Datenintegrität oder Hardware entsteht.

---

## 14. Zeitprüfung

Zertifikate können Gültigkeitszeiträume besitzen. Die lokale Systemzeit ist während des frühen Starts jedoch möglicherweise noch nicht vertrauenswürdig.

NovaOS verwendet deshalb:

- signierte Zeitinformationen aus Systemupdates,
- den letzten vertrauenswürdigen Zeitstand,
- Firmwarezeit nur als zusätzlichen Hinweis,
- optional TPM-gestützte monotone Zustände.

Eine unsichere Uhrzeit darf nicht automatisch dazu führen, dass ein eindeutig widerrufener Treiber wieder akzeptiert wird.

---

## 15. Integration in Secure Startup

Treiber, die während des Systemstarts geladen werden, werden in die Secure-Startup-Vertrauenskette einbezogen.

Dabei werden mindestens erfasst:

- Treiberpaket-Hash
- Herausgeber
- Signaturstatus
- Version
- Driver ABI
- Sicherheitsmodus
- Widerrufsstatus

Wenn die Plattform Messungen unterstützt, können diese Informationen zusätzlich an ein TPM oder anderes Trust Backend übergeben werden.

Ein TPM ist dafür nicht verpflichtend.

---

## 16. Treibercache

Erfolgreiche Prüfergebnisse dürfen zwischengespeichert werden, um wiederholte kryptografische Prüfungen zu beschleunigen.

Der Cache-Eintrag muss mindestens gebunden sein an:

- Paket-Hash
- Signatur-Hash
- Trust-Store-Version
- Widerrufsdaten-Version
- Sicherheitsrichtlinien-Version

Ändert sich einer dieser Werte, muss die vollständige Prüfung erneut erfolgen.

Ein Cache-Treffer darf niemals eine veränderte Treiberdatei autorisieren.

---

## 17. Updates und Rollback

Ein Treiberupdate wird als neues, vollständig signiertes Paket behandelt.

NovaOS muss:

- die neue Version vor Aktivierung prüfen,
- die bisherige Version für einen möglichen Rollback erhalten,
- Downgrade-Regeln beachten,
- widerrufene Vorgängerversionen nicht wiederherstellen,
- Treiber- und Gerätekompatibilität prüfen.

Ein automatischer Rollback ist nur auf eine weiterhin gültig signierte und nicht widerrufene Version zulässig.

---

## 18. API und Prüfergebnis

Das Driver Framework erhält ein strukturiertes Prüfergebnis:

```c
typedef struct driver_signature_result {
    uint32_t signature_status;
    uint32_t trust_class;
    uint32_t revocation_status;
    uint32_t policy_status;
    uint64_t trust_store_version;
    uint64_t revocation_version;
} driver_signature_result_t;
```

Mögliche Signaturzustände:

```c
#define DRIVER_SIGNATURE_VALID        1
#define DRIVER_SIGNATURE_INVALID      2
#define DRIVER_SIGNATURE_UNSIGNED     3
#define DRIVER_SIGNATURE_REVOKED      4
#define DRIVER_SIGNATURE_UNTRUSTED    5
#define DRIVER_SIGNATURE_EXPIRED      6
```

Das Driver Framework darf das Ergebnis nicht ohne erneute Sicherheitsprüfung überschreiben.

---

## 19. Sicherheit und Audit

Audit-Ereignisse werden erzeugt bei:

- ungültigen Treibersignaturen,
- Ladeversuchen widerrufener Treiber,
- Aktivierung des Entwicklungsmodus,
- Registrierung lokaler Vertrauensanker,
- Änderung des Driver Trust Store,
- Treiber-Downgrades,
- Verwendung von Test- oder Entwicklungsschlüsseln,
- erfolgreicher Aktivierung besonders privilegierter Kernel-Treiber.

Private Schlüssel und vollständige Zertifikatsgeheimnisse dürfen niemals im Audit erscheinen.

---

## 20. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Treiber ist gültig und freigegeben |
| `STATUS_DRIVER_UNSIGNED` | Keine Signatur vorhanden |
| `STATUS_DRIVER_SIGNATURE_INVALID` | Signaturprüfung fehlgeschlagen |
| `STATUS_DRIVER_UNTRUSTED` | Herausgeber ist nicht vertrauenswürdig |
| `STATUS_DRIVER_REVOKED` | Treiber oder Schlüssel wurde widerrufen |
| `STATUS_DRIVER_CERT_EXPIRED` | Zertifikat ist nicht mehr gültig |
| `STATUS_DRIVER_POLICY_DENIED` | Sicherheitsrichtlinie verbietet den Treiber |
| `STATUS_DRIVER_ABI_MISMATCH` | Driver ABI ist inkompatibel |
| `STATUS_DRIVER_PACKAGE_INVALID` | Paketstruktur oder Manifest ist ungültig |
| `STATUS_TIME_UNTRUSTED` | Zeitabhängige Prüfung ist nicht eindeutig möglich |

Bei uneindeutiger Sicherheitslage gilt für Kernel-Treiber grundsätzlich Fail Closed.

---

## 21. Alternative Ansätze

### 21.1 Keine verpflichtenden Signaturen

**Vorteile:**

- einfache Treiberentwicklung
- keine Schlüsselverwaltung

**Nachteile:**

- keine verifizierbare Herkunft
- leichte Manipulation von Treiberpaketen
- hohes Risiko für Kernel und Hardware

**Entscheidung:** Abgelehnt.

### 21.2 Ausschließlich NovaOS-signierte Treiber

**Vorteile:**

- sehr kontrollierte Vertrauenskette

**Nachteile:**

- zentrale Abhängigkeit
- erschwerte Entwicklung eigener Hardware
- ungeeignet für offene Treiberökosysteme

**Entscheidung:** Abgelehnt. Verifizierte Hersteller und lokale Entwicklungsschlüssel werden unterstützt.

### 21.3 Onlineprüfung bei jedem Laden

**Vorteile:**

- aktueller Widerrufsstatus

**Nachteile:**

- Systemstart wäre netzwerkabhängig
- Datenschutz- und Verfügbarkeitsprobleme

**Entscheidung:** Abgelehnt. Die Prüfung muss vollständig offline möglich sein.

---

## 22. Konsequenzen

### Positive Konsequenzen

- Schutz vor manipulierten Treiberpaketen
- überprüfbare Treiberherkunft
- widerrufbare kompromittierte Treiber
- sicherer Entwicklungsmodus
- Offline-Prüfbarkeit
- Integration in Secure Startup
- kontrollierte offene Treiberentwicklung

### Negative Konsequenzen

- zusätzliche Schlüssel- und Zertifikatsverwaltung
- Entwicklungsaufwand für Signierwerkzeuge
- fehlerhafte Signaturen verhindern das Laden
- Widerrufslisten müssen gepflegt werden

### Risiken

- Kompromittierung eines Signaturschlüssels
- veraltete Widerrufsdaten auf Offline-Systemen
- Verlust lokaler Entwicklungsschlüssel
- fehlerhafte Zeitprüfung
- Missbrauch dauerhaft aktivierter Entwicklungsmodi

---

## 23. Implementierungsrichtlinien

- Die Signatur muss das vollständige Treiberpaket abdecken.
- Paketserialisierung und Pfadnormalisierung müssen kanonisch sein.
- Unsichere Hash- und Signaturalgorithmen sind abzulehnen.
- Kernel-Treiber verwenden strengere Vertrauensrichtlinien als Userspace-Treiber.
- Widerruf hat Vorrang vor einem positiven Cache-Eintrag.
- Entwicklungsmodi müssen sichtbar und auditierbar sein.
- Private Schlüssel dürfen niemals im Treiberpaket enthalten sein.
- Trust-Store-Änderungen benötigen administrative Autorisierung.
- Die Prüfung muss ohne Netzwerkverbindung funktionieren.

---

## 24. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- gültige und ungültige Signaturen,
- veränderte Paketbestandteile,
- unbekannte Herausgeber,
- abgelaufene Zertifikate,
- widerrufene Schlüssel und Pakete,
- lokale Entwicklungssignaturen,
- Entwicklungsmodus,
- Offline-Prüfung,
- manipulierte Manifeste,
- Signaturcache-Invalidierung,
- Trust-Store-Updates,
- Rollback auf ältere Treiberversionen,
- inkompatible Driver ABIs,
- unsichere Systemzeit.

---

## 25. Verbindliche Invarianten

1. Produktiv geladene Treiber müssen gültig signiert sein.
2. Die Signatur umfasst alle sicherheitsrelevanten Paketbestandteile.
3. Ein widerrufener Treiber darf nicht neu geladen werden.
4. Ein Cache-Eintrag darf eine Widerrufsprüfung nicht umgehen.
5. Kernel-Treiber unterliegen strengeren Vertrauensregeln als Userspace-Treiber.
6. Lokale Entwicklungstreiber benötigen einen ausdrücklich aktivierten Entwicklungsmodus.
7. Der Entwicklungsmodus muss sichtbar und auditierbar sein.
8. TPM-Unterstützung bleibt optional.
9. Driver Signing muss offline funktionieren.
10. Eine gültige Signatur ersetzt keine Capability- und Ressourcenbegrenzung.

---

## 26. Zusammenfassung

NovaOS verlangt digitale Signaturen für alle produktiv verwendeten Treiberpakete.

Das Vertrauensmodell unterstützt offizielle NovaOS-Treiber, verifizierte Hersteller und lokal autorisierte Entwicklungstreiber. Widerruf, Offline-Prüfung und ein deutlich gekennzeichneter Entwicklungsmodus verbinden Systemsicherheit mit einer offenen Treiberentwicklung.

Eine gültige Signatur bestätigt Herkunft und Integrität, verleiht einem Treiber aber weiterhin nur die durch das Driver Framework ausdrücklich zugewiesenen Rechte.