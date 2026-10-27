# NPSPEC-WINDOW-ARCH-0004 – Nova Window Security and Ownership

**Status:** Angenommen  
**Bereich:** Window Manager / Sicherheit und Besitzrechte  
**System:** NovaOS

## 1. Zweck

Nova Window Security and Ownership definiert die Besitzverhältnisse, Zugriffsrechte und Sicherheitsgrenzen innerhalb der NovaOS-Fensterverwaltung.

Ziel ist die zuverlässige Isolation von Anwendungen und der Schutz vor unberechtigten Fensteroperationen, Fokusübernahmen und Manipulationen geschützter Systemoberflächen.

## 2. Architektur

- Integration in Nova Window Manager und Nova.Trust.
- Capability-basierte Autorisierung sämtlicher sicherheitsrelevanter Fensteroperationen.
- Eindeutige Zuordnung jedes Fensters zu einem Sicherheitskontext.
- Trennung zwischen Fensterbesitz, Fensterhierarchie und Darstellungsrechten.
- Durchsetzung von Zugriffsregeln an vertrauenswürdigen Systemgrenzen.
- Schutz von Systemfenstern und privilegierten Oberflächen.
- Integration in Nova IPC und Display Protocol.
- Nachvollziehbare Protokollierung sicherheitsrelevanter Operationen.

## 3. Besitzmodell

Jedes Fenster besitzt folgende Sicherheitsinformationen:

| Eigenschaft | Beschreibung |
|---|---|
| Window ID | Eindeutige Fensteridentität |
| Owner ID | Identität des Besitzers |
| Security Context | Zugehöriger Sicherheitskontext |
| Capability Set | Zulässige Fensteroperationen |
| Parent ID | Optionale Fensterhierarchie |
| Protection Level | Schutzklassifizierung |
| Lifetime Generation | Schutz gegen veraltete Referenzen |

Der Besitzer ist grundsätzlich die Anwendung oder Systemkomponente, die das Fenster autorisiert erstellt hat.

Eine Eltern-Kind-Beziehung begründet keine zusätzlichen Zugriffsrechte.

## 4. Kernfunktionen

- Sichere Registrierung und Zuordnung von Fenstern.
- Prüfung von Fensteroperationen.
- Isolation unterschiedlicher Anwendungen.
- Schutz vor unberechtigter Fokusübernahme.
- Kontrolle von Fensterhierarchien.
- Schutz privilegierter Systemoberflächen.
- Kontrollierte Rechteübertragung.
- Widerruf erteilter Berechtigungen.
- Bereinigung von Besitzrechten bei Anwendungsbeendigung.
- Auditierung sicherheitsrelevanter Vorgänge.

## 5. Normative Anforderungen

- WINDOW-001: Jedes Fenster MUSS einem gültigen Sicherheitskontext zugeordnet sein.
- WINDOW-002: Fensterbesitz MUSS durch den Window Manager verbindlich verwaltet werden.
- WINDOW-003: Anwendungen DÜRFEN Fenster anderer Anwendungen nicht ohne entsprechende Capability verändern.
- WINDOW-004: Fensteroperationen MÜSSEN vor ihrer Ausführung autorisiert werden.
- WINDOW-005: Fensteridentitäten DÜRFEN allein keine Zugriffsberechtigung darstellen.
- WINDOW-006: Fensterreferenzen MÜSSEN gegen unberechtigte Wiederverwendung geschützt sein.
- WINDOW-007: Die Übertragung von Fensterrechten MUSS ausdrücklich autorisiert werden.
- WINDOW-008: Übertragene Rechte DÜRFEN die Berechtigungen des übertragenden Sicherheitskontexts nicht überschreiten.
- WINDOW-009: Berechtigungen MÜSSEN widerrufbar sein.
- WINDOW-010: Anwendungen DÜRFEN den Tastaturfokus nicht eigenmächtig übernehmen.
- WINDOW-011: Geschützte Systemfenster MÜSSEN gegen unberechtigte Manipulation und Überlagerung geschützt werden.
- WINDOW-012: Anwendungen DÜRFEN keine privilegierten Systemoberflächen imitieren, sofern dadurch eine sicherheitsrelevante Verwechslung entsteht.
- WINDOW-013: Fensterhierarchien DÜRFEN Sicherheitsgrenzen nicht umgehen.
- WINDOW-014: Der Zugriff auf fremde Fensterinhalte MUSS gesondert autorisiert werden.
- WINDOW-015: Bildschirmaufnahmen und Fensterfreigaben MÜSSEN die geltenden Datenschutz- und Capability-Regeln einhalten.
- WINDOW-016: Bei Anwendungsbeendigung MÜSSEN zugehörige Fensterrechte kontrolliert ungültig werden.
- WINDOW-017: Sicherheitsverletzungen MÜSSEN kontrolliert zurückgewiesen und diagnostizierbar sein.
- WINDOW-018: Sicherheitsentscheidungen MÜSSEN ohne KI-Abhängigkeit deterministisch erfolgen.

## 6. Window Capabilities

NovaOS definiert folgende grundlegende Fensterberechtigungen:

| Capability | Berechtigung |
|---|---|
| Window.Create | Eigene Fenster erstellen |
| Window.Modify | Autorisierte Fenster verändern |
| Window.Destroy | Autorisierte Fenster entfernen |
| Window.Focus.Request | Aktivierung anfordern |
| Window.Relation | Zulässige Fensterbeziehungen herstellen |
| Window.Embed | Fremde Fensterinhalte autorisiert einbetten |
| Window.Observe | Freigegebene Fenstermetadaten beobachten |
| Window.Capture | Autorisierte Fensterinhalte erfassen |
| Window.Share | Fensterzugriff kontrolliert freigeben |
| Window.System | Privilegierte Systemfensteroperationen |

Capabilities MÜSSEN auf konkrete Ressourcen und zulässige Operationen begrenzbar sein.

Die Vergabe einer Capability bedeutet nicht automatisch, dass jede angeforderte Operation ausgeführt werden muss. Zusätzliche Systemrichtlinien bleiben verbindlich.

## 7. Sicherheitsklassen

### Application Window

Reguläres Anwendungsfenster innerhalb des Sicherheitskontexts seines Besitzers.

### Shared Window

Fenster mit ausdrücklich freigegebenen Zugriffsrechten für weitere Sicherheitskontexte.

### Protected Window

Fenster mit erhöhten Schutzanforderungen, beispielsweise für vertrauliche Inhalte.

### System Window

Privilegierte Systemoberfläche mit kontrollierten Darstellungs- und Eingaberechten.

### Secure Window

Besonders geschützte Oberfläche für Authentifizierung und andere sicherheitskritische Interaktionen.

Die Sicherheitsklasse MUSS durch eine vertrauenswürdige Systemkomponente festgelegt werden.

## 8. Autorisierungsablauf

1. **Request:** Anwendung fordert eine Fensteroperation an.
2. **Identify:** Aufrufenden Sicherheitskontext bestimmen.
3. **Resolve:** Fensteridentität und Besitzer ermitteln.
4. **Validate:** Capability und Ressourcenbindung prüfen.
5. **Policy:** Zusätzliche Sicherheitsrichtlinien anwenden.
6. **Execute:** Autorisierte Operation durchführen.
7. **Audit:** Sicherheitsrelevante Ergebnisse protokollieren.

Nicht autorisierte Anforderungen werden ohne Änderung des verbindlichen Fensterzustands zurückgewiesen.

## 9. Fehlerbehandlung und Zuverlässigkeit

- Ungültige Fensterreferenzen werden sicher zurückgewiesen.
- Veraltete Capabilities verlieren nach Widerruf ihre Gültigkeit.
- Anwendungen können keine fremden Fenster durch manipulierte IDs übernehmen.
- Fehlerhafte IPC-Nachrichten beeinflussen keine anderen Sicherheitskontexte.
- Geschützte Fenster bleiben auch bei Compositor-Neustarts geschützt.
- Sicherheitsentscheidungen werden an vertrauenswürdigen Systemgrenzen durchgesetzt.
- Audit-Daten dürfen keine unnötigen vertraulichen Fensterinhalte enthalten.
- Fehler der Rechteverwaltung führen zur Ablehnung der betroffenen Operation.

## 10. Ergebnis

NovaOS erhält ein konsequent Capability-basiertes Sicherheitsmodell für die Fensterverwaltung.

Jedes Fenster besitzt einen eindeutig definierten Besitzer und einen verbindlichen Sicherheitskontext.

Anwendungen können ausschließlich autorisierte Fensteroperationen durchführen, während geschützte Systemoberflächen, Fokusregeln und vertrauliche Fensterinhalte zuverlässig abgesichert werden.

**Grundsatz:** Der Besitz eines Fensters begründet Rechte an diesem Fenster, niemals automatisch Rechte an anderen Fenstern oder am gesamten Desktop.