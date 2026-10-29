/* NovaOS C-Kernel - VFS-Syscall-Layer (siehe vfsapi.h fuer Scope). */
#include "vfsapi.h"
#include "novafs.h"
#include "security.h"
#include "block.h"
#include "pmm.h"
#include "process.h"

typedef struct {
    uint32_t handle; /* 0 = Slot frei */
    uint32_t object_id;
    uint32_t rights;
    uint32_t owner_pid;
} vfsapi_handle_slot_t;

static vfsapi_handle_slot_t vfsapi_handles[VFSAPI_HANDLE_CAPACITY];
static uint32_t vfsapi_next_handle = 1;

/* Lazily aufgeloester "/Benutzer"-ObjectID fuer die Systembereichs-Policy
   (NPSPEC-POLICY-SYSTEMWRITE-0001) - siehe vfsapi_requires_system_write.
   0 = noch nicht gefunden (wird bei jedem Aufruf erneut versucht, bis
   "/Benutzer" existiert). */
static uint32_t vfsapi_benutzer_object_id = 0;

void vfsapi_initialize(void) {
    for (uint32_t i = 0; i < VFSAPI_HANDLE_CAPACITY; ++i) vfsapi_handles[i] = (vfsapi_handle_slot_t){0};
    vfsapi_next_handle = 1;
    vfsapi_benutzer_object_id = 0;
}

uint32_t vfsapi_handle_count(void) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < VFSAPI_HANDLE_CAPACITY; ++i) {
        if (vfsapi_handles[i].handle != 0) ++count;
    }
    return count;
}

static uint32_t vfsapi_new_handle(uint32_t pid, uint32_t object_id, uint32_t rights) {
    for (uint32_t i = 0; i < VFSAPI_HANDLE_CAPACITY; ++i) {
        if (vfsapi_handles[i].handle == 0) {
            uint32_t h = vfsapi_next_handle++;
            if (vfsapi_next_handle == 0) vfsapi_next_handle = 1; /* Ueberlauf, theoretisch */
            vfsapi_handles[i].handle = h;
            vfsapi_handles[i].object_id = object_id;
            vfsapi_handles[i].rights = rights;
            vfsapi_handles[i].owner_pid = pid;
            return h;
        }
    }
    return 0;
}

/* out_rights optional (0 = ignorieren). */
static int vfsapi_resolve_handle(uint32_t pid, uint32_t handle, uint32_t required_rights,
                                  uint32_t *out_object_id, uint32_t *out_rights) {
    if (handle == 0) return -1;
    for (uint32_t i = 0; i < VFSAPI_HANDLE_CAPACITY; ++i) {
        if (vfsapi_handles[i].handle != handle) continue;
        if (vfsapi_handles[i].owner_pid != pid) return -1;
        if ((vfsapi_handles[i].rights & required_rights) != required_rights) return -1;
        if (out_object_id != 0) *out_object_id = vfsapi_handles[i].object_id;
        if (out_rights != 0) *out_rights = vfsapi_handles[i].rights;
        return 0;
    }
    return -1;
}

int vfsapi_close(uint32_t pid, uint32_t handle) {
    for (uint32_t i = 0; i < VFSAPI_HANDLE_CAPACITY; ++i) {
        if (vfsapi_handles[i].handle != handle) continue;
        if (vfsapi_handles[i].owner_pid != pid) return -1;
        vfsapi_handles[i] = (vfsapi_handle_slot_t){0};
        return 0;
    }
    return -1;
}

/* Laeuft hoechstens 64 Ahnenstufen hoch (wie novafs_rename's Zyklen-
   Schutz) und meldet 1 (= Systembereichs-Capability erforderlich), wenn
   object_id (oder einer seiner Vorfahren) NICHT "/Benutzer" ist/enthaelt
   - inklusive wenn "/Benutzer" gar nicht existiert (konservativer
   Fallback: dann ist nichts geschuetzt ausgenommen). 0 = unterhalb (oder
   gleich) "/Benutzer" - normale FS_WRITE-Capability reicht. */
static int vfsapi_requires_system_write(uint32_t object_id) {
    /* Erneut versuchen, SOLANGE "/Benutzer" noch nicht gefunden wurde
       (nicht nur einmalig, siehe vfsapi_benutzer_resolved) - sonst wuerde
       der allererste Aufruf (der ja typischerweise genau dann passiert,
       wenn "/Benutzer" per vfsapi_create() selbst erst angelegt wird,
       siehe vfsapi_self_test) die Nichtexistenz permanent fuer den Rest
       des Starts zwischenspeichern, obwohl "/Benutzer" direkt danach
       erfolgreich angelegt wurde. Einmal gefunden wird NICHT erneut
       nachgesehen (ein spaeteres Loeschen von "/Benutzer" ist in diesem
       reduzierten Modell nicht vorgesehen). */
    if (vfsapi_benutzer_object_id == 0) {
        uint32_t id, type;
        if (novafs_resolve_from(1u, "/Benutzer", 9u, &id, &type) == 0 &&
            type == NOVAFS_TYPE_DIRECTORY) {
            vfsapi_benutzer_object_id = id;
        }
    }
    if (vfsapi_benutzer_object_id == 0) return 1;

    uint32_t current = object_id;
    for (uint32_t depth = 0; depth < 64u; ++depth) {
        if (current == vfsapi_benutzer_object_id) return 0;
        if (current == 1u) break; /* Wurzel erreicht, kein Treffer */
        uint32_t parent;
        if (novafs_object_parent(current, &parent) != 0) break;
        current = parent;
    }
    return 1;
}

/* Prueft Mountzustand, Read-only-Flag, FS_WRITE-Capability und ggf. die
   Systembereichs-Policy (Port aus vfs32.inc's vfs_check_write) - von
   jeder Schreiboperation bei JEDEM Aufruf erneut aufzurufen
   (Revalidierung vor Commit, siehe vfsapi.h). 0 = erlaubt, sonst ein
   VFSAPI_STATUS_*-Code. */
static int vfsapi_check_write(uint32_t pid, uint32_t object_id) {
    if (!novafs_is_mounted()) return VFSAPI_STATUS_NOT_FOUND;
    if (novafs_is_readonly()) return VFSAPI_STATUS_READ_ONLY;
    if (!security_check(pid, SECURITY_CAP_FS_WRITE)) return VFSAPI_STATUS_ACCESS;
    if (vfsapi_requires_system_write(object_id)) {
        if (!security_check(pid, SECURITY_CAP_FS_SYSTEM_WRITE)) return VFSAPI_STATUS_ACCESS;
    }
    return 0;
}

/* Port aus vfs32.inc's vfs_change_end: op_status==0 versucht einen
   Commit; scheitert dieser, wird wie bei einem bereits fehlgeschlagenen
   op_status abgebrochen (novafs_change_abort). Scheitert der Abort
   SELBST, wird NovaFS dauerhaft read-only (novafs_force_readonly) - der
   urspruengliche op_status bleibt dabei die Rueckgabe, nicht der
   Abort-Fehler (der Aufrufer soll erfahren, WARUM die Operation
   scheiterte, nicht dass zusaetzlich ihr Rollback Probleme hatte). */
static int vfsapi_change_end(int op_status) {
    if (op_status == 0) {
        if (novafs_change_commit() == 0) return 0;
        op_status = VFSAPI_STATUS_IO;
    }
    if (novafs_change_abort() != 0) {
        novafs_force_readonly();
    }
    return op_status;
}

int vfsapi_lookup(uint32_t pid, uint32_t dir_handle, const char *path, uint32_t path_len,
                   uint32_t flags, uint32_t *out_handle) {
    if (path == 0 || out_handle == 0) return VFSAPI_STATUS_VALIDATION;
    if (path_len == 0 || path_len > 255u) return VFSAPI_STATUS_VALIDATION;
    if (!novafs_is_mounted()) return VFSAPI_STATUS_NOT_FOUND;
    if (!security_check(pid, SECURITY_CAP_FS_READ)) return VFSAPI_STATUS_ACCESS;

    uint32_t start_dir = 1u; /* NovaFS-Wurzel - ein fuehrendes '/' im Pfad
                                 ignoriert start_dir ohnehin, siehe
                                 novafs_resolve_from. */
    if (dir_handle != 0u) {
        if (vfsapi_resolve_handle(pid, dir_handle, VFSAPI_RIGHT_QUERY, &start_dir, 0) != 0) {
            return VFSAPI_STATUS_ACCESS;
        }
    }

    uint32_t object_id, type;
    if (novafs_resolve_from(start_dir, path, path_len, &object_id, &type) != 0) {
        return VFSAPI_STATUS_NOT_FOUND;
    }

    uint32_t rights = VFSAPI_RIGHT_QUERY | VFSAPI_RIGHT_READ;
    if (flags & VFSAPI_LOOKUP_FLAG_WRITE) {
        int wr = vfsapi_check_write(pid, object_id);
        if (wr != 0) return wr;
        rights |= VFSAPI_RIGHT_WRITE;
    }

    uint32_t handle = vfsapi_new_handle(pid, object_id, rights);
    if (handle == 0) return VFSAPI_STATUS_LIMIT;
    *out_handle = handle;
    return 0;
}

int vfsapi_read(uint32_t pid, uint32_t handle, uint32_t offset, uint8_t *buffer,
                uint32_t length, uint32_t *out_transferred) {
    if (buffer == 0 || out_transferred == 0) return VFSAPI_STATUS_VALIDATION;
    if (length > VFSAPI_IO_MAX) return VFSAPI_STATUS_VALIDATION;
    if (!security_check(pid, SECURITY_CAP_FS_READ)) return VFSAPI_STATUS_ACCESS;

    uint32_t object_id;
    if (vfsapi_resolve_handle(pid, handle, VFSAPI_RIGHT_READ, &object_id, 0) != 0) {
        return VFSAPI_STATUS_ACCESS;
    }
    uint32_t type;
    if (novafs_object_info(object_id, &type, 0, 0) != 0) return VFSAPI_STATUS_IO;
    if (type != NOVAFS_TYPE_FILE) return VFSAPI_STATUS_NOT_FILE;

    if (length == 0u) {
        *out_transferred = 0u;
        return 0;
    }

    if (novafs_read(object_id, offset, length, buffer, out_transferred) != 0) {
        return VFSAPI_STATUS_IO;
    }
    return 0;
}

int vfsapi_write(uint32_t pid, uint32_t handle, uint32_t offset, const uint8_t *buffer,
                  uint32_t length, uint32_t *out_transferred) {
    if (buffer == 0 || out_transferred == 0) return VFSAPI_STATUS_VALIDATION;
    if (length > VFSAPI_IO_MAX) return VFSAPI_STATUS_VALIDATION;

    uint32_t object_id;
    if (vfsapi_resolve_handle(pid, handle, VFSAPI_RIGHT_WRITE, &object_id, 0) != 0) {
        return VFSAPI_STATUS_ACCESS;
    }
    int wr = vfsapi_check_write(pid, object_id);
    if (wr != 0) return wr;

    uint32_t type;
    if (novafs_object_info(object_id, &type, 0, 0) != 0) return VFSAPI_STATUS_IO;
    if (type != NOVAFS_TYPE_FILE) return VFSAPI_STATUS_NOT_FILE;

    if (length == 0u) {
        *out_transferred = 0u;
        return 0;
    }

    if (novafs_change_begin() != 0) return VFSAPI_STATUS_READ_ONLY;
    int rc = novafs_write(object_id, offset, length, buffer);
    int status = vfsapi_change_end((rc == 0) ? 0 : VFSAPI_STATUS_IO);
    if (status != 0) return status;

    *out_transferred = length;
    return 0;
}

int vfsapi_create(uint32_t pid, uint32_t dir_handle, const char *name, uint32_t name_len,
                   uint32_t object_type, uint32_t *out_handle) {
    if (name == 0 || out_handle == 0) return VFSAPI_STATUS_VALIDATION;
    if (name_len == 0 || name_len > 255u) return VFSAPI_STATUS_VALIDATION;
    if (object_type != NOVAFS_TYPE_FILE && object_type != NOVAFS_TYPE_DIRECTORY) {
        return VFSAPI_STATUS_VALIDATION;
    }
    if (!novafs_is_mounted()) return VFSAPI_STATUS_NOT_FOUND;

    uint32_t parent_id;
    if (vfsapi_resolve_handle(pid, dir_handle, VFSAPI_RIGHT_WRITE, &parent_id, 0) != 0) {
        return VFSAPI_STATUS_ACCESS;
    }
    int wr = vfsapi_check_write(pid, parent_id);
    if (wr != 0) return wr;

    uint32_t parent_type;
    if (novafs_object_info(parent_id, &parent_type, 0, 0) != 0) return VFSAPI_STATUS_IO;
    if (parent_type != NOVAFS_TYPE_DIRECTORY) return VFSAPI_STATUS_NOT_DIRECTORY;

    if (novafs_change_begin() != 0) return VFSAPI_STATUS_READ_ONLY;
    uint32_t new_id = 0;
    int rc = novafs_create(parent_id, object_type, name, name_len, &new_id);
    int status = vfsapi_change_end((rc == 0) ? 0 : VFSAPI_STATUS_IO);
    if (status != 0) return status;

    uint32_t handle = vfsapi_new_handle(pid, new_id,
                                         VFSAPI_RIGHT_QUERY | VFSAPI_RIGHT_READ | VFSAPI_RIGHT_WRITE);
    if (handle == 0) return VFSAPI_STATUS_LIMIT;
    *out_handle = handle;
    return 0;
}

int vfsapi_read_directory(uint32_t pid, uint32_t dir_handle, uint32_t index,
                           vfsapi_dir_entry_t *out_entry) {
    if (out_entry == 0) return VFSAPI_STATUS_VALIDATION;
    if (!novafs_is_mounted()) return VFSAPI_STATUS_NOT_FOUND;
    if (!security_check(pid, SECURITY_CAP_FS_READ)) return VFSAPI_STATUS_ACCESS;

    uint32_t dir_id;
    if (vfsapi_resolve_handle(pid, dir_handle, VFSAPI_RIGHT_READ, &dir_id, 0) != 0) {
        return VFSAPI_STATUS_ACCESS;
    }
    uint32_t dir_type;
    if (novafs_object_info(dir_id, &dir_type, 0, 0) != 0) return VFSAPI_STATUS_IO;
    if (dir_type != NOVAFS_TYPE_DIRECTORY) return VFSAPI_STATUS_NOT_DIRECTORY;

    uint8_t item[NOVAFS_DIR_ITEM_SIZE];
    if (novafs_dir_entry_at(dir_id, index, item) != 0) return VFSAPI_STATUS_NOT_FOUND;

    uint32_t child_id, child_type, name_len;
    novafs_dir_entry_decode(item, &child_id, &child_type, out_entry->name, &name_len);
    out_entry->object_id = child_id;
    out_entry->object_type = child_type;
    out_entry->name_len = name_len;

    uint32_t size_lo, size_hi;
    if (novafs_object_info(child_id, 0, &size_lo, &size_hi) != 0) return VFSAPI_STATUS_IO;
    out_entry->size_lo = size_lo;
    out_entry->size_hi = size_hi;
    return 0;
}

int vfsapi_query(uint32_t pid, uint32_t handle, vfsapi_info_t *out_info) {
    if (out_info == 0) return VFSAPI_STATUS_VALIDATION;
    if (!novafs_is_mounted()) return VFSAPI_STATUS_NOT_FOUND;

    uint32_t object_id, rights;
    if (vfsapi_resolve_handle(pid, handle, VFSAPI_RIGHT_QUERY, &object_id, &rights) != 0) {
        return VFSAPI_STATUS_ACCESS;
    }

    uint32_t type, size_lo, size_hi;
    if (novafs_object_info(object_id, &type, &size_lo, &size_hi) != 0) return VFSAPI_STATUS_IO;

    out_info->rights = rights;
    out_info->object_type = type;
    out_info->size_lo = size_lo;
    out_info->size_hi = size_hi;
    return 0;
}

int vfsapi_delete(uint32_t pid, uint32_t dir_handle, const char *name, uint32_t name_len) {
    if (name == 0) return VFSAPI_STATUS_VALIDATION;
    if (name_len == 0 || name_len > 255u) return VFSAPI_STATUS_VALIDATION;
    if (!novafs_is_mounted()) return VFSAPI_STATUS_NOT_FOUND;

    uint32_t parent_id;
    if (vfsapi_resolve_handle(pid, dir_handle, VFSAPI_RIGHT_WRITE, &parent_id, 0) != 0) {
        return VFSAPI_STATUS_ACCESS;
    }
    int wr = vfsapi_check_write(pid, parent_id);
    if (wr != 0) return wr;

    uint32_t parent_type;
    if (novafs_object_info(parent_id, &parent_type, 0, 0) != 0) return VFSAPI_STATUS_IO;
    if (parent_type != NOVAFS_TYPE_DIRECTORY) return VFSAPI_STATUS_NOT_DIRECTORY;

    if (novafs_change_begin() != 0) return VFSAPI_STATUS_READ_ONLY;
    int rc = novafs_delete(parent_id, name, name_len);
    return vfsapi_change_end((rc == 0) ? 0 : VFSAPI_STATUS_IO);
}

int vfsapi_rename(uint32_t pid, uint32_t src_dir_handle, const char *src_name, uint32_t src_len,
                   uint32_t dst_dir_handle, const char *dst_name, uint32_t dst_len) {
    if (src_name == 0 || dst_name == 0) return VFSAPI_STATUS_VALIDATION;
    if (src_len == 0 || src_len > 255u) return VFSAPI_STATUS_VALIDATION;
    if (dst_len == 0 || dst_len > 255u) return VFSAPI_STATUS_VALIDATION;
    if (!novafs_is_mounted()) return VFSAPI_STATUS_NOT_FOUND;

    uint32_t src_parent, dst_parent;
    if (vfsapi_resolve_handle(pid, src_dir_handle, VFSAPI_RIGHT_WRITE, &src_parent, 0) != 0) {
        return VFSAPI_STATUS_ACCESS;
    }
    if (vfsapi_resolve_handle(pid, dst_dir_handle, VFSAPI_RIGHT_WRITE, &dst_parent, 0) != 0) {
        return VFSAPI_STATUS_ACCESS;
    }
    int wr = vfsapi_check_write(pid, src_parent);
    if (wr != 0) return wr;
    wr = vfsapi_check_write(pid, dst_parent);
    if (wr != 0) return wr;

    uint32_t src_type, dst_type;
    if (novafs_object_info(src_parent, &src_type, 0, 0) != 0) return VFSAPI_STATUS_IO;
    if (src_type != NOVAFS_TYPE_DIRECTORY) return VFSAPI_STATUS_NOT_DIRECTORY;
    if (novafs_object_info(dst_parent, &dst_type, 0, 0) != 0) return VFSAPI_STATUS_IO;
    if (dst_type != NOVAFS_TYPE_DIRECTORY) return VFSAPI_STATUS_NOT_DIRECTORY;

    if (novafs_change_begin() != 0) return VFSAPI_STATUS_READ_ONLY;
    int rc = novafs_rename(src_parent, src_name, src_len, dst_parent, dst_name, dst_len);
    return vfsapi_change_end((rc == 0) ? 0 : VFSAPI_STATUS_IO);
}

/* ===================================================================== */
/* Selbsttest: eigenstaendige Fixture ueber novafs_build_test_volume()    */
/* (novafs.h) - nur die Wurzel existiert, "/Benutzer" und alles weitere   */
/* wird waehrend des Tests selbst per vfsapi_create angelegt (gleiches    */
/* Muster wie novafs.c's eigener Objekt-Lifecycle-Selbsttest).            */
/* ===================================================================== */

#define VFSAPI_TEST_MAX_BLOCKS 48u

static uint32_t vfsapi_test_block_numbers[VFSAPI_TEST_MAX_BLOCKS];
static uint8_t *vfsapi_test_block_pages[VFSAPI_TEST_MAX_BLOCKS];
static uint32_t vfsapi_test_block_count = 0;

static uint8_t *vfsapi_test_block(uint32_t block) {
    for (uint32_t i = 0; i < vfsapi_test_block_count; ++i) {
        if (vfsapi_test_block_numbers[i] == block) return vfsapi_test_block_pages[i];
    }
    if (vfsapi_test_block_count >= VFSAPI_TEST_MAX_BLOCKS) return 0;
    uint32_t numa;
    uint32_t page = pmm_alloc_page(&numa);
    if (page == 0) return 0;
    uint8_t *buf = (uint8_t *)(uintptr_t)page;
    for (uint32_t i = 0; i < NOVAFS_BLOCK_SIZE; ++i) buf[i] = 0;
    vfsapi_test_block_numbers[vfsapi_test_block_count] = block;
    vfsapi_test_block_pages[vfsapi_test_block_count] = buf;
    ++vfsapi_test_block_count;
    return buf;
}

static int vfsapi_test_read(void *record, uint32_t lba_lo, uint32_t lba_hi,
                             uint32_t count, void *buffer) {
    (void)record;
    (void)lba_hi;
    (void)count;
    uint32_t block = lba_lo / NOVAFS_SECTORS_PER_BLOCK;
    uint8_t *page = vfsapi_test_block(block);
    if (page == 0) return -1;
    for (uint32_t j = 0; j < NOVAFS_BLOCK_SIZE; ++j) ((uint8_t *)buffer)[j] = page[j];
    return 0;
}

static int vfsapi_test_write(void *record, uint32_t lba_lo, uint32_t lba_hi,
                              uint32_t count, void *buffer) {
    (void)record;
    (void)lba_hi;
    (void)count;
    uint32_t block = lba_lo / NOVAFS_SECTORS_PER_BLOCK;
    uint8_t *page = vfsapi_test_block(block);
    if (page == 0) return -1;
    const uint8_t *src = (const uint8_t *)buffer;
    for (uint32_t j = 0; j < NOVAFS_BLOCK_SIZE; ++j) page[j] = src[j];
    return 0;
}

static int vfsapi_test_flush(void *record) {
    (void)record;
    return 0;
}

int vfsapi_self_test(void) {
    uint32_t total = 64u;
    if (novafs_build_test_volume(vfsapi_test_block, total, 60u, 61u, 62u) != 0) return -1;

    block_register_params_t params = {
        .id = 0x56465331u, /* "VFS1" */
        .flags = 0,
        .block_size = 512u,
        .block_count_lo = total * NOVAFS_SECTORS_PER_BLOCK,
        .block_count_hi = 0,
        .transport = BLOCK_TRANSPORT_VIRT,
        .driver_handle = 0,
    };
    void *record = 0;
    if (block_device_register(&params, vfsapi_test_read, vfsapi_test_write, vfsapi_test_flush,
                               0, &record) != 0) {
        return -1;
    }
    if (novafs_probe_volume(record, 0u, total * NOVAFS_SECTORS_PER_BLOCK) != 0) return -1;

    vfsapi_initialize();

    /* Zwei Test-Prozesse: "root_pid" wie der Kernel (security_initialize
       gewaehrt PID 1 ohnehin SECURITY_KERNEL_CAPS), "user_pid" NUR mit
       FS_READ/FS_WRITE (OHNE FS_SYSTEM_WRITE) - fuer den eigentlichen
       Policy-Beweis (NPSPEC-POLICY-SYSTEMWRITE-0001). */
    uint32_t root_pid = 1u;
    uint32_t user_pid = process_create(0u, 0u);
    if (user_pid == 0u) return -1;
    if (security_grant(user_pid, SECURITY_CAP_FS_READ | SECURITY_CAP_FS_WRITE) != 0) return -1;

    /* "/Benutzer" als Kernel anlegen (Bootstrap eines Systembereichs-
       Verzeichnisses ist selbst immer ein Systembereichs-Schreiben). */
    uint32_t root_handle_k;
    if (vfsapi_lookup(root_pid, 0u, "/", 1u, VFSAPI_LOOKUP_FLAG_WRITE, &root_handle_k) != 0) {
        return -1;
    }
    uint32_t benutzer_handle_k;
    if (vfsapi_create(root_pid, root_handle_k, "Benutzer", 8u, NOVAFS_TYPE_DIRECTORY,
                       &benutzer_handle_k) != 0) {
        return -1;
    }
    if (vfsapi_close(root_pid, benutzer_handle_k) != 0) return -1;
    if (vfsapi_close(root_pid, root_handle_k) != 0) return -1;

    /* user_pid darf NICHT ausserhalb von "/Benutzer" schreiben - weder
       per Schreib-Lookup auf die Wurzel noch per anschliessendem
       vfsapi_create. */
    uint32_t root_handle_denied;
    if (vfsapi_lookup(user_pid, 0u, "/", 1u, VFSAPI_LOOKUP_FLAG_WRITE, &root_handle_denied) !=
        VFSAPI_STATUS_ACCESS) {
        return -1;
    }

    /* user_pid DARF unterhalb von "/Benutzer" schreiben, trotz fehlender
       FS_SYSTEM_WRITE-Capability. */
    uint32_t benutzer_handle_u;
    if (vfsapi_lookup(user_pid, 0u, "/Benutzer", 9u, VFSAPI_LOOKUP_FLAG_WRITE,
                       &benutzer_handle_u) != 0) {
        return -1;
    }

    uint32_t file_handle;
    if (vfsapi_create(user_pid, benutzer_handle_u, "notes.txt", 9u, NOVAFS_TYPE_FILE,
                       &file_handle) != 0) {
        return -1;
    }

    /* Falscher Objekttyp: eine Datei kann nicht als Verzeichnis gelesen
       werden - hier pruefen, SOLANGE file_handle noch auf ein lebendes
       Objekt zeigt (spaeter im Test wird die Datei umbenannt und
       geloescht, siehe unten). */
    vfsapi_dir_entry_t bogus_entry;
    if (vfsapi_read_directory(user_pid, file_handle, 0u, &bogus_entry) != VFSAPI_STATUS_NOT_DIRECTORY) {
        return -1;
    }

    /* Schreiben + Lesen + Query auf der neuen Datei. */
    const uint8_t content[5] = {'H', 'a', 'l', 'l', 'o'};
    uint32_t transferred = 0;
    if (vfsapi_write(user_pid, file_handle, 0u, content, 5u, &transferred) != 0) return -1;
    if (transferred != 5u) return -1;

    uint8_t readback[5] = {0};
    if (vfsapi_read(user_pid, file_handle, 0u, readback, 5u, &transferred) != 0) return -1;
    if (transferred != 5u) return -1;
    for (uint32_t i = 0; i < 5u; ++i) {
        if (readback[i] != content[i]) return -1;
    }

    vfsapi_info_t info;
    if (vfsapi_query(user_pid, file_handle, &info) != 0) return -1;
    if (info.object_type != NOVAFS_TYPE_FILE) return -1;
    if (info.size_lo != 5u || info.size_hi != 0u) return -1;
    if ((info.rights & (VFSAPI_RIGHT_QUERY | VFSAPI_RIGHT_READ | VFSAPI_RIGHT_WRITE)) !=
        (VFSAPI_RIGHT_QUERY | VFSAPI_RIGHT_READ | VFSAPI_RIGHT_WRITE)) {
        return -1;
    }

    /* VFS.ReadDirectory muss genau diese eine Datei auflisten. */
    vfsapi_dir_entry_t entry;
    if (vfsapi_read_directory(user_pid, benutzer_handle_u, 0u, &entry) != 0) return -1;
    if (entry.object_type != NOVAFS_TYPE_FILE) return -1;
    if (entry.name_len != 9u) return -1;
    for (uint32_t i = 0; i < 9u; ++i) {
        if (entry.name[i] != "notes.txt"[i]) return -1;
    }
    if (entry.size_lo != 5u) return -1;
    if (vfsapi_read_directory(user_pid, benutzer_handle_u, 1u, &entry) != VFSAPI_STATUS_NOT_FOUND) {
        return -1;
    }

    /* Umbenennen, dann Loeschen - beides unterhalb "/Benutzer" erlaubt. */
    if (vfsapi_rename(user_pid, benutzer_handle_u, "notes.txt", 9u, benutzer_handle_u,
                       "readme.txt", 10u) != 0) {
        return -1;
    }
    uint32_t renamed_handle;
    if (vfsapi_lookup(user_pid, benutzer_handle_u, "readme.txt", 10u, 0u, &renamed_handle) != 0) {
        return -1;
    }
    vfsapi_close(user_pid, renamed_handle); /* nur zur Existenzpruefung geoeffnet */
    if (vfsapi_lookup(user_pid, benutzer_handle_u, "notes.txt", 9u, 0u, &renamed_handle) !=
        VFSAPI_STATUS_NOT_FOUND) {
        return -1;
    }

    if (vfsapi_delete(user_pid, benutzer_handle_u, "readme.txt", 10u) != 0) return -1;
    if (vfsapi_lookup(user_pid, benutzer_handle_u, "readme.txt", 10u, 0u, &renamed_handle) !=
        VFSAPI_STATUS_NOT_FOUND) {
        return -1;
    }

    /* Falsches Handle-Recht: ein ohne VFSAPI_LOOKUP_FLAG_WRITE geoeffnetes
       Handle darf keine Schreiboperation zulassen. */
    uint32_t readonly_handle;
    if (vfsapi_lookup(user_pid, 0u, "/Benutzer", 9u, 0u, &readonly_handle) != 0) return -1;
    uint32_t dummy_handle;
    if (vfsapi_create(user_pid, readonly_handle, "x.txt", 5u, NOVAFS_TYPE_FILE, &dummy_handle) !=
        VFSAPI_STATUS_ACCESS) {
        return -1;
    }

    vfsapi_close(user_pid, file_handle);
    vfsapi_close(user_pid, benutzer_handle_u);
    vfsapi_close(user_pid, readonly_handle);
    if (vfsapi_handle_count() != 0u) return -1;

    return 0;
}
