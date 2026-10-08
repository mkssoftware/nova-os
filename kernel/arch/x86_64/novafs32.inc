; ---------------------------------------------------------------------------
; NovaFS 1.0 Phase 1 – Kernel-Implementierung (NPSPEC-NOVAFS-0001,
; NPSPEC-NOVAFS-ONDISK-0001)
;
; Superblock-Auswahl (primaer/Backup, hoechste Generation), CRC32C fuer alle
; Metadaten, Free-Space-Bitmap, Object/Directory/Extent Tree als B+-Baeume mit
; fester Itemgroesse (Hoehe <= 2), Pfadaufloesung, Lesen, Schreiben,
; Erzeugen, Loeschen und Umbenennen. Aenderungen folgen der Phase-1-Schreibreihenfolge
; DIRTY -> Daten -> Knoten -> Bitmap -> Superblock CLEAN -> Backup.
;
; Die Implementierung ist fuer den Single-Threaded-Bootpfad bestimmt; eine
; Syscall-Anbindung benoetigt zuvor eine Sperre um alle novafs_*-Einstiege.
; Alle Objekt-IDs, Blocknummern und Dateigroessen sind auf 32 Bit begrenzt
; (die High-DWORDs werden beim Mount bzw. Zugriff geprueft).
; ---------------------------------------------------------------------------

NOVAFS_BLOCK_SIZE          equ 4096
NOVAFS_NODE_HEADER         equ 80
NOVAFS_NODE_MAGIC          equ 0x4552544E   ; "NTRE"
NOVAFS_INNER_ITEM          equ 24
NOVAFS_INNER_MAX           equ 167
NOVAFS_MAX_BITMAP_BLOCKS   equ 4
NOVAFS_INCOMPAT_KNOWN      equ 3
NOVAFS_STATE_CLEAN         equ 0
NOVAFS_STATE_DIRTY         equ 1
NOVAFS_TYPE_FILE           equ 1
NOVAFS_TYPE_DIRECTORY      equ 2
NOVAFS_FIRST_DYNAMIC_ID    equ 256

; Transaction Log (Phase 2, NPSPEC-NOVAFS-ONDISK-0001 §9)
NOVAFS_JOURNAL_SLOTS       equ 16
NOVAFS_JOURNAL_BLOCKS      equ NOVAFS_JOURNAL_SLOTS + 1
NOVAFS_JOURNAL_MAGIC       equ 0x4E524A4E   ; "NJRN"
NFS_J_MAGIC       equ 0
NFS_J_VERSION     equ 4
NFS_J_GENERATION  equ 8
NFS_J_COUNT       equ 16
NFS_J_CHECKSUM    equ 24
NFS_J_ENTRIES     equ 56

; Superblock
NFS_SB_MAGIC          equ 0
NFS_SB_VMAJ           equ 8
NFS_SB_BLOCK_SIZE     equ 12
NFS_SB_FS_SIZE        equ 16
NFS_SB_TOTAL          equ 24
NFS_SB_AVAILABLE      equ 32
NFS_SB_GENERATION     equ 40
NFS_SB_OBJECT_TREE    equ 56
NFS_SB_DIRECTORY_TREE equ 64
NFS_SB_EXTENT_TREE    equ 72
NFS_SB_CHECKSUM_TREE  equ 88
NFS_SB_JOURNAL_BLOCK  equ 112
NFS_SB_FREE_SPACE     equ 96
NFS_SB_INCOMPAT       equ 128
NFS_SB_ROCOMPAT       equ 136
NFS_SB_UUID           equ 144
NFS_SB_NAME           equ 192
NFS_SB_CHECKSUM       equ 352
NFS_SB_STATE          equ 388
NFS_SB_NEXT_OBJECT    equ 392
NFS_SB_BITMAP_BLOCKS  equ 400
NFS_SB_OBJECT_COUNT   equ 408
NFS_SB_BACKUP_BLOCK   equ 416
NFS_SB_BITMAP_CRC     equ 424
NFS_SB_MOUNT_COUNT    equ 432
; §125 – Snapshot-Tabelle im Superblock (ab Offset 436)
NFS_SB_SNAP_COUNT     equ 436
NFS_SB_SNAP_TABLE     equ 440
NFS_SNAP_GENERATION   equ 0
NFS_SNAP_OBJ_ROOT     equ 8
NFS_SNAP_DIR_ROOT     equ 12
NFS_SNAP_EXT_ROOT     equ 16
NFS_SNAP_CSM_ROOT     equ 20
NFS_SNAP_TIMESTAMP    equ 24
NFS_SNAP_VALID        equ 32
NFS_SNAP_ENTRY        equ 40
NFS_MAX_SNAPSHOTS     equ 4
NFS_MAX_HEIGHT        equ 4

; Baumknoten
NFS_N_MAGIC       equ 0
NFS_N_LEVEL       equ 4
NFS_N_COUNT       equ 6
NFS_N_TREE        equ 8
NFS_N_BLOCK       equ 16
NFS_N_PARENT      equ 24
NFS_N_GENERATION  equ 32
NFS_N_CHECKSUM    equ 40
NFS_N_ITEM_SIZE   equ 72
NFS_N_MAX_ITEMS   equ 74
NFS_N_KEY_SIZE    equ 76

; Baumdeskriptor
NFS_TD_ID         equ 0
NFS_TD_ITEM       equ 4
NFS_TD_MAX        equ 8
NFS_TD_KEY        equ 12
NFS_TD_ROOT       equ 16

; Objekt-Item (152 Byte)
NFS_O_ID          equ 0
NFS_O_PARENT      equ 8
NFS_O_TYPE        equ 16
NFS_O_FLAGS       equ 20
NFS_O_SIZE        equ 24
NFS_O_ALLOCATED   equ 32
NFS_O_CREATED     equ 40   ; u64, Unix-Epoch-Sekunden (0 = unbekannt/Alt-Volume)
NFS_O_MODIFIED    equ 48   ; u64, Unix-Epoch-Sekunden
NFS_O_ACCESSED    equ 56   ; u64, Unix-Epoch-Sekunden (nur bei Erzeugung/Schreiben gesetzt, siehe Abschnitt 106)
NFS_O_CHANGED     equ 64   ; u64, Unix-Epoch-Sekunden (Metadaten-Aenderung)
NFS_O_PERMISSIONS equ 80
NFS_O_LINKS       equ 84
NFS_O_GENERATION  equ 112
NFS_O_CHECKSUM    equ 120
NFS_O_ITEM        equ 152

; Verzeichnis-Item (288 Byte)
NFS_D_PARENT      equ 0
NFS_D_HASH        equ 8
NFS_D_OBJECT      equ 16
NFS_D_TYPE        equ 24
NFS_D_NAME_LENGTH equ 28
NFS_D_NAME        equ 32
NFS_D_ITEM        equ 288

; Extent-Item (72 Byte)
NFS_E_OBJECT      equ 0
NFS_E_LOGICAL     equ 8
NFS_E_PHYSICAL    equ 16
NFS_E_COUNT       equ 24
NFS_E_UNCOMPRESSED equ 32
NFS_E_STORED      equ 40
NFS_E_GENERATION  equ 56
NFS_E_ITEM        equ 72

NOVAFS_ERR_NONE        equ 0
NOVAFS_ERR_IO          equ 1
NOVAFS_ERR_CORRUPT     equ 2
NOVAFS_ERR_NOT_FOUND   equ 3
NOVAFS_ERR_EXISTS      equ 4
NOVAFS_ERR_FULL        equ 5
NOVAFS_ERR_READ_ONLY   equ 6
NOVAFS_ERR_INVALID     equ 7
NOVAFS_ERR_TREE_FULL   equ 8
NOVAFS_ERR_NOT_EMPTY   equ 9
NOVAFS_ERR_PROTECTED   equ 10

NOVA_VOLUME_FS_NOVAFS  equ 2
VFS_FLAG_NOVAFS_ROOT   equ 0x00000004

; ---------------------------------------------------------------------------
; Blockzugriff
; ---------------------------------------------------------------------------

; EAX=Volume-Block, EDI=Seitenpuffer. CF bei Fehler.
novafs_read_block:
    pushad
    cmp eax, [nfs_limit_blocks]
    jae .invalid
    shl eax, 3
    add eax, [nfs_lba]
    jc .invalid
    mov edx, eax
    mov eax, [nfs_dev]
    mov ecx, 8
    call storage_read
    jc .io
    popad
    clc
    ret
.io:
    mov dword [nfs_error], NOVAFS_ERR_IO
.invalid:
    popad
    stc
    ret

novafs_write_block:
    pushad
    cmp dword [nfs_readonly], 0
    jne .invalid
    cmp eax, [nfs_limit_blocks]
    jae .invalid
    shl eax, 3
    add eax, [nfs_lba]
    jc .invalid
    mov edx, eax
    mov eax, [nfs_dev]
    mov ecx, 8
    mov esi, edi
    call storage_write
    jc .io
    inc dword [nfs_write_count]
    popad
    clc
    ret
.io:
    mov dword [nfs_error], NOVAFS_ERR_IO
.invalid:
    popad
    stc
    ret

; ---------------------------------------------------------------------------
; Transaction Log (Phase 2, NPSPEC-NOVAFS-ONDISK-0001 §9): Undo-Journal.
; Jeder Block, der innerhalb einer laufenden Aenderung (novafs_change_begin
; ... novafs_change_commit) zum ersten Mal geschrieben wird, wird vorher mit
; seinem bisherigen Inhalt gesichert. Nur novafs_write_block_logged nutzt
; dies; der Backup-Superblock bleibt bewusst aussen vor (siehe §9).
; ---------------------------------------------------------------------------

; Schreibt den Journal-Header aus nfs_journal_count/nfs_journal_entries.
; CF bei E/A-Fehler.
novafs_journal_write_header:
    pushad
    mov edi, [nfs_journal_header_page]
    xor eax, eax
    mov ecx, NOVAFS_BLOCK_SIZE / 4
    push edi
    rep stosd
    pop edi
    mov dword [edi + NFS_J_MAGIC], NOVAFS_JOURNAL_MAGIC
    mov dword [edi + NFS_J_VERSION], 1
    mov eax, [nfs_sb + NFS_SB_GENERATION]
    mov [edi + NFS_J_GENERATION], eax
    mov eax, [nfs_sb + NFS_SB_GENERATION + 4]
    mov [edi + NFS_J_GENERATION + 4], eax
    mov eax, [nfs_journal_count]
    mov [edi + NFS_J_COUNT], eax
    xor ecx, ecx
.copy:
    cmp ecx, [nfs_journal_count]
    jae .copied
    mov eax, [nfs_journal_entries + ecx * 4]
    mov [edi + NFS_J_ENTRIES + ecx * 4], eax
    inc ecx
    jmp .copy
.copied:
    mov esi, edi
    mov ecx, NOVAFS_BLOCK_SIZE
    mov edx, NFS_J_CHECKSUM
    call novafs_seal
    mov eax, [nfs_sb + NFS_SB_JOURNAL_BLOCK]
    call novafs_write_block
    popad
    ret

; EAX=Blocknummer, die vor einem Schreibzugriff gesichert werden soll. Ohne
; Effekt, wenn keine Aenderung laeuft oder der Block in dieser Transaktion
; bereits gesichert ist. Erhaelt EAX/EDI des Aufrufers. CF=1 bei Fehler
; (dann wurde weder das Journal noch der Zielblock durch diesen Aufruf
; veraendert, ausser der Daten-Slot wurde bereits geschrieben, aber der
; Header-Schreibzugriff schlug fehl).
novafs_journal_capture:
    cmp dword [nfs_txn_open], 0
    je .ok
    push eax
    push ecx
    push edx
    push esi
    push edi
    push ebx
    mov ebx, eax
    xor ecx, ecx
.scan:
    cmp ecx, [nfs_journal_count]
    jae .not_found
    cmp [nfs_journal_entries + ecx * 4], ebx
    je .ok_pop
    inc ecx
    jmp .scan
.not_found:
    cmp dword [nfs_journal_count], NOVAFS_JOURNAL_SLOTS
    jae .full
    mov eax, ebx
    mov edi, [nfs_journal_slot_page]
    call novafs_read_block
    jc .fail
    mov eax, [nfs_sb + NFS_SB_JOURNAL_BLOCK]
    add eax, 1
    add eax, [nfs_journal_count]
    mov edi, [nfs_journal_slot_page]
    call novafs_write_block
    jc .fail
    mov ecx, [nfs_journal_count]
    mov [nfs_journal_entries + ecx * 4], ebx
    inc ecx
    mov [nfs_journal_count], ecx
    call novafs_journal_write_header
    jc .fail
    mov eax, [nfs_dev]
    call storage_flush
    jc .fail
.ok_pop:
    pop ebx
    pop edi
    pop esi
    pop edx
    pop ecx
    pop eax
.ok:
    clc
    ret
.full:
    mov dword [nfs_error], NOVAFS_ERR_FULL
    jmp .fail_pop
.fail:
    mov dword [nfs_error], NOVAFS_ERR_IO
.fail_pop:
    pop ebx
    pop edi
    pop esi
    pop edx
    pop ecx
    pop eax
    stc
    ret

; EAX=Blocknummer, EDI=Puffer. Wie novafs_write_block, sichert aber zuerst
; den bisherigen Inhalt im Transaction Log (sofern eine Aenderung laeuft).
novafs_write_block_logged:
    call novafs_journal_capture
    jc .fail
    jmp novafs_write_block
.fail:
    stc
    ret

; Rollt alle in [nfs_sb+NFS_SB_JOURNAL_BLOCK] gesicherten Pre-Images zurueck.
; Erwartet, dass [nfs_sb] den aktuellen (DIRTY) Superblock enthaelt. CF=1,
; wenn keine gueltige, zu dieser Transaktion passende Journal-Region
; gefunden wurde; dann wurde nichts veraendert.
novafs_journal_undo:
    pushad
    mov eax, [nfs_sb + NFS_SB_JOURNAL_BLOCK]
    cmp eax, [nfs_total_blocks]
    jae .invalid
    mov edi, [nfs_journal_header_page]
    call novafs_read_block
    jc .invalid
    cmp dword [edi + NFS_J_MAGIC], NOVAFS_JOURNAL_MAGIC
    jne .invalid
    cmp dword [edi + NFS_J_VERSION], 1
    jne .invalid
    push edx
    mov esi, edi
    mov ecx, NOVAFS_BLOCK_SIZE
    mov edx, NFS_J_CHECKSUM
    call novafs_verify
    pop edx
    jc .invalid
    mov eax, [edi + NFS_J_GENERATION]
    cmp eax, [nfs_sb + NFS_SB_GENERATION]
    jne .invalid
    mov eax, [edi + NFS_J_GENERATION + 4]
    cmp eax, [nfs_sb + NFS_SB_GENERATION + 4]
    jne .invalid
    mov ecx, [edi + NFS_J_COUNT]
    cmp ecx, NOVAFS_JOURNAL_SLOTS
    ja .invalid
    cmp ecx, 1
    jb .invalid
    mov [nfs_journal_undo_count], ecx
    mov esi, edi
    add esi, NFS_J_ENTRIES
    mov edi, nfs_journal_undo_entries
    mov ecx, NOVAFS_JOURNAL_SLOTS
    rep movsd
    mov dword [nfs_journal_undo_saved_ro], 0
    cmp dword [nfs_readonly], 0
    je .ro_saved
    mov dword [nfs_journal_undo_saved_ro], 1
.ro_saved:
    mov dword [nfs_readonly], 0
    mov ecx, [nfs_journal_undo_count]
.replay:
    test ecx, ecx
    jz .replay_done
    dec ecx
    mov eax, [nfs_sb + NFS_SB_JOURNAL_BLOCK]
    add eax, 1
    add eax, ecx
    mov edi, [nfs_journal_slot_page]
    call novafs_read_block
    jc .restore_fail
    mov eax, [nfs_journal_undo_entries + ecx * 4]
    mov edi, [nfs_journal_slot_page]
    call novafs_write_block
    jc .restore_fail
    jmp .replay
.replay_done:
    mov eax, [nfs_dev]
    call storage_flush
    jc .restore_fail
    mov dword [nfs_journal_count], 0
    call novafs_journal_write_header
    jc .restore_fail
    mov eax, [nfs_dev]
    call storage_flush
    jc .restore_fail
    cmp dword [nfs_journal_undo_saved_ro], 0
    je .leave_rw
    mov dword [nfs_readonly], 1
.leave_rw:
    popad
    clc
    ret
.restore_fail:
    cmp dword [nfs_journal_undo_saved_ro], 0
    je .invalid
    mov dword [nfs_readonly], 1
.invalid:
    popad
    stc
    ret

; Bricht die laufende Aenderung ab (z. B. weil die Operation selbst einen
; Fehler meldet, ohne dass der Kernel neu startet) und stellt per
; novafs_journal_undo exakt den Zustand vor ihrem Beginn wieder her. Anders
; als das alte Phase-1-Verhalten bleibt das Volume dabei beschreibbar statt
; fuer den Rest des Boots read-only zu werden. CF=1, wenn die
; Wiederherstellung selbst fehlschlaegt; dann bleibt state DIRTY und das
; Volume wird (vom Aufrufer) read-only belassen.
novafs_change_abort:
    pushad
    cmp dword [nfs_sb + NFS_SB_STATE], NOVAFS_STATE_DIRTY
    jne .nothing
    call novafs_journal_undo
    jc .failed
    mov eax, 1
    mov edi, [nfs_leaf_page]
    call novafs_read_block
    jc .failed
    call novafs_superblock_validate
    jc .failed
    cmp dword [edi + NFS_SB_STATE], NOVAFS_STATE_CLEAN
    jne .failed
    mov esi, edi
    mov edi, nfs_sb
    mov ecx, NOVAFS_BLOCK_SIZE / 4
    rep movsd
    mov eax, [nfs_sb + NFS_SB_TOTAL]
    mov [nfs_total_blocks], eax
    mov [nfs_limit_blocks], eax
    mov eax, [nfs_sb + NFS_SB_BITMAP_BLOCKS]
    mov [nfs_bitmap_blocks], eax
    ; Die im Speicher gehaltene Bitmap-Kopie kann durch die abgebrochene
    ; Operation veraendert worden sein (Allokationen/Freigaben); von der
    ; wiederhergestellten Platte neu laden.
    xor ebx, ebx
    mov eax, 0xFFFFFFFF
.bitmap_reload:
    cmp ebx, [nfs_bitmap_blocks]
    jae .bitmap_reloaded
    mov edi, [nfs_bitmap_pages + ebx * 4]
    push eax
    lea eax, [ebx + 2]
    call novafs_read_block
    pop eax
    jc .failed
    mov esi, edi
    mov ecx, NOVAFS_BLOCK_SIZE
    call crc32c_update
    inc ebx
    jmp .bitmap_reload
.bitmap_reloaded:
    not eax
    cmp eax, [nfs_sb + NFS_SB_BITMAP_CRC]
    jne .failed
    mov dword [nfs_bitmap_dirty], 0
    mov dword [nfs_txn_open], 0
    popad
    clc
    ret
.nothing:
    mov dword [nfs_txn_open], 0
    popad
    clc
    ret
.failed:
    mov dword [nfs_txn_open], 0
    popad
    stc
    ret

; ---------------------------------------------------------------------------
; CRC32C-Hilfen: ESI=Struktur, ECX=Laenge, EDX=Offset des 32-Byte-Felds.
; ---------------------------------------------------------------------------
novafs_crc_struct:
    pushad
    lea edi, [esi + edx]
    mov eax, [edi + 0]
    mov [nfs_crc_saved + 0], eax
    mov eax, [edi + 4]
    mov [nfs_crc_saved + 4], eax
    mov dword [edi + 0], 0
    mov dword [edi + 4], 0
    mov eax, 0xFFFFFFFF
    call crc32c_update
    not eax
    mov [esp + 28], eax
    mov eax, [nfs_crc_saved + 0]
    mov [edi + 0], eax
    mov eax, [nfs_crc_saved + 4]
    mov [edi + 4], eax
    popad
    ret

; Setzt das Pruefsummenfeld (Bytes 4..31 werden 0).
novafs_seal:
    pushad
    lea edi, [esi + edx]
    push edi
    xor eax, eax
    mov ecx, 8
    rep stosd
    pop edi
    mov ecx, [esp + 24]
    call novafs_crc_struct
    mov [edi], eax
    popad
    ret

; CF=1 wenn die Pruefsumme nicht stimmt.
novafs_verify:
    pushad
    lea edi, [esi + edx]
    mov ebx, 1
.zero:
    cmp dword [edi + ebx * 4], 0
    jne .invalid
    inc ebx
    cmp ebx, 8
    jb .zero
    call novafs_crc_struct
    cmp eax, [edi]
    jne .invalid
    popad
    clc
    ret
.invalid:
    popad
    stc
    ret

; ---------------------------------------------------------------------------
; Bitmap
; ---------------------------------------------------------------------------

; EAX=Block -> EDI=Seite, ECX=Bitoffset innerhalb der Seite
novafs_bitmap_locate:
    mov ecx, eax
    shr ecx, 15
    mov edi, [nfs_bitmap_pages + ecx * 4]
    mov ecx, eax
    and ecx, 0x7FFF
    ret

; EAX=Block. CF=1 wenn belegt.
novafs_block_used:
    push ecx
    push edi
    call novafs_bitmap_locate
    bt dword [edi], ecx
    pop edi
    pop ecx
    ret

; EAX=gewuenschter Block. CF=1 wenn belegt oder ausserhalb.
novafs_alloc_specific:
    pushad
    cmp eax, [nfs_total_blocks]
    jae .invalid
    call novafs_bitmap_locate
    bts dword [edi], ecx
    jc .invalid
    sub dword [nfs_sb + NFS_SB_AVAILABLE], 1
    sbb dword [nfs_sb + NFS_SB_AVAILABLE + 4], 0
    mov dword [nfs_bitmap_dirty], 1
    popad
    clc
    ret
.invalid:
    popad
    stc
    ret

; EAX=Startpunkt der Suche. EAX=Block, CF wenn das Volume voll ist.
novafs_alloc_block:
    pushad
    mov ebx, [nfs_total_blocks]
    cmp eax, ebx
    jb .hint_ok
    xor eax, eax
.hint_ok:
    mov [nfs_alloc_start], eax
    mov edx, eax
.scan:
    mov eax, edx
    call novafs_block_used
    jnc .found
    inc edx
    cmp edx, ebx
    jb .wrap_check
    xor edx, edx
.wrap_check:
    cmp edx, [nfs_alloc_start]
    jne .scan
    mov dword [nfs_error], NOVAFS_ERR_FULL
    popad
    stc
    ret
.found:
    mov eax, edx
    call novafs_alloc_specific
    jc .scan_next
    mov [esp + 28], edx
    popad
    clc
    ret
.scan_next:
    inc edx
    jmp .scan

; EAX=freizugebender Block (nur fuer Rueckabwicklung nicht geschriebener
; Allokationen derselben Operation).
novafs_free_block:
    pushad
    cmp eax, [nfs_total_blocks]
    jae .done
    call novafs_bitmap_locate
    btr dword [edi], ecx
    jnc .done
    add dword [nfs_sb + NFS_SB_AVAILABLE], 1
    adc dword [nfs_sb + NFS_SB_AVAILABLE + 4], 0
    mov dword [nfs_bitmap_dirty], 1
.done:
    popad
    ret

; ---------------------------------------------------------------------------
; Aenderungsprotokoll Phase 1
; ---------------------------------------------------------------------------
novafs_change_begin:
    cmp dword [nfs_mounted], 1
    jne .invalid
    cmp dword [nfs_readonly], 0
    jne .read_only
    mov dword [nfs_journal_count], 0
    mov dword [nfs_txn_open], 1
    cmp dword [nfs_sb + NFS_SB_STATE], NOVAFS_STATE_DIRTY
    je .ready
    mov dword [nfs_sb + NFS_SB_STATE], NOVAFS_STATE_DIRTY
    call novafs_superblock_write_primary
    jc .invalid
    mov eax, [nfs_dev]
    call storage_flush
    jc .invalid
.ready:
    clc
    ret
.read_only:
    mov dword [nfs_error], NOVAFS_ERR_READ_ONLY
.invalid:
    mov dword [nfs_txn_open], 0
    stc
    ret

novafs_change_commit:
    cmp dword [nfs_sb + NFS_SB_STATE], NOVAFS_STATE_DIRTY
    jne .invalid
    cmp dword [nfs_bitmap_dirty], 0
    je .bitmap_done
    xor ebx, ebx
    mov eax, 0xFFFFFFFF
.bitmap_block:
    cmp ebx, [nfs_bitmap_blocks]
    jae .bitmap_written
    mov edi, [nfs_bitmap_pages + ebx * 4]
    push eax
    lea eax, [ebx + 2]
    call novafs_write_block_logged
    pop eax
    jc .invalid
    mov esi, edi
    mov ecx, NOVAFS_BLOCK_SIZE
    call crc32c_update
    inc ebx
    jmp .bitmap_block
.bitmap_written:
    not eax
    mov [nfs_sb + NFS_SB_BITMAP_CRC], eax
    mov dword [nfs_bitmap_dirty], 0
.bitmap_done:
    ; Daten und Knoten muessen vor dem sauberen Superblock dauerhaft sein.
    mov eax, [nfs_dev]
    call storage_flush
    jc .invalid
    add dword [nfs_sb + NFS_SB_GENERATION], 1
    adc dword [nfs_sb + NFS_SB_GENERATION + 4], 0
    mov dword [nfs_sb + NFS_SB_STATE], NOVAFS_STATE_CLEAN
    call novafs_superblock_write_primary
    jc .invalid
    mov eax, [nfs_dev]
    call storage_flush
    jc .invalid
    ; Ab hier ist die Transaktion unumkehrbar abgeschlossen; das Journal
    ; wird nicht mehr benoetigt (der Backup-Superblock wird bewusst nicht
    ; journalisiert, siehe NPSPEC-NOVAFS-ONDISK-0001 §9).
    mov dword [nfs_txn_open], 0
    mov dword [nfs_journal_count], 0
    call novafs_journal_write_header
    jc .invalid
    mov eax, [nfs_total_blocks]
    dec eax
    push eax
    mov esi, nfs_sb
    mov ecx, NOVAFS_BLOCK_SIZE
    mov edx, NFS_SB_CHECKSUM
    call novafs_seal
    pop eax
    mov esi, nfs_sb
    mov edi, [nfs_io_page]
    mov ecx, NOVAFS_BLOCK_SIZE / 4
    rep movsd
    mov edi, [nfs_io_page]
    call novafs_write_block
    jc .invalid
    mov eax, [nfs_dev]
    call storage_flush
    jc .invalid
    clc
    ret
.invalid:
    mov dword [nfs_txn_open], 0
    stc
    ret

novafs_superblock_write_primary:
    mov eax, 1
    mov edi, nfs_sb
novafs_superblock_write_at:
    push eax
    mov esi, nfs_sb
    mov ecx, NOVAFS_BLOCK_SIZE
    mov edx, NFS_SB_CHECKSUM
    call novafs_seal
    pop eax
    ; nfs_sb liegt im Kernelabbild (< 8 MiB, identitaetsabgebildet); fuer DMA
    ; wird trotzdem eine PMM-Seite verwendet, weil das Abbild nicht seiten-
    ; ausgerichtet sein muss.
    mov esi, nfs_sb
    mov edi, [nfs_io_page]
    mov ecx, NOVAFS_BLOCK_SIZE / 4
    rep movsd
    mov edi, [nfs_io_page]
    jmp novafs_write_block_logged

; ---------------------------------------------------------------------------
; Superblock-Validierung (EDI=Puffer). CF bei ungueltigem Inhalt.
; ---------------------------------------------------------------------------
novafs_superblock_validate:
    pushad
    mov esi, edi
    push esi
    push edi
    mov edi, novafs_magic
    mov ecx, 2
    repe cmpsd
    pop edi
    pop esi
    jne .invalid
    cmp word [esi + NFS_SB_VMAJ], 1
    jne .invalid
    cmp dword [esi + NFS_SB_BLOCK_SIZE], NOVAFS_BLOCK_SIZE
    jne .invalid
    mov ecx, NOVAFS_BLOCK_SIZE
    mov edx, NFS_SB_CHECKSUM
    call novafs_verify
    jc .invalid
    ; total_blocks: 64 <= total <= Partitionsbloecke
    cmp dword [esi + NFS_SB_TOTAL + 4], 0
    jne .invalid
    mov ebx, [esi + NFS_SB_TOTAL]
    cmp ebx, 64
    jb .invalid
    cmp ebx, [nfs_part_blocks]
    ja .invalid
    ; filesystem_size = total * 4096
    mov eax, ebx
    shl eax, 12
    cmp [esi + NFS_SB_FS_SIZE], eax
    jne .invalid
    mov eax, ebx
    shr eax, 20
    cmp [esi + NFS_SB_FS_SIZE + 4], eax
    jne .invalid
    ; Backup im letzten Block
    lea eax, [ebx - 1]
    cmp [esi + NFS_SB_BACKUP_BLOCK], eax
    jne .invalid
    cmp dword [esi + NFS_SB_BACKUP_BLOCK + 4], 0
    jne .invalid
    ; Bitmap
    lea eax, [ebx + 32767]
    shr eax, 15
    cmp [esi + NFS_SB_BITMAP_BLOCKS], eax
    jne .invalid
    cmp dword [esi + NFS_SB_BITMAP_BLOCKS + 4], 0
    jne .invalid
    cmp eax, NOVAFS_MAX_BITMAP_BLOCKS
    ja .invalid
    ; Transaction-Log-Region direkt nach der Bitmap (NPSPEC-NOVAFS-ONDISK-0001 §9)
    lea ecx, [eax + 2]
    cmp dword [esi + NFS_SB_JOURNAL_BLOCK], ecx
    jne .invalid
    cmp dword [esi + NFS_SB_JOURNAL_BLOCK + 4], 0
    jne .invalid
    cmp dword [esi + NFS_SB_FREE_SPACE], 2
    jne .invalid
    cmp dword [esi + NFS_SB_FREE_SPACE + 4], 0
    jne .invalid
    lea edx, [ecx + NOVAFS_JOURNAL_BLOCKS]    ; erster freier Bereich
    ; Baumwurzeln im Datenbereich
    mov ecx, NFS_SB_OBJECT_TREE
.root:
    cmp dword [esi + ecx + 4], 0
    jne .invalid
    mov eax, [esi + ecx]
    cmp eax, edx
    jb .invalid
    lea edi, [ebx - 1]
    cmp eax, edi
    jae .invalid
    add ecx, 8
    cmp ecx, NFS_SB_EXTENT_TREE
    jbe .root
    cmp dword [esi + NFS_SB_NEXT_OBJECT + 4], 0
    jne .invalid
    cmp dword [esi + NFS_SB_NEXT_OBJECT], NOVAFS_FIRST_DYNAMIC_ID
    jb .invalid
    popad
    clc
    ret
.invalid:
    popad
    stc
    ret

; ---------------------------------------------------------------------------
; Baumknoten
; ---------------------------------------------------------------------------

; EAX=Block, EDI=Puffer, EBX=Baumdeskriptor, EDX=erwartetes Level oder -1.
novafs_node_load:
    pushad
    mov ecx, [nfs_bitmap_blocks]
    add ecx, 2
    cmp eax, ecx
    jb .corrupt
    mov ecx, [nfs_total_blocks]
    dec ecx
    cmp eax, ecx
    jae .corrupt
    call novafs_read_block
    jc .invalid
    cmp dword [edi + NFS_N_MAGIC], NOVAFS_NODE_MAGIC
    jne .corrupt
    mov esi, edi
    push edx
    mov ecx, NOVAFS_BLOCK_SIZE
    mov edx, NFS_N_CHECKSUM
    call novafs_verify
    pop edx
    jc .corrupt
    mov ecx, [ebx + NFS_TD_ID]
    cmp [edi + NFS_N_TREE], ecx
    jne .corrupt
    cmp dword [edi + NFS_N_TREE + 4], 0
    jne .corrupt
    cmp [edi + NFS_N_BLOCK], eax
    jne .corrupt
    cmp dword [edi + NFS_N_BLOCK + 4], 0
    jne .corrupt
    movzx ecx, word [edi + NFS_N_LEVEL]
    cmp ecx, 1
    ja .corrupt
    cmp edx, 0xFFFFFFFF
    je .level_ok
    cmp ecx, edx
    jne .corrupt
.level_ok:
    mov eax, [ebx + NFS_TD_KEY]
    cmp ax, [edi + NFS_N_KEY_SIZE]
    jne .corrupt
    test ecx, ecx
    jnz .inner_geometry
    mov eax, [ebx + NFS_TD_ITEM]
    cmp ax, [edi + NFS_N_ITEM_SIZE]
    jne .corrupt
    mov eax, [ebx + NFS_TD_MAX]
    jmp .max_check
.inner_geometry:
    cmp word [edi + NFS_N_ITEM_SIZE], NOVAFS_INNER_ITEM
    jne .corrupt
    mov eax, NOVAFS_INNER_MAX
.max_check:
    cmp ax, [edi + NFS_N_MAX_ITEMS]
    jne .corrupt
    cmp ax, [edi + NFS_N_COUNT]
    jb .corrupt
    popad
    clc
    ret
.corrupt:
    mov dword [nfs_error], NOVAFS_ERR_CORRUPT
.invalid:
    popad
    stc
    ret

; EDI=Knoten. Setzt die Generation der laufenden Aenderung, versiegelt und
; schreibt den Knoten an seine eigene Blocknummer.
novafs_node_store:
    pushad
    mov eax, [nfs_sb + NFS_SB_GENERATION]
    mov edx, [nfs_sb + NFS_SB_GENERATION + 4]
    add eax, 1
    adc edx, 0
    mov [edi + NFS_N_GENERATION], eax
    mov [edi + NFS_N_GENERATION + 4], edx
    mov esi, edi
    mov ecx, NOVAFS_BLOCK_SIZE
    mov edx, NFS_N_CHECKSUM
    call novafs_seal
    mov eax, [edi + NFS_N_BLOCK]
    call novafs_write_block_logged
    popad
    ret

; EDI=Puffer, EBX=Baum, EAX=Block, EDX=Level, ECX=Parent-Block.
novafs_node_init:
    pushad
    push eax
    push edi
    xor eax, eax
    mov ecx, NOVAFS_BLOCK_SIZE / 4
    rep stosd
    pop edi
    pop eax
    mov ecx, [esp + 24]
    mov dword [edi + NFS_N_MAGIC], NOVAFS_NODE_MAGIC
    mov [edi + NFS_N_LEVEL], dx
    mov esi, [ebx + NFS_TD_ID]
    mov [edi + NFS_N_TREE], esi
    mov [edi + NFS_N_BLOCK], eax
    mov [edi + NFS_N_PARENT], ecx
    mov eax, [ebx + NFS_TD_KEY]
    mov [edi + NFS_N_KEY_SIZE], ax
    test edx, edx
    jnz .inner
    mov eax, [ebx + NFS_TD_ITEM]
    mov [edi + NFS_N_ITEM_SIZE], ax
    mov eax, [ebx + NFS_TD_MAX]
    mov [edi + NFS_N_MAX_ITEMS], ax
    popad
    ret
.inner:
    mov word [edi + NFS_N_ITEM_SIZE], NOVAFS_INNER_ITEM
    mov word [edi + NFS_N_MAX_ITEMS], NOVAFS_INNER_MAX
    popad
    ret

; ---------------------------------------------------------------------------
; Schluessel (nfs_key: k1 lo/hi, k2 lo/hi)
; ---------------------------------------------------------------------------

; ESI=Blatt-Item, EBX=Baum. EAX = 0 gleich, 1 Item groesser, -1 Item kleiner.
novafs_compare_item:
    mov eax, [esi + 4]
    cmp eax, [nfs_key + 4]
    ja .greater
    jb .less
    mov eax, [esi + 0]
    cmp eax, [nfs_key + 0]
    ja .greater
    jb .less
    cmp dword [ebx + NFS_TD_KEY], 16
    jne .short_key
    mov eax, [esi + 12]
    cmp eax, [nfs_key + 12]
    ja .greater
    jb .less
    mov eax, [esi + 8]
    cmp eax, [nfs_key + 8]
    ja .greater
    jb .less
    xor eax, eax
    ret
.short_key:
    mov eax, [nfs_key + 8]
    or eax, [nfs_key + 12]
    jnz .less
    xor eax, eax
    ret
.greater:
    mov eax, 1
    ret
.less:
    mov eax, 0xFFFFFFFF
    ret

; ESI=inneres Item (immer 16-Byte-Schluessel).
novafs_compare_inner:
    mov eax, [esi + 4]
    cmp eax, [nfs_key + 4]
    ja .greater
    jb .less
    mov eax, [esi + 0]
    cmp eax, [nfs_key + 0]
    ja .greater
    jb .less
    mov eax, [esi + 12]
    cmp eax, [nfs_key + 12]
    ja .greater
    jb .less
    mov eax, [esi + 8]
    cmp eax, [nfs_key + 8]
    ja .greater
    jb .less
    xor eax, eax
    ret
.greater:
    mov eax, 1
    ret
.less:
    mov eax, 0xFFFFFFFF
    ret

; ESI=Item, EBX=Baum -> nfs_key
novafs_key_from_item:
    mov eax, [esi + 0]
    mov [nfs_key + 0], eax
    mov eax, [esi + 4]
    mov [nfs_key + 4], eax
    xor eax, eax
    mov [nfs_key + 8], eax
    mov [nfs_key + 12], eax
    cmp dword [ebx + NFS_TD_KEY], 16
    jne .done
    mov eax, [esi + 8]
    mov [nfs_key + 8], eax
    mov eax, [esi + 12]
    mov [nfs_key + 12], eax
.done:
    ret

; EAX=k1, EDX=k2 (High-DWORDs 0)
novafs_key_set:
    mov [nfs_key + 0], eax
    mov dword [nfs_key + 4], 0
    mov [nfs_key + 8], edx
    mov dword [nfs_key + 12], 0
    ret

; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
; §125 – Pfadtiefe → Pufferadresse
; ECX=Pfadtiefe (0=root, 1=inner1, 2=inner2, >=3=leaf). EDI=Puffer. ECX zerstoert.
novafs_level_page:
    cmp ecx, 0
    jne .l1
    mov edi, [nfs_root_page]
    ret
.l1:
    cmp ecx, 1
    jne .l2
    mov edi, [nfs_inner1_page]
    ret
.l2:
    cmp ecx, 2
    jne .lleaf
    mov edi, [nfs_inner2_page]
    ret
.lleaf:
    mov edi, [nfs_leaf_page]
    ret

; Cursor (ein globaler Cursor; Blatt in nfs_leaf_page, Wurzel in nfs_root_page)
; ---------------------------------------------------------------------------

; EBX=Baum. Positioniert auf das erste Item >= nfs_key. CF bei Fehler.
; §125: generalisiert fuer Baumhoehe bis NFS_MAX_HEIGHT.
novafs_cursor_seek:
    pushad
    mov [nfs_cur_tree], ebx
    ; Wurzel laden
    mov ecx, [ebx + NFS_TD_ROOT]
    mov eax, [nfs_sb + ecx]
    mov [nfs_path_blocks + 0], eax
    mov edi, [nfs_root_page]
    mov edx, 0xFFFFFFFF
    call novafs_node_load
    jc .invalid
    mov edi, [nfs_root_page]
    ; Eltern-Prüfung: Wurzel muss parentlos sein
    mov eax, [edi + NFS_N_PARENT]
    or eax, [edi + NFS_N_PARENT + 4]
    jnz .corrupt
    ; Ist Wurzel schon ein Blatt?
    cmp word [edi + NFS_N_LEVEL], 0
    je .root_is_leaf
    ; Abstieg durch innere Knoten; EBP = aktuelle Pfadtiefe (0 = Wurzel)
    xor ebp, ebp
.inner_descent:
    ; EDI = Puffer der inneren Knotenebene EBP, EBX = Baum (unveraendert)
    movzx ecx, word [edi + NFS_N_COUNT]
    test ecx, ecx
    jz .corrupt
    ; Kindauswahl: groessten Separator-Index suchen, dessen Schluessel <= nfs_key
    ; nfs_path_indices[ebp] = gewaehlter Kind-Index (initial 0)
    mov dword [nfs_path_indices + ebp*4], 0
    mov edx, 1
.sep_scan:
    cmp edx, ecx
    jae .sep_done
    imul esi, edx, NOVAFS_INNER_ITEM
    lea esi, [edi + NOVAFS_NODE_HEADER + esi]
    call novafs_compare_inner       ; zerstoert nur EAX
    cmp eax, 0xFFFFFFFF
    jne .sep_done
    mov [nfs_path_indices + ebp*4], edx
    inc edx
    jmp .sep_scan
.sep_done:
    mov edx, [nfs_path_indices + ebp*4]
    ; Kind-Block aus dem inneren Knoten holen
    imul ecx, edx, NOVAFS_INNER_ITEM
    cmp dword [edi + NOVAFS_NODE_HEADER + ecx + 20], 0
    jne .corrupt
    mov eax, [edi + NOVAFS_NODE_HEADER + ecx + 16]
    ; Pfadtiefe erhoehen und Kind laden
    inc ebp
    mov [nfs_path_blocks + ebp*4], eax
    mov ecx, ebp
    call novafs_level_page          ; EDI = Puffer fuer Tiefe EBP (zerstoert ECX)
    mov edx, 0xFFFFFFFF
    call novafs_node_load
    jc .invalid
    ; Puffer neu setzen (novafs_node_load benutzt pushad, EDI bleibt)
    mov ecx, ebp
    call novafs_level_page
    ; Ist dieses Kind ein Blatt?
    cmp word [edi + NFS_N_LEVEL], 0
    je .leaf_loaded
    ; Noch ein innerer Knoten – weiter absteigen
    cmp ebp, NFS_MAX_HEIGHT - 2
    jae .corrupt
    jmp .inner_descent
.leaf_loaded:
    ; EBP = Pfadtiefe des Eltern-inneren-Knotens; Blatt steht in nfs_level_page(EBP)
    ; Für Rueckwaertskompatibilitaet: nfs_cur_height = EBP + 1, nfs_cur_child = path_indices[EBP-1]
    mov [nfs_path_depth], ebp
    mov eax, ebp
    inc eax
    mov [nfs_cur_height], eax
    mov eax, [nfs_path_indices + ebp*4 - 4]
    mov [nfs_cur_child], eax
    jmp .position
.root_is_leaf:
    ; Hoehe 1: Wurzel ist Blatt; kopiere in leaf_page fuer einheitlichen Zugriff
    mov dword [nfs_cur_height], 1
    mov dword [nfs_path_depth], 0
    mov dword [nfs_cur_child], 0
    mov dword [nfs_path_indices + 0], 0
    mov esi, edi
    mov edi, [nfs_leaf_page]
    mov ecx, NOVAFS_BLOCK_SIZE / 4
    rep movsd
.position:
    mov edi, [nfs_leaf_page]
    movzx ecx, word [edi + NFS_N_COUNT]
    mov edx, [ebx + NFS_TD_ITEM]
    xor ebp, ebp
    lea esi, [edi + NOVAFS_NODE_HEADER]
.item_scan:
    cmp ebp, ecx
    jae .positioned
    call novafs_compare_item
    cmp eax, 0xFFFFFFFF
    jne .positioned
    add esi, edx
    inc ebp
    jmp .item_scan
.positioned:
    mov [nfs_cur_index], ebp
    popad
    clc
    ret
.corrupt:
    mov dword [nfs_error], NOVAFS_ERR_CORRUPT
.invalid:
    popad
    stc
    ret

; §125 – Steigt den Pfad-Stack hoch und laedt das naechste Geschwister-Blatt.
; CF wenn kein naechstes Blatt vorhanden. Verwendet EBX=nfs_cur_tree.
novafs_cursor_advance_leaf:
    pushad
    mov ebx, [nfs_cur_tree]
    ; Von der Tiefe direkt ueber dem Blatt aufwaerts suchen
    mov ebp, [nfs_path_depth]
    test ebp, ebp
    jz .end                         ; Hoehe 1: kein naechstes Blatt
.climb:
    dec ebp
    ; Innere Knoten bei EBP laden (liegen noch im Puffer)
    mov ecx, ebp
    call novafs_level_page          ; EDI = innerer Puffer
    ; Versuche naechstes Kind
    mov edx, [nfs_path_indices + ebp*4]
    inc edx
    movzx ecx, word [edi + NFS_N_COUNT]
    cmp edx, ecx
    ja .climb                       ; kein naechstes Kind, eine Ebene hoeher
    ; Naechstes Kind gefunden bei EBP, Index EDX
    mov [nfs_path_indices + ebp*4], edx
    ; Fuer jede Ebene unterhalb EBP: lade erstes Kind
    ; Lade Kind-Block
    imul ecx, edx, NOVAFS_INNER_ITEM
    cmp dword [edi + NOVAFS_NODE_HEADER + ecx + 20], 0
    jne .corrupt
    mov eax, [edi + NOVAFS_NODE_HEADER + ecx + 16]
    inc ebp
.load_down:
    ; EAX = Block zu laden, EBP = Tiefe
    mov [nfs_path_blocks + ebp*4], eax
    mov ecx, ebp
    call novafs_level_page
    mov edx, 0xFFFFFFFF
    call novafs_node_load
    jc .invalid
    mov ecx, ebp
    call novafs_level_page
    ; Ist dieses ein Blatt?
    cmp word [edi + NFS_N_LEVEL], 0
    je .loaded
    ; Innerer Knoten: wähle erstes Kind (Index 0)
    mov dword [nfs_path_indices + ebp*4], 0
    cmp dword [edi + NOVAFS_NODE_HEADER + 20], 0
    jne .corrupt
    mov eax, [edi + NOVAFS_NODE_HEADER + 16]
    inc ebp
    jmp .load_down
.loaded:
    ; Blatt geladen; nfs_path_depth, nfs_cur_height, nfs_cur_child aktualisieren
    mov [nfs_path_depth], ebp
    mov eax, ebp
    inc eax
    mov [nfs_cur_height], eax
    cmp ebp, 0
    je .no_child_update
    mov eax, [nfs_path_indices + ebp*4 - 4]
    mov [nfs_cur_child], eax
.no_child_update:
    popad
    clc
    ret
.corrupt:
    mov dword [nfs_error], NOVAFS_ERR_CORRUPT
.invalid:
.end:
    popad
    stc
    ret

; §125 (Rueckwaertskompatibilitaet) – wird intern nicht mehr aufgerufen,
; bleibt als Alias fuer externen Code.
novafs_cursor_load_leaf:
    call novafs_cursor_advance_leaf
    ret

; ESI=aktuelles Item. Wechselt bei Bedarf ins naechste Blatt; CF am Ende.
; §125: nutzt novafs_cursor_advance_leaf statt hardcodierter Hoehe-2-Logik.
novafs_cursor_item:
    push eax
    push ecx
    push edi
.again:
    mov edi, [nfs_leaf_page]
    movzx ecx, word [edi + NFS_N_COUNT]
    mov eax, [nfs_cur_index]
    cmp eax, ecx
    jb .have
    ; Blatt erschoepft: naechstes Geschwister-Blatt laden
    call novafs_cursor_advance_leaf
    jc .end
    mov dword [nfs_cur_index], 0
    jmp .again
.have:
    mov ecx, [nfs_cur_tree]
    imul eax, [ecx + NFS_TD_ITEM]
    lea esi, [edi + NOVAFS_NODE_HEADER + eax]
    pop edi
    pop ecx
    pop eax
    clc
    ret
.end:
    pop edi
    pop ecx
    pop eax
    stc
    ret

; ---------------------------------------------------------------------------
; Einfuegen mit Blatt-Split (Baumhoehe <= 2)
; ---------------------------------------------------------------------------

; EDI=Knoten, EAX=Index, ESI=Quelle, ECX=Itemgroesse. Item an Index einfuegen.
novafs_node_insert_at:
    pushad
    movzx edx, word [edi + NFS_N_COUNT]
    mov ebx, edx
    sub ebx, eax                     ; zu verschiebende Items
    imul ebx, ecx
    imul eax, ecx
    lea ebp, [edi + NOVAFS_NODE_HEADER + eax]
    test ebx, ebx
    jz .copy
    push esi
    push edi
    push ecx
    lea esi, [ebp + ebx - 1]
    lea edi, [ebp + ebx - 1]
    add edi, ecx
    mov ecx, ebx
    std
    rep movsb
    cld
    pop ecx
    pop edi
    pop esi
.copy:
    push edi
    mov edi, ebp
    rep movsb
    pop edi
    inc word [edi + NFS_N_COUNT]
    popad
    ret

; EBX=Baum, ESI=Item. CF bei Fehler (nfs_error).
novafs_tree_insert:
    pushad
    mov [nfs_ins_item], esi
    mov dword [nfs_ins_right], 0
    mov dword [nfs_ins_new_root], 0
    call novafs_key_from_item
    call novafs_cursor_seek
    jc .invalid
    mov edi, [nfs_leaf_page]
    movzx ecx, word [edi + NFS_N_COUNT]
    cmp ecx, [ebx + NFS_TD_MAX]
    jae .split
    mov eax, [nfs_cur_index]
    mov ecx, [ebx + NFS_TD_ITEM]
    mov esi, [nfs_ins_item]
    call novafs_node_insert_at
    call novafs_node_store
    jc .invalid
    popad
    clc
    ret

.split:
    mov [nfs_ins_count], ecx
    cmp dword [nfs_cur_height], 2
    jne .split_alloc
    mov esi, [nfs_root_page]
    cmp word [esi + NFS_N_COUNT], NOVAFS_INNER_MAX
    jae .tree_full
.split_alloc:
    mov eax, [edi + NFS_N_BLOCK]
    mov [nfs_ins_leaf_block], eax
    call novafs_alloc_block
    jc .invalid
    mov [nfs_ins_right], eax
    cmp dword [nfs_cur_height], 1
    jne .parent_known
    mov eax, [nfs_ins_leaf_block]
    call novafs_alloc_block
    jc .rollback
    mov [nfs_ins_new_root], eax
    mov [nfs_ins_parent], eax
    ; ehemaliges Wurzelblatt erhaelt den neuen Parent
    mov edi, [nfs_leaf_page]
    mov [edi + NFS_N_PARENT], eax
    jmp .build_right
.parent_known:
    mov esi, [nfs_root_page]
    mov eax, [esi + NFS_N_BLOCK]
    mov [nfs_ins_parent], eax
.build_right:
    mov edi, [nfs_aux_page]
    mov eax, [nfs_ins_right]
    xor edx, edx
    mov ecx, [nfs_ins_parent]
    call novafs_node_init

    ; left = (count + 1) / 2
    mov eax, [nfs_ins_count]
    inc eax
    shr eax, 1
    mov [nfs_ins_left], eax
    mov edx, [ebx + NFS_TD_ITEM]
    mov eax, [nfs_cur_index]
    cmp eax, [nfs_ins_left]
    jae .into_right
    ; Neues Item links: Items [left-1, count) nach rechts verschieben.
    mov eax, [nfs_ins_left]
    dec eax
    call novafs_split_move
    mov edi, [nfs_leaf_page]
    mov eax, [nfs_cur_index]
    mov esi, [nfs_ins_item]
    mov ecx, edx
    call novafs_node_insert_at
    jmp .split_store
.into_right:
    mov eax, [nfs_ins_left]
    call novafs_split_move
    mov edi, [nfs_aux_page]
    mov eax, [nfs_cur_index]
    sub eax, [nfs_ins_left]
    mov esi, [nfs_ins_item]
    mov ecx, edx
    call novafs_node_insert_at
.split_store:
    mov edi, [nfs_aux_page]
    call novafs_node_store
    jc .invalid
    mov edi, [nfs_leaf_page]
    call novafs_node_store
    jc .invalid
    cmp dword [nfs_cur_height], 1
    jne .update_parent

    ; Neue innere Wurzel mit zwei Kindern
    mov edi, [nfs_root_page]
    mov eax, [nfs_ins_new_root]
    mov edx, 1
    xor ecx, ecx
    call novafs_node_init
    mov esi, [nfs_leaf_page]
    lea esi, [esi + NOVAFS_NODE_HEADER]
    lea edx, [edi + NOVAFS_NODE_HEADER]
    mov eax, [nfs_ins_leaf_block]
    call novafs_inner_entry_from_item
    mov esi, [nfs_aux_page]
    lea esi, [esi + NOVAFS_NODE_HEADER]
    lea edx, [edi + NOVAFS_NODE_HEADER + NOVAFS_INNER_ITEM]
    mov eax, [nfs_ins_right]
    call novafs_inner_entry_from_item
    mov word [edi + NFS_N_COUNT], 2
    call novafs_node_store
    jc .invalid
    mov ecx, [ebx + NFS_TD_ROOT]
    mov eax, [nfs_ins_new_root]
    mov [nfs_sb + ecx], eax
    mov dword [nfs_sb + ecx + 4], 0
    popad
    clc
    ret

.update_parent:
    ; §125 – Split-Propagation den Pfad-Stack aufwaerts.
    ; nfs_aux_page enthaelt das rechte Kind (Blatt oder innerer Knoten).
    ; Schleife: EBP = aktuelle Eltertiefe (beginnt bei nfs_path_depth - 1)
    mov ebp, [nfs_path_depth]
    dec ebp                     ; Elterknoten des gerade gespaltenen Kindes
.split_propagate:
    ; Innerer Knoten bei Tiefe EBP in seinen Puffer laden
    mov ecx, ebp
    call novafs_level_page      ; EDI = innerer Knoten-Puffer
    ; Separator-Eintrag fuer das rechte Kind erstellen
    mov esi, [nfs_aux_page]
    lea esi, [esi + NOVAFS_NODE_HEADER]
    mov edx, nfs_inner_temp
    mov eax, [nfs_ins_right]
    call novafs_inner_entry_from_item
    ; Einfuegeposition: nach dem Kind-Index dieser Ebene
    mov eax, [nfs_path_indices + ebp*4]
    inc eax
    movzx ecx, word [edi + NFS_N_COUNT]
    cmp ecx, NOVAFS_INNER_MAX
    jae .split_inner            ; innerer Knoten auch voll
    ; Platz vorhanden: Eintrag einfuegen und speichern
    mov esi, nfs_inner_temp
    mov ecx, NOVAFS_INNER_ITEM
    call novafs_node_insert_at
    call novafs_node_store
    jc .invalid
    popad
    clc
    ret

.split_inner:
    ; Innerer Knoten voll: aufteilen. Verwende nfs_aux_page als rechten Knoten.
    ; Vorher: aktueller Rechts-Separator in nfs_inner_temp.
    ; Wir brauchen einen neuen Block fuer den rechten inneren Knoten.
    ; Speichere Block-Nummer des linken Knotens
    mov eax, [edi + NFS_N_BLOCK]
    push eax                    ; linker Block auf Stack
    call novafs_alloc_block
    jc .rollback_inner
    mov [nfs_ins_right], eax    ; neuer rechter Block
    ; Ist das die Wurzel (EBP==0)?
    test ebp, ebp
    jnz .inner_parent_known
    ; Neue Wurzel allozieren
    call novafs_alloc_block
    jc .rollback_inner
    mov [nfs_ins_new_root], eax
    mov [nfs_ins_parent], eax
    mov [edi + NFS_N_PARENT], eax
    jmp .inner_build_right
.inner_parent_known:
    dec ebp
    mov ecx, ebp
    call novafs_level_page
    mov eax, [edi + NFS_N_BLOCK]
    mov [nfs_ins_parent], eax
    inc ebp
    mov ecx, ebp
    call novafs_level_page
.inner_build_right:
    ; Rechten inneren Knoten im aux_page aufbauen
    push edi                    ; linken Knoten-Puffer retten
    mov edi, [nfs_aux_page]
    mov eax, [nfs_ins_right]
    mov edx, 1
    mov ecx, [nfs_ins_parent]
    call novafs_node_init
    pop edi
    ; Split: linke Haelfte behaelt (count+1)/2 Eintraege
    movzx eax, word [edi + NFS_N_COUNT]
    inc eax
    shr eax, 1
    mov [nfs_ins_left], eax
    ; Pruefen, ob neuer Separator vor oder nach Split-Punkt
    mov edx, [nfs_path_indices + ebp*4]
    inc edx                     ; Einfuege-Index
    cmp edx, [nfs_ins_left]
    jae .inner_into_right
    ; Separator geht links: Items [left-1, count) nach rechts schieben
    mov eax, [nfs_ins_left]
    dec eax
    mov edx, NOVAFS_INNER_ITEM
    call novafs_split_move      ; verschiebt Items vom linken in aux_page
    mov edi, [nfs_path_blocks + ebp*4]   ; linker Block
    ; insert into left inner node
    mov ecx, ebp
    call novafs_level_page
    mov eax, [nfs_path_indices + ebp*4]
    inc eax
    mov esi, nfs_inner_temp
    mov ecx, NOVAFS_INNER_ITEM
    call novafs_node_insert_at
    jmp .inner_split_store
.inner_into_right:
    mov eax, [nfs_ins_left]
    mov edx, NOVAFS_INNER_ITEM
    call novafs_split_move
    mov edi, [nfs_aux_page]
    mov eax, [nfs_path_indices + ebp*4]
    sub eax, [nfs_ins_left]
    inc eax
    mov esi, nfs_inner_temp
    mov ecx, NOVAFS_INNER_ITEM
    call novafs_node_insert_at
.inner_split_store:
    ; Beide Haelften schreiben
    mov edi, [nfs_aux_page]
    call novafs_node_store
    jc .invalid
    mov ecx, ebp
    call novafs_level_page
    call novafs_node_store
    jc .invalid
    ; Neue Wurzel?
    test ebp, ebp
    jnz .inner_parent_propagate
    ; Neue innere Wurzel erzeugen (gleich wie beim Blatt-Split fuer Hoehe 1)
    pop ecx                     ; linker Block (oben auf Stack gepusht)
    mov edi, [nfs_root_page]
    mov eax, [nfs_ins_new_root]
    mov edx, [edi + NFS_N_LEVEL]
    inc edx
    xor ecx, ecx
    call novafs_node_init
    mov ecx, ebp
    call novafs_level_page
    lea esi, [edi + NOVAFS_NODE_HEADER]
    lea edx, [edi + NOVAFS_NODE_HEADER]       ; WRONG - edi is root now
    ; Rebuild: root_page is now the new root
    ; nfs_level_page(0) = root_page already loaded with new root init
    ; Recalculate: left child = old left inner node (path_blocks[0])
    ; right child = nfs_ins_right
    mov edi, [nfs_root_page]
    mov eax, [nfs_path_blocks + 0]            ; alter linker innerer Block
    ; Wir brauchen das erste Item des linken Knotens als Separator
    ; Lade linken Knoten kurz in inner1_page
    push edi
    mov edi, [nfs_inner1_page]
    mov edx, 0xFFFFFFFF
    call novafs_node_load
    lea esi, [edi + NOVAFS_NODE_HEADER]
    pop edi
    lea edx, [edi + NOVAFS_NODE_HEADER]
    call novafs_inner_entry_from_item
    ; rechtes Kind
    mov edi, [nfs_aux_page]
    lea esi, [edi + NOVAFS_NODE_HEADER]
    mov edi, [nfs_root_page]
    lea edx, [edi + NOVAFS_NODE_HEADER + NOVAFS_INNER_ITEM]
    mov eax, [nfs_ins_right]
    call novafs_inner_entry_from_item
    mov word [edi + NFS_N_COUNT], 2
    call novafs_node_store
    jc .invalid
    mov ecx, [ebx + NFS_TD_ROOT]
    mov eax, [nfs_ins_new_root]
    mov [nfs_sb + ecx], eax
    mov dword [nfs_sb + ecx + 4], 0
    popad
    clc
    ret

.inner_parent_propagate:
    ; Separator fuer den neuen rechten inneren Knoten berechnen und Stack hoeher
    mov edi, [nfs_aux_page]
    lea esi, [edi + NOVAFS_NODE_HEADER]
    mov edx, nfs_inner_temp
    mov eax, [nfs_ins_right]
    call novafs_inner_entry_from_item
    dec ebp
    jmp .split_propagate

.rollback_inner:
    pop eax                     ; linker Block (cleanup)
.tree_full:
    mov dword [nfs_error], NOVAFS_ERR_TREE_FULL
    popad
    stc
    ret
.rollback:
    mov eax, [nfs_ins_right]
    call novafs_free_block
.invalid:
    popad
    stc
    ret

; EAX=erster zu verschiebender Index im linken Blatt, EDX=Itemgroesse.
; Verschiebt Items [EAX, count) an den Anfang des rechten Knotens.
novafs_split_move:
    pushad
    mov ebx, eax
    mov edi, [nfs_leaf_page]
    movzx ecx, word [edi + NFS_N_COUNT]
    sub ecx, ebx
    mov [nfs_ins_moved], ecx
    imul ecx, edx
    imul ebx, edx
    lea esi, [edi + NOVAFS_NODE_HEADER + ebx]
    push esi
    push ecx
    mov edi, [nfs_aux_page]
    lea edi, [edi + NOVAFS_NODE_HEADER]
    rep movsb
    pop ecx
    pop edi
    ; freigewordene Bytes im linken Blatt nullen
    xor eax, eax
    rep stosb
    mov edi, [nfs_leaf_page]
    mov eax, [nfs_ins_moved]
    sub [edi + NFS_N_COUNT], ax
    mov edi, [nfs_aux_page]
    mov [edi + NFS_N_COUNT], ax
    popad
    ret

; ESI=Blatt-Item, EBX=Baum, EDX=Ziel (24 Byte), EAX=Kindblock.
novafs_inner_entry_from_item:
    push ecx
    mov ecx, [esi + 0]
    mov [edx + 0], ecx
    mov ecx, [esi + 4]
    mov [edx + 4], ecx
    xor ecx, ecx
    mov [edx + 8], ecx
    mov [edx + 12], ecx
    cmp dword [ebx + NFS_TD_KEY], 16
    jne .child
    mov ecx, [esi + 8]
    mov [edx + 8], ecx
    mov ecx, [esi + 12]
    mov [edx + 12], ecx
.child:
    mov [edx + 16], eax
    mov dword [edx + 20], 0
    pop ecx
    ret

; ---------------------------------------------------------------------------
; Objekte
; ---------------------------------------------------------------------------

; EAX=ObjectID. Kopiert den Datensatz nach nfs_object; ESI zeigt danach auf
; das Item im Blatt (gueltig bis zum naechsten Baumzugriff). CF wenn fehlend.
novafs_object_find:
    push eax
    push edx
    push ebx
    xor edx, edx
    call novafs_key_set
    mov ebx, nfs_tree_object
    call novafs_cursor_seek
    jc .invalid
    call novafs_cursor_item
    jc .missing
    call novafs_compare_item
    test eax, eax
    jnz .missing
    push ecx
    push edx
    mov ecx, NFS_O_ITEM
    mov edx, NFS_O_CHECKSUM
    call novafs_verify
    pop edx
    pop ecx
    jc .corrupt
    push esi
    push edi
    push ecx
    mov edi, nfs_object
    mov ecx, NFS_O_ITEM / 4
    rep movsd
    pop ecx
    pop edi
    pop esi
    pop ebx
    pop edx
    pop eax
    clc
    ret
.corrupt:
    mov dword [nfs_error], NOVAFS_ERR_CORRUPT
    jmp .invalid
.missing:
    mov dword [nfs_error], NOVAFS_ERR_NOT_FOUND
.invalid:
    pop ebx
    pop edx
    pop eax
    stc
    ret

; Schreibt nfs_object an seine Position im Object Tree zurueck.
novafs_object_store:
    pushad
    mov esi, nfs_object
    mov edi, nfs_object_saved
    mov ecx, NFS_O_ITEM / 4
    rep movsd
    mov eax, [nfs_object + NFS_O_ID]
    call novafs_object_find
    jc .invalid
    mov edi, esi
    mov esi, nfs_object_saved
    mov ecx, NFS_O_ITEM / 4
    push edi
    rep movsd
    pop edi
    mov eax, [nfs_sb + NFS_SB_GENERATION]
    mov edx, [nfs_sb + NFS_SB_GENERATION + 4]
    add eax, 1
    adc edx, 0
    mov [edi + NFS_O_GENERATION], eax
    mov [edi + NFS_O_GENERATION + 4], edx
    mov esi, edi
    mov ecx, NFS_O_ITEM
    mov edx, NFS_O_CHECKSUM
    call novafs_seal
    ; nfs_object spiegelt den gespeicherten Zustand
    mov edi, nfs_object
    mov ecx, NFS_O_ITEM / 4
    rep movsd
    mov edi, [nfs_leaf_page]
    call novafs_node_store
    jc .invalid
    popad
    clc
    ret
.invalid:
    popad
    stc
    ret

; ---------------------------------------------------------------------------
; Verzeichnisse und Pfade
; ---------------------------------------------------------------------------

; EAX=Eltern-ObjectID, ESI=Name, ECX=Laenge.
; EAX=ObjectID, EDX=Typ. CF wenn nicht vorhanden.
novafs_lookup:
    pushad
    test ecx, ecx
    jz .missing
    cmp ecx, 255
    ja .missing
    mov [nfs_name_ptr], esi
    mov [nfs_name_length], ecx
    mov ebx, eax
    call crc32c_compute
    mov edx, eax
    mov eax, ebx
    call novafs_key_set
    mov ebx, nfs_tree_directory
    call novafs_cursor_seek
    jc .invalid
.entry:
    call novafs_cursor_item
    jc .missing
    call novafs_compare_item
    test eax, eax
    jnz .missing
    movzx ecx, word [esi + NFS_D_NAME_LENGTH]
    cmp ecx, [nfs_name_length]
    jne .next
    push esi
    lea esi, [esi + NFS_D_NAME]
    mov edi, [nfs_name_ptr]
    repe cmpsb
    pop esi
    jne .next
    cmp dword [esi + NFS_D_OBJECT + 4], 0
    jne .corrupt
    mov eax, [esi + NFS_D_OBJECT]
    mov [esp + 28], eax
    mov eax, [esi + NFS_D_TYPE]
    mov [esp + 20], eax
    popad
    clc
    ret
.next:
    inc dword [nfs_cur_index]
    jmp .entry
.corrupt:
    mov dword [nfs_error], NOVAFS_ERR_CORRUPT
    jmp .invalid
.missing:
    mov dword [nfs_error], NOVAFS_ERR_NOT_FOUND
.invalid:
    popad
    stc
    ret

; ESI=absoluter Pfad, ECX=Laenge. EAX=ObjectID, EDX=Typ, CF wenn nicht gefunden.
novafs_resolve_path:
    test ecx, ecx
    jz novafs_resolve_missing
    cmp byte [esi], '/'
    jne novafs_resolve_missing
    mov eax, 1
; EAX=Start-Verzeichnis, ESI=Pfad, ECX=Laenge. Ein fuehrendes '/' beginnt bei
; der Wurzel. Leere Segmente werden ignoriert; "." und ".." existieren nicht.
novafs_resolve_from:
    pushad
    mov dword [nfs_error], NOVAFS_ERR_NOT_FOUND
    cmp dword [nfs_mounted], 1
    jne .missing
    test ecx, ecx
    jz .missing
    cmp byte [esi], '/'
    jne .relative
    mov eax, 1
.relative:
    lea ebp, [esi + ecx]             ; Ende
    push esi
    call novafs_object_find
    pop esi
    jc .missing
    mov edx, [nfs_object + NFS_O_TYPE]
.segment:
    cmp esi, ebp
    jae .done
    cmp byte [esi], '/'
    jne .name
    inc esi
    jmp .segment
.name:
    cmp edx, NOVAFS_TYPE_DIRECTORY
    jne .missing
    mov edi, esi
.name_end:
    cmp edi, ebp
    jae .have_name
    cmp byte [edi], '/'
    je .have_name
    inc edi
    jmp .name_end
.have_name:
    mov ecx, edi
    sub ecx, esi
    push edi
    push ebp
    call novafs_lookup
    pop ebp
    pop edi
    jc .missing
    mov esi, edi
    jmp .segment
.done:
    mov [esp + 28], eax
    mov [esp + 20], edx
    popad
    clc
    ret
.missing:
    popad
novafs_resolve_missing:
    stc
    ret

; EAX=Verzeichnis-ObjectID, EDX=Index. Kopiert den Index-ten Eintrag des
; Verzeichnisses nach nfs_item. CF am Ende oder bei Fehler.
novafs_dir_entry_at:
    pushad
    mov [nfs_dir_index], edx
    mov ebx, eax
    xor edx, edx
    call novafs_key_set
    mov eax, ebx
    mov ebx, nfs_tree_directory
    call novafs_cursor_seek
    jc .invalid
.entry:
    call novafs_cursor_item
    jc .end
    cmp [esi + NFS_D_PARENT], eax
    jne .end
    cmp dword [esi + NFS_D_PARENT + 4], 0
    jne .end
    cmp dword [nfs_dir_index], 0
    je .found
    dec dword [nfs_dir_index]
    inc dword [nfs_cur_index]
    jmp .entry
.found:
    mov edi, nfs_item
    mov ecx, NFS_D_ITEM / 4
    rep movsd
    popad
    clc
    ret
.end:
    mov dword [nfs_error], NOVAFS_ERR_NOT_FOUND
.invalid:
    popad
    stc
    ret

; EAX=ObjectID. CF=1, wenn das Objekt nicht unterhalb von /Benutzer (3) liegt
; und Aenderungen deshalb System-Write-Authority erfordern
; (NPSPEC-POLICY-SYSTEMWRITE-0001, Phase-1-Policy).
novafs_requires_system_write:
    pushad
    mov ecx, 64                      ; Tiefenbegrenzung gegen Zyklen
.walk:
    cmp eax, 3
    je .user_area
    cmp eax, 1
    jbe .protected
    push ecx
    call novafs_object_find
    pop ecx
    jc .protected
    cmp dword [nfs_object + NFS_O_PARENT + 4], 0
    jne .protected
    mov eax, [nfs_object + NFS_O_PARENT]
    loop .walk
.protected:
    popad
    stc
    ret
.user_area:
    popad
    clc
    ret

; ---------------------------------------------------------------------------
; Echtzeituhr (CMOS RTC) und Unix-Epoch (Abschnitt 106)
; ---------------------------------------------------------------------------
;
; AL=CMOS-Registerindex -> EAX=Rohwert (0..255, nullerweitert). Nur EAX wird
; veraendert.
novafs_cmos_read:
    out 0x70, al
    in al, 0x71
    movzx eax, al
    ret

; AL=BCD-Byte -> EAX=Binaerwert (High-Nibble*10 + Low-Nibble). Nur EAX wird
; veraendert, ECX/EDX werden intern benutzt und wiederhergestellt.
novafs_bcd_to_bin:
    push ecx
    push edx
    movzx eax, al
    mov edx, eax
    and edx, 0x0F
    push edx
    shr eax, 4
    and eax, 0x0F
    mov ecx, 10
    mul ecx                          ; EAX = Zehner-Nibble * 10, EDX wird dabei ueberschrieben
    pop edx
    add eax, edx
    pop edx
    pop ecx
    ret

; Liest alle RTC-Felder einmal roh aus CMOS (Ports 0x70/0x71) und wandelt sie
; (BCD- und 12h/24h-Korrektur anhand Status Register B) in die Skalare
; nfs_rtc_sec/min/hour/day/month/year (volles 4-stelliges Jahr). Kein
; UIP-Schutz hier; der Aufrufer (novafs_now_epoch) liest zweimal und
; vergleicht, um eine Lesung waehrend eines laufenden Registerupdates zu
; verwerfen. Keine NMI-Maskierung waehrend des Zugriffs (bewusste
; Vereinfachung, siehe Abschnitt 106 "Offen").
novafs_rtc_sample:
    push eax
    push ebx
    push ecx
    push edx

    mov al, 0x0B                     ; Status Register B
    out 0x70, al
    in al, 0x71
    movzx eax, al
    mov [nfs_rtc_regb], eax

    mov al, 0x00                     ; Sekunden
    call novafs_cmos_read
    mov [nfs_rtc_sec], eax
    mov al, 0x02                     ; Minuten
    call novafs_cmos_read
    mov [nfs_rtc_min], eax
    mov al, 0x04                     ; Stunden (Bit 7 = PM im 12h-Modus)
    call novafs_cmos_read
    mov [nfs_rtc_hour], eax
    mov al, 0x07                     ; Tag
    call novafs_cmos_read
    mov [nfs_rtc_day], eax
    mov al, 0x08                     ; Monat
    call novafs_cmos_read
    mov [nfs_rtc_month], eax
    mov al, 0x09                     ; Jahr (2-stellig)
    call novafs_cmos_read
    mov [nfs_rtc_year], eax
    mov al, 0x32                     ; Jahrhundert (nicht auf jeder Firmware belegt)
    call novafs_cmos_read
    mov [nfs_rtc_century], eax

    mov eax, [nfs_rtc_regb]
    test al, 0x04                    ; Bit 2 = 1: Werte sind bereits binaer
    jnz .binary_values

    mov eax, [nfs_rtc_sec]
    call novafs_bcd_to_bin
    mov [nfs_rtc_sec], eax
    mov eax, [nfs_rtc_min]
    call novafs_bcd_to_bin
    mov [nfs_rtc_min], eax
    mov eax, [nfs_rtc_hour]
    mov ebx, eax
    and ebx, 0x80                    ; PM-Flag vor der BCD-Wandlung sichern
    and eax, 0x7F
    call novafs_bcd_to_bin
    or eax, ebx
    mov [nfs_rtc_hour], eax
    mov eax, [nfs_rtc_day]
    call novafs_bcd_to_bin
    mov [nfs_rtc_day], eax
    mov eax, [nfs_rtc_month]
    call novafs_bcd_to_bin
    mov [nfs_rtc_month], eax
    mov eax, [nfs_rtc_year]
    call novafs_bcd_to_bin
    mov [nfs_rtc_year], eax
    mov eax, [nfs_rtc_century]
    call novafs_bcd_to_bin
    mov [nfs_rtc_century], eax
.binary_values:

    mov eax, [nfs_rtc_regb]
    test al, 0x02                    ; Bit 1 = 1: 24-Stunden-Format
    jnz .hours_24
    mov eax, [nfs_rtc_hour]
    mov ebx, eax
    and ebx, 0x80                    ; PM-Flag
    and eax, 0x7F                    ; Stundenwert 1..12
    cmp eax, 12
    jne .not_noon_midnight
    test ebx, ebx
    jz .midnight
    jmp .store_12h                   ; 12 PM (Mittag) bleibt 12
.midnight:
    xor eax, eax                     ; 12 AM (Mitternacht) = Stunde 0
    jmp .store_12h
.not_noon_midnight:
    test ebx, ebx
    jz .store_12h                    ; AM, 1..11: unveraendert
    add eax, 12                      ; PM, 1..11 -> 13..23
.store_12h:
    mov [nfs_rtc_hour], eax
.hours_24:
    and dword [nfs_rtc_hour], 0x7F   ; verbleibendes PM-Bit maskieren

    mov eax, [nfs_rtc_century]
    cmp eax, 19
    jb .century_default
    cmp eax, 21
    jbe .century_ok
.century_default:
    mov eax, 20                      ; unplausibel/nicht implementiert -> 20xx annehmen
.century_ok:
    mov ecx, 100
    mul ecx
    add eax, [nfs_rtc_year]
    mov [nfs_rtc_year], eax

    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

; ECX=Jahr (vierstellig) -> EAX=1, wenn Schaltjahr, sonst 0. ECX bleibt
; erhalten.
novafs_rtc_is_leap:
    push ecx
    push edx
    push ebx
    mov ebx, ecx
    mov eax, ebx
    xor edx, edx
    mov ecx, 4
    div ecx
    test edx, edx
    jnz .not_leap
    mov eax, ebx
    xor edx, edx
    mov ecx, 100
    div ecx
    test edx, edx
    jnz .leap
    mov eax, ebx
    xor edx, edx
    mov ecx, 400
    div ecx
    test edx, edx
    jnz .not_leap
.leap:
    mov eax, 1
    jmp .done
.not_leap:
    xor eax, eax
.done:
    pop ebx
    pop edx
    pop ecx
    ret

; Liefert EDX:EAX = aktuelle Unixzeit (Sekunden seit 1970-01-01T00:00:00,
; ohne Zeitzonenkorrektur: die RTC wird als UTC behandelt). Liest die
; CMOS-RTC zweimal hintereinander und verwirft die Lesung, falls sich
; zwischen beiden Lesungen etwas geaendert hat (Schutz gegen einen
; laufenden Registerupdate-Uebergang).
novafs_now_epoch:
    push ebx
    push ecx
    push esi
    push edi

.wait_uip:
    mov al, 0x0A
    out 0x70, al
    in al, 0x71
    test al, 0x80
    jnz .wait_uip

.read_again:
    call novafs_rtc_sample
    mov eax, [nfs_rtc_sec]
    mov [nfs_rtc_check], eax
    mov eax, [nfs_rtc_min]
    mov [nfs_rtc_check + 4], eax
    mov eax, [nfs_rtc_hour]
    mov [nfs_rtc_check + 8], eax
    mov eax, [nfs_rtc_day]
    mov [nfs_rtc_check + 12], eax
    mov eax, [nfs_rtc_month]
    mov [nfs_rtc_check + 16], eax
    mov eax, [nfs_rtc_year]
    mov [nfs_rtc_check + 20], eax

    call novafs_rtc_sample
    mov eax, [nfs_rtc_sec]
    cmp eax, [nfs_rtc_check]
    jne .read_again
    mov eax, [nfs_rtc_min]
    cmp eax, [nfs_rtc_check + 4]
    jne .read_again
    mov eax, [nfs_rtc_hour]
    cmp eax, [nfs_rtc_check + 8]
    jne .read_again
    mov eax, [nfs_rtc_day]
    cmp eax, [nfs_rtc_check + 12]
    jne .read_again
    mov eax, [nfs_rtc_month]
    cmp eax, [nfs_rtc_check + 16]
    jne .read_again
    mov eax, [nfs_rtc_year]
    cmp eax, [nfs_rtc_check + 20]
    jne .read_again

    ; Tage seit 1970-01-01 bis zum Beginn des gelesenen Jahres.
    mov dword [nfs_rtc_days], 0
    mov ecx, 1970
.year_loop:
    cmp ecx, [nfs_rtc_year]
    jge .year_done
    call novafs_rtc_is_leap
    mov edx, 365
    add edx, eax
    add [nfs_rtc_days], edx
    inc ecx
    jmp .year_loop
.year_done:

    ; Tage seit Jahresbeginn bis zum Beginn des Monats (Tabelle fuer
    ; Nicht-Schaltjahre; Schalttag wird separat addiert).
    mov eax, [nfs_rtc_month]
    dec eax
    mov esi, novafs_days_before_month
    mov eax, [esi + eax * 4]
    add [nfs_rtc_days], eax
    mov ecx, [nfs_rtc_year]
    call novafs_rtc_is_leap
    test eax, eax
    jz .no_leap_add
    cmp dword [nfs_rtc_month], 2
    jbe .no_leap_add
    inc dword [nfs_rtc_days]
.no_leap_add:

    ; Tag im Monat (1-basiert) -> Tage seit Monatsbeginn.
    mov eax, [nfs_rtc_day]
    dec eax
    add [nfs_rtc_days], eax

    ; Sekunden = Tage*86400 + Stunden*3600 + Minuten*60 + Sekunden (voll
    ; 64-Bit in EBX:ESI aufsummiert).
    mov eax, [nfs_rtc_days]
    mov ecx, 86400
    mul ecx
    mov ebx, eax
    mov esi, edx
    mov eax, [nfs_rtc_hour]
    mov ecx, 3600
    mul ecx
    add ebx, eax
    adc esi, edx
    mov eax, [nfs_rtc_min]
    mov ecx, 60
    mul ecx
    add ebx, eax
    adc esi, edx
    mov eax, [nfs_rtc_sec]
    add ebx, eax
    adc esi, 0

    mov eax, ebx
    mov edx, esi

    pop edi
    pop esi
    pop ecx
    pop ebx
    ret

novafs_days_before_month: dd 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334

; --- Nutzdatenprüfsummen ---
; novafs_data_checksum_write: EAX=Blocknummer, EDI=Datenpuffer (4096 Byte). CF=0 immer.
; Schreibt CRC32C des Datenpuffers in das Prüfsummen-Array des Volumes.
; Ist nfs_checksum_block=0 (altes Volume), wird nichts getan.
novafs_data_checksum_write:
    cmp dword [nfs_checksum_block], 0
    je .done
    pushad
    mov [nfs_csum_blk], eax
    ; CRC32C des Datenpuffers berechnen
    mov esi, edi
    mov ecx, NOVAFS_BLOCK_SIZE
    mov eax, 0xFFFFFFFF
    call crc32c_update
    not eax
    mov [nfs_csum_crc], eax
    ; Array-Block und Byte-Offset bestimmen (block/1024, (block%1024)*4)
    mov eax, [nfs_csum_blk]
    mov ecx, eax
    shr ecx, 10              ; ecx = block / 1024 = Offset im Array
    and eax, 0x3FF           ; eax = block % 1024
    shl eax, 2               ; eax = byte_offset = (block%1024)*4
    mov [nfs_csum_off], eax
    mov eax, [nfs_checksum_block]
    add eax, ecx             ; physischer Array-Block
    mov [nfs_csum_ablock], eax
    ; Array-Block lesen
    mov edi, [nfs_checksum_page]
    call novafs_read_block
    jc .skip
    ; CRC32C eintragen
    mov eax, [nfs_csum_off]
    mov ecx, [nfs_csum_crc]
    mov edi, [nfs_checksum_page]
    mov [edi + eax], ecx
    ; Array-Block zurückschreiben (journalisiert)
    mov eax, [nfs_csum_ablock]
    mov edi, [nfs_checksum_page]
    call novafs_write_block_logged
.skip:
    popad
.done:
    clc
    ret

; novafs_data_checksum_verify: EAX=Blocknummer, EDI=Datenpuffer. CF=1 bei Fehler.
; Liest CRC32C aus dem Prüfsummen-Array und vergleicht mit dem Datenpuffer.
; CRC=0 bedeutet nicht initialisiert -> Prüfung überspringen (CF=0).
; Ist nfs_checksum_block=0, wird nichts getan (CF=0).
novafs_data_checksum_verify:
    cmp dword [nfs_checksum_block], 0
    je .done_nc
    pushad
    mov [nfs_csum_blk], eax
    mov [nfs_csum_data], edi
    ; Array-Block und Byte-Offset
    mov ecx, eax
    shr ecx, 10
    and eax, 0x3FF
    shl eax, 2
    mov [nfs_csum_off], eax
    mov eax, [nfs_checksum_block]
    add eax, ecx
    ; Array-Block lesen
    mov edi, [nfs_checksum_page]
    call novafs_read_block
    jc .ok_pop               ; Lesefehler: Prüfung überspringen
    ; Gespeicherten CRC32C holen
    mov eax, [nfs_csum_off]
    mov esi, [nfs_checksum_page]
    mov ecx, [esi + eax]
    test ecx, ecx
    jz .ok_pop               ; 0 = nicht initialisiert -> OK
    mov [nfs_csum_crc], ecx
    ; CRC32C des Datenpuffers berechnen
    mov esi, [nfs_csum_data]
    mov ecx, NOVAFS_BLOCK_SIZE
    mov eax, 0xFFFFFFFF
    call crc32c_update
    not eax
    cmp eax, [nfs_csum_crc]
    jne .mismatch
.ok_pop:
    popad
.done_nc:
    clc
    ret
.mismatch:
    popad
    mov dword [nfs_error], NOVAFS_ERR_CORRUPT
    stc
    ret

; EAX=Eltern-ObjectID, ESI=Name, ECX=Laenge, EDX=Typ (Datei/Verzeichnis).
; EAX=neue ObjectID. Laufende Aenderung (novafs_change_begin) erforderlich.
novafs_create:
    pushad
    mov dword [nfs_error], NOVAFS_ERR_INVALID
    cmp dword [nfs_readonly], 0
    jne .read_only
    cmp edx, NOVAFS_TYPE_FILE
    je .type_ok
    cmp edx, NOVAFS_TYPE_DIRECTORY
    jne .invalid
.type_ok:
    mov [nfs_create_parent], eax
    mov [nfs_create_type], edx
    mov [nfs_create_name], esi
    mov [nfs_create_length], ecx
    call novafs_name_check
    jc .invalid
    mov eax, [nfs_create_parent]
    call novafs_object_find
    jc .invalid
    cmp dword [nfs_object + NFS_O_TYPE], NOVAFS_TYPE_DIRECTORY
    jne .invalid
    mov eax, [nfs_create_parent]
    mov esi, [nfs_create_name]
    mov ecx, [nfs_create_length]
    call novafs_lookup
    jnc .exists

    ; ObjectID vergeben (nie wiederverwendet)
    mov eax, [nfs_sb + NFS_SB_NEXT_OBJECT]
    cmp eax, 0xFFFFFFFF
    je .full
    mov [nfs_create_id], eax
    inc dword [nfs_sb + NFS_SB_NEXT_OBJECT]

    ; Objekt-Item
    mov edi, nfs_item
    xor eax, eax
    mov ecx, NFS_D_ITEM / 4
    rep stosd
    mov eax, [nfs_create_id]
    mov [nfs_item + NFS_O_ID], eax
    mov eax, [nfs_create_parent]
    mov [nfs_item + NFS_O_PARENT], eax
    mov eax, [nfs_create_type]
    mov [nfs_item + NFS_O_TYPE], eax
    mov dword [nfs_item + NFS_O_PERMISSIONS], 0644o
    cmp eax, NOVAFS_TYPE_DIRECTORY
    jne .perm_done
    mov dword [nfs_item + NFS_O_PERMISSIONS], 0755o
.perm_done:
    mov dword [nfs_item + NFS_O_LINKS], 1
    call novafs_now_epoch
    mov [nfs_item + NFS_O_CREATED], eax
    mov [nfs_item + NFS_O_CREATED + 4], edx
    mov [nfs_item + NFS_O_MODIFIED], eax
    mov [nfs_item + NFS_O_MODIFIED + 4], edx
    mov [nfs_item + NFS_O_ACCESSED], eax
    mov [nfs_item + NFS_O_ACCESSED + 4], edx
    mov [nfs_item + NFS_O_CHANGED], eax
    mov [nfs_item + NFS_O_CHANGED + 4], edx
    mov eax, [nfs_sb + NFS_SB_GENERATION]
    mov edx, [nfs_sb + NFS_SB_GENERATION + 4]
    add eax, 1
    adc edx, 0
    mov [nfs_item + NFS_O_GENERATION], eax
    mov [nfs_item + NFS_O_GENERATION + 4], edx
    mov esi, nfs_item
    mov ecx, NFS_O_ITEM
    mov edx, NFS_O_CHECKSUM
    call novafs_seal
    mov ebx, nfs_tree_object
    mov esi, nfs_item
    call novafs_tree_insert
    jc .failed

    ; Verzeichniseintrag
    mov edi, nfs_item
    xor eax, eax
    mov ecx, NFS_D_ITEM / 4
    rep stosd
    mov eax, [nfs_create_parent]
    mov [nfs_item + NFS_D_PARENT], eax
    mov esi, [nfs_create_name]
    mov ecx, [nfs_create_length]
    call crc32c_compute
    mov [nfs_item + NFS_D_HASH], eax
    mov eax, [nfs_create_id]
    mov [nfs_item + NFS_D_OBJECT], eax
    mov eax, [nfs_create_type]
    mov [nfs_item + NFS_D_TYPE], eax
    mov eax, [nfs_create_length]
    mov [nfs_item + NFS_D_NAME_LENGTH], ax
    mov esi, [nfs_create_name]
    mov edi, nfs_item + NFS_D_NAME
    mov ecx, [nfs_create_length]
    rep movsb
    mov ebx, nfs_tree_directory
    mov esi, nfs_item
    call novafs_tree_insert
    jc .failed
    add dword [nfs_sb + NFS_SB_OBJECT_COUNT], 1
    adc dword [nfs_sb + NFS_SB_OBJECT_COUNT + 4], 0
    mov eax, [nfs_create_id]
    mov [esp + 28], eax
    popad
    clc
    ret
.exists:
    mov dword [nfs_error], NOVAFS_ERR_EXISTS
    popad
    stc
    ret
.full:
    mov dword [nfs_error], NOVAFS_ERR_FULL
    popad
    stc
    ret
.read_only:
    mov dword [nfs_error], NOVAFS_ERR_READ_ONLY
.invalid:
.failed:
    popad
    stc
    ret

; ESI=Name, ECX=Laenge. Namensregeln: 1..255 Byte, kein '/', kein NUL, nicht
; "." oder "..". CF bei ungueltigem Namen.
novafs_name_check:
    pushad
    test ecx, ecx
    jz .invalid
    cmp ecx, 255
    ja .invalid
    xor ebx, ebx
.byte:
    mov al, [esi + ebx]
    test al, al
    jz .invalid
    cmp al, '/'
    je .invalid
    inc ebx
    cmp ebx, ecx
    jb .byte
    cmp byte [esi], '.'
    jne .valid
    cmp ecx, 1
    je .invalid
    cmp ecx, 2
    jne .valid
    cmp byte [esi + 1], '.'
    je .invalid
.valid:
    popad
    clc
    ret
.invalid:
    popad
    stc
    ret

; EBX=Baum. Entfernt das Item an der Cursorposition (nfs_leaf_page,
; nfs_cur_index) und schreibt das Blatt. Blaetter werden nicht zusammengelegt;
; leere Blaetter bleiben bestehen (NPSPEC-NOVAFS-ONDISK-0001 §5). Die
; Schluessel der inneren Wurzel bleiben gueltige Untergrenzen.
novafs_cursor_remove:
    pushad
    mov edi, [nfs_leaf_page]
    movzx ecx, word [edi + NFS_N_COUNT]
    mov eax, [nfs_cur_index]
    cmp eax, ecx
    jae .invalid
    mov edx, [ebx + NFS_TD_ITEM]
    mov ebp, ecx
    sub ebp, eax
    dec ebp
    imul ebp, edx                    ; Bytes hinter dem Item
    imul eax, edx
    lea edi, [edi + NOVAFS_NODE_HEADER + eax]
    lea esi, [edi + edx]
    mov ecx, ebp
    rep movsb
    mov ecx, edx                     ; frei gewordenen letzten Platz nullen
    xor eax, eax
    rep stosb
    mov edi, [nfs_leaf_page]
    dec word [edi + NFS_N_COUNT]
    call novafs_node_store
    jc .failed
    ; §125 – Blatt-Zusammenlegen pruefen (nur wenn Elternknoten vorhanden)
    cmp dword [nfs_path_depth], 0
    je .done
    call novafs_try_leaf_merge      ; ignoriere Fehler (best-effort)
.done:
    popad
    clc
    ret
.invalid:
    mov dword [nfs_error], NOVAFS_ERR_CORRUPT
.failed:
    popad
    stc
    ret

; §125 – Blatt-Zusammenlegen: rechtes Geschwister borgen oder verschmelzen.
; EBX=Baum. Nur aufgerufen wenn nfs_path_depth > 0 (Hoehe >= 2).
; Best-effort: Fehler werden ignoriert; Loeschen gelingt auch ohne Merge.
; Strategie: nur rechtes Geschwister, um Register-Druck gering zu halten.
; [nfs_aux_page] = rechtes Geschwister (wenn vorhanden).
novafs_try_leaf_merge:
    pushad
    ; Blatt-Itemzahl prüfen
    mov edi, [nfs_leaf_page]
    movzx ecx, word [edi + NFS_N_COUNT]
    mov edx, [ebx + NFS_TD_MAX]
    shr edx, 1
    cmp ecx, edx
    jae .done_ok                ; ausreichend gefuellt
    ; Elternknoten bestimmen (Pfadtiefe-1)
    mov ebp, [nfs_path_depth]
    dec ebp
    ; Rechtes Geschwister: Kind-Index+1 im Elternknoten
    mov eax, [nfs_path_indices + ebp*4]
    inc eax                     ; Index des rechten Geschwister-Eintrags
    mov ecx, ebp
    call novafs_level_page      ; EDI = Elternpuffer (Tiefe EBP)
    movzx ecx, word [edi + NFS_N_COUNT]
    cmp eax, ecx
    jae .done_ok                ; kein rechtes Geschwister
    ; Block des rechten Geschwisters holen
    imul ecx, eax, NOVAFS_INNER_ITEM
    cmp dword [edi + NOVAFS_NODE_HEADER + ecx + 20], 0
    jne .done_ok
    mov edx, [edi + NOVAFS_NODE_HEADER + ecx + 16]  ; Block des rechten Geschwister
    ; Rechtes Geschwister in nfs_aux_page laden
    push edi                    ; Elternpuffer retten
    push eax                    ; Einfüge-Index retten
    mov edi, [nfs_aux_page]
    mov eax, edx
    mov edx, 0xFFFFFFFF
    call novafs_node_load
    jc .pop2_done               ; Ladefehler – überspringen
    ; Anzahl Items im rechten Geschwister
    mov edi, [nfs_aux_page]
    movzx ecx, word [edi + NFS_N_COUNT]
    mov edx, [ebx + NFS_TD_MAX]
    shr edx, 1
    cmp ecx, edx
    jbe .do_merge               ; Geschwister hat <= min Items → verschmelzen
    ; Borrow-right: erstes Item des rechten Geschwister ans aktuelle Blatt anfügen
    lea esi, [edi + NOVAFS_NODE_HEADER]
    push edi                    ; aux_page retten
    mov edi, [nfs_leaf_page]
    movzx eax, word [edi + NFS_N_COUNT]
    mov ecx, [ebx + NFS_TD_ITEM]
    call novafs_node_insert_at  ; füge Item am Ende des aktuellen Blatts ein
    call novafs_node_store
    pop edi                     ; aux_page
    jc .pop2_done
    ; Erstes Item im rechten Geschwister entfernen
    movzx ecx, word [edi + NFS_N_COUNT]
    mov edx, [ebx + NFS_TD_ITEM]
    push edi
    lea esi, [edi + NOVAFS_NODE_HEADER + edx]
    lea edi, [edi + NOVAFS_NODE_HEADER]
    mov ecx, ecx
    dec ecx
    imul ecx, edx
    rep movsb
    pop edi
    movzx eax, word [edi + NFS_N_COUNT]
    dec eax
    mov word [edi + NFS_N_COUNT], ax
    imul eax, [ebx + NFS_TD_ITEM]
    lea edi, [edi + NOVAFS_NODE_HEADER + eax]
    mov ecx, [ebx + NFS_TD_ITEM]
    xor eax, eax
    rep stosb
    mov edi, [nfs_aux_page]
    call novafs_node_store
    ; Kein Eltern-Update nötig (Separator ist weiterhin gültige Untergrenze)
    pop eax                     ; Einfüge-Index (unbenutzt)
    pop edi                     ; Elternpuffer (unbenutzt)
    jmp .done_ok
.do_merge:
    ; Alle Items des rechten Geschwister ans aktuelle Blatt anhängen
    movzx edx, word [edi + NFS_N_COUNT]
    test edx, edx
    jz .pop2_done
    mov ecx, [ebx + NFS_TD_ITEM]
    imul ecx, edx
    lea esi, [edi + NOVAFS_NODE_HEADER]
    mov edi, [nfs_leaf_page]
    movzx eax, word [edi + NFS_N_COUNT]
    imul eax, [ebx + NFS_TD_ITEM]
    lea edi, [edi + NOVAFS_NODE_HEADER + eax]
    rep movsb
    ; Itemzahl des aktuellen Blatts erhöhen
    mov edi, [nfs_leaf_page]
    movzx eax, word [edi + NFS_N_COUNT]
    add eax, edx
    mov word [edi + NFS_N_COUNT], ax
    call novafs_node_store
    jc .pop2_done
    ; Rechtes Geschwister-Block freigeben
    mov edi, [nfs_aux_page]
    mov eax, [edi + NFS_N_BLOCK]
    call novafs_free_block
    ; Separator-Eintrag des rechten Geschwisters im Elternknoten entfernen
    pop eax                     ; rechter Geschwister-Index im Elternknoten
    pop edi                     ; Elternpuffer
    mov ecx, ebp
    call novafs_level_page      ; EDI = Elternpuffer
    movzx ecx, word [edi + NFS_N_COUNT]
    ; shift-delete: Item an Index EAX aus dem inneren Knoten entfernen
    mov edx, NOVAFS_INNER_ITEM
    mov ebp, ecx
    sub ebp, eax
    dec ebp
    imul ebp, edx
    imul eax, edx
    lea esi, [edi + NOVAFS_NODE_HEADER + eax]
    add esi, edx                ; esi = &items[aex+1]
    lea edi, [edi + NOVAFS_NODE_HEADER + eax]
    push ecx
    mov ecx, ebp
    rep movsb
    pop ecx
    dec ecx
    imul ecx, edx               ; letzten Eintrag nullen
    mov eax, ecx
    xor ecx, ecx
    ; edi zeigt schon auf die richtige Stelle
    mov ecx, edx
    rep stosb
    mov edi, [nfs_path_blocks + ebp*4]  ; EBP wurde überschrieben – nutze gespeicherte Tiefe
    ; Elternknoten schreiben: lade ihn über seinen Block
    mov ecx, [nfs_path_depth]
    dec ecx
    call novafs_level_page      ; EDI = Elternpuffer
    movzx eax, word [edi + NFS_N_COUNT]
    dec eax
    mov word [edi + NFS_N_COUNT], ax
    call novafs_node_store
    jmp .done_ok
.pop2_done:
    pop eax
    pop edi
.done_ok:
    popad
    clc
    ret

; EAX=Eltern-ObjectID, ESI=Name, ECX=Laenge. Loescht eine Datei samt Extents
; oder ein leeres Verzeichnis. Stabile Namespaces (ObjectID < 256) sind
; geschuetzt. Laufende Aenderung (novafs_change_begin) erforderlich.
; Reihenfolge: Extent-Item entfernen -> dessen Bloecke freigeben (die Bitmap
; wird erst im Commit geschrieben) -> Objekt-Item -> Verzeichniseintrag.
novafs_delete:
    pushad
    mov dword [nfs_error], NOVAFS_ERR_INVALID
    cmp dword [nfs_readonly], 0
    jne .read_only
    mov [nfs_del_parent], eax
    mov [nfs_del_name], esi
    mov [nfs_del_length], ecx
    call novafs_lookup
    jc .failed
    mov [nfs_del_object], eax
    mov [nfs_del_type], edx
    cmp eax, NOVAFS_FIRST_DYNAMIC_ID
    jb .protected
    cmp edx, NOVAFS_TYPE_DIRECTORY
    jne .extents
    xor edx, edx
    call novafs_dir_entry_at
    jnc .not_empty
    cmp dword [nfs_error], NOVAFS_ERR_NOT_FOUND
    jne .failed
.extents:
    mov eax, [nfs_del_object]
    xor edx, edx
    call novafs_key_set
    mov ebx, nfs_tree_extent
    call novafs_cursor_seek
    jc .failed
    call novafs_cursor_item
    jc .object
    mov eax, [nfs_del_object]
    cmp [esi + NFS_E_OBJECT], eax
    jne .object
    cmp dword [esi + NFS_E_OBJECT + 4], 0
    jne .object
    cmp dword [esi + NFS_E_PHYSICAL + 4], 0
    jne .corrupt
    cmp dword [esi + NFS_E_COUNT + 4], 0
    jne .corrupt
    mov eax, [esi + NFS_E_PHYSICAL]
    mov ecx, [esi + NFS_E_COUNT]
    test ecx, ecx
    jz .corrupt
    mov edx, [nfs_bitmap_blocks]
    add edx, 2
    cmp eax, edx
    jb .corrupt
    mov edx, eax
    add edx, ecx
    jc .corrupt
    mov edi, [nfs_total_blocks]
    dec edi
    cmp edx, edi
    ja .corrupt
    mov [nfs_del_physical], eax
    mov [nfs_del_count], ecx
    call novafs_cursor_remove
    jc .failed
    mov eax, [nfs_del_physical]
    mov ecx, [nfs_del_count]
.free:
    call novafs_free_block
    inc eax
    loop .free
    jmp .extents
.object:
    mov eax, [nfs_del_object]
    call novafs_object_find
    jc .failed
    mov ebx, nfs_tree_object
    call novafs_cursor_remove
    jc .failed
    mov eax, [nfs_del_parent]
    mov esi, [nfs_del_name]
    mov ecx, [nfs_del_length]
    call novafs_lookup
    jc .corrupt
    mov ebx, nfs_tree_directory
    call novafs_cursor_remove
    jc .failed
    sub dword [nfs_sb + NFS_SB_OBJECT_COUNT], 1
    sbb dword [nfs_sb + NFS_SB_OBJECT_COUNT + 4], 0
    popad
    clc
    ret
.protected:
    mov dword [nfs_error], NOVAFS_ERR_PROTECTED
    jmp .failed
.not_empty:
    mov dword [nfs_error], NOVAFS_ERR_NOT_EMPTY
    jmp .failed
.corrupt:
    mov dword [nfs_error], NOVAFS_ERR_CORRUPT
    jmp .failed
.read_only:
    mov dword [nfs_error], NOVAFS_ERR_READ_ONLY
.failed:
    popad
    stc
    ret

; Benennt um bzw. verschiebt: nfs_mv_src_parent/_name/_length ->
; nfs_mv_dst_parent/_name/_length. Ein vorhandenes Ziel wird nicht
; ueberschrieben; Verzeichnisse duerfen nicht in sich selbst oder ihre
; Nachfahren wandern. Der neue Verzeichniseintrag wird vor dem Entfernen des
; alten eingefuegt, danach folgt parent_id. Laufende Aenderung erforderlich.
novafs_rename:
    pushad
    mov dword [nfs_error], NOVAFS_ERR_INVALID
    cmp dword [nfs_readonly], 0
    jne .read_only
    mov esi, [nfs_mv_dst_name]
    mov ecx, [nfs_mv_dst_length]
    call novafs_name_check
    jc .failed
    mov eax, [nfs_mv_src_parent]
    mov esi, [nfs_mv_src_name]
    mov ecx, [nfs_mv_src_length]
    call novafs_lookup
    jc .failed
    mov [nfs_mv_object], eax
    mov [nfs_mv_type], edx
    cmp eax, NOVAFS_FIRST_DYNAMIC_ID
    jb .protected
    mov eax, [nfs_mv_dst_parent]
    call novafs_object_find
    jc .failed
    cmp dword [nfs_object + NFS_O_TYPE], NOVAFS_TYPE_DIRECTORY
    jne .invalid
    ; Zyklusschutz: das Objekt darf kein Vorfahre des Zielverzeichnisses sein.
    mov eax, [nfs_mv_dst_parent]
    mov ecx, 64
.walk:
    cmp eax, [nfs_mv_object]
    je .invalid
    cmp eax, 1
    jbe .walk_done
    push ecx
    call novafs_object_find
    pop ecx
    jc .failed
    cmp dword [nfs_object + NFS_O_PARENT + 4], 0
    jne .corrupt
    mov eax, [nfs_object + NFS_O_PARENT]
    loop .walk
    jmp .corrupt
.walk_done:
    mov eax, [nfs_mv_dst_parent]
    mov esi, [nfs_mv_dst_name]
    mov ecx, [nfs_mv_dst_length]
    call novafs_lookup
    jc .target_free
    cmp eax, [nfs_mv_object]
    je .done                         ; gleicher Name im gleichen Verzeichnis
    mov dword [nfs_error], NOVAFS_ERR_EXISTS
    jmp .failed
.target_free:
    cmp dword [nfs_error], NOVAFS_ERR_NOT_FOUND
    jne .failed
    mov edi, nfs_item
    xor eax, eax
    mov ecx, NFS_D_ITEM / 4
    rep stosd
    mov eax, [nfs_mv_dst_parent]
    mov [nfs_item + NFS_D_PARENT], eax
    mov esi, [nfs_mv_dst_name]
    mov ecx, [nfs_mv_dst_length]
    call crc32c_compute
    mov [nfs_item + NFS_D_HASH], eax
    mov eax, [nfs_mv_object]
    mov [nfs_item + NFS_D_OBJECT], eax
    mov eax, [nfs_mv_type]
    mov [nfs_item + NFS_D_TYPE], eax
    mov eax, [nfs_mv_dst_length]
    mov [nfs_item + NFS_D_NAME_LENGTH], ax
    mov esi, [nfs_mv_dst_name]
    mov edi, nfs_item + NFS_D_NAME
    mov ecx, [nfs_mv_dst_length]
    rep movsb
    mov ebx, nfs_tree_directory
    mov esi, nfs_item
    call novafs_tree_insert
    jc .failed
    mov eax, [nfs_mv_src_parent]
    mov esi, [nfs_mv_src_name]
    mov ecx, [nfs_mv_src_length]
    call novafs_lookup
    jc .corrupt
    cmp eax, [nfs_mv_object]
    jne .corrupt
    mov ebx, nfs_tree_directory
    call novafs_cursor_remove
    jc .failed
    mov eax, [nfs_mv_object]
    call novafs_object_find
    jc .failed
    call novafs_now_epoch
    mov [nfs_object + NFS_O_CHANGED], eax
    mov [nfs_object + NFS_O_CHANGED + 4], edx
    mov eax, [nfs_mv_dst_parent]
    cmp eax, [nfs_mv_src_parent]
    je .store_object
    mov [nfs_object + NFS_O_PARENT], eax
    mov dword [nfs_object + NFS_O_PARENT + 4], 0
.store_object:
    call novafs_object_store
    jc .failed
.done:
    popad
    clc
    ret
.protected:
    mov dword [nfs_error], NOVAFS_ERR_PROTECTED
    jmp .failed
.invalid:
    mov dword [nfs_error], NOVAFS_ERR_INVALID
    jmp .failed
.corrupt:
    mov dword [nfs_error], NOVAFS_ERR_CORRUPT
    jmp .failed
.read_only:
    mov dword [nfs_error], NOVAFS_ERR_READ_ONLY
.failed:
    popad
    stc
    ret

; ---------------------------------------------------------------------------
; Extents und Dateidaten
; ---------------------------------------------------------------------------

; EAX=ObjectID, EDX=logischer Block. EAX=physischer Block oder 0 (Loch).
; nfs_prev_* beschreibt das letzte Extent des Objekts mit Start <= Block.
novafs_extent_map:
    pushad
    mov [nfs_map_object], eax
    mov [nfs_map_block], edx
    mov dword [nfs_prev_valid], 0
    mov dword [nfs_map_result], 0
    xor edx, edx
    call novafs_key_set
    mov ebx, nfs_tree_extent
    call novafs_cursor_seek
    jc .invalid
.extent:
    call novafs_cursor_item
    jc .done
    mov eax, [nfs_map_object]
    cmp [esi + NFS_E_OBJECT], eax
    jne .done
    cmp dword [esi + NFS_E_OBJECT + 4], 0
    jne .done
    mov eax, [esi + NFS_E_LOGICAL]
    test eax, 0xFFF
    jnz .corrupt
    cmp dword [esi + NFS_E_LOGICAL + 4], 0
    jne .corrupt
    shr eax, 12                      ; Start als Blocknummer
    cmp eax, [nfs_map_block]
    ja .done
    mov ecx, [esi + NFS_E_COUNT]
    cmp dword [esi + NFS_E_COUNT + 4], 0
    jne .corrupt
    test ecx, ecx
    jz .corrupt
    mov edx, [esi + NFS_E_PHYSICAL]
    cmp dword [esi + NFS_E_PHYSICAL + 4], 0
    jne .corrupt
    mov [nfs_prev_logical], eax
    mov [nfs_prev_count], ecx
    mov [nfs_prev_physical], edx
    mov dword [nfs_prev_valid], 1
    mov edi, [nfs_map_block]
    sub edi, eax
    cmp edi, ecx
    jae .next
    add edx, edi
    ; physischer Block muss im Datenbereich liegen
    cmp edx, [nfs_total_blocks]
    jae .corrupt
    mov [nfs_map_result], edx
    jmp .done
.next:
    inc dword [nfs_cur_index]
    jmp .extent
.done:
    mov eax, [nfs_map_result]
    mov [esp + 28], eax
    popad
    clc
    ret
.corrupt:
    mov dword [nfs_error], NOVAFS_ERR_CORRUPT
.invalid:
    popad
    stc
    ret

; EAX=ObjectID, EDX=logischer Block. Ordnet einen neuen physischen Block zu:
; bevorzugt durch Verlaengern des vorherigen Extents. EAX=physischer Block.
novafs_extent_allocate:
    pushad
    mov [nfs_map_object], eax
    mov [nfs_map_block], edx
    cmp dword [nfs_prev_valid], 1
    jne .new_extent
    mov eax, [nfs_prev_logical]
    add eax, [nfs_prev_count]
    cmp eax, edx
    jne .new_extent_hint
    mov eax, [nfs_prev_physical]
    add eax, [nfs_prev_count]
    mov [nfs_alloc_result], eax
    call novafs_alloc_specific
    jc .new_extent_hint
    ; vorhandenes Extent um einen Block verlaengern
    mov eax, [nfs_prev_logical]
    shl eax, 12
    mov edx, eax
    mov eax, [nfs_map_object]
    call novafs_key_set
    mov ebx, nfs_tree_extent
    call novafs_cursor_seek
    jc .rollback
    call novafs_cursor_item
    jc .rollback
    call novafs_compare_item
    test eax, eax
    jnz .rollback
    inc dword [esi + NFS_E_COUNT]
    add dword [esi + NFS_E_UNCOMPRESSED], NOVAFS_BLOCK_SIZE
    adc dword [esi + NFS_E_UNCOMPRESSED + 4], 0
    add dword [esi + NFS_E_STORED], NOVAFS_BLOCK_SIZE
    adc dword [esi + NFS_E_STORED + 4], 0
    mov eax, [nfs_sb + NFS_SB_GENERATION]
    inc eax
    mov [esi + NFS_E_GENERATION], eax
    mov edi, [nfs_leaf_page]
    call novafs_node_store
    jc .invalid
    inc dword [nfs_prev_count]
    jmp .done
.new_extent_hint:
    mov eax, [nfs_prev_physical]
    add eax, [nfs_prev_count]
    jmp .allocate
.new_extent:
    mov eax, [nfs_bitmap_blocks]
    add eax, 2
.allocate:
    call novafs_alloc_block
    jc .invalid
    mov [nfs_alloc_result], eax
    mov edi, nfs_item
    xor eax, eax
    mov ecx, NFS_D_ITEM / 4
    rep stosd
    mov eax, [nfs_map_object]
    mov [nfs_item + NFS_E_OBJECT], eax
    mov eax, [nfs_map_block]
    shl eax, 12
    mov [nfs_item + NFS_E_LOGICAL], eax
    mov eax, [nfs_alloc_result]
    mov [nfs_item + NFS_E_PHYSICAL], eax
    mov dword [nfs_item + NFS_E_COUNT], 1
    mov dword [nfs_item + NFS_E_UNCOMPRESSED], NOVAFS_BLOCK_SIZE
    mov dword [nfs_item + NFS_E_STORED], NOVAFS_BLOCK_SIZE
    mov eax, [nfs_sb + NFS_SB_GENERATION]
    inc eax
    mov [nfs_item + NFS_E_GENERATION], eax
    mov ebx, nfs_tree_extent
    mov esi, nfs_item
    call novafs_tree_insert
    jc .rollback
    mov eax, [nfs_map_block]
    mov [nfs_prev_logical], eax
    mov dword [nfs_prev_count], 1
    mov eax, [nfs_alloc_result]
    mov [nfs_prev_physical], eax
    mov dword [nfs_prev_valid], 1
.done:
    mov eax, [nfs_alloc_result]
    mov [esp + 28], eax
    popad
    clc
    ret
.rollback:
    mov eax, [nfs_alloc_result]
    call novafs_free_block
.invalid:
    popad
    stc
    ret

; EAX=ObjectID, EDX=Offset, ECX=Laenge, EDI=Ziel.
; EAX=gelesene Bytes (gekuerzt auf die Dateigroesse). CF bei Fehler.
novafs_read:
    pushad
    mov [nfs_rw_object], eax
    mov [nfs_rw_offset], edx
    mov [nfs_rw_buffer], edi
    mov dword [nfs_rw_done], 0
    call novafs_object_find
    jc .invalid
    cmp dword [nfs_object + NFS_O_TYPE], NOVAFS_TYPE_FILE
    jne .invalid
    cmp dword [nfs_object + NFS_O_SIZE + 4], 0
    jne .invalid
    mov eax, [nfs_object + NFS_O_SIZE]
    sub eax, [nfs_rw_offset]
    jbe .done
    cmp ecx, eax
    jbe .length_ok
    mov ecx, eax
.length_ok:
    mov [nfs_rw_remaining], ecx
.block:
    cmp dword [nfs_rw_remaining], 0
    je .done
    mov eax, [nfs_rw_object]
    mov edx, [nfs_rw_offset]
    shr edx, 12
    call novafs_extent_map
    jc .invalid
    mov ebx, [nfs_rw_offset]
    and ebx, 0xFFF
    mov ecx, NOVAFS_BLOCK_SIZE
    sub ecx, ebx
    cmp ecx, [nfs_rw_remaining]
    jbe .chunk
    mov ecx, [nfs_rw_remaining]
.chunk:
    mov edi, [nfs_rw_buffer]
    test eax, eax
    jnz .mapped
    push ecx
    xor eax, eax
    rep stosb
    pop ecx
    jmp .advance
.mapped:
    mov edi, [nfs_data_page]
    call novafs_read_block
    jc .invalid
    ; EAX = Blocknummer (durch pushad/popad in novafs_read_block erhalten)
    mov edi, [nfs_data_page]
    call novafs_data_checksum_verify
    jc .invalid
    mov esi, [nfs_data_page]
    add esi, ebx
    mov edi, [nfs_rw_buffer]
    push ecx
    rep movsb
    pop ecx
.advance:
    add [nfs_rw_buffer], ecx
    add [nfs_rw_offset], ecx
    add [nfs_rw_done], ecx
    sub [nfs_rw_remaining], ecx
    jmp .block
.done:
    mov eax, [nfs_rw_done]
    mov [esp + 28], eax
    popad
    clc
    ret
.invalid:
    popad
    stc
    ret

; EAX=ObjectID, EDX=Offset, ECX=Laenge, ESI=Quelle. CF bei Fehler.
; Laufende Aenderung erforderlich. Neue Bloecke werden genullt, Teilbloecke
; per Read-Modify-Write aktualisiert, die Dateigroesse waechst monoton.
novafs_write:
    pushad
    mov [nfs_rw_object], eax
    mov [nfs_rw_offset], edx
    mov [nfs_rw_buffer], esi
    mov [nfs_rw_remaining], ecx
    mov dword [nfs_rw_new_blocks], 0
    cmp dword [nfs_readonly], 0
    jne .read_only
    mov eax, edx
    add eax, ecx
    jc .invalid
    mov [nfs_rw_end], eax
    mov eax, [nfs_rw_object]
    call novafs_object_find
    jc .invalid
    cmp dword [nfs_object + NFS_O_TYPE], NOVAFS_TYPE_FILE
    jne .invalid
    cmp dword [nfs_object + NFS_O_SIZE + 4], 0
    jne .invalid
.block:
    cmp dword [nfs_rw_remaining], 0
    je .metadata
    mov eax, [nfs_rw_object]
    mov edx, [nfs_rw_offset]
    shr edx, 12
    mov [nfs_rw_block], edx
    call novafs_extent_map
    jc .invalid
    mov [nfs_rw_physical], eax
    mov ebx, [nfs_rw_offset]
    and ebx, 0xFFF
    mov ecx, NOVAFS_BLOCK_SIZE
    sub ecx, ebx
    cmp ecx, [nfs_rw_remaining]
    jbe .chunk
    mov ecx, [nfs_rw_remaining]
.chunk:
    mov [nfs_rw_chunk], ecx
    cmp dword [nfs_rw_physical], 0
    jne .existing
    mov eax, [nfs_rw_object]
    mov edx, [nfs_rw_block]
    call novafs_extent_allocate
    jc .invalid
    mov [nfs_rw_physical], eax
    inc dword [nfs_rw_new_blocks]
    mov edi, [nfs_data_page]
    xor eax, eax
    mov ecx, NOVAFS_BLOCK_SIZE / 4
    rep stosd
    jmp .modify
.existing:
    ; §125 Copy-on-Write: neuen Block allozieren, alten Inhalt lesen (bei Teilschreibung),
    ; modifizierten Inhalt schreiben, Extent-Pointer aktualisieren, alten Block freigeben.
    mov eax, [nfs_rw_physical]
    mov [nfs_cow_old_phys], eax
    mov eax, [nfs_rw_object]
    mov [nfs_cow_object], eax
    mov eax, [nfs_rw_block]
    mov [nfs_cow_log_blk], eax
    ; Neuen Block allozieren
    call novafs_alloc_block
    jc .invalid
    mov [nfs_cow_new_phys], eax
    mov [nfs_rw_physical], eax      ; ab jetzt neuer Block
    ; Alten Block-Inhalt lesen (immer, auch bei Vollblock – Metadaten kopieren)
    mov eax, [nfs_cow_old_phys]
    mov edi, [nfs_data_page]
    call novafs_read_block
    jc .cow_rollback
.modify:
    mov esi, [nfs_rw_buffer]
    mov edi, [nfs_data_page]
    mov ebx, [nfs_rw_offset]
    and ebx, 0xFFF
    add edi, ebx
    mov ecx, [nfs_rw_chunk]
    rep movsb
    mov eax, [nfs_rw_physical]
    mov edi, [nfs_data_page]
    call novafs_write_block_logged
    jc .invalid
    mov eax, [nfs_rw_physical]
    mov edi, [nfs_data_page]
    call novafs_data_checksum_write
    ; §125 CoW: Extent-Eintrag auf neuen Block umbiegen
    cmp dword [nfs_cow_new_phys], 0
    je .cow_done
    mov eax, [nfs_cow_object]
    mov edx, [nfs_cow_log_blk]
    mov ecx, [nfs_cow_old_phys]
    mov ebx, [nfs_cow_new_phys]
    call novafs_extent_cow_update
    jc .invalid
    ; Alten Block freigeben (sofort wenn keine Snapshots)
    mov eax, [nfs_cow_old_phys]
    call novafs_cow_deferred_free
    mov dword [nfs_cow_new_phys], 0
.cow_done:
    mov ecx, [nfs_rw_chunk]
    add [nfs_rw_buffer], ecx
    add [nfs_rw_offset], ecx
    sub [nfs_rw_remaining], ecx
    jmp .block
.cow_rollback:
    mov eax, [nfs_cow_new_phys]
    call novafs_free_block
    jmp .invalid
.metadata:
    mov eax, [nfs_rw_object]
    call novafs_object_find
    jc .invalid
    mov eax, [nfs_rw_end]
    cmp eax, [nfs_object + NFS_O_SIZE]
    jbe .size_done
    mov [nfs_object + NFS_O_SIZE], eax
.size_done:
    mov eax, [nfs_rw_new_blocks]
    shl eax, 12
    add [nfs_object + NFS_O_ALLOCATED], eax
    adc dword [nfs_object + NFS_O_ALLOCATED + 4], 0
    call novafs_now_epoch
    mov [nfs_object + NFS_O_MODIFIED], eax
    mov [nfs_object + NFS_O_MODIFIED + 4], edx
    mov [nfs_object + NFS_O_CHANGED], eax
    mov [nfs_object + NFS_O_CHANGED + 4], edx
    mov [nfs_object + NFS_O_ACCESSED], eax
    mov [nfs_object + NFS_O_ACCESSED + 4], edx
    call novafs_object_store
    jc .invalid
    popad
    clc
    ret
.read_only:
    mov dword [nfs_error], NOVAFS_ERR_READ_ONLY
.invalid:
    popad
    stc
    ret

; ---------------------------------------------------------------------------
; §125 – Copy-on-Write Helfer
; ---------------------------------------------------------------------------

; EAX=ObjectID, EDX=logischer Block, ECX=alter phys. Block, EBX=neuer phys. Block.
; Aktualisiert den Extent-Baum-Eintrag (ggf. Aufteilung des Multi-Block-Extents).
; CF bei Fehler.
novafs_extent_cow_update:
    pushad
    mov [nfs_cow_object], eax
    mov [nfs_cow_log_blk], edx
    mov [nfs_cow_old_phys], ecx
    mov [nfs_cow_new_phys], ebx
    ; Extent für das Objekt+logischen Block suchen
    xor edx, edx
    call novafs_key_set
    mov ebx, nfs_tree_extent
    call novafs_cursor_seek
    jc .invalid
.scan:
    call novafs_cursor_item
    jc .invalid
    mov eax, [nfs_cow_object]
    cmp [esi + NFS_E_OBJECT], eax
    jne .invalid
    cmp dword [esi + NFS_E_OBJECT + 4], 0
    jne .invalid
    ; Prüfen ob logischer Block in diesem Extent liegt
    mov eax, [esi + NFS_E_LOGICAL]
    shr eax, 12                     ; Start-Block
    mov ecx, [esi + NFS_E_COUNT]
    mov edx, eax
    add edx, ecx
    mov ebp, [nfs_cow_log_blk]
    cmp ebp, eax
    jb .next
    cmp ebp, edx
    jae .next
    ; Gefunden. Ist es ein Single-Block-Extent?
    cmp ecx, 1
    je .single_block
    ; Multi-Block-Extent: aufteilen in bis zu 3 Teile.
    ; Teil 1: [start .. log_blk-1]  (vorhanden wenn log_blk > start)
    ; Teil 2: [log_blk, new_phys, 1]
    ; Teil 3: [log_blk+1 .. end-1] (vorhanden wenn log_blk < end-1)
    mov [nfs_ins_left], eax         ; Extent-Start
    mov [nfs_ins_count], ecx        ; Extent-Count
    ; Alten Extent-Block und Start-Physical merken
    mov eax, [esi + NFS_E_PHYSICAL]
    mov [nfs_ins_right], eax        ; alter Start-Physical
    ; Schrumpfe den bestehenden Eintrag auf Teil 1 (wenn möglich)
    mov eax, [nfs_ins_left]
    mov ecx, [nfs_cow_log_blk]
    sub ecx, eax                    ; Länge Teil 1
    test ecx, ecx
    jz .no_part1
    mov [esi + NFS_E_COUNT], ecx
    mov edi, [nfs_leaf_page]
    call novafs_node_store
    jc .invalid
    jmp .insert_part2
.no_part1:
    ; Teil 1 hat Länge 0: lösche bestehenden Eintrag
    mov ebx, nfs_tree_extent
    call novafs_cursor_remove
    jc .invalid
.insert_part2:
    ; Teil 2 einfügen: [log_blk, new_phys, 1]
    mov edi, nfs_item
    xor eax, eax
    mov ecx, NFS_E_ITEM / 4
    rep stosd
    mov eax, [nfs_cow_object]
    mov [nfs_item + NFS_E_OBJECT], eax
    mov eax, [nfs_cow_log_blk]
    shl eax, 12
    mov [nfs_item + NFS_E_LOGICAL], eax
    mov eax, [nfs_cow_new_phys]
    mov [nfs_item + NFS_E_PHYSICAL], eax
    mov dword [nfs_item + NFS_E_COUNT], 1
    mov dword [nfs_item + NFS_E_UNCOMPRESSED], NOVAFS_BLOCK_SIZE
    mov dword [nfs_item + NFS_E_STORED], NOVAFS_BLOCK_SIZE
    mov ebx, nfs_tree_extent
    mov esi, nfs_item
    call novafs_tree_insert
    jc .invalid
    ; Teil 3 einfügen: [log_blk+1 .. end-1] (wenn nötig)
    mov eax, [nfs_cow_log_blk]
    inc eax                         ; log_blk+1
    mov ecx, [nfs_ins_left]
    add ecx, [nfs_ins_count]        ; Extent-Ende
    sub ecx, eax                    ; Länge Teil 3
    jbe .done_cow
    mov edi, nfs_item
    push ecx
    xor ecx, ecx
    mov edx, NFS_E_ITEM / 4
    push edx
    push edi
    mov ecx, NFS_E_ITEM / 4
    rep stosd
    pop edi
    pop edx
    pop ecx
    mov edx, [nfs_cow_object]
    mov [nfs_item + NFS_E_OBJECT], edx
    mov edx, eax
    shl edx, 12
    mov [nfs_item + NFS_E_LOGICAL], edx
    ; Physischer Block des Teils 3: alter Start + (log_blk+1 - extent_start)
    mov edx, [nfs_ins_right]        ; alter Start-Physical
    mov ebp, eax
    sub ebp, [nfs_ins_left]
    add edx, ebp
    mov [nfs_item + NFS_E_PHYSICAL], edx
    mov [nfs_item + NFS_E_COUNT], ecx
    mov ebp, ecx
    shl ebp, 12
    mov [nfs_item + NFS_E_UNCOMPRESSED], ebp
    mov [nfs_item + NFS_E_STORED], ebp
    mov ebx, nfs_tree_extent
    mov esi, nfs_item
    call novafs_tree_insert
    jc .invalid
    jmp .done_cow
.single_block:
    ; Einfacher Fall: Extent hat genau 1 Block → Physical direkt überschreiben
    mov eax, [nfs_cow_new_phys]
    mov [esi + NFS_E_PHYSICAL], eax
    mov edi, [nfs_leaf_page]
    call novafs_node_store
    jc .invalid
    jmp .done_cow
.next:
    inc dword [nfs_cur_index]
    jmp .scan
.done_cow:
    popad
    clc
    ret
.invalid:
    popad
    stc
    ret

; EAX=physischer Block. Gibt ihn sofort frei, wenn keine Snapshots existieren.
; Mit Snapshots: Block wird nicht freigegeben (künftige Erweiterung: Ref-Count).
novafs_cow_deferred_free:
    pushad
    cmp dword [nfs_snap_count], 0
    jne .deferred
    call novafs_free_block
.deferred:
    popad
    clc
    ret

; ---------------------------------------------------------------------------
; §125 – Snapshots
; ---------------------------------------------------------------------------

; Snapshot anlegen. EAX = Snapshot-Index (0..3); CF bei Fehler (kein Platz).
novafs_snapshot_create:
    pushad
    ; Freien Slot suchen
    xor ebp, ebp
.find_slot:
    cmp ebp, NFS_MAX_SNAPSHOTS
    jae .full
    imul ecx, ebp, NFS_SNAP_ENTRY
    add ecx, NFS_SB_SNAP_TABLE
    cmp dword [nfs_sb + ecx + NFS_SNAP_VALID], 0
    je .slot_found
    inc ebp
    jmp .find_slot
.slot_found:
    ; Snapshot-Eintrag füllen
    imul ecx, ebp, NFS_SNAP_ENTRY
    add ecx, NFS_SB_SNAP_TABLE
    lea edi, [nfs_sb + ecx]
    ; Generation
    mov eax, [nfs_sb + NFS_SB_GENERATION]
    mov [edi + NFS_SNAP_GENERATION], eax
    mov eax, [nfs_sb + NFS_SB_GENERATION + 4]
    mov [edi + NFS_SNAP_GENERATION + 4], eax
    ; Tree roots
    mov eax, [nfs_sb + NFS_SB_OBJECT_TREE]
    mov [edi + NFS_SNAP_OBJ_ROOT], eax
    mov eax, [nfs_sb + NFS_SB_DIRECTORY_TREE]
    mov [edi + NFS_SNAP_DIR_ROOT], eax
    mov eax, [nfs_sb + NFS_SB_EXTENT_TREE]
    mov [edi + NFS_SNAP_EXT_ROOT], eax
    mov eax, [nfs_sb + NFS_SB_CHECKSUM_TREE]
    mov [edi + NFS_SNAP_CSM_ROOT], eax
    ; Timestamp
    call novafs_now_epoch
    mov [edi + NFS_SNAP_TIMESTAMP], eax
    mov [edi + NFS_SNAP_TIMESTAMP + 4], edx
    ; Als gültig markieren
    mov dword [edi + NFS_SNAP_VALID], 1
    inc dword [nfs_sb + NFS_SB_SNAP_COUNT]
    inc dword [nfs_snap_count]
    ; Rückgabe: EAX = Slot-Index
    mov [esp + 28], ebp             ; EAX in gespeichertem pushad-Rahmen
    popad
    clc
    ret
.full:
    mov dword [nfs_error], NOVAFS_ERR_FULL
    popad
    stc
    ret

; EAX=Snapshot-Index. Löscht einen Snapshot. CF bei ungültigem Index.
novafs_snapshot_delete:
    pushad
    cmp eax, NFS_MAX_SNAPSHOTS
    jae .invalid
    imul ecx, eax, NFS_SNAP_ENTRY
    add ecx, NFS_SB_SNAP_TABLE
    cmp dword [nfs_sb + ecx + NFS_SNAP_VALID], 0
    je .invalid
    ; Eintrag löschen
    lea edi, [nfs_sb + ecx]
    xor eax, eax
    mov ecx, NFS_SNAP_ENTRY / 4
    rep stosd
    dec dword [nfs_sb + NFS_SB_SNAP_COUNT]
    dec dword [nfs_snap_count]
    popad
    clc
    ret
.invalid:
    mov dword [nfs_error], NOVAFS_ERR_INVALID
    popad
    stc
    ret

; EDI=Puffer (NFS_MAX_SNAPSHOTS * NFS_SNAP_ENTRY Bytes).
; Kopiert die Snapshot-Tabelle in den Puffer. CF bei Fehler.
novafs_snapshot_list:
    pushad
    test edi, edi
    jz .invalid
    lea esi, [nfs_sb + NFS_SB_SNAP_TABLE]
    mov ecx, NFS_MAX_SNAPSHOTS * NFS_SNAP_ENTRY / 4
    rep movsd
    popad
    clc
    ret
.invalid:
    popad
    stc
    ret

; ---------------------------------------------------------------------------
; Mount
; ---------------------------------------------------------------------------

novafs_alloc_pages:
    cmp dword [nfs_root_page], 0
    jne .ready
    mov esi, nfs_page_slots
.next:
    cmp esi, nfs_page_slots_end
    jae .ready
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [esi], eax
    add esi, 4
    jmp .next
.ready:
    clc
    ret
.invalid:
    stc
    ret

; Sucht auf allen Datentraegern die NovaFS-Systempartition und mountet sie.
; Fehlt sie, bleibt das Bootstrap-RAMFS Root (kein Panic).
novafs_mount_system:
    mov dword [nfs_mounted], 0
    xor ebx, ebx
.device:
    cmp ebx, [storage_count]
    jae .none
    mov eax, ebx
    push ebx
    call storage_find_novafs_partition
    pop ebx
    jc .next
    push ebx
    mov ecx, edx
    mov edx, eax
    mov eax, ebx
    call novafs_mount
    pop ebx
    jnc .mounted
    mov esi, message_novafs_mount_failed
    call serial_write_string
    mov eax, [nfs_error]
    call serial_write_hex32
    mov esi, message_line_end
    call serial_write_string
.next:
    inc ebx
    jmp .device
.none:
    mov esi, message_novafs_absent
    call serial_write_string
    clc
    ret
.mounted:
    clc
    ret

; EAX=Storage-Index, EDX=erste LBA, ECX=Sektoren. CF bei Fehler.
novafs_mount:
    pushad
    mov dword [nfs_error], NOVAFS_ERR_INVALID
    mov dword [nfs_mounted], 0
    mov dword [nfs_readonly], 1
    mov dword [nfs_bitmap_dirty], 0
    mov [nfs_dev], eax
    mov [nfs_lba], edx
    shr ecx, 3
    cmp ecx, 64
    jb .invalid
    mov [nfs_part_blocks], ecx
    mov [nfs_limit_blocks], ecx
    call novafs_alloc_pages
    jc .invalid

    ; Primaer- und Backup-Superblock lesen und bewerten
    mov dword [nfs_primary_valid], 0
    mov dword [nfs_backup_valid], 0
    mov eax, 1
    mov edi, [nfs_root_page]
    call novafs_read_block
    jc .primary_done
    call novafs_superblock_validate
    jc .primary_done
    mov dword [nfs_primary_valid], 1
.primary_done:
    mov eax, [nfs_part_blocks]
    dec eax
    mov edi, [nfs_leaf_page]
    call novafs_read_block
    jc .backup_done
    call novafs_superblock_validate
    jc .backup_done
    mov eax, [nfs_part_blocks]
    cmp [edi + NFS_SB_TOTAL], eax
    jne .backup_done
    mov dword [nfs_backup_valid], 1
.backup_done:
    mov esi, [nfs_root_page]
    cmp dword [nfs_primary_valid], 1
    je .have_primary
    cmp dword [nfs_backup_valid], 1
    jne .corrupt
    mov esi, [nfs_leaf_page]
    mov dword [nfs_used_backup], 1
    jmp .select
.have_primary:
    mov dword [nfs_used_backup], 0
    cmp dword [nfs_backup_valid], 1
    jne .select
    ; hoechste vollstaendig bestaetigte Generation gewinnt
    mov edi, [nfs_leaf_page]
    mov eax, [edi + NFS_SB_GENERATION + 4]
    cmp eax, [esi + NFS_SB_GENERATION + 4]
    ja .take_backup
    jb .select
    mov eax, [edi + NFS_SB_GENERATION]
    cmp eax, [esi + NFS_SB_GENERATION]
    jbe .select
.take_backup:
    mov esi, edi
    mov dword [nfs_used_backup], 1
.select:
    mov edi, nfs_sb
    mov ecx, NOVAFS_BLOCK_SIZE / 4
    rep movsd

    ; Feature-Flags (NPSPEC-NOVAFS-0001 §59)
    cmp dword [nfs_sb + NFS_SB_INCOMPAT + 4], 0
    jne .unsupported
    mov eax, [nfs_sb + NFS_SB_INCOMPAT]
    test eax, ~NOVAFS_INCOMPAT_KNOWN
    jnz .unsupported
    mov dword [nfs_readonly], 0
    mov eax, [nfs_sb + NFS_SB_ROCOMPAT]
    or eax, [nfs_sb + NFS_SB_ROCOMPAT + 4]
    jz .rocompat_ok
    mov dword [nfs_readonly], 1
.rocompat_ok:
    mov eax, [nfs_sb + NFS_SB_TOTAL]
    mov [nfs_total_blocks], eax
    mov [nfs_limit_blocks], eax
    mov eax, [nfs_sb + NFS_SB_BITMAP_BLOCKS]
    mov [nfs_bitmap_blocks], eax
    mov dword [nfs_unclean], 0
    cmp dword [nfs_sb + NFS_SB_STATE], NOVAFS_STATE_CLEAN
    je .state_ok
    ; DIRTY: Journal-Wiederherstellung versuchen (NPSPEC-NOVAFS-ONDISK-0001 §9).
    ; Gelingt sie, zeigt der primaere Superblock danach wieder genau den
    ; Zustand vor der abgebrochenen Aenderung (state = CLEAN).
    call novafs_journal_undo
    jc .dirty_unrecovered
    mov eax, 1
    mov edi, [nfs_leaf_page]
    call novafs_read_block
    jc .dirty_unrecovered
    call novafs_superblock_validate
    jc .dirty_unrecovered
    cmp dword [edi + NFS_SB_STATE], NOVAFS_STATE_CLEAN
    jne .dirty_unrecovered
    mov esi, edi
    mov edi, nfs_sb
    mov ecx, NOVAFS_BLOCK_SIZE / 4
    rep movsd
    mov eax, [nfs_sb + NFS_SB_TOTAL]
    mov [nfs_total_blocks], eax
    mov [nfs_limit_blocks], eax
    mov eax, [nfs_sb + NFS_SB_BITMAP_BLOCKS]
    mov [nfs_bitmap_blocks], eax
    mov esi, message_novafs_journal_recovered
    call serial_write_string
    jmp .state_ok
.dirty_unrecovered:
    mov dword [nfs_unclean], 1
    mov dword [nfs_readonly], 1
.state_ok:

    ; Prüfsummen-Array-Block aus dem Superblock lesen (0 = altes Volume, keine Prüfsummen)
    mov eax, [nfs_sb + NFS_SB_CHECKSUM_TREE]
    mov [nfs_checksum_block], eax

    ; Bitmap laden und gegen bitmap_crc32c pruefen
    xor ebx, ebx
    mov eax, 0xFFFFFFFF
.bitmap:
    cmp ebx, [nfs_bitmap_blocks]
    jae .bitmap_loaded
    cmp dword [nfs_bitmap_pages + ebx * 4], 0
    jne .bitmap_page
    push eax
    call pmm_alloc_page
    test eax, eax
    jz .bitmap_nopage
    mov [nfs_bitmap_pages + ebx * 4], eax
    pop eax
.bitmap_page:
    mov edi, [nfs_bitmap_pages + ebx * 4]
    push eax
    lea eax, [ebx + 2]
    call novafs_read_block
    pop eax
    jc .invalid
    mov esi, edi
    mov ecx, NOVAFS_BLOCK_SIZE
    call crc32c_update
    inc ebx
    jmp .bitmap
.bitmap_nopage:
    pop eax
    jmp .invalid
.bitmap_loaded:
    not eax
    cmp eax, [nfs_sb + NFS_SB_BITMAP_CRC]
    jne .corrupt
    ; Systembloecke muessen als belegt markiert sein
    mov eax, 1
    call novafs_block_used
    jnc .corrupt
    mov eax, [nfs_total_blocks]
    dec eax
    call novafs_block_used
    jnc .corrupt

    ; Baumwurzeln pruefen
    mov ebx, nfs_tree_object
    call novafs_check_root
    jc .invalid
    mov ebx, nfs_tree_directory
    call novafs_check_root
    jc .invalid
    mov ebx, nfs_tree_extent
    call novafs_check_root
    jc .invalid

    mov dword [nfs_mounted], 1
    ; Root-Verzeichnis (ObjectID 1)
    mov eax, 1
    call novafs_object_find
    jc .unmount
    cmp dword [nfs_object + NFS_O_TYPE], NOVAFS_TYPE_DIRECTORY
    jne .unmount_corrupt

    ; Read-Write-Mount: mount_count fortschreiben (eigene Generation)
    cmp dword [nfs_readonly], 0
    jne .done
    call novafs_change_begin
    jc .unmount
    add dword [nfs_sb + NFS_SB_MOUNT_COUNT], 1
    adc dword [nfs_sb + NFS_SB_MOUNT_COUNT + 4], 0
    call novafs_change_commit
    jc .unmount
.done:
    mov dword [nfs_error], NOVAFS_ERR_NONE
    popad
    clc
    ret
.unmount_corrupt:
    mov dword [nfs_error], NOVAFS_ERR_CORRUPT
.unmount:
    mov dword [nfs_mounted], 0
    popad
    stc
    ret
.unsupported:
    mov dword [nfs_error], NOVAFS_ERR_INVALID
    popad
    stc
    ret
.corrupt:
    mov dword [nfs_error], NOVAFS_ERR_CORRUPT
.invalid:
    popad
    stc
    ret

; EBX=Baum. Wurzel laden und Parent pruefen.
novafs_check_root:
    pushad
    mov ecx, [ebx + NFS_TD_ROOT]
    mov eax, [nfs_sb + ecx]
    mov edi, [nfs_root_page]
    mov edx, 0xFFFFFFFF
    call novafs_node_load
    jc .invalid
    mov eax, [edi + NFS_N_PARENT]
    or eax, [edi + NFS_N_PARENT + 4]
    jnz .invalid
    popad
    clc
    ret
.invalid:
    mov dword [nfs_error], NOVAFS_ERR_CORRUPT
    popad
    stc
    ret

; ---------------------------------------------------------------------------
; Systemintegration nach erfolgreichem Mount
; ---------------------------------------------------------------------------

; Prueft, dass die stabilen Wurzel-Namespaces dieselben ObjectIDs tragen wie
; der Semantic Core (OBJI:1 ... OBJI:7). CF bei Abweichung.
novafs_check_root_layout:
    pushad
    mov ebx, novafs_layout_table
.entry:
    movzx ecx, byte [ebx]
    test ecx, ecx
    jz .done
    lea esi, [ebx + 2]
    mov eax, 1
    push ebx
    push ecx
    call novafs_lookup
    pop ecx
    pop ebx
    jc .invalid
    movzx edi, byte [ebx + 1]
    cmp eax, edi
    jne .invalid
    cmp edx, NOVAFS_TYPE_DIRECTORY
    jne .invalid
    lea ebx, [ebx + ecx + 2]
    jmp .entry
.done:
    popad
    clc
    ret
.invalid:
    popad
    stc
    ret

; Registriert das Volume im Semantic Core, stellt das VFS-Root um und meldet
; SystemRoot an die Boot-Health-Autoritaet.
novafs_publish_root:
    ; VolumeID: erste 32 Bit der filesystem_uuid (0/1 sind reserviert)
    mov edx, [nfs_sb + NFS_SB_UUID]
    cmp edx, 2
    jae .volume_id
    add edx, 2
.volume_id:
    mov [nfs_volume_id_low], edx
    mov eax, NOVA_VOLUME_FLAG_MOUNTED
    cmp dword [nfs_readonly], 0
    je .flags
    or eax, NOVA_VOLUME_FLAG_READONLY
.flags:
    push dword NOVA_VOLUME_FS_NOVAFS
    push eax
    mov eax, NOVA_ID_VOLUME_HIGH
    mov edx, [nfs_volume_id_low]
    mov ebx, NOVA_NAMESPACE_ROOT
    mov ecx, NOVA_ID_OBJECT_HIGH
    mov edi, 1
    call semantic_core_volume_add
    jc .invalid

    mov eax, VFS_FLAG_NOVAFS_ROOT
    cmp dword [nfs_readonly], 0
    je .vfs_flags
    or eax, VFS_FLAG_READ_ONLY
.vfs_flags:
    mov [vfs_state + VFS_FLAGS], eax
    mov [vfs_api + 24], eax

    mov edx, BOOT_HEALTH_MILESTONE_SYSTEM_ROOT_READY
    mov ecx, BOOT_HEALTH_PROVIDER_SYSTEM_ROOT
    mov ebx, BOOT_HEALTH_PROVIDER_READY
    cmp dword [nfs_readonly], 0
    je .report
    mov ebx, BOOT_HEALTH_PROVIDER_DEGRADED
.report:
    call boot_health_prepare_report
    mov eax, 1
    mov esi, boot_health_temp_report
    call boot_health_submit_report
    jc .invalid
    clc
    ret
.invalid:
    stc
    ret

; ---------------------------------------------------------------------------
; Selbsttest: persistenter Bootzaehler und Mehrblock-Muster
; ---------------------------------------------------------------------------
novafs_self_test:
    pushad
    cmp dword [nfs_readonly], 0
    jne .read_only
    call pmm_alloc_page
    test eax, eax
    jz .invalid
    mov [nfs_test_page], eax
    call novafs_change_begin
    jc .free_invalid

    ; /System/Diagnose
    mov esi, novafs_path_system
    mov ecx, novafs_path_system_end - novafs_path_system
    call novafs_resolve_path
    jc .free_invalid
    cmp eax, 2
    jne .free_invalid
    mov esi, novafs_name_diagnose
    mov ecx, novafs_name_diagnose_end - novafs_name_diagnose
    call novafs_lookup
    jnc .have_diagnose
    mov eax, 2
    mov esi, novafs_name_diagnose
    mov ecx, novafs_name_diagnose_end - novafs_name_diagnose
    mov edx, NOVAFS_TYPE_DIRECTORY
    call novafs_create
    jc .free_invalid
.have_diagnose:
    mov [nfs_test_dir], eax

    ; Bootzaehler lesen, erhoehen, schreiben, zuruecklesen
    mov eax, [nfs_test_dir]
    mov esi, novafs_name_bootcount
    mov ecx, novafs_name_bootcount_end - novafs_name_bootcount
    mov edx, NOVAFS_TYPE_FILE
    call novafs_open_or_create
    jc .free_invalid
    mov [nfs_test_file], eax
    mov edi, [nfs_test_page]
    mov dword [edi + 17], '0000'
    mov dword [edi + 21], '0000'
    xor edx, edx
    mov ecx, 64
    call novafs_read
    jc .free_invalid
    xor ebx, ebx
    test eax, eax
    jz .count_ready
    cmp eax, novafs_bootcount_template_end - novafs_bootcount_template
    jne .free_invalid
    mov esi, [nfs_test_page]
    add esi, 17
    call novafs_parse_hex32
    jc .free_invalid
    mov ebx, eax
.count_ready:
    inc ebx
    mov [nfs_test_count], ebx
    mov esi, novafs_bootcount_template
    mov edi, [nfs_test_page]
    mov ecx, novafs_bootcount_template_end - novafs_bootcount_template
    rep movsb
    mov edi, [nfs_test_page]
    add edi, 17
    mov eax, [nfs_test_count]
    call novafs_format_hex32
    mov eax, [nfs_test_file]
    xor edx, edx
    mov ecx, novafs_bootcount_template_end - novafs_bootcount_template
    mov esi, [nfs_test_page]
    call novafs_write
    jc .free_invalid
    mov edi, [nfs_test_page]
    mov dword [edi + 17], 0
    mov eax, [nfs_test_file]
    xor edx, edx
    mov ecx, 64
    call novafs_read
    jc .free_invalid
    mov esi, [nfs_test_page]
    add esi, 17
    call novafs_parse_hex32
    jc .free_invalid
    cmp eax, [nfs_test_count]
    jne .free_invalid

    ; Mehrblockdatei: 3 volle Bloecke + 100 Byte, danach ein ueber eine
    ; Blockgrenze laufender Teilschreibzugriff. Inhalt haengt vom Bootzaehler ab.
    mov eax, [nfs_test_dir]
    mov esi, novafs_name_pattern
    mov ecx, novafs_name_pattern_end - novafs_name_pattern
    mov edx, NOVAFS_TYPE_FILE
    call novafs_open_or_create
    jc .free_invalid
    mov [nfs_test_file], eax
    xor ebx, ebx                    ; Chunk-Index
.write_chunk:
    cmp ebx, 4
    jae .boundary_write
    mov eax, ebx
    call novafs_pattern_fill
    mov eax, [nfs_test_file]
    mov edx, ebx
    shl edx, 12
    mov ecx, NOVAFS_BLOCK_SIZE
    cmp ebx, 3
    jne .chunk_length
    mov ecx, 100
.chunk_length:
    mov esi, [nfs_test_page]
    call novafs_write
    jc .free_invalid
    inc ebx
    jmp .write_chunk
.boundary_write:
    ; 16 Byte ab Offset 8184 (Block 1/2) mit identischem Mustertext erneut
    mov eax, 1
    call novafs_pattern_fill
    mov esi, [nfs_test_page]
    add esi, 4088
    mov edi, nfs_boundary_bytes
    movsd
    movsd
    mov eax, 2
    call novafs_pattern_fill
    mov esi, [nfs_test_page]
    mov edi, nfs_boundary_bytes + 8
    movsd
    movsd
    mov eax, [nfs_test_file]
    mov edx, 8184
    mov ecx, 16
    mov esi, nfs_boundary_bytes
    call novafs_write
    jc .free_invalid
    call novafs_change_commit
    jc .free_invalid

    ; Zuruecklesen und vergleichen
    xor ebx, ebx
.verify_chunk:
    cmp ebx, 4
    jae .verified
    mov eax, [nfs_test_file]
    mov edx, ebx
    shl edx, 12
    mov ecx, NOVAFS_BLOCK_SIZE
    mov edi, [nfs_data_verify_page]
    call novafs_read
    jc .free_invalid
    mov ecx, NOVAFS_BLOCK_SIZE
    cmp ebx, 3
    jne .verify_length
    mov ecx, 100
.verify_length:
    cmp eax, ecx
    jne .free_invalid
    mov eax, ebx
    call novafs_pattern_fill
    mov esi, [nfs_test_page]
    mov edi, [nfs_data_verify_page]
    repe cmpsb
    jne .free_invalid
    inc ebx
    jmp .verify_chunk
.verified:
    ; Dateigroesse muss exakt 3*4096+100 Byte betragen
    mov eax, [nfs_test_file]
    call novafs_object_find
    jc .free_invalid
    cmp dword [nfs_object + NFS_O_SIZE], 3 * NOVAFS_BLOCK_SIZE + 100
    jne .free_invalid
    mov eax, [nfs_test_page]
    call pmm_free_page
    mov esi, message_novafs_bootcount
    call serial_write_string
    mov eax, [nfs_test_count]
    call serial_write_hex32
    mov esi, message_line_end
    call serial_write_string
    mov esi, message_novafs_rw_ok
    call serial_write_string
    popad
    clc
    ret
.read_only:
    mov esi, message_novafs_selftest_ro
    call serial_write_string
    popad
    clc
    ret
.free_invalid:
    mov eax, [nfs_test_page]
    call pmm_free_page
.invalid:
    popad
    stc
    ret

; EAX=Verzeichnis, ESI=Name, ECX=Laenge, EDX=Typ. EAX=ObjectID (bestehend
; oder neu erzeugt). CF bei Fehler oder Typkonflikt.
novafs_open_or_create:
    push edx
    push eax
    push esi
    push ecx
    call novafs_lookup
    jc .create
    add esp, 12
    pop ecx                          ; erwarteter Typ
    cmp edx, ecx
    jne .invalid
    clc
    ret
.create:
    pop ecx
    pop esi
    pop eax
    pop edx
    call novafs_create
    ret
.invalid:
    stc
    ret

; EAX=Chunk-Index. Fuellt nfs_test_page mit dem Muster des Chunks.
novafs_pattern_fill:
    pushad
    mov edi, [nfs_test_page]
    mov ebx, eax
    shl ebx, 10                      ; erstes DWORD des Chunks
    xor ecx, ecx
.dword:
    lea eax, [ebx + ecx]
    imul eax, eax, 0x9E3779B1
    xor eax, [nfs_test_count]
    mov [edi + ecx * 4], eax
    inc ecx
    cmp ecx, NOVAFS_BLOCK_SIZE / 4
    jb .dword
    popad
    ret

; ESI=8 Hexziffern -> EAX, CF bei ungueltiger Ziffer.
novafs_parse_hex32:
    push ecx
    push edx
    xor eax, eax
    mov ecx, 8
.digit:
    movzx edx, byte [esi]
    sub edx, '0'
    cmp edx, 9
    jbe .value
    sub edx, 'A' - '0'
    cmp edx, 5
    ja .invalid
    add edx, 10
.value:
    shl eax, 4
    or eax, edx
    inc esi
    loop .digit
    pop edx
    pop ecx
    clc
    ret
.invalid:
    pop edx
    pop ecx
    stc
    ret

; EAX=Wert, EDI=Ziel (8 Byte Hex)
novafs_format_hex32:
    pushad
    mov ebx, eax
    mov ecx, 8
.digit:
    rol ebx, 4
    mov eax, ebx
    and eax, 0x0F
    cmp al, 10
    jb .number
    add al, 'A' - 10
    jmp .store
.number:
    add al, '0'
.store:
    stosb
    loop .digit
    popad
    ret

; ---------------------------------------------------------------------------
; Bootphase Root Filesystem
; ---------------------------------------------------------------------------
novafs_initialize:
    call novafs_mount_system
    cmp dword [nfs_mounted], 1
    jne .done
    mov esi, message_novafs_mounted
    call serial_write_string
    mov eax, [nfs_sb + NFS_SB_GENERATION]
    call serial_write_hex32
    mov esi, message_novafs_volume
    call serial_write_string
    mov eax, [nfs_sb + NFS_SB_UUID]
    bswap eax
    call serial_write_hex32
    mov eax, [nfs_sb + NFS_SB_UUID + 4]
    bswap eax
    call serial_write_hex32
    mov esi, message_line_end
    call serial_write_string
    cmp dword [nfs_used_backup], 1
    jne .backup_reported
    mov esi, message_novafs_backup_used
    call serial_write_string
.backup_reported:
    cmp dword [nfs_unclean], 1
    jne .clean_reported
    mov esi, message_novafs_unclean
    call serial_write_string
.clean_reported:
    call novafs_check_root_layout
    jc .layout_failed
    mov esi, message_novafs_layout_ok
    call serial_write_string
    call novafs_self_test
    jc .selftest_failed
    call novafs_publish_root
    jc .publish_failed
    mov esi, message_novafs_root_ok
    call serial_write_string
.done:
    clc
    ret
.layout_failed:
    mov esi, message_novafs_layout_failed
    jmp .detach
.selftest_failed:
    mov esi, message_novafs_selftest_failed
    jmp .detach
.publish_failed:
    mov esi, message_novafs_publish_failed
.detach:
    ; Fail-closed: kein halb gemountetes Systemvolume als Root anbieten.
    call serial_write_string
    mov eax, [nfs_error]
    call serial_write_hex32
    mov esi, message_line_end
    call serial_write_string
    mov dword [nfs_mounted], 0
    clc
    ret

; ---------------------------------------------------------------------------
; Daten
; ---------------------------------------------------------------------------
align 4
nfs_tree_object:     dd 2, NFS_O_ITEM, 26, 8, NFS_SB_OBJECT_TREE
nfs_tree_directory:  dd 3, NFS_D_ITEM, 13, 16, NFS_SB_DIRECTORY_TREE
nfs_tree_extent:     dd 4, NFS_E_ITEM, 55, 16, NFS_SB_EXTENT_TREE

novafs_api:
    dd 48
    dw 1, 0
    dd novafs_resolve_path
    dd novafs_lookup
    dd novafs_read
    dd novafs_write
    dd novafs_create
    dd novafs_change_begin
    dd novafs_change_commit
    dd nfs_mounted
    dd nfs_readonly
    dd 0

novafs_magic:            db "NOVAFS", 1, 0
novafs_path_system:      db "/System"
novafs_path_system_end:
novafs_name_diagnose:    db "Diagnose"
novafs_name_diagnose_end:
novafs_name_bootcount:   db "novafs-bootcount"
novafs_name_bootcount_end:
novafs_name_pattern:     db "novafs-muster.bin"
novafs_name_pattern_end:
novafs_bootcount_template: db "NOVAFS-BOOTCOUNT 00000000", 13, 10
novafs_bootcount_template_end:
; Laenge, ObjectID, Name
novafs_layout_table:
    db 6, 2, "System"
    db 8, 3, "Benutzer"
    db 4, 4, "Apps"
    db 7, 5, "Volumes"
    db 4, 6, "Boot"
    db 9, 7, "Solutions"
    db 0

align 4
nfs_mounted:          dd 0
nfs_readonly:         dd 1
nfs_unclean:          dd 0
nfs_used_backup:      dd 0
nfs_primary_valid:    dd 0
nfs_backup_valid:     dd 0
nfs_error:            dd 0
nfs_dev:              dd 0
nfs_lba:              dd 0
nfs_part_blocks:      dd 0
nfs_limit_blocks:     dd 0
nfs_total_blocks:     dd 0
nfs_bitmap_blocks:    dd 0
nfs_bitmap_dirty:     dd 0
nfs_bitmap_pages:     times NOVAFS_MAX_BITMAP_BLOCKS dd 0
nfs_alloc_start:      dd 0
nfs_alloc_result:     dd 0
nfs_write_count:      dd 0
nfs_dir_index:        dd 0
nfs_volume_id_low:    dd 0
; Transaction Log (Phase 2)
nfs_txn_open:              dd 0
nfs_journal_count:         dd 0
nfs_journal_entries:       times NOVAFS_JOURNAL_SLOTS dd 0
nfs_journal_undo_entries:  times NOVAFS_JOURNAL_SLOTS dd 0
nfs_journal_undo_count:    dd 0
nfs_journal_undo_saved_ro: dd 0
; PMM-Seiten (identitaetsabgebildet, DMA-tauglich)
nfs_page_slots:
nfs_root_page:        dd 0
nfs_inner1_page:      dd 0
nfs_inner2_page:      dd 0
nfs_leaf_page:        dd 0
nfs_aux_page:         dd 0
nfs_data_page:        dd 0
nfs_io_page:          dd 0
nfs_data_verify_page: dd 0
nfs_journal_header_page: dd 0
nfs_journal_slot_page:   dd 0
nfs_checksum_page:       dd 0
nfs_page_slots_end:
nfs_test_page:        dd 0
nfs_test_dir:         dd 0
nfs_test_file:        dd 0
nfs_test_count:       dd 0
nfs_cur_tree:         dd 0
nfs_cur_height:       dd 0
nfs_cur_child:        dd 0
nfs_cur_index:        dd 0
; §125 – Pfad-Stack für Baumhöhe > 2
nfs_path_depth:       dd 0
nfs_path_blocks:      times NFS_MAX_HEIGHT dd 0
nfs_path_indices:     times NFS_MAX_HEIGHT dd 0
; §125 – Snapshot-Zähler
nfs_snap_count:       dd 0
; §125 – CoW-Scratch
nfs_cow_old_phys:     dd 0
nfs_cow_new_phys:     dd 0
nfs_cow_log_blk:      dd 0
nfs_cow_object:       dd 0
nfs_ins_item:         dd 0
nfs_ins_count:        dd 0
nfs_ins_left:         dd 0
nfs_ins_moved:        dd 0
nfs_ins_right:        dd 0
nfs_ins_new_root:     dd 0
nfs_ins_parent:       dd 0
nfs_ins_leaf_block:   dd 0
nfs_name_ptr:         dd 0
nfs_name_length:      dd 0
nfs_create_parent:    dd 0
nfs_create_type:      dd 0
nfs_create_name:      dd 0
nfs_create_length:    dd 0
nfs_create_id:        dd 0
nfs_del_parent:       dd 0
nfs_del_name:         dd 0
nfs_del_length:       dd 0
nfs_del_object:       dd 0
nfs_del_type:         dd 0
nfs_del_physical:     dd 0
nfs_del_count:        dd 0
nfs_mv_src_parent:    dd 0
nfs_mv_src_name:      dd 0
nfs_mv_src_length:    dd 0
nfs_mv_dst_parent:    dd 0
nfs_mv_dst_name:      dd 0
nfs_mv_dst_length:    dd 0
nfs_mv_object:        dd 0
nfs_mv_type:          dd 0
nfs_map_object:       dd 0
nfs_map_block:        dd 0
nfs_map_result:       dd 0
nfs_prev_valid:       dd 0
nfs_prev_logical:     dd 0
nfs_prev_count:       dd 0
nfs_prev_physical:    dd 0
nfs_rw_object:        dd 0
nfs_rw_offset:        dd 0
nfs_rw_buffer:        dd 0
nfs_rw_remaining:     dd 0
nfs_rw_done:          dd 0
nfs_rw_end:           dd 0
nfs_rw_block:         dd 0
nfs_rw_physical:      dd 0
nfs_rw_chunk:         dd 0
nfs_rw_new_blocks:    dd 0
nfs_rtc_regb:         dd 0
nfs_rtc_sec:          dd 0
nfs_rtc_min:          dd 0
nfs_rtc_hour:         dd 0
nfs_rtc_day:          dd 0
nfs_rtc_month:        dd 0
nfs_rtc_year:         dd 0
nfs_rtc_century:      dd 0
nfs_rtc_check:        times 6 dd 0
nfs_rtc_days:         dd 0
nfs_crc_saved:        dd 0, 0
nfs_checksum_block:   dd 0
nfs_csum_blk:         dd 0
nfs_csum_crc:         dd 0
nfs_csum_ablock:      dd 0
nfs_csum_off:         dd 0
nfs_csum_data:        dd 0
nfs_key:              dd 0, 0, 0, 0
nfs_inner_temp:       times NOVAFS_INNER_ITEM db 0
nfs_boundary_bytes:   times 16 db 0
align 4
nfs_object:           times NFS_O_ITEM db 0
nfs_object_saved:     times NFS_O_ITEM db 0
nfs_item:             times NFS_D_ITEM db 0
align 16
nfs_sb:               times NOVAFS_BLOCK_SIZE db 0
