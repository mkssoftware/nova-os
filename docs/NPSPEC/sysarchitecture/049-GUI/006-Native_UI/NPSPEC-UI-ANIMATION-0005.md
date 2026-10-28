# NPSPEC-UI-ANIMATION-0005 – Nova Reduced Motion Support

**Status:** Angenommen  
**Bereich:** UI / Animation / Accessibility  
**System:** NovaOS

## 1. Zweck

Nova Reduced Motion Support definiert die systemweite Anpassung und Reduzierung visueller Bewegungen innerhalb der nativen NovaOS-Benutzeroberfläche.

Ziel ist eine barrierefreie, ruhige und ressourcenschonende Benutzeroberfläche, die individuelle Bedürfnisse berücksichtigt, ohne Funktionalität einzuschränken.

Animationen dürfen reduziert oder deaktiviert werden, während sämtliche Funktionen und Zustandsänderungen vollständig erhalten bleiben.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Motion Preference Manager | Verwaltung der Bewegungseinstellungen |
| Motion Policy Resolver | Ermittlung der gültigen Bewegungsrichtlinien |
| Animation Adaptation Manager | Anpassung von Animationen |
| Motion Accessibility Controller | Durchsetzung der Barrierefreiheit |
| Motion Preference Observer | Benachrichtigung bei Einstellungsänderungen |

Die Umsetzung erfolgt über die Nova Animation Engine.

## 3. Bewegungsprofile

NovaOS unterstützt folgende Bewegungsprofile:

| Profil | Verhalten |
|---|---|
| Full | Vollständige Animationen |
| Reduced | Reduzierte Bewegung und vereinfachte Übergänge |
| None | Keine dekorativen Bewegungsanimationen |
| Custom | Benutzerdefinierte Bewegungseinstellungen |

Das Profil `Reduced` SOLL intensive Bewegungen durch kurze Überblendungen oder unmittelbare Zustandswechsel ersetzen.

Das Profil `None` MUSS sämtliche nicht notwendigen Bewegungsanimationen deaktivieren.

## 4. Systemeinstellungen

Bewegungspräferenzen werden zentral in den NovaOS-Einstellungen verwaltet.

Dabei gilt:

- Benutzer können ihr Bewegungsprofil jederzeit ändern.
- Änderungen werden ohne Neustart übernommen.
- Anwendungen und Controls MÜSSEN die wirksamen Einstellungen berücksichtigen.
- Benutzerdefinierte Einstellungen MÜSSEN dauerhaft gespeichert werden können.
- Die Einstellungen MÜSSEN auch ohne KI verfügbar sein.

NovaOS DARF ein geeignetes Profil vorschlagen, jedoch niemals ohne Zustimmung dauerhaft erzwingen.

## 5. Anpassung von Animationen

Die Animation Engine passt Animationen anhand der wirksamen Bewegungsrichtlinie an.

| Animation | Reduced | None |
|---|---|---|
| Fensterbewegung | Kurze Überblendung | Sofortiger Zustandswechsel |
| Skalierung | Reduziert oder ersetzt | Deaktiviert |
| Rotation | Deaktiviert oder ersetzt | Deaktiviert |
| Parallax | Deaktiviert | Deaktiviert |
| Scrollanimation | Reduziert | Sofortiges Scrollen |
| Hover-Übergang | Kurzer Farbwechsel | Sofortiger Farbwechsel |
| Ladeanimation | Ruhige Statusanzeige | Statische Statusanzeige |
| Themewechsel | Kurze Überblendung | Sofortiger Wechsel |

Die konkrete Anpassung MUSS zur funktionalen Bedeutung der Animation passen.

## 6. Funktionale Animationen

Nicht jede zeitliche Veränderung ist rein dekorativ.

Beispiele sind:

- Fortschrittsanzeigen.
- Audio- und Videowiedergabe.
- Technische Visualisierungen.
- Echtzeitdaten.
- Benutzerinteraktionen mit direkter Bewegungssteuerung.

Dabei gilt:

- Funktionale Informationen MÜSSEN erhalten bleiben.
- Notwendige Bewegungen DÜRFEN weiterhin dargestellt werden.
- Wo möglich, SOLLEN bewegungsarme Alternativen angeboten werden.
- Das Deaktivieren dekorativer Animationen DARF keine funktionalen Abläufe unterbrechen.

Reduced Motion ist keine allgemeine Deaktivierung dynamischer Inhalte.

## 7. Visual States und Transitions

Nova Reduced Motion Support arbeitet mit Nova Visual State Styling und Nova Transitions zusammen.

Dabei gilt:

- Funktionale Zustandsänderungen erfolgen unabhängig von Animationen.
- Hover-, Focus-, Pressed- und Disabled-Zustände bleiben erkennbar.
- Transitions können verkürzt, ersetzt oder deaktiviert werden.
- Sicherheitsrelevante Zustandsanzeigen MÜSSEN unmittelbar wirksam werden.
- Reduzierte Animationen DÜRFEN keine zusätzlichen Bedienhandlungen erfordern.

## 8. Dynamische Anpassung

Ändert der Benutzer sein Bewegungsprofil während einer laufenden Animation, MUSS NovaOS die Änderung unmittelbar berücksichtigen.

Dabei gilt:

- Laufende Animationen werden angepasst oder kontrolliert beendet.
- Der gültige Zielzustand bleibt erhalten.
- Änderungen DÜRFEN keine unkontrollierten visuellen Sprünge verursachen.
- Neue Animationen verwenden automatisch das aktuelle Profil.
- Eine Rückkehr zu vollständigen Animationen erfordert keinen Neustart.

## 9. Geltungsbereich und Prioritäten

Bewegungsrichtlinien können auf mehreren Ebenen definiert werden:

1. Systemweite Barrierefreiheitseinstellung.
2. Benutzerdefinierte Bewegungspräferenz.
3. Anwendungsspezifische Einstellung.
4. Animationsspezifische Definition.

Dabei gilt:

- Barrierefreiheitseinstellungen besitzen Vorrang vor dekorativen Animationen.
- Anwendungen DÜRFEN strengere Reduzierungen verwenden.
- Anwendungen DÜRFEN die wirksame systemweite Reduzierung nicht eigenmächtig aufheben.
- Konflikte MÜSSEN deterministisch aufgelöst werden.

## 10. Performance und Energieeffizienz

Reduzierte Animationen sollen gleichzeitig den Ressourcenverbrauch senken.

Dabei gilt:

- Deaktivierte Animationen DÜRFEN keine Animationsframes erzeugen.
- Nicht benötigte Animationstimer werden beendet.
- Reduzierte Übergänge SOLLEN weniger Render- und Layoutaktualisierungen benötigen.
- Damage Tracking bleibt aktiv.
- Das System DARF keine unsichtbaren Ersatzanimationen berechnen.

## 11. NovaStudio und Logic Graph

NovaStudio unterstützt:

- Vorschau sämtlicher Bewegungsprofile.
- Kennzeichnung bewegungsintensiver Animationen.
- Definition alternativer Übergänge.
- Prüfung der Reduced-Motion-Kompatibilität.
- Simulation dynamischer Einstellungsänderungen.

Logic Graph MUSS funktional unabhängig vom gewählten Bewegungsprofil arbeiten.

Animationsbezogene Event Nodes MÜSSEN auch bei deaktivierten Animationen ein definiertes Abschlussverhalten besitzen.

## 12. Sicherheit und Datenschutz

Nova Reduced Motion Support unterliegt Nova.Trust.

Dabei gilt:

- Anwendungen DÜRFEN Bewegungspräferenzen nur im erforderlichen Umfang auslesen.
- Einstellungen DÜRFEN keine zusätzlichen Capabilities verleihen.
- Sicherheitsrelevante UI-Elemente MÜSSEN unabhängig vom Bewegungsprofil sichtbar und bedienbar bleiben.
- Die Funktion MUSS vollständig lokal und ohne Cloudverbindung arbeiten.

## 13. Normative Anforderungen

- **UI-001:** NovaOS MUSS systemweite Reduced-Motion-Einstellungen bereitstellen.
- **UI-002:** Die Profile Full, Reduced und None MÜSSEN unterstützt werden.
- **UI-003:** Benutzerdefinierte Bewegungsprofile SOLLEN unterstützt werden.
- **UI-004:** Bewegungspräferenzen MÜSSEN zur Laufzeit änderbar sein.
- **UI-005:** Änderungen MÜSSEN ohne Neustart wirksam werden.
- **UI-006:** Dekorative Animationen MÜSSEN reduzierbar oder deaktivierbar sein.
- **UI-007:** Funktionale Zustandsänderungen MÜSSEN unabhängig von Animationen erfolgen.
- **UI-008:** Bewegungsarme Alternativen SOLLEN bereitgestellt werden.
- **UI-009:** Laufende Animationen MÜSSEN kontrolliert angepasst werden können.
- **UI-010:** Systemweite Barrierefreiheitseinstellungen MÜSSEN Vorrang besitzen.
- **UI-011:** Deaktivierte Animationen DÜRFEN keine unnötigen Frames erzeugen.
- **UI-012:** Animation Scheduling MUSS die Bewegungsrichtlinien berücksichtigen.
- **UI-013:** Visual States MÜSSEN ohne Animation erkennbar bleiben.
- **UI-014:** NovaStudio MUSS Reduced-Motion-Profile simulieren können.
- **UI-015:** Logic Graph MUSS unabhängig vom Bewegungsprofil funktionsfähig bleiben.
- **UI-016:** Reduced Motion MUSS unabhängig vom Renderbackend funktionieren.
- **UI-017:** Die Einstellungen MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-018:** Nova Reduced Motion Support MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova Animation Engine:** Anpassung aktiver Animationen.
- **Nova Property Animations:** Reduzierung animierter Eigenschaften.
- **Nova Transitions:** Bewegungsarme Zustandsübergänge.
- **Nova Animation Scheduling:** Vermeidung unnötiger Frames.
- **Nova Visual State Styling:** Erhaltung erkennbarer Zustände.
- **Nova Dynamic Theme Switching:** Bewegungsarme Themewechsel.
- **Nova UI Accessibility:** Systemweite Barrierefreiheit.
- **Nova UI Property System:** Dynamische Einstellungsübernahme.
- **Nova UI Renderer Abstraction:** Rendererunabhängige Umsetzung.
- **Nova Compositor:** Reduzierung visueller Effekte.
- **NovaStudio:** Vorschau und Validierung.
- **Nova Logic Graph:** Funktionale Animationssteuerung.
- **Nova.Trust:** Sicherheit und Datenschutz.

## 15. Ergebnis

NovaOS erhält eine systemweite, dynamisch anpassbare Unterstützung für reduzierte Bewegungen.

Benutzer können Animationen individuell reduzieren oder deaktivieren, ohne auf Funktionen oder verständliche Zustandsrückmeldungen verzichten zu müssen.

Die Umsetzung erfolgt zentral über die Nova Animation Engine und berücksichtigt Barrierefreiheit, Benutzerpräferenzen und Ressourcenökonomie.

**Grundsatz:** Bewegung unterstützt die Bedienung, ist aber niemals Voraussetzung dafür.