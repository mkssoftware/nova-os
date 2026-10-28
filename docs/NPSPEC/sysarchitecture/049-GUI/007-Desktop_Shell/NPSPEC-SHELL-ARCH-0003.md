# NPSPEC-SHELL-ARCH-0003 – Nova Shell Extension Model

**Status:** Angenommen  
**Bereich:** Shell / Architecture / Extensions  
**System:** NovaOS

## 1. Zweck

Nova Shell Extension Model definiert die Erweiterbarkeit der NovaOS Desktop Shell.

Anwendungen, Solutions und Capabilities können zusätzliche Funktionen und Bedienelemente in die Desktopumgebung integrieren, ohne die Shell selbst verändern zu müssen.

Erweiterungen müssen sicher, modular, ressourcenschonend und unabhängig voneinander verwaltbar sein.

## 2. Architektur

Das Erweiterungssystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Extension Manager | Verwaltung und Lebenszyklus der Erweiterungen |
| Extension Registry | Registrierung und Erkennung |
| Extension Host | Isolierte Ausführung |
| Extension Interface | Standardisierte Kommunikation mit der Shell |
| Contribution Manager | Integration von Funktionen und UI-Elementen |

Die Shell stellt definierte Erweiterungspunkte bereit. Erweiterungen DÜRFEN nicht direkt auf interne Shell-Strukturen zugreifen.

## 3. Erweiterungspunkte

NovaOS unterstützt Erweiterungen für:

- Desktop und Desktop-Widgets.
- Taskleiste und System Tray.
- Startmenü und Navigation.
- Kontextmenüs.
- Ribbon-Oberflächen.
- Benachrichtigungen und Schnellaktionen.
- Datei- und Objektoperationen.
- Suche und Suchanbieter.
- Capability-basierte Funktionen.

Weitere Erweiterungspunkte MÜSSEN ohne grundlegende Architekturänderungen ergänzt werden können.

Die Shell bestimmt die endgültige Darstellung und Platzierung.

## 4. Registrierung

Shell-Erweiterungen MÜSSEN deklarativ registriert werden.

Eine Registrierung enthält mindestens:

- Eindeutige Erweiterungs-ID.
- Name und Version.
- Unterstützte Shell-Schnittstellenversion.
- Verwendete Erweiterungspunkte.
- Erforderliche Capabilities.
- Implementierung oder Einstiegspunkt.

Die Registrierung DARF keine Berechtigungen automatisch erteilen.

Erweiterungen MÜSSEN ohne Änderung des Shell-Quellcodes hinzugefügt und entfernt werden können.

## 5. Capability-Integration

Shell-Erweiterungen dienen als Zugriffspunkte auf vorhandene Capabilities.

Dabei gilt:

- Capabilities bleiben unabhängig von der Desktop Shell.
- Erweiterungen DÜRFEN Capabilities deklarativ referenzieren.
- Die Ausführung erfolgt über die vorgesehenen Capability-Schnittstellen.
- Berechtigungen werden vor der Ausführung geprüft.
- Fehlende Capabilities DÜRFEN die Shell nicht beeinträchtigen.

Eine Shell-Erweiterung ist keine Voraussetzung für die Existenz oder Ausführung einer Capability.

## 6. UI-Integration

Erweiterungen mit grafischer Oberfläche verwenden das native Nova UI Framework.

Dabei gilt:

- UI-Beiträge MÜSSEN in die vorhandenen Styles und Themes integrierbar sein.
- Die Shell kontrolliert Position, Größe und Sichtbarkeit.
- Erweiterungen MÜSSEN Accessibility unterstützen.
- UI-Elemente MÜSSEN bedarfsgesteuert aktualisiert werden.
- Erweiterungen DÜRFEN keine unautorisierten systemweiten Overlays erzeugen.

## 7. Lebenszyklus

Shell-Erweiterungen besitzen folgende Zustände:

| Zustand | Bedeutung |
|---|---|
| Registered | Erweiterung ist registriert |
| Loading | Erweiterung wird initialisiert |
| Active | Erweiterung ist betriebsbereit |
| Suspended | Erweiterung ist vorübergehend inaktiv |
| Disabled | Erweiterung wurde deaktiviert |
| Failed | Erweiterung ist fehlerhaft |
| Unloaded | Erweiterung wurde entladen |

Erweiterungen MÜSSEN unabhängig voneinander aktiviert, deaktiviert und wiederhergestellt werden können.

Ein Fehler DARF nicht zum Absturz der gesamten Desktop Shell führen.

## 8. Sicherheit

Alle Shell-Erweiterungen unterliegen Nova.Trust.

Dabei gilt:

- Identität, Signatur und Herkunft MÜSSEN überprüfbar sein.
- Erweiterungen erhalten ausschließlich autorisierte Capabilities.
- Nicht vertrauenswürdiger Erweiterungscode MUSS isoliert ausgeführt werden.
- Privilegierte Shell-Funktionen MÜSSEN gesondert geschützt sein.
- Fehlerhafte Erweiterungen MÜSSEN deaktivierbar sein.
- Der Benutzer behält die Kontrolle über installierte Erweiterungen.

## 9. Performance

Erweiterungen MÜSSEN die Reaktionsfähigkeit der Desktop Shell erhalten.

Dabei gilt:

- Erweiterungen SOLLEN bedarfsgesteuert geladen werden.
- Inaktive Erweiterungen SOLLEN keine unnötigen Ressourcen beanspruchen.
- Erweiterungen DÜRFEN die UI Event Loop nicht blockieren.
- Grafische Erweiterungen MÜSSEN Damage Tracking unterstützen.
- Ressourcenverbrauch und Fehler SOLLEN diagnostizierbar sein.

## 10. Benutzerverwaltung

NovaOS MUSS eine zentrale Verwaltung für Shell-Erweiterungen bereitstellen.

Der Benutzer kann:

- Installierte Erweiterungen anzeigen.
- Erweiterungen aktivieren und deaktivieren.
- Berechtigungen einsehen.
- Erweiterungen entfernen.
- Fehlerhafte Erweiterungen identifizieren.

Änderungen SOLLEN möglichst ohne vollständigen Shell-Neustart wirksam werden.

## 11. Normative Anforderungen

- **SHELL-001:** Die Desktop Shell MUSS über definierte Erweiterungspunkte verfügen.
- **SHELL-002:** Erweiterungen MÜSSEN deklarativ registrierbar sein.
- **SHELL-003:** Erweiterungsschnittstellen MÜSSEN versioniert sein.
- **SHELL-004:** Erweiterungen DÜRFEN interne Shell-Strukturen nicht direkt verändern.
- **SHELL-005:** Capabilities MÜSSEN unabhängig von der Shell bleiben.
- **SHELL-006:** Grafische Erweiterungen MÜSSEN das native Nova UI Framework verwenden.
- **SHELL-007:** Erweiterungen MÜSSEN unabhängig voneinander verwaltbar sein.
- **SHELL-008:** Fehlerhafte Erweiterungen MÜSSEN isoliert werden können.
- **SHELL-009:** Erweiterungen MÜSSEN Nova.Trust berücksichtigen.
- **SHELL-010:** Erweiterungen SOLLEN bedarfsgesteuert geladen werden.
- **SHELL-011:** Erweiterungen DÜRFEN die Reaktionsfähigkeit der Shell nicht dauerhaft beeinträchtigen.
- **SHELL-012:** Der Benutzer MUSS Erweiterungen deaktivieren können.
- **SHELL-013:** Das Erweiterungssystem MUSS vollständig offline und ohne KI funktionieren.

## 12. Ergebnis

NovaOS erhält ein modulares Erweiterungssystem für die Desktop Shell.

Anwendungen, Solutions und Capabilities können Funktionen und Bedienelemente integrieren, ohne die Shell-Architektur verändern zu müssen.

Erweiterungen bleiben sicher, unabhängig und kontrollierbar.

**Grundsatz:** Die Desktop Shell ist erweiterbar, ohne dass Erweiterungen ihre Stabilität, Sicherheit oder Kontrolle übernehmen.