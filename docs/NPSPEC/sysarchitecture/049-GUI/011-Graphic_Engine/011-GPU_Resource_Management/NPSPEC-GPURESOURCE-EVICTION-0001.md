# NPSPEC-GPURESOURCE-EVICTION-0001 – Nova GPU Resource Eviction

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Resource Management

## 1. Zweck

Nova GPU Resource Eviction definiert die kontrollierte Verdrängung von GPU-Ressourcen zur Freigabe benötigter Speicherkapazitäten.

Ziel ist die effiziente Nutzung begrenzten GPU-Speichers, ohne aktive GPU-Operationen, gültige Ressourcendaten oder die Stabilität der Benutzeroberfläche zu gefährden.

Die Architektur MUSS sowohl GPUs mit nativer Residency-Verwaltung als auch Hardware ohne entsprechende Funktionen unterstützen.

## 2. Architekturprinzipien

- **Demand-Driven:** Eviction erfolgt bei tatsächlichem Speicherbedarf.
- **Priority-Aware:** Systemkritische Ressourcen werden bevorzugt geschützt.
- **Lifetime-Safe:** Aktive GPU-Zugriffe verhindern unsichere Verdrängung.
- **Cost-Aware:** Wiederherstellungskosten werden berücksichtigt.
- **Dependency-Aware:** Ressourcenabhängigkeiten bleiben erhalten.
- **Budget-Aware:** Entscheidungen berücksichtigen verfügbare Speicherbudgets.
- **Asynchronous:** Verdrängungen erfolgen möglichst ohne CPU-Blockierung.
- **Recoverable:** Verdrängte Ressourcen können bei Bedarf wiederhergestellt werden.

## 3. Eviction Model

NovaOS unterscheidet folgende Zustände:

| Zustand | Beschreibung |
|---|---|
| Resident | Ressource ist verfügbar |
| Eviction Candidate | Ressource wurde zur Verdrängung ausgewählt |
| Pending Eviction | Verdrängung wird vorbereitet oder ausgeführt |
| Evicted | Ressource wurde aus dem betreffenden Speicherbereich verdrängt |
| Pending Restore | Wiederherstellung wurde angefordert |
| Resident | Wiederherstellung erfolgreich abgeschlossen |
| Invalid | Ressource ist nicht mehr verwendbar |

Eviction-Zustände MÜSSEN mit Nova GPU Resource Residency abgestimmt werden.

Eine logische Ressourcenidentität DARF durch Eviction nicht verloren gehen.

## 4. Eviction Eligibility

Vor jeder Verdrängung MUSS geprüft werden, ob eine Ressource dafür geeignet ist.

Voraussetzungen:

- Keine ungeschützten aktiven GPU-Zugriffe.
- Bekannte Ressourcenabhängigkeiten.
- Gesicherte oder rekonstruierbare Inhalte.
- Zulässige Speicher- und Hardwareoperationen.
- Erfüllte Sicherheitsanforderungen.

Nicht wiederherstellbare Ressourcen DÜRFEN nicht ohne ausdrückliche Freigabe ihrer Inhalte verdrängt werden.

## 5. Eviction Priorities

NovaOS SOLL Ressourcen anhand ihrer Bedeutung klassifizieren.

| Priorität | Behandlung |
|---|---|
| Critical | Möglichst dauerhaft verfügbar halten |
| High | Nur bei erheblichem Speicherbedarf verdrängen |
| Normal | Reguläre Eviction-Kandidaten |
| Low | Bevorzugte Eviction-Kandidaten |
| Disposable | Jederzeit nach sicherem Zugriffsabschluss verwerfbar |

Systemkritische Display- und Compositor-Ressourcen MÜSSEN besonders geschützt werden.

## 6. Eviction Cost Model

NovaOS SOLL Eviction-Kandidaten anhand ihrer Gesamtkosten bewerten.

Berücksichtigt werden:

- Freizugebende Speichermenge.
- Letzte Verwendung.
- Nutzungshäufigkeit.
- Wahrscheinlichkeit erneuter Verwendung.
- Wiederherstellungskosten.
- Erforderliche Speichertransfers.
- Auswirkungen auf Frame-Latenz und GPU-Auslastung.

Ressourcen mit geringem Wiederherstellungsaufwand SOLLEN bevorzugt verdrängt werden.

## 7. Eviction Execution

Eine Eviction erfolgt grundsätzlich in folgenden Schritten:

1. Geeignete Ressourcen werden identifiziert.
2. Ihre Zugriffs- und Synchronisationszustände werden geprüft.
3. Erforderliche Daten werden gesichert oder als rekonstruierbar bestätigt.
4. Noch ausstehende GPU-Zugriffe werden abgeschlossen.
5. Die Ressource wird kontrolliert verdrängt oder freigegeben.
6. Residency- und Speicherverwaltungszustände werden aktualisiert.

Eine Speicherfreigabe DARF erst als abgeschlossen gelten, wenn der betreffende Speicher tatsächlich wieder verfügbar ist.

## 8. Resource Restoration

NovaOS MUSS die Wiederherstellung verdrängter, weiterhin logisch gültiger Ressourcen unterstützen.

Dabei gilt:

- Benötigte Ressourcen werden vor ihrer Verwendung wieder bereitgestellt.
- Gesicherte Inhalte werden zurückübertragen oder neu erzeugt.
- Erforderliche GPU-Synchronisation wird durchgeführt.
- Abhängige GPU-Aufträge warten auf die vollständige Wiederherstellung.
- Fehlgeschlagene Wiederherstellungen DÜRFEN keine gültige Residency vortäuschen.

## 9. Cache und Pool Integration

Nova GPU Resource Eviction MUSS mit folgenden Komponenten zusammenarbeiten:

- Nova GPU Resource Caching.
- Nova GPU Resource Pooling.
- Nova GPU Memory Allocation.
- Nova GPU Memory Budgeting.
- Nova GPU Memory Pressure Management.

Dabei gilt:

- Ungenutzte Cache-Ressourcen SOLLEN bevorzugt entfernt werden.
- Freie Pool-Speicherbereiche können vollständig freigegeben werden.
- Aktive Ressourcen bleiben bis zum sicheren Abschluss geschützt.
- Physische Speicherersparnisse MÜSSEN korrekt bilanziert werden.

Die Entfernung eines Cache-Eintrags ist von der Verdrängung einer weiterhin logisch gültigen Ressource zu unterscheiden.

## 10. Synchronization Integration

Nova GPU Resource Eviction MUSS mit Nova GPU Synchronization zusammenarbeiten.

Dabei gilt:

- GPU Fences und Timeline-Werte werden berücksichtigt.
- Queueübergreifende Zugriffe MÜSSEN abgeschlossen sein.
- Aktive CPU-Mappings MÜSSEN berücksichtigt werden.
- Ressourcen DÜRFEN nicht während inkompatibler Zugriffe verdrängt werden.
- Eviction und Restoration MÜSSEN gültige Speicher- und Cache-Sichtbarkeit gewährleisten.

## 11. Hardware Fallback

NovaOS MUSS GPUs ohne native Resource Eviction unterstützen.

In diesem Fall können:

- Ungenutzte Ressourcen vollständig freigegeben werden.
- Wiederherstellbare Inhalte im Systemspeicher verbleiben.
- Ressourcen bei Bedarf neu angelegt werden.
- Speicherbudgets weitere Allokationen begrenzen.

Eine nicht unterstützte Hardwarefunktion DARF nicht vorausgesetzt werden.

## 12. Sicherheit und Fehlerbehandlung

Eviction MUSS capability-basierte Eigentums- und Zugriffsrechte respektieren.

NovaOS MUSS insbesondere folgende Fehler behandeln:

- Fehlgeschlagene Speicherfreigaben.
- Nicht abgeschlossene GPU-Zugriffe.
- Fehlgeschlagene Datenübertragungen.
- Fehlende Wiederherstellungsdaten.
- Ungültige Ressourcenreferenzen.
- Geräteverlust.

Bei einem Fehler MUSS die Ressource in einem eindeutig definierten Zustand verbleiben.

Eine fehlgeschlagene Eviction DARF weder eine erfolgreiche Speicherfreigabe noch eine gültige Wiederherstellung vortäuschen.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS kontrollierte GPU Resource Eviction unterstützen.
2. Eviction MUSS mit Resource Residency zusammenarbeiten.
3. Aktive GPU-Ressourcen DÜRFEN nicht unsicher verdrängt werden.
4. Ressourceninhalte MÜSSEN vor erforderlicher Verdrängung gesichert oder rekonstruierbar sein.
5. Systemkritische Ressourcen MÜSSEN bevorzugt geschützt werden.
6. Eviction-Entscheidungen SOLLEN Wiederherstellungskosten berücksichtigen.
7. GPU Memory Budgeting MUSS integriert werden.
8. GPU Memory Pressure Management MUSS Eviction auslösen können.
9. Resource Caching und Pooling MÜSSEN berücksichtigt werden.
10. GPU Synchronization MUSS integriert werden.
11. Wiederherstellbare Ressourcen MÜSSEN bei Bedarf erneut bereitgestellt werden können.
12. Fehlgeschlagene Eviction-Operationen MÜSSEN kontrolliert behandelt werden.
13. Ein Fallback ohne native Hardware-Eviction MUSS verfügbar sein.
14. Eviction MUSS capability-basierte Zugriffsrechte respektieren.
15. Im deterministischen Modus MÜSSEN logische Eviction-Entscheidungen bei identischen Eingaben und Ressourcenbedingungen reproduzierbar sein.

## 14. Ergebnis

Nova GPU Resource Eviction ermöglicht die kontrollierte Freigabe von GPU-Speicher durch die Verdrängung entbehrlicher oder wiederherstellbarer Ressourcen.

Durch prioritäts- und kostenbasierte Entscheidungen, sichere Synchronisation und bedarfsgerechte Wiederherstellung wird GPU-Speicher effizient genutzt.

Damit bleibt NovaOS auch bei begrenzter GPU-Speicherkapazität möglichst leistungsfähig, stabil und reaktionsfähig.