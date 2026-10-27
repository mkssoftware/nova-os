# NPSPEC-GRAPHICS-ARCH-0005 – Nova Graphics Resource Lifecycle

**Status:** Angenommen  
**Bereich:** Graphics / Ressourcenlebenszyklus  
**System:** NovaOS

## 1. Zweck

Nova Graphics Resource Lifecycle definiert den Lebenszyklus grafischer Ressourcen von ihrer Erstellung bis zur endgültigen Freigabe.

Ziel ist eine sichere, effiziente und deterministische Ressourcenverwaltung für CPU- und GPU-Rendering.

Die Architektur verhindert Speicherlecks, ungültige Ressourcenzugriffe und die vorzeitige Freigabe noch verwendeter Grafikressourcen.

## 2. Architektur

Die Ressourcenverwaltung erfolgt zentral über den Nova Graphics Resource Manager.

| Komponente | Aufgabe |
|---|---|
| Resource Manager | Verwaltung des Ressourcenlebenszyklus |
| Resource Registry | Registrierung und Identifikation |
| Graphics Memory Manager | Speicherzuweisung und Freigabe |
| Resource Ownership | Eigentümerschaft und Zugriffsrechte |
| Resource Synchronization | Überwachung laufender Zugriffe |
| Resource Migration | Übertragung zwischen Geräten und Backends |
| Resource Recovery | Wiederherstellung verlorener Ressourcen |

Die Verwaltung ist unabhängig vom konkreten Rendering-Backend.

## 3. Ressourcentypen

Der Lebenszyklus gilt insbesondere für:

- Graphics Buffers
- Textures
- Render Targets
- Depth- und Stencil-Buffer
- Shader Modules
- Graphics Pipelines
- Command Buffers
- Samplers
- Shared Surfaces
- Synchronization Objects

Jede Ressource besitzt eine eindeutige Identität innerhalb ihres Gültigkeitsbereichs.

## 4. Ressourcenmodell

Jede Grafikressource besitzt mindestens folgende Eigenschaften:

| Eigenschaft | Beschreibung |
|---|---|
| Resource ID | Eindeutige Ressourcenidentität |
| Resource Type | Ressourcentyp |
| Owner Context | Eigentümer und Sicherheitskontext |
| Device ID | Zugeordnetes Grafikgerät |
| Backend ID | Verwendetes Rendering-Backend |
| Resource State | Aktueller Lebenszykluszustand |
| Memory Allocation | Zugeordneter Speicher |
| Usage Flags | Zulässige Verwendungsarten |
| Access Rights | Zugriffsberechtigungen |
| Generation | Version der Ressourcenidentität |

Ressourcenhandles MÜSSEN gegen die aktuelle Generation geprüft werden.

Dadurch wird verhindert, dass veraltete Handles auf neu zugewiesene Ressourcen verweisen.

## 5. Lebenszykluszustände

| Zustand | Beschreibung |
|---|---|
| Created | Ressourcenobjekt angelegt |
| Allocating | Speicherzuweisung läuft |
| Ready | Ressource ist verwendbar |
| InUse | Ressource wird durch Grafikoperationen verwendet |
| PendingRelease | Freigabe angefordert |
| Lost | Ressource durch Geräte- oder Backendfehler verloren |
| Destroying | Ressourcenfreigabe läuft |
| Destroyed | Ressource vollständig freigegeben |

Zulässige Zustandsübergänge werden durch den Resource Manager kontrolliert.

Eine Ressource im Zustand `Destroyed` DARF nicht erneut verwendet werden.

Eine verlorene Ressource MUSS neu erstellt oder durch eine gültige Ersatzressource ersetzt werden.

## 6. Ressourcenerstellung

Die Erstellung erfolgt in folgenden Schritten:

1. Ressourcenanforderung empfangen.
2. Parameter und Berechtigungen prüfen.
3. Geeignetes Gerät und Backend bestimmen.
4. Ressourcenidentität erzeugen.
5. Speicher zuweisen.
6. Backendressource initialisieren.
7. Ressource registrieren.
8. Gültiges Ressourcenhandle bereitstellen.

Schlägt die Erstellung fehl, MÜSSEN bereits reservierte Ressourcen kontrolliert freigegeben werden.

Unvollständig initialisierte Ressourcen DÜRFEN nicht als verwendbar veröffentlicht werden.

## 7. Ressourcenverwendung

Grafikressourcen können durch CPU, GPU oder mehrere autorisierte Komponenten verwendet werden.

Dabei gilt:

- Jeder Zugriff MUSS über gültige Ressourcenhandles erfolgen.
- Zugriffsrechte MÜSSEN vor der Verwendung geprüft werden.
- Ressourcenabhängigkeiten MÜSSEN eindeutig definiert sein.
- Gleichzeitige Zugriffe MÜSSEN korrekt synchronisiert werden.
- Laufende GPU-Operationen MÜSSEN bei der Ressourcenfreigabe berücksichtigt werden.

Gemeinsam verwendete Ressourcen benötigen ausdrücklich definierte Freigabe- und Synchronisationsregeln.

## 8. Ressourcenfreigabe

Die Freigabe erfolgt kontrolliert:

1. Freigabeanforderung empfangen.
2. Ressource für neue Verwendungen sperren.
3. Ausstehende Grafikoperationen ermitteln.
4. Erforderliche Synchronisation durchführen.
5. Backendressourcen freigeben.
6. Speicherzuweisungen aufheben.
7. Ressourcenregistrierung entfernen.
8. Handle und Generation invalidieren.

Eine Freigabe DARF nicht erfolgen, solange die Ressource noch durch laufende Operationen benötigt wird.

Der Resource Manager MUSS verzögerte Freigaben unterstützen.

## 9. Ressourcenbesitz und gemeinsame Nutzung

Jede Ressource besitzt einen definierten Eigentümer.

Gemeinsame Nutzung erfolgt ausschließlich über autorisierte Freigaben.

Unterstützt werden:

- Exklusiver Ressourcenbesitz.
- Kontrollierte gemeinsame Nutzung.
- Schreibgeschützte Ressourcenfreigaben.
- Ressourcenübertragung zwischen autorisierten Komponenten.
- Gemeinsame CPU-GPU-Speicherbereiche.

Die Freigabe eines Ressourcenhandles überträgt nicht automatisch dessen Eigentümerschaft.

Beim Beenden eines Prozesses MÜSSEN dessen Ressourcen kontrolliert bereinigt werden.

Gemeinsam genutzte Ressourcen bleiben nur bestehen, solange gültige Eigentums- oder Nutzungsbeziehungen dies erfordern.

## 10. Ressourcenmigration

Ressourcen können bei Bedarf zwischen Grafikgeräten oder Rendering-Backends übertragen werden.

Mögliche Auslöser:

- Backendwechsel.
- GPU-Gerätewechsel.
- Speicheroptimierung.
- Energieverwaltung.
- Geräteverlust.

Eine Migration MUSS die Ressourceninhalte und Zugriffsrechte erhalten, soweit dies technisch möglich ist.

Nicht migrierbare Ressourcen MÜSSEN ausdrücklich invalidiert werden.

Die betroffenen Grafikclients werden über erforderliche Neuerstellungen informiert.

## 11. Normative Anforderungen

- GRAPHICS-001: Jede Grafikressource MUSS einen definierten Lebenszyklus besitzen.
- GRAPHICS-002: Ressourcen MÜSSEN zentral registriert und verwaltet werden.
- GRAPHICS-003: Ressourcenhandles MÜSSEN eindeutig validierbar sein.
- GRAPHICS-004: Veraltete Handles DÜRFEN keinen Zugriff auf neu zugewiesene Ressourcen ermöglichen.
- GRAPHICS-005: Ressourcen MÜSSEN einem gültigen Sicherheitskontext zugeordnet sein.
- GRAPHICS-006: Ressourcenzugriffe MÜSSEN autorisiert werden.
- GRAPHICS-007: Unvollständig initialisierte Ressourcen DÜRFEN nicht verwendet werden.
- GRAPHICS-008: Laufende Grafikoperationen MÜSSEN bei der Freigabe berücksichtigt werden.
- GRAPHICS-009: Verzögerte Ressourcenfreigaben MÜSSEN unterstützt werden.
- GRAPHICS-010: Gemeinsam genutzte Ressourcen MÜSSEN eindeutige Besitz- und Zugriffsregeln besitzen.
- GRAPHICS-011: Ressourcenverluste MÜSSEN erkannt und gemeldet werden.
- GRAPHICS-012: Ressourcenmigrationen MÜSSEN kontrolliert erfolgen.
- GRAPHICS-013: Fehlerhafte Ressourcenoperationen DÜRFEN keine fremden Speicherbereiche beschädigen.
- GRAPHICS-014: Ressourcen MÜSSEN nach dem Ende ihres gültigen Lebenszyklus vollständig bereinigt werden.
- GRAPHICS-015: Die Ressourcenverwaltung MUSS unabhängig vom Rendering-Backend funktionieren.
- GRAPHICS-016: Der Ressourcenlebenszyklus MUSS ohne KI-Abhängigkeit deterministisch verwaltet werden.

## 12. Fehlerbehandlung und Wiederherstellung

- Ungültige Ressourcenhandles werden zurückgewiesen.
- Fehlgeschlagene Speicherzuweisungen hinterlassen keine unvollständigen Ressourcenregistrierungen.
- Doppelte Freigabeanforderungen dürfen keine Speicherbeschädigungen verursachen.
- Bei Prozessabstürzen werden nicht mehr benötigte Ressourcen automatisch bereinigt.
- Bei GPU-Geräteverlust werden betroffene Ressourcen als `Lost` markiert.
- Nicht wiederherstellbare Ressourcen werden den betroffenen Anwendungen gemeldet.
- Fehlgeschlagene Migrationen führen zu einem gültigen vorherigen Zustand oder zur kontrollierten Invalidierung.
- Ressourcenfehler DÜRFEN die Verwaltung unabhängiger Grafikressourcen nicht unkontrolliert beeinträchtigen.

## 13. Ergebnis

NovaOS erhält eine zentrale und zuverlässige Verwaltung des gesamten Lebenszyklus grafischer Ressourcen.

Grafikressourcen werden kontrolliert erstellt, verwendet, geteilt, migriert und freigegeben.

Durch eindeutige Eigentümerschaft, validierte Handles und synchronisierte Freigaben werden Speicherlecks, ungültige Zugriffe und Ressourcenverluste beherrschbar.

**Grundsatz:** Jede Grafikressource besitzt einen eindeutigen Eigentümer, einen kontrollierten Zustand und ein definiertes Ende ihres Lebenszyklus.