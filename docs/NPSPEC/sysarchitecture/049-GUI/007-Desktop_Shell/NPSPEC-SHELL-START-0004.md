# NPSPEC-SHELL-START-0004 – Nova Pinned Applications

**Status:** Angenommen  
**Bereich:** Shell / Start / Pinned Applications  
**System:** NovaOS

## 1. Zweck

Nova Pinned Applications definiert die Verwaltung dauerhaft angehefteter Anwendungen, Solutions und Capabilities innerhalb des NovaOS-Startmenüs.

Ziel ist ein individuell anpassbarer Schnellzugriff auf häufig verwendete Funktionen, unabhängig davon, ob diese aktuell ausgeführt werden.

Die Anheftung MUSS unabhängig vom Lebenszyklus der referenzierten Anwendung oder Capability erfolgen.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Pin Manager | Verwaltung angehefteter Elemente |
| Pin Registry | Registrierung und Identifikation |
| Pin Layout Manager | Anordnung und Gruppierung |
| Pin Resolver | Auflösung referenzierter Ressourcen |
| Pin Action Controller | Verarbeitung von Benutzeraktionen |
| Pin State Manager | Speicherung und Wiederherstellung |

Die Darstellung erfolgt über das Nova UI Framework innerhalb des Startmenüs.

## 3. Anheftungsmodell

Ein angeheftetes Element besitzt mindestens:

- Eindeutige Pin-ID.
- Referenz auf das Zielobjekt.
- Elementtyp.
- Anzeigename und Symbolreferenz.
- Position und Gruppenzugehörigkeit.
- Optionale benutzerdefinierte Eigenschaften.

Unterstützte Ziele:

- Anwendungen.
- Solutions.
- Capabilities.
- Systemfunktionen.
- Dateien und Ordner.
- Benutzerdefinierte Verknüpfungen.

Dabei gilt:

- Anheftungen MÜSSEN unabhängig von den referenzierten Ressourcen gespeichert werden.
- Mehrere Verknüpfungen auf dasselbe Ziel DÜRFEN unterstützt werden.
- Das Entfernen einer Anheftung DARF die referenzierte Ressource nicht löschen.
- Nicht mehr verfügbare Ziele MÜSSEN kontrolliert behandelt werden.

## 4. Verwaltung

NovaOS unterstützt:

- Anheften.
- Lösen.
- Verschieben.
- Umbenennen der Verknüpfung.
- Ändern des Symbols.
- Gruppieren.
- Sortieren.

Dabei gilt:

- Änderungen MÜSSEN unmittelbar sichtbar werden.
- Die Reihenfolge MUSS benutzerdefiniert festgelegt werden können.
- Manuelle Anordnungen DÜRFEN nicht ohne Zustimmung überschrieben werden.
- Anheftungen MÜSSEN über das Startmenü und autorisierte Shell-Aktionen verwaltbar sein.

## 5. Layout und Gruppierung

Angeheftete Elemente können in Listen, Rastern oder benutzerdefinierten Gruppen dargestellt werden.

Dabei gilt:

- Gruppen MÜSSEN benannt und neu angeordnet werden können.
- Unterschiedliche Symbolgrößen SOLLEN unterstützt werden.
- Das Layout MUSS unterschiedliche Bildschirmgrößen und DPI-Skalierungen berücksichtigen.
- Die Darstellung MUSS bei Platzmangel scrollbar oder adaptiv sein.
- Layoutinformationen MÜSSEN getrennt von den Zielreferenzen gespeichert werden.

## 6. Starten und Aktivieren

Die Ausführung erfolgt über die zuständigen NovaOS-Systeme.

Dabei gilt:

- Anwendungen MÜSSEN über Nova Application Launcher gestartet werden.
- Solutions MÜSSEN über ihre zuständige Runtime aktiviert werden.
- Capabilities MÜSSEN über autorisierte Capability-Schnittstellen aufgerufen werden.
- Bereits laufende Anwendungen MÜSSEN entsprechend ihrer Instanzrichtlinie aktiviert oder erneut gestartet werden können.
- Angeheftete Elemente DÜRFEN keine eigenen Ausführungsmechanismen implementieren.

## 7. Synchronisation mit der Taskleiste

Startmenü und Taskleiste verwenden kompatible Anwendungsidentitäten und Zielreferenzen.

Dabei gilt:

- Anheftungen im Startmenü und in der Taskleiste MÜSSEN unabhängig konfigurierbar bleiben.
- Ein Element DARF gleichzeitig an beiden Stellen angeheftet sein.
- Änderungen an der zugrunde liegenden Anwendung MÜSSEN in beiden Bereichen berücksichtigt werden.
- Eine gemeinsame Registry DARF verwendet werden, sofern getrennte Layout- und Anheftungszustände erhalten bleiben.

## 8. Zustand und Wiederherstellung

Der Pin State Manager speichert benutzerbezogen:

- Angeheftete Elemente.
- Reihenfolge und Gruppen.
- Individuelle Namen und Symbole.
- Layoutinformationen.

Dabei gilt:

- Änderungen MÜSSEN dauerhaft gespeichert werden.
- Anheftungen MÜSSEN nach einem Shell-Neustart wiederhergestellt werden.
- Nicht verfügbare Ziele DÜRFEN die Wiederherstellung nicht verhindern.
- Beschädigte Konfigurationen MÜSSEN kontrolliert behandelt werden.
- Benutzerdefinierte Anordnungen SOLLEN exportierbar und wiederherstellbar sein.

## 9. Performance und Sicherheit

Nova Pinned Applications MUSS ressourcenschonend und sicher arbeiten.

Dabei gilt:

- Symbole und Metadaten SOLLEN zwischengespeichert werden.
- Änderungen MÜSSEN ereignisgesteuert verarbeitet werden.
- Unveränderte Elemente DÜRFEN nicht permanent neu gerendert werden.
- Die Darstellung MUSS Damage Tracking unterstützen.
- Anheftungen DÜRFEN keine zusätzlichen Berechtigungen verleihen.
- Startaktionen MÜSSEN Nova.Trust berücksichtigen.
- Alle Funktionen MÜSSEN per Tastatur und über barrierefreie Eingabemethoden erreichbar sein.

## 10. Normative Anforderungen

- **START-PIN-001:** NovaOS MUSS dauerhaft angeheftete Startmenü-Elemente unterstützen.
- **START-PIN-002:** Anwendungen, Solutions und Capabilities MÜSSEN angeheftet werden können.
- **START-PIN-003:** Anheftungen MÜSSEN eindeutige Identitäten besitzen.
- **START-PIN-004:** Anheftungen MÜSSEN unabhängig von ihren Zielressourcen verwaltet werden.
- **START-PIN-005:** Elemente MÜSSEN frei angeordnet und gruppiert werden können.
- **START-PIN-006:** Benutzerdefinierte Anordnungen DÜRFEN nicht eigenmächtig verändert werden.
- **START-PIN-007:** Startaktionen MÜSSEN über die zuständigen Ausführungssysteme erfolgen.
- **START-PIN-008:** Startmenü- und Taskbar-Anheftungen MÜSSEN unabhängig konfigurierbar sein.
- **START-PIN-009:** Einstellungen MÜSSEN gespeichert und wiederhergestellt werden.
- **START-PIN-010:** Nicht verfügbare Ziele DÜRFEN das Startmenü nicht beeinträchtigen.
- **START-PIN-011:** Die Darstellung MUSS Damage Tracking unterstützen.
- **START-PIN-012:** Anheftungen MÜSSEN Nova.Trust und Accessibility berücksichtigen.
- **START-PIN-013:** Nova Pinned Applications MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 11. Integration

- **Nova Start Menu:** Darstellung angehefteter Elemente.
- **Nova Application Launcher:** Starten von Anwendungen.
- **Nova Application Search:** Auffinden und Anheften von Elementen.
- **Nova Taskbar Application Management:** Gemeinsame Anwendungsidentitäten.
- **Nova Capability System:** Capability-Verknüpfungen.
- **Nova Solutions:** Solution-Verknüpfungen.
- **Nova UI Framework:** Darstellung und Interaktion.
- **Nova Data Transfer:** Drag & Drop.
- **Nova Shell State Manager:** Speicherung und Wiederherstellung.
- **Nova.Trust:** Sicherheit und Berechtigungen.

## 12. Ergebnis

NovaOS erhält ein flexibles System zur Verwaltung angehefteter Anwendungen, Solutions und Capabilities im Startmenü.

Benutzer können ihre bevorzugten Funktionen individuell organisieren und dauerhaft verfügbar halten, ohne die zugrunde liegenden Ressourcen zu verändern.

**Grundsatz:** Angeheftete Elemente gehören zur persönlichen Arbeitsumgebung des Benutzers. NovaOS darf ihre Anordnung unterstützen, aber nicht ungefragt bestimmen.