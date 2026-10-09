
# NPSPEC-SOLUTION-UI-BINDING-0001 – NovaOS Solution UI Binding

## Status

Angenommen

## Kategorie

NovaOS / Solution / UI Binding

## Zweck

Definiert die Verbindung zwischen deklarativen Benutzeroberflächen, Logic Graphs und Solution-Zuständen.

Ziel ist eine typsichere, reaktive und sichere Datenübertragung zwischen UI und Anwendungslogik ohne direkte Systemzugriffe aus der Oberfläche.

## Architektur

Das UI-Binding-System besteht aus:

- **Binding Manager:** Verwaltung aller UI-Bindungen.
- **Binding Resolver:** Auflösung von Datenquellen und Zielen.
- **State Observer:** Überwachung relevanter Zustandsänderungen.
- **Update Dispatcher:** Koordination von UI-Aktualisierungen.
- **Event Bridge:** Weiterleitung von UI-Ereignissen an Logic Graphs.
- **Binding Validator:** Prüfung von Typen und Zugriffsregeln.

Das System verbindet den deklarativen `.nui`-Renderer mit der Solution Logic Runtime.

## Binding-Modell

Unterstützt werden:

- **OneWay:** Datenfluss vom Solution-Zustand zur UI.
- **TwoWay:** Synchronisation zwischen UI und freigegebenem Zustand.
- **OneTime:** Einmalige Übernahme eines Wertes.
- **Event Binding:** Weiterleitung von Benutzeraktionen an Graph-Eingänge.
- **Command Binding:** Auslösung definierter Logic-Graph-Operationen.

Bindungen referenzieren stabile, typisierte Schnittstellen und keine internen Speicheradressen.

## Datenfluss

Der reguläre Datenfluss lautet:

**Logic Graph → Solution State → UI Binding → UI Element**

Benutzeraktionen werden über den umgekehrten Ereignispfad verarbeitet:

**UI Element → Event Binding → Logic Graph → Solution State**

Die UI darf keine Capability Nodes eigenständig ausführen.

## NovaLang- und NUI-Integration

`.nui` verwendet die deklarative NovaLang-Syntax.

Binding-Ausdrücke werden anhand des NovaLang-Typsystems geprüft.

Die UI-Beschreibung definiert Darstellung und Bindungen, während der Logic Graph die Ausführungslogik kontrolliert.

## Reaktive Aktualisierung

Änderungen an beobachteten Zuständen lösen gezielte UI-Aktualisierungen aus.

Unterstützt werden:

- Änderungsbenachrichtigungen
- Zusammenfassung mehrerer Aktualisierungen
- Vermeidung unnötiger Neuberechnungen
- Erkennung zyklischer Bindungen
- Kontrollierte Fehlerbehandlung

UI-Aktualisierungen dürfen die Graph Runtime nicht blockieren.

## Lebenszyklus

Bindings werden beim Erstellen einer UI-Instanz aufgelöst und validiert.

Beim Entfernen eines UI-Elements oder Schließen der Solution werden zugehörige Beobachter und Ressourcen freigegeben.

Ungültige Bindungen müssen diagnostiziert werden, ohne die gesamte Oberfläche unnötig zu blockieren.

## Sicherheit

UI-Bindungen dürfen ausschließlich freigegebene Solution-Zustände und deklarierte Graph-Schnittstellen verwenden.

TwoWay-Bindings dürfen keine schreibgeschützten oder geschützten Zustände verändern.

Berechtigungen werden ausschließlich über die verifizierte Solution-Identität und die zentrale NovaOS-Berechtigungsverwaltung kontrolliert.

## Normative Anforderungen

1. Solutions MÜSSEN deklarative UI-Bindungen unterstützen.
2. OneWay, TwoWay, OneTime und Event Binding MÜSSEN verfügbar sein.
3. Bindungen MÜSSEN typisiert und validierbar sein.
4. UI-Ereignisse MÜSSEN über definierte Logic-Graph-Schnittstellen verarbeitet werden.
5. Zustandsänderungen MÜSSEN reaktive UI-Aktualisierungen auslösen können.
6. Zyklische Bindungen MÜSSEN erkannt und kontrolliert behandelt werden.
7. UI-Aktualisierungen DÜRFEN die Graph Runtime nicht blockieren.
8. Binding-Ressourcen MÜSSEN beim Ende ihres Lebenszyklus freigegeben werden.
9. Schreibgeschützte Zustände DÜRFEN nicht über TwoWay-Bindings verändert werden.
10. UI-Elemente DÜRFEN keine Capabilities direkt anfordern oder ausführen.
11. Bindungen DÜRFEN keine Solution- oder Capability-Grenzen umgehen.
12. Das UI-Binding-System MUSS unabhängig von NovaLang Studio und ohne KI funktionieren.

## Ergebnis

NovaOS erhält ein reaktives, typsicheres und sicheres UI-Binding-System, das deklarative Oberflächen mit Solution-Zuständen und Logic Graphs verbindet, ohne Darstellung und Ausführungslogik unkontrolliert zu vermischen.
