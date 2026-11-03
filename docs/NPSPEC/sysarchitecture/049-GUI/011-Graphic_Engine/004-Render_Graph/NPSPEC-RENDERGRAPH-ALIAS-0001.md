# NPSPEC-RENDERGRAPH-ALIAS-0001 – Nova Transient Resource Aliasing

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Render Graph

## 1. Zweck

Nova Transient Resource Aliasing definiert die gemeinsame Nutzung physischer Speicherbereiche durch temporäre Grafikressourcen innerhalb des Nova Render Graph.

Ziel ist die Reduzierung des GPU-Speicherverbrauchs, indem Ressourcen mit nicht überlappenden Nutzungszeiten denselben Speicher verwenden können.

Die Wiederverwendung erfolgt ohne unnötige Speicherallokationen oder Datenkopien.

## 2. Architekturprinzipien

- **Lifetime-Based:** Speicherwiederverwendung basiert auf tatsächlichen Ressourcenlebenszeiten.
- **Dependency-Aware:** Ausführungsabhängigkeiten bestimmen sichere Wiederverwendung.
- **Zero-Copy:** Ressourcen können denselben Speicher direkt verwenden.
- **Backend-Aware:** Hardwareabhängige Speicheranforderungen werden berücksichtigt.
- **Synchronization-Safe:** Laufende GPU-Zugriffe werden geschützt.
- **Incremental:** Gültige Speicherzuordnungen können erhalten bleiben.
- **Budget-Aware:** Speicherverbrauch wird innerhalb definierter Budgets optimiert.
- **Fallback-Safe:** Separate Speicherallokationen bleiben möglich.

## 3. Aliasing Model

Eine Aliasing-Zuordnung beschreibt die gemeinsame Verwendung eines physischen Speicherbereichs.

Sie enthält mindestens:

- **Alias Group ID:** Identifikation der Speichergruppe.
- **Resource IDs:** Zugeordnete logische Ressourcen.
- **Memory Requirements:** Größe, Alignment und Speicherklasse.
- **Lifetime Intervals:** Nutzungszeiträume der Ressourcen.
- **Physical Allocation:** Gemeinsamer Speicherbereich.
- **Synchronization Requirements:** Erforderliche Übergänge.

Logische Ressourcenidentitäten MÜSSEN unabhängig von der physischen Speicherzuordnung bleiben.

## 4. Voraussetzungen

Zwei Ressourcen DÜRFEN denselben Speicher verwenden, wenn:

1. Ihre tatsächlichen Nutzungszeiten nicht überlappen.
2. Sämtliche Zugriffe korrekt synchronisiert sind.
3. Ihre Speicheranforderungen kompatibel sind.
4. Das Rendering-Backend die Speicherüberlagerung unterstützt.
5. Keine erforderlichen Inhalte überschrieben werden.

Die zeitliche Reihenfolge MUSS durch gültige Ausführungsabhängigkeiten abgesichert sein.

Eine lediglich geplante Reihenfolge ohne entsprechende Synchronisation reicht nicht aus.

## 5. Lifetime Analysis

Der Nova Render Graph ermittelt die Lebensdauer temporärer Ressourcen anhand ihrer Zugriffe.

Dabei MÜSSEN berücksichtigt werden:

- Erster und letzter Ressourcenzugriff
- Parallele Render Passes
- Asynchrone GPU Queues
- Ressourcenabhängigkeiten
- Ausstehende GPU-Ausführungen
- Externe Ressourcenverwendung

Bei nicht eindeutig geordneten Nutzungszeiten MUSS eine mögliche Überlappung angenommen werden.

## 6. Aliasing Allocation

Der Resource Planner SOLL kompatible Ressourcen zu Aliasing-Gruppen zusammenfassen.

Dabei werden berücksichtigt:

- Speichergröße
- Alignment
- Ressourcenformat
- Speicherklasse
- Nutzungsintervalle
- Hardwarebeschränkungen

Die physische Allokation erfolgt durch den Nova GPU Resource Manager.

Die Optimierung SOLL den gesamten Speicherbedarf reduzieren, ohne unverhältnismäßigen Verwaltungsaufwand zu erzeugen.

## 7. Resource Initialization

Beim Wechsel zwischen Ressourcen einer Aliasing-Gruppe gelten vorherige Speicherinhalte grundsätzlich als undefiniert.

NovaOS MUSS sicherstellen, dass erforderliche Inhalte vor ihrer Verwendung initialisiert werden.

Dazu können gehören:

- Clear Operations
- Vollständige Schreiboperationen
- Definierte Initialisierungsdaten

Nicht initialisierte Speicherinhalte DÜRFEN weder für Rendering-Ergebnisse noch für den Zugriff anderer Prozesse offengelegt werden.

## 8. Synchronization

Aliasing erfordert eine korrekte Ordnung der Speicherzugriffe.

NovaOS MUSS:

- Vorherige Ressourcenzugriffe abschließen oder entsprechend synchronisieren.
- Erforderliche Speicherbarrieren einplanen.
- Queue-übergreifende Zugriffe berücksichtigen.
- Ungültige gleichzeitige Speichernutzung verhindern.

Die konkrete Synchronisation erfolgt über Nova Render Resource Barriers und Nova GPU Synchronization.

## 9. Incremental Aliasing

NovaOS SOLL bestehende Aliasing-Zuordnungen zwischen Graph-Aktualisierungen wiederverwenden.

Eine Neubewertung ist insbesondere erforderlich bei:

- Veränderten Ressourcenlebenszeiten
- Geänderten Render-Pass-Abhängigkeiten
- Neuen oder entfernten Ressourcen
- Veränderten Speicheranforderungen
- Geänderter Ausführungsparallelität

Bestehende Zuordnungen DÜRFEN nur erhalten bleiben, wenn ihre Sicherheitsbedingungen weiterhin erfüllt sind.

## 10. Einschränkungen

Folgende Ressourcen SOLLEN grundsätzlich nicht automatisch für Transient Aliasing verwendet werden:

- Persistent Resources
- Imported Resources
- Exportierte Ressourcen mit externer Nutzung
- Ressourcen mit unbekannter Lebensdauer
- Ressourcen mit nicht abgeschlossenen GPU-Zugriffen

Ausnahmen DÜRFEN nur bei ausdrücklich definierten Eigentums-, Lebensdauer- und Synchronisationsbedingungen erfolgen.

## 11. Adaptive Aliasing

NovaOS DARF den Umfang der Speicherüberlagerung dynamisch anpassen.

Entscheidungskriterien sind:

- Verfügbarer GPU-Speicher
- Anzahl temporärer Ressourcen
- Speicherersparnis
- Zusätzlicher Synchronisationsaufwand
- Auswirkungen auf parallele Ausführung
- Kosten der Ressourcenverwaltung

Aliasing SOLL nicht erzwungen werden, wenn separate Allokationen insgesamt effizienter sind.

## 12. Fehlerbehandlung

NovaOS MUSS ungültige Aliasing-Zuordnungen erkennen.

Dazu gehören:

- Überlappende Ressourcenlebenszeiten
- Inkompatible Speicheranforderungen
- Fehlende Synchronisation
- Ungültige Ressourcenreferenzen
- Nicht unterstützte Backend-Funktionen

Bei fehlgeschlagener Aliasing-Optimierung MUSS eine separate Ressourcenallokation möglich sein.

Eine ungültige Speicherüberlagerung DARF nicht ausgeführt werden.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS Transient Resource Aliasing unterstützen können.
2. Logische Ressourcenidentitäten MÜSSEN unabhängig von physischen Speicherbereichen bleiben.
3. Ressourcenlebenszeiten MÜSSEN vor einer Aliasing-Zuordnung analysiert werden.
4. Parallele und asynchrone GPU-Ausführungen MÜSSEN berücksichtigt werden.
5. Speicherüberlagerungen DÜRFEN nur bei nachweislich sicheren Nutzungsintervallen erfolgen.
6. Hardwareabhängige Speicheranforderungen MÜSSEN eingehalten werden.
7. Erforderliche Synchronisationsoperationen MÜSSEN eingeplant werden.
8. Neue Ressourceninhalte MÜSSEN vor lesender Verwendung gültig initialisiert sein.
9. Nicht initialisierte Speicherinhalte DÜRFEN nicht offengelegt werden.
10. Bestehende Aliasing-Zuordnungen SOLLEN inkrementell wiederverwendet werden.
11. Änderungen an Ressourcenabhängigkeiten MÜSSEN eine erneute Gültigkeitsprüfung auslösen.
12. Speicherersparnis und Synchronisationsaufwand SOLLEN gemeinsam bewertet werden.
13. Die physische Speicherverwaltung MUSS über den Nova GPU Resource Manager erfolgen.
14. Nicht unterstützte Aliasing-Verfahren MÜSSEN deaktivierbar sein.
15. Separate Ressourcenallokationen MÜSSEN als Fallback verfügbar bleiben.

## 14. Ergebnis

Nova Transient Resource Aliasing reduziert den GPU-Speicherbedarf durch die kontrollierte gemeinsame Nutzung physischer Speicherbereiche.

Durch präzise Lebenszeitanalyse, Abhängigkeitsprüfung und Synchronisation können temporäre Rendering-Ressourcen effizient wiederverwendet werden.

Damit unterstützt NovaOS eine speichersparende Grafikverarbeitung, ohne die Sicherheit, Korrektheit oder Parallelisierbarkeit des Render Graph zu beeinträchtigen.