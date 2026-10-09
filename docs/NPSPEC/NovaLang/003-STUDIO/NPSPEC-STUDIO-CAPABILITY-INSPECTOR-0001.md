
# NPSPEC-STUDIO-CAPABILITY-INSPECTOR-0001 – NovaLang Studio Capability Inspector

## Status

Angenommen

## Kategorie

NovaLang Studio / Capabilities / Inspektion

## Zweck

Definiert den Capability Inspector zur detaillierten Untersuchung einzelner NovaOS-Fähigkeiten.

Ziel ist die transparente Darstellung von Schnittstellen, Verträgen, Abhängigkeiten und Berechtigungen einer Capability direkt innerhalb von NovaLang Studio.

## Architektur

Der Capability Inspector besteht aus:

- **Metadata Viewer:** Anzeige von Identität, Version und Beschreibung.
- **Contract Inspector:** Untersuchung typisierter Schnittstellen.
- **Permission Inspector:** Anzeige erforderlicher Berechtigungen.
- **Dependency Inspector:** Darstellung von Abhängigkeiten.
- **Usage Inspector:** Anzeige der Verwendung innerhalb einer Solution.
- **Diagnostic Integration:** Darstellung von Vertrags- und Kompatibilitätsfehlern.

Die Informationen werden aus der zentralen NovaOS Capability Registry und den zugehörigen Verträgen bezogen.

## Inspektionsfunktionen

Angezeigt werden:

- Capability-ID und Version
- Beschreibung und Hersteller
- Typisierte Ein- und Ausgänge
- Ereignisse und Fehlerverträge
- Abhängigkeiten
- Erforderliche Berechtigungen
- Verfügbarkeits- und Kompatibilitätsstatus
- Verwendungsstellen im Logic Graph

Die Capability-ID folgt dem Schema `domain.authority.namespace.name`.

## Logic-Graph-Integration

Beim Auswählen eines Capability-Knotens zeigt der Inspector dessen Vertrag und Konfiguration.

Unterstützt werden:

- Prüfung verbundener Datentypen
- Anzeige gültiger Verbindungen
- Untersuchung von Eingangs- und Ausgangswerten im Debug-Modus
- Navigation zu abhängigen Knoten
- Anzeige von Vertragsverletzungen

Konfigurationsänderungen müssen über den Logic Graph erfolgen und rückgängig gemacht werden können.

## Berechtigungsinspektion

Der Inspector unterscheidet zwischen benötigten, erteilten und verweigerten Berechtigungen.

Berechtigungen werden der verifizierten Solution-Identität zugeordnet.

Der Inspector selbst darf keine Berechtigungen erteilen oder Sicherheitsprüfungen umgehen.

## Normative Anforderungen

1. NovaLang Studio MUSS einen integrierten Capability Inspector bereitstellen.
2. Capability-Identität, Version und Verträge MÜSSEN angezeigt werden.
3. Ein- und Ausgänge MÜSSEN mit ihren Datentypen dargestellt werden.
4. Abhängigkeiten und Versionskonflikte MÜSSEN erkennbar sein.
5. Berechtigungsanforderungen und tatsächlicher Berechtigungsstatus MÜSSEN getrennt dargestellt werden.
6. Der Inspector MUSS mit Capability Browser und Logic Graph integriert sein.
7. Vertragsverletzungen MÜSSEN an das zentrale Diagnosesystem gemeldet werden.
8. Laufzeitwerte DÜRFEN nur innerhalb autorisierter Debug-Sitzungen angezeigt werden.
9. Änderungen DÜRFEN keine Capability- oder Sicherheitsgrenzen umgehen.
10. Der Capability Inspector MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält einen zentralen Capability Inspector, der die Eigenschaften, Schnittstellen und Sicherheitsanforderungen jeder Fähigkeit transparent macht und deren korrekte Verwendung im Logic Graph unterstützt.
