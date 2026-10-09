
# NPSPEC-STUDIO-VERSIONCONTROL-0001 – NovaLang Studio Version Control

## Status

Angenommen

## Kategorie

NovaLang Studio / Versionsverwaltung

## Zweck

Definiert die integrierte Versionsverwaltung für NovaLang-Projekte, Workspaces und NovaOS-Solutions.

Ziel ist die nachvollziehbare Verwaltung von Änderungen mit direkter Integration in Code Editor, Logic Graph und UI Designer.

## Architektur

Die Versionsverwaltung besteht aus:

- **Version Control Manager:** Zentrale Steuerung.
- **Repository Provider:** Anbindung von Versionsverwaltungssystemen.
- **Change Tracker:** Erkennung lokaler Änderungen.
- **Diff Engine:** Vergleich von Dateiversionen.
- **Merge Engine:** Zusammenführung von Änderungen.
- **History Viewer:** Anzeige der Versionshistorie.
- **Conflict Resolver:** Behandlung von Konflikten.

Git wird als primärer Repository Provider unterstützt. Weitere Systeme können über Erweiterungen integriert werden.

## Funktionen

- Repository erstellen und öffnen
- Änderungen erkennen und vergleichen
- Dateien vormerken und committen
- Branches erstellen und wechseln
- Änderungen zusammenführen
- Historie durchsuchen
- Änderungen zurücksetzen
- Remote-Repositories synchronisieren
- Konflikte erkennen und beheben

## Studio-Integration

Die Versionsverwaltung unterstützt:

- `.nova`-Quellcode
- `.nlf`-Logic-Graph-Dateien
- `.nui`-Oberflächen
- Projekt- und Workspace-Konfigurationen
- `solution.xml`
- Ressourcen und Metadaten

Code Editor, Logic Graph und UI Designer müssen Änderungen und Konflikte konsistent darstellen.

## Strukturierte Vergleiche

Neben textbasierten Diffs werden semantische Vergleiche unterstützt, soweit die jeweiligen Dateiformate dies ermöglichen.

Insbesondere:

- Änderungen an Logic-Graph-Knoten und Verbindungen
- Änderungen an UI-Komponenten und Bindungen
- Änderungen an Capability-Anforderungen
- Änderungen an Solution-Metadaten

Unbekannte Inhalte dürfen bei Merge-Vorgängen nicht stillschweigend verworfen werden.

## Solution-Identität

Die dauerhafte Solution-GUID bleibt bei regulären Versionsänderungen erhalten.

Sicherheitsrelevante Änderungen müssen erkannt werden.

Ein Merge oder Checkout darf bestehende Capability-Berechtigungen nicht automatisch auf einen veränderten Solution-Inhalt übertragen.

Die Autorisierung erfolgt weiterhin durch NovaOS anhand der verifizierten Solution-Identität.

## Normative Anforderungen

1. NovaLang Studio MUSS eine integrierte Versionsverwaltung bereitstellen.
2. Git MUSS als Repository Provider unterstützt werden.
3. Änderungen, Commits, Branches und Historie MÜSSEN verwaltbar sein.
4. Lokale und entfernte Repositories MÜSSEN unterstützt werden.
5. Änderungen MÜSSEN direkt im Code Editor erkennbar sein.
6. `.nova`, `.nlf` und `.nui` MÜSSEN versionsverwaltbar sein.
7. Strukturierte Diffs für Logic Graph und UI SOLLEN unterstützt werden.
8. Merge-Konflikte MÜSSEN erkennbar und kontrolliert lösbar sein.
9. Nicht gespeicherte Änderungen DÜRFEN bei Repository-Operationen nicht unbeabsichtigt verloren gehen.
10. Sicherheitsrelevante Solution-Änderungen MÜSSEN eine erneute Berechtigungsprüfung ermöglichen.
11. Remote-Zugriffe MÜSSEN die NovaOS-Capability- und Authentifizierungsregeln einhalten.
12. Die Versionsverwaltung MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält eine integrierte Versionsverwaltung für Quellcode, Logic Graphs und Benutzeroberflächen mit nachvollziehbarer Historie, kontrollierten Merge-Vorgängen und sicherer Solution-Integration.
