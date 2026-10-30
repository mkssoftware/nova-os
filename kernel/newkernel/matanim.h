/* NovaOS C-Kernel - Nova Material Engine: Dynamik und Interaktion. Port
   aus docs/NPSPEC/sysarchitecture/049-GUI/010-Material_Engine/
   004-Dynamik_Interaktion/NPSPEC-MATERIAL-{ANIMATION,TRANSITION,
   INTERACTION,DEFORMATION}-0001.md.

   BEWUSSTE SCOPE-REDUKTION: ARCH-0001 Abschnitt 6 und ANIMATION-0001/
   TRANSITION-0001 verweisen auf ein "bestehendes NovaOS-Animationssystem"
   - ein solches eigenstaendiges System existiert in diesem Kernel nicht
   (kein animation.c, siehe Projektstatus). Dieses Modul implementiert
   daher einen eigenen, minimalen Tick-basierten Property-Tweener (lineare
   Interpolation, getaktet ueber interrupt.h's timer_get_ticks() - vom
   Aufrufer, typischerweise compositor_thread_body, einmal pro Frame via
   matanim_tick()/matanim_transition_tick()/matanim_deformation_tick()
   aufzurufen). Keine Keyframes/Ease-Kurven (nur linear) - fuer dezente
   UI-Uebergaenge ausreichend. DEFORMATION-0001 ("virtuelle
   Oberflaechenpunkte oder mathematische Funktionen") ist auf eine
   einzige mathematische Funktion reduziert: einen linear abklingenden
   Distortion-Impuls (Amplitude * Daempfung), der NICHT die
   Materialdefinition oder Instanz-Overrides mutiert (DEFORMATION-0001
   "urspruengliche Oberflaechenform bleibt als Referenz erhalten"),
   sondern rein additiv vom Renderer ueber matanim_deformation_bias()
   abgefragt und auf den resolvten Distortion-Wert aufaddiert wird. */
#ifndef NOVA_MATANIM_H
#define NOVA_MATANIM_H

#include <stdint.h>
#include "material.h"

void matanim_initialize(void);
int matanim_self_test(void); /* 0 = ok */

/* --- Animation (ANIMATION-0001) --- */
#define MATANIM_STATE_IDLE      0u
#define MATANIM_STATE_RUNNING   1u
#define MATANIM_STATE_PAUSED    2u
#define MATANIM_STATE_COMPLETED 3u

#define MATANIM_FLAG_REPEAT  (1u << 0) /* wiederholbar */
#define MATANIM_FLAG_REVERSE (1u << 1) /* Auto-Reverse nach jedem Durchlauf */

/* 0 = Fehler (Tabelle voll), sonst Animation-ID. Animiert field linear von
   from_value nach to_value ueber duration_ticks; mehrere Animationen
   koennen gleichzeitig auf verschiedene Felder derselben Instanz wirken
   (ANIMATION-0001 "Mehrere Eigenschaften koennen gleichzeitig animiert
   werden"). */
uint32_t matanim_start(uint32_t instance_id, uint32_t field,
                        uint32_t from_value, uint32_t to_value,
                        uint32_t duration_ticks, uint32_t flags, uint32_t now_tick);
int matanim_pause(uint32_t anim_id);
int matanim_resume(uint32_t anim_id, uint32_t now_tick);
int matanim_stop(uint32_t anim_id); /* haelt den zuletzt interpolierten Wert */
uint32_t matanim_state(uint32_t anim_id); /* 0xFFFFFFFF wenn unbekannt */
/* Interpoliert und schreibt alle laufenden Animationen fuer den
   gegebenen Tick (einmal pro Frame aufzurufen). */
void matanim_tick(uint32_t now_tick);

/* --- Transition (TRANSITION-0001) --- */
/* 0 = Fehler, sonst Transition-ID. Snapshotet den aktuellen resolvten
   Zustand der Instanz als Ausgangspunkt - ein erneuter Aufruf waehrend
   einer laufenden Transition macht sie dadurch strukturell umkehrbar
   (TRANSITION-0001 "unterbrechbar und umkehrbar"). */
uint32_t matanim_transition_start(uint32_t instance_id, uint32_t to_definition_id,
                                    uint32_t duration_ticks, uint32_t now_tick);
int matanim_transition_stop(uint32_t transition_id); /* bricht ab, Instanz bleibt im aktuellen Zwischenzustand */
int matanim_transition_is_running(uint32_t transition_id);
/* Interpoliert alle laufenden Uebergaenge; bei Abschluss wird die Instanz
   exakt auf die Zieldefinition gebunden und alle Overrides geleert
   (TRANSITION-0001 "Nach Abschluss muss ausschliesslich der definierte
   Zielzustand aktiv bleiben"). */
void matanim_transition_tick(uint32_t now_tick);

/* --- Interaction (INTERACTION-0001) --- */
#define MATANIM_EVENT_HOVER_ENTER 0u
#define MATANIM_EVENT_HOVER_LEAVE 1u
#define MATANIM_EVENT_PRESS_DOWN  2u
#define MATANIM_EVENT_PRESS_UP    3u
#define MATANIM_EVENT_FOCUS_GAIN  4u
#define MATANIM_EVENT_FOCUS_LOSE  5u
#define MATANIM_EVENT_DISABLE     6u
#define MATANIM_EVENT_ENABLE      7u

/* Bildet ein UI-Interaktionsereignis auf den Materialzustand ab
   (MATERIAL_STATE_*, material.c) und loest bei MATANIM_EVENT_PRESS_DOWN
   zusaetzlich einen Deformation-Impuls aus, sofern die zugrunde liegende
   Materialdefinition vom Typ MATERIAL_TYPE_LIQUID ist (einziges Material
   mit "fluessigem" Verformungsverhalten, siehe LIQUID-0001 Fluidity).
   0 = Erfolg, -1 wenn instance_id unbekannt. */
int matanim_handle_event(uint32_t instance_id, uint32_t event, uint32_t now_tick);

/* --- Deformation (DEFORMATION-0001) --- */
/* 0 = Fehler (Tabelle voll), sonst Deformation-ID. amplitude (0..255) klingt
   linear mit damping (0..255, hoeher = schnellerer Abfall) bis 0 ab. */
uint32_t matanim_deformation_trigger(uint32_t instance_id, uint32_t amplitude,
                                       uint32_t duration_ticks, uint32_t damping,
                                       uint32_t now_tick);
void matanim_deformation_tick(uint32_t now_tick); /* entfernt abgelaufene Eintraege */
/* Aktueller additiver Distortion-Bias (0..255) fuer diese Instanz - vom
   Renderer auf den resolvten Distortion-Wert aufzuaddieren (geclamped auf
   255). Summiert mehrere gleichzeitige Impulse derselben Instanz. */
uint32_t matanim_deformation_bias(uint32_t instance_id);

#endif
