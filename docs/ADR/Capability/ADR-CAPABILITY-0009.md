# ADR-CAPABILITY-0009: Fähigkeiten laufen standardmäßig isoliert

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine
- **Abhängigkeiten:** ADR-CAPABILITY-0001, ADR-CAPABILITY-0003, ADR-CAPABILITY-0005, ADR-CAPABILITY-0006, ADR-CAPABILITY-0008
- **Folgedokumente:** NPSPEC-CAPABILITY-SANDBOX-0001, NPSPEC-CAPABILITY-PERMISSIONS-0001, NPSPEC-CAPABILITY-RUNTIME-0001

---

# 1. Kontext

NovaOS basiert auf einem System aus eigenständigen Fähigkeiten (Capabilities).

Da Fähigkeiten von unterschiedlichen Entwicklern stammen können und unabhängig voneinander installiert, aktualisiert oder ersetzt werden, darf keine Fähigkeit uneingeschränkten Zugriff auf das Betriebssystem erhalten.

Stattdessen werden Fähigkeiten standardmäßig in einer isolierten Ausführungsumgebung gestartet.

Diese Isolation schützt:

- das Betriebssystem,
- andere Fähigkeiten,
- Benutzerdaten,
- laufende Prozesse,
- Hardware-Ressourcen.

Die Isolation ist ein grundlegendes Architekturprinzip der Nova Platform und gilt unabhängig von der verwendeten Programmiersprache oder Implementierung.

---

# 2. Problemstellung

Ohne Isolation entstehen erhebliche Risiken.

## 2.1 Sicherheitsrisiken

Eine fehlerhafte oder bösartige Fähigkeit könnte:

- Dateien verändern,
- Speicher manipulieren,
- Prozesse beeinflussen,
- Netzwerkzugriffe missbrauchen,
- Daten auslesen.

---

## 2.2 Instabilität

Ein Absturz innerhalb einer Fähigkeit könnte andere Komponenten oder sogar das gesamte System beeinträchtigen.

---

## 2.3 Unkontrollierte Ressourcenverwendung

Eine Fähigkeit könnte unbegrenzt:

- CPU-Zeit,
- Arbeitsspeicher,
- GPU-Ressourcen,
- Netzwerkbandbreite,
- Massenspeicher

beanspruchen.

---

## 2.4 Fehlende Nachvollziehbarkeit

Ohne klar definierte Grenzen ist schwer nachvollziehbar:

- welche Fähigkeit auf welche Ressourcen zugreift,
- welche Daten verarbeitet werden,
- welche Berechtigungen tatsächlich genutzt werden.

---

## 2.5 Geringe Vertrauenswürdigkeit

Eine Plattform, auf der jede Fähigkeit uneingeschränkten Zugriff besitzt, kann weder sicher verteilt noch automatisiert erweitert werden.

---

# 3. Entscheidung

Jede Fähigkeit wird standardmäßig innerhalb einer isolierten Laufzeitumgebung ausgeführt.

Die Fähigkeit besitzt ausschließlich Zugriff auf:

- ihre Ein- und Ausgangsports,
- explizit freigegebene Ressourcen,
- deklarierte Berechtigungen,
- definierte Systemdienste.

Ein direkter Zugriff auf interne Betriebssystemkomponenten ist grundsätzlich nicht zulässig.

---

# 4. Definition der Isolation

Isolation bedeutet die kontrollierte Trennung einer Fähigkeit von:

- anderen Fähigkeiten,
- internen Betriebssystemdiensten,
- Hardware,
- Benutzerdaten,
- Speicherbereichen,
- Prozessen.

Die Kommunikation erfolgt ausschließlich über definierte Schnittstellen.

---

# 5. Grundprinzipien

## 5.1 Isolation ist der Standard

Jede Capability wird isoliert gestartet.

Eine nicht isolierte Ausführung muss ausdrücklich vorgesehen und autorisiert werden.

---

## 5.2 Keine gemeinsamen Speicherbereiche

Zwei Fähigkeiten dürfen standardmäßig keinen gemeinsamen Speicher verwenden.

Datenaustausch erfolgt ausschließlich über:

- Ports,
- Streams,
- Shared Buffers mit expliziter Freigabe,
- definierte Runtime-Dienste.

---

## 5.3 Kein direkter Prozesszugriff

Eine Fähigkeit darf nicht:

- Prozesse starten,
- Prozesse beenden,
- Speicher fremder Prozesse lesen,
- Threads manipulieren,
- Handles übernehmen,

sofern hierfür keine explizite Berechtigung existiert.

---

## 5.4 Ressourcenzugriffe erfolgen ausschließlich über Berechtigungen

Der Zugriff auf Ressourcen erfolgt nur über deklarierte Berechtigungen.

Beispiele:

- Dateisystem
- Netzwerk
- Kamera
- Mikrofon
- Standort
- Zwischenablage
- Drucker
- USB-Geräte
- Bluetooth
- GPU

Nicht deklarierte Zugriffe werden von der Runtime verweigert.

---

## 5.5 Kommunikation erfolgt ausschließlich über Capability Contracts

Fähigkeiten kommunizieren niemals direkt miteinander.

Die Runtime vermittelt sämtliche Kommunikation.

Beispiel:

```text
Capability A
      │
      ▼
Nova Runtime
      │
      ▼
Capability B
```

Direkte Aufrufe zwischen Fähigkeiten sind nicht zulässig.

---

## 5.6 Fehler bleiben lokal

Ein Fehler innerhalb einer Fähigkeit darf ausschließlich diese Fähigkeit betreffen.

Die Runtime stellt sicher, dass:

- Speicher freigegeben wird,
- Ressourcen bereinigt werden,
- andere Fähigkeiten unbeeinflusst bleiben.

---

## 5.7 Isolation ist unabhängig von der Implementierung

Ob eine Fähigkeit implementiert ist in:

- NovaLang
- Rust
- C
- C++
- Zig
- Python
- Java
- C#

hat keinen Einfluss auf das Isolationsmodell.

Alle Implementierungen unterliegen denselben Regeln.

---

## 5.8 Isolation unterstützt verschiedene Sicherheitsstufen

Die Runtime kann unterschiedliche Isolationsstufen bereitstellen.

Beispielsweise:

```text
Standard

Erweitert

Echtzeit

Vertrauenswürdig

Systemintern
```

Die Sicherheitsstufe wird deklarativ beschrieben.

---

## 5.9 Isolationsgrenzen sind transparent

Der Entwickler muss nicht wissen, ob die Isolation durch:

- Prozesse,
- Container,
- Sandboxes,
- virtuelle Maschinen,
- Hardware-Schutzmechanismen

realisiert wird.

Die konkrete Umsetzung ist Aufgabe der Runtime.

---

## 5.10 Optimierungen dürfen Isolation nicht verletzen

Auch wenn Fähigkeiten gemäß ADR-CAPABILITY-0004 zu Modulen zusammengefasst werden, dürfen Sicherheitsgrenzen nicht aufgehoben werden.

Interne Optimierungen sind nur zulässig, wenn:

- Berechtigungen unverändert bleiben,
- Capability Contracts eingehalten werden,
- Isolationsregeln nicht verletzt werden.

---

# 6. Ressourcenmodell

Jede Fähigkeit erhält ausschließlich Zugriff auf deklarierte Ressourcen.

Beispiel:

```yaml
permissions:

filesystem:
  - "/documents"

network:
  outbound: true

camera: false

microphone: false

bluetooth: false
```

Alle anderen Ressourcen bleiben gesperrt.

---

# 7. Fehlerbehandlung

Tritt innerhalb einer Fähigkeit ein schwerer Fehler auf, kann die Runtime:

- die Fähigkeit neu starten,
- die Ausführung abbrechen,
- eine alternative Implementierung auswählen,
- die Pipeline anhalten,
- Diagnoseinformationen erfassen.

Andere Fähigkeiten bleiben weiterhin funktionsfähig.

---

# 8. Vorteile

Dieses Architekturprinzip ermöglicht:

- hohe Systemsicherheit,
- stabile Ausführung,
- geringe Fehlerausbreitung,
- kontrollierte Ressourcenverwaltung,
- reproduzierbares Verhalten,
- bessere Wartbarkeit,
- sichere Installation fremder Fähigkeiten,
- einfachere Zertifizierung,
- höhere Zuverlässigkeit.

---

# 9. Nachteile

Dieses Modell verursacht zusätzlichen Aufwand durch:

- Sandbox-Verwaltung,
- Berechtigungsprüfung,
- Ressourcenverwaltung,
- Laufzeitüberwachung,
- IPC zwischen isolierten Fähigkeiten.

Dieser Mehraufwand wird bewusst akzeptiert, da Sicherheit und Stabilität Vorrang besitzen.

---

# 10. Auswirkungen auf andere Komponenten

Dieses ADR beeinflusst insbesondere:

- Capability Runtime
- Sandbox Manager
- Permission Manager
- Security Manager
- Scheduler
- Pipeline Executor
- Memory Manager
- Process Manager
- Resource Manager
- Capability Registry
- Nova Assistant
- Remote Execution

Alle diese Komponenten müssen das Isolationsmodell berücksichtigen.

---

# 11. Implementierungshinweise

Die konkrete Isolationsstrategie ist nicht Bestandteil dieses ADR.

Eine Runtime kann beispielsweise verwenden:

- Betriebssystemprozesse,
- leichtgewichtige Sandboxes,
- Container,
- Capability-Domänen,
- hardwaregestützte Isolation.

Die Auswahl darf sich zwischen Plattformen unterscheiden, solange die definierten Sicherheitsgarantien eingehalten werden.

---

# 12. Konformitätskriterien

Eine Implementierung gilt als konform, wenn:

- Fähigkeiten standardmäßig isoliert ausgeführt werden,
- ausschließlich deklarierte Berechtigungen genutzt werden,
- Kommunikation nur über definierte Schnittstellen erfolgt,
- Fehler nicht auf andere Fähigkeiten übergreifen,
- Ressourcen kontrolliert zugewiesen werden,
- Optimierungen die Isolation nicht verletzen,
- die Runtime Verstöße zuverlässig verhindert.

---

# 13. Zusammenfassung

NovaOS führt jede Fähigkeit standardmäßig innerhalb einer isolierten Laufzeitumgebung aus.

Die Kommunikation erfolgt ausschließlich über deklarierte Capability Contracts und typisierte Ports.

Ressourcenzugriffe werden durch ein deklaratives Berechtigungsmodell kontrolliert und von der Runtime überwacht.

Dieses Architekturprinzip bildet eine wesentliche Grundlage für Sicherheit, Stabilität und langfristige Wartbarkeit der Nova-Plattform und ermöglicht gleichzeitig den sicheren Einsatz von Fähigkeiten unterschiedlicher Herkunft.