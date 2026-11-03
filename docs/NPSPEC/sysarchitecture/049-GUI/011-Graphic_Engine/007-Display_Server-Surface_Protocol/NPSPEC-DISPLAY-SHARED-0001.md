# NPSPEC-DISPLAY-SHARED-0001 – Nova Shared Surface Protocol

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Display Server

## 1. Zweck

Nova Shared Surface Protocol definiert die kontrollierte gemeinsame Nutzung grafischer Surfaces zwischen Anwendungen, Render Engine, Compositor und Display Server.

Ziel ist die effiziente Übertragung grafischer Inhalte über Prozessgrenzen hinweg, ohne unnötige Speicherkopien oder Verletzungen der Prozessisolation.

## 2. Architekturprinzipien

- **Zero-Copy:** Grafische Inhalte werden bevorzugt über gemeinsame Speicherreferenzen bereitgestellt.
- **Capability-Based:** Jede Freigabe benötigt explizite Berechtigungen.
- **Surface-Abstraction:** Surface-Identität und physischer Speicher bleiben getrennt.
- **Producer-Consumer:** Erzeugung und Verwendung grafischer Inhalte sind entkoppelt.
- **Synchronization-Aware:** Gleichzeitige Zugriffe werden kontrolliert.
- **Versioned:** Veröffentlichte Surface-Zustände sind eindeutig identifizierbar.
- **Backend-Independent:** Das Protokoll ist unabhängig von der verwendeten GPU-API.

## 3. Shared Surface Model

Eine Shared Surface besitzt mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Surface ID | Eindeutige Surface-Identifikation |
| Owner | Eigentümer der Surface |
| Shared Handle | Autorisierte Referenz |
| Buffer Reference | Zugeordneter Grafikbuffer |
| Access Rights | Freigegebene Zugriffsrechte |
| Content Version | Version des Inhalts |
| Damage Regions | Veränderte Bildbereiche |
| Synchronization | Acquire- und Release-Informationen |
| Lifecycle State | Aktueller Freigabezustand |

Eine Shared Surface MUSS unabhängig vom Adressraum des erzeugenden Prozesses verwendbar sein.

## 4. Sharing Operations

Das Protokoll MUSS folgende Operationen unterstützen:

- **ExportSurface:** Autorisierte Freigabe einer Surface.
- **ImportSurface:** Übernahme einer freigegebenen Surface-Referenz.
- **AttachSharedBuffer:** Zuordnung eines gemeinsam nutzbaren Buffers.
- **CommitSharedSurface:** Veröffentlichung eines neuen Inhaltszustands.
- **AcquireSurface:** Synchronisierter Zugriff auf einen veröffentlichten Zustand.
- **ReleaseSurface:** Abschluss einer Verwendung.
- **RevokeSurface:** Entzug zukünftiger Zugriffsberechtigungen.

Die Freigabe einer Surface DARF keine automatische Übertragung sämtlicher Eigentümerrechte bewirken.

## 5. Ownership und Access Control

NovaOS MUSS zwischen Eigentum, Freigabe und tatsächlicher Verwendung unterscheiden.

Unterstützte Berechtigungen:

| Berechtigung | Funktion |
|---|---|
| Read | Surface-Inhalte lesen |
| Write | Autorisierte Inhaltsänderungen |
| Share | Surface weitergeben |
| Manage | Surface-Eigenschaften verwalten |
| Destroy | Surface-Eigentum auflösen |

Berechtigungen MÜSSEN über Nova Capabilities kontrolliert werden.

Eine weitergegebene Referenz DARF keine höheren Rechte besitzen als die autorisierte Freigabe erlaubt.

## 6. Producer-Consumer Synchronization

Shared Surfaces MÜSSEN eine sichere Zusammenarbeit zwischen Producer und Consumer ermöglichen.

Dabei gilt:

1. Der Producer erstellt oder aktualisiert einen Buffer.
2. Der Producer veröffentlicht den Inhalt über einen gültigen Commit.
3. Der Consumer wartet erforderliche Acquire-Synchronisation ab.
4. Der Consumer verwendet den veröffentlichten Inhalt.
5. Nach Abschluss wird die Verwendung über Release-Synchronisation freigegeben.

Ein Producer DARF einen noch verwendeten Buffer nicht unzulässig überschreiben.

Mehrere Consumer DÜRFEN denselben unveränderlichen Inhaltszustand parallel lesen.

## 7. Atomic State Publication

Änderungen einer Shared Surface MÜSSEN atomar veröffentlicht werden.

Ein veröffentlichter Zustand umfasst:

- Buffer-Referenz
- Inhaltsversion
- Damage Regions
- Relevante Surface-Eigenschaften
- Synchronisationsinformationen

Consumer MÜSSEN einen konsistenten Zustand erhalten.

Ein fehlgeschlagener Commit DARF den zuletzt gültigen Zustand nicht verändern.

## 8. Zero-Copy Integration

NovaOS SOLL Shared Surfaces ohne zusätzliche Kopien zwischen Prozessen nutzbar machen.

Voraussetzungen sind:

- Kompatible Buffer-Formate
- Unterstützte Speicherlayouts
- Gültige Speicherfreigaben
- Korrekte Synchronisation
- Geeignete Hardwareunterstützung

Zero-Copy bedeutet die gemeinsame Verwendung desselben Speicherobjekts, nicht den Verzicht auf notwendige Synchronisation.

Ist direkte Nutzung nicht möglich, MUSS ein kontrollierter Kopier- oder Konvertierungspfad verfügbar sein.

## 9. Damage und Incremental Updates

Shared Surfaces MÜSSEN Damage Regions unterstützen.

Dabei gilt:

- Damage wird einem veröffentlichten Inhaltszustand zugeordnet.
- Consumer können ausschließlich betroffene Bereiche weiterverarbeiten.
- Unveränderte Inhalte dürfen wiederverwendet werden, sofern ihre Gültigkeit nachgewiesen ist.
- Ein Buffer-Wechsel MUSS die tatsächliche Gültigkeit vorhandener Pixel berücksichtigen.

Unvollständige Damage-Angaben DÜRFEN nicht zu fehlerhaften Bildinhalten führen.

## 10. Lifecycle und Resource Management

Die Lebensdauer einer Shared Surface MUSS unabhängig von einzelnen Client-Verbindungen verwaltet werden.

Dabei gilt:

- Surface-Referenzen werden kontrolliert verwaltet.
- Aktive Buffer-Verwendungen bleiben geschützt.
- Der Verlust einer Client-Verbindung wird erkannt.
- Freigaben und Widerrufe werden durchgesetzt.
- Ressourcen werden nach Abschluss aller Verwendungen freigegeben.

Ein Widerruf MUSS neue unberechtigte Zugriffe verhindern. Bereits laufende Hardwarezugriffe MÜSSEN sicher abgeschlossen oder kontrolliert abgebrochen werden.

## 11. Sicherheit und Isolation

Nova Shared Surface Protocol MUSS die NovaOS-Sicherheitsarchitektur einhalten.

Insbesondere:

- Keine unautorisierten Speicherzugriffe.
- Keine Offenlegung fremder Prozessdaten.
- Keine unkontrollierte Weitergabe von Surface-Rechten.
- Validierung sämtlicher Handles und Buffer-Metadaten.
- Begrenzung des Ressourcenverbrauchs.
- Schutz vor ungültigen Zustandsübergängen.

Gemeinsame Speicherbereiche MÜSSEN so initialisiert und freigegeben werden, dass keine vertraulichen Restdaten sichtbar werden.

## 12. Integration

Nova Shared Surface Protocol arbeitet mit:

- **Nova Display Protocol:** IPC und Objektverwaltung.
- **Nova Surface Protocol:** Surface-Zustände und Commits.
- **Nova Display Buffer Management:** Buffer-Verwaltung.
- **Nova Zero-Copy Buffer Sharing:** Direkte Speicherfreigabe.
- **Nova Surface Synchronization:** Zugriffskoordination.
- **Nova Compositor:** Verwendung freigegebener Inhalte.
- **Nova Damage Tracking:** Inkrementelle Aktualisierung.

Das Shared Surface Protocol verwaltet die kontrollierte gemeinsame Nutzung, nicht die eigentliche Komposition.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS Shared Surfaces zwischen Ring-3-Prozessen unterstützen.
2. Surface-Identität und Buffer-Speicher MÜSSEN getrennt bleiben.
3. Freigaben MÜSSEN capability-basiert autorisiert werden.
4. Eigentums- und Nutzungsrechte MÜSSEN unterscheidbar sein.
5. Producer-Consumer-Zugriffe MÜSSEN synchronisiert werden.
6. Veröffentlichte Surface-Zustände MÜSSEN atomar und versioniert sein.
7. Mehrere lesende Consumer MÜSSEN unterstützt werden können.
8. Zero-Copy SOLL bevorzugt werden.
9. Damage Regions MÜSSEN unterstützt werden.
10. Aktive Buffer DÜRFEN nicht vorzeitig freigegeben werden.
11. Widerrufene Berechtigungen DÜRFEN keine neuen Zugriffe ermöglichen.
12. Ein sicherer Kopierfallback MUSS verfügbar sein.
13. Fehlerhafte Clients DÜRFEN andere Clients nicht kompromittieren.
14. Das Protokoll MUSS unabhängig von einer bestimmten GPU-API bleiben.
15. Im deterministischen Modus MÜSSEN Zustandsübergänge reproduzierbar sein.

## 14. Ergebnis

Nova Shared Surface Protocol ermöglicht die sichere gemeinsame Nutzung grafischer Inhalte über Prozessgrenzen hinweg.

Durch capability-basierte Freigaben, atomare Zustandsveröffentlichung, explizite Synchronisation und Zero-Copy können Anwendungen und Systemdienste effizient zusammenarbeiten.

Damit bildet das Protokoll die Grundlage für eine performante, isolierte und ressourcenschonende Grafikkommunikation innerhalb von NovaOS.