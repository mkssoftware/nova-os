/* NovaOS C-Kernel - VFS-Syscall-Layer (Port aus vfs32.inc, NPSPEC-
   FILESYSTEM-NAMESPACE-0002, NPSPEC-FILESYSTEM-PERMISSION-0001,
   NPSPEC-POLICY-SYSTEMWRITE-0001). Reicht Ring-3-Clients echten
   Dateizugriff auf das bereits portierte NovaFS (novafs.c) durch:
   Pfad -> Aufloesung -> Capability-/Policy-Pruefung -> Handle.

   WICHTIG (Scope dieser Sitzung):
   - Pfadkenntnis erzeugt keine Authority: jede Operation verlangt ein
     Handle mit passendem Recht UND die Prozess-Capability (security.h).
     Schreiboperationen pruefen Capability und Systembereichs-Policy bei
     JEDEM Aufruf erneut (Revalidierung vor Commit, wie im alten ASM).
   - Ohne SECURITY_CAP_FS_SYSTEM_WRITE ist Schreiben nur unterhalb von
     "/Benutzer" erlaubt (vfsapi_requires_system_write, Ahnen-Walk via
     novafs_object_parent) - Administratorstatus allein genuegt nicht.
   - EIGENE, flache Handle-Tabelle (VFSAPI_HANDLE_CAPACITY Slots, {handle,
     object_id, rights, owner_pid}) statt des bereits vorhandenen
     object.c/handle.c-Paars: handle.c's `handle_owner_pid` ist ein
     EINZIGES globales Feld ohne oeffentlichen Setter (nur handle.c's
     eigener Selbsttest kann es ueberhaupt auf einen Wert != 0 setzen) -
     fuer einen zweiten echten Benutzer waere handle.c in seinem
     aktuellen Zustand unbenutzbar. Das bereits etablierte Muster fuer
     syscall-exponierte Ressourcen in diesem Kernel ist ohnehin eine
     eigene flache Tabelle (siehe surface.c/window.c fuer GUI) statt
     object.c/handle.c - dieses Modul folgt demselben Muster.
   - KEIN separates Bounce-Page/copy-from-user: dieser Kernel hat keine
     echte Pro-Prozess-Seitentabellen-Isolation (ein einziger, vom Ring-3-
     Testprogramm geteilter Adressraum, siehe ring3.h/syscall.c) - Pfad-/
     Puffer-Zeiger werden nach syscall_validate_user_range() direkt
     dereferenziert, wie es syscall.c's GUI-Dispatch bereits fuer seinen
     festen RING3_GUI_BUFFER_VADDR tut.
   - Fehlercodes sind gegenueber vfs32.inc's `vfs_map_error` reduziert:
     novafs.c selbst liefert bei einem Operationsfehler nur 0/-1 (kein
     detailliertes nfs_error-Analogon), daher mappen fehlgeschlagene
     novafs_*-Aufrufe hier einheitlich auf VFSAPI_STATUS_IO. Die feiner
     aufgeloesten Codes (ACCESS/NOT_DIRECTORY/NOT_FILE/VALIDATION/
     READ_ONLY/NOT_FOUND) liefert dagegen DIESE Schicht selbst, aus Infos,
     die ihr tatsaechlich vorliegen (Rechte-/Typ-/Mount-/Policy-Pruefung
     VOR dem novafs_*-Aufruf). */
#ifndef NOVA_VFSAPI_H
#define NOVA_VFSAPI_H

#include <stdint.h>

#define VFSAPI_HANDLE_CAPACITY 8u

#define VFSAPI_RIGHT_QUERY 0x01u
#define VFSAPI_RIGHT_READ  0x02u
#define VFSAPI_RIGHT_WRITE 0x04u

/* vfsapi_lookup()'s flags-Parameter. */
#define VFSAPI_LOOKUP_FLAG_WRITE 0x01u

#define VFSAPI_IO_MAX 4096u

#define VFSAPI_STATUS_OK            0
#define VFSAPI_STATUS_NOT_FOUND     (-2)
#define VFSAPI_STATUS_ACCESS        (-3)
#define VFSAPI_STATUS_IO            (-7)
#define VFSAPI_STATUS_READ_ONLY     (-12)
#define VFSAPI_STATUS_NOT_DIRECTORY (-20)
#define VFSAPI_STATUS_NOT_FILE      (-21)
#define VFSAPI_STATUS_LIMIT         (-24)
#define VFSAPI_STATUS_VALIDATION    (-26)

/* NovaVfsDirectoryEntry (reduziert, kein StructSize/Version-Feld - siehe
   Scope-Hinweis). name wird von vfsapi_read_directory() NICHT
   nullterminiert; name_len gibt die echte Laenge an. */
typedef struct {
    uint32_t object_id;
    uint32_t object_type;
    uint32_t name_len;
    uint32_t size_lo;
    uint32_t size_hi;
    char name[255];
} vfsapi_dir_entry_t;

/* NovaVfsObjectInfo (reduziert auf Rechte/Typ/Groesse - keine Zeitstempel,
   siehe novafs.h: kein RTC in novafs.c verdrahtet). */
typedef struct {
    uint32_t rights;
    uint32_t object_type;
    uint32_t size_lo;
    uint32_t size_hi;
} vfsapi_info_t;

void vfsapi_initialize(void);
int vfsapi_self_test(void); /* 0 = ok */

/* Loest path (path_len Byte, KEIN Nullterminator) relativ zu dir_handle
   auf - dir_handle==0 bedeutet "ab der NovaFS-Wurzel" (novafs_resolve_from
   behandelt ein fuehrendes '/' im Pfad ohnehin IMMER als absolut, siehe
   novafs.h). flags=VFSAPI_LOOKUP_FLAG_WRITE revalidiert sofort die
   Schreib-Policy (vfsapi_check_write) und vergibt zusaetzlich
   VFSAPI_RIGHT_WRITE auf das neue Handle. 0 = Erfolg (*out_handle
   gesetzt), sonst ein VFSAPI_STATUS_*-Code. */
int vfsapi_lookup(uint32_t pid, uint32_t dir_handle, const char *path, uint32_t path_len,
                   uint32_t flags, uint32_t *out_handle);

/* Liest bis zu length Byte (max. VFSAPI_IO_MAX) ab offset in buffer.
   *out_transferred kann wegen Dateiende kleiner als length sein (siehe
   novafs_read). 0 = Erfolg, sonst ein VFSAPI_STATUS_*-Code. */
int vfsapi_read(uint32_t pid, uint32_t handle, uint32_t offset, uint8_t *buffer,
                 uint32_t length, uint32_t *out_transferred);

/* Schreibt length Byte (max. VFSAPI_IO_MAX) aus buffer ab offset -
   erfordert ein Handle mit VFSAPI_RIGHT_WRITE (siehe vfsapi_lookup) UND
   eine erneute Policy-Pruefung (vfsapi_check_write). Journalisiert ueber
   novafs_change_begin/commit/abort; schlaegt der Rollback nach einem
   gescheiterten Commit selbst fehl, wird NovaFS fuer den Rest des Starts
   read-only (novafs_force_readonly). 0 = Erfolg (*out_transferred==length),
   sonst ein VFSAPI_STATUS_*-Code. */
int vfsapi_write(uint32_t pid, uint32_t handle, uint32_t offset, const uint8_t *buffer,
                  uint32_t length, uint32_t *out_transferred);

/* Legt name (name_len Byte) unter dir_handle an (object_type = NOVAFS_
   TYPE_FILE oder NOVAFS_TYPE_DIRECTORY, siehe novafs.h) - dir_handle
   braucht VFSAPI_RIGHT_WRITE. 0 = Erfolg (*out_handle mit QUERY|READ|
   WRITE auf das neue Objekt gesetzt), sonst ein VFSAPI_STATUS_*-Code. */
int vfsapi_create(uint32_t pid, uint32_t dir_handle, const char *name, uint32_t name_len,
                   uint32_t object_type, uint32_t *out_handle);

/* Liest den index-ten Verzeichniseintrag (0-basiert, KEINE garantierte
   Reihenfolge, siehe novafs_dir_entry_at) von dir_handle nach *out_entry.
   dir_handle braucht VFSAPI_RIGHT_READ. 0 = Erfolg, sonst ein
   VFSAPI_STATUS_*-Code (NOT_FOUND = Index >= Anzahl Eintraege). */
int vfsapi_read_directory(uint32_t pid, uint32_t dir_handle, uint32_t index,
                           vfsapi_dir_entry_t *out_entry);

/* Liest Rechte/Typ/Groesse von handle nach *out_info. 0 = Erfolg, sonst
   ein VFSAPI_STATUS_*-Code. */
int vfsapi_query(uint32_t pid, uint32_t handle, vfsapi_info_t *out_info);

/* Loescht eine Datei oder ein leeres Verzeichnis namens name unter
   dir_handle (VFSAPI_RIGHT_WRITE erforderlich). Offene Handles auf das
   geloeschte Objekt liefern danach NOT_FOUND bei weiteren Operationen
   (novafs_object_find schlaegt fehl). 0 = Erfolg, sonst ein
   VFSAPI_STATUS_*-Code. */
int vfsapi_delete(uint32_t pid, uint32_t dir_handle, const char *name, uint32_t name_len);

/* Benennt src_name unter src_dir_handle zu dst_name unter dst_dir_handle
   um (beide Handles brauchen VFSAPI_RIGHT_WRITE, Policy wird fuer beide
   Verzeichnisse geprueft). 0 = Erfolg, sonst ein VFSAPI_STATUS_*-Code. */
int vfsapi_rename(uint32_t pid, uint32_t src_dir_handle, const char *src_name, uint32_t src_len,
                   uint32_t dst_dir_handle, const char *dst_name, uint32_t dst_len);

/* Schliesst ein Handle (gibt den Tabellenslot frei). 0 = Erfolg, -1 wenn
   unbekannt oder owner_pid nicht passt. */
int vfsapi_close(uint32_t pid, uint32_t handle);

uint32_t vfsapi_handle_count(void);

#endif
