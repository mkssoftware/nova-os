# NPSPEC-APP-DOCUMENTCENTRIC-0001 – Nova Document-Centric App

## Status

Angenommen

## Kategorie

App / Document-Centric

## Zweck

NovaOS definiert ein dokumentzentriertes App-Modell, bei dem der Nutzer primär mit Datenobjekten und deren Inhalt arbeitet, nicht mit einer fest zugeordneten Anwendung.

Ein Dokument kann abhängig von Aufgabe und Kontext unterschiedliche Capabilities, Components, Services oder Programme verwenden.

## Grundprinzipien

```text
Document ≠ Application
File Type ≠ Owning App
Open ≠ Launch Fixed Program
ObjectID ≠ Path
Content ≠ Presentation
Capability ≠ File Association
```

## Modell

```text
Document
├── ObjectID
├── SemanticType
├── Content
├── Metadata
├── Relations[]
└── Available Actions[]
```

Die verfügbaren Aktionen ergeben sich aus semantischem Typ, Inhalt, Authority und verfügbaren Capabilities.

## Öffnen eines Dokuments

Das Öffnen eines Dokuments muss nicht automatisch eine fest zugeordnete monolithische Anwendung starten.

```text
Document
    ↓
Semantic Type
    ↓
Available Capabilities
    ↓
Task / User Intent
    ↓
Suitable UI + Execution
```

NovaOS kann daraus eine passende Arbeitsoberfläche erzeugen oder eine vorhandene App, Solution oder Capability-Komposition verwenden.

## Aktionen

Ein Bild kann beispielsweise Aktionen anbieten wie:

```text
View
Crop
Resize
Rotate
Convert
Annotate
Share
Print
```

Diese Funktionen müssen nicht Bestandteil eines einzelnen Bildbearbeitungsprogramms sein.

Sie können von unterschiedlichen Capability-Providern bereitgestellt werden.

## Semantische Typen

Die Auswahl möglicher Funktionen soll primär auf semantischen Typen beruhen:

```text
ObjectID
   ↓
SemanticType
   ↓
Compatible Operations
   ↓
Available Providers
```

Dateiendungen dürfen als Hinweis oder Kompatibilitätsmerkmal verwendet werden, bestimmen jedoch nicht allein die Semantik.

## Benutzerabsicht

Der aktuelle Arbeitskontext darf die angebotenen Funktionen beeinflussen.

```text
Document
    +
User Intent
    +
Workspace Context
    +
Available Capabilities
       ↓
Effective Toolset
```

Dadurch kann NovaOS nur die für die aktuelle Aufgabe relevanten Werkzeuge hervorheben.

## Bearbeitung

Änderungen an Dokumenten müssen über die normalen NovaOS-Mechanismen für Authority, Transaktionen und Objektidentität erfolgen.

Eine Bearbeitung darf die stabile `ObjectID` nicht allein deshalb verändern, weil ein anderer Capability-Provider verwendet wird.

## Providerwechsel

Capability-Provider dürfen ausgetauscht werden, solange:

```text
Semantic Contract
Compatibility
Authority
Trust
Execution Contract
```

erhalten bleiben.

Der Nutzer soll dadurch nicht von einem bestimmten Programm abhängig sein.

## Klassische Programme

Das dokumentzentrierte Modell ersetzt klassische Programme nicht.

Ein Nutzer darf weiterhin explizit ein bestimmtes Programm zum Öffnen oder Bearbeiten eines Dokuments auswählen.

NovaOS unterstützt damit beide Modelle:

```text
Document → Task → Capabilities

Document → Selected Program
```

## Sicherheit

Das Öffnen eines Dokuments erzeugt keine zusätzliche Authority.

Ein verwendeter Provider erhält ausschließlich die für die konkrete Operation autorisierten Ressourcen und Capabilities.

Dokumentinhalt darf nicht allein aufgrund seines Dateityps ausführbare Authority erhalten.

## Normative Anforderungen

1. Dokumente MÜSSEN unabhängig von einer fest zugeordneten App existieren können.
2. Dokumentidentität MUSS von Pfad und verwendeter App unabhängig bleiben.
3. NovaOS MUSS Aktionen anhand semantischer Typen ermitteln können.
4. Dateiendungen DÜRFEN nicht allein die verfügbare Funktionalität bestimmen.
5. Mehrere Provider für dieselbe Dokumentoperation MÜSSEN unterstützt werden können.
6. Providerwechsel DARF die Dokumentidentität nicht verändern.
7. Nutzer MÜSSEN weiterhin explizit ein bestimmtes Programm auswählen können.
8. Dokumentzentrierte Ausführung DARF keine zusätzliche Authority erzeugen.
9. Provider DÜRFEN nur die für ihre Operation erforderliche Authority erhalten.
10. Bearbeitungen MÜSSEN die NovaOS-Transaktions- und Sicherheitsmechanismen verwenden.
11. Workspace und User Intent DÜRFEN die angebotenen Werkzeuge beeinflussen.
12. Verwendete Operation, Capability, Provider und effektive Authority MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-APP-MODEL-0001`
- `NPSPEC-APP-CAPABILITYCOMPOSED-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-SEMANTIC-0001`
- `NPSPEC-FILESYSTEM-TRANSACTION-0001`
- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-SEMANTICTYPE-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-WORKSPACE-MODEL-0001`

## Ergebnis

NovaOS behandelt Dokumente als eigenständige semantische Objekte statt als Anhängsel bestimmter Anwendungen. Funktionen können passend zu Dokument, Aufgabe und Kontext aus Capabilities zusammengesetzt werden, während klassische Programme weiterhin gezielt verwendet werden können.