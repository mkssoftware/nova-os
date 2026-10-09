
# NPSPEC-LOGIC-TYPES-0001 – NovaOS Logic Graph Type System

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Typsystem

## Zweck

Definiert das einheitliche Typsystem für Knoten, Ports und Verbindungen innerhalb eines NovaOS Logic Graphs.

Ziel ist die vollständige Typkompatibilität mit NovaLang sowie die frühzeitige Erkennung ungültiger Datenflüsse.

## Architektur

Das Logic-Graph-Typsystem besteht aus:

- **Type Registry:** Verwaltung verfügbarer Typen.
- **Type Resolver:** Auflösung von Typreferenzen.
- **Type Checker:** Prüfung der Typkompatibilität.
- **Type Inference:** Ableitung eindeutig bestimmbarer Typen.
- **Conversion Resolver:** Prüfung zulässiger Typumwandlungen.
- **Type Diagnostics:** Meldung von Typfehlern.

NovaLang ist die verbindliche Grundlage für Typidentität und Typsemantik.

## Unterstützte Typen

- Primitive NovaLang-Datentypen
- Klassen und Strukturen
- Interfaces
- Generische Typen
- Nullable-Typen
- Enumerationen
- Collections
- Ereignis- und asynchrone Ergebnistypen
- Kontrollierte Ressourcen- und Capability-Referenzen

Eigene Logic-Graph-Datentypen dürfen keine widersprüchliche Parallelsemantik einführen.

## Typkompatibilität

Verbindungen sind zulässig, wenn die Typen gemäß NovaLang kompatibel sind.

Dabei gelten:

- Statische Typprüfung
- Definierte Zuweisungskompatibilität
- Generische Typregeln
- Nullability-Regeln
- Explizite und zulässige implizite Konvertierungen

Unsichere Konvertierungen müssen ausdrücklich gekennzeichnet oder abgelehnt werden.

## Typinferenz

Nicht explizit angegebene Typen dürfen aus eindeutigen Knoten- und Portverträgen abgeleitet werden.

Mehrdeutige oder widersprüchliche Typinformationen müssen als Diagnose ausgegeben werden.

Typinferenz darf keine Capability-Berechtigungen ableiten oder erzeugen.

## Capability-Typen

Capability Nodes können typisierte Daten und autorisierte Ressourcenreferenzen bereitstellen.

Ressourcentypen müssen ihre Zugriffs- und Lebenszyklusverträge erhalten.

Eine Typkonvertierung darf keine Erweiterung bestehender Berechtigungen bewirken.

## Normative Anforderungen

1. Das Logic-Graph-Typsystem MUSS die NovaLang-Typsemantik verwenden.
2. Jeder Datenport MUSS einen auflösbaren Datentyp besitzen.
3. Verbindungen MÜSSEN statisch auf Typkompatibilität geprüft werden.
4. Generics und Nullable-Typen MÜSSEN unterstützt werden.
5. Eindeutige Typinferenz MUSS unterstützt werden.
6. Mehrdeutige Typen MÜSSEN diagnostiziert werden.
7. Implizite Konvertierungen DÜRFEN nur gemäß NovaLang-Regeln erfolgen.
8. Unsichere Konvertierungen DÜRFEN nicht automatisch eingefügt werden.
9. Capability- und Ressourcenrechte DÜRFEN durch Typumwandlungen nicht erweitert werden.
10. Typänderungen MÜSSEN betroffene Verbindungen erneut validieren.
11. Typprüfung MUSS unabhängig vom grafischen Editor verfügbar sein.
12. Das Typsystem MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaOS erhält ein einheitliches, statisch geprüftes Logic-Graph-Typsystem, das vollständig mit NovaLang kompatibel ist und sichere Datenflüsse zwischen Capabilities, Scripts und weiteren Knoten gewährleistet.
