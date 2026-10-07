; ---------------------------------------------------------------------------
; NovaFS 1.0 Phase 1 – Kernel-Implementierung (NPSPEC-NOVAFS-0001,
; NPSPEC-NOVAFS-ONDISK-0001)
;
; Superblock-Auswahl (primaer/Backup, hoechste Generation), CRC32C fuer alle
; Metadaten, Free-Space-Bitmap, Object/Directory/Extent Tree als B+-Baeume mit
; fester Itemgroesse (Hoehe <= 2), Pfadaufloesung, Lesen, Schreiben und
; Erzeugen. Aenderungen folgen der Phase-1-Schreibreihenfolge
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
    call novafs_write_block
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
    mov eax, [nfs_total_blocks]
    dec eax
    mov edi, nfs_sb
    call novafs_superblock_write_at
    jc .invalid
    mov eax, [nfs_dev]
    call storage_flush
    jc .invalid
    clc
    ret
.invalid:
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
    jmp novafs_write_block

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
    cmp dword [esi + NFS_SB_FREE_SPACE], 2
    jne .invalid
    cmp dword [esi + NFS_SB_FREE_SPACE + 4], 0
    jne .invalid
    lea edx, [eax + 2]                ; erster freier Bereich
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
    call novafs_write_block
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
; Cursor (ein globaler Cursor; Blatt in nfs_leaf_page, Wurzel in nfs_root_page)
; ---------------------------------------------------------------------------

; EBX=Baum. Positioniert auf das erste Item >= nfs_key. CF bei Fehler.
novafs_cursor_seek:
    pushad
    mov [nfs_cur_tree], ebx
    mov ecx, [ebx + NFS_TD_ROOT]
    mov eax, [nfs_sb + ecx]
    mov edi, [nfs_root_page]
    mov edx, 0xFFFFFFFF
    call novafs_node_load
    jc .invalid
    mov edi, [nfs_root_page]
    mov eax, [edi + NFS_N_PARENT]
    or eax, [edi + NFS_N_PARENT + 4]
    jnz .corrupt
    cmp word [edi + NFS_N_LEVEL], 0
    jne .inner_root
    mov dword [nfs_cur_height], 1
    mov dword [nfs_cur_child], 0
    mov esi, edi
    mov edi, [nfs_leaf_page]
    mov ecx, NOVAFS_BLOCK_SIZE / 4
    rep movsd
    jmp .position
.inner_root:
    mov dword [nfs_cur_height], 2
    movzx ecx, word [edi + NFS_N_COUNT]
    test ecx, ecx
    jz .corrupt
    mov dword [nfs_cur_child], 0
    mov edx, 1
.child_scan:
    cmp edx, ecx
    jae .child_found
    imul esi, edx, NOVAFS_INNER_ITEM
    lea esi, [edi + NOVAFS_NODE_HEADER + esi]
    call novafs_compare_inner
    cmp eax, 0xFFFFFFFF
    jne .child_found
    mov [nfs_cur_child], edx
    inc edx
    jmp .child_scan
.child_found:
    call novafs_cursor_load_leaf
    jc .invalid
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

; Laedt das Blatt nfs_cur_child der inneren Wurzel. CF am Ende oder bei Fehler.
novafs_cursor_load_leaf:
    pushad
    mov ebx, [nfs_cur_tree]
    mov esi, [nfs_root_page]
    movzx ecx, word [esi + NFS_N_COUNT]
    mov edx, [nfs_cur_child]
    cmp edx, ecx
    jae .end
    imul edx, edx, NOVAFS_INNER_ITEM
    cmp dword [esi + NOVAFS_NODE_HEADER + edx + 20], 0
    jne .corrupt
    mov eax, [esi + NOVAFS_NODE_HEADER + edx + 16]
    mov edi, [nfs_leaf_page]
    xor edx, edx
    call novafs_node_load
    jc .invalid
    mov eax, [esi + NFS_N_BLOCK]
    cmp [edi + NFS_N_PARENT], eax
    jne .corrupt
    cmp dword [edi + NFS_N_PARENT + 4], 0
    jne .corrupt
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

; ESI=aktuelles Item. Wechselt bei Bedarf ins naechste Blatt; CF am Ende.
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
    cmp dword [nfs_cur_height], 2
    jne .end
    inc dword [nfs_cur_child]
    call novafs_cursor_load_leaf
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
    ; Eintrag fuer das rechte Blatt hinter dem geteilten Kind einfuegen.
    mov esi, [nfs_aux_page]
    lea esi, [esi + NOVAFS_NODE_HEADER]
    mov edx, nfs_inner_temp
    mov eax, [nfs_ins_right]
    call novafs_inner_entry_from_item
    mov edi, [nfs_root_page]
    mov eax, [nfs_cur_child]
    inc eax
    mov esi, nfs_inner_temp
    mov ecx, NOVAFS_INNER_ITEM
    call novafs_node_insert_at
    call novafs_node_store
    jc .invalid
    popad
    clc
    ret

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
    ; Namensregeln: 1..255 Byte, kein '/', kein NUL, nicht "." oder ".."
    test ecx, ecx
    jz .invalid
    cmp ecx, 255
    ja .invalid
    xor ebx, ebx
.name_check:
    mov al, [esi + ebx]
    test al, al
    jz .invalid
    cmp al, '/'
    je .invalid
    inc ebx
    cmp ebx, ecx
    jb .name_check
    cmp byte [esi], '.'
    jne .name_ok
    cmp ecx, 1
    je .invalid
    cmp ecx, 2
    jne .name_ok
    cmp byte [esi + 1], '.'
    je .invalid
.name_ok:
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
    cmp dword [nfs_rw_chunk], NOVAFS_BLOCK_SIZE
    je .modify
    mov eax, [nfs_rw_physical]
    mov edi, [nfs_data_page]
    call novafs_read_block
    jc .invalid
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
    call novafs_write_block
    jc .invalid
    mov ecx, [nfs_rw_chunk]
    add [nfs_rw_buffer], ecx
    add [nfs_rw_offset], ecx
    sub [nfs_rw_remaining], ecx
    jmp .block
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
    mov dword [nfs_unclean], 0
    cmp dword [nfs_sb + NFS_SB_STATE], NOVAFS_STATE_CLEAN
    je .state_ok
    mov dword [nfs_unclean], 1
    mov dword [nfs_readonly], 1
.state_ok:
    mov eax, [nfs_sb + NFS_SB_TOTAL]
    mov [nfs_total_blocks], eax
    mov [nfs_limit_blocks], eax
    mov eax, [nfs_sb + NFS_SB_BITMAP_BLOCKS]
    mov [nfs_bitmap_blocks], eax

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
; der Semantic Core (OBJI:1 ... OBJI:6). CF bei Abweichung.
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
; PMM-Seiten (identitaetsabgebildet, DMA-tauglich)
nfs_page_slots:
nfs_root_page:        dd 0
nfs_leaf_page:        dd 0
nfs_aux_page:         dd 0
nfs_data_page:        dd 0
nfs_io_page:          dd 0
nfs_data_verify_page: dd 0
nfs_page_slots_end:
nfs_test_page:        dd 0
nfs_test_dir:         dd 0
nfs_test_file:        dd 0
nfs_test_count:       dd 0
nfs_cur_tree:         dd 0
nfs_cur_height:       dd 0
nfs_cur_child:        dd 0
nfs_cur_index:        dd 0
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
nfs_crc_saved:        dd 0, 0
nfs_key:              dd 0, 0, 0, 0
nfs_inner_temp:       times NOVAFS_INNER_ITEM db 0
nfs_boundary_bytes:   times 16 db 0
align 4
nfs_object:           times NFS_O_ITEM db 0
nfs_object_saved:     times NFS_O_ITEM db 0
nfs_item:             times NFS_D_ITEM db 0
align 16
nfs_sb:               times NOVAFS_BLOCK_SIZE db 0
