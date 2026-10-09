
# NPSPEC-SOLUTION-IDENTITY-0001 – NovaOS Solution Identity

## Status

Angenommen

## Kategorie

NovaOS / Solutions / Identität und Sicherheit

## Zweck

Definiert die dauerhafte und überprüfbare Identität einer NovaOS-Solution.

Ziel ist die eindeutige Zuordnung von Solutions, Berechtigungen und vertrauenswürdigen Aktualisierungen.

## Architektur

Das Identity-System besteht aus:

- **Solution Identity Manager:** Verwaltung der Solution-Identität.
- **Identity Registry:** Registrierung bekannter Solutions.
- **Integrity Verifier:** Prüfung der Solution-Integrität.
- **Signature Validator:** Prüfung kryptografischer Signaturen.
- **Permission Registry Bridge:** Zuordnung erteilter Capability-Berechtigungen.

## Identitätsmodell

Jede Solution besitzt eine dauerhaft eindeutige GUID.

Die `solution.xml` enthält mindestens:

- Solution-GUID
- Name und Version
- Herausgeberinformationen
- Deklarierte Capability-Anforderungen
- Referenzen auf Integritäts- und Signaturinformationen

Die GUID bleibt bei regulären Updates erhalten.

Eine GUID allein ist kein vertrauenswürdiger Identitätsnachweis.

## Vertrauenswürdige Identität

Die vertrauenswürdige Solution-Identität ergibt sich aus:

- Deklarierter Solution-GUID
- Verifizierter Herausgeberidentität
- Kryptografischer Integritätsprüfung
- Gültiger Vertrauenskette

Die Identity Registry darf Solutions nicht allein aufgrund übereinstimmender GUIDs als identisch vertrauen.

## Berechtigungsbindung

Erteilte Capability-Berechtigungen werden in der zentralen Berechtigungs-Registry an die verifizierte Solution-Identität gebunden.

Bei unveränderter vertrauenswürdiger Identität und unveränderten Berechtigungsanforderungen können bestehende Freigaben wiederverwendet werden.

Neue oder erweiterte Capability-Anforderungen erfordern eine erneute Berechtigungsentscheidung.

## Änderungen und Updates

Bei jedem Start und nach relevanten Änderungen wird die Integrität entsprechend der Sicherheitsrichtlinie überprüft.

Reguläre Updates dürfen die GUID beibehalten, sofern die Herausgeberidentität vertrauenswürdig bestätigt wird.

Manipulierte oder nicht vertrauenswürdige Änderungen dürfen bestehende Berechtigungen nicht automatisch übernehmen.

Ein Herausgeberwechsel benötigt eine ausdrücklich autorisierte Identitätsmigration.

## Sicherheit

Solutions dürfen ihre Identität nicht durch Kopieren oder Verändern einer GUID übernehmen.

Custom Scripts, Logic Graphs und UI-Definitionen dürfen keine eigenen Berechtigungen erzeugen.

Die tatsächliche Autorisierung erfolgt ausschließlich durch NovaOS.

## Normative Anforderungen

1. Jede Solution MUSS eine dauerhaft eindeutige GUID besitzen.
2. Die GUID MUSS in `solution.xml` gespeichert werden.
3. Eine GUID allein DARF nicht als Identitätsnachweis gelten.
4. Vertrauenswürdige Identitäten MÜSSEN kryptografisch überprüfbar sein.
5. Capability-Berechtigungen MÜSSEN an die verifizierte Solution-Identität gebunden werden.
6. Reguläre Updates DÜRFEN die bestehende GUID beibehalten.
7. Neue Capability-Anforderungen MÜSSEN erneut autorisiert werden.
8. Manipulierte Solutions DÜRFEN bestehende Berechtigungen nicht automatisch übernehmen.
9. Herausgeberwechsel MÜSSEN ausdrücklich autorisiert werden.
10. Identitäts- und Integritätsfehler MÜSSEN diagnostizierbar sein.
11. Solutions DÜRFEN fremde Identitäten oder Berechtigungen nicht übernehmen.
12. Die Identitätsprüfung MUSS unabhängig von NovaLang Studio und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine dauerhafte, kryptografisch überprüfbare Solution-Identität, die sichere Updates und die zuverlässige Wiederverwendung autorisierter Capability-Berechtigungen ermöglicht.
