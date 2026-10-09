# NPSPEC-STUDIO-RIBBON-ARCHITECTURE-0001

**Status:** Angenommen

## Zweck

Legt die technische Trennung von Befehlen, Darstellung und Kontext fest.

## Festlegungen

- Befehle werden als stabile Command-IDs in einer zentralen Registry registriert.
- Ribbon-Ansichten binden sich deklarativ an Commands und deren Zustand.
- Ausführung und Berechtigungsprüfung erfolgen außerhalb der visuellen Controls.
- Kontextwechsel aktualisieren Bindungen, ohne Commands zu duplizieren.

## Ergebnis

Ein austauschbares Ribbon-Frontend über einer gemeinsamen Command-Schicht.
