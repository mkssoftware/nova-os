/* NovaOS C-Kernel - Nova Display Server / Surface Protocol
   (049-GUI/011-Graphic_Engine/007-Display_Server-Surface_Protocol, Phase 7
   des Graphics-Engine-Vorhabens). Elf NPSPECs dieses Ordners: ARCH/
   PROTOCOL/SURFACE/BUFFER/COMMIT/SYNC/PRESENT/FEEDBACK/DIRECT/OVERLAY/
   SHARED/ZERO-COPY-0001 (zwoelf gezaehlt, ZERO-COPY und SHARED beide
   eigene Dateien).

   Modulname bewusst NICHT "display.c/.h" - dieser Kernel hat bereits ein
   einfaches, funktionierendes Framebuffer-/VBE-Modul (`display.c`) fuer
   die tatsaechliche Pixelausgabe. Dieses Modul implementiert die NPSPEC-
   PROTOKOLL-Schicht (Surface-/Buffer-Lebenszyklus, atomarer Commit,
   explizite Fence-Synchronisation, Presentation-Feedback) darueber, ohne
   `display.c` zu beruehren.

   NPSPEC-DISPLAY-PROTOCOL-0001 beschreibt eine echte IPC-basierte
   Kommunikation zwischen Ring-3-Clients und dem Display Server. Diese
   Phase implementiert die Protokoll-SEMANTIK als direkte Funktions-API
   (konsistent mit allen vorherigen Graphics-Engine-Phasen, die ebenfalls
   direkte APIs statt echter IPC-Transport-Verdrahtung sind) - die
   tatsaechliche Verdrahtung ueber den bereits bestehenden IPC-Neuentwurf
   (channel.c/rpc.c) waere eine eigene, spaetere Integrationsarbeit.

   ECHTER AUFBAU AUF Phase 1 UND Phase 5:
   - `surface_commit_*()` ist ein direkter Wrapper um `gfx_transaction_*()`
     (Phase 1) - ein Surface Commit IST eine Graphics State Transaction:
     Pending-Aenderungen sammeln, validieren, atomar mit neuer
     Zustandsversion veroeffentlichen. Exakt dasselbe Transaktionsmodell
     wie bei Scene Graph (Phase 2), hier auf Surfaces angewandt.
   - `disp_surface_set_damage()` ruft `damage_add()` (Phase 5) mit target_id=
     surface_id auf - Surface Damage Regions sind dieselbe Datenstruktur
     wie ueberall sonst im Graphics-Engine-Vorhaben.

   Eine zentrale, in mehreren NPSPECs wiederkehrende Unterscheidung wird
   hier ECHT (nicht nur dokumentiert) umgesetzt: "Commit-Annahme bedeutet
   NICHT automatisch Buffer-Verfuegbarkeit" (COMMIT-0001 §5, SYNC-0001 §5).
   `disp_surface_commit_finish()` gelingt UNABHAENGIG vom Signalisierungsstatus
   des Acquire Fence. Erst `present_submit()` (ein tatsaechlicher
   Konsument) prueft den Fence und schlaegt mit DISPPROTO_ERR_NOT_READY
   fehl, wenn er noch nicht signalisiert ist.

   Bewusst NICHT Teil dieser Phase (reine GPU-Hardware-/Multi-Prozess-
   Konzepte): Direct Scanout (DIRECT-0001), Hardware Overlay Management
   (OVERLAY-0001 - beide GPU-Plane-spezifisch), Zero-Copy Buffer Sharing
   (ZERO-COPY-0001 - setzt gemeinsamen GPU-Speicher voraus, dieser Kernel
   hat generische Platzhalter-Buffer ohne echten Speicher), Shared Surface
   Protocol (SHARED-0001 - echte Cross-Prozess-Freigabe ueber Capabilities,
   eine eigene, groessere Integrationsarbeit mit dem bereits bestehenden
   IPC-/Capability-Neuentwurf). */
#ifndef NOVA_DISPPROTO_H
#define NOVA_DISPPROTO_H

#include <stdint.h>
#include "renderengine.h" /* render_rect_t */

#define DISP_OUTPUT_CAPACITY  4u
#define DISP_BUFFER_CAPACITY  16u
#define DISP_SURFACE_CAPACITY 16u
#define DISP_FENCE_CAPACITY   16u
#define DISP_PRESENT_CAPACITY 16u

/* NPSPEC-DISPLAY-BUFFER-0001 §4 Lebenszyklus (Created/Available
   kollabieren bewusst - dieser Kernel modelliert keine separate
   Allokationsphase ohne sofortige Verfuegbarkeit). */
#define BUFFER_AVAILABLE 1u
#define BUFFER_IN_USE    2u
#define BUFFER_RELEASED  3u

/* NPSPEC-DISPLAY-SURFACE-0001 §4 Lebenszyklus. */
#define SURFACE_CREATED    1u
#define SURFACE_CONFIGURED 2u
#define SURFACE_ACTIVE     3u

/* NPSPEC-DISPLAY-PRESENT-0001 §4 / NPSPEC-DISPLAY-FEEDBACK-0001 §4. */
#define PRESENT_SUBMITTED 1u
#define PRESENT_PRESENTED 2u

#define DISPPROTO_ERR_NOT_FOUND     (-1)
#define DISPPROTO_ERR_CAPACITY      (-2)
#define DISPPROTO_ERR_INVALID_STATE (-3)
#define DISPPROTO_ERR_NOT_READY     (-4)
#define DISPPROTO_ERR_BUSY          (-5)

typedef struct {
    uint32_t buffer_id;
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint32_t state;
    uint32_t ref_count;
} buffer_info_t;

typedef struct {
    uint32_t surface_id;
    uint32_t owner_pid;
    uint32_t lifecycle_state;
    uint32_t committed_buffer_id; /* 0 = kein Buffer */
    uint32_t content_version;
} surface_info_t;

typedef struct {
    uint32_t presentation_id;
    uint32_t output_id;
    uint32_t surface_id;
    uint32_t status;
    uint32_t submit_time;
    uint32_t presentation_time; /* 0 solange nicht PRESENTED */
} present_info_t;

void dispproto_initialize(void);
int dispproto_self_test(void); /* 0 = ok */

int output_create(uint32_t *out_output_id);

int buffer_create(uint32_t width, uint32_t height, uint32_t format, uint32_t *out_buffer_id);
/* Nur erfolgreich, wenn ref_count==0 (#4 "Freigabe eines Client-Handles
   darf nicht zur vorzeitigen Freigabe eines noch verwendeten Buffers
   fuehren"). DISPPROTO_ERR_BUSY sonst. */
int buffer_destroy(uint32_t buffer_id);
int buffer_info(uint32_t buffer_id, buffer_info_t *out_info);

int fence_create(uint32_t *out_fence_id); /* unsignalisiert */
int fence_signal(uint32_t fence_id);
int fence_is_signaled(uint32_t fence_id, int *out_signaled);

int disp_surface_create(uint32_t owner_pid, uint32_t *out_surface_id);
/* Nur erlaubt, wenn der zugeordnete Buffer bereits freigegeben ist (sonst
   DISPPROTO_ERR_BUSY) - der letzte referenzierte Buffer wird dabei
   dereferenziert. */
int disp_surface_destroy(uint32_t surface_id);
int disp_surface_info(uint32_t surface_id, surface_info_t *out_info);

/* 1:1-Wrapper um gfx_transaction_begin()/_abort() (Phase 1). */
int disp_surface_commit_begin(uint32_t surface_id, uint32_t *out_tx_id);
int disp_surface_commit_abort(uint32_t tx_id);

/* Sammelt Pending-Aenderungen - erst mit einem erfolgreichen
   disp_surface_commit_finish() sichtbar (#5 SURFACE-0001 Pending/Committed
   State). acquire_fence_id==0 bedeutet "keine Synchronisation
   erforderlich". */
int disp_surface_attach_buffer(uint32_t tx_id, uint32_t surface_id, uint32_t buffer_id,
                           uint32_t acquire_fence_id);
/* Ruft `damage_add()` (Phase 5) fuer surface_id auf - echte
   Wiederverwendung, keine eigene Damage-Verwaltung. */
int disp_surface_set_damage(uint32_t tx_id, uint32_t surface_id, render_rect_t rect);

/* Veroeffentlicht die Pending-Aenderungen atomar (Wrapper um
   gfx_transaction_commit(), Phase 1): neue content_version, der vorherige
   committed Buffer wird dereferenziert, der neue referenziert - UNABHAENGIG
   davon, ob dessen Acquire Fence bereits signalisiert ist (siehe
   Dateikopf). 0 = Erfolg (*out_content_version gesetzt). */
int disp_surface_commit_finish(uint32_t tx_id, uint32_t *out_content_version);

/* Reicht surface_id's aktuellen committed Buffer zur Praesentation ein.
   Prueft den zugehoerigen Acquire Fence - DISPPROTO_ERR_NOT_READY, wenn
   er existiert und noch nicht signalisiert ist (#5 SYNC-0001 "Commit-
   Annahme != Fence signalisiert"). 0 = Erfolg (*out_presentation_id
   gesetzt, Status SUBMITTED). */
int present_submit(uint32_t output_id, uint32_t surface_id, uint32_t *out_presentation_id);

/* Bestaetigt eine tatsaechliche Praesentation (Status -> PRESENTED,
   presentation_time per timer_get_ticks() gesetzt) - getrennt von der
   bloßen Einreichung (#4 PRESENT-0001 "Ein Frame gilt erst dann als
   praesentiert, wenn die entsprechende Bestaetigung... vorliegt"). */
int present_mark_presented(uint32_t presentation_id);

int present_info(uint32_t presentation_id, present_info_t *out_info);

#endif
