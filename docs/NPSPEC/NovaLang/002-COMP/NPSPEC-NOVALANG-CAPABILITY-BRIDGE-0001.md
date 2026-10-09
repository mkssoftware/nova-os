
# NPSPEC-NOVALANG-CAPABILITY-BRIDGE-0001 – NovaLang Capability Bridge

## Status

Angenommen

## Kategorie

NovaLang / Runtime / Capability-System

## Zweck

Definiert die Capability Bridge als sichere Verbindung zwischen NovaLang-Code und den Systemfähigkeiten von NovaOS.

Sie ermöglicht typisierte Systemzugriffe, ohne Programmen direkten Zugriff auf privilegierte Kernel- oder Systemfunktionen zu gewähren.

## Architektur

Die Capability Bridge ist Bestandteil der NovaLang Runtime.

| Komponente | Aufgabe |
|---|---|
| Capability Resolver | Auflösung registrierter Fähigkeiten |
| Contract Validator | Prüfung von Typen und Schnittstellen |
| Handle Manager | Verwaltung autorisierter Capability-Handles |
| Invocation Gateway | Kontrollierte Ausführung von Operationen |
| Policy Enforcement | Durchsetzung der NovaOS-Berechtigungen |
| Audit Interface | Nachvollziehbarkeit von Zugriffen |

Die verbindliche Autorisierung erfolgt durch das NovaOS-Capability-System.

## Capability-Identität

Capabilities besitzen eine eindeutige Kennung nach folgendem Schema:

`domain.authority.namespace.name`

Beispiel:

`de.nova.network.http.request`

Die Kennung identifiziert die Fähigkeit, erteilt jedoch keine Berechtigung.

## Zugriffsmodell

1. NovaOS autorisiert eine Capability für den jeweiligen Ausführungskontext.
2. Die Runtime erhält ein gültiges, eingeschränktes Capability-Handle.
3. NovaLang-Code ruft die typisierte Schnittstelle auf.
4. Die Bridge validiert Handle, Vertrag und Zugriffsbedingungen.
5. Die Operation wird über den autorisierten Systemdienst ausgeführt.
6. Ergebnis oder Fehler wird an den Aufrufer zurückgegeben.

## NovaLang-Verwendung

Beispiel für eine bereits autorisierte Capability:

```vb
Public Async Function LadeDaten(
    netzwerk As INetworkRequest
) As Task(Of String)

    Return Await netzwerk.GetTextAsync("https://example.org")
End Function
```

`INetworkRequest` ist eine beispielhafte Schnittstelle. Ihre konkrete API wird durch die jeweilige Capability-Spezifikation definiert.

Die Funktion kann keine Netzwerkberechtigung selbst erzeugen.

## Logic Graph und Solutions

In Solutions werden benötigte Capabilities ausdrücklich im Logic Graph verbunden.

Beispiel:

`Netzwerk-Capability → Custom Script → Datenverarbeitung`

- Custom Scripts erhalten ausschließlich bereitgestellte Daten und Handles.
- Scripts dürfen keine zusätzlichen Capabilities selbst anfordern.
- Berechtigungen werden an die geprüfte Solution-Identität gebunden.
- Änderungen an sicherheitsrelevanten Capability-Abhängigkeiten erfordern eine erneute Berechtigungsprüfung.
- Die Solution-GUID allein gilt nicht als Sicherheitsnachweis.

## Sicherheit

- Capability-Handles müssen gegen Fälschung geschützt sein.
- Zugriffe müssen auf den autorisierten Umfang begrenzt bleiben.
- Widerrufene Handles dürfen nicht weiterverwendet werden.
- Handle-Übertragungen benötigen ausdrückliche Autorisierung.
- `Imports`, Metadaten und Capability-IDs dürfen keine Berechtigungen erzeugen.
- AOT-, JIT- und Interpreter-Code unterliegen denselben Zugriffsregeln.

## Fehlerbehandlung

Die Bridge muss folgende Fehler kontrolliert behandeln:

- Capability nicht verfügbar
- Berechtigung verweigert
- Handle ungültig oder widerrufen
- Schnittstellen- oder Versionskonflikt
- Zeitüberschreitung oder Ressourcenlimit
- Fehler des ausführenden Systemdienstes

Fehler werden über die definierten NovaLang-Mechanismen zurückgegeben.

## Normative Anforderungen

1. NovaLang MUSS Systemfähigkeiten über eine kontrollierte Capability Bridge verwenden.
2. Capability-Aufrufe MÜSSEN typisierte und versionierte Verträge besitzen.
3. Jede geschützte Operation MUSS durch NovaOS autorisiert sein.
4. Capability-Handles MÜSSEN fälschungssicher und widerrufbar sein.
5. Berechtigungen DÜRFEN NICHT allein aus Capability-IDs oder `Imports` entstehen.
6. Custom Scripts DÜRFEN keine zusätzlichen Systemberechtigungen selbst anfordern.
7. Capability-Übertragungen MÜSSEN ausdrücklich autorisiert werden.
8. AOT, JIT und Interpreter MÜSSEN identische Capability-Sicherheitsregeln einhalten.
9. Die Bridge MUSS ohne .NET-Runtime funktionieren.

## Ergebnis

NovaLang erhält eine native, typisierte und sichere Capability Bridge, die Programme und Solutions mit NovaOS-Systemfähigkeiten verbindet, ohne das Prinzip der minimalen Berechtigung oder die Isolation der Ausführungskontexte zu verletzen.
