/* NovaOS C-Kernel - Nova Capability Core: kanonische Rechte-Pruef-Engine
   fuer die Kernel-Capability-Konzepte dieses Kernels (023-CAPABILITY-
   Konsolidierung, Option 2 aus der Nutzerentscheidung: "neues cap_core.c
   als kanonische Schicht ueber object.c/handle.c, alte Module werden
   duenne Wrapper").

   AUSGANGSLAGE (vor dieser Aenderung, per Recherche-Fork ermittelt): fuenf
   bis sechs parallele Objekt-/Capability-Konzepte existieren in diesem
   Kernel, die GROSSTEILS bereits im alten ASM-Kernel so nebeneinander
   bestanden (siehe die jeweiligen eigenen "NICHT zu verwechseln mit..."-
   Kommentare in cap.h/uobj.h/object.h/nobj.h) - echte, beabsichtigte
   Trennung nach NPSPEC-Kapitel, kein versehentliches Chaos:
   - `object.c`/`handle.c` (memory32.inc, NPSPEC-KERNEL-0012/0013): der
     tatsaechlich breit genutzte Lifetime-/Rechte-Unterbau, Aufrufer u.a.
     component.c/ipc.c/process.c/threadmgr.c/semantic_core.c. BLEIBT
     UNVERAENDERT - cap_core.c baut konzeptionell darauf auf (ein
     cap_core-Capability KANN einen echten object.c-Handle als Subject
     tragen), erzwingt das aber nicht (siehe `subject`-Feld unten),
     da cap.c's Bootstrap-Capabilities (Kernel-Root/Boot-Kontext) keine
     echten Kernelobjekte sind, sondern abstrakte Policy-Marken.
   - `cap.c`/`cap_integration.c`/`uobj.c` (§102/§103, security32.inc/
     cap_integration32.inc): ein in sich geschlossenes Trio, das - anders
     als object.c/handle.c - NUR von kmain.c's Boot-Selbsttests ausgeuebt
     wird (null echte Laufzeit-Aufrufer ausserhalb der eigenen
     Selbsttests). `cap.c` und `uobj.c` werden durch diese Aenderung zu
     duennen Wrappern UEBER cap_core.c (siehe deren eigene Dateien);
     `cap_integration.c` bleibt unveraendert (ruft weiterhin cap.h/uobj.h
     auf, die Konsolidierung darunter ist fuer cap_integration.c
     transparent).
   - `program.c`s `cap_register`/`cap_check_permission`/`pkg_cap_revoke`:
     nachweislich toter Code (null Aufrufer ausserhalb von program.c
     selbst). Bleibt unveraendert, mit Verweis auf cap_core.c als
     kanonischen Mechanismus fuer kuenftigen echten Bedarf.
   - `nobj.c`: trotz des Namens KEIN Capability-System (Daten-/Stream-/
     Pipeline-Objekte, siehe dessen eigener Header-Kommentar) - zaehlt
     nicht zur Capability-Fragmentierung, bleibt unberuehrt.
   - `semantic_core.c`s `semantic_core_capability_*`: ein sechstes,
     eigenstaendiges Capability-Konzept mit eigenem 64-Bit-Objekt-ID-
     Schema (object_high/object_low), eingebettet in ein deutlich
     groesseres Namespace-/Registry-/Volume-Bootstrapsystem. Bewusst
     AUSSERHALB des Scopes dieser Konsolidierung - eine echte
     Zusammenfuehrung wuerde eine eigene, dedizierte Aenderung an diesem
     grossen Subsystem erfordern, ohne hier einen Nutzen fuer die (ebenso
     ungenutzten) semantic_core-Capability-Funktionen zu bringen. */
#ifndef NOVA_CAP_CORE_H
#define NOVA_CAP_CORE_H

#include <stdint.h>

#define CAP_CORE_MAX_CAPS 96u

#define CAP_CORE_STATE_FREE    0u
#define CAP_CORE_STATE_VALID   1u
#define CAP_CORE_STATE_REVOKED 2u

/* Identische Zahlenwerte wie cap.h's NP_ERR_CAP_* - cap.c kann sie 1:1
   durchreichen, ohne eine Uebersetzungstabelle zu brauchen. */
#define CAP_CORE_ERR_INVALID  (-60)
#define CAP_CORE_ERR_REVOKED  (-61)
#define CAP_CORE_ERR_EXPIRED  (-62)
#define CAP_CORE_ERR_EXHAUSTED (-63)
#define CAP_CORE_ERR_RIGHTS   (-64)
#define CAP_CORE_ERR_DELEGATE (-65)
#define CAP_CORE_ERR_DOMAIN   (-66)
#define CAP_CORE_ERR_POLICY   (-67)
#define CAP_CORE_ERR_STALE    (-68)
#define CAP_CORE_ERR_LIMIT    (-69)

void cap_core_initialize(void);
int cap_core_self_test(void); /* 0 = ok */

/* subject: ein opakes 32-Bit-Tag - KANN ein echter object.c-Handle sein
   (object_create()'s Rueckgabe), MUSS es aber nicht (cap.c's Bootstrap-
   Capabilities verwenden z.B. reine Policy-IDs ohne zugehoeriges
   Kernelobjekt). cap_core.c selbst interpretiert `rights` nicht - die
   Bit-Bedeutung legt ausschliesslich der Aufrufer fest (cap.h's
   NP_CAP_*, uobj.h's UOBJ_CAP_* und handle.h's HANDLE_RIGHT_* bleiben
   als eigene Vokabulare bestehen und muessen NICHT bitidentisch sein -
   der Vergleich (rights & required) == required funktioniert unabhaengig
   von der semantischen Bit-Zuordnung). 0 = Erfolg (*out_cap_id gesetzt),
   sonst CAP_CORE_ERR_* (negativ). delegable_rights MUSS Teilmenge von
   rights sein. */
int cap_core_create(uint32_t subject, uint32_t rights, uint32_t delegable_rights,
                     uint32_t domain, uint32_t *out_cap_id);

/* 0 = Erlaubt, CAP_CORE_ERR_* (negativ) bei Ablehnung (fail-closed). */
int cap_core_check(uint32_t cap_id, uint32_t required_rights);

/* 0 = Erfolg (*out_cap_id gesetzt), sonst CAP_CORE_ERR_*. Rechte duerfen
   nur abgeschwaecht werden (Teilmenge der Quelle). */
int cap_core_derive(uint32_t src_cap_id, uint32_t new_rights, uint32_t new_delegable,
                     uint32_t *out_cap_id);

int cap_core_revoke(uint32_t cap_id); /* 0 = Erfolg, CAP_CORE_ERR_* bei Fehler */

#endif
