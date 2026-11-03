# NPSPEC-DISPLAY-ZERO-COPY-0001 – Nova Zero-Copy Buffer Sharing

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Display Server

## 1. Zweck

Nova Zero-Copy Buffer Sharing definiert die gemeinsame Nutzung grafischer Speicherbuffer zwischen Anwendungen, Render Engine, Compositor und Display Server ohne unnötige Speicherkopien.

Ziel ist die Reduzierung von Speicherbandbreite, CPU-Auslastung und Darstellungslatenz bei gleichzeitiger Wahrung von Sicherheit, Synchronisation und Ressourcenintegrität.

## 2. Architekturprinzipien

- **Shared Memory:** Komponenten verwenden dasselbe Speicherobjekt.
- **Capability-Based:** Buffer-Zugriffe erfolgen über autorisierte Handles.
- **Copy Avoidance:** Unnötige Speichertransfers werden vermieden.
- **Format-Aware:** Speicherformate und Layouts werden berücksichtigt.
- **Synchronization-Aware:** Zugriffe werden explizit koordiniert.
- **Lifetime-Safe:** Buffer bleiben während aktiver Verwendung gültig.
- **Fallback-Safe:** Inkompatible Buffer können kopiert oder konvertiert werden.

## 3. Zero-Copy Model

Ein gemeinsam verwendbarer Buffer besitzt mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Buffer ID | Eindeutige Identifikation |
| Memory Handle | Autorisierte Speicherreferenz |
| Memory Domain | CPU-, GPU- oder gemeinsam zugänglicher Speicher |
| Pixel Format | Format der Bilddaten |
| Memory Layout | Strides, Offsets und Modifier |
| Usage Flags | Zulässige Verwendungsarten |
| Access Rights | Lese- und Schreibrechte |
| Synchronization | Zugehörige Fences |
| Lifetime State | Aktueller Ressourcenstatus |

Zero-Copy MUSS ohne Offenlegung unkontrollierter physischer Speicheradressen möglich sein.

## 4. Buffer Export und Import

NovaOS MUSS den kontrollierten Export und Import grafischer Buffer unterstützen.

Der Ablauf umfasst:

1. Der Producer erstellt einen geeigneten Buffer.
2. Der Buffer wird über einen autorisierten Handle exportiert.
3. Der Consumer importiert die Speicherreferenz.
4. Format, Layout und Zugriffsrechte werden geprüft.
5. Erforderliche Synchronisationsobjekte werden übernommen.
6. Der Consumer verwendet den Buffer direkt.

Ein erfolgreicher Import DARF keine zusätzliche Kopie voraussetzen.

Die Importfähigkeit MUSS jedoch anhand der tatsächlichen Hardware- und Treiberunterstützung geprüft werden.

## 5. Memory Compatibility

Zero-Copy DARF nur verwendet werden, wenn sämtliche beteiligten Komponenten den Buffer korrekt interpretieren und erreichen können.

Zu prüfen sind:

- Speicherzugänglichkeit
- Pixel- und Farbformat
- Speicherlayout und Modifier
- Strides und Plane-Offsets
- Alignment
- Zugriffsrechte
- Unterstützte GPU-Operationen
- Scanout-Kompatibilität

Ein Buffer kann für GPU-Rendering geeignet sein, ohne direkt für Display-Scanout geeignet zu sein.

## 6. Synchronization

NovaOS MUSS explizite Synchronisation für gemeinsam verwendete Buffer unterstützen.

Dabei gilt:

- Schreiboperationen müssen vor abhängigen Lesezugriffen abgeschlossen sein.
- Acquire Fences sichern die Verfügbarkeit neuer Inhalte.
- Release Fences kennzeichnen den Abschluss einer Verwendung.
- Gleichzeitige Zugriffe dürfen keine unzulässigen Read-/Write-Hazards erzeugen.
- CPU-Cache-Kohärenz muss bei entsprechenden Speicherarchitekturen berücksichtigt werden.

Zero-Copy DARF Synchronisationsanforderungen nicht umgehen.

## 7. Buffer Ownership und Lifetime

Die Eigentümerschaft eines Buffers MUSS von dessen aktueller Verwendung getrennt sein.

Dabei gilt:

- Mehrere Consumer dürfen denselben Buffer lesen.
- Schreibzugriffe benötigen geeignete Exklusivität oder Synchronisation.
- Die Freigabe eines Handles beendet nicht automatisch sämtliche Verwendungen.
- Aktive GPU- und Display-Zugriffe müssen abgeschlossen sein, bevor Speicher wiederverwendet wird.
- Ein Client-Absturz darf keine ungültigen Speicherzugriffe verursachen.

Die Ressourcenverwaltung MUSS ausstehende Hardwarezugriffe berücksichtigen.

## 8. Integration in die Grafikpipeline

Zero-Copy SOLL insbesondere zwischen folgenden Komponenten eingesetzt werden:

| Verbindung | Verwendung |
|---|---|
| Anwendung → Display Server | Shared Surface Buffer |
| Render Engine → Compositor | Direkte Verwendung gerenderter Inhalte |
| Compositor → Display Server | Übergabe fertiger Composition Buffer |
| Display Server → Scanout | Direkte Bildschirmausgabe |
| GPU → Hardware Overlay | Direkte Darstellung geeigneter Surfaces |

Zero-Copy Buffer Sharing bedeutet nicht automatisch Direct Scanout oder den Wegfall der Komposition.

## 9. Damage und Incremental Updates

NovaOS MUSS Zero-Copy mit Damage Tracking kombinieren können.

Dabei gilt:

- Damage Regions werden dem jeweiligen Inhaltszustand zugeordnet.
- Unveränderte Buffer-Bereiche dürfen wiederverwendet werden.
- Buffer Age und Content Validity müssen berücksichtigt werden.
- Teilaktualisierungen dürfen keine ungültigen Pixel sichtbar machen.

Zero-Copy beschreibt den Speicherzugriff und ersetzt nicht die Prüfung grafischer Inhaltsgültigkeit.

## 10. Fallback

Ist Zero-Copy nicht möglich, MUSS NovaOS einen sicheren alternativen Datenpfad bereitstellen.

Mögliche Verfahren:

1. Direkte gemeinsame Buffer-Nutzung.
2. GPU-interne Kopie oder Formatkonvertierung.
3. CPU-basierte Kopie oder Konvertierung.

Die Auswahl SOLL anhand von Kompatibilität, Kosten und Ressourcenverfügbarkeit erfolgen.

Ein Fallback DARF den veröffentlichten Surface-Zustand nicht beschädigen.

## 11. Sicherheit

Nova Zero-Copy Buffer Sharing MUSS die Speicherisolation von NovaOS gewährleisten.

Insbesondere:

- Buffer-Handles MÜSSEN capability-basiert geschützt sein.
- Speichergrenzen und Layoutangaben MÜSSEN validiert werden.
- Nicht autorisierte Zugriffe MÜSSEN verhindert werden.
- Wiederverwendeter Speicher DARF keine fremden Restdaten offenlegen.
- DMA-Zugriffe MÜSSEN durch geeignete Treiber- und Hardwaremechanismen kontrolliert werden.

Eine direkte Speicherfreigabe DARF keine Umgehung der NovaOS-Sicherheitsarchitektur ermöglichen.

## 12. Verbindliche Anforderungen

1. NovaOS MUSS den kontrollierten Austausch grafischer Buffer-Handles unterstützen.
2. Zero-Copy SOLL bevorzugt werden, wenn es technisch möglich und wirtschaftlich sinnvoll ist.
3. Buffer-Import und -Export MÜSSEN autorisiert erfolgen.
4. Speicherformate und Layouts MÜSSEN validiert werden.
5. Gemeinsame Buffer-Zugriffe MÜSSEN synchronisiert werden.
6. CPU-Cache-Kohärenz MUSS berücksichtigt werden.
7. Buffer-Lebensdauer und Handle-Lebensdauer MÜSSEN getrennt sein.
8. Aktive Buffer DÜRFEN nicht vorzeitig freigegeben werden.
9. Zero-Copy MUSS mit Damage Tracking zusammenarbeiten können.
10. Ein Kopier- oder Konvertierungsfallback MUSS verfügbar sein.
11. Direkte Speicherfreigaben DÜRFEN die Prozessisolation nicht verletzen.
12. Zero-Copy MUSS unabhängig von einer bestimmten GPU-API abstrahiert werden.
13. Im deterministischen Modus MÜSSEN Buffer-Zustandsübergänge reproduzierbar sein.

## 13. Ergebnis

Nova Zero-Copy Buffer Sharing ermöglicht die effiziente gemeinsame Nutzung grafischer Speicherressourcen innerhalb der NovaOS-Grafikpipeline.

Durch kontrollierte Buffer-Handles, direkte Speicherverwendung und explizite Synchronisation werden unnötige Kopien und Speichertransfers reduziert.

Damit unterstützt NovaOS eine ressourcenschonende Grafikarchitektur mit geringer Latenz und schafft die Grundlage für Shared Surfaces, Hardware Overlays und Direct Scanout.