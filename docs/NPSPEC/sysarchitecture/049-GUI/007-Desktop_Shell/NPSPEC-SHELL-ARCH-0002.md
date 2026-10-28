# NPSPEC-SHELL-ARCH-0002 – Nova Shell Lifecycle

**Status:** Angenommen  
**Bereich:** Shell / Architecture / Lifecycle  
**System:** NovaOS

## 1. Zweck

Nova Shell Lifecycle definiert den Lebenszyklus der Desktop Shell von NovaOS.

Ziel ist ein schneller, kontrollierter und wiederherstellbarer Betrieb der grafischen Benutzerumgebung.

Die Desktop Shell MUSS unabhängig vom Kernel und möglichst unabhängig von laufenden Anwendungen gestartet, angehalten und neu gestartet werden können.

## 2. Lebenszyklus

Die Desktop Shell besitzt folgende Zustände:

| Zustand | Bedeutung |
|---|---|
| Created | Shell-Instanz wurde erstellt |
| Initializing | Erforderliche Komponenten werden initialisiert |
| Ready | Grundlegende Desktopfunktionen sind verfügbar |
| Active | Shell ist vollständig betriebsbereit |
| Suspended | Nicht notwendige Shell-Aktivitäten sind ausgesetzt |
| Recovering | Fehlerbehandlung und Wiederherstellung laufen |
| Stopped | Shell wurde kontrolliert beendet |

Zustandsübergänge MÜSSEN eindeutig definiert und nachvollziehbar sein.

## 3. Initialisierung

Die Shell wird gestartet, sobald die erforderlichen Systemkomponenten verfügbar sind.

Zu den Mindestvoraussetzungen gehören:

- Funktionsfähige Grafikausgabe.
- Display Server und Compositor.
- Window Manager.
- Nova UI Runtime.
- Grundlegende Eingabeverarbeitung.
- Zugriff auf die erforderlichen Shell-Ressourcen.

Dabei gilt:

- Nur unmittelbar benötigte Komponenten MÜSSEN vor der ersten Desktopdarstellung initialisiert werden.
- Optionale Komponenten SOLLEN anschließend im Hintergrund geladen werden.
- Die Initialisierung MUSS asynchron erfolgen können.
- Fehler optionaler Komponenten DÜRFEN den Desktopstart nicht verhindern.

## 4. Ready-Zustand

Der Zustand `Ready` wird erreicht, sobald der Desktop grundsätzlich bedienbar ist.

Mindestens verfügbar sein MÜSSEN:

- Desktopdarstellung.
- Grundlegende Taskleiste.
- Startmenü oder gleichwertiger Anwendungsstarter.
- Fensterwechsel.
- Eingabeverarbeitung.
- Zugriff auf wesentliche Systemfunktionen.

Der Zustand `Ready` DARF bereits erreicht werden, während optionale Shell-Dienste noch initialisiert werden.

## 5. Active-Zustand

Im Zustand `Active` stehen sämtliche erfolgreich initialisierten Shell-Komponenten zur Verfügung.

Dabei gilt:

- Die Shell verarbeitet Benutzerinteraktionen.
- Komponenten arbeiten ereignisgesteuert.
- Hintergrunddienste werden bedarfsgesteuert ausgeführt.
- Nicht benötigte Komponenten DÜRFEN inaktiv bleiben.
- Die Shell MUSS auf Änderungen der Systemumgebung reagieren können.

Der Zustand `Active` bedeutet nicht, dass sämtliche optionalen Funktionen dauerhaft geladen sein müssen.

## 6. Suspend und Resume

Die Shell MUSS nicht notwendige Aktivitäten vorübergehend aussetzen können.

Mögliche Auslöser:

- Energiesparmodus.
- Benutzerwechsel.
- Sperrbildschirm.
- Sitzungsunterbrechung.
- Systembedingte Ressourcenbegrenzung.

Dabei gilt:

- Der Shell-Zustand SOLL erhalten bleiben.
- Nicht benötigte Animationen und Aktualisierungen werden ausgesetzt.
- Ressourcen DÜRFEN kontrolliert freigegeben werden.
- Nach `Resume` MUSS die Shell ihren gültigen Zustand wiederherstellen.
- Sicherheitsrelevante Zustandsänderungen MÜSSEN berücksichtigt werden.

## 7. Fehlerbehandlung und Recovery

Bei einem schwerwiegenden Shell-Fehler wechselt das System in den Zustand `Recovering`.

Dabei gilt:

- Der Fehler MUSS protokolliert werden.
- Fehlerhafte Komponenten SOLLEN zunächst isoliert wiederhergestellt werden.
- Ein vollständiger Shell-Neustart erfolgt nur bei Bedarf.
- Laufende Anwendungen SOLLEN erhalten bleiben.
- Der letzte gültige Shell-Zustand SOLL wiederhergestellt werden.
- Wiederholte Fehler MÜSSEN erkannt werden.

Ist eine reguläre Wiederherstellung nicht möglich, MUSS eine minimale Recovery-Oberfläche verfügbar gemacht werden können.

## 8. Shell-Neustart

NovaOS MUSS einen kontrollierten Neustart der Desktop Shell unterstützen.

Dabei gilt:

- Der Kernel bleibt unverändert aktiv.
- Display Server, Compositor und Window Manager SOLLEN weiterlaufen.
- Laufende Anwendungen DÜRFEN nicht allein wegen des Shell-Neustarts beendet werden.
- Shell-spezifische Ressourcen werden kontrolliert freigegeben.
- Die neue Shell-Instanz übernimmt den gültigen Benutzerzustand.
- Verlorene Shell-Verbindungen MÜSSEN erneut hergestellt werden können.

## 9. Zustandsverwaltung

Der Nova Shell State Manager verwaltet wiederherstellbare Shell-Informationen.

Dazu gehören:

- Desktop-Layout.
- Taskleistenkonfiguration.
- Angeheftete Elemente.
- Startmenü-Einstellungen.
- Aktive Shell-Bereiche.
- Benachrichtigungseinstellungen.
- Benutzerdefinierte Anpassungen.

Dabei gilt:

- Zustandsänderungen SOLLEN inkrementell gespeichert werden.
- Gespeicherte Zustände MÜSSEN validiert werden.
- Beschädigte Zustandsdaten DÜRFEN den Shell-Start nicht verhindern.
- Ein sicherer Standardzustand MUSS verfügbar sein.

## 10. Beendigung

Beim kontrollierten Beenden der Shell gilt:

- Neue Shell-Aktionen werden nicht mehr angenommen.
- Laufende Shell-Operationen werden abgeschlossen oder abgebrochen.
- Der wiederherstellbare Zustand wird gespeichert.
- Ereignisabonnements und Ressourcen werden freigegeben.
- Shell-Komponenten werden in definierter Reihenfolge beendet.
- Anwendungen bleiben aktiv, sofern nicht ausdrücklich die gesamte Sitzung beendet wird.

Ein Shell-Neustart MUSS von einer Benutzerabmeldung oder Systemabschaltung unterschieden werden.

## 11. Sicherheit

Der Shell Lifecycle unterliegt Nova.Trust.

Dabei gilt:

- Shell-Instanzen MÜSSEN autorisiert gestartet werden.
- Wiederhergestellte Zustände DÜRFEN keine zusätzlichen Berechtigungen verleihen.
- Sitzungs- und Benutzergrenzen MÜSSEN eingehalten werden.
- Fehlerhafte Erweiterungen DÜRFEN die Recovery nicht blockieren.
- Sicherheitskritische Systemfunktionen MÜSSEN unabhängig vom Zustand der Desktop Shell verfügbar bleiben können.

## 12. Normative Anforderungen

- **SHELL-001:** Die Desktop Shell MUSS einen definierten Lebenszyklus besitzen.
- **SHELL-002:** Zustandsübergänge MÜSSEN kontrolliert erfolgen.
- **SHELL-003:** Die Shell MUSS unabhängig vom Kernel neu gestartet werden können.
- **SHELL-004:** Die erste Desktopdarstellung MUSS nur unmittelbar notwendige Komponenten voraussetzen.
- **SHELL-005:** Optionale Komponenten SOLLEN asynchron initialisiert werden.
- **SHELL-006:** Der Zustand `Ready` MUSS einen bedienbaren Desktop gewährleisten.
- **SHELL-007:** Suspend und Resume MÜSSEN unterstützt werden.
- **SHELL-008:** Fehlerhafte Shell-Komponenten SOLLEN isoliert wiederhergestellt werden.
- **SHELL-009:** Ein Shell-Neustart DARF laufende Anwendungen nicht unnötig beenden.
- **SHELL-010:** Wiederherstellbare Shell-Zustände MÜSSEN gespeichert werden können.
- **SHELL-011:** Beschädigte Zustandsdaten DÜRFEN den Shell-Start nicht dauerhaft verhindern.
- **SHELL-012:** Ein sicherer Recovery-Modus MUSS verfügbar sein.
- **SHELL-013:** Ressourcen MÜSSEN beim Beenden kontrolliert freigegeben werden.
- **SHELL-014:** Sämtliche Lebenszyklusoperationen MÜSSEN Nova.Trust berücksichtigen.
- **SHELL-015:** Nova Shell Lifecycle MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova Desktop Shell Architecture:** Übergeordnete Shell-Architektur.
- **Nova Shell Manager:** Verwaltung der Shell-Instanzen.
- **Nova Shell State Manager:** Zustandsspeicherung.
- **Nova UI Runtime:** Lebenszyklus der UI-Komponenten.
- **Nova Window Manager:** Verwaltung laufender Fenster.
- **Nova Compositor:** Darstellung der Oberflächen.
- **Nova Display Server:** Grafikausgabe.
- **Nova Input System:** Eingabeverarbeitung.
- **Nova Recovery System:** Fehlerbehandlung und Wiederherstellung.
- **Nova.Trust:** Sicherheit und Isolation.

## 14. Ergebnis

NovaOS erhält einen klar definierten und fehlertoleranten Lebenszyklus für seine Desktop Shell.

Die Benutzeroberfläche kann frühzeitig angezeigt, schrittweise initialisiert und bei Fehlern unabhängig vom Betriebssystemkern wiederhergestellt werden.

Laufende Anwendungen und Benutzereinstellungen bleiben dabei möglichst erhalten.

**Grundsatz:** Die Desktop Shell darf jederzeit neu starten können, ohne dass der Benutzer deshalb seine Arbeit verliert.