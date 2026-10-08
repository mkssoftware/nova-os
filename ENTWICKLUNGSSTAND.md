# NovaOS – aktueller Entwicklungsstand

**Stand:** 8. Oktober 2026
**Projektpfad:** `C:\recoverboot\nova-os`  
**Aktueller Schwerpunkt:** UEFI-Bootpfad, Kernel-Handoff und persistentes NovaFS-Systemvolume

Diese Datei fasst den bisher implementierten und getesteten Stand zusammen. Die
ADRs und NPSPECs unter `docs/` bleiben die normative Quelle. Diese Übersicht ist
kein Ersatz für die Spezifikationen, sondern dokumentiert deren bisherigen
Umsetzungsstand.

## 1. Erzeugte Startmedien

Die Namen der Entwicklungsabbilder bleiben dauerhaft unverändert:

| Artefakt | Zweck | Aktueller Stand |
|---|---|---|
| `build/nova-uefi.img` | GPT mit FAT32-ESP und NovaFS-Systempartition | Aktueller Entwicklungsstand |
| `build/novafs-system.img` | Vorlage für ein frisch formatiertes NovaFS-Systemvolume | nur bei `make novafs-reset` erneut verwendet |
| `build/nova-bios.img` | Raw-Image für Legacy-BIOS | Letzter BIOS-Entwicklungsstand |
| `build/kernel.nki` | Bevorzugter Nova-Kernelcontainer | Aktueller Kernel |
| `kernel/build/kernel.elf` | Direkt ladbares ELF32 | Aktueller Kernel |
| `kernel/build/kernel.bin` | Kernel-Rohabbild | Aktueller Kernel |

Derzeit wird bewusst keine ISO bei jedem Arbeitsschritt erzeugt. Das UEFI-Image
ist das primäre Testartefakt. Konvertierungsskripte für ISO-Abbilder liegen im
Projektstamm.

## 2. Architekturentscheidungen, die bereits berücksichtigt werden

- UEFI ist der bevorzugte Bootpfad; Legacy-BIOS bleibt als separater Fallback
  erhalten.
- BIOS und UEFI übergeben dem Kernel denselben TLV-basierten NBHP/BIB-Vertrag.
- NKI ist das bevorzugte Produktionsformat.
- Gültige ELF32- und ELF64-Dateien können vom UEFI-Loader direkt geladen
  werden.
- Kernel und Bootloader bleiben über den versionierten Handoff getrennt.
- Boot-UI-Koordinaten und Layoutdefinitionen verwenden DLU als logische Einheit.
- Der Kernel erhält genau einen Zeiger auf den zusammenhängenden BIB.
- Firmwareabhängige Daten werden vor dem Kernelstart normalisiert.
- Semantic Types ergänzen primitive Repräsentationstypen und werden nicht durch
  diese ersetzt.

## 3. Bootloader und Bootmanager

### Gemeinsame Boot-UI-Grundlage

Im Bootmanager existieren inzwischen Komponenten für:

- Seiten- und Navigationsmodell
- Scene Graph, Render Queue, Layer und Surfaces
- Dirty-Region-Verwaltung und partielles Neuzeichnen
- Present Scheduler und Compositor
- Software-Renderer und Framebuffer-Backend
- DLU-basiertes Layout
- abgerundete Geometrie, Vektorgeometrie und SVG-Rendering
- Clip-Masken und 2D-Transformationen
- Bilder, Effekte und Hintergrundunschärfe
- Theme- und Darstellungsverwaltung
- Unicode-Text, Umlaute und Font-Ressourcen
- Icons, Branding und NovaOS-Logo
- Tastatur-, Maus-, Pointer- und Fokusverarbeitung
- Dialoge, Bestätigungen, Warnungen und Tooltips
- Listen, Scrollbereiche, Fortschrittsanzeigen und Recovery-Kacheln
- Diagnose-, Konfigurations-, Runtime- und Zustandsmodule
- Boot- und UI-Diagnosemarken für automatisierte QEMU-Tests

Der Bootmanager besitzt einen sichtbaren Countdown. Ohne Eingabe wird nach fünf
Sekunden der erste Eintrag gestartet. Eine Eingabe beendet die automatische
Auswahl und entfernt den Countdown-Hinweis. Noch nicht ausführbare Funktionen
können ihren Hinweis im dafür vorgesehenen Statusbereich anzeigen.

### Gestaltung

Die Oberfläche wurde an die bereitgestellten NovaOS-Entwürfe angenähert:

- dunkles Nova-Farbschema
- blaue Akzentfarbe für normale Bootansichten
- rote beziehungsweise magentafarbene Akzente für Fehleransichten
- NovaOS-Schriftzug und Morschustier-Branding
- unterschiedliche Logoausrichtung für normale Ansichten und Fehlerseiten
- gerundete obere Akzentleiste, Panels, Auswahlflächen und Schaltflächen
- Bootmanager-Icons für Start, Installation, Einstellungen, Diagnose,
  Wiederherstellung und Ausschalten
- geglättete Schriftwiedergabe über die Font- und Software-Render-Pipeline
- einheitlicher Fehlerbildschirm mit verständlichem Erklärungstext,
  Fehlercode und Nova-Smiley

Die grafische Exaktheit muss bei späteren Designänderungen weiterhin anhand der
Referenzbilder und realer Screenshots geprüft werden.

## 4. UEFI-Bootpfad

Der UEFI-Pfad ist momentan der aktive Entwicklungsschwerpunkt.

### EFI-Anwendung und Startmedium

- `BOOTX64.EFI` wird als eigenständige UEFI-Anwendung gebaut.
- Das UEFI-Abbild enthält eine GPT und eine FAT32-EFI-Systempartition.
- Die Standardposition `EFI/BOOT/BOOTX64.EFI` wird verwendet.
- Das Image enthält den bevorzugten Kernel `NOVA.NKI` und zusätzlich
  `KERNEL.ELF` als direkten Fallback.
- Für QEMU wird ein zusammengesetztes EDK2/OVMF-Firmwareabbild unter
  `build/uefi/edk2-x86_64.fd` verwendet.

### Grafik und Eingabe

- GOP wird als UEFI-Grafikbackend verwendet.
- Framebuffer-Adresse, Auflösung, Pitch, Bittiefe und Pixelformat werden in den
  NBHP/BIB übernommen.
- Tastatur sowie relative und absolute Pointer-Protokolle sind angebunden.
- Die Boot-UI besitzt Software-Rendering und kann ohne Firmware-Zeichenroutinen
  weiterarbeiten.

### Kerneldateien und Formaterkennung

Der Loader arbeitet in folgender Reihenfolge:

1. `NOVA.NKI` suchen und als bevorzugtes Format validieren.
2. NKI-Header, Architektur, Flags, Größe und CRC32 prüfen.
3. Das enthaltene ELF32 zusätzlich vollständig validieren.
4. Wenn `NOVA.NKI` nicht existiert, `KERNEL.ELF` direkt laden.
5. Fehlt auch ELF32, `KERNEL64.ELF` als direkten ELF64-Kernel laden.
6. Ein vorhandenes, aber ungültiges NKI führt kontrolliert zum Fehler und wird
   nicht stillschweigend durch ELF umgangen.

Für ELF32 werden derzeit geprüft:

- ELF-Magic, Klasse, Little Endian und ELF-Version
- `ET_EXEC` und `EM_386`
- Header- und Program-Header-Größen
- Grenzen sämtlicher Program Header
- vorhandene `PT_LOAD`-Segmente
- `p_filesz <= p_memsz`
- Adressüberläufe und Dateigrenzen
- Alignment als Zweierpotenz und Adress-/Datei-Kongruenz
- Segmentüberlappungen
- W^X: gleichzeitig schreibbare und ausführbare Segmente werden abgewiesen
- BSS-Nullinitialisierung
- ausführbarer und innerhalb eines Segmentes liegender Einstiegspunkt
- GNU-Build-ID aus `PT_NOTE`
- Nova-Metadatenversion und minimale Loader-ABI
- erforderliche CPUID-Featurebits

NKI- und ELF-Build-ID müssen zusammenpassen. Der BIB kennzeichnet außerdem, ob
der geladene ELF-Kernel aus einem NKI-Container oder aus einer direkten
ELF-Datei stammt.

Für direktes ELF64 werden zusätzlich ELF-Klasse 64, `EM_X86_64`, 64-Bit-
Program-Header und ein vollständig unterhalb der aktuellen 4-GiB-BIB-Grenze
liegendes Ladelayout geprüft. Der Loader validiert auch hier GNU-Build-ID,
Nova-Requirements, W^X, Alignment, Segmentüberlappungen und Einstiegspunkt. Der
UEFI-x64-Loader bleibt nach `ExitBootServices()` im Long Mode und übergibt über
ein separates, nicht zurückkehrendes 64-Bit-Trampolin.

### Speicherkarte und `ExitBootServices()`

Die UEFI-Speicherübergabe wurde gehärtet:

- Die Firmware bestimmt zunächst die benötigte Größe der Memory Map.
- Der Puffer wird dynamisch mit zusätzlichem Deskriptor-Spielraum angelegt.
- Ein erneutes `EFI_BUFFER_TOO_SMALL` führt zu kontrollierter Vergrößerung.
- Deskriptorgröße, Längen und Adressüberläufe werden validiert.
- Die normalisierte Speicherkarte umfasst maximal 256 Einträge.
- Eine Überschreitung wird als Fehler gemeldet und nicht still gekürzt.
- Vor jedem `ExitBootServices()`-Versuch wird eine frische Memory Map gelesen.
- Der vollständige NBHP/BIB wird aus genau dieser aktuellen Karte neu erzeugt.
- Nach einem ungültigen Map-Key erfolgen maximal drei kontrollierte Versuche.
- Zwischen finalem `GetMemoryMap()` und `ExitBootServices()` findet keine
  weitere UEFI-Speicherallokation statt.

Vorhandene Diagnosemarken sind unter anderem:

```text
UEFI:NKI-VALIDATED
UEFI:ELF32-DIRECT-VALIDATED
UEFI:ELF64-DIRECT-VALIDATED
UEFI:NBHP-BIB-READY
UEFI:EXIT-BOOT-SERVICES-RETRY
UEFI:EXIT-BOOT-SERVICES-READY
UEFI:KERNEL-HANDOFF-READY
```

### NBHP/BIB-Inhalt des UEFI-Loaders

Der aktuelle BIB ist versioniert, zusammenhängend, TLV-basiert und enthält:

- Firmwaretyp UEFI
- normalisierte Speicherkarte
- optionale GOP-/Framebufferdaten
- Kernel-Ladeadresse, Größe und Einstiegspunkt
- Security-Status
- CPUID-Hersteller und Featurebits
- frühen Entropie-Seed
- System-/Bootversuchsinformationen
- validierten ACPI-RSDP, wenn vorhanden
- Kernel-Build-ID und Kernelformat

Der Security-TLV verwendet keine festen Platzhalter mehr. Er unterscheidet:

- unbekannten Prüfzustand,
- strukturell validiertes direktes ELF,
- integritätsgeprüftes NKI mit CRC32 und passender innerer ELF-Build-ID,
- signaturgeprüft und policyautorisiert als reservierte höhere Zustände.

Da das aktuelle NKI-Format noch keinen kryptografischen Signaturcontainer
enthält, wird ausdrücklich **nicht** behauptet, der Kernel sei signaturgeprüft.
Die Diagnose meldet stattdessen `UEFI:KERNEL-SIGNATURE-NOT-PRESENT`.

Der Firmwarezustand wird unmittelbar vor der Kernelprüfung erneut aus den
UEFI-Variablen gelesen und als `Unknown`, `Disabled`, `Enabled` oder
`SetupMode` in den BIB übertragen. Zusätzliche Security-Flags unterscheiden
bekannten Firmwarezustand, gültige ELF-Build-ID, gültige NKI-CRC32 und eine
zukünftig vorhandene Signatur. UEFI Secure Boot bleibt damit zusätzliche
Evidenz und wird nicht mit NovaOS-Trust oder Ausführungsautorität gleichgesetzt.

Unbekannte optionale TLVs können vom Kernel anhand ihrer Länge übersprungen
werden. Unbekannte als erforderlich markierte TLVs führen zum kontrollierten
Bootabbruch.

### Kernelübergang

- Der normale NovaOS-Entwicklungskernel wird derzeit als x86-32-Kernel geladen.
- Direkte ELF64-Testkernel werden als x86-64 geladen und im Long Mode gestartet.
- Der UEFI-x64-Loader wechselt über einen eigenen Trampolinpfad in den vom
  Kernel erwarteten CPU-Zustand.
- Der Einstiegspunkt, der einzelne BIB-Zeiger und ein reservierter Kernelstack
  werden übergeben.
- Ist die bevorzugte feste Stackadresse nicht verfügbar, wird ein geeigneter
  Stack unterhalb der unterstützten Adressgrenze dynamisch reserviert.
- Nach erfolgreichem `ExitBootServices()` werden keine Boot Services mehr
  verwendet.

## 5. Legacy-BIOS-Pfad

Der vorhandene BIOS-Stand umfasst:

- 512-Byte Stage 1 mit Bootsignatur
- Stage 2 und Protected-Mode-Übergang
- VBE-Grafik mit kontrolliertem Textmodus-Fallback
- Bootmanager und Kernelstart
- NKI-Validierung und ELF-basierte Kernelladung
- TLV-basierten NBHP/BIB-Handoff
- Fehleransicht bei beschädigtem oder unvollständigem Kernel
- QEMU-Boot bis `NOVA_KERNEL_READY`

Seit der Festlegung des aktuellen Arbeitsschwerpunkts wurde das BIOS-Image nicht
weiter verändert oder erneut gebaut.

## 6. Kernel

Der aktuelle Kernel startet über BIOS und UEFI; die neuesten Arbeiten wurden
nur noch über UEFI getestet.

### Früher Kernelstart

- Kernel Entry und strukturierte Bootphasen
- Validierung von NBHP/BIB-Header, Version, Größe und Prüfsumme
- TLV-Parser mit Required-/Optional-Semantik
- Übernahme der Kernel-Build-ID
- ACPI-RSDP-Prüfung und MADT-Auswertung
- Erkennung der vorhandenen CPUs
- Framebuffer- und Kernelkontextübernahme

### Implementierte Kernelgrundlagen

Die Startdiagnose meldet aktuell funktionsfähige Grundlagen für:

- Logging mit Ring- und Reservepuffer
- Panic Reporter und reservierten Crash-Dump-Pfad
- Physical Memory Manager und Seitentest
- Heap und Schreibtest
- Object Manager
- Component Manager
- Paging und Speichertest
- IDT, PIC und PIT mit 100 Hz
- IPC mit begrenzter FIFO
- Service Manager
- Process Manager
- Security-/Capability-Grundlage
- CPU Manager mit BSP-Topologie und Per-CPU-Daten
- Module Loader mit Trust-, ABI- und W^X-Prüfungen
- Thread Manager
- Scheduler mit zwei Testthreads
- SMP-Grundlage, BSP-Barriere und lokaler TLB-Pfad
- Device Manager
- VFS, Mount-Namespace und Bootstrap-Root
- Netzwerkgrundlage für IPv4, IPv6, UDP, ICMP und TCP
- Power Manager und Shutdown-Pfad
- x86-32-Userspace, TSS und System-Call-ABI
- Shared Service Page

### Userspace-Selbsttests

Der Bootstrap-Userspace prüft unter anderem:

- `Process.QuerySelf` und `Process.OpenSelf`
- `Thread.QuerySelf` und `Thread.OpenSelf`
- typ- und rechtegeprüfte Handles
- atomaren IPC-Inline-Roundtrip
- VFS-Root und Lookup von `/`
- Schließen und Ungültigwerden alter Handles
- Power-Abfragen, Wake-Locks und Profile
- Ablehnung eines Shutdowns ohne Capability
- IPv4-/IPv6-Socketobjekte
- UDP-Loopback und begrenzte Warteschlangen
- TCP-Stream, Listener und Accept
- Capability-Prüfung für Raw Sockets
- Logging-Abfragen und atomar veröffentlichte Records
- Shared Service Page

### SMP-Stand

ACPI/MADT erkennt in den aktuellen QEMU-Tests vier CPUs. Der BSP läuft; alle
weiteren APs werden per INIT-SIPI-SIPI-Sequenz hochgefahren (§124).

#### §124 – AP-Aktivierung und echter SMP-Betrieb

- **AP-Trampoline** (16-Bit-Blob bei 0x8000): `ap_trampoline_blob` wird vom BSP
  dorthin kopiert; Patch-Bereich enthält GDT-Limit/Basis und den 32-Bit-Einsprung
  `ap_entry32_pm`; `lgdt [0x8002]` + `o32 jmp far [0x8008]` schalten den AP in
  Protected Mode.
- **INIT-SIPI-SIPI**: `smp_boot_ap` sendet INIT, wartet ~10 ms, dann zwei SIPIs
  mit Vektor 0x08 (= 0x8000 >> 12) über den xAPIC-MMIO-ICR; `apic_wait_icr_idle`
  prüft Delivery-Status-Bit.
- **AP-Einsprung** (`ap_entry32_pm`): AP lädt Kernel-Segmente und IDT, sucht
  per CPUID-APIC-ID seinen Slot in `acpi_apic_ids`, richtet Stack aus
  `ap_stack_area` ein (Slot × 4 KiB), aktiviert Spurious-Interrupt-Enable im
  LAPIC und setzt `cpu_online_set`, `cpu_active_set`, `cpu_online_count` und
  `ap_alive_count` atomar.
- **`smp_start_aps`**: iteriert über alle entdeckten CPUs (Slot 1…N), ruft
  `smp_boot_ap` auf und wartet je AP auf `cpu_online_count`.
- **`smp_send_ipi`**: iteriert über Ziel-Bitmask, schlägt APIC-ID per Slot nach,
  schreibt Fixed-IPI (Vektor 0xFE) in ICR – tatsächliches Senden statt Stub.
- **`smp_tlb_shootdown_page`**: lokales `invlpg` für BSP-Bit, danach
  `smp_send_ipi` mit `SMP_IPI_TLB_SHOOTDOWN` für entfernte APs (fire-and-forget).
- **`smp_self_test`**: prüft UP- und SMP-Pfad; erwartet `(1 << cpu_discovered_count) - 1`
  als `cpu_online_set`/`cpu_active_set`; im SMP-Fall wird geprüft, dass ein IPI
  an CPU 1 tatsächlich `smp_remote_ipis_sent` inkrementiert.
- **Stapelspeicher**: `ap_stack_area` – 7 × 4 KiB, page-aligned nach dem
  Datensegment; `ap_alive_count` – atomarer Zähler.
- Assembly-Verifikation: NASM 2.16.01, 239 268 Bytes, kein Assemblerfehler.

## 7. Semantic Types

Auf Grundlage der Semantic-ADRs wurde ein erster Kernelkern für semantische
Typisierung umgesetzt.

### Registry und Typidentität

- namespacefähige stabile Typnamen
- internierte Type Handles für schnelle Kernelpfade
- explizite Typversionen
- getrennte primitive Repräsentation und semantische Identität
- Boot-Control-Plane für Registrierung
- versiegelte Registry vor Userspace und Schedulerbetrieb
- Ablehnung nachträglicher oder mehrdeutiger Registrierung

Aktuell registrierte Demonstrationstypen:

```text
nova.kernel.ipc.inline-data
nova.kernel.ipc.diagnostic
```

Beide verwenden technisch `Bytes8`, gelten dadurch aber nicht automatisch als
semantisch gleich.

### Kompatibilität und Konvertierung

Die Kompatibilitätsprüfung unterscheidet:

- `Unknown`
- `Exact`
- `Subtype` (für spätere Spezifikation reserviert)
- `Trait` (für spätere Spezifikation reserviert)
- `Convertible`
- `Incompatible`

Konvertierungen werden explizit mit Source Type, Source Version, Target Type,
Target Version, Capability-ID und Verlustklasse registriert. Lossless und Lossy
werden unterschieden. Die Registry beschreibt nur zulässige Pfade; sie führt
keine stillschweigende Konvertierung aus.

### Mehrere Semantic Types pro Ressource (§119)

Ein Object-Handle kann neben seinem Primary Type bis zu vier Secondary Types tragen.

- `object_semantic_attach_secondary` hängt einen weiteren Typ an ein Objekt; Primary muss bereits gesetzt sein, Secondary darf weder dem Primary noch einem bereits registrierten Secondary entsprechen.
- `object_semantic_has_type` prüft, ob ein Typ als Primary oder als Secondary eines Objekts eingetragen ist.
- `object_semantic_query_secondary` fragt einen Secondary-Slot nach Index ab und liefert Type Handle, Version und ValidationStatus.
- `object_semantic_secondary_count` gibt die Anzahl aktuell gesetzter Secondary Types zurück.
- `object_semantic_clear_secondary` setzt alle Secondary-Slots zurück (nach `object_release` aufzurufen, damit der Slot für neue Objekte sauber ist).
- `semantic_initialize` löscht die neuen Secondary-Sidecar-Arrays beim Start explizit.
- `object_semantic_attach` setzt beim Setzen des Primary Types den Secondary-Count auf 0 (Slot-Wiederverwendung sauber).
- Der `semantic_self_test` prüft Positive- und Negativfälle: Secondary anhängen, Slot abfragen, `has_type` für Primary und Secondary, Duplikat- und Primary-Gleichheitsabweisung, Count-Abfrage, Freigabe mit Clear.

### Semantic Discovery (§122)

Die vollständige Semantic-Registry kann jetzt zur Laufzeit abgefragt werden.

- `semantic_find_by_name` — ESI=NUL-terminierter Name; gibt EAX=type_handle zurück (CF=1 wenn nicht gefunden); durchsucht alle registrierten Typen nach exaktem Namens-Match.
- `semantic_query_type_info` — EAX=handle; gibt ECX=representation, EDX=version, ESI=Zeiger auf den Namen im Record zurück.
- `semantic_enumerate_type` — EAX=0-basierter Index; gibt EAX=handle, ECX=repr, EDX=version, ESI=name_ptr zurück; CF=1 bei Index ≥ type_count.
- `semantic_enumerate_conversion` — EAX=Index; füllt `semantic_disc_src/src_v/tgt/tgt_v/cap/loss`; CF=1 bei Index ≥ conversion_count.
- `semantic_enumerate_subtype` — EAX=Index; füllt `semantic_disc_child/child_v/parent/parent_v`; CF=1 bei Index ≥ subtype_count.
- `semantic_enumerate_trait` — EAX=Index; füllt `semantic_disc_type/type_v/trait/trait_v`; CF=1 bei Index ≥ trait_count.
- `semantic_self_test` erweitert mit 13 neuen §122-Fällen: find_by_name (Treffer INLINE_DATA, Treffer TRAIT_READABLE, Miss), query_type_info, enumerate_type (Indizes 0, 2, 3→Fehler), enumerate_conversion (Index 0 mit Feldprüfung, Index 1→Fehler), enumerate_subtype, enumerate_trait.

### Semantic Relationships – Subtypes und Traits (§121)

Typen können jetzt explizite Hierarchie- und Rollenbeziehungen deklarieren.

- `semantic_register_subtype` — registriert `child < parent` (beide Typen müssen existieren, pre-seal, keine Duplikate; EAX=child, ECX=child_ver, EDX=parent, EBX=parent_ver).
- `semantic_register_trait` — registriert, dass ein Typ ein Trait implementiert (gleiche Signatur, gleiche Invarianten; EAX=type, EDX=trait).
- `semantic_compatibility` prüft jetzt in dieser Reihenfolge: EXACT → SUBTYPE (direkte Subtype-Registry) → TRAIT (direkte Trait-Registry) → CONVERTIBLE (Conversion-Registry) → INCOMPATIBLE.
- Dritter registrierter Demonstrationstyp: `nova.kernel.trait.readable` (Handle 3, `SEMANTIC_TRAIT_READABLE`).
- In `semantic_initialize` registriert: DIAGNOSTIC < INLINE_DATA (Subtype), INLINE_DATA implements readable (Trait).
- `semantic_self_test` aktualisiert: DIAGNOSTIC→INLINE_DATA = SUBTYPE (war INCOMPATIBLE); neue Fälle: INLINE_DATA→TRAIT_READABLE = TRAIT; DIAGNOSTIC→TRAIT_READABLE = INCOMPATIBLE (keine direkte Registrierung); Duplikat-Subtype nach Versiegelung scheitert; INLINE_DATA→DIAGNOSTIC bleibt CONVERTIBLE.

### Ausführung registrierter Conversion Capabilities (§120)

Konvertierungen können jetzt tatsächlich ausgeführt werden, nicht nur deklariert.

- `semantic_register_conversion_handler` registriert einen Funktionszeiger für eine Capability-ID (nur vor Registry-Versiegelung, max. 8 Handler, keine Duplikate).
- `semantic_execute_conversion` führt eine Konvertierung durch: validiert Source- und Target-Type-Version, sucht den passenden Konvertierungspfad in der Registry, findet den registrierten Handler für die Capability-ID und ruft ihn auf.
- Der eingebaute Handler `semantic_handler_inline_to_diag` konvertiert `nova.kernel.ipc.inline-data → nova.kernel.ipc.diagnostic`: kopiert die ersten 4 Bytes als Diagnose-Code (muss ≠ 0 sein, gemäß `RULE_DIAGNOSTIC_CODE`); gibt 4 oder 0 zurück.
- Der Aufrufer füllt vor dem Aufruf das Scratch-Bereich `semantic_exec_*` (src/tgt Type+Version, src_ptr, src_len, dst_ptr, dst_max); danach stehen `semantic_exec_dst_written` und der geschriebene Inhalt im Zielpuffer bereit.
- `semantic_initialize` registriert den eingebauten Handler direkt nach der Konvertierungsdeklaration, vor dem Versiegeln der Registry.
- Der `semantic_self_test` prüft: INLINE_DATA→DIAGNOSTIC gelingt (CF=0, EAX=4, Zielpuffer enthält `0x41564F4E`); DIAGNOSTIC→INLINE_DATA scheitert (kein Konvertierungspfad → CF=1).

### Typed Resources, IPC und Capabilities

- Object-Handles besitzen getrennte Semantic-Type-Sidecars.
- Typinformation ersetzt weder Objektidentität noch Rechteprüfung.
- IPC-Pakete tragen Type Handle und Version.
- Endpoint-Contract, User-Claim, Handle-Rechte und Payloadprüfung bleiben
  getrennte Schritte.
- Falscher Typ und falsche Version werden vor Queue-Mutation abgewiesen.
- Service Contracts besitzen deklarierte Input- und Outputtypen.
- Gleiche primitive Repräsentation reicht nicht für Providerkompatibilität.
- Eine registrierte Conversion Capability bleibt explizit erforderlich.

### Semantic Validation

Typkompatibilität und Wertgültigkeit werden getrennt behandelt:

1. Type Handle und Version prüfen.
2. typspezifische Validation Rule ausführen.
3. zusätzliche Capability-Contract-Regeln anwenden.
4. erst danach die Operation ausführen.

Implementiert sind:

- deklarierte Validator-ID pro Typ
- `NONEMPTY`-Regel für Inline-Daten
- Regel für einen vorhandenen Diagnosecode
- zusätzliches Längenmaximum aus dem Capability Contract
- Validierungszustände `Declared`, `Verified` und `Invalid`
- eigener System-Call-Status für Validierungsfehler
- strukturierter Fehler mit Regel, Erwartungswert und Istwert
- Garantie, dass Validierung den geprüften Wert nicht verändert

Ein Userspace-Test sendet absichtlich einen korrekt typisierten und korrekt
versionierten, aber leeren IPC-Wert. Dieser wird als Validierungsfehler getrennt
von einem Typfehler abgewiesen.

## 8. Buildsystem und Artefaktprüfung

Das Buildsystem prüft beziehungsweise erzeugt unter anderem:

- C-Layoutchecks für Bootprotokoll und Kernel-ABI
- exakte Größe und Signatur der BIOS Stage 1
- Größenlimit des BIOS Stage 2
- Kernelgrößenlimit
- ELF32-Container mit GNU-Build-ID und Nova-Requirements-Note
- NKI mit CRC32 und Build-ID
- separate `BACKUP.NKI`- und `RECOVERY.NKI`-Container im UEFI-Startmedium
- Übereinstimmung der NKI- und ELF-Build-ID
- GPT/FAT32-UEFI-Abbild
- EFI-Anwendung und zusammengesetzte EDK2-Firmware

Das Root- und Kernel-Makefile berücksichtigt jetzt den Kernel-Assemblerquelltext,
alle `arch/x86_64/*.inc`-Dateien und die gemeinsamen ABI-Includes als echte
Abhängigkeiten. Änderungen daran lösen dadurch sicher einen Neuaufbau von
`kernel.bin`, ELF, NKI, Recovery-NKI und UEFI-Image aus.

Wichtige Befehle:

```powershell
# Nur UEFI-Anwendung und aktuelles UEFI-Image bauen
& 'C:\msys64\usr\bin\bash.exe' -lc 'cd /c/recoverboot/nova-os && make uefi-image'

# Kernel bauen
& 'C:\msys64\usr\bin\bash.exe' -lc 'cd /c/recoverboot/nova-os && make kernel'

# BIOS-Image bauen – derzeit nicht der Arbeitsschwerpunkt
& 'C:\msys64\usr\bin\bash.exe' -lc 'cd /c/recoverboot/nova-os && make image'
```

## 9. Zuletzt erfolgreich geprüfter UEFI-Start

Der aktuelle UEFI-Stand wurde unter QEMU mit Q35, EDK2, 256 MiB RAM und vier
virtuellen CPUs geprüft. Der bevorzugte NKI-Pfad meldete:

```text
UEFI:NKI-VALIDATED
UEFI:NBHP-BIB-READY
UEFI:EXIT-BOOT-SERVICES-READY
UEFI:KERNEL-HANDOFF-READY
NOVA_KERNEL_READY
```

Zusätzlich wurde ein temporäres UEFI-Testabbild ohne NKI erzeugt. Der direkte
ELF32-Fallback meldete:

```text
UEFI:ELF32-DIRECT-VALIDATED
UEFI:NBHP-BIB-READY
UEFI:EXIT-BOOT-SERVICES-READY
UEFI:KERNEL-HANDOFF-READY
NOVA_KERNEL_READY
```

Ein weiteres temporäres Abbild enthielt ausschließlich `KERNEL64.ELF`. Der
direkte ELF64-Pfad meldete:

```text
UEFI:ELF64-DIRECT-VALIDATED
UEFI:NBHP-BIB-READY
UEFI:EXIT-BOOT-SERVICES-READY
UEFI:KERNEL-HANDOFF-READY
NOVA_ELF64_LONG_MODE_READY
```

Die UEFI-Kernellader-Fehlerpfade besitzen außerdem einen automatisierten
Negativtest (`make test-uefi-kernel-validation`). Er erzeugt ausschließlich
temporäre Abbilder und prüft:

- beschädigtes NKI bei gleichzeitig vorhandenem gültigem ELF32,
- ungültiges direktes ELF32,
- ungültiges direktes ELF64,
- kontrollierte Meldung von `UEFI:KERNEL-VALIDATION-ERROR`,
- ausbleibenden `UEFI:KERNEL-HANDOFF-READY`-Marker,
- keinen stillschweigenden ELF-Fallback bei vorhandenem, aber ungültigem NKI,
- Vorrang von `BACKUP.NKI` vor einem gleichzeitig vorhandenen Recovery-NKI,
- automatischen Wechsel auf ein separat validiertes `RECOVERY.NKI`, wenn der
  Hauptkernel beschädigt ist,
- Übertragung und Kernel-seitige Bestätigung des Recovery-Modus im
  NBHP/BIB-Boot-Options-TLV.

Die drei isolierten Negativfälle werden vor dem Kernel-Handoff abgewiesen. Ein
vierter Test startet bei beschädigtem Haupt-NKI vorrangig das unabhängig
adressierte Backup-NKI. Ein fünfter Test entfernt die Backupgeneration und
bestätigt den Wechsel auf Recovery bis `NOVA_KERNEL_READY`. Die temporären
Images und Logs werden anschließend automatisch entfernt.

Das temporäre Testabbild wurde danach entfernt. Im Buildordner bleiben die
festgelegten Image-Namen erhalten.

### Persistenter UEFI-Boot-Control-Zustand

Der UEFI-Pfad besitzt jetzt einen ersten persistenten A/B-Boot-Control-Kern:

- zwei abwechselnd beschriebene UEFI-Variablen (`NovaBootState0` und
  `NovaBootState1`),
- 64-Byte-Datensätze mit Formatversion, Sequenznummer und CRC32,
- Auswahl der neuesten noch gültigen Kopie beim Start,
- logischer Active-, Candidate- und Known-Good-Slot,
- policyfähiges Versuchslimit im Datensatz statt einer fest verdrahteten
  Entscheidung im Auswahlalgorithmus,
- sofortige Candidate-Deaktivierung bei einem eindeutig ungültigen Artefakt,
- automatischer Wechsel zur Known-Good-Generation nach Erreichen des
  Versuchslimits,
- erzwungener automatischer Recovery-Start, wenn vorhandene Bootmetadaten in
  beiden Kopien ungültig sind; die beschädigten Datensätze werden dabei nicht
  still mit einem normalen Standardzustand überschrieben,
- sicherer flüchtiger Betrieb, falls UEFI-Variablen nicht geschrieben werden
  können.

Die Initialisierung schreibt beide redundanten Kopien und liest jede Änderung
nach dem Schreiben zur Validierung zurück. Der isolierte Zustandsautomat prüft
beide Slotrichtungen, alle Versuchslimits von 1 bis 16, deterministische
Auswahl, Limit-Rollback, Artefaktfehler, abgelehnte Übergänge, CRC-Erkennung und
den Überlauf der Sequenznummer. Ein zusätzlicher QEMU-Test startet mit einer
beschreibbaren Firmwarekopie, beschädigt danach gezielt den neuesten
Boot-State-Datensatz und startet erneut. Der zweite Start verwirft die defekte
Kopie, stellt die ältere gültige Kopie wieder her und erreicht wie der erste den
vollständigen primären NKI-Handoff bis `NOVA_KERNEL_READY`. Vor einem dritten
Start werden beide Kopien beschädigt. Dieser Lauf muss `RECOVERY.NKI` auswählen,
den automatischen Recovery-Modus über NBHP/BIB an den Kernel übertragen und
ebenfalls `NOVA_KERNEL_READY` erreichen.

Der sicherheitskritische Health-Commit ist nun als deterministischer
Boot-Control-Zustandsautomat umgesetzt. Kernel Entry allein markiert weiterhin
keinen Candidate als Known-Good. Ein Commit verlangt vollständige Required-
Milestones, passende Slot-/Generations-/Attempt-Identität, bestätigten Trust
und eine vorgelagerte Capability-Autorisierung. Offen ist noch die geschützte
Transportbrücke vom laufenden Kernel beziehungsweise den kritischen
Userspace-Diensten zur persistenten UEFI-Boot-Control-Autorität.

## 10. Noch offene oder nur teilweise umgesetzte Punkte

Die folgenden Bereiche sind noch nicht vollständig abgeschlossen:

- ~~LZ4-Dekompression für Kernelabbilder~~ (§113: LZ4-Block-Decompressor in `kernel_loader.c`, `build-nki.ps1 -Compress`, zwei Testfälle in `test-uefi-kernel-validation.ps1`)
- ~~ZSTD-Dekompression für Kernelabbilder~~ (§114: ZSTD-Frame-Decompressor in `kernel_loader.c`, `build-nki.ps1 -CompressZstd`, zwei Testfälle in `test-uefi-kernel-validation.ps1`)
- ~~GZIP-Dekompression für Kernelabbilder~~ (§115: `gzip_decompress` in `kernel_loader.c`, DEFLATE Stored + Fixed-Huffman, CRC32-Footer-Verifikation; `-CompressGzip` in `build-nki.ps1`)
- ~~DEFLATE Dynamic-Huffman (BTYPE=10) für GZIP-Decompressor~~ (§116: `dht_t`/`dht_build`/`dht_decode`/`dht_fixed`/`dht_read_dynamic`/`dht_inflate_block` in `kernel_loader.c`; `-CompressGzipReal`/`Compress-GzipReal` in `build-nki.ps1`; `nki-v2-gzip-dyn-valid` Testfall in `test-uefi-kernel-validation.ps1`)
- ~~HMAC-SHA-256 für NKI v2 DevSign (Phase-2 Dev-Key)~~ (§117: SHA-256 FIPS 180-4 + HMAC RFC 2104 in `kernel_loader.c` (`sha256_t`, `sha256_init/block/update/final`, `g_hmac_dev_key`, `hmac_sha256_nki`); DevSign-Block um `NOVA_NKI_SCHEME_HMACSHA256=2` erweitert; `-SignHmacSha256` in `build-nki.ps1`; `nki-v2-hmacsha256-valid` Testfall in `test-uefi-kernel-validation.ps1`)
- ~~vollständige NovaOS-Trustentscheidung (Policy-Authorized)~~ (§118: `NOVA_BOOT_VERIFICATION_POLICY_AUTHORIZED` (Stufe 4) wenn HMAC-SHA-256 verifiziert + Secure-Boot-Zustand bekannt; `sig_hmac`-Rückgabe in `load_nki_elf32`; `UEFI:KERNEL-POLICY-AUTHORIZED` Debug-Marker; BIB Security TLV trägt `verification_state=4`; Testfall `nki-v2-hmacsha256-valid` erwartet `UEFI:KERNEL-POLICY-AUTHORIZED`)
- kryptografischer Kernelsignaturcontainer, Schlüssel-/Revocation-Policy und
  vollständige NovaOS-Trustentscheidung abgeschlossen; der UEFI-Secure-Boot- und
  Integritätszustand wird getrennt in den BIB übertragen
- autorisierte Candidate-Staging-Schnittstelle und die capabilitygeschützte
  Kernel-/Userspace-Transportbrücke für Health Evidence; eindeutige logische
  Generationen, Health-Aggregation, Candidate-Commit, redundante Speicherung,
  Versuchslimit und Rollback sind vorhanden, alle drei Container verwenden
  derzeit aber noch dasselbe Entwicklungskernelpayload
- echte Prozess-/Stromunterbrechung an jedem einzelnen UEFI-Schreibzeitpunkt;
  die CRC-beschädigte neueste Kopie und der Rückfall auf die ältere Kopie sind
  bereits in QEMU geprüft
- ~~vollständige AP-Aktivierung und echter SMP-Betrieb~~ (§124: INIT-SIPI-SIPI, `ap_trampoline_blob`, `ap_entry32_pm`, `smp_boot_ap`, `smp_start_aps`; `smp_send_ipi` und `smp_tlb_shootdown_page` senden echte IPIs; `smp_self_test` prüft UP- und SMP-Pfad)
- ~~vollständige Semantic Relationships, Subtypes und Traits~~ (§121: `semantic_register_subtype`, `semantic_register_trait`; Subtype/Trait-Scans in `semantic_compatibility`; dritter Typ `nova.kernel.trait.readable`; DIAGNOSTIC < INLINE_DATA, INLINE_DATA implements readable; `semantic_self_test` aktualisiert)
- ~~mehrere kompatible Semantic Types pro Ressource~~ (§119: `object_semantic_attach_secondary`, `object_semantic_has_type`, `object_semantic_query_secondary`, `object_semantic_secondary_count`, `object_semantic_clear_secondary`; bis zu 4 Secondary Types pro Ressource; Secondary-Sidecar-Arrays in `semantic32.inc`; `semantic_initialize` löscht Secondary-Felder; `object_semantic_attach` setzt Secondary-Count zurück; erweiterter `semantic_self_test`)
- ~~Typed Files und persistente Semantic Metadata~~ (§123: `novafs_typed_file_create`, `novafs_typed_file_read`, `novafs_typed_file_write`; Semantic-Type-Handle in NovaFS-Inode; `semantic_self_test` erweitert)
- ~~Semantic Discovery und Semantic Execution~~ (§122: `semantic_find_by_name`, `semantic_query_type_info`, `semantic_enumerate_type/conversion/subtype/trait`; Discovery-Scratch `semantic_disc_*`; `semantic_self_test` mit 13 neuen §122-Fällen)
- ~~tatsächliche Ausführung registrierter Conversion Capabilities~~ (§120: `semantic_register_conversion_handler`, `semantic_execute_conversion`, eingebauter Handler `semantic_handler_inline_to_diag`; Handler-Registry bis 8 Einträge; Ausführungs-Scratch `semantic_exec_*`; `semantic_self_test` erweitert mit Positiv- und Negativfall)
- ~~NovaFS: echtes Copy-on-Write, Checkpoints/Snapshots,
  Zusammenlegen leerer Blätter und Baumhöhe > 2~~ (§125: CoW in
  `novafs_write`, `novafs_extent_cow_update`, `novafs_cow_deferred_free`;
  Snapshots per `novafs_snapshot_create/delete/list`; B+Baum-Höhe > 2 mit
  Pfadstapel `nfs_path_depth/blocks/indices`, `novafs_level_page`,
  `novafs_cursor_seek` generalisiert, innere Split-Propagation;
  Blatt-Verschmelzung per `novafs_try_leaf_merge`; Transaction Log/Crash
  Recovery, Löschen, Umbenennen, Zeitstempel und Nutzdatenprüfsummen waren
  bereits umgesetzt, siehe Abschnitt 16, `dev_detail.md` Abschnitte 101–107)
- ~~persistenter Userspace und Trust-Provider~~ (§126: `boot_health_publish_system_root`
  meldet SYSTEM_ROOT READY/DEGRADED nach `novafs_initialize`; `boot_health_advance`
  wertet DEGRADED-Provider als erfüllt — Meilensteine werden auch via Degraded-Pfad
  durchlaufen; `boot_health_provider_degraded`-Tabelle parallel zu `provider_ready`;
  `.provider_degraded`-Pfad in `submit_report` ruft jetzt `boot_health_advance` auf;
  serielle Meldungen für HealthConfirmed / DegradedConfirmed / not-confirmed;
  Boot Health erreicht `CONFIRMED` auch mit unsigniertem Kernel, Status dann DEGRADED)
- Tests auf realer UEFI-Hardware
- erneute End-to-End-Prüfung des aktuellen Images in VirtualBox
- pixelgenauer visueller Vergleich aller Bootmanagerseiten mit sämtlichen
  Referenzbildern und Zielauflösungen

## 11. Empfohlene nächste Schritte

Für die weitere Arbeit am derzeit priorisierten UEFI-Pfad bietet sich diese
Reihenfolge an:

1. ~~Dateien in der System-UI nutzen (Explorer zeigt echte Inhalte aus
   `/Benutzer` über `VFS.ReadDirectory`), danach `Delete` und `Rename`~~ —
   umgesetzt (Abschnitte 102–105),
2. ~~NovaFS Phase 2: Transaction Log und Crash Recovery, damit ein
   abgebrochener Schreibvorgang automatisch repariert statt nur read-only
   gemountet wird; anschließend Zeitstempel~~ — umgesetzt (`dev_detail.md`
   Abschnitte 101 und 106); ~~Nutzdatenprüfsummen~~ — umgesetzt (Abschnitt
   107); ~~echtes Copy-on-Write, Snapshots und Baumhöhe > 2~~ — umgesetzt
   (§125, `novafs32.inc`),
3. ~~normativen Kernel-Signaturcontainer sowie Schlüssel- und Revocation-Policy
   spezifizieren beziehungsweise implementieren (Trust-Provider für Boot
   Health)~~ — umgesetzt (Abschnitt 108, NKI v2 mit DevSign-Block, Trust- und
   Session-Provider; Boot Health erreicht jetzt HealthConfirmed),
4. ~~autorisierte Candidate-Staging-Schnittstelle~~ — umgesetzt (Abschnitt 109,
   `firmware_runtime_health_commit` schreibt HEALTHY-Wire nach HealthConfirmed;
   198948 Bytes Kernel); ~~`test-uefi-kernel-validation.ps1`: NKI-v2-Testfall
   ergänzen~~ — umgesetzt (Abschnitt 110: Positivfall DevSign-Verifizierung,
   ungültige sig_size, abgeschnittener DevSign-Block); ~~Revocation-Policy:
   `revocation_gen > 0` ablehnen~~ — umgesetzt (Abschnitt 111:
   `kernel_loader.c` hart abgewiesen + `bad-nki-v2-devsign-revoked` Testfall);
   ~~`test-uefi-boot-control.ps1` für §109 aktualisieren~~ — umgesetzt
   (Abschnitt 112: bridge-consume erwartet COMMITTED, bridge-candidate prüft
   HEALTHY-Wire-Log); danach VirtualBox-UEFI und reale Hardware mit dem
   GPT-Image (ESP + NovaFS) erneut validieren.

Die Health-Evidence-Brücke aus dem früheren Schritt 2 ist inzwischen umgesetzt
(siehe „UEFI-Runtime-Transport für Boot Health“).

## 12. Wichtige Quellbereiche

| Bereich | Pfad |
|---|---|
| Spezifikationen | `docs/` |
| BIOS-Bootloader | `boot/bootloader/` |
| UEFI-Anwendung | `boot/bootloader/uefi/` |
| Bootmanager | `boot/bootloader/bootmenu/` |
| Gemeinsames Bootprotokoll | `boot/include/` |
| Kernel Entry und Subsysteme | `kernel/arch/x86_64/entry32.asm` |
| Semantic-Type-Kern | `kernel/arch/x86_64/semantic32.inc` |
| Storage (PCI, AHCI, GPT) | `kernel/arch/x86_64/storage32.inc` |
| NovaFS im Kernel | `kernel/arch/x86_64/novafs32.inc` |
| NovaFS-Format (normativ) | `docs/NPSPEC/NPSPEC-NOVAFS-ONDISK-0001.md` |
| NovaFS-Host-Werkzeug | `tools/novafs/novafs.c` |
| Build- und Testskripte | `scripts/` und `tests/` |
| Erzeugte Artefakte | `build/` und `kernel/build/` |

## 13. Pflege dieser Datei

Diese Datei soll nach größeren abgeschlossenen Arbeitsschritten aktualisiert
werden. Neue Einträge müssen klar unterscheiden zwischen:

- **spezifiziert:** in ADR/NPSPEC beschrieben,
- **implementiert:** im Quellcode vorhanden,
- **automatisiert getestet:** durch einen reproduzierbaren Test bestätigt,
- **manuell geprüft:** visuell oder in einer VM kontrolliert,
- **offen:** noch nicht oder nur teilweise umgesetzt.

## 14. Native UI-Systemarchitektur

Der vollständige Dokumentbestand und insbesondere die 26 angenommenen
Spezifikationen in `docs/NPSPEC/sysarchitecture/001-UI` wurden am 23. September
2026 neu eingelesen. Unter `ui/` existiert nun ein eigenständiger, in C17
kompilierbarer UI-Architekturkern. Er implementiert die gemeinsamen Verträge
für Retained Mode, deklaratives Reconciliation, Scene Graph, einen logisch
getrennten Accessibility Tree, Semantic UI, Theme-/Color-Tokens, Damage
Tracking, Frame Scheduling, VRR, Display-, Surface-, Window- und Input-Routing
sowie capabilitybasierte Startmenü-, Ribbon-, Dashboard- und
Taskleisten-Contributions.

Alle Layoutgrößen werden als DLU geführt. Identitäten verschiedener Domänen
sind als unterschiedliche C-Typen modelliert, damit beispielsweise eine
`WindowID` nicht versehentlich als `SurfaceID` verwendet wird. Feste Kapazitäten
und begrenzte Frame Queues vermeiden unbeschränkten Speicherverbrauch.

`make ui-architecture-runtime-check` baut die Runtime mit
`-Wall -Wextra -Werror` und prüft die zentralen positiven und negativen Pfade.
Der Test umfasst unter anderem Discovery ohne Autorisierung, capabilitygeprüfte
Accessibility-Actions, Session-Isolation, sicheren Capture-Abbau bei
Owner-Ausfall, fehlerhafte Surface-Damage-Daten, Direct Scanout,
GPU-/Software-Fallback, Provider-Ausfall, Display-Hot-Unplug und VRR.

Noch nicht als Hardwareintegration vorhanden sind reale GPU- und Displaytreiber,
prozessübergreifende Shared Buffer, persistente Desktop-/Startmenüdaten,
Suchindex, Privacy-Dienst und ausführender Capability Broker. Die Runtime stellt
hierfür die kontrollierten Providergrenzen bereit; bis zur Treiberanbindung
bleibt der Softwarepfad der definierte funktionale Fallback.

### Kernel-/Userspace-Displaypfad

Der initiale Ring-3-Systemdienst verwendet nun `NOVA_SERVICE_DISPLAY` ABI 1.0.
`Display.QueryPrimary` liefert ausschließlich Display-ID, Größe, Pitch, BPP,
DLU-Skalierung und Generation; die physische GOP-Framebufferadresse bleibt im
Kernel. `Display.SubmitSystemScene` übernimmt eine pointerfreie, 64 Byte große
deklarative Szene. Größe, ABI, monotone Generation, Flags, Theme-/Color-Tokens,
Workspace und reservierte Felder werden vor jeder Darstellung validiert.

Die erste Eingabeanbindung ist ebenfalls aktiv. Der Kernel normalisiert
PS/2-Scan-Codes aus Set 1 und 2 in begrenzte semantische Ereignisse und liefert
sie über `Display.PollInput` an die capabilitygeschützte Ring-3-System-UI. Die
Nova-/Windows-Taste schaltet das Startmenü um, `Tab` bewegt den semantischen
Fokus und `Enter` erzeugt eine Aktivierungsaktion. `Esc` schließt zuerst ein
geöffnetes Startmenü; ist es bereits geschlossen, startet der geordnete
Power-Manager-Shutdown.

Der Kernel-Display-Provider rendert daraus die erste sichtbare System-UI mit
Desktop, geöffnetem Startmenü, Ribbon und schwebender Taskleiste. Der
System-UI-Prozess benötigt dafür `SECURITY_CAP_DISPLAY_SYSTEM_UI`; normale
Anwendungen erhalten weder MMIO noch diese geschützte Rolle. Das aktuelle
`build/nova-uefi.img` enthält diesen Stand. Der automatisierte QEMU-Test
`make test-uefi-display-server` bestätigt Kernel-Handoff, capabilitygeprüfte
Displayabfrage, Scene-Presentation und `NOVA_KERNEL_READY`. Zusätzlich sendet
er über QMP `Tab` und zweimal `Esc`: `Tab` muss eine neue visuelle Fokusszene
erzeugen, das erste `Esc` das Startmenü schließen und das zweite bis
`PLATFORM_OFF` gelangen.

### UEFI-Bootsplash statt Kernelkonsole

Der normale UEFI-Start zeigt nach dem Bootmanager nicht mehr den grafischen
Kernel-Logbildschirm. `boot/image/bootsplash.png` wird beim Build deterministisch
in eine CRC32-geschützte RGB888-Ressource mit 1280×720 Pixeln umgewandelt und
als `SPLASH.NBS` in die EFI-Systempartition gelegt. Der Kernellader validiert
und skaliert die Ressource unmittelbar vor `ExitBootServices` mit bilinearer
Filterung und unter Beibehaltung des
Seitenverhältnisses. Der Kernel bewahrt diesen Framebuffer bis zur ersten
Ring-3-Desktop-Szene. Die bestehende Kernelkonsole bleibt im Code für Fehler-
und Diagnosepfade erhalten, wird beim erfolgreichen Start aber nicht mehr
gezeichnet. Der UEFI-QEMU-Test verlangt `UEFI:BOOTSPLASH-READY`.

### NovaOS Desktop-Redesign

Die fünf neuen Designreferenzen wurden als ein gemeinsames NovaOS-System
ausgewertet. Die ersten vier bestimmen Desktop, Fenster, Taskleiste und Apps;
die Startmenü-Referenz gilt ausschließlich für das Startmenü. Unter `ui/` sind
nun zentrale Aurora-/Acrylic-Tokens sowie eine heapfreie Desktop-Shell-Schicht
vorhanden. Sie verwaltet responsive DLU-Geometrie, Taskleisten-Pins,
Startmenü-Transition, Outside-Click, Escape, `Ctrl+K` und Fensterzustände für
Explorer, Nova Sheet und Fähigkeiten Studio. `make ui-shell-runtime-check`
prüft diese Verträge automatisiert.

Die Shell ist inzwischen an die produktive Ring-3-Szene angebunden. Nach dem
Bootsplash erscheinen Branding, globale Befehlsleiste, Systemstatus, ein
Explorer-Fenster, die abgesetzte Aurora-Taskleiste und das dreispaltige
Startmenü. Über die Tastatur lassen sich Explorer, Nova Sheet und Fähigkeiten
Studio als unterschiedliche Arbeitsbereiche aktivieren. Der QEMU-Displaytest
bestätigt Scene-Presentation, Fokusweitergabe, Schließen des Startmenüs und
geordneten Shutdown.

Pfeiltasten navigieren inzwischen innerhalb der aktiven Oberfläche. Explorer,
Nova Sheet und Fähigkeiten Studio zeigen den jeweiligen Datei-, Zell- oder
Node-Fokus sichtbar an. Der Display-Provider zeichnet bei solchen
Fokusänderungen nur das betroffene Fenster beziehungsweise Startmenü neu. Der
automatisierte QEMU-Test prüft diese Navigation mit einer realen virtuellen
Pfeiltaste.

## 15. Task-Scopes und Structured Concurrency

Der Kernel implementiert jetzt den ersten angenommenen
Structured-Concurrency-Unterbau. Der initiale Kernelprozess besitzt einen
dauerhaften Root-Scope, dem die vorhandenen Kernelthreads über ihr versioniertes
ABI-Record zugeordnet sind. Weitere Scopes besitzen eine stabile ID, einen
expliziten Prozess-Owner und optional einen Parent-Scope.

Cancellation wird an alle Nachfahren weitergegeben und speichert einen
eindeutigen Grund. Ein Parent-Scope kann nicht geschlossen werden, solange noch
aktive Kinder existieren. Die feste Tabelle ist bewusst begrenzt und verhindert
unbeschränkte Allokation im frühen Kernel.

ABI-Layout, positiver Lifecycle und negative Abschlussfälle werden automatisch
geprüft. Der QEMU-UEFI-End-to-End-Test verlangt den erfolgreichen
Task-Scope-Selbsttest und erreicht anschließend weiterhin Ring 3, die
Desktop-Szene und den geordneten Shutdown bis `PLATFORM_OFF`.

Darauf aufbauend verwaltet der Kernel jetzt einzelne Tasks mit stabiler ID,
Prozess-Owner, owning Scope, optionalem Parent, Resultat und Cancellation-Grund.
Cancellation wird hierarchisch angefordert, aber nicht als harter Thread-Abbruch
ausgeführt. Erst ein expliziter Cancellation Point schließt Cleanup und den
terminalen `Cancelled`-Zustand ab. Solange ein Task nicht terminal ist, bleibt
sein Scope aktiv und kann nicht erfolgreich geschlossen werden.

Die vorhandenen drei Kernelthreads sind inzwischen echte verwaltete Tasks im
Kernel-Root-Scope. Der Timer-Scheduler spiegelt seine Round-Robin-Auswahl in den
Taskzuständen `Ready` und `Running`, ohne Cancellation- oder Terminalzustände zu
überschreiben. Die Zuordnung bleibt über feste Task-IDs introspektierbar.

Task-Deadlines verwenden jetzt die monotone 100-Hz-Kernelzeit und unterscheiden
Hard, Firm und Soft. Child-Tasks können eine Parent-Deadline verschärfen, aber
nicht verlängern. Deadline Miss und Cancellation bleiben getrennte Zustände;
eine Cancel-Policy verwendet anschließend den normalen kooperativen
Cancellation-Pfad. Der IRQ0-Poll ist durch die feste Taskkapazität begrenzt.

Mehrere Tasks lassen sich nun als Task Group mit `WaitAll` oder `FailFast`
verwalten. `WaitAll` wartet auf alle erforderlichen Ergebnisse. `FailFast`
propagiert den ersten Fehler, fordert Cancellation für verbleibende Tasks an
und erreicht den terminalen Fehlerzustand erst nach vollständigem Drain. Damit
bleiben Gruppen-Lifecycle, Fehler und Scope-Abschluss konsistent.

Auf dieser Grundlage ist nun auch der erste gemeinsame Async-I/O-Unterbau
vorhanden. I/O-Requests sind eindeutig einer Task und ihrem Scope zugeordnet,
übernehmen deren Deadline und liefern genau eine Completion mit Erfolg,
Teilerfolg, Fehler, Cancellation oder Deadline Miss. Die feste Queue mit acht
offenen Requests erzeugt bei Überlastung definierte Backpressure. Task-Abbruch
und Deadline-Miss schließen abhängige Requests kontrolliert, ohne einen Thread
pro I/O anzulegen. Ein echter Geräteprovider ist in diesem Schritt noch nicht
angebunden; das Modell stellt dafür die geprüfte ABI- und Lifecycle-Basis bereit.

Der I/O-Scheduler verarbeitet jetzt die Klassen Realtime, Interactive, Normal,
Background und Maintenance. Angeforderte und wirksame Priorität bleiben
getrennt, wobei unprivilegierte Realtime-Anforderungen kontrolliert begrenzt
werden. Deadline-Requests können niedrigere Priorität überstimmen; ansonsten
gelten wirksame Priorität und FIFO-Alter. Eine begrenzte Aging-Regel hebt lange
wartende Requests schrittweise an und verhindert dadurch Starvation. Requeue
erhält Identität und Wartehistorie, wenn ein späterer Provider vorübergehend
keine Kapazität besitzt.

Ein separates I/O-QoS-Modell ergänzt diesen Scheduler. Profile beschreiben
Latenzziel, Durchsatzwunsch, Bandbreitengrenze, Klasse und ein Budget für offene
Requests. Harte Anforderungen durchlaufen Admission Control: Ohne geeigneten
Provider werden Durchsatz- oder Bandbreitengarantien abgewiesen, statt nur auf
dem Papier zugesagt zu werden. Weiche, derzeit nicht vollständig erfüllbare
Wünsche bleiben nutzbar, sind jedoch explizit als degradiert markiert.
Completion-Accounting erfasst beobachtete Latenz, Bytes und Zielverletzungen;
Budgetüberschreitungen werden kontrolliert gedrosselt und gezählt.

Terminale I/O-Ergebnisse werden jetzt außerdem über eine eigene Completion
Queue zugestellt. Der begrenzte 16er-FIFO enthält Request-ID, Ergebnisstatus,
Bytes, Fehler, Zeitstempel und Latenz und kann mehrere Einträge als Batch
ausgeben. Submission- und Completion-Reihenfolge sind dadurch nicht gekoppelt.
Ein voller Ring wächst nicht unkontrolliert; Overflow wird gezählt, während das
autoritative Ergebnis weiterhin im Request-Datensatz erhalten bleibt.

Der Kernel besitzt jetzt außerdem Shared Buffers mit stabiler ID, explizitem
Owner und Read-/Write-/Transfer-/DMA-/Release-Rechten. Ein begrenzter interner
4×4-KiB-Pool hält den frühen Kernelpfad deterministisch; Backing-Adressen werden
nicht Teil des öffentlichen ABI. Eine I/O-Lease übergibt das Ownership bis zur
Completion exklusiv an den Provider und verhindert vorzeitige Freigabe. Danach
kehrt der Buffer automatisch in den CPU-eigenen Zustand zurück. Für nicht
Zero-Copy-fähige Pfade ist ein validierter und gezählter Copy-Fallback vorhanden.
Echtes DMA-Mapping wird erst mit der dafür geforderten HAL-/IOMMU-Prüfung
freigeschaltet.

### Kontrolliertes DMA-Mapping und Pinning

Auf dem Shared-Buffer-Lifecycle baut jetzt die erste hardwareunabhängige
DMA-Mapping-Schicht auf. Ein Mapping besitzt eine stabile ID und bindet genau
einen I/O-Request, einen Shared Buffer, ein Gerät, eine explizite Richtung,
minimale Read-/Write-Rechte, Länge, Device-Adresse, Domain, Generation und eine
begrenzte Lebensdauer zusammen. Virtuelle CPU-Adressen werden dabei niemals
automatisch als Device-Adressen behandelt.

Vor dem Mapping prüft der Kernel Request-Owner, Device-ID, Buffer-ID, aktive
Provider-Lease, Länge sowie `TRANSFER`- und `DMA`-Rechte. `ToDevice` benötigt
Leserecht, `FromDevice` Schreibrecht und `Bidirectional` beide Rechte. Pro
Request ist höchstens ein aktives Mapping zulässig. Ein aktives Mapping erhöht
die sichtbaren Mapping- und Pin-Zähler des Buffers und verhindert dadurch eine
vorzeitige Freigabe.

Completion, Fehler, Cancellation und Deadline Miss führen über denselben
terminalen Request-Pfad zuerst zum Unmapping und Unpinning und erst danach zur
Rückgabe der Buffer-Lease an die CPU. Zusätzlich kann ein IOMMU-/DMA-Fehler dem
verursachenden Gerät und Mapping zugeordnet werden; sein Fehlerzustand bleibt
im Datensatz sichtbar, obwohl die Ressourcen kontrolliert freigegeben werden.

Da der aktuelle Bootstrap noch keinen erkannten IOMMU- oder echten
Geräteprovider besitzt, meldet die Schicht diesen Zustand ausdrücklich und
verwendet eine begrenzte Restricted-/Bounce-Apertur. Sie behauptet weder echte
Hardware-DMA-Übertragung noch IOMMU-Isolation. Das ABI in
`kernel/include/nova/dma_mapping.h` schafft die geprüfte Grenze, an die ein
späterer Plattformprovider angebunden wird.

Der Start-Selbsttest prüft bidirektionale Rechte, die getrennte Device-Adresse,
Mapping- und Pin-Accounting, die Ablehnung eines zweiten Mappings, blockierte
Buffer-Freigabe sowie automatisches Unmapping bei I/O-Completion. ABI-Check,
Kernel-Build und der vollständige UEFI-QEMU-Displaytest einschließlich Ring 3,
Tastaturnavigation und geordnetem `PLATFORM_OFF` sind erfolgreich.

### Scatter/Gather-Descriptoren

Der Kernel kann nun mehrere unabhängige Shared-Buffer-Bereiche als einen
geordneten logischen Datenstrom beschreiben. Jeder Segmentdatensatz verwendet
ausschließlich `BufferId`, Offset, Länge, Rechte, logischen Offset, Flags und
Generation. Physische Adressen oder ungeprüfte Kernelpointer sind kein Teil des
allgemeinen Scatter/Gather-ABIs.

Descriptoren entstehen zunächst im Zustand `Building`. Jeder neue Bereich wird
auf Owner, Zustand, Rechte und überlaufsicheres `Offset + Length` geprüft.
Benachbarte, kompatible Bereiche desselben Buffers werden ohne Änderung der
logischen Reihenfolge zusammengeführt. `Seal` validiert alle Segmente erneut
und hält je Segment eine Buffer-Referenz, sodass keiner der beteiligten Buffer
vorzeitig freigegeben werden kann.

Die Bootstrap-Grenzen sind vier Descriptoren mit jeweils höchstens vier
Segmenten. Der aktuelle Schritt stellt die architekturunabhängige Darstellung,
Lifetime und Introspection bereit. Die Übersetzung der Segmente in mehrere
Device-Adressen folgt erst mit der SG-fähigen DMA-Provider-Anbindung; bis dahin
bleibt ein Copy-/Bounce-Fallback explizit erlaubt.

Der Selbsttest prüft zwei verschiedene Buffer, logische Reihenfolge,
Coalescing, Bereichsüberschreitung, Retain/Release und blockierte vorzeitige
Freigabe. Der vollständige UEFI-QEMU-Test bestätigt anschließend weiterhin
Ring 3, Desktopdarstellung, Eingabe und `PLATFORM_OFF`.

### Scatter/Gather-DMA-Providergrenze

Versiegelte Scatter/Gather-Descriptoren können jetzt an einen kontrollierten
DMA-SG-Provider gebunden werden. Dieser prüft I/O-Request, Prozess-Owner,
Device-ID, Descriptor-ID, Gesamtlänge, Buffer-Lifetime sowie `TRANSFER`-,
`DMA`- und richtungsabhängige Zugriffsrechte. Ein Request kann nicht
gleichzeitig ein lineares und ein Scatter/Gather-DMA-Mapping besitzen.

Der Bootstrap-Provider beschreibt seine Hardwaregrenzen explizit: maximal vier
Device-Segmente, 64 Byte je Segment, 4-Byte-Alignment, 32-Bit-Adressbreite und
eine 4-KiB-Boundary. Größere Quellsegmente werden unter Erhalt ihrer logischen
Reihenfolge aufgeteilt. Übersteigt die Übersetzung die Segmentkapazität oder
enthält sie eine für diesen Provider nicht unterstützte wiederholte
Buffer-Referenz, wird sie kontrolliert abgewiesen und kann in einen späteren
Copy-Fallback wechseln.

Während des Mappings wechseln alle beteiligten Buffer in den
Provider-Ownership-Zustand und werden gezählt gepinnt. Completion,
Cancellation, Fehler und Deadline Miss entfernen sämtliche Device-Segmente,
heben Pinning und Mapping auf und geben erst dann die Buffer an die CPU zurück.
Der versiegelte SG-Descriptor bleibt bis zu seinem eigenen Release erhalten.

Ohne IOMMU werden weiterhin ausschließlich Device-Adressen aus einer
reservierten Restricted-/Bounce-Apertur ausgegeben. Der Selbsttest prüft reales
Segment-Splitting, Alignment, Boundary, Consumer-Lifetime und automatischen
Abbau. Der UEFI-QEMU-End-to-End-Test erreicht danach weiterhin Ring 3 und
`PLATFORM_OFF`.

### IOMMU-Domains, Gruppen und Fault-Zuordnung

Der Kernel besitzt jetzt eine eigenständige IOMMU-Abstraktion mit stabilen
Domain-, Device- und Mapping-Identitäten. Eine Domain führt Owner, Zustand,
Isolationsmodus, Geräte- und Gruppenzahl, aktive Autorisierungen, gemappte
Bytes, Fault-Zähler, Adressbreite und ihren erlaubten IOVA-Bereich. Geräte
werden immer zusammen mit ihrer hardwarebedingten IOMMU-Gruppe gebunden.

Geräte derselben Gruppe dürfen nicht auf verschiedene Domains verteilt werden.
Eine Mapping-Autorisierung prüft Domain-Owner, Device-Binding, IOVA-Ausrichtung,
Bereich, Länge, minimale Read-/Write-Rechte und Überlappungen mit bestehenden
Mappings. Aktive Autorisierungen blockieren den Domain-Abbau; `Revoke` entfernt
sie kontrolliert und gleicht Mapping- sowie Byte-Accounting aus.

IOMMU-Faults werden mit Sequenz, Domain, Device, externer Mapping-ID, IOVA,
Zugriffsart und Fehlercode erfasst. Ein betroffenes Mapping wechselt sichtbar
in `Faulted`, bleibt aber bis zum expliziten Revoke zurechenbar. Beim späteren
Domain-Release werden alle Geräte der zugehörigen Gruppen gemeinsam gelöst.

Der aktuelle QEMU-Start erkennt noch keinen programmierten virtuellen oder
physischen IOMMU-Provider. Deshalb akzeptiert der Bootstrap nur
`Restricted`-Domains; Anforderungen nach Hardware- oder virtueller Isolation
werden abgewiesen, statt eine nicht vorhandene Garantie zu melden. Der
Selbsttest prüft zwei Geräte in einer Gruppe, eine gültige Autorisierung,
überlappende IOVAs, Fault-Zuordnung und vollständigen Ressourcenabbau. Der
UEFI-End-to-End-Test bleibt erfolgreich.

### Automatischer DMA-/IOMMU-Lifecycle

Die linearen und Scatter/Gather-DMA-Pfade sind jetzt direkt mit der
IOMMU-Domainverwaltung verbunden. Ist ein Device an eine Domain gebunden,
erzeugt das DMA-Mapping automatisch die notwendigen IOVA-Autorisierungen. Die
Domain-ID wird im linearen und im SG-DMA-Mapping sichtbar gespeichert.

Ein lineares Mapping erhält genau eine Autorisierung für seine kontrollierte
Device-Seite. Beim Scatter/Gather-Pfad wird jedes erzeugte Device-Segment
einzeln autorisiert. Schlägt eine Autorisierung in der Mitte fehl, widerruft
der Kernel alle zuvor angelegten Einträge, bevor Buffer oder Descriptoren in
den Providerzustand wechseln. Dadurch existieren keine halbfertigen Mappings.

Der gemeinsame terminale I/O-Pfad widerruft sämtliche IOMMU-Autorisierungen,
bevor Pinning, Provider-Ownership und Buffer-Referenzen zurückgegeben werden.
Das gilt identisch für Erfolg, Teilerfolg, Fehler, Cancellation und Deadline
Miss. Erst wenn das IOMMU-Accounting wieder null meldet, dürfen Test-Domain und
Device-Bindings freigegeben werden.

Die erweiterten Selbsttests prüfen eine lineare Autorisierung sowie drei
separate Autorisierungen für ein gesplittetes SG-Mapping. Nach Completion
müssen jeweils keine aktiven IOMMU-Mappings mehr vorhanden sein. Der reale
UEFI-QEMU-Test bestätigt den gemeinsamen Lifecycle-Marker und erreicht danach
weiterhin Ring 3, Desktop und `PLATFORM_OFF`.

### IOMMU-Fault-Propagation bis zur I/O-Completion

IOMMU-Faults bleiben nicht länger reine Diagnosedatensätze. Externe
Mapping-Identitäten enthalten jetzt einen getrennten Typanteil für lineares DMA
und Scatter/Gather-DMA. Dadurch kann der Fault-Handler das betroffene
Kernelmapping eindeutig finden, ohne IDs verschiedener Domänen zu vermischen.

Ein Fault markiert zunächst Autorisierung und DMA-Mapping als `Faulted`. Danach
werden alle IOVA-Autorisierungen widerrufen, die Device-Mappings entfernt,
Buffer entpinnt und das Ownership an die CPU zurückgegeben. Erst nach diesem
sicheren Abbau wird der zugehörige I/O-Request mit `Failed`, einer
`IO_COMPLETION_FAILED`-Completion und dem ursprünglichen Gerätefehler beendet.

Der SG-Pfad behandelt einen Fault auf einem einzelnen Device-Segment als Fehler
der gesamten Operation. Alle übrigen Segmente werden ebenfalls revoked, bevor
der Request abgeschlossen wird. Dadurch bleibt kein Teilmapping aktiv und ein
unbekannter Transferzustand kann nicht als Erfolg erscheinen.

Die linearen und SG-Selbsttests lösen nun absichtlich IOMMU-Faults aus. Sie
prüfen Mappingzustand, Requestzustand, Completionstatus, Fehlercode,
IOMMU-Accounting, Pinning und Ownership. Der UEFI-End-to-End-Test bestätigt die
neue Fault-Markierung und erreicht anschließend weiterhin Desktop und
`PLATFORM_OFF`.

### Normalisierte HAL-Hardwaretopologie

Der Kernel besitzt jetzt einen versionierten Topologiegraphen für System,
UMA-/NUMA-Knoten, CPU-Package, Core, Hardware-Thread, Speicher, Interrupt-
Controller, Busse, IOMMU-Gruppen und Devices. Parent-/Child-Beziehungen,
eindeutige Topologie-IDs, Generationen und monotone Änderungssequenzen machen
die Struktur kontrolliert auswertbar.

Der aktuelle Graph ist ein ausdrücklich markiertes Bootstrap-/UMA-Modell. Er
stellt die vorhandenen DMA-/IOMMU-Testprovider dar, behauptet aber noch keine
reale PCI-Erkennung. Kontrollierte Zustandswechsel decken bereits
`Online → Quiescing → Offline → Online` ab; nicht online befindliche Geräte
werden nicht als nutzbar geliefert.

Die IOMMU-Bindung vertraut einer vom Aufrufer übergebenen Gruppennummer nicht
mehr allein. Device und Gruppe müssen im normalisierten HAL-Modell
übereinstimmen. ABI-Prüfung, Kernelbau und der vollständige UEFI-QEMU-Test bis
Desktop und geordnetem Shutdown sind erfolgreich.

### CPU-Erkennung aus ACPI/MADT normalisiert

Der HAL-Graph verwendet jetzt die bereits validierte ACPI-MADT-Liste statt
eines fest eingetragenen CPU-Threads. Aktivierte Local-APIC- und x2APIC-
Einträge werden dedupliziert und mit ihrer echten APIC-Hardware-ID als
`CPU_THREAD` übernommen. Der automatisierte UEFI-Test startet QEMU mit vier
CPUs und verlangt entsprechend vier normalisierte Threads.

Package- und Core-Beziehungen stammen nicht aus der MADT. Sie werden jetzt
getrennt über die standardisierten x86-CPUID-Topologieblätter normalisiert.
Die NUMA-Lokalität benötigt weiterhin ACPI SRAT. Ohne gültige MADT bleibt ein
klar gekennzeichneter BSP-Fallback mit der über CPUID ermittelten APIC-ID
erhalten.

### Package-, Core- und SMT-Hierarchie

Der HAL-Provider wertet bevorzugt CPUID `0x1F` und ersatzweise `0x0B` aus.
Nach validierten SMT- und Core-Bitgrenzen werden die MADT-APIC-IDs stabil zu
Package, Core und Hardware-Thread gruppiert. SMT-Threads desselben Kerns teilen
sich dadurch tatsächlich denselben Core-Knoten.

Fehlen vollständige CPUID-Level, wird keine genaue CPU-Hierarchie behauptet.
Der Selbsttest prüft jede Parent-Kette bis zum NUMA-Fallback. ABI-Prüfung,
Kernelbau und der vollständige UEFI-QEMU-Test bis Desktop und Shutdown bleiben
erfolgreich.

### CPU Manager an den HAL-Graphen angebunden

Der CPU Manager besitzt nun eine versionierte 112-Byte-CPU-ABI mit getrennten
Feldern für Package, Die, Cluster, Core, Hardware-Thread und NUMA. Das behebt
den alten internen Datensatz, in dem ein eigenes Core-Feld fehlte.

BSP und erkannte APs beziehen Package-, Core- und Thread-Identitäten jetzt
direkt aus dem normalisierten HAL-Graphen. Der CPU Manager wertet dafür weder
MADT noch CPUID-Topologie erneut aus. Topologieknoten-ID und Generation bleiben
im CPU-Datensatz erhalten, sodass spätere Änderungen nachvollziehbar sind.

Unbekannte Die-, Cluster-, Cache- und NUMA-Zuordnungen bleiben ausdrücklich
`0xFFFFFFFF`. ABI-Prüfung, Kernel-Selbsttest und der vollständige UEFI-QEMU-
Test bis Desktop und geordnetem Shutdown sind erfolgreich.

### ACPI SRAT und echte NUMA-Affinitäten

NovaOS validiert und parst jetzt ACPI SRAT für Local-APIC-, x2APIC- und
Speicher-Affinitäten. CPU-Einträge werden gegen die MADT abgeglichen;
widersprüchliche Domains, überlappende Speicherbereiche und Überläufe werden
fail-closed abgewiesen.

Validierte Proximity-Domains erscheinen als stabile NUMA-Knoten im
Hardwaregraphen. Hardware-Threads und Speicherregionen erhalten nur bei
bestätigter SRAT-Zuordnung `LOCALITY_KNOWN`. Der CPU Manager übernimmt die
NUMA-ID direkt aus diesem Graphen. Ohne SRAT bleibt die Lokalität ausdrücklich
unbekannt.

Ein neuer UEFI-QEMU-Test startet zwei NUMA-Knoten mit vier CPUs und getrennten
Speicher-Backends. Er bestätigt vier CPU- und drei Memory-Affinitäten sowie den
vollständigen Kernelstart. Der normale UEFI-Desktoptest bleibt ebenfalls
erfolgreich.

### NUMA-bewusste PMM-Allokation

Der physische Bootstrap-Speichermanager taggt nun jeden verwalteten Frame mit
seiner validierten SRAT-Proximity-Domain oder mit einem expliziten Unknown-
Wert. Seine abwärtskompatibel erweiterte ABI 1.1 unterstützt eine bevorzugte
Allokation mit Fallback, eine strikte node-lokale Allokation und die Abfrage
der Lokalität einer physischen Seite.

Der Kernel-Selbsttest prüft lokale Entnahme, fehlende Domains, Fallback und die
vollständige Wiederherstellung des Frame-Zählers. ABI-Check, normaler UEFI-
Desktoppfad und Zwei-Knoten-NUMA-Test sind erfolgreich. Eine systemweite
Placement-/Migrationspolitik und SLIT-Distanzen bleiben spätere Schritte.

### Kernel-seitige Boot-Health-Autorität

Der Kernel besitzt jetzt eine versionierte Boot-Health-ABI 1.0. Reports sind
an die vom NBHP/BIB übernommene Systemgeneration und den aktuellen Bootversuch
gebunden und werden ausschließlich nach einer Capability-Prüfung angenommen.
Getrennte Rechte schützen das Melden von Providerzuständen und den Export einer
persistierbaren Health Evidence.

Die Autorität aggregiert Kernel, Speicher, SystemRoot, Trust, Capability, IPC
und Session zu lückenlosen Milestones. Fehlerhafte, veraltete, fremde oder
unautorisierte Meldungen werden gezählt und verändern den erreichten Zustand
nicht. Ein Kernel-Selbsttest prüft diese Regeln auf einem Snapshot, sodass der
reale Bootzustand unverändert bleibt.

Im echten UEFI-Boot werden `KernelInitialized` sowie die vorhandenen
Capability- und IPC-Provider bestätigt. Das derzeitige Bootstrap-RAMFS und der
nur integritätsgeprüfte NKI-Pfad werden bewusst nicht als persistentes
`SystemRootReady` beziehungsweise als Trust-Nachweis ausgegeben. Deshalb bleibt
der reale Health-Zustand korrekt auf `Pending`, bis diese Dienste existieren.

ABI-Prüfung, UEFI-Desktop einschließlich Eingabe und Shutdown, persistentes
Boot-Control/Rollback sowie der Zwei-Knoten-NUMA-Test sind erfolgreich. Der
anschließend umgesetzte Laufzeittransport ist im folgenden Abschnitt
beschrieben.

### UEFI-Runtime-Transport für Boot Health

Der NBHP/BIB kann nun einen optionalen, versionierten Firmware-Runtime-TLV an
den Kernel übergeben. Er enthält ausschließlich Provider-ID, Capability,
Kontext, Einstiegspunkt und maximale Nutzlastgröße. Der Kernel benutzt diesen
Provider nur bei einem tatsächlich laufenden A/B-Candidate; normale
Known-Good-Boots schreiben keine Firmwarevariable.

Die 64-Byte-Health-Evidence wird mit Slot, 64-Bit-Generation, Bootversuch,
Milestones, Status, Trust-Zustand, Sequenznummer und CRC32 in die
`NovaBootHealth`-Inbox geschrieben. Der IA32-Kernel wechselt dafür kontrolliert
in den von UEFI benötigten x64-Runtime-Kontext und stellt anschließend seine
Seitentabellen, GDT und Interruptlage vollständig wieder her.

Beim folgenden Start validiert der Bootloader Transportgröße, Wire-Version,
CRC, Slot, Generation, Attempt, Sequenz und Zustandsregeln. Erst danach wird
die Evidence crash-konsistent in die redundanten Boot-Control-Variablen
übernommen; die Inbox wird erst nach erfolgreicher Persistierung gelöscht.
Ungültige, fremde, veraltete oder wiederholte Evidence kann den persistenten
Zustand nicht verändern.

Der automatisierte UEFI-Test deckt den kompletten Pfad ab: Candidate-Staging,
Kernel-Checkpoint, persistente Inbox, Übernahme beim Neustart sowie die
bisherigen Redundanz-, CRC- und Recovery-Fälle. Vollständiges
`HealthConfirmed` bleibt absichtlich aus, solange NovaOS noch kein echtes
persistentes SystemRoot und keinen Trust-Provider besitzt; die Transportbrücke
selbst ist vollständig funktionsfähig.

### ABI Discovery, Global State und Transactions

Der Kernel besitzt nun erste öffentliche V1-Layouts für Syscall-Feature-
Discovery, API-Discovery, semantische API-Deskriptoren, Operationsergebnisse,
versionierte Global-State-Records und Transaction-Records. Die Layouts liegen
in `kernel/include/nova/syscall.h`, `kernel/include/nova/state.h` und
`kernel/include/nova/transaction.h` und werden durch den vorhandenen
`make abi-check` statisch geprüft.

Im Kernelstart laufen außerdem ein Global-State- und ein Transaction-
Bootstrap-Selbsttest. Der State-Test prüft stabile State-ID, expliziten
Unknown-State, Versionsfortschritt und Stale-State-Erkennung bei veralteter
Änderung. Der Transaction-Test prüft Begin, Prepare, Commit und Verify sowie
einen Versionskonflikt vor Commit. Der UEFI-Smoke-Test erreicht danach weiter
`NOVA_KERNEL_READY` und meldet:

```text
NOVA: Global State ABI 1.0, Versionen und Unknown-State-Pruefung bereit
NOVA: Transaction ABI 1.0, Begin-Prepare-Commit-Verify bereit
```

### Neuer UEFI-Boot-Splash und Boot-Konsole

Der neue NPSPEC-Pfad `docs/NPSPEC/Boot/newBoot` ist eingelesen und der UEFI-
Bootpfad nutzt nun ein gemeinsames Hintergrundbild als einzige statische
Bootgrafik. `boot/image/bootbackground.png` wird verlustfrei in das interne
NBS-Format gewandelt und als `BACKGRND.NBS` in die GPT/FAT32-UEFI-Image-Datei
geschrieben. Splash und Boot-Konsole werden nicht mehr als fertige Screenshots
angezeigt, sondern dynamisch auf diesen Hintergrund gezeichnet.

Die NBS-Konvertierung skaliert per hochwertigem Aspect-Fill mit zentriertem
Crop. Dadurch wird der Space-/Earth-Hintergrund nicht mehr verzerrt oder mit
sichtbaren schwarzen Rändern angezeigt. Der UEFI-Loader rendert den Hintergrund
bildschirmfüllend mit bilinearer Abtastung.

Der normale Boot zeigt standardmäßig den Splashscreen mit dem neuen Logo-Asset
`LOGO.NBS`. Schwarze Pixel des Logos werden beim Zeichnen ausgeblendet, sodass
nur Stern, Glow und NovaOS-Schriftzug über dem Hintergrund sichtbar bleiben.
Darunter wird die echte Fortschrittsleiste gezeichnet. `F3` öffnet die
Boot-Konsole; `ESC` wechselt von dort zurück zum Splash. Die Boot-Konsole
zeichnet ihr Panel, den Fortschrittsbalken und echte Loader-Logzeilen live aus
dem Bootzustand. Kernel-Lade- oder Validierungsfehler führen direkt in diese
Console-Ansicht und setzen den Diagnosemarker `UEFI:BOOT-CONSOLE-ERROR-VIEW`.
Der bestehende Kernel-Handoff bleibt unverändert und der erfolgreiche
UEFI-Display-Server-Test erreicht weiterhin den Desktop-, Startmenü-, Ribbon-
und Shutdown-Pfad.

### Semantic Core: Lookup, Projektionen und Kernel-Handles

Der UEFI-Kernel besitzt jetzt einen ersten statischen Semantic Core für die
späteren Systemdienste. Neben den bestehenden stabilen ObjectID-, NamespaceID-,
CapabilityID- und SemanticTypeID-Grundlagen werden Registry-Einträge,
Namespace-Einträge und ObjectID-Projektionen intern auflösbar.

Der Kernel prüft beim Start:

- Registry-Lookup für ObjectID und CapabilityID
- Namespace-Lookup für Systemeinträge
- ObjectID-zu-Namespace-Projektion
- Ablehnung unbekannter ObjectIDs
- Öffnen, Auflösen, Schließen und danach ungültiges Lookup eines
  Kernel-Object-Handles

Zusätzlich gibt der Kernel jetzt sichtbar aus:

```text
NOVA: ObjectID Registry Lookup bereit
NOVA: Namespace Lookup bereit
NOVA: ObjectID Projection Map bereit
NOVA: Kernel Object Handle ABI bereit
```

Die neue Handle-Schicht trennt erste Kernelreferenzen von rohen ObjectIDs.
Geschlossene Handles werden als inaktiv markiert und können vom frühen Kernel
wiederverwendet werden, ohne unbegrenzt neue Slots zu belegen. Der Build wurde
als UEFI-Image `build/nova-uefi.img` neu erzeugt.

Darauf aufbauend besitzt der frühe Kernel jetzt eine erste statische Capability-
Authority. Feste Boot-Capabilities erlauben nur definierte Rechte auf die
System-, Apps- und Boot-Objekte. `semantic_core_handle_open_by_object` prüft die
angeforderten Rechte vor dem Erzeugen eines Handles. Der Kernel-Selbsttest
bestätigt erlaubtes Lesen und weist ein nicht gewährtes Schreibrecht ab. Im
Bootlog erscheint zusätzlich:

```text
NOVA: Capability Authority Rechtepruefung bereit
```

Die Capability Authority besitzt nun außerdem Lookup und Revoke für einzelne
CapabilityIDs. Der Selbsttest legt eine frühe Volumes-Test-Capability an,
prüft deren Leserecht, entzieht sie wieder und bestätigt danach, dass weder der
Capability-Lookup noch die Rechteprüfung sie weiterhin akzeptieren. Dadurch ist
der Rechtepfad nicht nur statisch prüfend, sondern besitzt einen ersten
Lebenszyklus.

```text
NOVA: Capability Lifecycle Lookup und Revoke bereit
```

Handles werden nun auch bei späterer Nutzung erneut validiert. Die neue
Handle-Rechtevalidierung prüft das aktive Handle, die angeforderten Rechte und
danach erneut die aktuell aktive Capability. Der Selbsttest öffnet ein Handle
auf das Volumes-Objekt, validiert es, widerruft anschließend die zugehörige
Capability und bestätigt, dass dasselbe Handle danach nicht mehr nutzbar ist.

```text
NOVA: Handle Rechtevalidierung gegen Capabilities bereit
```

Der Namespace-Core kann jetzt zusätzlich absolute frühe Ein-Segment-Pfade
auflösen. Unterstützt werden im Bootstrap-Pfad `"/"` sowie die Root-Kinder
`"/System"`, `"/Benutzer"`, `"/Apps"`, `"/Volumes"` und `"/Boot"`. Der
Selbsttest prüft Root-, System- und Apps-Auflösung sowie die Ablehnung eines
nicht vorhandenen Pfads.

```text
NOVA: Namespace Pfadauflösung bereit
```

Auf dieser Pfadauflösung baut nun ein erster Pfad-zu-Handle-Pfad auf.
`semantic_core_handle_open_by_path` löst einen Namespace-Pfad auf, übernimmt die
ObjectID des Namespace-Eintrags, prüft die Capability-Rechte und erzeugt dann
ein Kernel-Object-Handle. Der Selbsttest öffnet `/System` mit Leserecht,
validiert das Handle und schließt es wieder.

```text
NOVA: Namespace Pfad zu Handle bereit
```

Der Namespace-Core besitzt nun erste Introspection. Über
`semantic_core_namespace_count_children` kann der frühe Kernel Kinder eines
Namespace-Knotens zählen und optional nach Flags filtern. Der Selbsttest prüft
für Root fünf Kinder, davon drei benutzersichtbare und zwei System-Namespace-
Einträge.

```text
NOVA: Namespace Introspection bereit
```

Die Introspection wurde um Enumeration erweitert.
`semantic_core_namespace_child_at` liefert einen direkten Kind-Namespace nach
Ordinal und optionalem Flag-Filter. Der Selbsttest fragt benutzersichtbare
Root-Kinder ab, prüft einen Out-of-range-Fall und liest außerdem das letzte
ungefilterte Root-Kind.

```text
NOVA: Namespace Enumeration bereit
```

Zusätzlich besitzt die ObjectID-Projection-Map nun erste Introspection.
`semantic_core_projection_count_by_namespace` zählt Projektionen pro Namespace,
`semantic_core_projection_at_namespace` liefert eine Projektion nach Ordinal.
Der Selbsttest prüft System-, Root- und Apps-Projektionen sowie einen
Out-of-range-Fall.

```text
NOVA: Projection Introspection bereit
```

Nach dem erneuten Einlesen der neuen Filesystem-NPSPECs besitzt der frühe
Kernel zusätzlich eine erste Filesystem Object Registry. Die sechs
Bootstrap-Namespace-Objekte `/`, `System`, `Benutzer`, `Apps`, `Volumes` und
`Boot` werden jetzt als stabile ObjectID-Datensätze mit Namespace-Slot, Name,
Flags und semantischer Namespace-Zuordnung geführt. Der Selbsttest prüft Lookup,
System-/UserVisible-Flagzählung und die Ablehnung unbekannter ObjectIDs.

```text
NOVA: Filesystem Object Registry bereit
```

Die Filesystem Object Registry kann jetzt auch nach Flags enumerieren.
`semantic_core_object_at_by_flags` liefert ein Objekt nach Ordinal innerhalb
eines Flag-Filters. Der Selbsttest prüft benutzersichtbare Objekte,
Systemobjekte und die Out-of-range-Ablehnung.

```text
NOVA: Filesystem Object Enumeration bereit
```

Object Registry und Projection Map besitzen nun eine frühe Konsistenzprüfung.
`semantic_core_object_validate_projection` prüft, ob eine ObjectID in der
Object Registry existiert, ob eine Projection dafür vorhanden ist und ob
Namespace-Slot sowie semantisches Ziel übereinstimmen. Der Selbsttest bestätigt
die gültige Apps-Projektion und weist eine unbekannte ObjectID ab.

```text
NOVA: Filesystem Object Projection Konsistenz bereit
```

Der Handle-Open-Pfad ist jetzt ebenfalls an diese Konsistenzprüfung gebunden.
`semantic_core_handle_open_by_object` validiert vor der Capability-Prüfung, dass
die angeforderte ObjectID in Object Registry und Projection Map konsistent
bekannt ist. Erst danach kann aus Capability und Projection ein Handle entstehen.
Der Selbsttest weist zusätzlich einen Handle-Open-Versuch auf eine unbekannte
ObjectID ab.

```text
NOVA: Handle Object-Registry Bindung bereit
```

Zusätzlich kann der Kernel Filesystem-Objekte jetzt direkt über Namespace-Pfade
auflösen. `semantic_core_object_resolve_path` löst den Pfad über den Namespace-
Core auf, übernimmt die ObjectID, validiert Object Registry und Projection Map
und gibt anschließend den Object-Datensatz zurück. Der Selbsttest prüft
`/System`, `/Apps` und einen fehlenden Pfad.

```text
NOVA: Filesystem Object Pfadauflösung bereit
```

## 16. Storage und persistentes NovaFS-Systemvolume

Der Kernel besitzt jetzt erstmals einen echten Datenträgerpfad und ein
persistentes Dateisystem als SystemRoot. Grundlage sind NPSPEC-NOVAFS-0001
(Phase 1 „NovaFS Core“), die Filesystem-/Storage-NPSPECs unter
`docs/NPSPEC/sysarchitecture/038-FILESYSTEM/` sowie die neue Byte-Layout-
Spezifikation `docs/NPSPEC/NPSPEC-NOVAFS-ONDISK-0001.md`.

### Storage-Bootstrap

- **implementiert:** PCI-Scan, AHCI-Controller im Polling-Betrieb mit
  HBA-Reset, SATA-Datenträgererkennung per `IDENTIFY DEVICE`, stabile
  DeviceID aus Seriennummer und Modell, `READ/WRITE DMA EXT`,
  `FLUSH CACHE EXT`, Registrierung im Device Manager und GPT-Suche mit
  Header- und Eintrags-CRC.
- **automatisiert getestet:** DMA-Lesetest und Bereichsprüfung beim Start;
  System ohne AHCI fällt sauber auf das Bootstrap-RAMFS zurück; zwei
  Datenträger mit Systemvolume auf Port 1.
- **offen:** Interrupt-Betrieb, NCQ, Anbindung an DMA-Mapping/IOMMU, NVMe.

### NovaFS 1.0 Phase 1

- **spezifiziert:** On-Disk-Format mit Superblock (primär und Backup),
  Bitmap, Object/Directory/Extent Tree, CRC32C und GPT-Typ-GUID
  `4E4F5641-4653-5359-5354-454D30303031`.
- **implementiert:** Mount mit Auswahl der höchsten gültigen Generation,
  Feature-Flag-Regeln, DIRTY → Read-only, Pfadauflösung, Lookup, Create,
  Read und Write mit Extents, Teilblöcken und Blatt-Splits; Schreibprotokoll
  DIRTY → Daten → Knoten → Bitmap → Flush → CLEAN → Backup.
- **integriert:** Root-Namespaces mit den Semantic-Core-ObjectIDs 2–6
  abgeglichen, Volume im Semantic Core registriert, VFS-Root auf
  `VFS_FLAG_NOVAFS_ROOT`, Boot Health `SystemRootReady`.
- **automatisiert getestet:** `make test-uefi-novafs` (12 QEMU-Starts:
  Persistenz über Neustarts, Wurzel- und Blatt-Splits aller Bäume durch den
  Kernel, Backup-Superblock mit Reparatur, DIRTY-Volume, unformatierte
  Partition) und `make novafs-check` (Host-Werkzeug mit `fsck`).
- **offen:** echtes Copy-on-Write, Checkpoints/Snapshots,
  Zusammenlegen leerer Blätter, Baumhöhe > 2.
  Transaction Log/Crash Recovery (Phase 2, `dev_detail.md` Abschnitt 101),
  Löschen und Umbenennen (Abschnitte 102–105), Zeitstempel (Abschnitt 106)
  und Nutzdatenprüfsummen (Abschnitt 107) sind inzwischen umgesetzt.

### VFS-Syscalls auf NovaFS (VFS ABI 1.1)

- **implementiert:** `VFS.Lookup` (absolute und relative Pfade, optional mit
  Schreibrecht), `Read`, `Write`, `Create`, `ReadDirectory` und `Query` über
  prozesslokale Handles mit den Rechten `READ`/`WRITE`; neue Capabilities
  `FS_READ`, `FS_WRITE` und getrennt `FS_SYSTEM_WRITE`. Ohne
  System-Write-Authority darf nur unterhalb von `/Benutzer` geschrieben
  werden; Capability und Policy werden bei jedem Schreibzugriff erneut
  geprüft.
- **automatisiert getestet:** Das Ring-3-Bootstrap-Programm liest die vom
  Kernel geschriebene Systemdatei, scheitert erwartungsgemäß beim Schreiben
  ohne Recht, mit veraltetem Handle und im Systembereich, legt
  `Willkommen.txt` im Profilordner `Dokumente` an, schreibt, liest zurück und findet sie per
  `ReadDirectory`. `make test-uefi-novafs` prüft das Ergebnis vom Host.
- **offen:** größere Benutzerpuffer mit allgemeinem
  Prozessspeicher, objektbezogene Capabilities und deklarative Policies.

### Explorer mit echten NovaFS-Inhalten

- **implementiert:** `make novafs-image` legt das Benutzerprofil
  `/Benutzer/<NOVAFS_USER>/{Desktop,Dokumente,Downloads,Bilder,Musik,Videos}`
  an (Standard `Matthias`). Das Ring-3-Programm öffnet das erste Profil unter
  `/Benutzer`, legt dort `Dokumente/Willkommen.txt` an, liest das Verzeichnis
  per `ReadDirectory` und übergibt dem Display-Dienst eine
  `NovaExplorerViewV1` (Display-Operation 4). Der Explorer zeigt daraus
  Breadcrumb, Ordnerkacheln, Dateiliste mit Typ und Größe, markiert
  „Dokumente“ im Schnellzugriff und zählt in der Fußzeile Ordner, Dateien und
  freien NovaFS-Platz.
- **Navigation:** Ordnerkarten per Klick oder Enter öffnen, „Zurück“ bzw.
  Backspace führt eine Ebene nach oben (bis `/Benutzer`), der Schnellzugriff
  (Start, Desktop, Dokumente, Downloads, Bilder, Musik, Videos) springt direkt
  in das Benutzerprofil. Pfeiltasten wechseln zwischen Dateizeilen und
  Ordnerkarten; die Seitenleiste markiert den aktuellen Ordner.
- **Grenzen:** höchstens 8 Einträge pro Ansicht (je 4 Ordner und Dateien),
  nur ASCII-Namen in der Anzeige, kein Datum, kein Scrollen, Dateien lassen
  sich noch nicht öffnen.

### Löschen und Umbenennen (VFS ABI 1.2)

- **implementiert:** `VFS.Delete` (Datei samt Extents oder leeres
  Verzeichnis) und `VFS.Rename` (Umbenennen und Verschieben zwischen
  Verzeichnissen) im Kernel und im Host-Werkzeug (`novafs rm`, `novafs mv`).
  Stabile Namespaces sind geschützt, ein vorhandenes Ziel wird nicht
  überschrieben, Verzeichnisse können nicht in sich selbst wandern. Beide
  Operationen verlangen Verzeichnis-Handles mit `WRITE` und unterliegen der
  `/Benutzer`-Policy.
- **automatisiert getestet:** Ring 3 legt bei jedem beschreibbaren Start eine
  Testdatei an, benennt sie um, verschiebt sie in einen Testordner, prüft
  `EXISTS`, `NOT_EMPTY` und den Zyklusschutz, löscht beides und stellt fest,
  dass ein offenes Handle danach `NOT_FOUND` liefert. Das Host-`fsck` prüft
  jetzt zusätzlich `parent_id` gegen den Verzeichniseintrag; der Host-Test
  leert einen geteilten Baum bis auf leere Blätter und befüllt ihn neu.
- **offen:** echtes Copy-on-Write (die hier gemeinte Phase-2-Crash-Recovery
  per Transaction Log ist inzwischen umgesetzt, siehe unten). Dateien im
  Explorer öffnen sowie Kontextaktionen (Löschen/Umbenennen) in der
  Explorer-Oberfläche sind ebenfalls umgesetzt (`dev_detail.md`
  Abschnitte 102–105).

Das Systemvolume bleibt über Kernel-Neubauten erhalten: `make uefi-image`
übernimmt die bestehende NovaFS-Partition aus `build/nova-uefi.img`.
`make novafs-reset` formatiert es neu, `make novafs-fsck` prüft es vom Host.

Der aktuelle UEFI-Start meldet unter anderem:

```text
NOVA: Storage ABI 1.0, AHCI-Controller (Polling) aktiv, Datentraeger 0x00000001
NOVA: NovaFS 1.0 Systemvolume gemountet, Generation 0x... VolumeID 0x...
NOVA: NovaFS Root-Layout konsistent mit Semantic-Core-ObjectIDs
NOVA: NovaFS persistenter Bootzaehler 0x...
NOVA: NovaFS ist persistentes SystemRoot unter /
NOVA: Boot Health SystemRoot bereit, wartet auf Trust
```

Technische Details stehen in `dev_detail.md`, Abschnitte 95 bis 107.

### NovaFS-/Semantic-Core-Abgleich `/Solutions`

- **neu eingelesen:** `NPSPEC-NOVAFS-ONDISK-0001` definiert `/Solutions` als
  stabiles Root-Objekt mit ObjectID 7 und `NAMESPACE`-Flag.
- **implementiert:** Der Kernel-Semantic-Core kennt jetzt
  `NOVA_NAMESPACE_SOLUTIONS`, legt Namespace, Projection, Object Registry und
  eine Read/Execute-Capability für ObjectID 7 an. Pfadauflösung und
  Selbsttest prüfen `/Solutions`.
- **NovaFS:** `novafs_check_root_layout` validiert jetzt zusätzlich
  `/Solutions → ObjectID 7`, damit das Systemvolume nicht als konsistent gilt,
  wenn der neue stabile Rootbereich fehlt.
- **Userspace-VFS:** Das Ring-3-Testprogramm öffnet `/`, enumeriert das
  Root-Verzeichnis per `ReadDirectory` und bricht ab, falls `/Solutions` nicht
  als Verzeichnis sichtbar ist. Damit ist der neue Namespace nicht nur im
  On-Disk-Layout und Semantic-Core vorhanden, sondern auch über die
  Userspace-Dateisystem-ABI erreichbar.
- **Build:** UEFI-Image neu erstellt: Kernel Build-ID
  `AA18D312460BC36F9C29DD530979316949681F16`, NKI CRC32 `B3A36CCE`.

### NovaFS Phase 2: Transaction Log und Crash Recovery

- **Spezifikation:** `NPSPEC-NOVAFS-ONDISK-0001` §9 (neu) beschreibt eine
  feste Transaction-Log-Region direkt nach der Bitmap (Header + 16
  Pre-Image-Slots) und ein Undo-Journal: jeder Block, der innerhalb einer
  laufenden Änderung zum ersten Mal beschrieben wird, wird vorher mit
  seinem bisherigen Inhalt gesichert. Normative Anforderung #6 erlaubt
  jetzt read-write-Mounts eines `DIRTY`-Volumes, wenn eine
  Journal-Wiederherstellung es zuvor nachweislich auf `CLEAN`
  zurückgeführt hat; neue Anforderungen #13/#14 schreiben das
  Capture-vor-Schreiben-Prinzip und die Alles-oder-nichts-Garantie der
  Wiederherstellung fest.
- **Kernel:** `novafs_write_block_logged` sichert vor jedem geschützten
  Schreibzugriff (Baumknoten, Bitmap, Datenblöcke, der Superblock selbst)
  das Pre-Image im Journal. `novafs_journal_undo` rollt beim Mount ein
  `DIRTY`-Volume mit gültigem, zur aktuellen Generation passendem Journal
  vollständig auf den Zustand vor der abgebrochenen Änderung zurück –
  das Volume wird danach normal (read-write) weitergemountet, statt wie
  bisher dauerhaft read-only zu bleiben. Scheitert eine VFS-Operation,
  ohne dass der Kernel neu startet, rollt `novafs_change_abort` sofort
  zurück (inklusive Neuladen der im Speicher gehaltenen Bitmap-Kopie);
  das Volume bleibt beschreibbar, statt für den Rest des Boots read-only
  zu werden. Nur wenn die Wiederherstellung selbst fehlschlägt (kein
  gültiges Journal, z. B. ein Volume aus der Zeit vor Phase 2), bleibt es
  beim alten Phase-1-Rückfall.
- **Host-Werkzeug:** `novafs.c` kennt dieselbe Journal-Region (Layout,
  Validierung, `fsck`-Reservierung); `mkfs` reserviert und initialisiert
  sie, `mark-dirty` journalisiert den Superblock-Schreibzugriff selbst,
  sodass es ein Abbild erzeugt, das der Kernel tatsächlich reparieren
  kann.
- **Test:** `test-uefi-novafs.ps1` deckt beide Fälle ab – ein `DIRTY`-Volume
  mit gültigem Journal wird repariert und bleibt beschreibbar
  (Bootzähler/Schreibtest laufen weiter, `fsck` danach fehlerfrei,
  `info` zeigt wieder `CLEAN`); ein `DIRTY`-Volume mit beschädigtem
  Journal fällt weiterhin auf Read-only zurück. End-to-End per QEMU-Boot
  gegen das echte UEFI-Image verifiziert.
- **Grenzen:** höchstens 16 gesicherte Blöcke pro Transaktion; die
  Wiederherstellung kennt nur Rollback (kein Redo), eine kurz vor dem
  letzten Schritt abgebrochene Transaktion gilt daher als nicht
  abgeschlossen; das Host-Werkzeug journalisiert nur den
  Superblock-Schreibzugriff in `mark-dirty`, nicht einzelne
  Knoten/Bitmap-Schreibzugriffe. Details in `dev_detail.md`, Abschnitt
  101.

### Explorer: Entf löscht den fokussierten Eintrag

- **implementiert:** Die Entf-Taste (Set 1 `0x53`, Set 2 `0x71`, jeweils
  erweitert) wird kernelseitig auf eine neue semantische Eingabeaktion
  `SYSTEM_INPUT_DELETE` abgebildet und wie alle anderen Eingaben über den
  Input-Router an das Ring-3-System-UI zugestellt. Im Explorer-Arbeitsbereich
  löscht sie den fokussierten Eintrag: Dateizeile (Fokus 20..23) oder
  Ordnerkarte (Fokus 24..27), je nach Fokuswert als `NOVAFS_TYPE_FILE` oder
  `NOVAFS_TYPE_DIRECTORY`. Eine neue Ring-3-Routine `ufs_delete_nth`
  durchsucht das aktuelle Arbeitsverzeichnis nach dem n-ten Eintrag dieses
  Typs (gleiches Scan-Muster wie `ufs_enter_child`) und löscht ihn über das
  bereits vorhandene `ufs_delete`. Da die Explorer-Navigation ihre
  Verzeichnis-Handles nur lesend öffnet, öffnet `ufs_delete_nth` den
  aktuellen Pfad (`UFS_CWD`) dafür kurz zusätzlich mit Schreibrecht und
  schließt dieses Handle danach wieder. Nach erfolgreichem Löschen wird die
  Ansicht sofort über `ufs_present` neu aus NovaFS aufgebaut und an den
  Display Server übergeben, sodass der Eintrag sofort verschwindet; schlägt
  das Löschen fehl (z. B. nicht-leerer Ordner), bleibt die bisherige Ansicht
  unverändert bestehen.
- **Ressourcengrenze:** Der Ring-3-Userspace-Code ist weiterhin hart auf
  zwei 4-KiB-Seiten begrenzt (`%error`-Prüfung beim Assemblieren). Die
  Tastatur-Zuordnung selbst liegt dagegen im Kernel (Ring 0) und unterliegt
  dieser Grenze nicht.
- **automatisiert getestet:** `test-uefi-display-server.ps1` öffnet den
  Explorer deterministisch über das Startmenü (Fokus wird dabei fest auf
  20 gesetzt), bewegt den Fokus mit vier Tab-Tastendrücken auf die erste
  Ordnerkarte, sendet Entf und prüft anhand der geloggten Eintragszahl,
  dass genau ein Eintrag verschwunden ist und das Verzeichnis dabei
  unverändert bleibt. Bei dieser Gelegenheit wurde auch ein spezifischer
  Fehler des Testskripts auf Linux/pwsh behoben (`-WindowStyle Hidden` ist
  unter PowerShell Core auf Linux nicht unterstützt), und ein Fehler im
  eigenen Testaufbau: `build-uefi-image.ps1` übernimmt ohne `-ResetNovaFs`
  eine bereits im Ziel-Image vorhandene NovaFS-Partition unverändert, statt
  sie durch das frisch formatierte `-NovaFsImage` zu ersetzen – wiederholte
  Testläufe mutierten so stillschweigend dasselbe persistente Testvolume
  weiter (z. B. eine zuvor gelöschte Testordner-Annahme des Skripts traf
  beim nächsten Lauf nicht mehr zu). Für reproduzierbare Testläufe daher
  `-ResetNovaFs` setzen.
- **später ergänzt (siehe nächster Abschnitt):** `ufs_delete_nth` wurde
  umgebaut, um die Suche nach dem n-ten Eintrag eines Typs in eine
  gemeinsame Routine `ufs_find_nth` auszulagern (auch von
  `ufs_preview_nth` genutzt) und dabei einen überflüssigen zweiten
  Verzeichnis-Scan unter dem Schreibrecht-Handle zu entfernen (der
  gefundene Name wird jetzt direkt per `ufs_delete` auf dem neu geöffneten
  Schreibrecht-Handle gelöscht, ohne ihn dort erneut zu suchen).
- **offen:** Umbenennen (F2) aus dem Explorer heraus; durch die sehr enge
  Ring-3-Codegrenze (siehe nächster Abschnitt: nur noch 13 Byte frei)
  voraussichtlich nur mit einer strukturellen Änderung (z. B. Verschieben
  von `USER_STACK_ADDRESS`, um den Codebereich zu vergrößern) machbar.

### Explorer: Enter öffnet eine Datei (einfache Inhaltsvorschau)

- **implementiert:** Enter/Aktivieren auf einer fokussierten Dateizeile
  (Fokus 20..23) tat bisher nichts (fiel wirkungslos in die
  Startmenü-Aktivierungslogik). Jetzt ruft es `ufs_preview_nth` auf: liest
  bis zu 56 Byte (`EXPLORER_PATH_MAX`) des Dateiinhalts und zeigt sie
  anstelle des Breadcrumbs (Pfadzeile) an. Es gibt noch kein eigenes
  Anzeigeelement für Dateiinhalte – die Vorschau missbraucht bewusst das
  vorhandene Pfadfeld, da ein neues UI-Element das Codebudget (siehe unten)
  nicht mehr zulässt. Nicht darstellbare Bytes werden wie beim Breadcrumb
  vom Kernel automatisch durch `-` ersetzt. Navigiert man danach weiter
  (Backspace, Enter auf einer Ordnerkarte, …), baut `ufs_present` den
  Breadcrumb wie gewohnt neu auf; die Vorschau ist also rein transient.
- **Refactoring für Platz:** `ufs_delete_nth` und das neue
  `ufs_preview_nth` teilen sich jetzt `ufs_find_nth` (EAX=NovaFS-Typ,
  EBX=Index -> Eintrag in `UFS_ENTRY`), statt den Verzeichnis-Scan doppelt
  zu implementieren. `ufs_preview_nth` öffnet die gefundene Datei readonly
  über `ufs_lookup`/`ufs_io` (Lesen reicht das vorhandene `HANDLE_RIGHT_READ`
  aus einem Lookup ohne `VFS_LOOKUP_FLAG_WRITE`), schreibt die gelesenen
  Bytes direkt in `UFS_VIEW + 40` und ruft `SYSCALL_DISPLAY_SUBMIT_EXPLORER_VIEW`
  erneut auf, ohne `ufs_build_view` (das den Breadcrumb wieder überschreiben
  würde) erneut zu durchlaufen.
- **Ressourcengrenze:** Nach diesem Feature plus dem oben beschriebenen
  Refactoring sind nur noch **13 von 8192 Byte** des Ring-3-Codebudgets
  frei – praktisch ausgeschöpft. Jede weitere Erweiterung (insbesondere
  Umbenennen/F2, das zusätzlich eine Texteingabe bräuchte) wird ohne eine
  strukturelle Änderung (zusätzliche Codeseite, verschobene
  `USER_STACK_ADDRESS`-Konstanten) nicht mehr hineinpassen.
- **automatisiert getestet:** `test-uefi-display-server.ps1` öffnet den
  Explorer über denselben Startmenü-Trick (Fokus fest auf 20, Verzeichnis
  bleibt dabei unverändert – beim Boot `Matthias/Dokumente` mit der
  einzigen Datei `Willkommen.txt`), sendet Enter und prüft, dass die
  nächste protokollierte Explorer-Ansicht den Dateiinhalt
  ("Willkommen bei NovaOS.") an Stelle des Pfads zeigt.
- **offen:** Es gibt keine echte Dateiinhalts-Anzeige (eigenes Fenster/
  Textfeld, Scrollen, mehr als 56 Byte); das ist ebenfalls erst nach einer
  strukturellen Erweiterung des Ring-3-Codebudgets sinnvoll umsetzbar.

### Ring-3-Codebudget: dritte Codeseite (strukturelle Erweiterung)

- **implementiert:** Das Ring-3-Codebudget wurde von zwei auf drei
  4-KiB-Seiten erweitert (`USER_STACK_ADDRESS` von `0x00403000` auf
  `0x00404000`; alle `UFS_*`-Scratch-Offsets sind relativ dazu definiert
  und verschieben sich automatisch korrekt mit). `userspace_initialize`
  kopiert jetzt drei statt zwei Codeseiten, jede unbedingt mit voller
  `PMM_PAGE_SIZE` (nicht längenabhängig, da NASMs `%if` nicht auf
  `userspace_program_end` vorwärtsverweisen kann).
- **zwei Fehler dabei gefunden und behoben:**
  1. `SHARED_SERVICE_ADDRESS` war hart auf den *alten*
     `USER_STACK_ADDRESS`-Wert codiert und kollidierte nach der
     Verschiebung mit der neuen Stackseite (sofortiger Seitenfehler-Crash
     beim ersten Stack-Zugriff). Behoben durch
     `SHARED_SERVICE_ADDRESS equ USER_STACK_ADDRESS` statt eines Literals.
  2. `userspace_system_scene` (die beim Boot einmalig gesendete
     Desktop-Szene) deklarierte nur 60 statt der von
     `SYSTEM_SCENE_SIZE`/dem Kernel erwarteten 64 Byte. Mit der alten,
     längenabhängigen Seitenkopie blieb das fehlende Byte zufällig immer
     `0` (die Seite wurde vorher genullt) und der Fehler blieb unsichtbar;
     mit der neuen, unbedingten Vollseitenkopie wurde dort echter
     Kernel-Bytewert sichtbar, die Reserviert-Prüfung schlug fehl und die
     Desktop-Szene wurde nie präsentiert (kein Absturz, nur ein
     stillschweigend übersprungener Darstellungsschritt). Behoben durch
     ein zusätzliches Reserve-Dword (`times 6 dd 0` statt `times 5`).
- **automatisiert getestet:** Nach beiden Fixes liefen
  `test-uefi-display-server.ps1` (alle drei Szenarien) und die
  vollständige `test-uefi-novafs.ps1`-Regressionssuite fehlerfrei durch,
  jeweils auf einem mit `-ResetNovaFs` frisch aufgesetzten Image.
- **Ergebnis:** **4099 von 12288 Byte** des Ring-3-Codebudgets sind jetzt
  frei – genug für Umbenennen (F2) samt einer einfachen Texteingabe.
- **offen:** Umbenennen (F2) selbst ist noch nicht implementiert.

### Explorer: Umbenennen (F2) mit einfacher Texteingabe

- **implementiert:** F2 auf einer fokussierten Datei-/Ordnerzeile startet
  die Umbenennung (vorbefüllt mit dem alten Namen); Tippen hängt Zeichen
  an, Backspace löscht das letzte Zeichen, Enter bestätigt (ruft
  `ufs_rename` auf), ein zweites F2 bricht ohne Speichern ab. Dafür
  wurde der Kernel-Tastatur-Handler um eine echte Texteingabe erweitert:
  eine neue 256-Byte-Tabelle (`keyboard_ascii_table`, Set 1,
  Kleinbuchstaben) übersetzt Tastendrücke in ASCII-Zeichen, die als neue
  semantische Aktion `SYSTEM_INPUT_TEXT_CHAR` an Ring 3 weitergereicht
  werden; `SYSTEM_INPUT_RENAME_KEY` ist die neue F2-Aktion. Backspace und
  Enter werden dabei kontextabhängig umgedeutet (normalerweise
  "Ordner hoch"/"öffnen", während der Umbenennung "Zeichen löschen"/
  "bestätigen") statt neue Tasten zu belegen; Escape bricht bewusst
  **nicht** ab, da es außerhalb des Startmenüs sonst sofort herunterfährt.
  Die Live-Anzeige nutzt wie die Dateivorschau (Abschnitt 103) das
  Pfadfeld zweckentfremdet, da es kein eigenes Eingabefeld gibt.
- **Ressourcengrenze:** Danach sind **3611 von 12288 Byte** des
  Ring-3-Codebudgets frei (vorher 4099).
- **automatisiert getestet:** `test-uefi-display-server.ps1` tippt nach
  dem Dateivorschau-Test (Fokus 20, `Willkommen.txt`) F2, 14x Backspace,
  dann `hallo`, und prüft nach jedem Tastendruck die protokollierte
  Zwischenansicht; ein abschließendes Enter muss wieder eine Ansicht mit
  normalem Breadcrumb liefern. Die tatsächliche Umbenennung auf der
  Platte wurde zusätzlich manuell mit dem Host-Werkzeug geprüft (Eintrag
  heißt danach `hallo`, gleiche Inode und Dateigröße wie vorher
  `Willkommen.txt` – echte Umbenennung, kein Löschen-und-Neuanlegen).
  Anschließend liefen die übrigen Display-Server-Szenarien und die
  vollständige `test-uefi-novafs.ps1`-Regressionssuite fehlerfrei durch.
- **offen:** kein eigenes Eingabefeld-UI-Element; keine
  Umschalt-Großschreibung/Sonderzeichen/Set-2-Texteingabe; Escape bricht
  nicht ab (nur zweites F2); Wechsel ins Startmenü während der
  Umbenennung nicht gezielt getestet; keine automatisierte
  Platten-Verifikation innerhalb des Testskripts selbst (nur manuell
  geprüft).

### NovaFS: Zeitstempel (created/modified/accessed/changed_time)

- **implementiert:** Die vier u64-Zeitstempelfelder im Object-Tree-Item
  (Offsets 40/48/56/64, bislang laut Spec „Phase 1: 0, keine Uhr") werden
  jetzt mit echter Unixzeit befüllt. Der Kernel liest dafür erstmals die
  batteriegepufferte CMOS-Echtzeituhr direkt über die Ports 0x70/0x71
  (`novafs_now_epoch`/`novafs_rtc_sample` in `novafs32.inc`), wertet BCD-
  und 12/24-Stunden-Kodierung über Status Register B aus und wandelt das
  Ergebnis über eine klassische Tage-seit-1970-Berechnung (mit
  Schaltjahrprüfung) in Sekunden seit der Unix-Epoche um; zwei
  aufeinanderfolgende Lesungen müssen übereinstimmen, sonst wird verworfen
  (Schutz gegen einen laufenden Registerupdate-Übergang). `created`,
  `modified`, `accessed` und `changed` werden beim Anlegen eines Objekts
  gleich gesetzt; ein Inhaltsschreiben (`novafs_write`) aktualisiert
  `modified`/`accessed`/`changed`; ein Umbenennen/Verschieben
  (`novafs_rename`) aktualisiert nur `changed` – auch bei einem reinen
  Namenswechsel innerhalb desselben Verzeichnisses, was vorher (für das
  Verschieben von `parent_id`) übersprungen wurde. Das Host-Werkzeug
  (`tools/novafs/novafs.c`) spiegelt dasselbe Verhalten mit `time(NULL)`
  an den entsprechenden Stellen (`create_object`, Root-Objekt-Init in
  `mkfs`, `file_write`, `object_rename`) und bekommt dafür einen neuen
  `stat <image> <pfad>`-Befehl, der alle vier Zeitstempel menschenlesbar
  anzeigt (bewusst als eigener Befehl statt einer Änderung an `ls`, damit
  bestehende Testskripte, die `ls`-Ausgabe zeilenweise parsen, unverändert
  funktionieren).
- **automatisiert getestet:** Nach einem mit dem neuen Host-Werkzeug frisch
  per `mkfs` erzeugten Systemvolume liefen `test-uefi-novafs.ps1`
  (vollständige Regressionssuite) und `test-uefi-display-server.ps1` (alle
  Szenarien, einschließlich des F2-Umbenennen-Tests aus Abschnitt 105)
  fehlerfrei durch. Mit `novafs stat` wurde anschließend am realen
  Testabbild geprüft, dass die umbenannte Datei einen späteren
  `changed`- als `created`/`modified`-Zeitstempel trägt (die
  Kernel-RTC-Uhrzeit stimmt dabei sichtbar mit der Wanduhrzeit des
  QEMU-Laufs überein) und dass ein reines Host-Tool-`mv` ebenfalls nur
  `changed` weiterschiebt. `novafs fsck` akzeptiert die neuen Felder ohne
  Änderung, da sie innerhalb des bereits durch CRC32C geschützten
  152-Byte-Objekt-Items lagen und vorher nur ungenutzte Nullbytes waren.
- **offen:** keine Zeitzonenkorrektur (die RTC wird als UTC behandelt);
  keine NMI-Maskierung während des CMOS-Zugriffs; `accessed_time` wird
  nur bei Erzeugung und Inhaltsschreiben aktualisiert, nicht bei jedem
  lesenden Zugriff (bewusste Vereinfachung, um nicht bei jedem Lesen eine
  journalisierte Schreibtransaktion auszulösen); Jahrhundert-Register
  0x32 wird nur im Bereich 19–21 akzeptiert (sonst wird 20 angenommen);
  Nutzdatenprüfsummen sind in §107 umgesetzt.

Details in `dev_detail.md`, Abschnitt 102 (Löschen), Abschnitt 103
(Öffnen), Abschnitt 104 (Codebudget-Erweiterung), Abschnitt 105
(Umbenennen) und Abschnitt 106 (Zeitstempel).

### NovaFS: Nutzdatenprüfsummen (CRC32C-Datenabsicherung)

Implementiert in `dev_detail.md` Abschnitt 107 (§107).

Die zweite explizit genannte Phase-1-Grenze aus `NPSPEC-NOVAFS-ONDISK-0001`
§8 ist jetzt umgesetzt: Jeder geschriebene Datei-Inhalts-Block erhält einen
CRC32C-Eintrag in einem flachen Prüfsummen-Array auf dem Volume; beim Lesen
wird der CRC32C nachgerechnet und bei Abweichung `NOVAFS_ERR_CORRUPT` (Kernel)
bzw. eine Warnung auf stderr (Userspace-Tool) ausgegeben.

**Kernel (`novafs32.inc`):** Zwei neue Subroutinen
`novafs_data_checksum_write` und `novafs_data_checksum_verify` flankieren
`novafs_write`/`novafs_read`; separater Page-Slot `nfs_checksum_page`.

**Userspace-Tool (`novafs.c`):** `checksum_store`/`checksum_verify` in
`file_write`/`file_read`; `cmd_mkfs` alloziert `ceil(N/1024)` Array-Blöcke;
`cmd_fsck` markiert diese als belegt.

Rückwärtskompatibel: Alte Volumes mit `checksum_tree_block = 0` erhalten
keine Prüfsummen-Operationen (No-op). Kernel-Binary: 198113 Byte
(Spielraum ~59 KB bis zur Decke 258048 Byte).

---

## §127 VFS.Query v2 — Zeitstempel (2026-10-08)

**Ziel:** Die vier NovaFS-Zeitstempel (Created, Modified, Accessed, Changed),
die seit §106 auf Disk gespeichert werden, über den VFS.Query-Syscall an
Userspace exponieren. Sichtbare Nutzung: Explorer-Dateivorschau zeigt ab jetzt
"YYYY-MM-DD [Inhalt]" statt nur Dateiinhalt.

**Kernel (`vfs32.inc`):**
`VFS_INFO_SIZE` von 48 → 80 Byte (acht neue Felder à 4 Byte).
`vfs_op_query` füllt nach Generation (Offset +40) die vier Timestamps:
`Created` (+48), `Modified` (+56), `Accessed` (+64), `Changed` (+72),
je als little-endian u64. Interner `vfs_args`-Puffer von 64 → 80 Byte.
Struct-Kommentar aktualisiert auf `NovaVfsObjectInfoV2`.

**Ring-3 (`entry32.asm`):**
`ufs_preview_nth` ruft jetzt vor dem VFS.Read einen `ufs_query`-Syscall auf,
liest `UFS_ARGS+56` (Modified-Timestamp, low 32 Bit) und übergibt ihn an die
neue Hilfsfunktion `ufs_format_date`. Diese berechnet Gregorianisches Datum
via `/ 86400` + Jahres-/Monatsschleife und schreibt "YYYY-MM-DD " (11 Byte) in
`UFS_VIEW+40`. Dateiinhalt folgt in `UFS_VIEW+51` (max. 45 Byte). Die Gesamtlänge
im View wird als `11 + bytes_read` eingetragen. Neue Datentabelle `ufs_month_days`
(12 Byte) für die Monatslängen.

`VFS_INFO_SIZE`-Konstante ist per `%include` geteilt → `ufs_query` (Ring-3)
nutzt automatisch den neuen Wert 80; kein gesonderter Ring-3-Patch nötig.
Schaltjahre: `% 4 == 0`-Test reicht für 1970–2099 (32-Bit-Timestamps laufen
spätestens 2106 über). UFS_ARGS-Puffer (96 Byte, USER_STACK_ADDRESS-1408) hat
ausreichend Platz für 80 Byte.

Kernel-Binary: 245760 Byte (~239 KB; Spielraum ~13 KB bis zur Decke 258048 Byte).

---

## §128 AP-Aktivierung & echter SMP-Betrieb (2026-10-08)

**Ziel:** Application Processors antworten korrekt auf Cross-CPU-IPIs. Bisher
fehlte der IDT-Eintrag für Vektor 0xFE (SMP_IPI_VECTOR), sodass jeder IPI
einen `isr_unexpected`-Pfad → Kernel-Panic ausgelöst hätte. Außerdem kannte
der AP nicht den empfangenen IPI-Typ, und TLB-Shootdown-Adressen wurden remote
nicht übergeben.

**Kernel (`entry32.asm`):**

*IPI-Dispatch-Handler `isr_ipi` (Vektor 0xFE):*
Neuer ISR ohne Ring-Wechsel (AP in Ring-0). Ablauf:
1. LAPIC-EOI (erlaubt erneute IPIs dieses Vektors)
2. Eigene APIC-ID aus LAPIC-ID-Register (`[0xFEE00020]` Bits 31:24)
3. CPU-Slot via `acpi_apic_ids`-Tabelle ermitteln
4. Mailbox `smp_ipi_mailbox[slot]` atomar mit `xchg` leeren
5. Dispatch: `TLB_SHOOTDOWN` → `invlpg [smp_shootdown_addr]`;
   `CPU_STOP/PANIC_STOP` → `cli; hlt`; `RESCHEDULE` → kein explizites Handling
   (AP kehrt nach `iret` in `sti; hlt`-Idle-Schleife zurück)

*Neue Datenbereiche:*
- `smp_ipi_mailbox: times CPU_CAPACITY dd 0` — per-CPU-Bitmaske ausstehender IPI-Typen
- `smp_shootdown_addr: dd 0` — virtuelle Adresse für Remote-TLB-Shootdown

*`smp_send_ipi` erweitert:* Schreibt `1 << IPI-Typ` per `lock or` in die
Mailbox des Ziel-CPU-Slots, bevor die LAPIC-ICR-Nachricht abgeschickt wird.

*`smp_tlb_shootdown_page` erweitert:* Schreibt die virtuelle Seitenadresse in
`smp_shootdown_addr` bevor der TLB_SHOOTDOWN-IPI gesendet wird.

*IDT-Registrierung:* `interrupt_initialize` trägt `isr_ipi` an Vektor 0xFE
(= SMP_IPI_VECTOR) ein — nach dem Syscall-Gate, vor `lidt`. Damit ist der
Vektor sowohl im PIC- als auch APIC-Pfad aktiv.

**Verhalten:**
- UP (1 CPU): kein AP-Trampoline, SMP-Code nicht aktiv → keine Änderung
- SMP (≥2 CPUs): APs empfangen RESCHEDULE-IPIs ohne Fault; TLB-Shootdown
  wirksam auf allen beteiligten APs; CPU_STOP hält APs sicher an
- `smp_self_test` unverändert bestanden (bestehende SMP-Prüfung verifiziert,
  dass `smp_send_ipi` erfolgreich war; Mailbox-Befüllung tritt jetzt dazu)

Kernel-Binary: 245760 Byte (~239 KB; Spielraum ~13 KB).
