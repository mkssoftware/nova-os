# NPSPEC-DISPLAY-SURFACE-0001 – Nova Surface Protocol

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Display Server

## 1. Zweck

Nova Surface Protocol definiert den Lebenszyklus, die Zustandsverwaltung und die Kommunikation grafischer Surfaces zwischen Anwendungen, Compositor und Display Server.

Ziel ist die sichere, atomare und effiziente Bereitstellung grafischer Inhalte unabhängig vom verwendeten Rendering-Backend.

Eine Surface beschreibt eine grafische Inhaltsquelle und ist von ihrem Speicherbuffer sowie ihrer Position innerhalb des Composition Tree getrennt.

## 2. Architekturprinzipien

- **Surface-Based:** Grafische Inhalte werden über eigenständige Surface-Objekte bereitgestellt.
- **Stateful:** Jede Surface besitzt einen persistenten Zustand.
- **Transactional:** Änderungen werden gesammelt und atomar veröffentlicht.
- **Buffer-Independent:** Surface-Identität und Buffer-Lebensdauer sind getrennt.
- **Damage-Aware:** Inhaltsänderungen werden räumlich beschrieben.
- **Asynchronous:** Buffer-Verarbeitung und Präsentation erfolgen entkoppelt.
- **Capability-Based:** Surface-Zugriffe unterliegen expliziten Berechtigungen.
- **Versioned:** Veröffentlichte Surface-Zustände sind eindeutig versioniert.

## 3. Surface Model

Eine Nova Surface enthält mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Surface ID | Eindeutige Identifikation |
| Owner | Autorisierter Eigentümer |
| Buffer Reference | Zugeordneter grafischer Buffer |
| Content Version | Version des Surface-Inhalts |
| Surface State | Aktueller Lebenszykluszustand |
| Damage Regions | Veränderte Inhaltsbereiche |
| Buffer Transform | Transformation des Buffer-Inhalts |
| Buffer Scale | Skalierungsinformation |
| Synchronization | Zugehörige Synchronisationsobjekte |

Die Surface MUSS unabhängig von ihrer Darstellung auf einem bestimmten Bildschirm existieren können.

Position, Z-Order und Fensterfokus gehören nicht zum grundlegenden Surface Protocol.

## 4. Surface Lifecycle

Eine Surface durchläuft folgende Zustände:

`Created → Configured → Active → Destroyed`

Zusätzlich kann eine Surface ohne aktuell zugeordneten Buffer bestehen.

Folgende Operationen MÜSSEN unterstützt werden:

- **CreateSurface:** Surface erstellen.
- **AttachBuffer:** Buffer zuordnen.
- **DetachBuffer:** Buffer-Zuordnung entfernen.
- **SetDamage:** Veränderte Bereiche melden.
- **SetProperties:** Surface-Eigenschaften ändern.
- **CommitSurface:** Änderungen veröffentlichen.
- **DestroySurface:** Surface entfernen.

Die Zerstörung einer Surface DARF noch verwendete Buffer nicht vorzeitig freigeben.

## 5. Pending und Committed State

NovaOS MUSS zwischen zwei Zuständen unterscheiden:

**Pending State**
- Enthält noch nicht veröffentlichte Änderungen.
- Kann durch mehrere Protokolloperationen verändert werden.
- Ist für die laufende Komposition nicht sichtbar.

**Committed State**
- Enthält den zuletzt erfolgreich übernommenen Zustand.
- Wird dem Compositor als konsistente Version bereitgestellt.
- Bleibt während aktiver Verarbeitung unverändert.

Ein Commit MUSS sämtliche zugehörigen Änderungen atomar veröffentlichen.

Ungültige Commits DÜRFEN den zuletzt gültigen Zustand nicht beschädigen.

## 6. Buffer Attachment

Eine Surface DARF unterschiedliche Buffer nacheinander verwenden.

Dabei gilt:

1. Buffer werden über gültige Ressourcenreferenzen eingebunden.
2. Format, Größe und Speicherlayout müssen validiert werden.
3. Zugriffsrechte und Synchronisation müssen gewährleistet sein.
4. Ein neuer Buffer wird erst mit einem gültigen Commit veröffentlicht.
5. Vorherige Buffer bleiben bis zum Abschluss ihrer Verwendung geschützt.

Das Anhängen eines Buffers DARF keine automatische Kopie seines Inhalts erzwingen.

## 7. Surface Damage

Nova Surface Protocol MUSS Damage Regions unterstützen.

Damage beschreibt Änderungen des Surface-Inhalts gegenüber einem definierten vorherigen Zustand.

Dabei gilt:

- Damage MUSS eindeutig einem Koordinatensystem zugeordnet sein.
- Buffer-Transformationen und Skalierung MÜSSEN berücksichtigt werden.
- Mehrere Damage Regions DÜRFEN zusammengefasst werden.
- Damage MUSS mit dem zugehörigen Commit veröffentlicht werden.

Wird ein Buffer verwendet, dessen unveränderte Bereiche nicht nachweislich gültig sind, MUSS eine ausreichende Invalidierung erfolgen.

Damage-Angaben DÜRFEN nicht als Ersatz für die Prüfung der Buffer-Gültigkeit verwendet werden.

## 8. Surface Synchronization

NovaOS MUSS die sichere Übergabe von Surface-Inhalten gewährleisten.

Dazu gehören:

- Acquire Fence für die Verfügbarkeit neuer Inhalte.
- Release Fence für den Abschluss der Buffer-Verwendung.
- Schutz vor gleichzeitigen unzulässigen Schreib- und Lesezugriffen.
- Versionssichere Veröffentlichung.
- Kontrollierte Ressourcenfreigabe.

Ein angenommener Surface-Commit bedeutet nicht automatisch, dass der Buffer bereits gelesen oder präsentiert wurde.

## 9. Surface Events

Das Protokoll SOLL folgende Ereignisse bereitstellen:

| Ereignis | Bedeutung |
|---|---|
| CommitAccepted | Surface-Zustand wurde übernommen |
| CommitRejected | Zustandsänderung wurde abgewiesen |
| BufferReleased | Buffer-Verwendung wurde abgeschlossen |
| FrameCallback | Rückmeldung zur Frame-Verarbeitung |
| SurfaceConfigured | Neue Konfiguration wurde bereitgestellt |
| SurfaceDestroyed | Surface wurde endgültig entfernt |

Presentation Feedback wird durch das zuständige Display-Protokoll bereitgestellt.

Frame-Callbacks DÜRFEN nicht mit einer tatsächlichen Präsentationsbestätigung gleichgesetzt werden.

## 10. Integration

Nova Surface Protocol arbeitet mit:

- **Nova Display Protocol:** IPC-Kommunikation und Objektverwaltung.
- **Nova Display Buffer Management:** Buffer-Import und Ressourcenlebensdauer.
- **Nova Atomic Surface Commit:** Veröffentlichung konsistenter Zustände.
- **Nova Surface Synchronization:** Producer-Consumer-Synchronisation.
- **Nova Compositor:** Verwendung veröffentlichter Surface-Inhalte.
- **Nova Damage Tracking:** Verarbeitung grafischer Änderungen.
- **Nova Frame Scheduling:** Bedarfsgesteuerte Aktualisierung.

Das Surface Protocol verwaltet Inhaltszustände, nicht die eigentliche Komposition oder Frame-Präsentation.

## 11. Sicherheit und Fehlerbehandlung

Surface-Operationen MÜSSEN capability-basiert autorisiert werden.

NovaOS MUSS insbesondere verhindern:

- Zugriff auf fremde Surfaces ohne Berechtigung.
- Verwendung ungültiger Buffer-Handles.
- Freigabe noch verwendeter Ressourcen.
- Ungültige Surface-Zustandsübergänge.
- Unbegrenzten Ressourcenverbrauch.
- Veröffentlichung unvollständiger Commits.

Bei einem Client-Absturz MÜSSEN dessen Surface-Ressourcen kontrolliert bereinigt werden.

Bereits laufende GPU- oder Display-Zugriffe MÜSSEN dabei berücksichtigt werden.

## 12. Verbindliche Anforderungen

1. NovaOS MUSS eigenständige Surface-Objekte unterstützen.
2. Surface-Identität und Buffer-Lebensdauer MÜSSEN getrennt sein.
3. Pending und Committed State MÜSSEN unterschieden werden.
4. Surface-Änderungen MÜSSEN atomar veröffentlicht werden.
5. Surface-Zustände MÜSSEN versioniert sein.
6. Buffer-Referenzen MÜSSEN validiert werden.
7. Damage Regions MÜSSEN unterstützt werden.
8. Buffer-Gültigkeit MUSS unabhängig von Damage-Angaben geprüft werden.
9. Surface-Buffer MÜSSEN korrekt synchronisiert werden.
10. Asynchrone Surface-Ereignisse MÜSSEN unterstützt werden.
11. Surface-Zugriffe MÜSSEN capability-basiert geschützt sein.
12. Ressourcen DÜRFEN nicht während aktiver Verwendung freigegeben werden.
13. Ungültige Commits DÜRFEN gültige Surface-Zustände nicht verändern.
14. Das Protokoll MUSS unabhängig von einer bestimmten GPU-API bleiben.
15. Identische Operationen MÜSSEN im deterministischen Modus reproduzierbare Zustandsübergänge erzeugen.

## 13. Ergebnis

Nova Surface Protocol stellt die einheitliche Verwaltung grafischer Inhaltsquellen innerhalb der NovaOS-Grafikarchitektur bereit.

Durch atomare Zustandsänderungen, getrennte Buffer-Verwaltung, Damage Tracking und sichere Synchronisation können Anwendungen ihre Inhalte effizient an den Compositor übergeben.

Damit bildet das Protokoll die Grundlage für Zero-Copy Surface Sharing, Incremental Composition und eine sichere, vollständig im Ring 3 nutzbare Grafikkommunikation.