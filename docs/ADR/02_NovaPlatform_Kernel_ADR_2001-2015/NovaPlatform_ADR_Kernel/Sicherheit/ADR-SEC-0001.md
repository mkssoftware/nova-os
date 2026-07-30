# ADR-SEC-0001 – Secure Kernel Startup

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-SEC-0001 |
| Titel | Secure Kernel Startup |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-30 |
| Bereich | Sicherheit / Boot / Kernel / Vertrauenskette |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-BOOT-0001, ADR-BOOT-1012, ADR-KERNEL-0103, ADR-KERNEL-0105 |
| Nachfolgende Spezifikation | NPSPEC-SECURESTARTUP-0001 |

---

## 1. Kontext

Der NovaOS-Kernel bildet die zentrale Vertrauensbasis des Betriebssystems.

Wird vor oder während des Kernelstarts manipulierte Software ausgeführt, können sämtliche späteren Sicherheitsmechanismen umgangen werden.

Dies betrifft unter anderem:

- Capability-Prüfungen,
- Prozessisolation,
- Speicherisolation,
- Dateisystemschutz,
- Treibersicherheit,
- lokale Telemetrie,
- Verschlüsselung,
- Benutzerrechte,
- Recovery-Funktionen,
- Integritätsprüfungen.

Ein Angreifer könnte versuchen:

- den Bootloader zu verändern,
- einen manipulierten Kernel zu laden,
- Kernelmodule auszutauschen,
- Bootparameter zu manipulieren,
- eine ältere verwundbare Systemversion zu starten,
- die Boot-Handoff-Daten zu verändern,
- einen unsicheren Recovery-Pfad zu verwenden,
- Sicherheitsprüfungen über BIOS-, UEFI- oder Firmwareunterschiede zu umgehen.

NovaOS unterstützt gemäß ADR-BOOT-0001 sowohl BIOS als auch UEFI. Die Sicherheitsarchitektur darf deshalb nicht ausschließlich von UEFI Secure Boot abhängen.

Gleichzeitig soll NovaOS auch auf älteren Rechnern funktionieren, die:

- kein UEFI Secure Boot,
- kein TPM,
- keine moderne Firmware-Vertrauenskette,
- keine Hardware-Root-of-Trust

besitzen.

Secure Kernel Startup muss daher mehrere Vertrauensstufen unterstützen und klar kennzeichnen, welche Sicherheitsgarantien auf der jeweiligen Hardware tatsächlich verfügbar sind.

---

## 2. Problemstellung

NovaOS benötigt einen sicheren und nachvollziehbaren Startpfad vom Bootloader bis zum vollständig initialisierten Kernel.

Dabei müssen folgende Probleme gelöst werden:

- BIOS besitzt keine standardisierte Secure-Boot-Kette.
- UEFI Secure Boot kann verfügbar, deaktiviert oder fehlerhaft konfiguriert sein.
- TPM-Hardware ist nicht auf allen Zielgeräten vorhanden.
- Boot-Handoff-Daten können manipuliert oder beschädigt sein.
- Kernel, Module und Startkonfiguration müssen gemeinsam authentifiziert werden.
- Signaturprüfung allein verhindert keinen Rollback auf verwundbare Versionen.
- Recovery muss auch bei beschädigtem Hauptsystem funktionieren.
- Entwicklungs-Builds benötigen kontrollierte Ausnahmen.
- Ein Sicherheitsfehler darf nicht unbemerkt in einen normalen Boot übergehen.
- Vertrauensentscheidungen müssen für spätere Systemdienste nachvollziehbar sein.

---

## 3. Entscheidungsziele

Secure Kernel Startup soll:

1. den Kernel vor der Ausführung kryptografisch verifizieren,
2. Bootkomponenten in eine durchgängige Vertrauenskette einbeziehen,
3. BIOS und UEFI unterstützen,
4. ohne verpflichtendes TPM funktionieren,
5. vorhandene Hardware-Vertrauensanker nutzen,
6. Boot-Handoff-Daten validieren,
7. Rollback-Angriffe begrenzen,
8. kontrollierte Recovery ermöglichen,
9. Entwicklungs- und Produktionsmodi trennen,
10. den erreichten Vertrauenszustand an den Kernel übergeben,
11. Sicherheitsfehler eindeutig anzeigen und protokollieren,
12. keine stärkeren Garantien behaupten, als tatsächlich vorhanden sind.

---

## 4. Begriffsdefinitionen

### 4.1 Secure Kernel Startup

Secure Kernel Startup bezeichnet den verifizierten und kontrollierten Übergang vom NovaOS-Bootloader zur Ausführung eines freigegebenen Kernels.

---

### 4.2 Chain of Trust

Eine Chain of Trust ist eine Vertrauenskette, in der jede Stufe die Authentizität und Integrität der nächsten Stufe überprüft, bevor diese ausgeführt wird.

---

### 4.3 Root of Trust

Eine Root of Trust ist der Ausgangspunkt einer Vertrauenskette.

Mögliche Vertrauensanker sind:

- Firmware-Schlüssel,
- eingebetteter NovaOS-Root-Key,
- TPM,
- Hardware-Sicherheitsmodul,
- schreibgeschützter Bootbereich,
- manuell bestätigter Schlüssel-Fingerprint.

---

### 4.4 Verified Boot

Verified Boot bedeutet, dass eine Komponente nur ausgeführt wird, wenn ihre kryptografische Signatur und Integrität erfolgreich geprüft wurden.

---

### 4.5 Measured Boot

Measured Boot bedeutet, dass Hashwerte gestarteter Komponenten in ein vertrauenswürdiges Messsystem, beispielsweise ein TPM, übertragen werden.

Measured Boot verhindert die Ausführung nicht automatisch.

---

### 4.6 Boot Manifest

Das Boot Manifest ist eine signierte Beschreibung aller für den Kernelstart benötigten Artefakte, Versionen, Hashwerte, Richtlinien und Abhängigkeiten.

---

### 4.7 Trust State

Der Trust State beschreibt, welche Prüfungen durchgeführt wurden und welche Vertrauensgarantien beim aktuellen Start gelten.

---

### 4.8 Rollback Protection

Rollback Protection verhindert oder kennzeichnet den Start einer älteren Systemversion, die unterhalb einer festgelegten Sicherheitsversion liegt.

---

## 5. Bedrohungsmodell

Secure Kernel Startup schützt insbesondere gegen:

- manipulierte Kerneldateien,
- manipulierte Kernelmodule,
- veränderte Startkonfigurationen,
- nicht autorisierte Bootparameter,
- beschädigte Bootartefakte,
- manipulierte Recovery-Images,
- Rollback auf gesperrte Versionen,
- versehentliche Ausführung unfertiger Builds,
- Austausch von Artefakten zwischen Systemgenerationen.

Secure Kernel Startup schützt nicht vollständig gegen:

- kompromittierte CPU oder Firmware,
- physischen Austausch des gesamten Geräts,
- Hardware-Keylogger,
- Angriffe nach erfolgreichem Kernelstart,
- gestohlene gültige Signaturschlüssel,
- invasive Hardwareangriffe,
- manipulierte Firmware ohne externe Vertrauenswurzel,
- Seitenkanäle außerhalb der Bootkette.

Diese Grenzen müssen in der Sicherheitsdiagnose sichtbar bleiben.

---

## 6. Betrachtete Alternativen

### 6.1 Keine kryptografische Verifikation

Der Bootloader lädt den Kernel ausschließlich anhand von Pfad und Dateiformat.

#### Vorteile

- einfache Implementierung,
- geringe Bootzeit,
- keine Schlüsselverwaltung.

#### Nachteile

- manipulierte Kernel werden ausgeführt,
- keine authentische Vertrauenskette,
- keine sichere Recovery,
- ungeeignet für den Capability-basierten Systemschutz.

Diese Alternative wird verworfen.

---

### 6.2 Ausschließliche Abhängigkeit von UEFI Secure Boot

NovaOS verlässt sich vollständig auf die Firmwareprüfung.

#### Vorteile

- Nutzung vorhandener Industriestandards,
- geringer eigener Implementierungsaufwand,
- Hardwareintegration möglich.

#### Nachteile

- BIOS-Systeme werden ausgeschlossen,
- Secure Boot kann deaktiviert sein,
- Firmwareimplementierungen sind unterschiedlich,
- interne NovaOS-Artefakte benötigen weiterhin Verifikation,
- Recovery und eigenes Boot-Handoff bleiben ungeschützt.

Diese Alternative wird verworfen.

---

### 6.3 Verpflichtendes TPM

NovaOS startet nur auf Systemen mit funktionsfähigem TPM.

#### Vorteile

- starke Gerätebindung,
- Measured Boot,
- versiegelte Geheimnisse,
- gute Rollback-Unterstützung.

#### Nachteile

- ältere Hardware wird ausgeschlossen,
- TPM-Fehler könnten das System unbrauchbar machen,
- widerspricht dem Ziel breiter Hardwareunterstützung,
- erhöht Recovery-Komplexität.

Diese Alternative wird verworfen.

---

### 6.4 Eigene verifizierte Bootkette mit optionalen Hardwareankern

NovaOS verifiziert seine Bootartefakte selbst und nutzt UEFI Secure Boot, TPM oder andere Hardwaremechanismen zusätzlich, wenn sie verfügbar sind.

#### Vorteile

- BIOS- und UEFI-Unterstützung,
- kein verpflichtendes TPM,
- einheitliche NovaOS-Vertrauenslogik,
- kontrollierte Recovery,
- klare Vertrauensstufen,
- erweiterbar auf zukünftige Trust Backends.

#### Nachteile

- eigene kryptografische Bootimplementierung erforderlich,
- komplexe Schlüsselverwaltung,
- BIOS kann keine vollständig unveränderbare Vertrauenswurzel garantieren,
- höherer Testaufwand.

Diese Alternative wird gewählt.

---

## 7. Entscheidung

NovaOS verwendet eine eigene verifizierte Bootkette mit optionalen Hardware-Vertrauensankern.

Der Kernel darf im normalen Produktionsmodus nur ausgeführt werden, wenn mindestens folgende Prüfungen erfolgreich sind:

1. Boot Manifest syntaktisch gültig,
2. Manifestversion unterstützt,
3. Manifest-Signatur gültig,
4. Kernelhash stimmt mit dem Manifest überein,
5. Kernelversion ist erlaubt,
6. Sicherheitsversion erfüllt die Rollback-Policy,
7. erforderliche Module und Startartefakte sind gültig,
8. Bootparameter entsprechen der signierten Policy,
9. Boot-Handoff-Struktur ist gültig,
10. Sicherheitsmodus und Schlüsselstatus sind zulässig.

UEFI Secure Boot, TPM und weitere Hardwarefunktionen verstärken diese Kette, sind aber nicht zwingend erforderlich.

Das Fehlen eines Hardware-Vertrauensankers wird im Trust State ausdrücklich vermerkt.

---

## 8. Vertrauenskette

Die angestrebte Vertrauenskette lautet:

```text
Firmware oder BIOS
        ↓
NovaOS Boot Stage 1
        ↓
NovaOS Boot Stage 2
        ↓
signiertes Boot Manifest
        ↓
Kernel und frühe Module
        ↓
Boot-Handoff
        ↓
Kernel-Sicherheitsinitialisierung
        ↓
Systemdienste und Userspace
```

Jede NovaOS-Stufe prüft die nächste Stufe, bevor sie Kontrolle übergibt.

Ist eine frühere Stufe auf der jeweiligen Plattform nicht kryptografisch verifizierbar, wird dies als Vertrauenslücke im Trust State dokumentiert.

---

## 9. Plattform-Vertrauensstufen

NovaOS unterscheidet mehrere Start-Vertrauensstufen.

| Stufe | Bezeichnung | Bedeutung |
|---|---|---|
| `UNVERIFIED` | Unverified | keine vollständige kryptografische Verifikation |
| `NOVA_VERIFIED` | Nova Verified | NovaOS-Artefakte wurden mit eingebettetem Schlüssel geprüft |
| `FIRMWARE_VERIFIED` | Firmware Verified | Firmware Secure Boot und NovaOS-Verifikation aktiv |
| `MEASURED` | Measured | Bootkomponenten zusätzlich in Hardware gemessen |
| `HARDWARE_BOUND` | Hardware Bound | Schlüssel oder Policy an Hardware-Vertrauensanker gebunden |
| `RECOVERY_VERIFIED` | Recovery Verified | verifizierter Recovery-Pfad aktiv |
| `DEVELOPMENT` | Development | autorisierter Entwicklungsmodus aktiv |

Diese Werte werden nicht ausschließlich als lineare Rangfolge behandelt. Der Trust State enthält einzelne Fähigkeits- und Prüfflags.

---

## 10. BIOS-Unterstützung

Auf BIOS-Systemen kann NovaOS keine standardisierte Firmware-Secure-Boot-Garantie voraussetzen.

Der NovaOS-Bootloader verwendet deshalb einen eingebetteten oder kontrolliert geladenen Root Public Key zur Prüfung des Boot Manifests.

Die Verifikation umfasst mindestens:

- Boot Manifest,
- Kernel,
- frühe Module,
- Bootkonfiguration,
- Recovery-Auswahl.

Grenze dieser Lösung:

Wenn ein Angreifer den Bootloader und den darin eingebetteten Schlüssel gemeinsam ersetzen kann, ist ohne zusätzliche Hardware oder schreibgeschützten Datenträger keine vollständige Vertrauenswurzel garantiert.

NovaOS kennzeichnet diesen Zustand als:

```text
NOVA_VERIFIED
firmware_root_trusted = false
```

---

## 11. UEFI-Unterstützung

Auf UEFI-Systemen verwendet NovaOS UEFI Secure Boot, wenn:

- es verfügbar ist,
- es aktiviert ist,
- die Firmware den NovaOS-Loader akzeptiert,
- die verwendete Signaturkette gültig ist.

Der UEFI-Loader prüft anschließend zusätzlich die NovaOS-eigene Signaturkette.

Dadurch entsteht eine zweistufige Prüfung:

```text
UEFI Secure Boot
        ↓
NovaOS Loader
        ↓
NovaOS Boot Manifest
        ↓
Kernel
```

NovaOS verlässt sich nicht ausschließlich auf die Firmwareprüfung.

---

## 12. TPM-Unterstützung

Ein TPM ist optional.

Ist ein kompatibles TPM verfügbar, kann NovaOS:

- Bootartefakte messen,
- PCR-Werte erweitern,
- Schlüssel an einen bekannten Bootzustand binden,
- Rollback-Metadaten absichern,
- Geräteidentität und Attestation unterstützen,
- verschlüsselte Systemressourcen freigeben.

Fehlt das TPM oder ist es deaktiviert, darf NovaOS weiterhin starten, sofern die konfigurierte Sicherheitsrichtlinie dies erlaubt.

Der Trust State enthält:

```text
tpm_present
tpm_enabled
measured_boot_active
sealed_secrets_available
```

---

## 13. Trust-Backend-Abstraktion

NovaOS kapselt Hardware-Vertrauensanker hinter einer einheitlichen Trust-Backend-Schnittstelle.

Mögliche Backends sind:

- `NOVA_TRUST_NONE`,
- `NOVA_TRUST_EMBEDDED_KEY`,
- `NOVA_TRUST_UEFI`,
- `NOVA_TRUST_TPM`,
- `NOVA_TRUST_HSM`,
- `NOVA_TRUST_PLATFORM_SPECIFIC`.

Eine konzeptionelle Schnittstelle lautet:

```c
typedef struct nova_trust_backend
{
    nova_status_t (*initialize)(void);

    nova_status_t (*verify_key)(
        const nova_public_key_t* key
    );

    nova_status_t (*measure)(
        uint32_t register_id,
        const void* data,
        size_t size
    );

    nova_status_t (*read_security_version)(
        uint64_t* version
    );

    nova_status_t (*commit_security_version)(
        uint64_t version
    );
} nova_trust_backend_t;
```

Die sichere Bootlogik darf nicht direkt von einem bestimmten TPM- oder Firmwareanbieter abhängen.

---

## 14. Boot Manifest

Jede startfähige NovaOS-Systemgeneration besitzt ein signiertes Boot Manifest.

Das Manifest enthält mindestens:

```text
manifest_format
manifest_version
system_generation
kernel_version
security_version
minimum_bootloader_version
creation_timestamp
signing_key_id
kernel_hash
module_hashes
boot_asset_hashes
allowed_boot_parameters
required_security_features
rollback_policy
recovery_policy
signature
```

Das Manifest wird vollständig geprüft, bevor seine Inhalte als vertrauenswürdig verwendet werden.

---

## 15. Manifest-Kanonisierung

Die Signaturprüfung erfordert eine eindeutige Byte-Darstellung des Manifests.

NovaOS verwendet deshalb ein kanonisches Manifestformat.

Das Format muss:

- eine eindeutige Feldreihenfolge besitzen,
- Längen explizit speichern,
- unbekannte kritische Felder ablehnen,
- Integer eindeutig codieren,
- Mehrdeutigkeiten bei Zeichenketten vermeiden,
- Signaturfelder klar vom signierten Bereich trennen,
- Größenlimits definieren.

Ein Parser darf vor der Signaturprüfung keine gefährlichen dynamischen Aktionen anhand unbestätigter Manifestdaten ausführen.

---

## 16. Signaturalgorithmen

NovaOS verwendet versionierte, austauschbare Signaturalgorithmen.

Anforderungen:

- öffentlich dokumentierter Algorithmus,
- ausreichend starke Schlüsselgrößen,
- konstante oder seitenkanalreduzierte Implementierung,
- klare Algorithmus-ID,
- keine implizite Algorithmusaushandlung,
- Schutz gegen Downgrade auf unsichere Algorithmen.

Ein Manifest enthält eine explizite Algorithmuskennung.

Unbekannte oder gesperrte Algorithmen werden abgelehnt.

Die konkrete initiale Algorithmusauswahl wird in einer kryptografischen Spezifikation festgelegt.

---

## 17. Hashalgorithmen

Alle Bootartefakte werden über kryptografische Hashwerte identifiziert.

Das Manifest muss für jedes Artefakt mindestens enthalten:

- Artefakttyp,
- Pfad oder Objekt-ID,
- Größe,
- Hashalgorithmus,
- Hashwert,
- Ladepolicy,
- Kritikalitätsstatus.

Hashalgorithmen werden versioniert und können über Sicherheitsupdates gesperrt werden.

Ein gültiger Hash ohne gültige Manifest-Signatur stellt keine Authentizität her.

---

## 18. Schlüsselhierarchie

NovaOS verwendet eine Schlüsselhierarchie.

```text
Offline Root Key
        ↓
Release Signing Key
        ↓
Boot Manifest
        ↓
Kernel und Bootartefakte
```

Der Offline Root Key wird nicht für tägliche Builds verwendet.

Er autorisiert:

- Release Signing Keys,
- Recovery Signing Keys,
- Schlüsselrotationen,
- Sperrlisten,
- Notfallschlüssel.

Produktions-, Entwicklungs- und Recovery-Schlüssel werden getrennt.

---

## 19. Schlüsselrollen

NovaOS unterscheidet mindestens:

| Schlüsselrolle | Zweck |
|---|---|
| Root Key | oberster langfristiger Vertrauensanker |
| Release Key | signiert freigegebene Systemgenerationen |
| Recovery Key | signiert Recovery-Systeme |
| Development Key | signiert Entwicklungs-Builds |
| Emergency Key | autorisiert Notfallwiederherstellung oder Schlüsselwechsel |
| Revocation Key | autorisiert Sperrlisten und Schlüsselwiderruf |

Ein Schlüssel darf nur für seine erlaubte Rolle verwendet werden.

---

## 20. Schlüsselrotation

Schlüssel müssen kontrolliert austauschbar sein.

Eine Rotation kann erfolgen durch:

1. neues Schlüsselzertifikat mit altem vertrauenswürdigem Schlüssel signieren,
2. neue und alte Schlüssel vorübergehend parallel akzeptieren,
3. neue Systemgenerationen mit dem neuen Schlüssel signieren,
4. Übergangsfrist abwarten,
5. alten Schlüssel über eine signierte Policy sperren.

Die Rotation darf bestehende Recovery-Fähigkeiten nicht unkontrolliert zerstören.

---

## 21. Schlüsselwiderruf

Kompromittierte oder veraltete Schlüssel können widerrufen werden.

Widerrufsinformationen können enthalten:

```text
key_id
revocation_generation
reason
effective_security_version
replacement_key_id
signature
```

Ein widerrufener Schlüssel darf keine neue Systemgeneration autorisieren.

Für bereits installierte Recovery-Systeme kann eine kontrollierte Notfallpolicy gelten, wenn andernfalls keine Wiederherstellung möglich wäre. Dieser Zustand muss deutlich als eingeschränktes Vertrauen angezeigt werden.

---

## 22. Kernelverifikation

Vor dem Laden oder spätestens vor der Ausführung des Kernels prüft der Bootloader:

1. erwartete Dateigröße,
2. Dateiformat,
3. Maschinenarchitektur,
4. Hashwert,
5. Manifestzuordnung,
6. Sicherheitsversion,
7. Ladegrenzen,
8. Segmentberechtigungen,
9. Überlappungen,
10. Entry-Point-Gültigkeit.

Der Kernel darf erst ausgeführt werden, wenn sämtliche kritischen Prüfungen erfolgreich sind.

---

## 23. Verifikation früher Module

Frühe Kernelmodule und Boottreiber werden wie der Kernel verifiziert.

Dazu können gehören:

- Dateisystemtreiber,
- Storage-Treiber,
- Kryptografiemodule,
- Recovery-Komponenten,
- Plattformmodule,
- frühe Grafik- oder Konsolentreiber.

Jedes Modul muss im Manifest aufgeführt oder durch eine signierte Delegationspolicy erlaubt sein.

Unbekannte frühe Module werden im Produktionsmodus nicht geladen.

---

## 24. Initramfs und frühe Userspace-Komponenten

Falls NovaOS ein Initramfs oder einen frühen Userspace verwendet, wird dieser ebenfalls verifiziert.

Die Prüfung umfasst:

- Container- oder Imagehash,
- Manifest,
- enthaltene kritische Programme,
- Startkonfiguration,
- Berechtigungs- und Capability-Profile.

Ein verifizierter Kernel darf seine Vertrauenskette nicht unmittelbar an einen ungeprüften privilegierten frühen Userspace verlieren.

---

## 25. Bootparameter

Bootparameter können die Sicherheit des Kernels wesentlich verändern.

Beispiele:

- Deaktivierung von Signaturprüfungen,
- Aktivierung von Debugschnittstellen,
- Änderung von Speicherschutzregeln,
- Laden alternativer Module,
- Deaktivierung von Sicherheitsmitigationen,
- Auswahl eines anderen Root-Dateisystems.

Das Boot Manifest definiert deshalb:

- erlaubte Parameter,
- erforderliche Parameter,
- unveränderliche Parameter,
- nur im Entwicklungsmodus zulässige Parameter.

Nicht autorisierte sicherheitskritische Parameter führen im Produktionsmodus zum Bootabbruch.

---

## 26. Boot-Handoff-Verifikation

Das eigene Boot-Handoff-Protokoll gemäß ADR-BOOT-1012 wird sicherheitsrelevant behandelt.

Die Handoff-Struktur enthält mindestens:

```text
magic
protocol_version
structure_size
bootloader_version
boot_mode
memory_map
kernel_image_info
module_list
boot_parameters
trust_state
manifest_digest
system_generation
security_version
integrity_check
```

Der Kernel validiert die Struktur vollständig, bevor er enthaltene Zeiger oder Größen verwendet.

---

## 27. Schutz der Handoff-Struktur

Die Handoff-Struktur wird:

- in einem dokumentierten Speicherbereich abgelegt,
- mit Größen- und Versionsangaben versehen,
- gegen Integerüberläufe validiert,
- auf Überlappungen geprüft,
- nach Möglichkeit schreibgeschützt übergeben,
- durch einen Integritätswert geschützt.

Ein Integritätswert allein ersetzt keine Vertrauenskette. Er dient dazu, Beschädigungen und unerwartete Änderungen zwischen Bootloader und Kernel zu erkennen.

---

## 28. Kernel-Selbstverifikation

Der Kernel übernimmt den Trust State nicht ungeprüft.

Während der frühen Initialisierung prüft er mindestens:

- Boot-Handoff-Magic,
- Protokollversion,
- Strukturgrößen,
- Manifest-Digest,
- Kernel-Image-Identität,
- Sicherheitsversion,
- Bootmodus,
- Schlüssel-ID,
- Modulstatus,
- Trust-Backend-Status.

Optional kann der Kernel kritische Bereiche seines geladenen Images erneut hashen.

Dadurch werden Fehler zwischen Bootloaderprüfung und Kernelinitialisierung erkannt.

---

## 29. Trust State

Der Bootloader übergibt dem Kernel einen strukturierten Trust State.

Eine mögliche Struktur lautet:

```c
typedef struct
{
    uint32_t structure_size;
    uint32_t structure_version;

    uint32_t trust_level;
    uint32_t trust_flags;

    uint64_t system_generation;
    uint64_t security_version;

    nova_key_id_t signing_key_id;
    nova_digest_t manifest_digest;
    nova_digest_t kernel_digest;

    uint32_t verification_result;
    uint32_t measurement_result;

    uint32_t trust_backend;
    uint32_t boot_policy;

    uint64_t event_log_address;
    uint64_t event_log_size;
} nova_boot_trust_state_t;
```

Der Kernel kopiert die validierten Daten in einen eigenen geschützten Speicherbereich.

---

## 30. Trust Flags

Mögliche Trust Flags sind:

```c
typedef enum
{
    NOVA_TRUST_FLAG_NONE                  = 0,
    NOVA_TRUST_FLAG_BOOTLOADER_VERIFIED   = 1u << 0,
    NOVA_TRUST_FLAG_MANIFEST_VERIFIED     = 1u << 1,
    NOVA_TRUST_FLAG_KERNEL_VERIFIED       = 1u << 2,
    NOVA_TRUST_FLAG_MODULES_VERIFIED      = 1u << 3,
    NOVA_TRUST_FLAG_UEFI_SECURE_BOOT      = 1u << 4,
    NOVA_TRUST_FLAG_TPM_PRESENT           = 1u << 5,
    NOVA_TRUST_FLAG_MEASURED_BOOT         = 1u << 6,
    NOVA_TRUST_FLAG_ROLLBACK_PROTECTED    = 1u << 7,
    NOVA_TRUST_FLAG_RECOVERY_BOOT         = 1u << 8,
    NOVA_TRUST_FLAG_DEVELOPMENT_MODE      = 1u << 9,
    NOVA_TRUST_FLAG_UNVERIFIED_COMPONENT  = 1u << 10,
    NOVA_TRUST_FLAG_POLICY_DEGRADED       = 1u << 11
} nova_trust_flags_t;
```

Flags werden einzeln ausgewertet. Eine einzelne zusammengefasste Vertrauenszahl reicht nicht aus.

---

## 31. Sicherheitsmodi

NovaOS unterstützt folgende Boot-Sicherheitsmodi:

| Modus | Verhalten |
|---|---|
| `ENFORCING` | nur vollständig erlaubte Artefakte werden gestartet |
| `RECOVERY` | nur signiertes Recovery-System wird gestartet |
| `DEVELOPMENT` | autorisierte Entwicklungsartefakte möglich |
| `AUDIT` | Verstöße werden protokolliert, soweit Policy dies zulässt |
| `UNVERIFIED` | Start ohne vollständige Verifikation, nur ausdrücklich erlaubt |

Der Produktionsstandard ist:

```text
ENFORCING
```

Ein stiller automatischer Wechsel von `ENFORCING` zu `UNVERIFIED` ist nicht zulässig.

---

## 32. Enforcing Mode

Im Enforcing Mode führen folgende Fehler zum Abbruch des normalen Starts:

- ungültige Manifest-Signatur,
- Kernelhash stimmt nicht,
- erforderliches Modul ungültig,
- Schlüssel widerrufen,
- Sicherheitsversion zu niedrig,
- nicht erlaubte Bootparameter,
- unbekanntes kritisches Manifestfeld,
- manipuliertes Handoff,
- erforderliches Trust Backend nicht verfügbar.

Der Benutzer erhält einen eindeutigen Bootfehler und kontrollierte Recovery-Optionen.

---

## 33. Development Mode

Der Development Mode dient der Kernel- und Treiberentwicklung.

Er kann erlauben:

- Development Keys,
- zusätzliche Debugparameter,
- nicht freigegebene Kernelmodule,
- reduzierte Rollback-Grenzen,
- ausführlichere Diagnose.

Der Development Mode muss ausdrücklich aktiviert werden.

Mögliche Aktivierungsbedingungen:

- physische Benutzerbestätigung,
- Entwicklungs-Capability,
- signierte Development-Policy,
- einmaliges Boot-Token,
- Firmware- oder Gerätepolicy.

Der Zustand wird dauerhaft und deutlich als `DEVELOPMENT` gekennzeichnet.

---

## 34. Kein stilles Development-Fallback

Ein fehlgeschlagener Produktionsstart darf nicht automatisch in den Development Mode wechseln.

Der Wechsel benötigt eine bewusste und nachvollziehbare Aktion.

Dadurch wird verhindert, dass ein Angreifer absichtlich eine Signaturprüfung beschädigt und dadurch einen weniger sicheren Modus erzwingt.

---

## 35. Unverified Mode

Der Unverified Mode ist ausschließlich für:

- frühe Entwicklung,
- nicht sicherheitskritische Altgeräte,
- kontrollierte Reparatur,
- ausdrücklich freigegebene Testsysteme

vorgesehen.

Er muss:

- bewusst aktiviert werden,
- deutlich angezeigt werden,
- im Trust State vermerkt werden,
- bestimmte Sicherheitsfunktionen einschränken können,
- eine lokale Diagnosemeldung erzeugen.

Produktionsrichtlinien können den Unverified Mode vollständig deaktivieren.

---

## 36. Rollback Protection

Jede Systemgeneration besitzt:

- eine Systemgeneration,
- eine Sicherheitsversion.

Die Systemgeneration identifiziert den konkreten Systemstand.

Die Sicherheitsversion beschreibt die minimale Sicherheitsstufe.

Ein älterer Build kann eine gleiche Sicherheitsversion besitzen, sofern kein sicherheitskritischer Rückschritt vorliegt.

NovaOS lehnt eine Systemgeneration ab, wenn:

```text
manifest.security_version
    <
minimum_allowed_security_version
```

---

## 37. Speicherung der Sicherheitsversion

Die minimale erlaubte Sicherheitsversion kann gespeichert werden in:

- TPM-NV-Speicher,
- UEFI-authentifizierter Variable,
- Hardware-Sicherheitsmodul,
- signierter Bootmetadatenpartition,
- mehrfach replizierten und signierten NovaOS-Metadaten.

Die Stärke der Rollback-Garantie hängt vom verwendeten Backend ab.

Ohne manipulationsgeschützte Hardware kann NovaOS Rollback erkennen und erschweren, aber nicht in jedem physischen Angriffsszenario vollständig verhindern.

---

## 38. Kontrollierter Rollback

Ein kontrollierter Rollback kann erforderlich sein, wenn eine neue Systemgeneration nicht startet.

NovaOS trennt deshalb:

- funktionale Systemgeneration,
- minimale Sicherheitsversion.

Ein Rollback auf eine ältere Generation ist erlaubt, wenn deren Sicherheitsversion nicht unterhalb der Sicherheitsgrenze liegt.

```text
ältere Generation
+ erlaubte Sicherheitsversion
= kontrollierter Rollback möglich
```

Ein sicherheitskritisch gesperrter Build bleibt verboten.

---

## 39. Recovery-System

NovaOS besitzt ein separat signiertes Recovery-System.

Das Recovery-System:

- verwendet einen eigenen Recovery-Schlüssel oder eine getrennte Schlüsselrolle,
- wird unabhängig vom Hauptkernel verifiziert,
- besitzt minimale, klar definierte Funktionen,
- kann Systemgenerationen prüfen und reparieren,
- darf die Sicherheitsversion nicht unautorisiert absenken,
- protokolliert sicherheitsrelevante Änderungen.

Recovery ist kein unkontrollierter Weg zur Umgehung der Produktionsrichtlinie.

---

## 40. Recovery-Auslöser

Recovery kann gestartet werden durch:

- Benutzerwahl im Bootmenü,
- wiederholten Bootfehler,
- ungültige Hauptsystemgeneration,
- fehlgeschlagenes Kernel-Startup,
- Watchdog- oder Gesundheitsrichtlinie,
- signierte Fernwartungspolicy,
- physische Recovery-Aktion.

Der automatische Recovery-Start muss ebenfalls verifiziert werden.

---

## 41. Recovery und Schlüsselreparatur

Das Recovery-System kann Schlüssel- oder Manifestmetadaten nur ändern, wenn:

- eine gültige Recovery-Autorisierung vorliegt,
- die neue Konfiguration kryptografisch geprüft wurde,
- keine verbotene Sicherheitsversion aktiviert wird,
- die Änderung nachvollziehbar protokolliert wird.

Ein Recovery-System darf nicht beliebige neue Root Keys ohne explizite Vertrauensentscheidung installieren.

---

## 42. Systemgenerationen

Jede installierte NovaOS-Systemgeneration besitzt:

- eigenes Boot Manifest,
- eigenen Kernel,
- zugehörige Module,
- zugehörige Bootassets,
- unveränderliche Identität,
- Statusinformationen.

Mögliche Zustände:

```text
INSTALLED
VERIFIED
PENDING
BOOTING
CONFIRMED
FAILED
REVOKED
RECOVERY
```

Eine neue Generation wird erst nach erfolgreichem Start und Gesundheitscheck als `CONFIRMED` markiert.

---

## 43. Trial Boot

Updates können zunächst als Trial Boot gestartet werden.

Der Ablauf lautet:

```text
neue Generation installieren
        ↓
Signatur und Manifest prüfen
        ↓
als PENDING markieren
        ↓
einmalig starten
        ↓
Kernel- und Systemgesundheit prüfen
        ↓
als CONFIRMED markieren
```

Schlägt der Start wiederholt fehl, kann NovaOS zu einer älteren, weiterhin sicherheitszulässigen Generation zurückkehren.

---

## 44. Boot-Erfolg

Ein Kernelstart gilt nicht bereits beim Erreichen des Kernel Entry Points als erfolgreich.

Eine Systemgeneration wird erst bestätigt, wenn mindestens:

- Kernelinitialisierung abgeschlossen,
- Speicherverwaltung funktionsfähig,
- Sicherheitsdienste gestartet,
- kritische Dateisysteme verfügbar,
- System-Trust-State übernommen,
- definierter Health Check erfolgreich

sind.

Die Bestätigung erfolgt über einen privilegierten, authentisierten Systemdienst.

---

## 45. Boot-Failure-Zähler

NovaOS verwaltet pro Systemgeneration einen Boot-Failure-Zähler.

Der Zähler wird erhöht, wenn:

- der Kernel vor Bestätigung abstürzt,
- ein Watchdog den Start abbricht,
- der Health Check fehlschlägt,
- der Benutzer den fehlgeschlagenen Start als defekt markiert.

Nach Überschreitung einer Grenze kann NovaOS:

- die Generation als `FAILED` markieren,
- eine sichere ältere Generation auswählen,
- Recovery starten.

Boot-Failure-Metadaten müssen gegen einfache Manipulation und Beschädigung geschützt werden.

---

## 46. Measured Boot

Ist ein Messbackend verfügbar, misst NovaOS mindestens:

- Bootloaderstufen,
- Boot Manifest,
- Kernel,
- frühe Module,
- Bootparameter,
- Systemgeneration,
- Sicherheitsmodus,
- Recovery-Zustand.

Die Messungen werden in einem strukturierten Event Log dokumentiert.

Das Event Log wird dem Kernel übergeben und später schreibgeschützt für autorisierte Diagnosedienste verfügbar gemacht.

Measured Boot ersetzt Verified Boot nicht.

---

## 47. Attestation

Eine spätere Attestation kann den gemessenen Bootzustand gegenüber einem autorisierten Prüfer nachweisen.

Attestation ist:

- optional,
- policygesteuert,
- datenschutzsensibel,
- nicht Voraussetzung für den lokalen Systemstart.

NovaOS darf Bootmessungen nicht ohne ausdrückliche Richtlinie automatisch an externe Dienste übertragen.

Lokale Verifikation bleibt der Standard.

---

## 48. Entschlüsselung und versiegelte Schlüssel

Ein Hardware-Trust-Backend kann Schlüssel an einen bekannten Bootzustand binden.

Dadurch können beispielsweise:

- Systempartitionen,
- Recovery-Geheimnisse,
- Geräteidentitäten,
- lokale Benutzergeheimnisse

erst bei einem zulässigen Trust State freigegeben werden.

Es muss einen kontrollierten Recovery-Pfad geben, falls:

- Hardware ausgetauscht wird,
- Firmware aktualisiert wird,
- Messwerte sich zulässig ändern,
- das TPM ausfällt.

Ein TPM-Ausfall darf nicht ohne vorher definierte Recovery-Strategie zum dauerhaften Datenverlust führen.

---

## 49. Zufallszahlen beim frühen Start

Kryptografische Operationen können während des frühen Starts Zufallswerte benötigen.

NovaOS verwendet dafür nach Möglichkeit:

- Hardware-Zufallsquellen,
- Firmware-Zufallsquellen,
- gespeicherten und geschützten Seed,
- mehrere kombinierte Entropiequellen.

Nicht geprüfte oder einzelne schwache Quellen dürfen nicht stillschweigend als vollständig vertrauenswürdig gelten.

Deterministische Signaturverifikation darf nicht unnötig von verfügbarer Laufzeitentropie abhängen.

---

## 50. Zeitabhängige Prüfungen

Frühe Bootphasen besitzen möglicherweise keine vertrauenswürdige Echtzeituhr.

Sicherheitsentscheidungen dürfen deshalb nicht ausschließlich von einer unbestätigten Kalenderzeit abhängen.

Bevorzugt werden:

- monotone Sicherheitsversionen,
- signierte Generationen,
- Hardwarezähler,
- Widerrufsgenerationen.

Zertifikatslaufzeiten können ergänzend geprüft werden, wenn eine vertrauenswürdige Zeitquelle verfügbar ist.

---

## 51. Fehlerdarstellung

Ein Secure-Startup-Fehler wird über den NovaOS-Bootfehlerpfad dargestellt.

Die Meldung enthält mindestens:

- verständlichen Fehlertext,
- technischen Fehlercode,
- betroffene Komponente,
- aktuelle Systemgeneration,
- Trust State,
- zulässige Recovery-Optionen.

Geheime Schlüssel, vollständige interne Speicheradressen und sensible Gerätedaten werden nicht angezeigt.

Beispiel:

```text
NovaOS konnte nicht sicher gestartet werden.

Der Kernel stimmt nicht mit dem signierten Boot Manifest überein.

Fehlercode: NOVA_BOOT_SECURITY_KERNEL_HASH_MISMATCH
Systemgeneration: 42
```

---

## 52. Verhalten bei Verifikationsfehlern

Bei einem Verifikationsfehler im Enforcing Mode darf NovaOS:

- den normalen Start abbrechen,
- ein signiertes Recovery-System anbieten,
- eine andere verifizierte Systemgeneration auswählen,
- einen kontrollierten Shutdown durchführen.

NovaOS darf nicht:

- den fehlerhaften Kernel trotzdem starten,
- die Prüfung stillschweigend deaktivieren,
- automatisch in den Development Mode wechseln,
- unbestätigte externe Module nachladen.

---

## 53. Fehlercodes

Mögliche Fehlercodes sind:

```text
NOVA_BOOT_SECURITY_MANIFEST_NOT_FOUND
NOVA_BOOT_SECURITY_MANIFEST_INVALID
NOVA_BOOT_SECURITY_SIGNATURE_INVALID
NOVA_BOOT_SECURITY_KEY_REVOKED
NOVA_BOOT_SECURITY_ALGORITHM_DISABLED
NOVA_BOOT_SECURITY_KERNEL_HASH_MISMATCH
NOVA_BOOT_SECURITY_MODULE_HASH_MISMATCH
NOVA_BOOT_SECURITY_ROLLBACK_DETECTED
NOVA_BOOT_SECURITY_PARAMETER_DENIED
NOVA_BOOT_SECURITY_HANDOFF_INVALID
NOVA_BOOT_SECURITY_TRUST_BACKEND_FAILED
NOVA_BOOT_SECURITY_REQUIRED_TPM_MISSING
NOVA_BOOT_SECURITY_RECOVERY_INVALID
NOVA_BOOT_SECURITY_POLICY_CONFLICT
```

Fehlercodes bleiben über versionierte Boot- und Kernel-Schnittstellen stabil.

---

## 54. Boot-Sicherheitsereignisse

Der Bootloader führt ein begrenztes Sicherheitsereignisprotokoll.

Ein Ereignis kann enthalten:

```text
sequence
timestamp_or_counter
component_type
component_id
digest
verification_result
key_id
policy_action
error_code
```

Wenn keine vertrauenswürdige Uhr verfügbar ist, wird eine monotone Ereignissequenz verwendet.

Das Protokoll wird dem Kernel übergeben und dort in die lokale Telemetrie übernommen.

---

## 55. Kernelinitialisierung nach erfolgreicher Prüfung

Nach Übernahme des Trust State führt der Kernel mindestens folgende Schritte aus:

1. Handoff-Daten kopieren und schützen,
2. Trust State validieren,
3. Sicherheitsmodus festlegen,
4. Manifest-Digest übernehmen,
5. Modulpolicy aktivieren,
6. Debugpolicy festlegen,
7. Rollbackstatus erfassen,
8. Bootereignisse übernehmen,
9. Trust-State-Kernelobjekt erzeugen,
10. privilegierte Abfrageschnittstelle bereitstellen.

Der Trust State darf nach der Initialisierung nicht durch normalen Userspace verändert werden.

---

## 56. Auswirkungen auf Kernelmodule

Die Secure-Startup-Entscheidung gilt auch für später geladene Kernelmodule.

Der Kernel darf im Enforcing Mode nur Module laden, die:

- gültig signiert sind,
- durch eine erlaubte Schlüsselrolle autorisiert wurden,
- zur laufenden Kernel-ABI passen,
- nicht widerrufen wurden,
- die aktive Sicherheitsversion erfüllen,
- mit der Systemgeneration oder einer erlaubten Erweiterungspolicy kompatibel sind.

Die vollständige Laufzeit-Modulpolicy wird in einer gesonderten ADR definiert.

---

## 57. Debugger und Diagnose

Kernel-Debugschnittstellen können die Sicherheitsgarantie reduzieren.

Im Enforcing Mode gelten deshalb mindestens folgende Regeln:

- Kernel-Speicherschreibzugriff benötigt spezielle Autorisierung,
- unsignierter Debugcode ist nicht zulässig,
- Debugaktivierung wird im Trust State markiert,
- bestimmte versiegelte Geheimnisse können gesperrt bleiben,
- Remote-Debugging ist standardmäßig deaktiviert.

Ein autorisierter Produktions-Debugmodus wird getrennt vom allgemeinen Development Mode behandelt.

---

## 58. Sicherheitsstatus für Userspace

Autorisierte Systemdienste können den Trust State schreibgeschützt abfragen.

Beispiele für sichtbare Informationen:

- Kernel verifiziert,
- Module verifiziert,
- UEFI Secure Boot aktiv,
- TPM vorhanden,
- Measured Boot aktiv,
- Rollback-Schutz aktiv,
- Development Mode aktiv,
- Recovery Boot aktiv,
- Sicherheitsversion,
- Systemgeneration.

Normale Anwendungen erhalten nur die Informationen, die ihre Sicherheits- und Datenschutzrechte erlauben.

---

## 59. API-Grundmodell

Der Kernel stellt eine schreibgeschützte Trust-State-Schnittstelle bereit.

```c
nova_status_t nova_security_get_boot_trust_state(
    nova_boot_trust_info_t* info
);

nova_status_t nova_security_get_boot_event_log(
    void* buffer,
    size_t buffer_size,
    size_t* required_size
);

nova_status_t nova_security_query_boot_component(
    nova_boot_component_id_t component_id,
    nova_boot_component_info_t* info
);
```

Eine mögliche Informationsstruktur lautet:

```c
typedef struct
{
    uint32_t structure_size;
    uint32_t structure_version;

    uint32_t trust_level;
    uint32_t trust_flags;

    uint64_t system_generation;
    uint64_t security_version;

    nova_key_id_t signing_key_id;

    nova_digest_t manifest_digest;
    nova_digest_t kernel_digest;

    uint32_t boot_policy;
    uint32_t trust_backend;

    nova_status_t verification_status;
} nova_boot_trust_info_t;
```

Die Schnittstelle erlaubt keine Veränderung des Trust State.

---

## 60. Sicherheits- und Capability-Modell

Mögliche Capabilities sind:

```text
CAP_SECURITY_QUERY_BOOT_STATE
CAP_SECURITY_READ_BOOT_LOG
CAP_SECURITY_MANAGE_KEYS
CAP_SECURITY_MANAGE_REVOCATIONS
CAP_SECURITY_AUTHORIZE_DEVELOPMENT
CAP_SECURITY_AUTHORIZE_RECOVERY
CAP_SECURITY_COMMIT_VERSION
CAP_SECURITY_BOOT_ADMIN
```

Schlüssel- und Rollbackoperationen erfordern zusätzliche physische, kryptografische oder administrative Bestätigung.

Ein allgemeines Administratorrecht allein muss nicht automatisch zum Ersetzen des Root of Trust berechtigen.

---

## 61. Schutz kryptografischer Implementierungen

Kryptografische Bootfunktionen müssen:

- kleine, überprüfbare Implementierungen verwenden,
- keine unnötige dynamische Speicherallokation benötigen,
- Eingabelängen vollständig validieren,
- Integerüberläufe verhindern,
- fehlerhafte Signaturen eindeutig ablehnen,
- keine unbekannten Algorithmen erraten,
- Testvektoren besitzen,
- gegen Parserdifferenzen geschützt sein.

Fehlerhafte Kryptografie darf nicht durch eine scheinbar sichere Benutzeroberfläche verdeckt werden.

---

## 62. Updateintegration

Ein NovaOS-Update darf erst als startfähig registriert werden, wenn:

- alle Artefakte vollständig geschrieben wurden,
- das Manifest gültig ist,
- Signaturen erfolgreich geprüft wurden,
- die Systemgeneration atomar registriert wurde,
- Bootmetadaten konsistent sind,
- ein sicherer Fallback erhalten bleibt.

Ein Stromausfall während des Updates darf keine teilweise vertrauenswürdige Generation aktivieren.

---

## 63. Atomare Aktivierung

Die Aktivierung einer neuen Systemgeneration erfolgt atomar.

Konzeptionell:

```text
Artefakte schreiben
        ↓
Hashwerte prüfen
        ↓
Manifest prüfen
        ↓
Generation als INSTALLED registrieren
        ↓
Bootauswahl atomar auf PENDING setzen
        ↓
Trial Boot
```

Der Bootloader darf ausschließlich vollständig registrierte Generationen auswählen.

---

## 64. Sicherheitsgrenzen ohne Hardwareanker

Ohne UEFI Secure Boot, TPM oder vergleichbare Hardware kann NovaOS:

- Kernel und Module kryptografisch prüfen,
- Beschädigungen erkennen,
- signierte Updates erzwingen,
- nicht autorisierte Artefakte ablehnen,
- Software-Rollback-Metadaten prüfen.

NovaOS kann dann jedoch nicht vollständig garantieren, dass:

- der Bootloader selbst nicht ersetzt wurde,
- eingebettete Root Keys nicht ausgetauscht wurden,
- Rollback-Metadaten physisch nicht zurückgesetzt wurden.

Dieser Unterschied wird im Trust State ausdrücklich sichtbar gemacht.

---

## 65. Performance

Verifikation erhöht die Bootzeit durch:

- Manifestprüfung,
- Hashberechnung,
- Signaturprüfung,
- TPM-Messungen,
- Modulvalidierung.

NovaOS akzeptiert diesen Aufwand im Produktionsmodus als notwendige Sicherheitskosten.

Optimierungen dürfen erfolgen durch:

- Streaming-Hashing während des Ladens,
- hardwarebeschleunigte Hashfunktionen,
- parallele Prüfung unabhängiger Artefakte,
- Caching nicht sicherheitskritischer Metadaten.

Kritische Prüfungen dürfen nicht übersprungen werden, nur um den Start zu beschleunigen.

---

## 66. Telemetrie und Diagnose

Secure Kernel Startup erfasst mindestens:

- Bootmodus,
- Trust Backend,
- aktivierte Sicherheitsrichtlinie,
- verwendete Schlüssel-ID,
- Manifestversion,
- Systemgeneration,
- Sicherheitsversion,
- Prüfergebnisse aller kritischen Artefakte,
- Rollbackentscheidung,
- Recoveryentscheidung,
- Development-Status,
- Messstatus,
- Fehlercodes.

Die Telemetrie wird lokal gespeichert.

Eine externe Übertragung erfolgt nur nach ausdrücklicher Richtlinie.

---

## 67. Fehlerbehandlung

Secure Kernel Startup muss bei jedem Fehler einen eindeutig definierten Zustand erreichen.

Mögliche Reaktionen sind:

| Fehlerklasse | Standardreaktion |
|---|---|
| Manifest beschädigt | normalen Start abbrechen |
| Signatur ungültig | normalen Start abbrechen |
| Kernelhash falsch | normalen Start abbrechen |
| optionales Modul ungültig | Modul auslassen, wenn Manifest dies erlaubt |
| kritisches Modul ungültig | normalen Start abbrechen |
| Rollback erkannt | Generation ablehnen |
| TPM nicht verfügbar | gemäß Policy degradiert starten oder abbrechen |
| Handoff ungültig | Kernelstart abbrechen |
| Recovery ungültig | Recovery nicht starten |
| Development-Key im Produktionsmodus | Artefakt ablehnen |

Ein sicherheitsrelevanter Fehler darf nicht als gewöhnliche Warnung ignoriert werden.

---

## 68. Konsequenzen

### 68.1 Positive Konsequenzen

- manipulierte Kernel werden vor der Ausführung erkannt,
- BIOS und UEFI werden unterstützt,
- TPM bleibt optional,
- einheitliche NovaOS-Vertrauenskette,
- kontrollierte Recovery,
- Rollback-Schutz,
- sichere Systemgenerationen,
- klare Trennung von Produktion und Entwicklung,
- nachvollziehbarer Trust State,
- Grundlage für sichere Kernelmodule und verschlüsselte Ressourcen.

### 68.2 Negative Konsequenzen

- deutlich höhere Bootloaderkomplexität,
- Schlüsselverwaltung wird notwendig,
- zusätzliche Bootzeit,
- Recovery und Schlüsselrotation müssen langfristig gepflegt werden,
- BIOS-Systeme bieten ohne Hardwareanker geringere Garantien,
- fehlerhafte Sicherheitsmetadaten können den Start verhindern.

### 68.3 Risiken

- Verlust oder Kompromittierung von Signaturschlüsseln,
- fehlerhafte Kryptografieimplementierung,
- unbrauchbares System nach falschem Schlüsselwiderruf,
- Rollback-Metadatenkorruption,
- unsicherer Development Mode,
- manipulierte Recovery-Komponenten,
- falsche Trust-State-Darstellung,
- Firmwareangriffe unterhalb des NovaOS-Bootloaders.

Diese Risiken werden begrenzt durch:

- Schlüsselhierarchie,
- Offline Root Keys,
- getrennte Schlüsselrollen,
- signierte Widerrufslisten,
- atomare Systemgenerationen,
- verifiziertes Recovery,
- Trust-Backend-Abstraktion,
- umfangreiche Testvektoren,
- klare Garantiegrenzen.

---

## 69. Auswirkungen auf andere Subsysteme

### 69.1 Bootloader

Alle Bootloaderstufen müssen Manifest-, Signatur- und Hashprüfung unterstützen.

### 69.2 Boot-Handoff

Das Handoff-Protokoll muss einen versionierten Trust State und Boot Event Log transportieren.

### 69.3 Kernel Entry

Der Kernel Entry muss Handoff und Trust State validieren, bevor komplexe Subsysteme gestartet werden.

### 69.4 Speicherverwaltung

Handoff-, Manifest- und Trust-State-Daten müssen nach der Übernahme geschützt werden.

### 69.5 Kernelmodulsystem

Später geladene Kernelmodule müssen die aktive Signatur- und Sicherheitsrichtlinie einhalten.

### 69.6 Dateisystem

Systemgenerationen, Manifeste und Bootartefakte müssen atomar und integritätsgeschützt gespeichert werden.

### 69.7 Update-System

Updates müssen vollständig verifiziert werden, bevor sie als startfähig registriert werden.

### 69.8 Recovery-System

Recovery benötigt eine eigene verifizierte Vertrauenskette und klar begrenzte Rechte.

### 69.9 Capability-System

Schlüssel-, Recovery-, Development- und Trust-State-Operationen werden durch spezielle Capabilities geschützt.

### 69.10 Telemetrie

Bootereignisse und Trust State werden in die lokale Kerneltelemetrie übernommen.

### 69.11 Verschlüsselung

Versiegelte Schlüssel können von einem bekannten Bootzustand abhängig gemacht werden.

---

## 70. Implementierungsrichtlinien

1. Die erste Implementierung verwendet ein signiertes Boot Manifest.
2. Kernelhash und Manifest-Signatur werden vor der Kernelausführung geprüft.
3. BIOS und UEFI verwenden dieselbe NovaOS-Manifestlogik.
4. UEFI Secure Boot wird als zusätzlicher Vertrauensanker unterstützt.
5. TPM-Unterstützung wird optional hinter einer Trust-Backend-Schnittstelle ergänzt.
6. Der Produktionsstandard ist `ENFORCING`.
7. Ein fehlgeschlagener Enforcing-Start darf nicht automatisch in einen schwächeren Modus wechseln.
8. Development- und Release-Schlüssel werden strikt getrennt.
9. Bootparameter werden in die Signatur- und Policyprüfung einbezogen.
10. Der Kernel validiert den Boot-Handoff vollständig erneut.
11. Systemgenerationen werden atomar registriert und aktiviert.
12. Ein verifiziertes Recovery-System bleibt verfügbar.
13. Rollback Protection verwendet eine getrennte Sicherheitsversion.
14. Unbekannte kritische Manifestfelder führen zur Ablehnung.
15. Kryptografische Parser verwenden feste Größenlimits und kanonische Formate.
16. Der erreichte Trust State wird vollständig an den Kernel übergeben.
17. Fehlende Hardware-Vertrauensanker werden sichtbar dokumentiert.
18. Boottelemetrie bleibt standardmäßig lokal.

---

## 71. Testanforderungen

Mindestens folgende Szenarien müssen getestet werden:

- gültiges signiertes Manifest,
- gültiger Kernel,
- beschädigter Kernel,
- beschädigtes Manifest,
- ungültige Signatur,
- unbekannter Signaturalgorithmus,
- gesperrter Algorithmus,
- widerrufener Schlüssel,
- Schlüsselrotation,
- fehlendes Manifest,
- falsche Kernelarchitektur,
- ungültiger Entry Point,
- überlappende Kernelbereiche,
- gültige und ungültige frühe Module,
- nicht erlaubte Bootparameter,
- gültiger BIOS-Start,
- BIOS-Start ohne Hardwareanker,
- gültiger UEFI-Secure-Boot-Start,
- deaktiviertes UEFI Secure Boot,
- TPM vorhanden,
- TPM nicht vorhanden,
- TPM-Fehler,
- Measured Boot,
- Boot Event Log,
- gültiger Trust State,
- manipuliertes Boot-Handoff,
- ungültige Handoff-Version,
- Rollback auf erlaubte Generation,
- Rollback auf gesperrte Sicherheitsversion,
- Trial Boot,
- fehlgeschlagener Trial Boot,
- Boot-Failure-Zähler,
- automatischer Recovery-Start,
- ungültiges Recovery-Image,
- Development Mode,
- verweigerter Development-Key im Produktionsmodus,
- Unverified Mode,
- physische Bestätigung,
- Stromausfall während eines Updates,
- atomare Generationsaktivierung,
- Kernel-Selbstverifikation,
- Systeme mit vertrauensunwürdiger Uhr,
- Boot auf älterer Hardware,
- große Manifestdateien,
- Parser-Fuzzing,
- kryptografische Testvektoren,
- Telemetrie und Fehlerdarstellung.

---

## 72. Nicht Bestandteil dieser Entscheidung

Diese ADR legt nicht abschließend fest:

- den konkreten initialen Signaturalgorithmus,
- den konkreten Hashalgorithmus,
- das vollständige binäre Manifestformat,
- die TPM-PCR-Belegung,
- Remote Attestation,
- die vollständige Festplattenverschlüsselung,
- konkrete Schlüsselverwahrungsprozesse,
- Secure-Boot-Zertifizierung einzelner Geräte,
- die vollständige Kernelmodul-Signaturpolicy,
- Firmwareupdates,
- die endgültige öffentliche ABI.

Diese Details werden in nachfolgenden ADRs und NPSPEC-Dokumenten definiert.

---

## 73. Verbindliche Invarianten

1. Der Produktionskernel wird vor seiner Ausführung kryptografisch verifiziert.
2. Das Boot Manifest muss gültig signiert sein.
3. Kernelhash und Manifestangabe müssen übereinstimmen.
4. Kritische frühe Module müssen verifiziert sein.
5. Nicht autorisierte sicherheitskritische Bootparameter werden abgelehnt.
6. Der Kernel validiert das Boot-Handoff erneut.
7. Ein fehlgeschlagener Enforcing-Start darf nicht stillschweigend in einen schwächeren Modus wechseln.
8. Development-, Release- und Recovery-Schlüssel besitzen getrennte Rollen.
9. Widerrufene Schlüssel dürfen keine neuen Produktionsgenerationen autorisieren.
10. Rollback Protection verwendet eine getrennte Sicherheitsversion.
11. Eine Capability allein darf die kryptografische Verifikation nicht umgehen.
12. Recovery muss eine eigene gültige Vertrauenskette besitzen.
13. TPM und UEFI Secure Boot sind unterstützte, aber nicht allgemein verpflichtende Vertrauensanker.
14. Das Fehlen eines Hardware-Vertrauensankers muss im Trust State sichtbar sein.
15. Measured Boot ersetzt Verified Boot nicht.
16. Ein gültiger Hash ohne gültige Manifest-Signatur beweist keine Authentizität.
17. Systemgenerationen werden nur atomar aktiviert.
18. Eine neue Generation wird erst nach erfolgreichem Health Check bestätigt.
19. Trust-State-Daten werden nach der Kernelübernahme gegen normale Veränderungen geschützt.
20. Boottelemetrie wird ohne ausdrückliche Policy nicht automatisch extern übertragen.
21. Unbekannte kritische Manifestfelder oder Algorithmen werden abgelehnt.
22. Kryptografische Bootparser müssen Größen, Längen und Integerüberläufe vollständig validieren.
23. NovaOS darf keine stärkere Vertrauensgarantie anzeigen, als die aktive Plattform tatsächlich bereitstellt.
24. KI- oder lernbasierte Komponenten dürfen Verifikations-, Schlüssel-, Rollback-, Recovery- oder Trust-State-Regeln nicht umgehen.

---

## 74. Referenzen

- ADR-BOOT-0001 – BIOS und UEFI parallel unterstützen
- ADR-BOOT-1012 – Eigenes Boot-Handoff-Protokoll
- ADR-KERNEL-0100 – Kernel Object Graph statt isolierter Subsysteme
- ADR-KERNEL-0101 – Ereignisgesteuerter Kernel-Core
- ADR-KERNEL-0102 – Einheitliches Objektmodell für Kernel und Userspace
- ADR-KERNEL-0103 – Native Capability-basierte Ressourcenverwaltung
- ADR-KERNEL-0104 – Integrierte Telemetrie- und Diagnoseschnittstellen
- ADR-KERNEL-0105 – ABI-Stabilität über versionierte Kernel-Services
- ADR-PROC-0001 – Process Lifecycle
- ADR-PROC-0002 – Thread Lifecycle
- NPSPEC-BOOTHANDOFF-0001 – Boot Handoff Specification
- NPSPEC-KERNELENTRY-0001 – Kernel Entry Specification
- NPSPEC-BOOTERROR-0001 – Boot Error Specification
- NPSPEC-SECURESTARTUP-0001 – Secure Kernel Startup Specification
- NPSPEC-KERNELMODULE-0001 – Kernel Module Specification
- NPSPEC-RECOVERY-0001 – Recovery Specification

---

## 75. Zusammenfassung der Entscheidung

NovaOS verwendet eine eigene verifizierte Bootkette für den sicheren Kernelstart.

Der Bootloader prüft ein signiertes Boot Manifest, den Kernel, frühe Module, Bootparameter und die zugehörige Systemgeneration, bevor der Kernel ausgeführt wird. Der Kernel validiert anschließend das Boot-Handoff und den übergebenen Trust State erneut.

Die NovaOS-Verifikation funktioniert sowohl unter BIOS als auch unter UEFI. UEFI Secure Boot, TPM, Measured Boot und weitere Hardware-Vertrauensanker werden zusätzlich genutzt, sind aber nicht allgemein verpflichtend.

Jede Systemgeneration besitzt ein eigenes Manifest sowie eine getrennte Sicherheitsversion. Dadurch kann NovaOS funktionale Rollbacks zulassen, ohne einen Rückfall auf sicherheitskritisch gesperrte Versionen zu erlauben.

Produktions-, Entwicklungs- und Recovery-Schlüssel werden getrennt. Ein fehlgeschlagener Produktionsstart darf niemals automatisch in einen weniger sicheren Modus wechseln.

Ein separat signiertes Recovery-System ermöglicht Reparatur und Wiederherstellung, ohne die Vertrauenskette zu umgehen. Der erreichte Sicherheitszustand wird als strukturierter Trust State an den Kernel übergeben und bleibt für autorisierte lokale Diagnose verfügbar.