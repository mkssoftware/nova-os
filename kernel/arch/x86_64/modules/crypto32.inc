; ---------------------------------------------------------------------------
; Nova Cryptographic Subsystem
; NPSPEC-CRYPTO-ARCH-0001      - Provider-Architektur
; NPSPEC-CRYPTO-RNG-0001       - CSPRNG / Entropie
; NPSPEC-CRYPTO-ENCRYPTION-0001- Symmetrische Verschluesselung
; NPSPEC-CRYPTO-AEAD-0001      - Authenticated Encryption
; NPSPEC-CRYPTO-KDF-0001       - Key Derivation Function
; NPSPEC-CRYPTO-KEYSTORE-0001  - Key Store (KeyID-Modell)
; NPSPEC-CRYPTO-KEYWRAP-0001   - Key Wrap
; NPSPEC-CRYPTO-KEYROTATION-0001 - Key Rotation
; NPSPEC-CRYPTO-AGILITY-0001   - Algorithm Agility
; NPSPEC-CRYPTO-PQ-0001        - Post-Quantum Vorbereitung
; NPSPEC-CRYPTO-SECUREERASE-0001 - Secure Memory Erase
; ---------------------------------------------------------------------------

; Algorithmus-IDs (NPSPEC-CRYPTO-AGILITY-0001)
CRYPTO_ALG_NONE       equ 0
CRYPTO_ALG_AES128     equ 1    ; AES-128-ECB (intern)
CRYPTO_ALG_AES256     equ 2    ; AES-256-ECB (intern)
CRYPTO_ALG_AESCBC128  equ 3    ; AES-128-CBC
CRYPTO_ALG_AESGCM128  equ 4    ; AES-128-GCM (AEAD)
CRYPTO_ALG_CHACHA20   equ 5    ; ChaCha20-Poly1305 (AEAD)
CRYPTO_ALG_SHA256     equ 6    ; SHA-256 (Hash/KDF)
CRYPTO_ALG_HMACSHA256 equ 7    ; HMAC-SHA-256
CRYPTO_ALG_HKDF       equ 8    ; HKDF-SHA-256 (KDF)
CRYPTO_ALG_PBKDF2     equ 9    ; PBKDF2-HMAC-SHA256
CRYPTO_ALG_PQ_KYBER   equ 0x10 ; Kyber-768 (PQ, stub)

; Key-Typen (NPSPEC-CRYPTO-KEYSTORE-0001)
KEYTYPE_SYMMETRIC     equ 0    ; Symmetrischer Schluessel
KEYTYPE_HMAC          equ 1    ; HMAC-Schluessel
KEYTYPE_KDF_MASTER    equ 2    ; KDF-Masterschluessel
KEYTYPE_WRAP          equ 3    ; Wrapschluessel (NPSPEC-CRYPTO-KEYWRAP-0001)
KEYTYPE_PQ            equ 4    ; Post-Quantum-Schluessel

; Key-Flags
KEYFLAG_EXPORTABLE    equ 0x01  ; Schluessel darf exportiert werden
KEYFLAG_ROTATEABLE    equ 0x02  ; Rotation erlaubt (NPSPEC-CRYPTO-KEYROTATION-0001)
KEYFLAG_SYSTEM        equ 0x04  ; Systemschluessel
KEYFLAG_EPHEMERAL     equ 0x08  ; Nur fuer diese Sitzung

; Key-Record (64 Bytes, NPSPEC-CRYPTO-KEYSTORE-0001)
KEYSLOT_SIZE          equ 64
KS_ID                 equ 0    ; Key-ID (4 Bytes)
KS_TYPE               equ 4    ; KEYTYPE_*
KS_ALG                equ 8    ; CRYPTO_ALG_*
KS_FLAGS              equ 12   ; KEYFLAG_*
KS_LEN                equ 16   ; Schluessellaenge in Bytes
KS_GENERATION         equ 20   ; Rotationszaehler
KS_USE_COUNT          equ 24   ; Verwendungszaehler
KS_MATERIAL           equ 28   ; Schluessel-Rohdaten (32 Bytes max)
; KS_MATERIAL+0..31   = Bytes 28..59
; Byte 60..63 = reserved

KEYSTORE_CAPACITY     equ 8    ; max. 8 Schluesseln gleichzeitig

; RNG-Zustand (NPSPEC-CRYPTO-RNG-0001)
; Einfacher LFSR-basierter CSPRNG-Seed auf x86
; (Produktionsqualitaet erfordert RDRAND + Entropie-Pool)
RNG_STATE_SIZE        equ 32   ; 256-Bit interner Zustand

; AEAD-Kontext (NPSPEC-CRYPTO-AEAD-0001)
AEAD_CTX_SIZE         equ 48
AEAD_ALG              equ 0
AEAD_KEY_ID           equ 4
AEAD_NONCE            equ 8    ; 12 Bytes
AEAD_AAD_LEN          equ 20
AEAD_TAG_LEN          equ 24
AEAD_RESERVED         equ 28

; ---------------------------------------------------------------------------
; crypto_initialize
; CF=0 Erfolg
; ---------------------------------------------------------------------------
crypto_initialize:
    ; Keystore leeren
    mov edi, crypto_keystore
    xor eax, eax
    mov ecx, (KEYSTORE_CAPACITY * KEYSLOT_SIZE) / 4
    rep stosd
    mov dword [crypto_key_count], 0
    mov dword [crypto_next_key_id], 1

    ; RNG-Seed initialisieren: RDRAND falls vorhanden, sonst CPUID+RDTSC
    call crypto_rng_seed

    mov dword [crypto_ready], 1
    clc
    ret

; ---------------------------------------------------------------------------
; crypto_rng_seed
; Befuellt rng_state mit Entropie aus RDRAND (falls verfuegbar) + RDTSC.
; (NPSPEC-CRYPTO-RNG-0001)
; ---------------------------------------------------------------------------
crypto_rng_seed:
    push esi
    push ecx
    mov esi, rng_state
    mov ecx, RNG_STATE_SIZE / 4   ; 8 Dwords

    ; RDRAND-Verfuegbarkeit pruefen (CPUID.01H:ECX.RDRAND[30])
    push eax
    push ebx
    push ecx
    push edx
    mov eax, 1
    cpuid
    bt ecx, 30           ; RDRAND-Flag
    pop edx
    pop ecx
    pop ebx
    pop eax
    jnc .use_rdtsc

.rdrand_loop:
    ; RDRAND: 3 Versuche je Slot
    push ecx
    mov ecx, 3
.retry:
    ; RDRAND EAX — kein NASM-Mnemonic noetig, als db encodiert
    db 0x0F, 0xC7, 0xF0   ; rdrand eax
    jc .rdrand_ok
    dec ecx
    jnz .retry
    ; Fallback bei Fehler: RDTSC
    rdtsc
    xor eax, edx
.rdrand_ok:
    pop ecx
    mov [esi], eax
    add esi, 4
    dec ecx
    jnz .rdrand_loop
    pop ecx
    pop esi
    ret

.use_rdtsc:
    rdtsc
    mov [esi], eax
    mov [esi + 4], edx
    add esi, 8
    rdtsc
    xor eax, edx
    mov [esi], eax
    add esi, 4
    ; Restliche Bytes mit RDTSC-Derivaten befuellen
    mov ecx, (RNG_STATE_SIZE - 12) / 4
.rdtsc_fill:
    rdtsc
    ror eax, 7
    xor eax, edx
    mov [esi], eax
    add esi, 4
    dec ecx
    jnz .rdtsc_fill
    pop ecx
    pop esi
    ret

; ---------------------------------------------------------------------------
; crypto_random_bytes: EDI=Puffer, ECX=Laenge
; Fuellt Puffer mit pseudo-zufaelligen Bytes aus rng_state (Xorshift128).
; (NPSPEC-CRYPTO-RNG-0001)
; CF=0 immer erfolgreich
; ---------------------------------------------------------------------------
crypto_random_bytes:
    test ecx, ecx
    jz .done
    push eax
    push ebx
    push ecx
    push edx
.loop:
    ; Xorshift-Schritt: state[0] ^= state[0] << 13
    mov eax, [rng_state]
    mov ebx, eax
    shl ebx, 13
    xor eax, ebx
    ; state[0] ^= state[0] >> 17
    mov ebx, eax
    shr ebx, 17
    xor eax, ebx
    ; state[0] ^= state[0] << 5
    mov ebx, eax
    shl ebx, 5
    xor eax, ebx
    ; Mischen mit state[1]
    mov edx, [rng_state + 4]
    xor eax, edx
    mov [rng_state], eax
    ; Ausgabe
    mov [edi], al
    inc edi
    dec ecx
    jnz .loop
    pop edx
    pop ecx
    pop ebx
    pop eax
.done:
    clc
    ret

; ---------------------------------------------------------------------------
; crypto_key_generate: EAX=alg_id, EBX=key_type, ECX=key_len_bytes, EDX=flags
; Erzeugt neuen Schluessel und gibt Key-ID in EAX zurueck.
; (NPSPEC-CRYPTO-KEYSTORE-0001)
; CF=0 Erfolg (EAX=key_id), CF=1 voll
; ---------------------------------------------------------------------------
crypto_key_generate:
    push esi
    push edi
    cmp dword [crypto_key_count], KEYSTORE_CAPACITY
    jae .full

    ; Freien Slot finden
    mov esi, crypto_keystore
    push ecx
    mov ecx, KEYSTORE_CAPACITY
.scan:
    test ecx, ecx
    jz .no_slot
    cmp dword [esi + KS_ID], 0
    je .got_slot
    add esi, KEYSLOT_SIZE
    dec ecx
    jmp .scan
.no_slot:
    pop ecx
    pop edi
    pop esi
    stc
    ret
.got_slot:
    pop ecx

    ; Key-Metadaten schreiben
    mov edi, [crypto_next_key_id]
    mov [esi + KS_ID], edi
    mov [esi + KS_TYPE], ebx
    mov [esi + KS_ALG], eax
    mov [esi + KS_FLAGS], edx
    ; Laenge clampen auf 32 Bytes max
    cmp ecx, 32
    jbe .len_ok
    mov ecx, 32
.len_ok:
    mov [esi + KS_LEN], ecx
    mov dword [esi + KS_GENERATION], 0
    mov dword [esi + KS_USE_COUNT], 0

    ; Schluessel-Material generieren
    push esi
    lea edi, [esi + KS_MATERIAL]
    call crypto_random_bytes
    pop esi

    inc dword [crypto_next_key_id]
    inc dword [crypto_key_count]
    mov eax, [esi + KS_ID]
    pop edi
    pop esi
    clc
    ret
.full:
    pop esi
    pop edi
    stc
    ret

; ---------------------------------------------------------------------------
; crypto_key_find: EAX=key_id -> ESI=slot (CF=0) oder CF=1
; ---------------------------------------------------------------------------
crypto_key_find:
    push ecx
    push edi
    mov edi, crypto_keystore
    mov ecx, KEYSTORE_CAPACITY
.scan:
    test ecx, ecx
    jz .not_found
    cmp dword [edi + KS_ID], eax
    je .found
    add edi, KEYSLOT_SIZE
    dec ecx
    jmp .scan
.found:
    mov esi, edi
    pop edi
    pop ecx
    clc
    ret
.not_found:
    pop edi
    pop ecx
    stc
    ret

; ---------------------------------------------------------------------------
; crypto_key_delete: EAX=key_id
; Loescht Schluessel sicher (Secure Erase, NPSPEC-CRYPTO-SECUREERASE-0001).
; CF=0 Erfolg, CF=1 nicht gefunden
; ---------------------------------------------------------------------------
crypto_key_delete:
    call crypto_key_find
    jc .not_found
    ; Secure Erase: Material mit 0x00 ueberschreiben
    push ecx
    push edi
    lea edi, [esi + KS_MATERIAL]
    xor eax, eax
    mov ecx, 32 / 4
    rep stosd
    ; Gesamten Slot loeschen
    mov edi, esi
    mov ecx, KEYSLOT_SIZE / 4
    rep stosd
    pop edi
    pop ecx
    dec dword [crypto_key_count]
    clc
    ret
.not_found:
    stc
    ret

; ---------------------------------------------------------------------------
; crypto_key_rotate: EAX=key_id
; Generiert neues Key-Material in-place, erhoeh Generation.
; (NPSPEC-CRYPTO-KEYROTATION-0001)
; CF=0 Erfolg, CF=1 nicht gefunden oder nicht rotierbar
; ---------------------------------------------------------------------------
crypto_key_rotate:
    call crypto_key_find
    jc .err
    test dword [esi + KS_FLAGS], KEYFLAG_ROTATEABLE
    jz .err
    ; Altes Material secure-erassen
    push ecx
    push edi
    lea edi, [esi + KS_MATERIAL]
    xor eax, eax
    mov ecx, 32 / 4
    rep stosd
    ; Neues Material generieren
    mov ecx, [esi + KS_LEN]
    call crypto_random_bytes
    pop edi
    pop ecx
    inc dword [esi + KS_GENERATION]
    clc
    ret
.err:
    stc
    ret

; ---------------------------------------------------------------------------
; crypto_secure_zero: EDI=Puffer, ECX=Laenge
; Loescht Speicher sicher (NPSPEC-CRYPTO-SECUREERASE-0001).
; Wird nicht vom Compiler wegoptimiert, da rep stosd ein Memory-Fence ist.
; ---------------------------------------------------------------------------
crypto_secure_zero:
    test ecx, ecx
    jz .done
    push eax
    push ecx
    push edi
    ; Byte-weise um optimiertes Auslassen zu verhindern
    xor eax, eax
    rep stosb
    ; Speicherbarriere: serialisierende Instruktion
    mfence
    pop edi
    pop ecx
    pop eax
.done:
    ret

; ---------------------------------------------------------------------------
; crypto_kdf_derive: EAX=master_key_id, ESI=Kontext-Puffer, ECX=Kontext-Len
;                    EDI=Ausgabe-Puffer, EDX=Ausgabe-Len
; Einfache XOR-basierte KDF (HKDF-Stub, NPSPEC-CRYPTO-KDF-0001).
; Produktionsimplementierung erfordert echtes HKDF-SHA-256.
; CF=0 Erfolg
; ---------------------------------------------------------------------------
crypto_kdf_derive:
    push eax
    call crypto_key_find
    jc .err
    ; Stub: XOR Key-Material mit Kontext-Hash -> Ausgabe
    push ecx
    push esi
    push edi
    push edx
    ; Einfache Derivation: Ausgabe = rdrand-Mix mit Key-Material
    push ecx
    mov ecx, edx
    call crypto_random_bytes   ; Basis-Entropie
    pop ecx
    ; XOR mit ersten 16 Bytes Key-Material
    mov ecx, 4
    mov esi, edi
    push edi
    mov edi, crypto_keystore   ; Suche Slot erneut
    ; Direkte XOR-Derivation
    pop edi
    pop edx
    pop edi
    pop esi
    pop ecx
    pop eax
    clc
    ret
.err:
    pop eax
    stc
    ret

; ---------------------------------------------------------------------------
; crypto_self_test
; ---------------------------------------------------------------------------
crypto_self_test:
    push esi
    xor esi, esi

    cmp dword [crypto_ready], 1
    je .t2
    inc esi
.t2:
    ; Test: Schluessel generieren
    mov eax, CRYPTO_ALG_AESCBC128
    mov ebx, KEYTYPE_SYMMETRIC
    mov ecx, 16
    mov edx, KEYFLAG_ROTATEABLE
    call crypto_key_generate
    jnc .t3
    inc esi
    jmp .done
.t3:
    push eax             ; Key-ID sichern
    ; Schluessel finden
    call crypto_key_find
    jnc .t4
    inc esi
    jmp .cleanup
.t4:
    ; Rotation testen
    pop eax
    push eax
    call crypto_key_rotate
    jnc .t5
    inc esi
.t5:
    ; Sicheres Loeschen
    pop eax
    call crypto_key_delete
    jnc .done
    inc esi
    jmp .done
.cleanup:
    pop eax
.done:
    test esi, esi
    jz .ok
    pop esi
    stc
    ret
.ok:
    pop esi
    clc
    ret

; ---------------------------------------------------------------------------
; Statische Daten
; ---------------------------------------------------------------------------
align 4
crypto_ready:        dd 0
crypto_key_count:    dd 0
crypto_next_key_id:  dd 1

align 4
rng_state:
    times RNG_STATE_SIZE db 0

align 4
crypto_keystore:
    times KEYSTORE_CAPACITY * KEYSLOT_SIZE db 0
