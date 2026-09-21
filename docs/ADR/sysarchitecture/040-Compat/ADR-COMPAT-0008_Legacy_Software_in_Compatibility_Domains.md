# ADR-COMPAT-0008 – Legacy Software in Compatibility Domains

## Status

Angenommen

## Kategorie

Compatibility / Legacy / Isolation / Security

## Kontext

Legacy-Software kann veraltete Annahmen über Betriebssystem, Sicherheit, Dateisystem, Berechtigungen oder Hardware besitzen.

Beispiele:

```text
Globale Schreibrechte
Unsichere Bibliotheken
Veraltete APIs
Direkter Gerätezugriff
Historische Dateisystemstrukturen
Nicht mehr zeitgemäße Sicherheitsmodelle
```

Diese Anforderungen dürfen nicht dazu führen, dass NovaOS seine nativen Sicherheits- und Architekturprinzipien abschwächt.

## Entscheidung

Legacy-Software wird in **Compatibility Domains** ausgeführt.

```text
Legacy Application
        ↓
Compatibility Domain
        ↓
Compatibility Personality
        ↓
Controlled Nova Capabilities
        ↓
NovaOS
```

Eine Compatibility Domain kapselt die für die Anwendung benötigte Legacy-Umgebung und begrenzt deren Einfluss auf das restliche System.

## Domain

Eine Compatibility Domain definiert eine kontrollierte Ausführungsumgebung mit eigenen:

```text
Namespace Views
Filesystem Views
Environment
Compatibility Services
Resource Limits
Capabilities
Security Policies
```

Die Anwendung erhält damit die benötigte Umgebung, ohne dass NovaOS diese global bereitstellen muss.

## Isolation

Legacy-Anwendungen werden grundsätzlich als weniger vertrauenswürdig behandelt als native Systemkomponenten.

Die Domain begrenzt insbesondere:

```text
Filesystem Access
Device Access
IPC
Network Access
System Configuration
Other Processes
```

Zugriffe außerhalb der Domain benötigen explizite NovaOS-Autorisierung.

## Virtuelle Legacy-Umgebung

Historische Systemstrukturen können innerhalb einer Domain virtualisiert werden.

Beispiele:

```text
Legacy Paths
Registry Views
Configuration Files
Environment Variables
Virtual Devices
Expected Directory Layouts
```

Diese virtuellen Strukturen müssen nicht dem tatsächlichen NovaOS-System entsprechen.

## Capability Mapping

Legacy-Berechtigungen werden auf begrenzte NovaOS-Capabilities abgebildet.

```text
Legacy Permission
       ↓
Compatibility Mapping
       ↓
Restricted Capability
```

Eine Legacy-Anwendung darf dadurch keine stärkere Autorität erhalten als ihr NovaOS-Sicherheitskontext erlaubt.

## Gemeinsame Ressourcen

Der Zugriff auf native NovaOS-Ressourcen erfolgt über kontrollierte Übergänge.

```text
Compatibility Domain
        ↓
Validated Boundary
        ↓
Native Resource
```

Dadurch können beispielsweise Dokumente gezielt zwischen Legacy- und nativer Umgebung verfügbar gemacht werden.

## Fehlerisolation

Fehler innerhalb einer Compatibility Domain sollen möglichst auf diese Domain begrenzt bleiben.

```text
Legacy Failure
      ↓
Domain Isolation
      ↓
NovaOS continues
```

Ein Absturz oder Fehlverhalten einer Legacy-Anwendung darf nicht automatisch die Integrität des Gesamtsystems gefährden.

## Normative Anforderungen

1. Legacy-Software SOLL innerhalb definierter Compatibility Domains ausgeführt werden.
2. Compatibility Domains MÜSSEN ihre Ressourcen- und Sicherheitsgrenzen explizit definieren.
3. Legacy-Semantik DARF native NovaOS-Regeln nicht abschwächen.
4. Legacy-Berechtigungen MÜSSEN auf kontrollierte NovaOS-Capabilities abgebildet werden.
5. Historische Systemstrukturen SOLLEN innerhalb der Domain virtualisiert werden können.
6. Zugriffe auf native Ressourcen MÜSSEN über kontrollierte Grenzen erfolgen.
7. Legacy-Software DARF keinen impliziten globalen Systemzugriff erhalten.
8. Fehler innerhalb einer Domain SOLLEN gegenüber dem restlichen System isoliert werden.
9. Compatibility Domains MÜSSEN unabhängig versionierbar und entfernbar sein.

## Konsequenzen

### Positiv

- alte Software kann unterstützt werden, ohne NovaOS strukturell zu belasten,
- unsichere Legacy-Annahmen bleiben lokal begrenzt,
- historische Umgebungen können gezielt emuliert werden,
- native Sicherheitsmechanismen bleiben maßgeblich.

### Negativ

- Domains benötigen zusätzliche Isolation und Übersetzung,
- bestimmte Legacy-Anwendungen benötigen umfangreiche virtuelle Umgebungen,
- Integration mit nativen Ressourcen muss explizit erfolgen.

## Abhängigkeiten

- `ADR-COMPAT-0001_Native_Nova_Architektur_nicht_durch_POSIX_Linux_oder_Win32_bestimmen`
- `ADR-COMPAT-0003_Compatibility_Personalities`
- `ADR-COMPAT-0007_Execution_Compatibility_Ladder`
- `ADR-CAPABILITY-0001`
- `ADR-CAPABILITY-0012_Sandboxing`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

## Ergebnis

NovaOS folgt für Legacy-Software dem Prinzip:

```text
Compatibility
without
Architecture Contamination
```

Legacy-Software erhält die Umgebung, die sie benötigt, jedoch innerhalb einer kontrollierten Compatibility Domain, deren Grenzen durch NovaOS bestimmt werden.