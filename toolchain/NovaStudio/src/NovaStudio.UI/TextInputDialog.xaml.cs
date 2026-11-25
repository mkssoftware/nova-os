using System.Windows;
using System.Windows.Input;

namespace NovaStudio.UI;

/// <summary>Wiederverwendbarer Eingabe-Dialog, siehe XAML-Kommentar.</summary>
public partial class TextInputDialog : Window
{
    public string Value => ValueBox.Text;

    public TextInputDialog(string title, string prompt, string initialValue = "")
    {
        InitializeComponent();
        Title = title;
        PromptText.Text = prompt;
        ValueBox.Text = initialValue;
        ValueBox.SelectAll();
        Loaded += (_, _) => ValueBox.Focus();
    }

    private void OnOkClick(object sender, RoutedEventArgs e) => DialogResult = true;
    private void OnCancelClick(object sender, RoutedEventArgs e) => DialogResult = false;

    private void OnValueBoxKeyDown(object sender, KeyEventArgs e)
    {
        if (e.Key == Key.Enter) { DialogResult = true; e.Handled = true; }
        else if (e.Key == Key.Escape) { DialogResult = false; e.Handled = true; }
    }
}
