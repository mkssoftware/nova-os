# NPSPEC-KERNEL-0020 – Security Manager

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0020 |
| Titel | Security Manager |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Sicherheit |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0001, NPSPEC-KERNEL-0002, NPSPEC-KERNEL-0004, NPSPEC-KERNEL-0011, NPSPEC-KERNEL-0012, NPSPEC-KERNEL-0013, NPSPEC-KERNEL-0015, NPSPEC-KERNEL-0017, NPSPEC-KERNEL-0019 |
| Zugehörige ADRs | ADR-SEC-0001, ADR-SEC-0002, ADR-SEC-0003, ADR-SEC-0004, ADR-SEC-0005, ADR-SEC-0006, ADR-SEC-0007, ADR-SEC-0008, ADR-SEC-0009, ADR-SEC-0010, ADR-KERNEL-0103 |

---

## 1. Zweck

Diese Spezifikation definiert den Security Manager des NovaOS-Kernels.

Der Security Manager verwaltet:

- Sicherheitsidentitäten,
- Capability-Domains,
- Zugriffsentscheidungen,
- Sicherheitsprofile und Sandboxen,
- ACL-Integration,
- Vertrauens- und Integritätsstufen,
- Image- und Treiberprüfung,
- Schlüssel- und Trust-Backends,
- Security Audit,
- Debug- und Recovery-Richtlinien,
- Sicherheitsstatus des Systems.

## 2. Ziele

Der Security Manager muss:

- das Prinzip minimaler Rechte erzwingen,
- objektbezogene Capabilities verwenden,
- Prozesse und Treiber voneinander isolieren,
- Rechte delegierbar, aber nicht selbstständig erweiterbar machen,
- ältere Hardware mit ehrlich ausgewiesenem Sicherheitsstatus unterstützen,
- optionales TPM ohne zwingende TPM-Abhängigkeit ermöglichen,
- Sicherheitsentscheidungen zentral und nachvollziehbar treffen,
- Datenschutz und lokale Verarbeitung als Standard berücksichtigen,
- Recovery ermöglichen, ohne Sicherheitsgrenzen stillschweigend aufzugeben.

## 3. Grundprinzipien

Für NovaOS gelten folgende Sicherheitsprinzipien:

1. Standardmäßig wird Zugriff verweigert.
2. Rechte werden für konkrete Objekte und Operationen vergeben.
3. Namen, Kennungen und Pfade verleihen keine Rechte.
4. Prozesse können ihre eigenen Privilegien nicht erhöhen.
5. Sicherheitsprüfungen erfolgen vor der Zustandsänderung.
6. Sicherheitsrelevante Eingaben gelten als nicht vertrauenswürdig.
7. Fehlende Hardwarefunktionen werden nicht als aktiv dargestellt.
8. Recovery bleibt verifiziert und von regulären Systemzuständen getrennt.
9. Telemetrie und Audit bleiben standardmäßig lokal.

## 4. Abgrenzung

| Komponente | Verantwortung |
|---|---|
| Security Manager | zentrale Sicherheitsentscheidungen und Richtlinien |
| Object Manager | Kernelobjekte und Objektbeziehungen |
| Handle Manager | konkrete Handle-Rechte |
| Process Manager | Prozess- und Identitätskontext |
| VFS | ACLs und dateispezifische Rechte |
| Driver Framework | isolierte Treiberdomains |
| Audit Subsystem | persistente Sicherheitsereignisse |
| Trust Backend | kryptografischer Vertrauensanker |
| Recovery Manager | verifizierte Wiederherstellungsumgebung |

## 5. Sicherheitsidentität

```c
typedef uint64_t np_identity_id_t;
```

Eine Sicherheitsidentität kann repräsentieren:

- Benutzer,
- Systemdienst,
- Anwendungspaket,
- Treiber,
- Gerät,
- Sitzung,
- Recovery-Komponente,
- System selbst.

Eine numerische Identitätskennung allein verleiht keine Rechte.

## 6. Identitätsobjekt

```c
typedef struct np_security_identity {
    np_kernel_object_t object;

    np_identity_id_t identity_id;
    np_identity_type_t type;

    np_string_t display_name;
    np_security_attributes_t attributes;

    uint32_t flags;
    uint32_t state;
} np_security_identity_t;
```

Anzeige- und Kontonamen sind nicht die primäre Sicherheitsidentität.

## 7. Identitätstypen

```c
typedef enum np_identity_type {
    NP_IDENTITY_SYSTEM,
    NP_IDENTITY_USER,
    NP_IDENTITY_SERVICE,
    NP_IDENTITY_APPLICATION,
    NP_IDENTITY_DRIVER,
    NP_IDENTITY_DEVICE,
    NP_IDENTITY_SESSION,
    NP_IDENTITY_RECOVERY
} np_identity_type_t;
```

Identitäten können Beziehungen zu Benutzerkonten oder Anwendungspaketen besitzen, bleiben jedoch eigenständige Sicherheitsobjekte.

## 8. Sicherheitskontext

Jeder Thread besitzt einen effektiven Sicherheitskontext.

```c
typedef struct np_security_context {
    np_security_identity_t* primary_identity;
    np_capability_domain_t* capability_domain;

    np_integrity_level_t integrity_level;
    np_trust_level_t trust_level;

    np_security_profile_t* profile;
    np_session_id_t session_id;

    uint64_t flags;
} np_security_context_t;
```

Threads eines Prozesses verwenden standardmäßig den Sicherheitskontext des Prozesses.

Temporäre Abweichungen müssen ausdrücklich autorisiert und begrenzt sein.

## 9. Vertrauensstufen

```c
typedef enum np_trust_level {
    NP_TRUST_UNTRUSTED,
    NP_TRUST_RESTRICTED,
    NP_TRUST_USER,
    NP_TRUST_VERIFIED,
    NP_TRUST_SYSTEM,
    NP_TRUST_RECOVERY,
    NP_TRUST_KERNEL
} np_trust_level_t;
```

Eine höhere Vertrauensstufe verleiht nicht automatisch Zugriff auf jedes Objekt.

Sie dient als zusätzliche Sicherheitsgrenze und Richtlinieninformation.

## 10. Integritätsstufen

```c
typedef enum np_integrity_level {
    NP_INTEGRITY_UNTRUSTED,
    NP_INTEGRITY_LOW,
    NP_INTEGRITY_MEDIUM,
    NP_INTEGRITY_HIGH,
    NP_INTEGRITY_SYSTEM,
    NP_INTEGRITY_KERNEL
} np_integrity_level_t;
```

Ein niedrigerer Integritätskontext darf ein Objekt höherer Integrität nicht verändern, sofern keine ausdrückliche Capability dies unter einer zulässigen Richtlinie erlaubt.

## 11. Capability

Eine Capability beschreibt ein Recht auf ein konkretes Objekt oder eine klar begrenzte Ressourcengruppe.

```c
typedef struct np_capability {
    np_kernel_object_t* target;
    np_capability_rights_t rights;

    np_capability_constraints_t constraints;
    uint64_t generation;
    uint32_t flags;
} np_capability_t;
```

Capabilities werden über geschützte Handles oder kernelinterne Referenzen genutzt.

## 12. Capability-Rechte

```c
typedef uint64_t np_capability_rights_t;
```

Allgemeine Rechte umfassen:

```c
typedef enum np_generic_rights {
    NP_RIGHT_QUERY       = 1ULL << 0,
    NP_RIGHT_READ        = 1ULL << 1,
    NP_RIGHT_WRITE       = 1ULL << 2,
    NP_RIGHT_EXECUTE     = 1ULL << 3,
    NP_RIGHT_MODIFY      = 1ULL << 4,
    NP_RIGHT_WAIT        = 1ULL << 5,
    NP_RIGHT_SIGNAL      = 1ULL << 6,
    NP_RIGHT_DELEGATE    = 1ULL << 7,
    NP_RIGHT_DELETE      = 1ULL << 8,
    NP_RIGHT_ADMIN       = 1ULL << 9
} np_generic_rights_t;
```

Objekttypen definieren zusätzliche spezialisierte Rechte.

## 13. Capability-Domain

Eine Capability-Domain begrenzt die erreichbaren Objekte und Rechte eines Prozesses oder Dienstes.

```c
typedef struct np_capability_domain {
    np_kernel_object_t object;

    np_capability_domain_t* parent;
    np_capability_set_t capabilities;

    np_security_policy_t* policy;
    uint64_t generation;
    uint32_t flags;

    np_rwlock_t lock;
} np_capability_domain_t;
```

Eine Domain kann Kind-Domains mit gleichen oder geringeren Rechten erzeugen.

## 14. Capability-Erzeugung

Eine neue Capability kann entstehen durch:

- initiale Prozessrichtlinie,
- autorisierte Delegation,
- Öffnen eines Objekts,
- IPC-Handle-Übertragung,
- Treiberbindung,
- Dienstregistrierung,
- kontrollierte administrative Vergabe.

Es gilt:

```text
delegated_rights ⊆ delegator_rights
```

Eine normale Delegation darf keine neuen Rechte erzeugen.

## 15. Capability-Reduktion

Capabilities und Handles dürfen dauerhaft eingeschränkt werden.

```c
np_status_t np_capability_restrict(
    np_handle_t handle,
    np_capability_rights_t reduced_rights,
    const np_capability_constraints_t* constraints
);
```

Rechte und Einschränkungen dürfen ohne neue Autorisierung nur restriktiver werden.

## 16. Capability-Constraints

Eine Capability kann zusätzlich begrenzt werden durch:

- Ablaufzeit,
- Sitzung,
- Prozess oder Job,
- Namespace,
- Geräte- oder Dateisystem-Subtree,
- zulässige Operationen,
- maximale Datenmenge,
- CPU- oder Ressourcenbudget,
- aktive Systemphase.

Abgelaufene oder nicht mehr passende Capabilities werden abgelehnt.

## 17. Capability-Widerruf

Bestimmte Capabilities müssen widerrufbar sein.

```c
np_status_t np_capability_revoke(
    np_capability_handle_t capability,
    uint32_t flags
);
```

Ein Widerruf kann betreffen:

- eine einzelne Capability,
- abgeleitete Capabilities,
- alle Capabilities einer Domain,
- Geräte- oder Treiberressourcen,
- zeitlich begrenzte Sitzungsrechte.

Der Widerruf muss gegen laufende Operationen eindeutig definiert sein.

## 18. Sicherheitsentscheidung

```c
typedef struct np_access_request {
    np_security_context_t* subject;
    np_kernel_object_t* object;

    np_capability_rights_t requested_rights;
    uint32_t operation;
    uint32_t flags;

    np_security_metadata_t metadata;
} np_access_request_t;
```

```c
np_status_t np_security_check_access(
    const np_access_request_t* request,
    np_access_decision_t* decision
);
```

Die Entscheidung wird vor der geschützten Operation getroffen.

## 19. Entscheidungsreihenfolge

Eine Zugriffsentscheidung berücksichtigt mindestens:

1. gültigen Sicherheitskontext,
2. Zielobjekt und Objekttyp,
3. Handle- und Capability-Rechte,
4. Capability-Constraints,
5. Integritätsstufe,
6. Sicherheitsprofil,
7. Namespace- und Sandboxgrenzen,
8. ACLs oder Objektpolicy,
9. Job- und Systemrichtlinien,
10. aktuellen Sicherheitsmodus.

Eine zwingende Verweigerungsregel hat Vorrang vor einer normalen Erlaubnis.

## 20. Default-Deny

Existiert keine ausdrückliche Erlaubnis, wird der Zugriff verweigert.

Unbekannte:

- Rechte,
- Objekttypen,
- Policyfelder,
- Sicherheitsprofile,
- Signaturalgorithmen,
- Trust-Zustände

dürfen nicht automatisch als zulässig behandelt werden.

## 21. Sicherheitsprofile

```c
typedef enum np_security_profile_class {
    NP_PROFILE_KERNEL,
    NP_PROFILE_SYSTEM_SERVICE,
    NP_PROFILE_USER_APPLICATION,
    NP_PROFILE_SANDBOXED_APPLICATION,
    NP_PROFILE_USERSPACE_DRIVER,
    NP_PROFILE_INSTALLER,
    NP_PROFILE_RECOVERY,
    NP_PROFILE_DEVELOPMENT
} np_security_profile_class_t;
```

Ein Profil definiert die maximal zulässigen Rechte und Ressourcen einer Komponente.

## 22. Anwendungs-Sandbox

Eine sandboxed Application erhält standardmäßig nur:

- eigene Paketdateien,
- eigenen Datenbereich,
- ausdrücklich ausgewählte Benutzerdateien,
- freigegebene IPC-Dienste,
- notwendige UI- und Eingabedienste,
- ausdrücklich genehmigte Geräte- und Netzwerkrechte.

Der Zugriff auf Kamera, Mikrofon, Standort, Kontakte oder ähnliche Ressourcen erfordert separate Berechtigungen.

## 23. Systemdienste

Systemdienste erhalten ein fest definiertes Dienstprofil.

Die Kennzeichnung als Systemdienst verleiht keine uneingeschränkte Root- oder Kernelberechtigung.

Jeder Dienst erhält:

- eigene Identität,
- eigene Capability-Domain,
- Ressourcenlimits,
- definierte IPC-Endpunkte,
- begrenzte VFS- und Gerätezugriffe,
- eine Recovery-Richtlinie.

## 24. Userspace-Treiber

Userspace-Treiber erhalten ausschließlich:

- das zugewiesene Geräteobjekt,
- benötigte MMIO-Objekte,
- Interrupt-Endpunkte,
- DMA-Domain,
- erforderliche Systemdienste,
- begrenzte Diagnosefunktionen.

Bei einem Treiberabsturz oder Geräteentfernen werden diese Rechte widerrufen.

## 25. Kernel-Module

Kernelmodule werden vor dem Laden geprüft auf:

- Signatur und Integrität,
- kompatible Kernel-ABI,
- zugelassenen Herausgeber,
- Sicherheitsprofil,
- benötigte Kernelservices,
- bekannte Sperrlisten,
- Debug- oder Entwicklungsstatus.

Ein geladenes Kernelmodul erhält keinen Userspace-artigen Vollzugriffsschlüssel, sondern wird durch interne API- und Subsystemgrenzen eingeschränkt.

## 26. Image-Verifikation

Ausführbare Images können geprüft werden anhand von:

- kryptografischem Hash,
- Signatur,
- Paketmanifest,
- Herausgeber,
- Systemgeneration,
- Sicherheitsprofil,
- Sperrliste,
- erlaubter ABI.

Das Ergebnis wird als verifizierter Sicherheitsstatus am Prozessobjekt gespeichert.

## 27. Secure Kernel Startup

Während des Bootvorgangs übernimmt der Security Manager:

- Boot-Vertrauensstatus,
- Kernel-Image-Verifikation,
- Systemgenerationsstatus,
- Bootloader- und Handoff-Status,
- verfügbare Hardware-Sicherheitsfunktionen,
- Recovery-Verifikation.

Ein unsicherer Zustand darf nicht als vollständig verifiziert dargestellt werden.

## 28. Trust Backend

NovaOS verwendet eine abstrakte Trust-Backend-Schnittstelle.

```c
typedef struct np_trust_backend_operations {
    np_status_t (*verify)(
        const np_trust_verification_request_t* request,
        np_trust_result_t* result
    );

    np_status_t (*seal)(
        const np_secure_blob_t* input,
        np_secure_blob_t* output
    );

    np_status_t (*unseal)(
        const np_secure_blob_t* input,
        np_secure_blob_t* output
    );

    np_status_t (*attest)(
        const np_attestation_request_t* request,
        np_attestation_result_t* result
    );
} np_trust_backend_operations_t;
```

## 29. Trust-Backend-Arten

Unterstützte Backends können sein:

- TPM 2.0,
- Firmware Secure Boot,
- CPU- oder Plattform-Sicherheitsmodul,
- verschlüsselter softwarebasierter Keystore,
- Recovery-Trust-Store,
- externer Unternehmens-Trust-Provider.

Ein TPM ist optional und nicht zwingende Voraussetzung für NovaOS.

## 30. Sicherheitsstufen des Systems

```c
typedef enum np_system_security_level {
    NP_SECURITY_UNVERIFIED,
    NP_SECURITY_DEGRADED,
    NP_SECURITY_BASELINE,
    NP_SECURITY_HARDENED,
    NP_SECURITY_VERIFIED
} np_system_security_level_t;
```

Die Stufe wird aus tatsächlich aktiven Eigenschaften berechnet.

Beispiele sind:

- NX,
- Stack Protector,
- Kernel ASLR,
- SMEP/SMAP oder Entsprechungen,
- IOMMU,
- verifizierter Bootpfad,
- Trust Backend,
- Debugstatus.

## 31. Degraded Security

Ältere Hardware darf in einem reduzierten Sicherheitsmodus starten, wenn zwingende Mindestbedingungen erfüllt sind.

Dabei muss NovaOS:

- fehlende Schutzfunktionen benennen,
- die effektive Sicherheitsstufe reduzieren,
- riskante Funktionen gegebenenfalls deaktivieren,
- lokale Diagnoseinformationen bereitstellen,
- keine nicht vorhandene Absicherung vortäuschen.

Sicherheitskritische Unternehmensprofile können den Start im Degraded Mode verweigern.

## 32. Speicherschutz

Der Security Manager verwaltet Richtlinien für:

- NX Memory,
- W^X,
- Kernel ASLR,
- Userspace ASLR,
- Stack Guard Pages,
- Stack Protector,
- Kernel-Userspace-Isolation,
- geschützte Kernel-Daten,
- sensible Speicherlöschung.

Ausnahmen benötigen eine explizite, begrenzte und auditierbare Autorisierung.

## 33. Geräte- und DMA-Schutz

Gerätezugriffe unterliegen:

- Device Capabilities,
- MMIO-Bereichsbegrenzung,
- Interrupt-Zuordnung,
- DMA-Domain,
- IOMMU-Policy,
- Treiberprofil,
- Geräteklasse und Benutzerfreigabe.

Ohne IOMMU müssen riskante DMA-Geräte eingeschränkt, isoliert oder im Sicherheitsstatus klar ausgewiesen werden.

## 34. VFS-Sicherheit

Dateizugriffe berücksichtigen:

- File-Handle-Rechte,
- Capability-Domain,
- ACLs,
- Mount-Flags,
- Prozess-Root,
- Mount Namespace,
- Dateiattribute,
- Integritäts- und Signaturstatus,
- Sandboxrichtlinien.

Pfadnamen ersetzen keine objektbezogene Rechteprüfung.

## 35. IPC-Sicherheit

IPC-Kommunikation erfordert:

- gültige Endpunkthandles,
- Send- oder Receive-Rechte,
- erlaubte Handle-Delegation,
- Queue- und Ressourcenlimits,
- Protokoll- und Strukturvalidierung,
- Sicherheitsdomain-Prüfung.

Peer-Identitäten werden aus Kernelobjekten und nicht aus selbst gemeldeten Nachrichtendaten bestimmt.

## 36. Netzwerk-Sicherheit

Netzwerkrechte werden getrennt vergeben für:

- lokale Kommunikation,
- ausgehende Verbindungen,
- eingehende Listener,
- Raw Sockets,
- Interface-Konfiguration,
- Routing und Firewall,
- VPN und Tunnel,
- Paketfilter.

Ein allgemeines Netzwerkrecht soll vermieden werden, wenn eine feinere Einschränkung möglich ist.

## 37. Debug-Sicherheit

Debugging ist standardmäßig eingeschränkt.

Debugzugriff erfordert:

- `CAP_DEBUG`,
- Recht auf das konkrete Zielobjekt,
- kompatiblen Sicherheits- und Integritätskontext,
- aktiven Secure-Debugging-Modus,
- gegebenenfalls Benutzer- oder Administratorfreigabe.

Kerneldebugging und Zugriff auf geschützte Systemprozesse benötigen zusätzliche Rechte.

## 38. Secure Debugging

Der Secure-Debugging-Status kann sein:

```c
typedef enum np_secure_debug_state {
    NP_DEBUG_DISABLED,
    NP_DEBUG_USER_ONLY,
    NP_DEBUG_AUTHORIZED,
    NP_DEBUG_KERNEL_AUTHORIZED,
    NP_DEBUG_DEVELOPMENT
} np_secure_debug_state_t;
```

Ein Wechsel in einen weniger sicheren Debugmodus wird auditiert und kann einen Neustart oder physischen Bestätigungsschritt erfordern.

## 39. Schlüsselverwaltung

Kryptografisches Schlüsselmaterial wird als geschütztes Kernelobjekt verwaltet.

```c
typedef struct np_key_object {
    np_kernel_object_t object;

    np_key_type_t type;
    np_key_usage_t usage;

    uint32_t flags;
    np_secure_memory_t material;
} np_key_object_t;
```

Schlüsselmaterial wird nicht über normale Query- oder Diagnose-APIs ausgegeben.

## 40. Schlüsselrechte

Rechte werden getrennt für folgende Operationen vergeben:

- Verwenden,
- Signieren,
- Verifizieren,
- Verschlüsseln,
- Entschlüsseln,
- Ableiten,
- Exportieren,
- Löschen,
- Verwalten.

Ein verwendbarer Schlüssel muss nicht exportierbar sein.

Private Schlüssel sind standardmäßig nicht exportierbar.

## 41. Sicherer Speicher

Sensible Daten werden in speziell markiertem Speicher gehalten.

Dieser Speicher:

- ist nicht ausführbar,
- wird nach Gebrauch sicher gelöscht,
- kann gegen Swap oder Dump gesperrt sein,
- wird in Crash Dumps standardmäßig ausgeschlossen,
- besitzt begrenzte Diagnosezugriffe.

Die physische Seitenwiederverwendung erfolgt erst nach sicherer Behandlung.

## 42. Zufallszahlen

Der Security Manager nutzt einen zentralen kryptografisch geeigneten Zufallszahlengenerator.

```c
np_status_t np_secure_random(
    void* output,
    size_t output_size
);
```

Die Initialisierung kombiniert verfügbare Entropiequellen.

Vor ausreichender Initialisierung dürfen keine langfristigen kryptografischen Schlüssel erzeugt werden.

Ein schwacher Rückfallwert darf nicht als kryptografisch sicher ausgegeben werden.

## 43. Policy-Modell

Sicherheitsrichtlinien werden als versionierte, verifizierte Policy-Objekte verwaltet.

Sie können definieren:

- erlaubte Capabilities,
- Integritätsgrenzen,
- Prozessprofile,
- Treiberregeln,
- Mount- und Dateiregeln,
- Netzwerkzugriffe,
- Debugstatus,
- Sicherheitsminimum,
- Auditverhalten.

Unbekannte zwingende Policyfelder führen zur Ablehnung.

## 44. Policy-Aktualisierung

Eine Policy-Aktualisierung erfolgt transaktional.

```text
Neue Policy laden
  -> Signatur und Version prüfen
  -> Syntax und Semantik validieren
  -> Auswirkungen analysieren
  -> neue Policy atomar aktivieren
  -> abhängige Domains aktualisieren
  -> bei Fehler alte Policy beibehalten
```

Rechteerweiterungen können zusätzliche Benutzer- oder Administratorbestätigung erfordern.

## 45. Policy-Vererbung

Kind-Domains und Sandboxes können eine Elternpolicy erben und weiter einschränken.

Sie dürfen zwingende Elternregeln nicht lockern.

Es gilt:

```text
effective_policy =
    system_policy
    ∩ parent_policy
    ∩ profile_policy
    ∩ object_policy
```

## 46. ACL-Integration

ACLs ergänzen, aber ersetzen nicht das Capability-System.

Eine ACL kann Zugriff erlauben oder verweigern, wenn der Aufrufer das Objekt über einen zulässigen Namespace- oder Open-Pfad erreicht.

Ein bereits erzeugtes Handle besitzt die beim Öffnen gewährten Rechte. Spätere ACL-Änderungen können abhängig von der Objektpolicy bestehende Handles erhalten oder widerrufen.

## 47. Widerrufsmodell

NovaOS unterstützt mehrere Widerrufsarten:

| Art | Wirkung |
|---|---|
| Future Access | verhindert neue Handles |
| Handle Revocation | macht konkrete Handles ungültig |
| Domain Revocation | widerruft Rechte einer Capability-Domain |
| Resource Revocation | entzieht Geräte-, MMIO- oder DMA-Rechte |
| Session Revocation | beendet sitzungsgebundene Rechte |
| Emergency Revocation | sofortige Sicherheitsisolierung |

Laufende Operationen müssen entsprechend ihrer Widerrufsklasse beendet oder kontrolliert abgeschlossen werden.

## 48. Sicherheitsereignisse

Der Security Manager erzeugt strukturierte Ereignisse:

```text
SECURITY_ACCESS_DENIED
SECURITY_CAPABILITY_GRANTED
SECURITY_CAPABILITY_REVOKED
SECURITY_POLICY_CHANGED
SECURITY_DRIVER_REJECTED
SECURITY_IMAGE_VERIFICATION_FAILED
SECURITY_DEBUG_STATE_CHANGED
SECURITY_TRUST_BACKEND_CHANGED
SECURITY_INTEGRITY_VIOLATION
SECURITY_LEVEL_CHANGED
```

Ereignisse enthalten nur die für den Empfänger zulässigen Informationen.

## 49. Audit

Auditpflichtige Operationen umfassen mindestens:

- administrative Rechtevergaben,
- Capability-Delegation zwischen Sicherheitsdomains,
- Debugzugriffe,
- Richtlinienänderungen,
- Laden von Kernelmodulen und Treibern,
- Veränderung von Trust Stores,
- wiederholte Zugriffsverletzungen,
- Deaktivierung von Sicherheitsfunktionen,
- Start im Degraded Mode,
- Recovery-Aktivierung.

Audit ist manipulationsgeschützt und standardmäßig lokal.

## 50. Audit-Datenschutz

Auditdaten dürfen nicht unnötig enthalten:

- Passwörter,
- Schlüsselmaterial,
- vollständige IPC-Payloads,
- vollständige Datei- oder Speicherinhalte,
- unnötige personenbezogene Inhalte.

Identitäten, Objekte und Ereignisse sollen über stabile interne Kennungen referenziert werden.

Zugriff und Aufbewahrung unterliegen eigenen Capabilities und Richtlinien.

## 51. Recovery-Sicherheit

Die Recovery-Umgebung muss:

- verifiziert sein,
- getrennte Capability-Domains verwenden,
- nur erforderliche Geräte und Dateisysteme einbinden,
- reguläre Benutzerdaten nicht unkontrolliert freigeben,
- Reparaturen auditiert durchführen,
- Rollback und Wiederherstellung unterstützen.

Recovery ist kein uneingeschränkter Debug- oder Root-Modus.

## 52. Kernel-Panic-Integration

Bei einer Sicherheitsverletzung im Kernel entscheidet die Panic Policy abhängig von:

- Integritätsauswirkung,
- betroffener Komponente,
- Möglichkeit lokaler Isolation,
- Debugstatus,
- Recovery-Verfügbarkeit,
- Systemprofil.

Eine mögliche Kompromittierung kritischer Kernelstrukturen darf nicht durch unkontrollierte Fortsetzung ignoriert werden.

## 53. Sicherheits-API

```c
np_status_t np_security_query_context(
    np_process_handle_t process,
    np_security_context_info_t* information
);

np_status_t np_security_check_handle(
    np_handle_t handle,
    np_capability_rights_t required_rights
);

np_status_t np_security_create_domain(
    const np_security_domain_create_info_t* create_info,
    np_handle_t* domain
);

np_status_t np_security_delegate(
    const np_capability_delegation_t* delegation,
    np_handle_t* delegated_handle
);

np_status_t np_security_revoke(
    np_handle_t capability,
    uint32_t flags
);
```

Alle Strukturen sind über Größe und ABI-Version erweiterbar.

## 54. Administrative Capabilities

Mindestens folgende administrative Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_SECURITY_QUERY` | eigenen oder freigegebenen Sicherheitsstatus lesen |
| `CAP_SECURITY_DELEGATE` | Rechte kontrolliert delegieren |
| `CAP_SECURITY_REVOKE` | delegierte Rechte widerrufen |
| `CAP_SECURITY_POLICY` | Sicherheitsrichtlinien verwalten |
| `CAP_SECURITY_IDENTITY` | Identitätsobjekte verwalten |
| `CAP_SECURITY_KEYS` | Schlüsselobjekte verwalten |
| `CAP_SECURITY_AUDIT` | geschützte Auditereignisse lesen |
| `CAP_SECURITY_DEBUG` | Secure-Debugging-Rechte verwalten |
| `CAP_SECURITY_TRUST` | Trust Stores und Backends verwalten |
| `CAP_SECURITY_ADMIN` | globale Sicherheitsverwaltung |

Auch `CAP_SECURITY_ADMIN` darf nicht automatisch direkte Kernelcode-Ausführung erlauben.

## 55. Sicherheit des Security Managers

Der Security Manager selbst muss:

- früh während des Bootvorgangs initialisiert werden,
- minimale Abhängigkeiten besitzen,
- kritische Policies in schreibgeschütztem Speicher halten,
- keine unsicheren Userspace-Callbacks ausführen,
- Notfallpfade ohne reguläre Heapallokation unterstützen,
- Policy- und Capability-Strukturen gegen Race Conditions schützen,
- eigene Integritätsverletzungen als kritisch behandeln.

## 56. Diagnose

Autorisierte Diagnosedienste können abrufen:

- aktive System-Sicherheitsstufe,
- verfügbare und fehlende Schutzfunktionen,
- Trust-Backend-Status,
- Anzahl Capability-Domains,
- Zugriffsverweigerungen,
- Policy-Versionen,
- Signaturfehler,
- Revocation-Ereignisse,
- Debugstatus,
- Degraded-Security-Gründe.

Schlüssel, Tokens und vollständige Policy-Geheimnisse werden nicht ausgegeben.

## 57. Statistik

```c
typedef struct np_security_statistics {
    uint64_t access_checks;
    uint64_t access_grants;
    uint64_t access_denials;

    uint64_t capability_delegations;
    uint64_t capability_revocations;
    uint64_t policy_updates;

    uint64_t image_verifications;
    uint64_t verification_failures;
    uint64_t driver_rejections;

    uint64_t audit_events;
    uint64_t integrity_violations;
} np_security_statistics_t;
```

Statistiken sollen pro Sicherheitsdomain, Prozess und System abrufbar sein.

## 58. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_ACCESS_DENIED` | Zugriff wurde verweigert |
| `NP_ERR_INVALID_CAPABILITY` | Capability ist ungültig |
| `NP_ERR_CAPABILITY_EXPIRED` | Capability ist abgelaufen |
| `NP_ERR_CAPABILITY_REVOKED` | Capability wurde widerrufen |
| `NP_ERR_RIGHTS_ESCALATION` | unzulässige Rechteerhöhung |
| `NP_ERR_IDENTITY_INVALID` | Sicherheitsidentität ist ungültig |
| `NP_ERR_POLICY_REJECTED` | Policy ist ungültig oder nicht zulässig |
| `NP_ERR_INTEGRITY_LEVEL` | Integritätsgrenze wurde verletzt |
| `NP_ERR_TRUST_FAILURE` | Vertrauensprüfung fehlgeschlagen |
| `NP_ERR_SIGNATURE_INVALID` | Signatur ist ungültig |
| `NP_ERR_SECURITY_DEGRADED` | Operation im reduzierten Modus nicht zulässig |
| `NP_ERR_DEBUG_DISABLED` | Debugging ist deaktiviert |
| `NP_ERR_NOT_SUPPORTED` | Funktion wird nicht unterstützt |

## 59. Fehlerbehandlung

Normale Zugriffsverletzungen werden abgelehnt und gegebenenfalls auditiert.

Mögliche Reaktionen auf Sicherheitsfehler sind:

- Operation verweigern,
- Handle oder Capability widerrufen,
- Thread oder Prozess beenden,
- Treiber isolieren,
- Gerät sperren,
- Sicherheitsdomain deaktivieren,
- in den Degraded Mode wechseln,
- Recovery aktivieren,
- Kernel-Panic-Policy anwenden.

Die Reaktion richtet sich nach Auswirkung und Sicherheitsprofil.

## 60. Kritische Sicherheitsverletzungen

Als kritisch gelten insbesondere:

- Manipulation zentraler Capability-Strukturen,
- unautorisierte Kernelcode-Ausführung,
- Umgehung der Kernel-Userspace-Isolation,
- beschädigte Trust- oder Policy-Daten,
- Rechteausweitung ohne autorisierte Quelle,
- unkontrollierter DMA-Zugriff auf Kernelspeicher,
- Offenlegung privater Schlüssel,
- Manipulation des verifizierten Recovery-Systems.

Eine Fortsetzung ist nur zulässig, wenn die betroffene Domain sicher isoliert werden kann.

## 61. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Erzeugung einer Sicherheitsidentität,
2. Erzeugung einer Capability-Domain,
3. Zugriff mit gültiger Capability,
4. Default-Deny ohne Capability,
5. Zugriff mit unzureichenden Rechten,
6. Capability-Delegation mit reduzierten Rechten,
7. Ablehnung einer Rechteerhöhung,
8. Capability-Constraint nach Zeit,
9. Capability-Constraint nach Namespace,
10. Widerruf eines Handles,
11. Widerruf abgeleiteter Capabilities,
12. Domain-weites Revocation,
13. Integritätsgrenzen,
14. Sicherheitsprofil einer Anwendung,
15. Sandbox-Zugriff auf Benutzerdateien,
16. Systemdienst mit minimalen Rechten,
17. isolierter Userspace-Treiber,
18. Image-Verifikation,
19. Ablehnung eines manipulierten Images,
20. Treibersignaturprüfung,
21. Trust Backend mit TPM,
22. Trust Backend ohne TPM,
23. Start im Degraded Mode,
24. korrekte Sicherheitsstufenanzeige,
25. W^X-Policy,
26. MMIO- und DMA-Begrenzung,
27. VFS-ACL-Integration,
28. sichere IPC-Handle-Delegation,
29. Secure-Debugging-Prüfung,
30. Änderung des Debugstatus,
31. Schlüsselobjekt ohne Exportrecht,
32. sichere Löschung von Schlüsselmaterial,
33. Policy-Aktualisierung,
34. Rollback einer ungültigen Policy,
35. Policy-Vererbung,
36. Sicherheits-Audit,
37. Datenschutzfilter im Audit,
38. Recovery-Sicherheitsprofil,
39. parallele Zugriffsentscheidungen auf mehreren CPUs,
40. Verhalten bei kritischer Integritätsverletzung.

## 62. Verbindliche Invarianten

1. Zugriff wird ohne ausdrückliche Erlaubnis verweigert.
2. Namen, Pfade, IDs und Handlewerte verleihen allein keine Rechte.
3. Eine normale Delegation erzeugt keine zusätzlichen Rechte.
4. Kind-Domains besitzen höchstens die Rechte ihrer Quelle.
5. Prozesse können ihre Vertrauens- oder Integritätsstufe nicht selbst erhöhen.
6. Jede privilegierte Operation wird vor ihrer Ausführung geprüft.
7. Widerrufene Capabilities werden nicht für neue Operationen akzeptiert.
8. Private Schlüssel werden nicht über normale APIs exportiert.
9. Fehlende Hardware-Sicherheitsfunktionen werden im Status korrekt ausgewiesen.
10. Debugging geschützter Ziele benötigt ausdrückliche Secure-Debugging-Autorisierung.
11. Sicherheitsrelevante Diagnose und Audit bleiben standardmäßig lokal.
12. Eine kritische Verletzung zentraler Kernelintegrität wird nicht unkontrolliert ignoriert.

## 63. Referenzablauf einer Zugriffsprüfung

```text
Kernel-Service erhält Objektoperation
  -> aktuellen Thread- und Prozesskontext bestimmen
  -> Handle und Zielobjekt auflösen
  -> angeforderte Rechte bestimmen
  -> Capability-Rechte und Constraints prüfen
  -> Integritäts- und Vertrauensgrenzen prüfen
  -> Sicherheitsprofil und Sandbox anwenden
  -> ACL-, Namespace-, Job- und Systempolicy prüfen
  -> Erlaubnis oder Verweigerung erzeugen
  -> sicherheitsrelevantes Ergebnis gegebenenfalls auditieren
  -> Operation ausführen oder mit Fehler beenden
```

## 64. Zusammenfassung

Der NovaOS Security Manager verbindet Identitäten, Capability-Domains, Integritätsstufen, Sicherheitsprofile, ACLs und systemweite Policies zu einem zentralen Zugriffsmodell.

Rechte werden für konkrete Objekte vergeben und können kontrolliert reduziert, delegiert oder widerrufen werden. Anwendungen, Systemdienste und Userspace-Treiber erhalten eigene isolierte Domains mit minimal erforderlichen Capabilities.

Optionale Trust-Backends, ehrliche Degraded-Security-Anzeige, Secure Debugging, geschützte Schlüsselobjekte und lokales Audit ermöglichen starke Sicherheit auch auf Hardware ohne TPM, ohne nicht vorhandene Schutzfunktionen vorzutäuschen.