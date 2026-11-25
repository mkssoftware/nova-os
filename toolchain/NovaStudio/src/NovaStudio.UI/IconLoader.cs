using System;
using System.Collections.Generic;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using SharpVectors.Converters;

namespace NovaStudio.UI;

/// <summary>
/// Laedt echte Fluent-UI-System-Icons (SVG, aus Conceptual/Icons/fluentui-
/// system-icons-main/assets/ kopiert und auf unsere helle Textfarbe
/// umgefaerbt - siehe Assets/Icons/*.svg) ueber SharpVectors als Vektor-
/// grafik. KEINE Rasterbilder/Bitmaps - bleibt bei jeder Groesse scharf.
/// Ein DrawingImage ist eine ImageSource und damit beliebig oft
/// wiederverwendbar (gefroren/Frozen), ein WPF Image-Element (das
/// tatsaechliche UIElement, das in den visuellen Baum eingehaengt wird)
/// kann dagegen JEWEILS NUR EINEN Eltern-Knoten haben - deshalb cached
/// diese Klasse nur die geparste DrawingImage, nicht das Image-Element
/// selbst, und erzeugt bei jedem Load()-Aufruf ein frisches Image.
/// </summary>
public static class IconLoader
{
    private static readonly Dictionary<string, DrawingImage> _cache = new();

    public static Image? Load(string iconName, double size = 16)
    {
        var drawingImage = LoadDrawingImage(iconName);
        if (drawingImage == null) return null;
        return new Image
        {
            Source = drawingImage,
            Width = size,
            Height = size,
            Stretch = Stretch.Uniform,
            SnapsToDevicePixels = true,
        };
    }

    public static DrawingImage? LoadDrawingImage(string iconName)
    {
        if (_cache.TryGetValue(iconName, out var cached)) return cached;

        string path = Path.Combine(AppContext.BaseDirectory, "Assets", "Icons", iconName + ".svg");
        if (!File.Exists(path)) return null;

        try
        {
            var reader = new FileSvgReader(new SharpVectors.Renderers.Wpf.WpfDrawingSettings());
            var drawing = reader.Read(path);
            if (drawing == null) return null;
            var image = new DrawingImage(drawing);
            image.Freeze();
            _cache[iconName] = image;
            return image;
        }
        catch (Exception)
        {
            // Ehrlich fehlschlagen statt eine kaputte/fehlende Icon-Datei
            // zum Startabsturz zu machen - der Button bleibt dann einfach
            // ohne Icon (Text allein ist immer noch da).
            return null;
        }
    }
}
