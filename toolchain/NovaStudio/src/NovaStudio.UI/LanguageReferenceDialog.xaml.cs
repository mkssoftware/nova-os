using System.Windows;
using System.Windows.Controls;

namespace NovaStudio.UI;

/// <summary>
/// Zeigt die echte NovaLang-Sprachreferenz (siehe Kommentar in der XAML-
/// Datei) - dieselbe Liste wie Syntax/NovaLang.xshd, direkt aus dem
/// Compiler-Quellcode abgeglichen statt erfunden.
/// </summary>
public partial class LanguageReferenceDialog : Window
{
    private static readonly (string Name, string Desc)[] Keywords =
    {
        ("Module", "Beginnt ein Modul (Namensraum der obersten Ebene)."),
        ("End", "Schließt Module/Function/If/While (z. B. 'End Function')."),
        ("Function", "Deklariert eine Funktion."),
        ("As", "Trennt Name und Typ (z. B. 'Dim x As Integer', 'Function Main() As Int32')."),
        ("Return", "Beendet eine Funktion mit einem Rückgabewert."),
        ("Dim", "Deklariert eine lokale Variable."),
        ("If / Then / Else", "Bedingte Verzweigung."),
        ("While", "Schleife mit Vorabbedingung."),
    };

    private static readonly (string Name, string Desc)[] Types =
    {
        ("Integer", "Ganzzahl-Variablentyp bei 'Dim'/Parametern."),
        ("String", "Zeichenketten-Variablentyp bei 'Dim'."),
        ("Buffer", "Roher Speicherpuffer-Typ."),
        ("Int32", "EINZIG unterstützter Function-Rückgabetyp (nicht 'Integer')."),
    };

    private static readonly (string Name, string Desc)[] Builtins =
    {
        ("Console.WriteLine(s)", "Gibt eine Zeichenkette aus (siehe hello_nova.nova)."),
        ("WriteBuffer", "Schreibt einen Puffer direkt."),
        ("AddressOf", "Liefert die Adresse einer Variable/Funktion."),
        ("PeekInt / PeekByte", "Liest 4/1 Byte aus einer Speicheradresse."),
        ("PokeInt / PokeByte / PokeString", "Schreibt Werte an eine Speicheradresse."),
        ("SysCall", "Direkter, allgemeiner Syscall-Aufruf (service/operation/Argumente)."),
    };

    public LanguageReferenceDialog()
    {
        InitializeComponent();
        Fill(KeywordsList, Keywords);
        Fill(TypesList, Types);
        Fill(BuiltinsList, Builtins);
    }

    private static void Fill(ItemsControl target, (string Name, string Desc)[] entries)
    {
        foreach (var (name, desc) in entries)
        {
            var panel = new StackPanel { Margin = new Thickness(0, 2, 0, 2) };
            panel.Children.Add(new TextBlock { Text = name, FontFamily = (System.Windows.Media.FontFamily)Application.Current.FindResource("NovaEditorFontFamily") });
            panel.Children.Add(new TextBlock
            {
                Text = desc,
                FontSize = 12,
                Foreground = (System.Windows.Media.Brush)Application.Current.FindResource("SecondaryTextBrush"),
                TextWrapping = TextWrapping.Wrap,
            });
            target.Items.Add(panel);
        }
    }

    private void OnCloseClick(object sender, RoutedEventArgs e) => Close();
}
