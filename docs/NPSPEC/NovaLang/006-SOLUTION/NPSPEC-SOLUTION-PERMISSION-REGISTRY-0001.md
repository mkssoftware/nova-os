
# NPSPEC-SOLUTION-PERMISSION-REGISTRY-0001 – NovaOS Solution Permission Registry

## Status

Angenommen

## Kategorie

NovaOS / Solutions / Sicherheit / Berechtigungsverwaltung

## Zweck

Definiert die zentrale Registrierung und Verwaltung dauerhaft erteilter Capability-Berechtigungen für NovaOS-Solutions.

Ziel ist, autorisierte Systemfähigkeiten ohne wiederholte Berechtigungsabfragen nutzen zu können, solange die verifizierte Solution-Identität und die Sicherheitsbedingungen unverändert bleiben.

## Architektur

Die Permission Registry besteht aus:

- **Permission Registry:** Geschützte Speicherung erteilter Berechtigungen.
- **Identity Resolver:** Zuordnung zur verifizierten Solution-Identität.
- **Integrity Validator:** Prüfung der Solution-Integrität.
- **Permission Evaluator:** Entscheidung über angeforderte Zugriffe.
- **Consent Manager:** Verwaltung erforderlicher Benutzerfreigaben.
- **Revocation Manager:** Widerruf bestehender Berechtigungen.
- **Audit Logger:** Nachvollziehbarkeit sicherheitsrelevanter Entscheidungen.

Die Registry ist ein zentraler NovaOS-Systemdienst und keine Komponente einzelner Solutions.

## Berechtigungsmodell

Ein Berechtigungseintrag enthält:

- Solution-GUID
- Verifizierten Herausgeber beziehungsweise Vertrauensanker
- Integritäts- oder Versionsbindung
- Vollständige Capability-ID
- Genehmigten Berechtigungsumfang
- Gültigkeitsbedingungen
- Berechtigungsstatus
- Zeitpunkt und Ursprung der Freigabe

Capability-IDs verwenden das Format `domain.authority.namespace.name`.

Die GUID identifiziert eine Solution, ist allein jedoch kein ausreichender Identitätsnachweis.

## Berechtigungsablauf

1. Die Solution wird anhand ihrer `solution.xml` identifiziert.
2. Integrität und vertrauenswürdige Identitätsbindung werden geprüft.
3. Die Logic Graph Runtime ermittelt die benötigten Capabilities.
4. Die Permission Registry prüft vorhandene Freigaben.
5. Fehlende Berechtigungen werden gemäß Systemrichtlinie angefragt oder verweigert.
6. Genehmigte Berechtigungen werden sicher registriert.
7. Capability-Zugriffe werden zur Laufzeit erneut autorisiert.

Eine unveränderte, gültig autorisierte Solution benötigt keine erneute Benutzerabfrage.

## Änderungen und Widerruf

Sicherheitsrelevante Änderungen an Solution, Logic Graph oder Capability-Anforderungen lösen eine erneute Berechtigungsprüfung aus.

Nicht sicherheitsrelevante Änderungen dürfen bestehende Freigaben erhalten, sofern Identität und Integritätsrichtlinien dies zulassen.

Berechtigungen können jederzeit widerrufen werden. Der Widerruf muss auch bereits laufende Zugriffe gemäß Capability-Vertrag erfassen.

## Sicherheit

Custom Scripts dürfen keine Berechtigungen selbst anfordern oder erweitern.

Systemzugriffe erfolgen ausschließlich über autorisierte Capability Nodes.

Die Registry muss gegen Manipulation durch Solutions geschützt sein.

Gespeicherte Berechtigungen dürfen nicht allein durch Kopieren einer GUID oder `solution.xml` auf eine andere Solution übertragen werden.

## Normative Anforderungen

1. NovaOS MUSS eine zentrale Permission Registry bereitstellen.
2. Berechtigungen MÜSSEN an eine verifizierte Solution-Identität gebunden sein.
3. Die Solution-GUID MUSS dauerhaft eindeutig sein, DARF jedoch nicht allein zur Authentifizierung dienen.
4. Jede Freigabe MUSS eine konkrete Capability-ID und einen definierten Umfang besitzen.
5. Vorhandene gültige Freigaben MÜSSEN ohne erneute Benutzerabfrage nutzbar sein.
6. Sicherheitsrelevante Änderungen MÜSSEN eine erneute Berechtigungsprüfung auslösen.
7. Berechtigungen MÜSSEN widerrufbar sein.
8. Capability-Zugriffe MÜSSEN zur Laufzeit autorisiert werden.
9. Custom Scripts DÜRFEN Berechtigungen weder anfordern noch umgehen.
10. Solutions DÜRFEN die Permission Registry nicht eigenständig verändern.
11. Berechtigungsentscheidungen MÜSSEN nachvollziehbar protokolliert werden.
12. Die Berechtigungsverwaltung MUSS ohne KI und unabhängig von NovaLang Studio funktionieren.

## Ergebnis

NovaOS erhält eine zentrale, manipulationsgeschützte und identitätsgebundene Berechtigungsverwaltung, die wiederholte Freigabeabfragen vermeidet und gleichzeitig sicherheitsrelevante Änderungen sowie den Widerruf von Capability-Zugriffen zuverlässig berücksichtigt.
