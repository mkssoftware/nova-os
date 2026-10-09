
# NPSPEC-SOLUTION-CAPABILITY-FLOW-0001 – NovaOS Solution Capability Flow

## Status

Angenommen

## Kategorie

NovaOS / Solutions / Capability Flow

## Zweck

Definiert den kontrollierten Daten- und Ressourcenfluss zwischen Capabilities, Logic Graphs und Custom Scripts innerhalb einer NovaOS-Solution.

Ziel ist, Systemfähigkeiten ausschließlich über explizite, autorisierte Verbindungen nutzbar zu machen.

## Architektur

Der Capability Flow besteht aus:

- **Capability Node:** Bereitstellung einer registrierten Systemfähigkeit.
- **Flow Controller:** Steuerung des Daten- und Ressourcenflusses.
- **Contract Validator:** Prüfung von Porttypen und Capability-Verträgen.
- **Permission Bridge:** Anbindung an die zentrale Berechtigungsverwaltung.
- **Resource Handle Manager:** Verwaltung autorisierter Ressourcenreferenzen.
- **Flow Diagnostics:** Nachverfolgung von Ausführungen und Fehlern.

Die Ausführung erfolgt über die bestehende Logic Graph Runtime.

## Grundprinzip

Systemzugriffe erfolgen ausschließlich über Capability Nodes.

Ein typischer Ablauf lautet:

**Network Capability → Custom Script → Storage Capability**

Die Network Capability beschafft autorisierte Daten.

Das Custom Script verarbeitet ausschließlich die bereitgestellten Eingaben.

Die Storage Capability übernimmt anschließend die autorisierte Speicherung.

Custom Scripts dürfen Systemfähigkeiten nicht selbstständig anfordern oder direkt aufrufen.

## Capability-Verträge

Jede verwendete Capability besitzt:

- Eindeutige Capability-ID
- Versionierten Schnittstellenvertrag
- Typisierte Eingangs- und Ausgangsports
- Deklarierte Berechtigungsanforderungen
- Definierte Fehler- und Ressourcenregeln

Capability-IDs verwenden das Format `domain.authority.namespace.name`.

## Daten- und Ressourcenfluss

Daten werden über typisierte Graphverbindungen übertragen.

Ressourcenreferenzen dürfen nur innerhalb ihrer autorisierten Gültigkeitsbereiche verwendet werden.

Die Weitergabe einer Ressourcenreferenz darf keine zusätzlichen Rechte erzeugen.

Nicht autorisierte oder inkompatible Verbindungen werden zurückgewiesen.

## Berechtigungen

Benötigte Capabilities werden aus der Graphdefinition ermittelt und der Solution zugeordnet.

Die zentrale Berechtigungsverwaltung bindet Freigaben an die verifizierte Solution-Identität aus `solution.xml`.

Neue oder erweiterte Capability-Anforderungen müssen erneut geprüft werden.

Eine Solution-GUID allein gilt nicht als Sicherheitsnachweis.

## Ausführung und Fehler

Vor einer Capability-Ausführung werden Vertrag, Berechtigung und Ressourcenlimits geprüft.

Fehler werden über definierte Fehlerpfade an die Logic Graph Runtime weitergeleitet.

Abbruch, Zeitüberschreitungen und Ressourcenfreigabe folgen den bestehenden Runtime-Verträgen.

## Normative Anforderungen

1. Systemzugriffe innerhalb einer Solution MÜSSEN über autorisierte Capability Nodes erfolgen.
2. Custom Scripts DÜRFEN Capabilities weder direkt aufrufen noch selbstständig anfordern.
3. Capability-Verbindungen MÜSSEN explizit im Logic Graph definiert sein.
4. Daten- und Ressourcenports MÜSSEN typisiert sein.
5. Capability-Verträge MÜSSEN vor der Ausführung validiert werden.
6. Ressourcenreferenzen DÜRFEN keine zusätzlichen Berechtigungen übertragen.
7. Berechtigungen MÜSSEN an die verifizierte Solution-Identität gebunden sein.
8. Sicherheitsrelevante Graphänderungen MÜSSEN eine erneute Berechtigungsprüfung auslösen.
9. Capability-Fehler MÜSSEN kontrolliert weitergeleitet werden.
10. Ressourcenlimits und Abbruchregeln MÜSSEN eingehalten werden.
11. Capability-Flüsse MÜSSEN diagnostizierbar sein.
12. Der Capability Flow MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält einen expliziten, typsicheren und berechtigungskontrollierten Capability Flow, der Systemfähigkeiten mit Logic Graphs und Custom Scripts verbindet, ohne deren Sicherheitsgrenzen aufzuheben.
