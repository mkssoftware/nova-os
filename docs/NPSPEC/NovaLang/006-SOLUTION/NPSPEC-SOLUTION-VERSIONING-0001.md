
# NPSPEC-SOLUTION-VERSIONING-0001 – NovaOS Solution Versioning

## Status

Angenommen

## Kategorie

NovaOS / Solutions / Versionsverwaltung

## Zweck

Definiert die Versionierung und Kompatibilitätsprüfung von NovaOS-Solutions.

Ziel ist die sichere Weiterentwicklung, Aktualisierung und Wiederherstellung von Solutions unter Erhaltung ihrer Identität, Daten und Berechtigungsintegrität.

## Architektur

Das Versioning-System besteht aus:

- **Version Manager:** Verwaltung der Solution-Versionen.
- **Compatibility Checker:** Prüfung von Schnittstellen und Abhängigkeiten.
- **Change Detector:** Erkennung relevanter Änderungen.
- **Migration Manager:** Aktualisierung von Konfigurationen und Daten.
- **Update Coordinator:** Kontrollierte Übernahme neuer Versionen.
- **Rollback Controller:** Wiederherstellung vorheriger gültiger Versionen.

## Versionsmodell

Jede Solution besitzt eine Version im Format `MAJOR.MINOR.PATCH`.

- **MAJOR:** Inkompatible Änderungen.
- **MINOR:** Rückwärtskompatible Erweiterungen.
- **PATCH:** Kompatible Fehlerkorrekturen.

Die Version wird in `solution.xml` gespeichert.

Die dauerhaft eindeutige Solution-GUID bleibt bei regulären Updates unverändert.

## Versionierte Bestandteile

Zur Solution-Version gehören:

- Logic Graphs und deren Schnittstellen
- NovaLang-Scripts
- Deklarative UI und Bindings
- Capability-Anforderungen
- Ressourcen und Konfigurationen
- Abhängigkeiten und deren Versionen

Ein versioniertes Manifest referenziert die zugehörigen Bestandteile und Integritätsinformationen.

## Kompatibilität

Vor einem Update werden geprüft:

- Graph- und Script-Schnittstellen
- UI-Bindings
- Capability-Verträge
- Datenformate und Zustände
- Abhängigkeiten
- Erforderliche Runtime-Versionen

Inkompatible Änderungen müssen erkannt und vor der Übernahme behandelt werden.

## Update und Migration

Updates erfolgen kontrolliert und möglichst transaktional.

Bestehende Benutzerdaten und persistente Zustände müssen erhalten oder über definierte Migrationen übertragen werden.

Schlägt ein Update fehl, muss ein gültiger vorheriger Zustand wiederherstellbar sein, soweit die beteiligten Ressourcen dies unterstützen.

Irreversible Migrationen müssen vor ihrer Ausführung ausdrücklich kenntlich gemacht werden.

## Identität und Berechtigungen

Die Solution-GUID dient als dauerhafte Identitätsreferenz, nicht als alleiniger Vertrauensnachweis.

Bei Updates werden Integrität, Herkunft und sicherheitsrelevante Änderungen überprüft.

Bestehende Berechtigungen dürfen nur übernommen werden, wenn die aktualisierte Solution weiterhin vertrauenswürdig ist und die Berechtigungsverträge unverändert gültig bleiben.

Neue oder erweiterte Capability-Anforderungen erfordern eine erneute Autorisierung.

## Normative Anforderungen

1. Jede Solution MUSS eine eindeutige GUID und eine Version besitzen.
2. Die Version MUSS dem Format `MAJOR.MINOR.PATCH` entsprechen.
3. Reguläre Updates MÜSSEN die Solution-GUID erhalten.
4. Versionierte Bestandteile und Abhängigkeiten MÜSSEN nachvollziehbar sein.
5. Kompatibilität MUSS vor einem Update geprüft werden.
6. Inkompatible Änderungen DÜRFEN nicht stillschweigend übernommen werden.
7. Datenmigrationen MÜSSEN versioniert und validierbar sein.
8. Fehlgeschlagene Updates MÜSSEN kontrolliert behandelt werden.
9. Rollback DARF keine falsche Wiederherstellung irreversibler Seiteneffekte zusichern.
10. Updates MÜSSEN die Solution-Integrität erneut prüfen.
11. Berechtigungen DÜRFEN nicht allein aufgrund einer unveränderten GUID übernommen werden.
12. Neue Capability-Anforderungen MÜSSEN erneut autorisiert werden.
13. Die Versionsverwaltung MUSS unabhängig von NovaLang Studio und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine sichere und nachvollziehbare Versionsverwaltung für Solutions mit Kompatibilitätsprüfung, kontrollierten Updates, Datenmigration und Schutz bestehender Berechtigungen.
