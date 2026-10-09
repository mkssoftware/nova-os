
# NPSPEC-LOGIC-PERMISSIONS-0001 – NovaOS Logic Graph Permissions

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Sicherheit / Berechtigungen

## Zweck

Definiert die Berechtigungsprüfung für Capabilities innerhalb eines NovaOS Logic Graphs.

Ziel ist die kontrollierte Nutzung von Systemfähigkeiten durch Solutions nach dem Prinzip der minimal erforderlichen Berechtigungen.

## Architektur

Das Permission-System besteht aus:

- **Permission Resolver:** Ermittlung benötigter Berechtigungen.
- **Permission Validator:** Prüfung bestehender Autorisierungen.
- **Capability Guard:** Durchsetzung der Zugriffsgrenzen.
- **Solution Identity Validator:** Überprüfung der Solution-Identität.
- **Permission Registry Bridge:** Anbindung an die NovaOS-Berechtigungsverwaltung.
- **Audit Logger:** Protokollierung sicherheitsrelevanter Vorgänge.

Die zentrale Berechtigungs-Registry von NovaOS bleibt die verbindliche Autorität.

## Berechtigungsmodell

Berechtigungen werden an die verifizierte Identität einer Solution gebunden.

Diese umfasst:

- Dauerhafte Solution-GUID aus `solution.xml`
- Verifizierte Integrität und Herkunft
- Sicherheitsrelevante Versions- und Inhaltsinformationen
- Deklarierte Capability-Anforderungen

Eine GUID allein stellt keinen ausreichenden Identitätsnachweis dar.

## Berechtigungsermittlung

Benötigte Capabilities werden aus sämtlichen Graphen und Subgraphs einer Solution ermittelt.

Nur tatsächlich verwendete Capability Nodes begründen eine Berechtigungsanforderung.

Custom Scripts dürfen keine zusätzlichen Capabilities anfordern.

## Autorisierung

Vor einer geschützten Operation prüft NovaOS:

1. Die verifizierte Solution-Identität.
2. Die angeforderte Capability.
3. Den Umfang der erteilten Berechtigung.
4. Die Gültigkeit der Autorisierung.
5. Die Einhaltung geltender Sicherheitsrichtlinien.

Fehlende Berechtigungen werden über den zentralen NovaOS-Berechtigungsdialog behandelt.

Eine verweigerte Berechtigung muss einen definierten Fehler erzeugen.

## Änderungen und Widerruf

Erteilte Berechtigungen können gemäß NovaOS-Richtlinien gespeichert werden.

Sicherheitsrelevante Änderungen an einer Solution müssen erkannt werden und können eine erneute Autorisierung erfordern.

Widerrufene Berechtigungen dürfen nicht weiterverwendet werden.

Laufende Operationen müssen einen Widerruf gemäß ihrem Sicherheitsvertrag berücksichtigen.

## Normative Anforderungen

1. Systemzugriffe MÜSSEN über explizite Capability Nodes erfolgen.
2. Berechtigungen MÜSSEN durch NovaOS geprüft und durchgesetzt werden.
3. Berechtigungen MÜSSEN an die verifizierte Solution-Identität gebunden sein.
4. Die Solution-GUID allein DARF keine Autorisierung begründen.
5. Capability-Anforderungen MÜSSEN aus dem gesamten Logic Graph ermittelt werden.
6. Custom Scripts DÜRFEN keine Berechtigungen eigenständig anfordern.
7. Berechtigungen MÜSSEN auf den erforderlichen Umfang begrenzt werden.
8. Fehlende oder verweigerte Berechtigungen MÜSSEN definiert behandelt werden.
9. Sicherheitsrelevante Solution-Änderungen MÜSSEN eine erneute Berechtigungsprüfung ermöglichen.
10. Widerrufene Berechtigungen DÜRFEN nicht weiterverwendet werden.
11. Sicherheitsrelevante Berechtigungsentscheidungen MÜSSEN nachvollziehbar protokollierbar sein.
12. Das Permission-System MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält ein identitätsgebundenes und zentral durchgesetztes Berechtigungsmodell für Logic Graphs, das Capabilities kontrolliert freigibt und unautorisierte Systemzugriffe verhindert.
