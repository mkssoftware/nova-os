using System.Windows;

namespace NovaStudio.UI;

/// <summary>
/// Minimaler Commit-Nachrichten-Dialog - WPF hat kein eingebautes InputBox.
/// Kein Platzhalter: der eingegebene Text wird echt an "git commit -m"
/// uebergeben (siehe OnGitCommitClick in MainWindow.xaml.cs).
/// </summary>
public partial class CommitDialog : Window
{
    public string CommitMessage => MessageBox.Text;

    public CommitDialog()
    {
        InitializeComponent();
        MessageBox.Focus();
    }

    private void OnCommitClick(object sender, RoutedEventArgs e)
    {
        DialogResult = true;
    }

    private void OnCancelClick(object sender, RoutedEventArgs e)
    {
        DialogResult = false;
    }
}
