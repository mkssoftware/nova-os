using System.Windows.Media;
using ICSharpCode.AvalonEdit.CodeCompletion;
using ICSharpCode.AvalonEdit.Document;
using ICSharpCode.AvalonEdit.Editing;

namespace NovaStudio.UI;

/// <summary>
/// Ein Vorschlag im IntelliSense-Vervollstaendigungsfenster. KEIN
/// Language-Server-Ergebnis (es gibt keinen NovaLang Language Server,
/// siehe IMPLEMENTATION_STATUS.md) - nur lexikalische Vervollstaendigung:
/// echte Compiler-Schluesselwoerter/Builtins (siehe NovaLangCompletionProvider)
/// plus Namen, die tatsaechlich im geoeffneten Dokument per einfachem
/// Text-Scan gefunden wurden. Keine Typpruefung, kein Go-to-Definition,
/// keine projektweite Symbolaufloesung - das bleibt bewusst den bereits
/// deaktivierten "Gehe zu Definition"/"Alle Verweise"-Buttons vorbehalten.
/// </summary>
public sealed class NovaLangCompletionData : ICompletionData
{
    public NovaLangCompletionData(string text, string description)
    {
        Text = text;
        Description = description;
    }

    public System.Windows.Media.ImageSource? Image => null;
    public string Text { get; }
    public object Content => Text;
    public object Description { get; }
    public double Priority => 0;

    public void Complete(TextArea textArea, ISegment completionSegment, System.EventArgs insertionRequestEventArgs)
    {
        textArea.Document.Replace(completionSegment, Text);
    }
}
