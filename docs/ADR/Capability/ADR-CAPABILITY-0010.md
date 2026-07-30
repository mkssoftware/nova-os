# ADR-CAPABILITY-0010: Klassische Anwendungen bleiben als Kompatibilitätsschicht erhalten

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine
- **Abhängigkeiten:** ADR-CAPABILITY-0001, ADR-CAPABILITY-0003, ADR-CAPABILITY-0005, ADR-CAPABILITY-0007, ADR-CAPABILITY-0009
- **Folgedokumente:** NPSPEC-COMPATIBILITY-0001, NPSPEC-SOLUTION-0001, NPSPEC-APPLICATIONHOST-0001, NPSPEC-CAPABILITY-BRIDGE-0001

---

# 1. Kontext

NovaOS definiert Fähigkeiten (Capabilities) als primäres Softwaremodell.

Gleichzeitig existiert eine sehr große Anzahl bestehender Anwendungen für:

- Windows
- Linux
- Android
- Web
- ältere Nova-Versionen

Eine vollständige Neuentwicklung aller Anwendungen ist weder wirtschaftlich noch realistisch.

Ebenso erwarten Anwender, dass vorhandene Software weiterhin genutzt werden kann.

NovaOS muss daher moderne Fähigkeiten und klassische Anwendungen gleichzeitig unterstützen, ohne das Capability-Modell zu verwässern.

---

# 2. Problemstellung

Ein vollständiger Verzicht auf klassische Anwendungen hätte erhebliche Nachteile.

## 2.1 Fehlende Softwarebasis

Zum Marktstart stünde nur eine geringe Anzahl nativer Fähigkeiten zur Verfügung.

---

## 2.2 Geringe Akzeptanz

Anwender erwarten, dass ihre vorhandene Software weiterhin funktioniert.

---

## 2.3 Hoher Migrationsaufwand

Unternehmen müssten bestehende Anwendungen vollständig neu entwickeln.

---

## 2.4 Verlust bestehender Investitionen

Viele Anwendungen wurden über Jahrzehnte entwickelt.

Diese Investitionen dürfen nicht verloren gehen.

---

## 2.5 Langsame Plattformentwicklung

Ohne bestehendes Softwareangebot würde die Verbreitung der Plattform erheblich erschwert.

---

# 3. Entscheidung

NovaOS unterstützt weiterhin klassische Anwendungen.

Diese bilden jedoch **nicht mehr das primäre Softwaremodell**, sondern ausschließlich eine **Kompatibilitätsschicht**.

Neue Software soll grundsätzlich als Fähigkeiten entwickelt werden.

Bestehende Anwendungen können unverändert oder mit einer Bridge weiter betrieben werden.

---

# 4. Definition einer klassischen Anwendung

Eine klassische Anwendung ist eine Softwareeinheit mit:

- eigenem Prozess,
- eigener Benutzeroberfläche,
- eigenem Lebenszyklus,
- interner Programmlogik,
- eigener Zustandsverwaltung.

Sie entspricht dem traditionellen Anwendungsmodell heutiger Betriebssysteme.

---

# 5. Grundprinzipien

## 5.1 Fähigkeiten bleiben das primäre Modell

Alle neuen Plattformfunktionen werden als Fähigkeiten entwickelt.

Anwendungen sind kein Ersatz für Capabilities.

---

## 5.2 Anwendungen sind Kompatibilitätskomponenten

Eine klassische Anwendung wird von NovaOS als spezielle Laufzeiteinheit behandelt.

Sie erhält keinen Sonderstatus gegenüber Fähigkeiten.

---

## 5.3 Anwendungen können Fähigkeiten verwenden

Eine klassische Anwendung darf Capabilities aufrufen.

Beispielsweise:

```text
Windows-Anwendung
          │
          ▼
Capability Runtime
          │
          ▼
OCR

Übersetzung

PDF

KI-Assistent
```

Dadurch profitieren bestehende Anwendungen unmittelbar von neuen Plattformfunktionen.

---

## 5.4 Fähigkeiten dürfen Anwendungen nutzen

Ist keine native Capability vorhanden, darf eine spezielle Bridge eine klassische Anwendung verwenden.

Beispiel:

```text
Capability

↓

Application Bridge

↓

Klassische Anwendung

↓

Ergebnis
```

Die eigentliche Pipeline bleibt weiterhin auf Ebene der Fähigkeiten definiert.

---

## 5.5 Anwendungen bleiben isoliert

Klassische Anwendungen unterliegen denselben Sicherheitsprinzipien wie Fähigkeiten.

Insbesondere gelten:

- Berechtigungsmodell,
- Sandbox,
- Ressourcenkontrolle,
- Sicherheitsrichtlinien.

Eine Anwendung erhält keine automatischen Systemprivilegien.

---

## 5.6 Anwendungen besitzen Capability-Adapter

Bestehende Anwendungen können durch Adapter in die Capability-Plattform integriert werden.

Ein Adapter beschreibt beispielsweise:

- verfügbare Funktionen,
- Eingänge,
- Ausgänge,
- Datentypen,
- Einschränkungen.

Dadurch können Anwendungen schrittweise in Pipelines eingebunden werden.

---

## 5.7 Migration erfolgt schrittweise

NovaOS erzwingt keine vollständige Neuentwicklung.

Eine Anwendung kann nach und nach in einzelne Fähigkeiten zerlegt werden.

Beispiel:

```text
Version 1

Komplette Anwendung
```

↓

```text
Version 2

Anwendung

+

OCR Capability
```

↓

```text
Version 3

Mehrere Capabilities

+

kleine Oberfläche
```

↓

```text
Version 4

Solution

+

Capabilities
```

---

## 5.8 Oberflächen bleiben erhalten

Eine bestehende grafische Oberfläche kann weiterhin genutzt werden.

Lediglich die interne Funktionalität wird schrittweise durch Fähigkeiten ersetzt.

---

## 5.9 Neue APIs basieren auf Capabilities

Neue Plattform-APIs werden ausschließlich für Fähigkeiten entwickelt.

Klassische Anwendungen greifen über Adapter oder Bridges auf diese APIs zu.

---

## 5.10 Langfristige Ablösung ist möglich

Mit zunehmender Verbreitung nativer Fähigkeiten sinkt die Bedeutung klassischer Anwendungen.

Die Architektur bleibt jedoch dauerhaft kompatibel.

Eine Entfernung der Kompatibilitätsschicht ist nicht Bestandteil dieses ADR.

---

# 6. Migrationsmodell

Typischer Entwicklungsweg:

```text
Klassische Anwendung

↓

Capability Adapter

↓

Hybride Anwendung

↓

Mehrere Capabilities

↓

Solution

↓

Native Capability-Plattform
```

Dieser Übergang kann über mehrere Jahre erfolgen.

---

# 7. Beispiele

## Bildbearbeitung

Heute:

```text
Bildbearbeitungsprogramm
```

Später:

```text
UI

↓

Capability

Bild laden

↓

Capability

Filter

↓

Capability

Skalieren

↓

Capability

Export
```

---

## Office

Heute:

```text
Office-Anwendung
```

Später:

```text
Solution

↓

Text

↓

PDF

↓

Übersetzung

↓

Rechtschreibprüfung

↓

KI-Assistent
```

---

## CAD

Heute:

```text
CAD-Anwendung
```

Später:

```text
CAD UI

↓

Geometry Capability

↓

Rendering Capability

↓

Simulation Capability

↓

Export Capability
```

---

# 8. Vorteile

Dieses Architekturprinzip ermöglicht:

- sofortige Nutzbarkeit bestehender Software,
- hohe Plattformakzeptanz,
- schrittweise Migration,
- Wiederverwendung bestehender Investitionen,
- Integration moderner Fähigkeiten,
- geringeres Risiko,
- langfristige Modernisierung,
- stabile Übergangsphase.

---

# 9. Nachteile

Dieses Modell erfordert:

- Kompatibilitätsschichten,
- Bridge-Komponenten,
- Adapter,
- zusätzliche Laufzeitkomponenten,
- höheren Wartungsaufwand.

Diese Nachteile werden bewusst akzeptiert, da sie den Übergang von einem anwendungsorientierten zu einem fähigkeitsorientierten Betriebssystem erheblich erleichtern.

---

# 10. Auswirkungen auf andere Komponenten

Dieses ADR beeinflusst insbesondere:

- Capability Runtime
- Application Host
- Compatibility Layer
- Application Bridge
- Solution Framework
- Pipeline Planner
- Nova Shell
- Permission Manager
- Sandbox Manager
- Package Manager
- Capability Registry

Alle diese Komponenten müssen klassische Anwendungen als sekundäres Ausführungsmodell unterstützen.

---

# 11. Implementierungshinweise

NovaOS sollte einen Application Host bereitstellen, der klassische Anwendungen innerhalb der Plattform ausführt.

Bridges und Adapter sollten deklarativ beschrieben werden, sodass Anwendungen möglichst transparent in Capability-Pipelines integriert werden können.

Neue SDKs und Entwicklungswerkzeuge sollten Entwickler dabei unterstützen, bestehende Anwendungen schrittweise in native Fähigkeiten zu überführen.

---

# 12. Konformitätskriterien

Eine Implementierung gilt als konform, wenn:

- Fähigkeiten das primäre Softwaremodell bleiben,
- klassische Anwendungen ausschließlich als Kompatibilitätsschicht behandelt werden,
- Anwendungen Capabilities nutzen können,
- Capabilities Anwendungen über definierte Bridges einbinden können,
- Anwendungen den Sicherheitsrichtlinien der Plattform unterliegen,
- Migration schrittweise möglich ist,
- neue Plattformfunktionen primär als Fähigkeiten bereitgestellt werden.

---

# 13. Zusammenfassung

NovaOS führt mit den Fähigkeiten ein neues primäres Softwaremodell ein, ohne die bestehende Softwarelandschaft aufzugeben.

Klassische Anwendungen bleiben als Kompatibilitätsschicht erhalten und können schrittweise in das Capability-Modell integriert werden.

Durch Capability-Adapter, Bridges und einen Application Host wird ein sanfter Übergang ermöglicht, der bestehende Investitionen schützt und gleichzeitig die langfristige Entwicklung hin zu einer vollständig fähigkeitsorientierten Plattform unterstützt.