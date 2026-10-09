
# NPSPEC-STUDIO-PERMISSION-EDITOR-0001 – NovaLang Studio Permission Editor

## Status

Angenommen

## Kategorie

NovaLang Studio / Solutions / Berechtigungen

## Zweck

Definiert den Permission Editor zur Verwaltung und Prüfung der Berechtigungsanforderungen von NovaOS-Solutions.

Ziel ist eine transparente, sichere und verständliche Darstellung aller benötigten Systemzugriffe, ohne die zentrale Berechtigungsverwaltung von NovaOS zu umgehen.

## Architektur

Der Permission Editor besteht aus:

- **Permission Analyzer:** Ermittlung benötigter Berechtigungen.
- **Permission Viewer:** Darstellung der Berechtigungsanforderungen.
- **Capability Mapper:** Zuordnung zu verwendeten Capabilities.
- **Permission Validator:** Prüfung von Anforderungen und Verträgen.
- **Solution Integration:** Synchronisierung mit `solution.xml`.
- **Change Tracker:** Erkennung sicherheitsrelevanter Änderungen.

Die NovaOS-Berechtigungs-Registry bleibt die alleinige Instanz für tatsächlich erteilte Berechtigungen.

## Berechtigungsmodell

Berechtigungen werden aus den verwendeten Capabilities des Logic Graph abgeleitet.

Der Editor unterscheidet:

- Erforderliche Berechtigungen
- Optionale Berechtigungen
- Bereits erteilte Berechtigungen
- Verweigerte Berechtigungen
- Nicht verfügbare Berechtigungen

Jede Anforderung muss auf ihre verursachende Capability zurückführbar sein.

## Solution-Integration

Die `solution.xml` enthält die deklarativen Berechtigungsanforderungen und die dauerhafte Solution-GUID.

Berechtigungen werden durch NovaOS an die verifizierte Solution-Identität gebunden.

Sicherheitsrelevante Änderungen müssen erkannt werden und können eine erneute Autorisierung erforderlich machen.

Eine GUID allein gilt nicht als Identitäts- oder Integritätsnachweis.

## Bearbeitung

Der Permission Editor ermöglicht:

- Anzeigen benötigter Berechtigungen
- Navigation zur verursachenden Capability
- Erkennen unnötiger oder fehlender Anforderungen
- Prüfen von Berechtigungskonflikten
- Aktualisieren deklarativer Anforderungen
- Anzeigen sicherheitsrelevanter Änderungen

Systemzugriffe entstehen ausschließlich durch explizit verwendete Capabilities im Logic Graph.

Custom Scripts dürfen keine eigenständigen Berechtigungsanforderungen erzeugen.

## Normative Anforderungen

1. NovaLang Studio MUSS einen integrierten Permission Editor bereitstellen.
2. Berechtigungsanforderungen MÜSSEN aus den verwendeten Capabilities ableitbar sein.
3. Jede Anforderung MUSS ihrer verursachenden Capability zugeordnet werden können.
4. Deklarierte Anforderungen und tatsächlich erteilte Berechtigungen MÜSSEN getrennt dargestellt werden.
5. Änderungen MÜSSEN mit `solution.xml` synchronisiert werden.
6. Sicherheitsrelevante Änderungen MÜSSEN erkannt und gekennzeichnet werden.
7. Der Editor DARF keine Berechtigungen selbstständig erteilen oder verändern.
8. Custom Scripts DÜRFEN keine eigenständigen Systemberechtigungen anfordern.
9. Die NovaOS-Berechtigungs-Registry MUSS die maßgebliche Autorisierungsinstanz bleiben.
10. Der Permission Editor MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält einen zentralen Permission Editor, der Berechtigungsanforderungen von Solutions nachvollziehbar macht und ihre sichere Verwaltung unterstützt, ohne die Autoritätsgrenzen von NovaOS zu verändern.
