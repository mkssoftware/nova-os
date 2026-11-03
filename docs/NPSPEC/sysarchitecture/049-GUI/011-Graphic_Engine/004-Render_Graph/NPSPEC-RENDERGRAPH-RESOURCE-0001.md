# NPSPEC-RENDERGRAPH-RESOURCE-0001 – Nova Render Graph Resource Management

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Render Graph

## 1. Zweck

Nova Render Graph Resource Management definiert die Verwaltung logischer Grafikressourcen innerhalb des Nova Render Graph.

Ziel ist die effiziente Bereitstellung, Wiederverwendung und Freigabe von Ressourcen unter Berücksichtigung ihrer Abhängigkeiten und Lebenszeiten.

Die physische Speicherverwaltung verbleibt beim Nova GPU Resource Manager.

## 2. Architekturprinzipien

- **Declarative:** Render Passes deklarieren ihre Ressourcenanforderungen.
- **Lifetime-Aware:** Ressourcen werden entsprechend ihrer tatsächlichen Nutzungsdauer verwaltet.
- **Dependency-Aware:** Ressourcenzugriffe bestimmen Abhängigkeiten und Synchronisation.
- **Transient-First:** Temporäre Ressourcen werden möglichst effizient wiederverwendet.
- **Zero-Copy:** Unnötige Speicherübertragungen werden vermieden.
- **Incremental:** Unveränderte Ressourcenbindungen können erhalten bleiben.
- **Backend-Independent:** Logische Ressourcen bleiben unabhängig von konkreten GPU-Schnittstellen.
- **Budget-Aware:** Speicherbudgets werden berücksichtigt.

## 3. Ressourcenmodell

Eine logische Render-Graph-Ressource besitzt mindestens:

- **Resource ID:** Eindeutige Identifikation.
- **Resource Type:** Texture, Buffer oder unterstützter Ressourcentyp.
- **Descriptor:** Format, Größe und Nutzungseigenschaften.
- **Lifetime:** Erforderliche Lebensdauer.
- **Access State:** Deklarierte Lese- und Schreibzugriffe.
- **Physical Binding:** Optionale Zuordnung zu einer physischen Ressource.
- **Version:** Generation zur Erkennung ungültiger Referenzen.

Logische Ressourcen MÜSSEN unabhängig von ihrer physischen Speicherzuordnung identifizierbar sein.

## 4. Ressourcenklassen

NovaOS unterscheidet drei grundlegende Ressourcenklassen:

| Klasse | Beschreibung |
|---|---|
| Transient | Temporäre Ressourcen innerhalb einer Graph-Ausführung |
| Persistent | Ressourcen mit einer Lebensdauer über mehrere Graph-Ausführungen |
| Imported | Extern bereitgestellte Ressourcen mit fremder Eigentümerschaft |

Die Ressourcenklasse bestimmt Lebensdauer, Eigentümerschaft und Freigaberegeln.

Imported Resources DÜRFEN nicht eigenmächtig freigegeben werden.

## 5. Resource Lifecycle

Eine Ressource durchläuft folgende logische Phasen:

1. **Declared:** Ressourcenanforderung registriert.
2. **Validated:** Eigenschaften und Verwendung geprüft.
3. **Allocated/Imported:** Physische Ressource zugeordnet.
4. **Active:** Ressource wird verwendet.
5. **Retired:** Keine neuen Zugriffe vorgesehen.
6. **Released/Recycled:** Speicher freigegeben oder wiederverwendet.

Eine physische Ressource DARF erst freigegeben oder überschrieben werden, wenn sämtliche relevanten CPU- und GPU-Zugriffe abgeschlossen sind.

## 6. Resource Lifetime Analysis

Der Render Graph MUSS die Lebensdauer seiner Ressourcen anhand der geplanten Zugriffe bestimmen.

Dabei werden berücksichtigt:

- Erster und letzter Ressourcenzugriff
- Render-Pass-Abhängigkeiten
- Parallele Ausführung
- Asynchrone GPU Queues
- Externe Synchronisation
- Ressourcenexporte

Die Lebensdauer DARF nicht ausschließlich anhand der linearen Pass-Reihenfolge bestimmt werden.

## 7. Resource Allocation

Der Render Graph erstellt logische Allokationsanforderungen.

Die physische Bereitstellung erfolgt über den Nova GPU Resource Manager.

NovaOS SOLL:

- Bereits vorhandene kompatible Ressourcen wiederverwenden.
- Temporäre Allokationen minimieren.
- Unnötige Speicherreservierungen vermeiden.
- Ressourcenanforderungen zusammenfassen können.
- Speicherbudgets berücksichtigen.

Eine Ressourcenallokation MUSS vor dem ersten tatsächlichen Zugriff abgeschlossen sein.

## 8. Resource Aliasing

Transient Resources DÜRFEN denselben physischen Speicher verwenden, sofern ihre Nutzungsintervalle nachweislich nicht überlappen.

Dabei MÜSSEN:

- Ressourcenformate und Speicheranforderungen kompatibel sein.
- Sämtliche GPU-Zugriffe abgeschlossen oder korrekt geordnet sein.
- Erforderliche Speicherbarrieren eingeplant werden.
- Ungültige Inhalte vor der Nutzung initialisiert werden.

Die konkrete Optimierung wird durch Nova Transient Resource Aliasing definiert.

## 9. Resource Access Tracking

Jeder Render Pass MUSS seine Ressourcenzugriffe deklarieren.

Unterstützte Zugriffsarten umfassen:

- Read
- Write
- Read/Write
- Copy Source
- Copy Destination
- Attachment Access

Zugriffe SOLLEN auf Subressourcenebene erfasst werden können.

Nicht deklarierte Zugriffe DÜRFEN die Abhängigkeits- und Synchronisationsverwaltung nicht umgehen.

## 10. Incremental Resource Management

NovaOS SOLL gültige Ressourcenbindungen zwischen Graph-Aktualisierungen erhalten.

Eine erneute Zuordnung ist insbesondere erforderlich bei:

- Veränderten Ressourceneigenschaften
- Ungültigen Ressourcenreferenzen
- Geänderten Lebenszeiten
- Inkompatiblen Speicheranforderungen
- Ressourcenverlust

Unveränderte Ressourcen SOLLEN nicht unnötig neu angelegt werden.

## 11. Resource Synchronization

Der Render Graph stellt Informationen für die Synchronisationsplanung bereit.

Dabei MÜSSEN berücksichtigt werden:

- Ressourcenübergänge
- Lese- und Schreibkonflikte
- Queue-übergreifende Zugriffe
- CPU-GPU-Synchronisation
- Externe Ressourcenübergaben

Die konkrete Synchronisation erfolgt durch Nova GPU Synchronization und Nova Render Resource Barriers.

## 12. Resource Budgeting

Der Render Graph SOLL den erwarteten Ressourcenbedarf vor der Ausführung bestimmen.

Bei Speicherknappheit DÜRFEN:

- Temporäre Ressourcen stärker wiederverwendet werden.
- Nicht benötigte Ressourcen entfallen.
- Cache-Ergebnisse verworfen werden.
- Alternative Rendering-Strategien gewählt werden.

Speicheroptimierungen DÜRFEN keine erforderlichen Ressourcen oder gültigen Inhalte vorzeitig entfernen.

## 13. Integration

Die Verantwortlichkeiten sind getrennt:

- **Render Graph Resource Management:** Logische Ressourcen und Lebenszeiten.
- **GPU Resource Manager:** Physische Speicherverwaltung.
- **Render Pass Dependencies:** Ressourcenzugriffsabhängigkeiten.
- **Render Graph Scheduling:** Ausführungsplanung.
- **Transient Resource Aliasing:** Sichere gemeinsame Speichernutzung.
- **GPU Synchronization:** Zugriffssicherheit und Synchronisation.

Der Render Graph DARF keine hardwareabhängige Speicherverwaltung direkt voraussetzen.

## 14. Fehlerbehandlung

NovaOS MUSS folgende Fehler erkennen können:

- Ungültige Ressourcenreferenzen
- Inkompatible Ressourcenformate
- Fehlende Ressourceninitialisierung
- Unzulässige Speicherüberlagerung
- Ressourcenmangel
- Vorzeitige Ressourcenfreigabe

Bei fehlgeschlagener Ressourcenoptimierung MUSS eine konservative Speicherzuordnung möglich sein.

Ungültige Ressourcen DÜRFEN nicht zur Rendering-Ausführung verwendet werden.

## 15. Verbindliche Anforderungen

1. NovaOS MUSS logische Render-Graph-Ressourcen verwalten.
2. Ressourcen MÜSSEN eindeutig identifizierbar und versionierbar sein.
3. Transient, Persistent und Imported Resources MÜSSEN unterschieden werden.
4. Ressourcenlebenszeiten MÜSSEN anhand tatsächlicher Zugriffe bestimmt werden.
5. Parallele und asynchrone Ausführungen MÜSSEN berücksichtigt werden.
6. Render Passes MÜSSEN ihre Ressourcenzugriffe deklarieren.
7. Physische Allokationen MÜSSEN über den GPU Resource Manager erfolgen.
8. Kompatible Ressourcen SOLLEN wiederverwendet werden.
9. Resource Aliasing DARF nur bei nachweislich sicherer Nutzung erfolgen.
10. Ressourcen DÜRFEN nicht vor Abschluss ihrer Verwendung freigegeben werden.
11. Unveränderte Ressourcenbindungen SOLLEN erhalten bleiben.
12. Speicherbudgets MÜSSEN berücksichtigt werden können.
13. Externe Ressourcen MÜSSEN entsprechend ihrer Eigentümerschaft behandelt werden.
14. Erforderliche Synchronisationsinformationen MÜSSEN bereitgestellt werden.
15. Ein konservativer Ressourcen-Fallback MUSS verfügbar sein.

## 16. Ergebnis

Nova Render Graph Resource Management organisiert sämtliche logischen Grafikressourcen innerhalb des Nova Render Graph.

Durch Lebenszeitanalyse, inkrementelle Ressourcenverwaltung und kontrollierte Wiederverwendung werden unnötige Allokationen und Speicherübertragungen reduziert.

Die Trennung zwischen logischer Ressourcenplanung und physischer GPU-Speicherverwaltung ermöglicht eine effiziente, sichere und hardwareunabhängige Rendering-Architektur.