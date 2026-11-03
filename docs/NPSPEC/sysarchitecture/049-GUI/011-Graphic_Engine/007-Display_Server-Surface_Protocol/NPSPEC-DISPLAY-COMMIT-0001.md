# NPSPEC-DISPLAY-COMMIT-0001 – Nova Atomic Surface Commit

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Display Server

## 1. Zweck

Nova Atomic Surface Commit definiert die atomare Veröffentlichung von Änderungen an grafischen Surfaces.

Ziel ist die konsistente Übernahme zusammengehöriger Surface-Eigenschaften, Buffer-Referenzen und Damage-Informationen, ohne dass Zwischenzustände sichtbar werden.

Ein Surface Commit verändert den veröffentlichten Surface-Zustand, stellt jedoch keine unmittelbare Bildschirmausgabe dar.

## 2. Architekturprinzipien

- **Atomic:** Änderungen werden vollständig oder gar nicht veröffentlicht.
- **Transactional:** Mehrere Operationen bilden eine gemeinsame Zustandsänderung.
- **Versioned:** Jeder veröffentlichte Zustand besitzt eine eindeutige Version.
- **Immutable:** Veröffentlichte Zustands-Snapshots bleiben während ihrer Verwendung unverändert.
- **Asynchronous:** Commit-Verarbeitung und Frame-Präsentation sind entkoppelt.
- **Damage-Aware:** Inhaltsänderungen werden dem jeweiligen Commit zugeordnet.
- **Synchronization-Aware:** Buffer-Zugriffe berücksichtigen explizite Synchronisationsbedingungen.

## 3. Commit Model

Ein Surface Commit enthält mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Commit ID | Eindeutige Commit-Identifikation |
| Surface ID | Betroffene Surface |
| State Version | Neue Zustandsversion |
| Buffer Reference | Zugeordneter Buffer |
| Surface Properties | Geänderte Eigenschaften |
| Damage Regions | Veränderte Inhaltsbereiche |
| Acquire Fence | Synchronisation des neuen Buffer-Inhalts |
| Commit Status | Verarbeitungszustand |

Nicht veränderte Eigenschaften werden aus dem vorherigen gültigen Zustand übernommen.

## 4. Pending und Committed State

Jede Surface MUSS zwei logisch getrennte Zustandsbereiche besitzen.

**Pending State**
- Enthält noch nicht veröffentlichte Änderungen.
- Kann durch mehrere Protokolloperationen verändert werden.
- Ist für den Compositor nicht sichtbar.

**Committed State**
- Enthält den zuletzt erfolgreich übernommenen Surface-Zustand.
- Wird als konsistenter Snapshot bereitgestellt.
- Besitzt eine eindeutige Zustandsversion.

Ein Commit MUSS den Pending State validieren und anschließend atomar veröffentlichen.

Ein ungültiger Commit DARF den Committed State nicht verändern.

## 5. Commit Lifecycle

Ein Commit durchläuft folgende Zustände:

`Pending → Validating → Accepted → Published`

Weitere mögliche Zustände:

- **Rejected:** Validierung fehlgeschlagen.
- **Superseded:** Durch einen neueren, noch nicht benötigten Zustand ersetzt.
- **Retired:** Keine aktive Verwendung des Snapshots mehr.

Die Veröffentlichung eines Commits DARF nicht mit der tatsächlichen Präsentation verwechselt werden.

Ein akzeptierter Commit kann erst in einem späteren Frame sichtbar werden.

## 6. Commit Validation

Vor der Veröffentlichung MÜSSEN mindestens folgende Bedingungen geprüft werden:

1. Surface und Buffer existieren.
2. Der Client besitzt die erforderlichen Berechtigungen.
3. Buffer-Format und Speicherlayout sind gültig.
4. Surface-Eigenschaften liegen innerhalb zulässiger Grenzen.
5. Damage Regions besitzen gültige Koordinaten.
6. Synchronisationsreferenzen sind gültig.
7. Ressourcenlimits werden eingehalten.

Die Validierung MUSS abgeschlossen sein, bevor der neue Zustand veröffentlicht wird.

Die tatsächliche Verfügbarkeit der Buffer-Inhalte wird zusätzlich durch die Synchronisation gewährleistet.

## 7. Atomic State Publication

Ein erfolgreicher Commit MUSS sämtliche zugehörigen Änderungen als eine konsistente Zustandsversion veröffentlichen.

Dabei gilt:

- Der Compositor darf keine Mischung aus altem und neuem Surface-Zustand beobachten.
- Bereits verwendete Snapshots bleiben gültig.
- Neue Leser erhalten den veröffentlichten Zustand.
- Zustandsversionen müssen eindeutig und geordnet sein.

Die Implementierung DARF geeignete Verfahren wie Snapshot-Swapping oder versionierte Zustandsreferenzen verwenden.

## 8. Damage Integration

Damage Regions MÜSSEN eindeutig dem veröffentlichten Inhaltszustand zugeordnet werden.

Dabei gilt:

- Mehrere Änderungen dürfen innerhalb eines Commits zusammengefasst werden.
- Damage muss Buffer-Wechsel und Inhaltsgültigkeit berücksichtigen.
- Veraltete Commits dürfen keine erforderlichen Aktualisierungen verlieren.
- Der Compositor muss betroffene Bereiche korrekt invalidieren können.

Werden mehrere Commits für die Darstellung zusammengefasst, MUSS der resultierende Damage sämtliche seit dem zuletzt verwendeten Zustand relevanten Änderungen abdecken.

## 9. Synchronization

Ein Commit MUSS die Synchronisationsinformationen seines Buffers übernehmen können.

Dabei gilt:

- Acquire Fences bestimmen, wann Inhalte gelesen werden dürfen.
- Die Annahme eines Commits erfordert nicht zwingend den Abschluss des Renderings.
- Der Compositor darf erst nach erfüllter Synchronisation auf Inhalte zugreifen.
- Alte Buffer bleiben während aktiver Verwendungen geschützt.
- Release-Signale erfolgen erst nach Abschluss der jeweiligen Buffer-Verwendung.

Ein Commit DARF keine unzulässigen gleichzeitigen Speicherzugriffe verursachen.

## 10. Asynchronous Processing

NovaOS SOLL Commits asynchron verarbeiten.

Mehrere Commits einer Surface DÜRFEN zusammengefasst werden, sofern:

- Die gültige Zustandsreihenfolge erhalten bleibt.
- Keine erforderlichen Änderungen verloren gehen.
- Damage korrekt akkumuliert wird.
- Buffer-Lebenszeiten eingehalten werden.
- Protokollzusagen und Synchronisationsabhängigkeiten erhalten bleiben.

Nicht jeder angenommene Commit muss als eigenständiger Frame präsentiert werden.

## 11. Multi-Surface Consistency

Ein einzelner Atomic Surface Commit garantiert ausschließlich die atomare Veröffentlichung des Zustands einer Surface.

Die gleichzeitige Veröffentlichung mehrerer Surfaces erfordert eine gesonderte Transaktions- oder Synchronisationsfunktion.

Der Display Server DARF keine globale Atomarität mehrerer Surfaces voraussetzen.

Der Compositor MUSS für einen Frame einen konsistenten Satz veröffentlichter Surface-Zustände auswählen.

## 12. Fehlerbehandlung

NovaOS MUSS fehlgeschlagene Commits eindeutig behandeln.

Mögliche Fehler:

- Ungültige Surface-Referenz
- Fehlende Berechtigung
- Ungültiger Buffer
- Nicht unterstütztes Format
- Fehlerhafte Synchronisationsreferenz
- Ressourcenerschöpfung
- Ungültige Zustandsparameter

Ein abgewiesener Commit DARF keinen teilweise veröffentlichten Zustand hinterlassen.

Fehler MÜSSEN über das Nova Display Protocol gemeldet werden.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS atomare Surface-Commits unterstützen.
2. Pending und Committed State MÜSSEN getrennt verwaltet werden.
3. Jeder veröffentlichte Zustand MUSS eindeutig versioniert sein.
4. Commits MÜSSEN vor ihrer Veröffentlichung validiert werden.
5. Ungültige Commits DÜRFEN gültige Zustände nicht verändern.
6. Veröffentlichte Snapshots MÜSSEN während aktiver Verwendung unveränderlich bleiben.
7. Damage MUSS dem jeweiligen Inhaltszustand zugeordnet sein.
8. Buffer-Synchronisation MUSS berücksichtigt werden.
9. Aktive Buffer DÜRFEN nicht vorzeitig freigegeben werden.
10. Asynchrone Commit-Verarbeitung SOLL unterstützt werden.
11. Zusammengefasste Commits DÜRFEN keine erforderlichen Änderungen verlieren.
12. Commit-Annahme und tatsächliche Präsentation MÜSSEN unterscheidbar sein.
13. Multi-Surface-Atomarität DARF nicht implizit vorausgesetzt werden.
14. Fehler MÜSSEN eindeutig gemeldet werden.
15. Im deterministischen Modus MÜSSEN identische Commit-Sequenzen reproduzierbare Zustandsübergänge erzeugen.

## 14. Ergebnis

Nova Atomic Surface Commit gewährleistet die konsistente und atomare Veröffentlichung grafischer Surface-Zustände.

Durch versionierte Snapshots, explizite Synchronisation und Damage-Integration können Anwendungen ihre Inhalte ohne sichtbare Zwischenzustände an den Compositor übergeben.

Damit bildet das Verfahren eine zentrale Grundlage für Incremental Composition, Shared Surfaces und eine zuverlässige, ereignisgesteuerte Grafikpipeline in NovaOS.