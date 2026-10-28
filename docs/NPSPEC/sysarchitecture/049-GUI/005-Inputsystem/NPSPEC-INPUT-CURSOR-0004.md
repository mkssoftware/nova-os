# NPSPEC-INPUT-CURSOR-0004 – Nova Cursor Confinement

**Status:** Angenommen  
**Bereich:** Input / Cursor / Confinement  
**System:** NovaOS

## 1. Zweck

Nova Cursor Confinement definiert die kontrollierte Begrenzung der Cursorbewegung auf bestimmte Bereiche innerhalb von NovaOS.

Ziel ist die Unterstützung von Anwendungen wie Spielen, CAD-Programmen, Remote-Desktops und spezialisierten Benutzeroberflächen, ohne die Kontrolle des Benutzers über den Systemcursor einzuschränken.

Cursorbegrenzungen müssen sicher, vorhersehbar und jederzeit durch das System aufhebbar sein.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Cursor Confinement Manager | Verwaltung aktiver Cursorbegrenzungen |
| Confinement Region Controller | Definition und Aktualisierung zulässiger Bereiche |
| Confinement Policy Controller | Prüfung von Berechtigungen und Prioritäten |
| Cursor Position Validator | Begrenzung der logischen Cursorposition |

Die Cursorbegrenzung erfolgt innerhalb der zentralen Cursorverwaltung.

Sie ist unabhängig davon, ob der Cursor durch Hardware oder Software dargestellt wird.

## 3. Confinement-Modell

NovaOS unterstützt folgende Begrenzungsarten:

| Modus | Beschreibung |
|---|---|
| None | Keine Cursorbegrenzung |
| Window | Begrenzung auf den Bereich eines Fensters |
| ClientArea | Begrenzung auf den Inhaltsbereich eines Fensters |
| Region | Begrenzung auf einen definierten Bereich |
| Display | Begrenzung auf einen bestimmten Bildschirm |
| RelativeMode | Relative Zeigereingabe ohne reguläre absolute Cursorbewegung |

`RelativeMode` ist ein eigenständiger Eingabemodus und keine geometrische Begrenzung. Er wird über dieselbe kontrollierte Anforderungs- und Freigabeverwaltung koordiniert.

## 4. Begrenzungsbereiche

Confinement-Bereiche werden in einem definierten logischen Koordinatensystem beschrieben.

Dabei gilt:

- Bereiche können fenster- oder displaybezogen sein.
- Display-Skalierung und Rotation werden berücksichtigt.
- Fensterbewegungen und Größenänderungen aktualisieren zugehörige Begrenzungen.
- Ungültige oder leere Bereiche werden zurückgewiesen.
- Bei Aktivierung wird eine außerhalb liegende Cursorposition in den zulässigen Bereich überführt.

Die Berechnung MUSS unabhängig vom verwendeten Cursor-Rendering-Backend erfolgen.

## 5. Aktivierung und Freigabe

Anwendungen können Cursor Confinement über eine definierte API anfordern.

Eine Anforderung enthält:

- Zielbereich oder Begrenzungsmodus.
- Zugehörigen Input Seat.
- Anfordernden Eingabekontext.
- Optionalen relativen Eingabemodus.

Die Aktivierung erfolgt nur nach erfolgreicher Berechtigungs- und Fokusprüfung.

Eine Begrenzung MUSS automatisch aufgehoben werden, wenn:

- Die Anwendung beendet wird oder abstürzt.
- Das zugehörige Fenster zerstört wird.
- Der berechtigte Eingabefokus verloren geht.
- Die Benutzersitzung beendet wird.
- Das System eine sicherheitskritische Eingabe übernimmt.

Anwendungen MÜSSEN ihre Begrenzungen ausdrücklich freigeben können.

## 6. Fokus und Prioritäten

Cursor Confinement wird durch den Nova Window Manager und Nova Input Focus Manager koordiniert.

Festlegungen:

- Reguläre Anwendungen dürfen den Cursor nur bei berechtigtem Eingabefokus begrenzen.
- Eine Anwendung darf keine Begrenzung einer fremden Sitzung verändern.
- Systemdialoge und geschützte Eingabekontexte besitzen Vorrang.
- Konkurrierende Anforderungen werden nach definierten Prioritätsregeln behandelt.
- Eine aufgehobene Begrenzung darf nicht ohne erneute Prüfung automatisch aktiviert werden.

Pro Input Seat DARF höchstens eine wirksame Cursorbegrenzung aktiv sein.

## 7. Relative Eingabe

Der relative Eingabemodus ermöglicht Anwendungen die Verarbeitung von Bewegungsdifferenzen unabhängig von der sichtbaren Cursorposition.

Dabei gilt:

- Relative Bewegungsdaten bleiben verfügbar.
- Der sichtbare Cursor kann ausgeblendet werden.
- Die logische Cursorposition muss nicht mit jeder relativen Bewegung verändert werden.
- Der Modus wird bei Fokusverlust oder Sitzungswechsel beendet.
- Die bisherige Cursorposition SOLL beim Verlassen des Modus wiederhergestellt werden können.

Relative Eingaben und Cursor Confinement MÜSSEN unabhängig voneinander nutzbar bleiben.

## 8. Sicherheit und Fehlerbehandlung

Nova Cursor Confinement unterliegt Nova Input Security und Nova.Trust.

- Unberechtigte Begrenzungsanforderungen werden zurückgewiesen.
- Das System MUSS jede Begrenzung unabhängig von der Anwendung aufheben können.
- Eine Anwendung DARF den Benutzer nicht dauerhaft innerhalb eines Bereichs festhalten.
- Fehlerhafte Koordinaten DÜRFEN keine ungültigen Cursorzustände verursachen.
- Bei Geräte- oder Displayverlust wird die Begrenzung angepasst oder aufgehoben.
- Bei Fehlern MUSS ein sicherer Zustand ohne anwendungsbedingte Begrenzung wiederhergestellt werden.

## 9. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS Cursor Confinement zentral verwalten.
- **INPUT-002:** Fenster-, Bereichs- und Displaybegrenzungen MÜSSEN unterstützt werden.
- **INPUT-003:** Begrenzungen MÜSSEN unabhängig vom Cursor-Rendering-Backend funktionieren.
- **INPUT-004:** Confinement-Anforderungen MÜSSEN auf Berechtigung und Eingabefokus geprüft werden.
- **INPUT-005:** Pro Input Seat DARF höchstens eine wirksame Begrenzung aktiv sein.
- **INPUT-006:** Fensterbewegungen und Größenänderungen MÜSSEN berücksichtigt werden.
- **INPUT-007:** Display-Skalierung und Rotation MÜSSEN korrekt verarbeitet werden.
- **INPUT-008:** Bei Fokusverlust MUSS eine anwendungsbezogene Begrenzung aufgehoben werden.
- **INPUT-009:** Bei Anwendungsabsturz MUSS die zugehörige Begrenzung automatisch aufgehoben werden.
- **INPUT-010:** Sicherheitskritische Systeminteraktionen MÜSSEN Cursorbegrenzungen außer Kraft setzen können.
- **INPUT-011:** Relative Zeigereingaben MÜSSEN unabhängig von geometrischen Begrenzungen unterstützt werden können.
- **INPUT-012:** Ungültige Begrenzungsbereiche MÜSSEN sicher behandelt werden.
- **INPUT-013:** Das System MUSS jederzeit die vollständige Cursorsteuerung wiederherstellen können.
- **INPUT-014:** Nova Cursor Confinement MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 10. Integration

- **Nova Cursor Management:** Cursorposition und Bewegungssteuerung.
- **Nova Hardware and Software Cursor:** Unabhängige Cursordarstellung.
- **Nova Mouse Input:** Relative und absolute Zeigereingaben.
- **Nova Multi-Device Input:** Input-Seat-Zuordnung.
- **Nova Input Focus Manager:** Fokusabhängige Aktivierung.
- **Nova Window Manager:** Fensterbereiche und Fokuswechsel.
- **Nova Display System:** Displaygrenzen und Koordinatentransformation.
- **Nova Input Security:** Zugriffskontrolle.
- **Nova.Trust:** Berechtigungen und Sicherheitsrichtlinien.

## 11. Ergebnis

NovaOS erhält eine sichere und flexible Architektur zur Begrenzung der Cursorbewegung.

Anwendungen können den Cursor innerhalb definierter Bereiche führen oder relative Eingaben verwenden, ohne dauerhaft die Kontrolle über den Systemcursor zu erhalten.

**Grundsatz:** Anwendungen dürfen die Cursorbewegung für ihre Aufgaben begrenzen. Die endgültige Kontrolle über den Cursor bleibt jederzeit beim Benutzer und beim Betriebssystem.