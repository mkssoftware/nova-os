/* NovaOS C-Kernel - Nova Executable Format (NEF) 1.0 - Binaerformat-
   Spezifikation (Phase 2 des NEF-Masterauftrags).

   WICHTIG - Scope/Stand dieser Phase: dieses Modul definiert NUR das
   On-Disk-Format selbst (Header/Section-Directory/Extension-Directory)
   plus Parser/Serializer/Validator. Es laedt NOCH KEINEN Prozess (das
   ist nef_loader.c, eine SPAETERE Phase) - absichtlich, damit das Format
   selbst zuerst unabhaengig getestet werden kann (siehe Phase-Vorgabe
   "nach jeder Phase kompilieren+testen, erst dann die naechste Phase").

   Entwurfsentscheidung "keine compilerabhaengigen Speicherlayouts als
   Dateiformat" (Masterauftrag Abschnitt 3): der On-Disk-Header wird NIE
   per Pointer-Cast auf eine C-struct gelesen/geschrieben (C-struct-
   Padding/Alignment ist compiler-/ABI-abhaengig). Stattdessen gibt es
   fuer jede Struktur ein Byte-Offset-Layout (NEF_HDR_OFF_*-Makros) PLUS
   explizite serialize/parse-Funktionen, die Feld fuer Feld lesen/
   schreiben (little-endian, wie x86 selbst). Die in-memory nef_header_t/
   nef_section_entry_t/nef_extension_entry_t-structs sind NUR bequeme
   Zwischendarstellungen fuer den Rest des Kernels, NIE die Byte-Quelle.

   Architektur-Hinweis (Masterauftrag Abschnitt 6): dieser Kernel ist
   32-Bit Protected Mode x86 (siehe project_c_kernel_architecture-Memo)
   - die EINZIGE tatsaechlich ausfuehrbare Zielarchitektur ist NEF_ARCH_
   X86_32. NEF_ARCH_X86_64/_ARM64 sind als Enum-Werte beschrieben (das
   Format KANN sie benennen), aber es gibt in diesem Kernel keinen
   Ausfuehrungsmechanismus dafuer - ein Loader MUSS eine Datei, deren
   einzige Architekturvariante nicht X86_32 ist, ablehnen, niemals
   vortaeuschen, sie auszufuehren (siehe nef_loader.c, spaetere Phase). */
#ifndef NOVA_NEF_H
#define NOVA_NEF_H

#include <stdint.h>

/* ---- Magic/Versionierung ------------------------------------------- */

/* 8 Bytes, ASCII "NOVANEF1" - erste 7 Bytes identifizieren das Format,
   die '1' ist bewusst Teil der Signatur (nicht der Versionsfelder), da
   ein voellig anderes Major-Format-Redesign (NEF 2.0 als eigenes
   On-Disk-Layout) eine eigene Signatur bekommen sollte, statt sich auf
   die Versionsfelder allein zu verlassen - ein Parser, der die FALSCHE
   Major-Version-Interpretation faelschlich auf eine inkompatible
   Byte-Struktur anwendet, waere gefaehrlicher als ein zweiter Magic-
   String. */
#define NEF_MAGIC_SIZE 8u
extern const uint8_t NEF_MAGIC[NEF_MAGIC_SIZE]; /* "NOVANEF1" */

#define NEF_VERSION_MAJOR_CURRENT 1u
#define NEF_VERSION_MINOR_CURRENT 0u

/* ---- Header (96 Bytes, siehe Byte-Layout unten) --------------------- */

#define NEF_HEADER_SIZE 96u

#define NEF_HDR_OFF_MAGIC               0u  /* 8 Byte */
#define NEF_HDR_OFF_VERSION_MAJOR       8u  /* u16 */
#define NEF_HDR_OFF_VERSION_MINOR       10u /* u16 */
#define NEF_HDR_OFF_HEADER_SIZE         12u /* u32 */
#define NEF_HDR_OFF_FILE_SIZE           16u /* u64 */
#define NEF_HDR_OFF_FORMAT_FLAGS        24u /* u32 */
#define NEF_HDR_OFF_TARGET_ARCH         28u /* u32 */
#define NEF_HDR_OFF_ABI_ID              32u /* u32 */
#define NEF_HDR_OFF_MIN_OS_VERSION      36u /* u32 */
#define NEF_HDR_OFF_ENTRY_POINT_REF     40u /* u64 */
#define NEF_HDR_OFF_SECTION_DIR_OFFSET  48u /* u64 */
#define NEF_HDR_OFF_SECTION_COUNT       56u /* u32 */
#define NEF_HDR_OFF_EXTENSION_DIR_OFFSET 60u /* u64 */
#define NEF_HDR_OFF_EXTENSION_COUNT     68u /* u32 */
#define NEF_HDR_OFF_MANIFEST_REF        72u /* u64 */
#define NEF_HDR_OFF_SIGNATURE_DIR_REF   80u /* u64 */
#define NEF_HDR_OFF_INTEGRITY_REF       88u /* u64 */
/* Ende: 96 Byte gesamt (NEF_HEADER_SIZE). */

/* Format-Flags (format_flags, Bitmaske). */
#define NEF_FLAG_EXECUTABLE       0x00000001u
#define NEF_FLAG_DYNAMIC_LIBRARY  0x00000002u
#define NEF_FLAG_SYSTEM_MODULE    0x00000004u
#define NEF_FLAG_SIGNED           0x00000008u
#define NEF_FLAG_HAS_NOVA_IR      0x00000010u
#define NEF_FLAG_AOT_COMPILED     0x00000020u

/* Zielarchitektur (target_arch) - siehe Dateikopf-Hinweis: NUR X86_32
   hat in diesem Kernel einen echten Ausfuehrungspfad. */
#define NEF_ARCH_UNKNOWN 0u
#define NEF_ARCH_X86_32  1u
#define NEF_ARCH_X86_64  2u
#define NEF_ARCH_ARM64   3u

/* ABI-Identifier (abi_id) - NEF's EIGENER ABI-Namespace, nicht
   zu verwechseln mit compat.h's ABI_*-Konstanten (die beschreiben
   FREMDE Personas/Binaerformate fuer die Kompatibilitaetsschicht;
   NEF_ABI_NOVA_NATIVE ist das native, direkte NovaOS-Syscall-ABI). */
#define NEF_ABI_NOVA_NATIVE 0u

/* In-memory-Darstellung - NIE direkt auf Dateibytes casten, siehe
   Dateikopf-Hinweis. Feld-Reihenfolge hier ist absichtlich identisch
   zur Byte-Reihenfolge (Lesbarkeit), aber C-Padding macht sizeof()
   dieser struct NIEMALS gleich NEF_HEADER_SIZE - deshalb serialisiert
   nef_header_serialize() jedes Feld einzeln. */
typedef struct {
    uint16_t version_major;
    uint16_t version_minor;
    uint32_t header_size;
    uint64_t file_size;
    uint32_t format_flags;
    uint32_t target_arch;
    uint32_t abi_id;
    uint32_t min_os_version;
    uint64_t entry_point_ref;
    uint64_t section_dir_offset;
    uint32_t section_count;
    uint64_t extension_dir_offset;
    uint32_t extension_count;
    uint64_t manifest_ref;
    uint64_t signature_dir_ref;
    uint64_t integrity_ref;
} nef_header_t;

/* Schreibt den Header kanonisch (little-endian, Feld fuer Feld) nach
   out_buf[0..NEF_HEADER_SIZE). out_cap MUSS >= NEF_HEADER_SIZE sein.
   magic wird IMMER auf NEF_MAGIC gesetzt (der Aufrufer kann es nicht
   faelschlich auf etwas anderes setzen). 0 = Erfolg, -1 wenn out_cap
   zu klein. */
int nef_header_serialize(const nef_header_t *in, uint8_t *out_buf, uint32_t out_cap);

/* Liest+validiert einen Header aus buf[0..buf_len). Prueft: buf_len
   reicht fuer NEF_HEADER_SIZE, Magic stimmt, header_size stimmt mit
   NEF_HEADER_SIZE ueberein (kein abweichendes Layout wird akzeptiert -
   siehe Kompatibilitaets-Hinweis zu Major-Versionen unten), file_size
   ist PLAUSIBEL (>= header_size, siehe nef_validate_header fuer die
   vollstaendige Cross-Validierung gegen die tatsaechliche Dateigroesse).
   0 = Erfolg (*out gesetzt), -1 bei jedem Strukturfehler (siehe
   NEF_PARSE_ERR_*-Rueckgabe ueber *out_err, falls out_err != 0). */
#define NEF_PARSE_OK                0
#define NEF_PARSE_ERR_TOO_SHORT     1
#define NEF_PARSE_ERR_BAD_MAGIC     2
#define NEF_PARSE_ERR_BAD_HDRSIZE   3
#define NEF_PARSE_ERR_BAD_FILESIZE  4
int nef_header_parse(const uint8_t *buf, uint32_t buf_len, nef_header_t *out, int *out_err);

/* Versions-/Kompatibilitaetsregeln (Masterauftrag Abschnitt 4 - Adaptive
   Versioned NEF Compatibility Engine, HIER der Kernformat-Teil; die
   Extension-spezifischen Regeln sind unten bei nef_extension_* und in
   nef_compat_resolve()). 1 = kompatibel, 0 = NICHT kompatibel (Datei
   MUSS kontrolliert abgelehnt werden, niemals stillschweigend
   akzeptiert - siehe Masterauftrag Abschnitt 4). Eine unbekannte Major-
   Version ist IMMER inkompatibel (das Kernformat selbst koennte sich
   unvorhersehbar geaendert haben). Eine hoehere Minor-Version innerhalb
   derselben Major-Version ist NUR kompatibel, wenn der Loader die
   tatsaechlich benutzten Minor-Features kennt - das prueft diese
   Funktion bewusst NICHT (sie kennt nur die Major/Minor-Zahlen, nicht
   den Inhalt) - die Pruefung gegen REALE Feature-Nutzung gehoert zu
   nef_compat_resolve() (Extension-Ebene), nicht hierher. */
int nef_header_version_supported(const nef_header_t *hdr);

/* ---- Section Directory (48 Byte pro Eintrag) ------------------------ */

#define NEF_SECTION_ENTRY_SIZE 48u

#define NEF_SEC_OFF_TYPE       0u  /* u32 */
#define NEF_SEC_OFF_FLAGS      4u  /* u32 */
#define NEF_SEC_OFF_FILE_OFFSET 8u /* u64 */
#define NEF_SEC_OFF_FILE_SIZE  16u /* u64 */
#define NEF_SEC_OFF_VADDR      24u /* u64 */
#define NEF_SEC_OFF_VSIZE      32u /* u64 */
#define NEF_SEC_OFF_ALIGNMENT  40u /* u32 */
#define NEF_SEC_OFF_RESERVED   44u /* u32 */
/* Ende: 48 Byte (NEF_SECTION_ENTRY_SIZE). */

#define NEF_SECTION_NULL        0u
#define NEF_SECTION_CODE        1u
#define NEF_SECTION_RODATA      2u
#define NEF_SECTION_DATA        3u
#define NEF_SECTION_BSS         4u
#define NEF_SECTION_IMPORTS     5u
#define NEF_SECTION_EXPORTS     6u
#define NEF_SECTION_RELOCATIONS 7u
#define NEF_SECTION_TLS         8u
#define NEF_SECTION_RESOURCES   9u
#define NEF_SECTION_MANIFEST    10u
#define NEF_SECTION_CAPABILITIES 11u
#define NEF_SECTION_SIGNATURES  12u
#define NEF_SECTION_DEBUG       13u
#define NEF_SECTION_OPTIMIZATION 14u
#define NEF_SECTION_NOVA_IR     15u
#define NEF_SECTION_ARCH_VARIANTS 16u
#define NEF_SECTION_EXTENSIONS  17u
#define NEF_SECTION_TYPE_COUNT  18u

/* Section-Flags (flags, Bitmaske) - Speicherzugriffsrechte UND Datei-
   Darstellung in einem Feld, siehe nef_validate_section() fuer die
   W^X-Durchsetzung (Masterauftrag Abschnitt 5: "Sections duerfen keine
   unzulaessigen ausfuehrbaren und beschreibbaren Speicherbereiche
   erzwingen" - READ|WRITE|EXEC zusammen ist ein harter Validierungs-
   fehler, nicht nur eine Empfehlung). */
#define NEF_SEC_FLAG_READ       0x00000001u
#define NEF_SEC_FLAG_WRITE      0x00000002u
#define NEF_SEC_FLAG_EXEC       0x00000004u
#define NEF_SEC_FLAG_COMPRESSED 0x00000008u
#define NEF_SEC_FLAG_SHARED     0x00000010u /* unveraenderliche, prozessuebergreifend teilbare Daten */

typedef struct {
    uint32_t type;
    uint32_t flags;
    uint64_t file_offset;
    uint64_t file_size;
    uint64_t vaddr;   /* 0 = nicht in den Prozess-Adressraum abgebildet */
    uint64_t vsize;   /* >= file_size (Differenz = Null-initialisiert, wie ELF .bss) */
    uint32_t alignment;
} nef_section_entry_t;

void nef_section_entry_serialize(const nef_section_entry_t *in, uint8_t *out_buf);
void nef_section_entry_parse(const uint8_t *buf, nef_section_entry_t *out);

/* Reine Struktur-/Richtlinienpruefung EINES Eintrags (kein Datei-
   Kontext) - W^X, vsize>=file_size, alignment ist Zweierpotenz. 0 =
   gueltig, -1 sonst. Cross-Validierung gegen die tatsaechliche
   Dateigroesse/Ueberlappung macht nef_validate_header(). */
int nef_validate_section(const nef_section_entry_t *sec);

/* ---- Extension Directory (24 Byte pro Eintrag) ---------------------- */

#define NEF_EXTENSION_ENTRY_SIZE 24u

#define NEF_EXT_OFF_TYPE_ID    0u  /* u32 */
#define NEF_EXT_OFF_VERSION    4u  /* u16 */
#define NEF_EXT_OFF_REQUIRED   6u  /* u8: 0=optional, 1=required */
#define NEF_EXT_OFF_RESERVED   7u  /* u8 */
#define NEF_EXT_OFF_DATA_OFFSET 8u /* u64 */
#define NEF_EXT_OFF_DATA_SIZE  16u /* u64 */
/* Ende: 24 Byte (NEF_EXTENSION_ENTRY_SIZE). */

typedef struct {
    uint32_t type_id;
    uint16_t version;
    uint8_t required; /* 0 = optional, 1 = verpflichtend */
    uint64_t data_offset;
    uint64_t data_size;
} nef_extension_entry_t;

void nef_extension_entry_serialize(const nef_extension_entry_t *in, uint8_t *out_buf);
void nef_extension_entry_parse(const uint8_t *buf, nef_extension_entry_t *out);

/* Bekannte Extension-Type-IDs - ein Loader, der einen TYP nicht kennt,
   behandelt ihn rein ueber das required-Flag (siehe nef_compat_resolve):
   unbekannt+optional -> ueberspringen, unbekannt+required -> ablehnen.
   Diese Liste ist absichtlich klein (Phase 2) - weitere Typen werden in
   spaeteren Phasen ergaenzt (Resources/Capabilities/Signatures haben
   zusaetzlich bereits eigene SECTION-Typen oben; ob ein konkretes
   Feature als SECTION oder EXTENSION beschrieben wird, haengt davon ab,
   ob es Rohdaten (Section) oder ein typisiertes, versioniertes
   Metadatum (Extension) ist). */
#define NEF_EXT_TYPE_RESERVED_NONE 0u

/* ---- Validierung des GESAMTEN Headers inkl. Cross-Checks ------------ */

#define NEF_VALIDATE_OK                   0
#define NEF_VALIDATE_ERR_PARSE             1  /* nef_header_parse schlug fehl */
#define NEF_VALIDATE_ERR_VERSION           2  /* unbekannte/inkompatible Major-Version */
#define NEF_VALIDATE_ERR_FILESIZE_MISMATCH 3  /* file_size stimmt nicht mit echter Dateigroesse */
#define NEF_VALIDATE_ERR_SECTION_DIR_RANGE 4  /* Section-Directory ausserhalb/ueberlappend */
#define NEF_VALIDATE_ERR_EXTENSION_DIR_RANGE 5
#define NEF_VALIDATE_ERR_SECTION_RANGE     6  /* eine Section zeigt ausserhalb der Datei */
#define NEF_VALIDATE_ERR_SECTION_POLICY    7  /* W^X o.ae. Richtlinienverstoss */
#define NEF_VALIDATE_ERR_SECTION_OVERLAP   8  /* zwei Sections ueberlappen sich im Dateibereich */
#define NEF_VALIDATE_ERR_OVERFLOW          9  /* Integer-Ueberlauf in einer Offset/Groessen-Rechnung */

/* Vollstaendige strukturelle Validierung EINER bereits im Speicher
   liegenden NEF-Datei (buf[0..buf_len), buf_len MUSS die echte, vom
   VFS gemeldete Dateigroesse sein - file_size im Header ist nur eine
   BEHAUPTUNG, die hier gegen die Wahrheit (buf_len) geprueft wird, nie
   umgekehrt). Prueft Header, Section-Directory-Lage, JEDE Section
   (Range+Policy+Overlap), Extension-Directory-Lage - NICHT die
   Extension-Eintraege selbst inhaltlich (das ist nef_compat_resolve(),
   da es die Faehigkeiten des LAUFENDEN Systems braucht, keine reine
   Dateiformat-Frage ist). Jede Offset/Groessen-Rechnung wird auf
   Ueberlauf geprueft (uint64_t-Additionen koennen theoretisch
   umlaufen), bevor sie gegen buf_len verglichen wird. 0 = strukturell
   gueltig, sonst einer der NEF_VALIDATE_ERR_*-Codes. */
int nef_validate_header(const uint8_t *buf, uint32_t buf_len, nef_header_t *out_hdr,
                         int *out_err);

int nef_self_test(void); /* 0 = ok */

#endif
