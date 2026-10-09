# NPSPEC-STUDIO-UI-RESPONSIVENESS-0001

**Status:** Angenommen

## Zweck
Reaktionsfähige Oberfläche.

## Festlegungen
- Der UI-Thread darf nicht auf Build, Dateizugriff, Graphanalyse oder Netzwerk warten.
- Längere Aktionen müssen abbrechbar sein und einen nicht blockierenden Fortschritt anzeigen.
- Als Ziel gilt eine sichtbare Eingabereaktion innerhalb von 100 ms; Abweichungen werden erfasst.
- Ergebnisse asynchroner Arbeiten dürfen veraltete Editorzustände nicht überschreiben.

## Ergebnis
Interaktionen reagieren unmittelbar und lang laufende Aufgaben bleiben kontrollierbar.
