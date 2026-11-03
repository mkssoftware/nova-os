# NPSPEC-GPURESOURCE-CACHE-0001 – Nova GPU Resource Caching

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Resource Management

## 1. Zweck

Nova GPU Resource Caching definiert die Zwischenspeicherung und Wiederverwendung bereits erzeugter GPU-Ressourcen.

Ziel ist die Vermeidung unnötiger Ressourceninitialisierungen, Datenübertragungen und GPU-Berechnungen bei möglichst geringem Speicherverbrauch.

Das Caching MUSS mit GPU Memory Management, Resource Residency und Memory Pressure Management zusammenarbeiten.

## 2. Architekturprinzipien

- **Reuse-First:** Gültige Ressourcen werden bevorzugt wiederverwendet.
- **Content-Aware:** Cache-Einträge berücksichtigen ihre tatsächlichen Inhalte.
- **Dependency-Aware:** Änderungen relevanter Abhängigkeiten invalidieren betroffene Einträge.
- **Versioned:** Ressourcen- und Inhaltsversionen werden verfolgt.
- **Budget-Aware:** Cache-Speicher unterliegt definierten Speicherbudgets.
- **Lifetime-Safe:** Aktive GPU-Ressourcen bleiben geschützt.
- **Incremental:** Nur betroffene Cache-Einträge werden aktualisiert.
- **Backend-Independent:** Einheitliches logisches Cache-Modell.

## 3. Cache Architecture

NovaOS unterscheidet folgende Cache-Kategorien:

| Cache | Verwendung |
|---|---|
| Texture Cache | Wiederverwendung gültiger Texturinhalte |
| Buffer Cache | Wiederverwendung gültiger Buffer-Inhalte |
| Render Target Cache | Zwischenspeicherung gerenderter Ergebnisse |
| Material Resource Cache | Ressourcen für Materialeffekte |
| Transfer Cache | Wiederverwendung vorbereiteter GPU-Daten |
| Derived Resource Cache | Aus anderen Ressourcen berechnete Daten |

Die Cache-Verwaltung MUSS zwischen Ressourcenobjekten und ihren gespeicherten Inhalten unterscheiden.

## 4. Cache Entry Model

Jeder Cache-Eintrag besitzt mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Cache ID | Eindeutige Cache-Identifikation |
| Resource ID | Zugehörige GPU-Ressource |
| Cache Key | Schlüssel zur Identifikation des Inhalts |
| Content Version | Version der gespeicherten Daten |
| Dependency Signature | Versionen relevanter Abhängigkeiten |
| Validity State | Gültigkeit des Cache-Eintrags |
| Memory Usage | Tatsächlicher Speicherverbrauch |
| Last Access | Letzte Verwendung |
| Synchronization | Noch ausstehende GPU-Zugriffe |

Cache-Schlüssel MÜSSEN alle Eigenschaften berücksichtigen, die für die korrekte Wiederverwendung erforderlich sind.

## 5. Cache Lifecycle

NovaOS definiert folgende Zustände:

`Empty → Creating → Valid → Invalid → Evictable → Released`

Zusätzlich kann ein gültiger Cache-Eintrag vorübergehend durch aktive GPU-Zugriffe gebunden sein.

Dabei gilt:

1. Ein Cache-Eintrag wird angefordert.
2. Der Cache-Schlüssel wird geprüft.
3. Vorhandene Inhalte werden anhand ihrer Abhängigkeiten validiert.
4. Gültige Einträge können wiederverwendet werden.
5. Fehlende oder ungültige Inhalte werden neu erzeugt.
6. Nicht benötigte Einträge können verdrängt werden.

Ein vorhandenes Ressourcenobjekt bedeutet nicht automatisch einen gültigen Cache-Inhalt.

## 6. Cache Validation

Vor jeder Wiederverwendung MUSS die Gültigkeit des Cache-Eintrags geprüft werden.

Dabei sind insbesondere zu berücksichtigen:

- Ressourcenidentität und Generation.
- Inhaltsversion.
- Abhängige Ressourcen.
- Speicher- und Datenformat.
- Erforderliche Ressourcenlayouts.
- GPU-Backend-Kompatibilität.
- Vollständigkeit vorheriger GPU-Operationen.

Nicht abgeschlossene Schreiboperationen DÜRFEN nicht als gültige Cache-Ergebnisse behandelt werden.

## 7. Cache Invalidation

NovaOS MUSS gezielte Cache-Invalidierung unterstützen.

Auslöser können sein:

- Änderungen an Quelldaten.
- Änderungen von Ressourcenparametern.
- Überschreiben durch andere GPU-Operationen.
- Resource Aliasing.
- Änderungen abhängiger Render- oder Materialzustände.
- Ressourcenfreigabe.
- Geräteverlust.

Dabei gilt:

- Nur tatsächlich betroffene Einträge SOLLEN invalidiert werden.
- Abhängige Cache-Einträge MÜSSEN berücksichtigt werden.
- Ungültige Inhalte DÜRFEN nicht wiederverwendet werden.
- Invalidation MUSS mit laufenden GPU-Zugriffen synchronisiert werden.

## 8. Incremental Cache Updates

NovaOS SOLL partielle Cache-Aktualisierungen ermöglichen.

Dabei gilt:

- Änderungen können auf einzelne Buffer- oder Texturbereiche begrenzt werden.
- Unveränderte gültige Inhalte SOLLEN erhalten bleiben.
- Abhängige Bereiche MÜSSEN korrekt invalidiert werden.
- Teilweise aktualisierte Ressourcen DÜRFEN nicht vorzeitig als vollständig gültig gelten.
- Bei unzureichender Abhängigkeitsinformation MUSS eine vollständige Invalidierung erfolgen.

Die räumliche Änderungsanalyse erfolgt in Zusammenarbeit mit Nova Damage Tracking.

## 9. Cache Replacement

NovaOS MUSS entbehrliche Cache-Einträge kontrolliert verdrängen können.

Die Auswahl SOLL folgende Faktoren berücksichtigen:

- Speicherverbrauch.
- Nutzungshäufigkeit.
- Letzte Verwendung.
- Wiederherstellungskosten.
- Ressourcenpriorität.
- Aktuelle Speicherbelastung.

Eine adaptive Kombination geeigneter Strategien wie LRU und kostenbasierter Verdrängung DARF verwendet werden.

Aktive GPU-Ressourcen DÜRFEN nicht unsicher freigegeben werden.

## 10. Memory Budget und Pressure Integration

Nova GPU Resource Caching MUSS mit folgenden Subsystemen zusammenarbeiten:

- Nova GPU Memory Budgeting.
- Nova GPU Memory Pressure Management.
- Nova GPU Resource Residency.
- Nova GPU Resource Eviction.

Dabei gilt:

- Cache-Speicher MUSS vollständig bilanziert werden.
- Cache-Kapazitäten SOLLEN dynamisch angepasst werden.
- Entbehrliche Einträge SOLLEN bei Speicherknappheit bevorzugt freigegeben werden.
- Systemkritische Ressourcen MÜSSEN geschützt bleiben.
- Caching DARF keine Voraussetzung für die korrekte Grafikverarbeitung sein.

## 11. Resource Pooling und Render Cache

Nova GPU Resource Caching MUSS von anderen Wiederverwendungsmechanismen unterschieden werden.

| Mechanismus | Verantwortung |
|---|---|
| GPU Resource Pooling | Wiederverwendung von Ressourcenobjekten und Speicher |
| GPU Resource Caching | Wiederverwendung gültiger GPU-Ressourceninhalte |
| Render Result Caching | Wiederverwendung gerenderter Ergebnisse |
| Composition Cache | Wiederverwendung zusammengesetzter Bildbereiche |
| Scene Cache | Wiederverwendung abgeleiteter logischer Szenendaten |

Die spezialisierten Caches DÜRFEN GPU Resource Caching als gemeinsame Ressourcenverwaltung verwenden.

Ihre fachliche Cache-Gültigkeit bleibt jedoch in der Verantwortung des jeweiligen Subsystems.

## 12. Synchronization Integration

Nova GPU Resource Caching MUSS GPU Synchronization berücksichtigen.

Dabei gilt:

- Schreiboperationen MÜSSEN vor der Freigabe gültiger Inhalte abgeschlossen sein.
- Aktive Lesezugriffe MÜSSEN vor einer inkompatiblen Überschreibung berücksichtigt werden.
- GPU Fences und Timeline-Werte MÜSSEN integrierbar sein.
- Queueübergreifende Ressourcenverwendungen MÜSSEN korrekt synchronisiert werden.
- Cache-Invalidierung DARF keine noch benötigten Ressourcen vorzeitig zerstören.

## 13. Sicherheit und Isolation

Cache-Ressourcen MÜSSEN capability-basiert geschützt werden.

Dabei gilt:

- Cache-Einträge besitzen definierte Eigentümer oder Freigaberegeln.
- Prozessübergreifende Wiederverwendung erfordert Autorisierung.
- Cache-Schlüssel DÜRFEN keine Zugriffsrechte ersetzen.
- Wiederverwendete Ressourcen DÜRFEN keine fremden Daten offenlegen.
- Ungültige Ressourcenreferenzen MÜSSEN zurückgewiesen werden.

## 14. Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Cache-Einträge.
- Fehlgeschlagene Ressourceninitialisierung.
- Fehlgeschlagene GPU-Operationen.
- Überschrittene Speicherbudgets.
- Nicht verfügbare Cache-Ressourcen.
- Synchronisationsfehler.
- Geräteverlust.

Bei einem Cache-Fehler MUSS die betroffene Operation nach Möglichkeit ohne Cache-Wiederverwendung ausführbar bleiben.

Fehlerhafte Cache-Einträge DÜRFEN nicht als gültig bereitgestellt werden.

## 15. Verbindliche Anforderungen

1. NovaOS MUSS GPU Resource Caching bereitstellen.
2. Cache-Einträge MÜSSEN eindeutig identifizierbar sein.
3. Inhaltsversionen und relevante Abhängigkeiten MÜSSEN berücksichtigt werden.
4. Cache-Gültigkeit MUSS vor Wiederverwendung geprüft werden.
5. Gezielte Cache-Invalidierung MUSS unterstützt werden.
6. Inkrementelle Aktualisierungen SOLLEN möglich sein.
7. Ungültige Cache-Inhalte DÜRFEN nicht verwendet werden.
8. GPU Synchronization MUSS integriert werden.
9. Aktive GPU-Ressourcen DÜRFEN nicht vorzeitig freigegeben werden.
10. GPU Memory Budgeting MUSS berücksichtigt werden.
11. Memory Pressure Management MUSS Cache-Kapazitäten reduzieren können.
12. Resource Residency MUSS berücksichtigt werden.
13. Resource Pooling und Resource Caching MÜSSEN logisch getrennt bleiben.
14. Spezialisierte Render- und Composition-Caches MÜSSEN integrierbar sein.
15. Cache-Zugriffe MÜSSEN capability-basierte Berechtigungen respektieren.
16. Bei Cache-Fehlern MUSS ein sicherer Fallback möglich sein.
17. Im deterministischen Modus MÜSSEN logische Cache-Entscheidungen bei identischen Eingaben und Ressourcenbedingungen reproduzierbar sein.

## 16. Ergebnis

Nova GPU Resource Caching ermöglicht die effiziente Wiederverwendung gültiger GPU-Ressourceninhalte.

Durch versionsbasierte Validierung, gezielte Invalidierung, inkrementelle Aktualisierung und adaptive Speicherverwaltung werden unnötige GPU-Berechnungen und Datenübertragungen vermieden.

Damit erhält NovaOS eine ressourceneffiziente Cache-Grundlage für Rendering, Compute, Material Engine und Compositor.