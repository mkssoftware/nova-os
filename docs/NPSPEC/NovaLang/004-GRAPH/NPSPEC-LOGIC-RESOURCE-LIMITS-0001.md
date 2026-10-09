
# NPSPEC-LOGIC-RESOURCE-LIMITS-0001 – NovaOS Logic Graph Resource Limits

## Status

Angenommen

## Kategorie

NovaOS / Logic Graph / Ressourcenverwaltung

## Zweck

Definiert die Begrenzung und Überwachung des Ressourcenverbrauchs innerhalb eines Logic Graphs.

Ziel ist die zuverlässige Ausführung von Solutions ohne unkontrollierte Ressourcenbelegung oder Beeinträchtigung anderer Systemkomponenten.

## Architektur

Das Resource-Limit-System besteht aus:

- **Resource Budget Manager:** Verwaltung verfügbarer Ressourcenbudgets.
- **Resource Monitor:** Überwachung des tatsächlichen Verbrauchs.
- **Limit Enforcer:** Durchsetzung festgelegter Grenzen.
- **Quota Controller:** Verwaltung gleichzeitiger Ressourcenanforderungen.
- **Cancellation Controller:** Abbruch bei Grenzüberschreitungen.
- **Resource Diagnostics:** Erfassung und Meldung von Ressourcenproblemen.

Die Durchsetzung erfolgt gemeinsam mit NovaLang Runtime und NovaOS-Ressourcenverwaltung.

## Ressourcenarten

Begrenzt werden können:

- CPU-Zeit und Rechenbudget
- Arbeitsspeicher und Heap
- Maximale Ausführungsdauer
- Anzahl paralleler Aufgaben
- Warteschlangen und Ereignispuffer
- Rekursions- und Verschachtelungstiefe
- Offene Ressourcenreferenzen
- Datenübertragungsvolumen

Weitere Ressourcenarten können über versionierte Verträge ergänzt werden.

## Budgetmodell

Ressourcenbudgets werden hierarchisch verwaltet:

**Solution → Graph → Subgraph → Execution → Node**

Untergeordnete Budgets dürfen die Grenzen ihres übergeordneten Kontexts nicht überschreiten.

Nicht genutzte Ressourcen dürfen gemäß NovaOS-Richtlinien dynamisch zugeteilt werden.

## Grenzwertbehandlung

Bei Annäherung an Ressourcenlimits können Warnungen ausgegeben werden.

Bei Grenzüberschreitungen werden betroffene Ausführungen kontrolliert gedrosselt, angehalten oder abgebrochen.

Bereits reservierte Ressourcen müssen zuverlässig freigegeben werden.

## Capability-Integration

Capability Nodes unterliegen zusätzlich den Ressourcen- und Berechtigungsregeln ihrer jeweiligen Systemfähigkeit.

Custom Scripts dürfen Ressourcenlimits weder verändern noch umgehen.

Eine Capability-Autorisierung bedeutet keine unbegrenzte Ressourcennutzung.

## Normative Anforderungen

1. Jede Graph-Ausführung MUSS einem Ressourcenbudget zugeordnet sein.
2. Ressourcenbudgets MÜSSEN hierarchisch verwaltbar sein.
3. Untergeordnete Kontexte DÜRFEN übergeordnete Limits nicht überschreiten.
4. CPU-, Speicher- und Zeitlimits MÜSSEN durchsetzbar sein.
5. Parallelität und Warteschlangen MÜSSEN begrenzbar sein.
6. Rekursion und verschachtelte Ausführungen MÜSSEN kontrollierbar sein.
7. Ressourcenverbrauch MUSS überwacht werden können.
8. Grenzüberschreitungen MÜSSEN definiert behandelt werden.
9. Abgebrochene Ausführungen MÜSSEN Ressourcen kontrolliert freigeben.
10. Custom Scripts und Capability Nodes DÜRFEN Ressourcenlimits nicht umgehen.
11. Ressourcenfehler MÜSSEN diagnostizierbar sein.
12. Das Resource-Limit-System MUSS unabhängig vom grafischen Editor und ohne KI funktionieren.

## Ergebnis

NovaOS erhält eine hierarchische und durchsetzbare Ressourcenverwaltung für Logic Graphs, die faire Ressourcennutzung, kontrollierte Ausführung und Systemstabilität gewährleistet.
