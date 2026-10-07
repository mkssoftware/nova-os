/*
 * NovaFS 1.0 Host-Werkzeug (Phase 1, NPSPEC-NOVAFS-ONDISK-0001)
 *
 *   novafs mkfs  <image> [--size-mib N] [--label NAME] [--uuid HEX32]
 *   novafs info  <image> [--gpt|--offset BYTES]
 *   novafs ls    <image> [--gpt|--offset BYTES] <path>
 *   novafs cat   <image> [--gpt|--offset BYTES] <path>
 *   novafs mkdir <image> [--gpt|--offset BYTES] <path>
 *   novafs put   <image> [--gpt|--offset BYTES] <hostfile> <path>
 *   novafs fsck  <image> [--gpt|--offset BYTES]
 *   novafs tree  <image> [--gpt|--offset BYTES]
 *   novafs mark-dirty <image> [--gpt|--offset BYTES]   (Testhilfe: Abbruch simulieren)
 *
 * Das Werkzeug ist bewusst eine unabhaengige Referenz zur Kernel-
 * Implementierung: Format, Baumalgorithmus und Schreibreihenfolge folgen
 * derselben Spezifikation, damit Host und Kernel sich gegenseitig pruefen.
 */
#define _FILE_OFFSET_BITS 64
#define _POSIX_C_SOURCE 200809L
#ifdef _WIN32
#define _CRT_RAND_S
#endif
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#define fseek64 _fseeki64
#define ftell64 _ftelli64
#else
#define fseek64 fseeko
#define ftell64 ftello
#endif

#define BLOCK_SIZE 4096u
#define NODE_HEADER_SIZE 80u
#define NODE_MAGIC 0x4552544Eu /* "NTRE" */

#define TREE_OBJECT 2u
#define TREE_DIRECTORY 3u
#define TREE_EXTENT 4u

#define OBJ_ITEM 152u
#define DIR_ITEM 288u
#define EXT_ITEM 72u
#define INNER_ITEM 24u

#define OBJ_MAX ((BLOCK_SIZE - NODE_HEADER_SIZE) / OBJ_ITEM)
#define DIR_MAX ((BLOCK_SIZE - NODE_HEADER_SIZE) / DIR_ITEM)
#define EXT_MAX ((BLOCK_SIZE - NODE_HEADER_SIZE) / EXT_ITEM)
#define INNER_MAX ((BLOCK_SIZE - NODE_HEADER_SIZE) / INNER_ITEM)

#define INCOMPAT_FREE_SPACE_BITMAP 1u
#define INCOMPAT_FIXED_ITEM_TREES 2u
#define INCOMPAT_KNOWN (INCOMPAT_FREE_SPACE_BITMAP | INCOMPAT_FIXED_ITEM_TREES)

#define STATE_CLEAN 0u
#define STATE_DIRTY 1u

#define TYPE_FILE 1u
#define TYPE_DIRECTORY 2u

#define OBJ_FLAG_SYSTEM 1u
#define OBJ_FLAG_NAMESPACE 2u

#define FIRST_DYNAMIC_OBJECT 256u

/* Superblock-Offsets */
#define SB_MAGIC 0
#define SB_VMAJ 8
#define SB_VMIN 10
#define SB_BLOCK_SIZE 12
#define SB_FS_SIZE 16
#define SB_TOTAL 24
#define SB_AVAILABLE 32
#define SB_GENERATION 40
#define SB_OBJECT_TREE 56
#define SB_DIRECTORY_TREE 64
#define SB_EXTENT_TREE 72
#define SB_FREE_SPACE 96
#define SB_FEATURE 120
#define SB_INCOMPAT 128
#define SB_ROCOMPAT 136
#define SB_UUID 144
#define SB_NAME 192
#define SB_CHECKSUM 352
#define SB_EXT_SIZE 384
#define SB_STATE 388
#define SB_NEXT_OBJECT 392
#define SB_BITMAP_BLOCKS 400
#define SB_OBJECT_COUNT 408
#define SB_BACKUP_BLOCK 416
#define SB_BITMAP_CRC 424
#define SB_CHECKSUM_TYPE 428
#define SB_MOUNT_COUNT 432

/* Node-Header */
#define N_MAGIC 0
#define N_LEVEL 4
#define N_COUNT 6
#define N_TREE 8
#define N_BLOCK 16
#define N_PARENT 24
#define N_GENERATION 32
#define N_CHECKSUM 40
#define N_ITEM_SIZE 72
#define N_MAX_ITEMS 74
#define N_KEY_SIZE 76

/* Objekt-Item */
#define O_ID 0
#define O_PARENT 8
#define O_TYPE 16
#define O_FLAGS 20
#define O_SIZE 24
#define O_ALLOCATED 32
#define O_PERMISSIONS 80
#define O_LINKS 84
#define O_GENERATION 112
#define O_CHECKSUM 120

/* Verzeichnis-Item */
#define D_PARENT 0
#define D_HASH 8
#define D_OBJECT 16
#define D_TYPE 24
#define D_NAME_LENGTH 28
#define D_FLAGS 30
#define D_NAME 32

/* Extent-Item */
#define E_OBJECT 0
#define E_LOGICAL 8
#define E_PHYSICAL 16
#define E_COUNT 24
#define E_UNCOMPRESSED 32
#define E_STORED 40
#define E_GENERATION 56

static const uint8_t NOVAFS_MAGIC[8] = {'N', 'O', 'V', 'A', 'F', 'S', 1, 0};

static void die(const char *format, ...) {
    va_list args;
    va_start(args, format);
    fputs("novafs: ", stderr);
    vfprintf(stderr, format, args);
    fputc('\n', stderr);
    va_end(args);
    exit(1);
}

static uint16_t r16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static uint32_t r32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static uint64_t r64(const uint8_t *p) { return (uint64_t)r32(p) | ((uint64_t)r32(p + 4) << 32); }
static void w16(uint8_t *p, uint16_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
static void w32(uint8_t *p, uint32_t v) {
    for (int i = 0; i < 4; ++i) p[i] = (uint8_t)(v >> (8 * i));
}
static void w64(uint8_t *p, uint64_t v) { w32(p, (uint32_t)v); w32(p + 4, (uint32_t)(v >> 32)); }

static uint32_t crc32c_table[256];
static void crc32c_init(void) {
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t c = i;
        for (int j = 0; j < 8; ++j) c = (c & 1) ? (c >> 1) ^ 0x82F63B78u : (c >> 1);
        crc32c_table[i] = c;
    }
}
static uint32_t crc32c(const uint8_t *data, size_t length) {
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < length; ++i) crc = crc32c_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    return crc ^ 0xFFFFFFFFu;
}
/* CRC32C ueber eine Struktur, deren 32-Byte-Pruefsummenfeld als 0 gilt. */
static uint32_t crc32c_without(const uint8_t *data, size_t length, size_t field) {
    uint8_t copy[BLOCK_SIZE];
    if (length > sizeof copy) die("interner Fehler: CRC-Puffer");
    memcpy(copy, data, length);
    memset(copy + field, 0, 32);
    return crc32c(copy, length);
}
static void seal(uint8_t *data, size_t length, size_t field) {
    memset(data + field, 0, 32);
    w32(data + field, crc32c(data, length));
}
static int sealed(const uint8_t *data, size_t length, size_t field) {
    for (int i = 4; i < 32; ++i)
        if (data[field + i]) return 0;
    return r32(data + field) == crc32c_without(data, length, field);
}

typedef struct Volume {
    FILE *file;
    uint64_t base;          /* Byte-Offset der Partition im Image */
    uint64_t partition_blocks;
    uint8_t sb[BLOCK_SIZE];
    uint8_t *bitmap;
    uint64_t bitmap_blocks;
    int dirty_bitmap;
    int writable;
    int changed;
} Volume;

static void disk_read(Volume *v, uint64_t block, uint8_t *out) {
    if (fseek64(v->file, (int64_t)(v->base + block * BLOCK_SIZE), SEEK_SET) != 0 ||
        fread(out, 1, BLOCK_SIZE, v->file) != BLOCK_SIZE)
        die("Lesefehler in Block %llu", (unsigned long long)block);
}
static void disk_write(Volume *v, uint64_t block, const uint8_t *data) {
    if (!v->writable) die("interner Fehler: Schreibzugriff auf Read-only-Volume");
    if (fseek64(v->file, (int64_t)(v->base + block * BLOCK_SIZE), SEEK_SET) != 0 ||
        fwrite(data, 1, BLOCK_SIZE, v->file) != BLOCK_SIZE)
        die("Schreibfehler in Block %llu", (unsigned long long)block);
}

/* ---------------- Bitmap ---------------- */
static int bit_get(Volume *v, uint64_t b) { return (v->bitmap[b >> 3] >> (b & 7)) & 1; }
static void bit_set(Volume *v, uint64_t b, int on) {
    if (on) v->bitmap[b >> 3] |= (uint8_t)(1u << (b & 7));
    else v->bitmap[b >> 3] &= (uint8_t)~(1u << (b & 7));
    v->dirty_bitmap = 1;
}
static uint64_t total_blocks(Volume *v) { return r64(v->sb + SB_TOTAL); }
static uint64_t alloc_block(Volume *v, uint64_t hint) {
    uint64_t total = total_blocks(v);
    for (uint64_t pass = 0; pass < 2; ++pass) {
        uint64_t start = pass == 0 ? hint : 0;
        uint64_t end = pass == 0 ? total : hint;
        for (uint64_t b = start; b < end; ++b)
            if (!bit_get(v, b)) {
                bit_set(v, b, 1);
                w64(v->sb + SB_AVAILABLE, r64(v->sb + SB_AVAILABLE) - 1);
                return b;
            }
    }
    die("Volume voll");
    return 0;
}

/* ---------------- Baeume ---------------- */
typedef struct TreeInfo {
    uint32_t tree_id;
    uint32_t item_size;
    uint32_t max_items;
    uint32_t key_size;
    uint32_t sb_root;
} TreeInfo;

static const TreeInfo TREES[3] = {
    {TREE_OBJECT, OBJ_ITEM, OBJ_MAX, 8, SB_OBJECT_TREE},
    {TREE_DIRECTORY, DIR_ITEM, DIR_MAX, 16, SB_DIRECTORY_TREE},
    {TREE_EXTENT, EXT_ITEM, EXT_MAX, 16, SB_EXTENT_TREE},
};
static const TreeInfo *tree_info(uint32_t tree_id) {
    for (int i = 0; i < 3; ++i)
        if (TREES[i].tree_id == tree_id) return &TREES[i];
    die("unbekannter Baum %u", tree_id);
    return NULL;
}

typedef struct Key { uint64_t k1, k2; } Key;
static int key_cmp(Key a, Key b) {
    if (a.k1 != b.k1) return a.k1 < b.k1 ? -1 : 1;
    if (a.k2 != b.k2) return a.k2 < b.k2 ? -1 : 1;
    return 0;
}
static Key item_key(const TreeInfo *t, const uint8_t *item) {
    Key k = {r64(item), t->key_size == 16 ? r64(item + 8) : 0};
    return k;
}
static uint8_t *node_item(uint8_t *node, uint32_t size, uint32_t index) {
    return node + NODE_HEADER_SIZE + (size_t)index * size;
}

static void node_init(Volume *v, uint8_t *node, const TreeInfo *t, uint16_t level, uint64_t block,
                      uint64_t parent) {
    memset(node, 0, BLOCK_SIZE);
    w32(node + N_MAGIC, NODE_MAGIC);
    w16(node + N_LEVEL, level);
    w64(node + N_TREE, t->tree_id);
    w64(node + N_BLOCK, block);
    w64(node + N_PARENT, parent);
    w64(node + N_GENERATION, r64(v->sb + SB_GENERATION) + 1);
    w16(node + N_ITEM_SIZE, (uint16_t)(level ? INNER_ITEM : t->item_size));
    w16(node + N_MAX_ITEMS, (uint16_t)(level ? INNER_MAX : t->max_items));
    w16(node + N_KEY_SIZE, (uint16_t)t->key_size);
}

static void node_check(Volume *v, const uint8_t *node, const TreeInfo *t, uint64_t block, int want_level) {
    if (r32(node + N_MAGIC) != NODE_MAGIC) die("Knoten %llu: falsche Magic", (unsigned long long)block);
    if (!sealed(node, BLOCK_SIZE, N_CHECKSUM)) die("Knoten %llu: CRC32C ungueltig", (unsigned long long)block);
    if (r64(node + N_TREE) != t->tree_id) die("Knoten %llu: falsche tree_id", (unsigned long long)block);
    if (r64(node + N_BLOCK) != block) die("Knoten %llu: block_id passt nicht", (unsigned long long)block);
    unsigned level = r16(node + N_LEVEL);
    if (level > 1) die("Knoten %llu: Level %u nicht unterstuetzt", (unsigned long long)block, level);
    if (want_level >= 0 && (int)level != want_level) die("Knoten %llu: unerwartetes Level", (unsigned long long)block);
    uint32_t size = level ? INNER_ITEM : t->item_size;
    uint32_t max = level ? INNER_MAX : t->max_items;
    if (r16(node + N_ITEM_SIZE) != size || r16(node + N_MAX_ITEMS) != max || r16(node + N_KEY_SIZE) != t->key_size)
        die("Knoten %llu: Itemgeometrie ungueltig", (unsigned long long)block);
    if (r16(node + N_COUNT) > max) die("Knoten %llu: zu viele Items", (unsigned long long)block);
    (void)v;
}

static void node_read(Volume *v, const TreeInfo *t, uint64_t block, uint8_t *node, int want_level) {
    if (block < 2 || block >= total_blocks(v)) die("Baumverweis %llu ausserhalb", (unsigned long long)block);
    disk_read(v, block, node);
    node_check(v, node, t, block, want_level);
}
static void node_write(Volume *v, uint8_t *node) {
    w64(node + N_GENERATION, r64(v->sb + SB_GENERATION) + 1);
    seal(node, BLOCK_SIZE, N_CHECKSUM);
    disk_write(v, r64(node + N_BLOCK), node);
    v->changed = 1;
}

/* Cursor: Position des ersten Items >= Schluessel. */
typedef struct Cursor {
    const TreeInfo *t;
    uint8_t root[BLOCK_SIZE];
    uint8_t leaf[BLOCK_SIZE];
    int height;            /* 1 = Wurzel ist Blatt, 2 = innere Wurzel */
    uint32_t child;        /* Kindindex in der Wurzel bei height 2 */
    uint32_t index;        /* Itemindex im Blatt */
} Cursor;

static uint64_t inner_child(uint8_t *root, uint32_t i) { return r64(node_item(root, INNER_ITEM, i) + 16); }
static Key inner_key(uint8_t *root, uint32_t i) {
    uint8_t *it = node_item(root, INNER_ITEM, i);
    Key k = {r64(it), r64(it + 8)};
    return k;
}

static int cursor_load_leaf(Volume *v, Cursor *c) {
    if (c->height == 1) return c->child == 0;
    if (c->child >= r16(c->root + N_COUNT)) return 0;
    node_read(v, c->t, inner_child(c->root, c->child), c->leaf, 0);
    if (r64(c->leaf + N_PARENT) != r64(c->root + N_BLOCK)) die("Blatt mit falschem Parent");
    return 1;
}

static void cursor_seek(Volume *v, Cursor *c, const TreeInfo *t, Key key) {
    c->t = t;
    node_read(v, t, r64(v->sb + t->sb_root), c->root, -1);
    if (r64(c->root + N_PARENT) != 0) die("Wurzel mit Parent");
    if (r16(c->root + N_LEVEL) == 0) {
        c->height = 1;
        c->child = 0;
        memcpy(c->leaf, c->root, BLOCK_SIZE);
    } else {
        c->height = 2;
        uint32_t count = r16(c->root + N_COUNT);
        if (count == 0) die("leere innere Wurzel");
        c->child = 0;
        for (uint32_t i = 1; i < count; ++i)
            if (key_cmp(inner_key(c->root, i), key) < 0) c->child = i;
        cursor_load_leaf(v, c);
    }
    uint32_t count = r16(c->leaf + N_COUNT);
    c->index = 0;
    while (c->index < count && key_cmp(item_key(t, node_item(c->leaf, t->item_size, c->index)), key) < 0)
        c->index++;
}

/* Liefert das aktuelle Item oder NULL am Ende; wechselt bei Bedarf das Blatt. */
static uint8_t *cursor_item(Volume *v, Cursor *c) {
    for (;;) {
        if (c->index < r16(c->leaf + N_COUNT)) return node_item(c->leaf, c->t->item_size, c->index);
        if (c->height == 1) return NULL;
        c->child++;
        if (!cursor_load_leaf(v, c)) return NULL;
        c->index = 0;
    }
}

static void root_set(Volume *v, const TreeInfo *t, uint64_t block) { w64(v->sb + t->sb_root, block); }

static void tree_insert(Volume *v, const TreeInfo *t, const uint8_t *item) {
    Cursor c;
    Key key = item_key(t, item);
    cursor_seek(v, &c, t, key);
    uint32_t count = r16(c.leaf + N_COUNT);
    if (count < t->max_items) {
        uint8_t *at = node_item(c.leaf, t->item_size, c.index);
        memmove(at + t->item_size, at, (size_t)(count - c.index) * t->item_size);
        memcpy(at, item, t->item_size);
        w16(c.leaf + N_COUNT, (uint16_t)(count + 1));
        node_write(v, c.leaf);
        return;
    }
    /* Blatt voll: Items + neues Item haelftig auf zwei Blaetter verteilen. */
    uint8_t all[(OBJ_MAX > DIR_MAX ? (OBJ_MAX > EXT_MAX ? OBJ_MAX : EXT_MAX) : (DIR_MAX > EXT_MAX ? DIR_MAX : EXT_MAX)) + 1][DIR_ITEM];
    for (uint32_t i = 0, j = 0; i <= count; ++i) {
        if (i == c.index) memcpy(all[i], item, t->item_size);
        else memcpy(all[i], node_item(c.leaf, t->item_size, j++), t->item_size);
    }
    uint32_t total = count + 1, left = total / 2, right = total - left;
    if (c.height == 2 && r16(c.root + N_COUNT) >= INNER_MAX) die("Baum %u voll (Phase-1-Hoehe 2)", t->tree_id);

    uint64_t leaf_block = r64(c.leaf + N_BLOCK);
    uint64_t right_block = alloc_block(v, leaf_block);
    uint64_t root_block = r64(c.root + N_BLOCK);
    uint8_t right_node[BLOCK_SIZE];

    if (c.height == 1) {
        /* Wurzelblatt teilen: alte Wurzel wird linkes Blatt, neue innere Wurzel. */
        uint64_t new_root = alloc_block(v, leaf_block);
        node_init(v, c.leaf, t, 0, leaf_block, new_root);
        node_init(v, right_node, t, 0, right_block, new_root);
        for (uint32_t i = 0; i < left; ++i) memcpy(node_item(c.leaf, t->item_size, i), all[i], t->item_size);
        for (uint32_t i = 0; i < right; ++i) memcpy(node_item(right_node, t->item_size, i), all[left + i], t->item_size);
        w16(c.leaf + N_COUNT, (uint16_t)left);
        w16(right_node + N_COUNT, (uint16_t)right);
        uint8_t root[BLOCK_SIZE];
        node_init(v, root, t, 1, new_root, 0);
        Key k0 = item_key(t, all[0]), k1 = item_key(t, all[left]);
        uint8_t *e0 = node_item(root, INNER_ITEM, 0), *e1 = node_item(root, INNER_ITEM, 1);
        w64(e0, k0.k1); w64(e0 + 8, k0.k2); w64(e0 + 16, leaf_block);
        w64(e1, k1.k1); w64(e1 + 8, k1.k2); w64(e1 + 16, right_block);
        w16(root + N_COUNT, 2);
        node_write(v, right_node);
        node_write(v, c.leaf);
        node_write(v, root);
        root_set(v, t, new_root);
        return;
    }
    node_init(v, right_node, t, 0, right_block, root_block);
    uint16_t keep_level_count = (uint16_t)left;
    memset(c.leaf + NODE_HEADER_SIZE, 0, BLOCK_SIZE - NODE_HEADER_SIZE);
    for (uint32_t i = 0; i < left; ++i) memcpy(node_item(c.leaf, t->item_size, i), all[i], t->item_size);
    for (uint32_t i = 0; i < right; ++i) memcpy(node_item(right_node, t->item_size, i), all[left + i], t->item_size);
    w16(c.leaf + N_COUNT, keep_level_count);
    w16(right_node + N_COUNT, (uint16_t)right);
    uint32_t rcount = r16(c.root + N_COUNT);
    uint8_t *at = node_item(c.root, INNER_ITEM, c.child + 1);
    memmove(at + INNER_ITEM, at, (size_t)(rcount - c.child - 1) * INNER_ITEM);
    Key k = item_key(t, all[left]);
    w64(at, k.k1); w64(at + 8, k.k2); w64(at + 16, right_block);
    w16(c.root + N_COUNT, (uint16_t)(rcount + 1));
    node_write(v, right_node);
    node_write(v, c.leaf);
    node_write(v, c.root);
}

/* Exaktes Item suchen (ohne Namen); liefert 1 und kopiert den Cursor. */
static int tree_find_exact(Volume *v, const TreeInfo *t, Key key, Cursor *c) {
    cursor_seek(v, c, t, key);
    uint8_t *it = cursor_item(v, c);
    return it && key_cmp(item_key(t, it), key) == 0;
}
static void cursor_write_leaf(Volume *v, Cursor *c) { node_write(v, c->leaf); }

/* ---------------- Superblock / Mount ---------------- */
static int superblock_valid(const uint8_t *sb, uint64_t partition_blocks) {
    if (memcmp(sb + SB_MAGIC, NOVAFS_MAGIC, 8) != 0) return 0;
    if (r16(sb + SB_VMAJ) != 1 || r32(sb + SB_BLOCK_SIZE) != BLOCK_SIZE) return 0;
    if (!sealed(sb, BLOCK_SIZE, SB_CHECKSUM)) return 0;
    uint64_t total = r64(sb + SB_TOTAL);
    if (total < 16 || total > partition_blocks) return 0;
    if (r64(sb + SB_BACKUP_BLOCK) != total - 1) return 0;
    if (r64(sb + SB_FS_SIZE) != total * BLOCK_SIZE) return 0;
    uint64_t bitmap_blocks = (total + BLOCK_SIZE * 8 - 1) / (BLOCK_SIZE * 8);
    if (r64(sb + SB_BITMAP_BLOCKS) != bitmap_blocks || r64(sb + SB_FREE_SPACE) != 2) return 0;
    for (int i = 0; i < 3; ++i) {
        uint64_t b = r64(sb + TREES[i].sb_root);
        if (b < 2 + bitmap_blocks || b >= total - 1) return 0;
    }
    return 1;
}

static void bitmap_load(Volume *v) {
    v->bitmap_blocks = r64(v->sb + SB_BITMAP_BLOCKS);
    v->bitmap = calloc(v->bitmap_blocks, BLOCK_SIZE);
    if (!v->bitmap) die("kein Speicher");
    for (uint64_t i = 0; i < v->bitmap_blocks; ++i) disk_read(v, 2 + i, v->bitmap + i * BLOCK_SIZE);
    if (crc32c(v->bitmap, v->bitmap_blocks * BLOCK_SIZE) != r32(v->sb + SB_BITMAP_CRC))
        die("Bitmap-CRC32C ungueltig");
}

static uint64_t find_gpt_partition(FILE *f, uint64_t *blocks) {
    static const uint8_t type[16] = {0x41, 0x56, 0x4F, 0x4E, 0x53, 0x46, 0x59, 0x53,
                                     0x53, 0x54, 0x45, 0x4D, 0x30, 0x30, 0x30, 0x31};
    uint8_t header[512];
    if (fseek64(f, 512, SEEK_SET) != 0 || fread(header, 1, 512, f) != 512) die("GPT-Header fehlt");
    if (memcmp(header, "EFI PART", 8) != 0) die("kein GPT-Datentraeger");
    uint64_t entries = r64(header + 72);
    uint32_t count = r32(header + 80), size = r32(header + 84);
    uint8_t entry[128];
    for (uint32_t i = 0; i < count && i < 128; ++i) {
        if (fseek64(f, (int64_t)(entries * 512 + (uint64_t)i * size), SEEK_SET) != 0 || fread(entry, 1, 128, f) != 128)
            die("GPT-Eintraege unlesbar");
        if (memcmp(entry, type, 16) == 0) {
            uint64_t first = r64(entry + 32), last = r64(entry + 40);
            *blocks = (last - first + 1) * 512 / BLOCK_SIZE;
            return first * 512;
        }
    }
    die("keine NovaFS-Partition im GPT gefunden");
    return 0;
}

static void volume_open(Volume *v, const char *path, int writable, int gpt, uint64_t offset) {
    memset(v, 0, sizeof *v);
    v->file = fopen(path, writable ? "r+b" : "rb");
    if (!v->file) die("%s: %s", path, strerror(errno));
    v->writable = writable;
    if (gpt) {
        v->base = find_gpt_partition(v->file, &v->partition_blocks);
    } else {
        v->base = offset;
        fseek64(v->file, 0, SEEK_END);
        v->partition_blocks = ((uint64_t)ftell64(v->file) - offset) / BLOCK_SIZE;
    }
    uint8_t primary[BLOCK_SIZE], backup[BLOCK_SIZE];
    disk_read(v, 1, primary);
    disk_read(v, v->partition_blocks - 1, backup);
    int pv = superblock_valid(primary, v->partition_blocks);
    int bv = superblock_valid(backup, v->partition_blocks) && r64(backup + SB_TOTAL) == v->partition_blocks;
    if (!pv && !bv) die("kein gueltiger NovaFS-Superblock");
    if (pv && bv) memcpy(v->sb, r64(backup + SB_GENERATION) > r64(primary + SB_GENERATION) ? backup : primary, BLOCK_SIZE);
    else memcpy(v->sb, pv ? primary : backup, BLOCK_SIZE);
    if (!pv) fprintf(stderr, "novafs: Warnung: primaerer Superblock ungueltig, Backup verwendet\n");
    uint64_t incompat = r64(v->sb + SB_INCOMPAT);
    if (incompat & ~(uint64_t)INCOMPAT_KNOWN) die("unbekannte incompat_flags 0x%llx", (unsigned long long)incompat);
    if (writable && r64(v->sb + SB_ROCOMPAT)) die("unbekannte readonly_compat_flags, nur Read-only moeglich");
    if (writable && r32(v->sb + SB_STATE) != STATE_CLEAN) die("Volume ist nicht CLEAN (DIRTY), Schreiben verweigert");
    bitmap_load(v);
}

static void volume_commit(Volume *v) {
    if (v->dirty_bitmap) {
        for (uint64_t i = 0; i < v->bitmap_blocks; ++i) disk_write(v, 2 + i, v->bitmap + i * BLOCK_SIZE);
        w32(v->sb + SB_BITMAP_CRC, crc32c(v->bitmap, v->bitmap_blocks * BLOCK_SIZE));
        v->dirty_bitmap = 0;
    }
    w64(v->sb + SB_GENERATION, r64(v->sb + SB_GENERATION) + 1);
    w32(v->sb + SB_STATE, STATE_CLEAN);
    seal(v->sb, BLOCK_SIZE, SB_CHECKSUM);
    disk_write(v, 1, v->sb);
    disk_write(v, total_blocks(v) - 1, v->sb);
    fflush(v->file);
}

static void volume_close(Volume *v) {
    fclose(v->file);
    free(v->bitmap);
}

static void volume_begin(Volume *v) {
    if (r32(v->sb + SB_STATE) == STATE_DIRTY) return;
    w32(v->sb + SB_STATE, STATE_DIRTY);
    seal(v->sb, BLOCK_SIZE, SB_CHECKSUM);
    disk_write(v, 1, v->sb);
}

/* ---------------- Objekte, Verzeichnisse, Daten ---------------- */
static int object_get(Volume *v, uint64_t id, uint8_t *out) {
    Cursor c;
    Key k = {id, 0};
    if (!tree_find_exact(v, tree_info(TREE_OBJECT), k, &c)) return 0;
    uint8_t *it = cursor_item(v, &c);
    if (!sealed(it, OBJ_ITEM, O_CHECKSUM)) die("Objekt %llu: CRC32C ungueltig", (unsigned long long)id);
    memcpy(out, it, OBJ_ITEM);
    return 1;
}
static void object_put(Volume *v, const uint8_t *object) {
    Cursor c;
    Key k = {r64(object), 0};
    if (!tree_find_exact(v, tree_info(TREE_OBJECT), k, &c)) die("Objekt fehlt beim Aktualisieren");
    uint8_t *it = cursor_item(v, &c);
    memcpy(it, object, OBJ_ITEM);
    w64(it + O_GENERATION, r64(v->sb + SB_GENERATION) + 1);
    seal(it, OBJ_ITEM, O_CHECKSUM);
    cursor_write_leaf(v, &c);
}

static uint64_t dir_lookup(Volume *v, uint64_t parent, const char *name, size_t length, uint8_t *entry_out) {
    Cursor c;
    Key k = {parent, crc32c((const uint8_t *)name, length)};
    const TreeInfo *t = tree_info(TREE_DIRECTORY);
    cursor_seek(v, &c, t, k);
    for (uint8_t *it; (it = cursor_item(v, &c)) && key_cmp(item_key(t, it), k) == 0; c.index++) {
        if (r16(it + D_NAME_LENGTH) == length && memcmp(it + D_NAME, name, length) == 0) {
            if (entry_out) memcpy(entry_out, it, DIR_ITEM);
            return r64(it + D_OBJECT);
        }
    }
    return 0;
}

static uint64_t resolve(Volume *v, const char *path) {
    if (path[0] != '/') die("Pfad muss mit / beginnen: %s", path);
    uint64_t id = 1;
    const char *p = path;
    while (*p) {
        while (*p == '/') p++;
        if (!*p) break;
        const char *end = strchr(p, '/');
        size_t length = end ? (size_t)(end - p) : strlen(p);
        id = dir_lookup(v, id, p, length, NULL);
        if (!id) return 0;
        p += length;
    }
    return id;
}

static void validate_name(const char *name, size_t length) {
    if (length == 0 || length > 255) die("ungueltige Namenslaenge");
    if ((length == 1 && name[0] == '.') || (length == 2 && name[0] == '.' && name[1] == '.')) die("reservierter Name");
    for (size_t i = 0; i < length; ++i)
        if (name[i] == '/' || name[i] == 0) die("ungueltiges Zeichen im Namen");
}

static uint64_t create_object(Volume *v, uint64_t parent, const char *name, size_t length, uint32_t type,
                              uint32_t flags, uint64_t fixed_id) {
    validate_name(name, length);
    uint8_t parent_object[OBJ_ITEM];
    if (!object_get(v, parent, parent_object) || r32(parent_object + O_TYPE) != TYPE_DIRECTORY)
        die("Elternobjekt ist kein Verzeichnis");
    if (dir_lookup(v, parent, name, length, NULL)) die("Name existiert bereits: %.*s", (int)length, name);
    /* Erst nach erfolgreicher Validierung wird das Volume als DIRTY markiert,
       damit abgewiesene Operationen keinen unsauberen Zustand hinterlassen. */
    if (v->writable && fixed_id == 0) volume_begin(v);
    uint64_t id = fixed_id;
    if (!id) {
        id = r64(v->sb + SB_NEXT_OBJECT);
        w64(v->sb + SB_NEXT_OBJECT, id + 1);
    }
    uint8_t object[OBJ_ITEM] = {0};
    w64(object + O_ID, id);
    w64(object + O_PARENT, parent);
    w32(object + O_TYPE, type);
    w32(object + O_FLAGS, flags);
    w32(object + O_PERMISSIONS, type == TYPE_DIRECTORY ? 0755 : 0644);
    w32(object + O_LINKS, 1);
    w64(object + O_GENERATION, r64(v->sb + SB_GENERATION) + 1);
    seal(object, OBJ_ITEM, O_CHECKSUM);
    tree_insert(v, tree_info(TREE_OBJECT), object);
    uint8_t entry[DIR_ITEM] = {0};
    w64(entry + D_PARENT, parent);
    w64(entry + D_HASH, crc32c((const uint8_t *)name, length));
    w64(entry + D_OBJECT, id);
    w32(entry + D_TYPE, type);
    w16(entry + D_NAME_LENGTH, (uint16_t)length);
    memcpy(entry + D_NAME, name, length);
    tree_insert(v, tree_info(TREE_DIRECTORY), entry);
    w64(v->sb + SB_OBJECT_COUNT, r64(v->sb + SB_OBJECT_COUNT) + 1);
    return id;
}

/* Logischen Block auf physischen Block abbilden (0 = Loch). */
static uint64_t extent_map(Volume *v, uint64_t object, uint64_t logical_block) {
    Cursor c;
    const TreeInfo *t = tree_info(TREE_EXTENT);
    Key start = {object, 0};
    cursor_seek(v, &c, t, start);
    uint64_t found = 0;
    for (uint8_t *it; (it = cursor_item(v, &c)) && r64(it + E_OBJECT) == object; c.index++) {
        uint64_t off = r64(it + E_LOGICAL) / BLOCK_SIZE, n = r64(it + E_COUNT);
        if (logical_block >= off && logical_block < off + n) found = r64(it + E_PHYSICAL) + (logical_block - off);
        if (off > logical_block) break;
    }
    return found;
}

static void file_write(Volume *v, uint64_t id, const uint8_t *data, uint64_t length) {
    uint8_t object[OBJ_ITEM];
    if (!object_get(v, id, object) || r32(object + O_TYPE) != TYPE_FILE) die("kein Dateiobjekt");
    if (r64(object + O_SIZE) != 0) die("put unterstuetzt nur neue, leere Dateien");
    uint64_t blocks = (length + BLOCK_SIZE - 1) / BLOCK_SIZE;
    uint64_t done = 0;
    uint64_t hint = 2 + v->bitmap_blocks;
    while (done < blocks) {
        /* zusammenhaengenden freien Bereich suchen */
        uint64_t first = alloc_block(v, hint), count = 1;
        while (done + count < blocks && first + count < total_blocks(v) && !bit_get(v, first + count)) {
            bit_set(v, first + count, 1);
            w64(v->sb + SB_AVAILABLE, r64(v->sb + SB_AVAILABLE) - 1);
            count++;
        }
        for (uint64_t i = 0; i < count; ++i) {
            uint8_t block[BLOCK_SIZE] = {0};
            uint64_t off = (done + i) * BLOCK_SIZE;
            uint64_t n = length - off < BLOCK_SIZE ? length - off : BLOCK_SIZE;
            memcpy(block, data + off, n);
            disk_write(v, first + i, block);
        }
        uint8_t extent[EXT_ITEM] = {0};
        w64(extent + E_OBJECT, id);
        w64(extent + E_LOGICAL, done * BLOCK_SIZE);
        w64(extent + E_PHYSICAL, first);
        w64(extent + E_COUNT, count);
        w64(extent + E_UNCOMPRESSED, count * BLOCK_SIZE);
        w64(extent + E_STORED, count * BLOCK_SIZE);
        w64(extent + E_GENERATION, r64(v->sb + SB_GENERATION) + 1);
        tree_insert(v, tree_info(TREE_EXTENT), extent);
        done += count;
        hint = first + count;
    }
    w64(object + O_SIZE, length);
    w64(object + O_ALLOCATED, blocks * BLOCK_SIZE);
    object_put(v, object);
}

static uint8_t *file_read(Volume *v, uint64_t id, uint64_t *length_out) {
    uint8_t object[OBJ_ITEM];
    if (!object_get(v, id, object)) die("Objekt fehlt");
    if (r32(object + O_TYPE) != TYPE_FILE) die("kein Dateiobjekt");
    uint64_t length = r64(object + O_SIZE);
    uint8_t *out = calloc(1, length + 1);
    if (!out) die("kein Speicher");
    for (uint64_t b = 0; b * BLOCK_SIZE < length; ++b) {
        uint64_t phys = extent_map(v, id, b);
        if (!phys) continue;
        uint8_t block[BLOCK_SIZE];
        disk_read(v, phys, block);
        uint64_t n = length - b * BLOCK_SIZE < BLOCK_SIZE ? length - b * BLOCK_SIZE : BLOCK_SIZE;
        memcpy(out + b * BLOCK_SIZE, block, n);
    }
    *length_out = length;
    return out;
}

static void split_path(const char *path, char *parent, const char **name) {
    const char *slash = strrchr(path, '/');
    if (!slash || path[0] != '/' || !slash[1]) die("ungueltiger Pfad: %s", path);
    size_t n = (size_t)(slash - path);
    if (n == 0) strcpy(parent, "/");
    else { memcpy(parent, path, n); parent[n] = 0; }
    *name = slash + 1;
}

/* ---------------- mkfs ---------------- */
static void parse_uuid(const char *hex, uint8_t *out) {
    if (strlen(hex) != 32) die("--uuid erwartet 32 Hexziffern");
    for (int i = 0; i < 16; ++i) {
        unsigned value;
        if (sscanf(hex + 2 * i, "%2x", &value) != 1) die("ungueltige UUID");
        out[i] = (uint8_t)value;
    }
}

static void random_uuid(uint8_t *out) {
#ifdef _WIN32
    for (int i = 0; i < 16; i += 4) {
        unsigned int value = 0;
        if (rand_s(&value) != 0) die("rand_s fehlgeschlagen");
        memcpy(out + i, &value, 4);
    }
    out[6] = (uint8_t)((out[6] & 0x0F) | 0x40);
    out[8] = (uint8_t)((out[8] & 0x3F) | 0x80);
    return;
#endif
    FILE *r = fopen("/dev/urandom", "rb");
    if (r && fread(out, 1, 16, r) == 16) {
        fclose(r);
    } else {
        if (r) fclose(r);
        uint64_t seed = (uint64_t)time(NULL) ^ (uint64_t)(uintptr_t)out ^ ((uint64_t)clock() << 32);
        for (int i = 0; i < 16; ++i) {
            seed = seed * 6364136223846793005ull + 1442695040888963407ull;
            out[i] = (uint8_t)(seed >> 56);
        }
    }
    out[6] = (uint8_t)((out[6] & 0x0F) | 0x40);
    out[8] = (uint8_t)((out[8] & 0x3F) | 0x80);
}

static void cmd_mkfs(const char *path, uint64_t size_mib, const char *label, const char *uuid) {
    uint64_t total = size_mib * 1024 * 1024 / BLOCK_SIZE;
    if (total < 64) die("Volume zu klein");
    FILE *f = fopen(path, "w+b");
    if (!f) die("%s: %s", path, strerror(errno));
    Volume v;
    memset(&v, 0, sizeof v);
    v.file = f;
    v.writable = 1;
    v.partition_blocks = total;
    uint8_t zero[BLOCK_SIZE] = {0};
    for (uint64_t b = 0; b < total; ++b) disk_write(&v, b, zero);

    uint64_t bitmap_blocks = (total + BLOCK_SIZE * 8 - 1) / (BLOCK_SIZE * 8);
    v.bitmap_blocks = bitmap_blocks;
    v.bitmap = calloc(bitmap_blocks, BLOCK_SIZE);
    for (uint64_t b = total; b < bitmap_blocks * BLOCK_SIZE * 8; ++b) v.bitmap[b >> 3] |= (uint8_t)(1u << (b & 7));
    memcpy(v.sb + SB_MAGIC, NOVAFS_MAGIC, 8);
    w16(v.sb + SB_VMAJ, 1);
    w16(v.sb + SB_VMIN, 0);
    w32(v.sb + SB_BLOCK_SIZE, BLOCK_SIZE);
    w64(v.sb + SB_FS_SIZE, total * BLOCK_SIZE);
    w64(v.sb + SB_TOTAL, total);
    w64(v.sb + SB_AVAILABLE, total);
    w64(v.sb + SB_GENERATION, 0);
    w64(v.sb + SB_FREE_SPACE, 2);
    w64(v.sb + SB_INCOMPAT, INCOMPAT_KNOWN);
    if (uuid) parse_uuid(uuid, v.sb + SB_UUID);
    else random_uuid(v.sb + SB_UUID);
    size_t label_length = strlen(label);
    if (label_length > 127) die("Volume-Name zu lang");
    memcpy(v.sb + SB_NAME, label, label_length);
    w32(v.sb + SB_EXT_SIZE, 128);
    w32(v.sb + SB_STATE, STATE_CLEAN);
    w64(v.sb + SB_NEXT_OBJECT, FIRST_DYNAMIC_OBJECT);
    w64(v.sb + SB_BITMAP_BLOCKS, bitmap_blocks);
    w64(v.sb + SB_BACKUP_BLOCK, total - 1);
    w32(v.sb + SB_CHECKSUM_TYPE, 1);

    /* feste Bereiche reservieren */
    for (uint64_t b = 0; b < 2 + bitmap_blocks; ++b) { bit_set(&v, b, 1); w64(v.sb + SB_AVAILABLE, r64(v.sb + SB_AVAILABLE) - 1); }
    bit_set(&v, total - 1, 1);
    w64(v.sb + SB_AVAILABLE, r64(v.sb + SB_AVAILABLE) - 1);

    for (int i = 0; i < 3; ++i) {
        uint64_t block = alloc_block(&v, 2 + bitmap_blocks);
        uint8_t node[BLOCK_SIZE];
        node_init(&v, node, &TREES[i], 0, block, 0);
        node_write(&v, node);
        root_set(&v, &TREES[i], block);
    }
    /* Root-Objekt (ObjectID 1) ohne Verzeichniseintrag */
    uint8_t root[OBJ_ITEM] = {0};
    w64(root + O_ID, 1);
    w32(root + O_TYPE, TYPE_DIRECTORY);
    w32(root + O_FLAGS, OBJ_FLAG_SYSTEM | OBJ_FLAG_NAMESPACE);
    w32(root + O_PERMISSIONS, 0755);
    w32(root + O_LINKS, 1);
    w64(root + O_GENERATION, 1);
    seal(root, OBJ_ITEM, O_CHECKSUM);
    tree_insert(&v, tree_info(TREE_OBJECT), root);
    w64(v.sb + SB_OBJECT_COUNT, 1);
    static const struct { const char *name; uint64_t id; uint32_t flags; } layout[] = {
        {"System", 2, OBJ_FLAG_SYSTEM | OBJ_FLAG_NAMESPACE},
        {"Benutzer", 3, OBJ_FLAG_NAMESPACE},
        {"Apps", 4, OBJ_FLAG_NAMESPACE},
        {"Volumes", 5, OBJ_FLAG_NAMESPACE},
        {"Boot", 6, OBJ_FLAG_SYSTEM | OBJ_FLAG_NAMESPACE},
        {"Solutions", 7, OBJ_FLAG_NAMESPACE},
    };
    for (size_t i = 0; i < sizeof layout / sizeof layout[0]; ++i)
        create_object(&v, 1, layout[i].name, strlen(layout[i].name), TYPE_DIRECTORY, layout[i].flags, layout[i].id);
    v.dirty_bitmap = 1;
    volume_commit(&v);
    printf("NovaFS-Volume erstellt: %s (%llu Bloecke, %llu MiB)\n", path, (unsigned long long)total,
           (unsigned long long)size_mib);
    volume_close(&v);
}

/* ---------------- fsck ---------------- */
typedef struct FsckTree {
    uint64_t items;
    uint64_t nodes;
} FsckTree;

static uint8_t *fsck_used;
static void fsck_mark(Volume *v, uint64_t block, const char *what) {
    if (block >= total_blocks(v)) die("fsck: %s verweist ausserhalb (%llu)", what, (unsigned long long)block);
    if (fsck_used[block]) die("fsck: Block %llu doppelt belegt (%s)", (unsigned long long)block, what);
    fsck_used[block] = 1;
}

typedef void (*ItemVisitor)(Volume *v, const uint8_t *item, void *context);

static FsckTree fsck_tree(Volume *v, const TreeInfo *t, ItemVisitor visit, void *context) {
    FsckTree result = {0, 0};
    uint8_t root[BLOCK_SIZE], leaf[BLOCK_SIZE];
    uint64_t root_block = r64(v->sb + t->sb_root);
    node_read(v, t, root_block, root, -1);
    fsck_mark(v, root_block, "Baumwurzel");
    result.nodes++;
    Key previous = {0, 0};
    int have_previous = 0;
    uint32_t children = r16(root + N_LEVEL) ? r16(root + N_COUNT) : 1;
    if (r16(root + N_LEVEL) && children < 1) die("fsck: leere innere Wurzel");
    for (uint32_t child = 0; child < children; ++child) {
        uint8_t *node = root;
        if (r16(root + N_LEVEL)) {
            uint64_t block = inner_child(root, child);
            node_read(v, t, block, leaf, 0);
            fsck_mark(v, block, "Blatt");
            if (r64(leaf + N_PARENT) != root_block) die("fsck: Blatt %llu mit falschem Parent", (unsigned long long)block);
            result.nodes++;
            node = leaf;
            Key lower = inner_key(root, child);
            if (child > 0 && key_cmp(lower, inner_key(root, child - 1)) < 0) die("fsck: innere Schluessel unsortiert");
            for (uint32_t i = 0; i < r16(leaf + N_COUNT); ++i) {
                Key k = item_key(t, node_item(leaf, t->item_size, i));
                if (child > 0 && key_cmp(k, lower) < 0) die("fsck: Schluessel unter innerer Grenze");
                if (child + 1 < children && key_cmp(k, inner_key(root, child + 1)) > 0) die("fsck: Schluessel ueber naechster Grenze");
            }
        }
        for (uint32_t i = 0; i < r16(node + N_COUNT); ++i) {
            const uint8_t *it = node_item(node, t->item_size, i);
            Key k = item_key(t, it);
            if (have_previous) {
                int cmp = key_cmp(previous, k);
                if (cmp > 0 || (cmp == 0 && t->tree_id != TREE_DIRECTORY)) die("fsck: Baum %u unsortiert oder doppelt", t->tree_id);
            }
            previous = k;
            have_previous = 1;
            result.items++;
            if (visit) visit(v, it, context);
        }
    }
    return result;
}

typedef struct FsckState {
    uint64_t max_object;
    uint64_t objects;
    uint64_t *ids;
    uint32_t *types;
    uint32_t *refs;
    size_t capacity;
} FsckState;

static size_t fsck_find(FsckState *s, uint64_t id) {
    size_t lo = 0, hi = s->objects;
    while (lo < hi) {
        size_t mid = (lo + hi) / 2;
        if (s->ids[mid] < id) lo = mid + 1; else hi = mid;
    }
    return lo < s->objects && s->ids[lo] == id ? lo : (size_t)-1;
}

static void visit_object(Volume *v, const uint8_t *it, void *context) {
    FsckState *s = context;
    (void)v;
    if (!sealed(it, OBJ_ITEM, O_CHECKSUM)) die("fsck: Objekt %llu CRC32C ungueltig", (unsigned long long)r64(it));
    uint32_t type = r32(it + O_TYPE);
    if (type != TYPE_FILE && type != TYPE_DIRECTORY) die("fsck: Objekt %llu unbekannter Typ", (unsigned long long)r64(it));
    if (s->objects == s->capacity) {
        s->capacity = s->capacity ? s->capacity * 2 : 64;
        s->ids = realloc(s->ids, s->capacity * sizeof *s->ids);
        s->types = realloc(s->types, s->capacity * sizeof *s->types);
        s->refs = realloc(s->refs, s->capacity * sizeof *s->refs);
    }
    s->ids[s->objects] = r64(it);
    s->types[s->objects] = type;
    s->refs[s->objects] = 0;
    s->objects++;
    if (r64(it) > s->max_object) s->max_object = r64(it);
}

static void visit_directory(Volume *v, const uint8_t *it, void *context) {
    FsckState *s = context;
    (void)v;
    uint16_t length = r16(it + D_NAME_LENGTH);
    if (length == 0 || length > 255) die("fsck: Namenslaenge ungueltig");
    if (r64(it + D_HASH) != crc32c(it + D_NAME, length)) die("fsck: Namens-Hash ungueltig");
    size_t parent = fsck_find(s, r64(it + D_PARENT)), child = fsck_find(s, r64(it + D_OBJECT));
    if (parent == (size_t)-1 || s->types[parent] != TYPE_DIRECTORY) die("fsck: Eintrag ohne Elternverzeichnis");
    if (child == (size_t)-1) die("fsck: Eintrag %.*s ohne Objekt", length, it + D_NAME);
    if (s->types[child] != r32(it + D_TYPE)) die("fsck: Eintragstyp passt nicht zum Objekt");
    s->refs[child]++;
}

typedef struct ExtentState {
    FsckState *objects;
    uint64_t last_object;
    uint64_t last_end;
} ExtentState;

static void visit_extent(Volume *v, const uint8_t *it, void *context) {
    ExtentState *e = context;
    uint64_t object = r64(it + E_OBJECT), logical = r64(it + E_LOGICAL), count = r64(it + E_COUNT);
    size_t index = fsck_find(e->objects, object);
    if (index == (size_t)-1 || e->objects->types[index] != TYPE_FILE) die("fsck: Extent ohne Dateiobjekt");
    if (logical % BLOCK_SIZE || count == 0) die("fsck: Extent-Geometrie ungueltig");
    if (object == e->last_object && logical < e->last_end) die("fsck: Extents ueberlappen");
    e->last_object = object;
    e->last_end = logical + count * BLOCK_SIZE;
    for (uint64_t b = 0; b < count; ++b) fsck_mark(v, r64(it + E_PHYSICAL) + b, "Extent");
}

static void cmd_fsck(Volume *v) {
    uint64_t total = total_blocks(v);
    fsck_used = calloc(total, 1);
    for (uint64_t b = 0; b < 2 + v->bitmap_blocks; ++b) fsck_mark(v, b, "Metadaten");
    fsck_mark(v, total - 1, "Backup-Superblock");
    FsckState state = {0};
    FsckTree objects = fsck_tree(v, tree_info(TREE_OBJECT), visit_object, &state);
    FsckTree dirs = fsck_tree(v, tree_info(TREE_DIRECTORY), visit_directory, &state);
    ExtentState extents_state = {&state, 0, 0};
    FsckTree extents = fsck_tree(v, tree_info(TREE_EXTENT), visit_extent, &extents_state);
    size_t root = fsck_find(&state, 1);
    if (root == (size_t)-1 || state.types[root] != TYPE_DIRECTORY) die("fsck: Root-Verzeichnis fehlt");
    for (size_t i = 0; i < state.objects; ++i) {
        uint32_t want = state.ids[i] == 1 ? 0 : 1;
        if (state.refs[i] != want) die("fsck: Objekt %llu hat %u Verzeichniseintraege", (unsigned long long)state.ids[i], state.refs[i]);
    }
    if (state.objects != r64(v->sb + SB_OBJECT_COUNT)) die("fsck: object_count passt nicht");
    if (state.max_object >= r64(v->sb + SB_NEXT_OBJECT) && state.max_object >= FIRST_DYNAMIC_OBJECT)
        die("fsck: next_object_id zu klein");
    uint64_t used = 0;
    for (uint64_t b = 0; b < total; ++b) {
        if (fsck_used[b] != bit_get(v, b)) die("fsck: Bitmap weicht bei Block %llu ab", (unsigned long long)b);
        used += fsck_used[b];
    }
    if (total - used != r64(v->sb + SB_AVAILABLE)) die("fsck: available_blocks passt nicht");
    /* Dateigroessen gegen Extents */
    printf("fsck: OK  Generation %llu, Objekte %llu, Verzeichniseintraege %llu, Extents %llu, Knoten %llu, belegt %llu/%llu\n",
           (unsigned long long)r64(v->sb + SB_GENERATION), (unsigned long long)objects.items,
           (unsigned long long)dirs.items, (unsigned long long)extents.items,
           (unsigned long long)(objects.nodes + dirs.nodes + extents.nodes), (unsigned long long)used,
           (unsigned long long)total);
    free(fsck_used);
    free(state.ids);
    free(state.types);
    free(state.refs);
}

static void cmd_info(Volume *v) {
    printf("NovaFS %u.%u  Generation %llu  Zustand %s\n", r16(v->sb + SB_VMAJ), r16(v->sb + SB_VMIN),
           (unsigned long long)r64(v->sb + SB_GENERATION), r32(v->sb + SB_STATE) == STATE_CLEAN ? "CLEAN" : "DIRTY");
    printf("VolumeID ");
    for (int i = 0; i < 16; ++i) printf("%02x", v->sb[SB_UUID + i]);
    printf("\nName %.*s\n", 128, (const char *)v->sb + SB_NAME);
    printf("Bloecke %llu, frei %llu, Objekte %llu, naechste ObjectID %llu, Mounts %llu\n",
           (unsigned long long)total_blocks(v), (unsigned long long)r64(v->sb + SB_AVAILABLE),
           (unsigned long long)r64(v->sb + SB_OBJECT_COUNT), (unsigned long long)r64(v->sb + SB_NEXT_OBJECT),
           (unsigned long long)r64(v->sb + SB_MOUNT_COUNT));
}

static void cmd_ls(Volume *v, const char *path) {
    uint64_t id = resolve(v, path);
    if (!id) die("nicht gefunden: %s", path);
    Cursor c;
    const TreeInfo *t = tree_info(TREE_DIRECTORY);
    Key k = {id, 0};
    cursor_seek(v, &c, t, k);
    for (uint8_t *it; (it = cursor_item(v, &c)) && r64(it + D_PARENT) == id; c.index++) {
        uint8_t object[OBJ_ITEM];
        uint64_t size = 0;
        if (object_get(v, r64(it + D_OBJECT), object)) size = r64(object + O_SIZE);
        printf("%s %8llu  %6llu  %.*s\n", r32(it + D_TYPE) == TYPE_DIRECTORY ? "d" : "-",
               (unsigned long long)size, (unsigned long long)r64(it + D_OBJECT), r16(it + D_NAME_LENGTH),
               (const char *)it + D_NAME);
    }
}

static void cmd_tree(Volume *v) {
    static const char *names[3] = {"Object", "Directory", "Extent"};
    for (int i = 0; i < 3; ++i) {
        const TreeInfo *t = &TREES[i];
        uint8_t root[BLOCK_SIZE], leaf[BLOCK_SIZE];
        node_read(v, t, r64(v->sb + t->sb_root), root, -1);
        printf("%-9s Hoehe %d:", names[i], r16(root + N_LEVEL) + 1);
        if (!r16(root + N_LEVEL)) printf(" %u", r16(root + N_COUNT));
        else
            for (uint32_t c = 0; c < r16(root + N_COUNT); ++c) {
                node_read(v, t, inner_child(root, c), leaf, 0);
                printf(" %u", r16(leaf + N_COUNT));
            }
        printf("\n");
    }
}

int main(int argc, char **argv) {
    crc32c_init();
    if (argc < 3) {
        fprintf(stderr, "Verwendung: novafs mkfs|info|ls|cat|mkdir|put|fsck|tree|mark-dirty <image> [Optionen] [Argumente]\n");
        return 2;
    }
    const char *command = argv[1], *image = argv[2];
    int gpt = 0;
    uint64_t offset = 0, size_mib = 32;
    const char *label = "NovaOS System", *uuid = NULL;
    const char *args[4];
    int nargs = 0;
    for (int i = 3; i < argc; ++i) {
        if (!strcmp(argv[i], "--gpt")) gpt = 1;
        else if (!strcmp(argv[i], "--offset") && i + 1 < argc) offset = strtoull(argv[++i], NULL, 0);
        else if (!strcmp(argv[i], "--size-mib") && i + 1 < argc) size_mib = strtoull(argv[++i], NULL, 0);
        else if (!strcmp(argv[i], "--label") && i + 1 < argc) label = argv[++i];
        else if (!strcmp(argv[i], "--uuid") && i + 1 < argc) uuid = argv[++i];
        else if (nargs < 4) args[nargs++] = argv[i];
        else die("zu viele Argumente");
    }
    if (!strcmp(command, "mkfs")) { cmd_mkfs(image, size_mib, label, uuid); return 0; }
    Volume v;
    if (!strcmp(command, "mark-dirty")) {
        /* Testhilfe: simuliert einen abgebrochenen Schreibvorgang */
        volume_open(&v, image, 1, gpt, offset);
        volume_begin(&v);
        fflush(v.file);
        volume_close(&v);
        return 0;
    }
    int writes = !strcmp(command, "put") || !strcmp(command, "mkdir");
    volume_open(&v, image, writes, gpt, offset);
    if (!strcmp(command, "info")) cmd_info(&v);
    else if (!strcmp(command, "fsck")) cmd_fsck(&v);
    else if (!strcmp(command, "tree")) cmd_tree(&v);
    else if (!strcmp(command, "ls")) { if (nargs != 1) die("ls <pfad>"); cmd_ls(&v, args[0]); }
    else if (!strcmp(command, "cat")) {
        if (nargs != 1) die("cat <pfad>");
        uint64_t id = resolve(&v, args[0]), length;
        if (!id) die("nicht gefunden: %s", args[0]);
        uint8_t *data = file_read(&v, id, &length);
        fwrite(data, 1, length, stdout);
        free(data);
    } else if (!strcmp(command, "mkdir") || !strcmp(command, "put")) {
        int is_put = !strcmp(command, "put");
        if (nargs != (is_put ? 2 : 1)) die(is_put ? "put <hostdatei> <pfad>" : "mkdir <pfad>");
        const char *target = args[is_put ? 1 : 0];
        char parent_path[4096];
        const char *name;
        if (strlen(target) >= sizeof parent_path) die("Pfad zu lang");
        split_path(target, parent_path, &name);
        uint64_t parent = resolve(&v, parent_path);
        if (!parent) die("Elternverzeichnis fehlt: %s", parent_path);
        uint8_t *data = NULL;
        long length = 0;
        if (is_put) {
            FILE *in = fopen(args[0], "rb");
            if (!in) die("%s: %s", args[0], strerror(errno));
            fseek(in, 0, SEEK_END);
            length = ftell(in);
            fseek(in, 0, SEEK_SET);
            data = malloc((size_t)length + 1);
            if (length && fread(data, 1, (size_t)length, in) != (size_t)length) die("Lesefehler %s", args[0]);
            fclose(in);
        }
        uint64_t id = create_object(&v, parent, name, strlen(name), is_put ? TYPE_FILE : TYPE_DIRECTORY, 0, 0);
        if (is_put && length) file_write(&v, id, data, (uint64_t)length);
        volume_commit(&v);
        free(data);
    } else die("unbekannter Befehl %s", command);
    volume_close(&v);
    return 0;
}
