from __future__ import annotations

import html
import os
import re
import subprocess
import sys
import threading
import webbrowser
from pathlib import Path
from tkinter import BooleanVar, StringVar, Tk, filedialog, messagebox
from tkinter import ttk

try:
    import markdown
except ImportError:
    markdown = None

try:
    import pygments
except ImportError:
    pygments = None


APP_NAME = "Markdown zu HTML"
APP_VERSION = "1.2.0"


# ============================================================
# Hilfsfunktionen
# ============================================================

def natural_sort_key(value: str) -> list:
    """
    Erzeugt einen Sortierschlüssel für eine natürliche Sortierung.

    Beispiel:
        Kapitel2.md
        Kapitel10.md

    wird korrekt als Kapitel2 vor Kapitel10 sortiert.
    """
    return [
        int(part) if part.isdigit() else part.lower()
        for part in re.split(r"(\d+)", value)
    ]


def create_slug(value: str) -> str:
    """
    Erzeugt eine HTML-kompatible ID aus einem Text.
    """
    value = value.strip().lower()
    value = (
        value.replace("ä", "ae")
        .replace("ö", "oe")
        .replace("ü", "ue")
        .replace("ß", "ss")
    )

    value = re.sub(r"<[^>]+>", "", value)
    value = re.sub(r"[^a-z0-9_-]+", "-", value)
    value = re.sub(r"-+", "-", value)

    return value.strip("-") or "abschnitt"


def make_unique_slug(value: str, used_slugs: set[str]) -> str:
    """
    Erzeugt eine eindeutige HTML-ID.
    """
    base_slug = create_slug(value)
    slug = base_slug
    counter = 2

    while slug in used_slugs:
        slug = f"{base_slug}-{counter}"
        counter += 1

    used_slugs.add(slug)
    return slug


def read_text_file(file_path: Path) -> str:
    """
    Liest eine Textdatei mit mehreren möglichen Kodierungen.
    """
    encodings = (
        "utf-8",
        "utf-8-sig",
        "cp1252",
        "latin-1",
    )

    for encoding in encodings:
        try:
            return file_path.read_text(encoding=encoding)
        except UnicodeDecodeError:
            continue

    return file_path.read_text(
        encoding="utf-8",
        errors="replace",
    )


def open_path_in_file_manager(path: Path) -> None:
    """
    Öffnet einen Ordner im Dateimanager des Betriebssystems.
    """
    try:
        if sys.platform.startswith("win"):
            os.startfile(path)  # type: ignore[attr-defined]
        elif sys.platform == "darwin":
            subprocess.run(["open", str(path)], check=False)
        else:
            subprocess.run(["xdg-open", str(path)], check=False)
    except Exception as error:
        raise RuntimeError(
            f"Der Ordner konnte nicht geöffnet werden:\n{error}"
        ) from error


# ============================================================
# Markdown-Verarbeitung
# ============================================================

def add_heading_ids(
    html_content: str,
    used_slugs: set[str],
) -> tuple[str, list[dict[str, str | int]]]:
    """
    Ergänzt Überschriften um eindeutige IDs und erzeugt die Daten
    für das Inhaltsverzeichnis.
    """
    headings: list[dict[str, str | int]] = []

    heading_pattern = re.compile(
        r"<h([1-6])([^>]*)>(.*?)</h\1>",
        flags=re.IGNORECASE | re.DOTALL,
    )

    def replace_heading(match: re.Match) -> str:
        level = int(match.group(1))
        attributes = match.group(2)
        content = match.group(3)

        plain_text = re.sub(r"<[^>]+>", "", content)
        plain_text = html.unescape(plain_text).strip()

        existing_id_match = re.search(
            r'\bid=["\']([^"\']+)["\']',
            attributes,
            flags=re.IGNORECASE,
        )

        if existing_id_match:
            slug = existing_id_match.group(1)

            if slug in used_slugs:
                slug = make_unique_slug(plain_text, used_slugs)
                attributes = re.sub(
                    r'\bid=["\'][^"\']+["\']',
                    f'id="{slug}"',
                    attributes,
                    count=1,
                    flags=re.IGNORECASE,
                )
            else:
                used_slugs.add(slug)
        else:
            slug = make_unique_slug(plain_text, used_slugs)
            attributes = f'{attributes} id="{slug}"'

        headings.append(
            {
                "level": level,
                "title": plain_text,
                "id": slug,
            }
        )

        return f"<h{level}{attributes}>{content}</h{level}>"

    updated_html = heading_pattern.sub(
        replace_heading,
        html_content,
    )

    return updated_html, headings


def create_table_of_contents(
    document_entries: list[dict],
) -> str:
    """
    Erstellt das HTML für das Inhaltsverzeichnis.
    """
    toc_parts = [
        '<nav class="toc" id="toc">',
        '<div class="toc-header">',
        '<div class="toc-title">Inhaltsverzeichnis</div>',
        '<button class="toc-close" type="button" '
        'onclick="toggleNavigation()" aria-label="Navigation schließen">',
        "×",
        "</button>",
        "</div>",
        '<div class="toc-content">',
    ]

    for document in document_entries:
        document_title = html.escape(document["title"])
        document_id = html.escape(document["document_id"])

        toc_parts.append(
            f'''
            <div class="toc-document">
                <a class="toc-document-link" href="#{document_id}">
                    <span class="toc-document-icon">▤</span>
                    <span>{document_title}</span>
                </a>
            '''
        )

        headings = document.get("headings", [])

        if headings:
            toc_parts.append('<div class="toc-headings">')

            for heading in headings:
                level = int(heading["level"])
                heading_title = html.escape(str(heading["title"]))
                heading_id = html.escape(str(heading["id"]))

                toc_parts.append(
                    f'''
                    <a
                        class="toc-heading toc-level-{level}"
                        href="#{heading_id}"
                    >
                        {heading_title}
                    </a>
                    '''
                )

            toc_parts.append("</div>")

        toc_parts.append("</div>")

    toc_parts.extend(
        [
            "</div>",
            "</nav>",
        ]
    )

    return "\n".join(toc_parts)


def convert_markdown_to_html(
    markdown_text: str,
) -> str:
    """
    Konvertiert Markdown in HTML.
    """
    if markdown is None:
        raise RuntimeError(
            "Das Python-Paket 'markdown' ist nicht installiert.\n\n"
            "Installation:\n"
            "pip install markdown"
        )

    if pygments is None:
        raise RuntimeError(
            "Das Python-Paket 'pygments' ist nicht installiert.\n\n"
            "Installation:\n"
            "pip install pygments"
        )

    return markdown.markdown(
        markdown_text,
        extensions=[
            "extra",
            "fenced_code",
            "codehilite",
            "tables",
            "sane_lists",
            "smarty",
            "toc",
        ],
        extension_configs={
            "codehilite": {
                "css_class": "codehilite",
                "guess_lang": False,
                "noclasses": False,
            },
            "toc": {
                "permalink": False,
            }
        },
        output_format="html5",
    )


def collect_markdown_files(
    source_directory: Path,
    recursive: bool,
    output_file: Path | None = None,
) -> list[Path]:
    """
    Sammelt alle Markdown-Dateien aus einem Ordner.
    """
    if recursive:
        files = list(source_directory.rglob("*.md"))
    else:
        files = list(source_directory.glob("*.md"))

    normalized_output = (
        output_file.resolve()
        if output_file is not None
        else None
    )

    filtered_files = []

    for file_path in files:
        if not file_path.is_file():
            continue

        if normalized_output is not None:
            try:
                if file_path.resolve() == normalized_output:
                    continue
            except OSError:
                pass

        filtered_files.append(file_path)

    filtered_files.sort(
        key=lambda path: natural_sort_key(
            str(path.relative_to(source_directory))
        )
    )

    return filtered_files


def create_html_document(
    source_directory: Path,
    markdown_files: list[Path],
    document_title: str,
) -> str:
    """
    Erzeugt das vollständige HTML-Dokument.
    """
    document_entries: list[dict] = []
    document_sections: list[str] = []
    used_slugs: set[str] = set()

    for index, file_path in enumerate(markdown_files, start=1):
        relative_path = file_path.relative_to(source_directory)
        markdown_text = read_text_file(file_path)

        converted_html = convert_markdown_to_html(markdown_text)

        document_id = make_unique_slug(
            f"document-{relative_path.as_posix()}",
            used_slugs,
        )

        converted_html, headings = add_heading_ids(
            converted_html,
            used_slugs,
        )

        display_title = file_path.stem
        display_title = display_title.replace("_", " ")
        display_title = display_title.replace("-", " ")
        display_title = re.sub(r"\s+", " ", display_title).strip()

        relative_display = html.escape(relative_path.as_posix())
        escaped_title = html.escape(display_title)

        document_entries.append(
            {
                "title": display_title,
                "document_id": document_id,
                "headings": headings,
            }
        )

        document_sections.append(
            f'''
            <article class="markdown-document" id="{document_id}">
                <header class="document-header">
                    <div class="document-number">
                        {index:02d}
                    </div>

                    <div class="document-title-area">
                        <h1 class="document-title">
                            {escaped_title}
                        </h1>

                        <div class="document-path">
                            {relative_display}
                        </div>
                    </div>
                </header>

                <div class="markdown-content">
                    {converted_html}
                </div>
            </article>
            '''
        )

    toc_html = create_table_of_contents(document_entries)
    content_html = "\n".join(document_sections)

    escaped_document_title = html.escape(document_title)
    escaped_source_directory = html.escape(str(source_directory))

    return f"""<!DOCTYPE html>
<html lang="de">
<head>
    <meta charset="UTF-8">

    <meta
        name="viewport"
        content="width=device-width, initial-scale=1.0"
    >

    <meta
        name="color-scheme"
        content="dark"
    >

    <title>{escaped_document_title}</title>

    <style>
        :root {{
            color-scheme: dark;

            --background: #0c0c0c;
            --background-secondary: #111111;
            --surface: rgba(32, 32, 32, 0.82);
            --surface-solid: #202020;
            --surface-hover: rgba(255, 255, 255, 0.075);
            --surface-active: rgba(255, 255, 255, 0.11);

            --border: rgba(255, 255, 255, 0.085);
            --border-strong: rgba(255, 255, 255, 0.15);

            --text: #ffffff;
            --text-secondary: #c8c8c8;
            --text-muted: #8f8f8f;

            --accent: #60cdff;
            --accent-hover: #86d8ff;
            --accent-muted: rgba(96, 205, 255, 0.15);

            --success: #6ccb5f;
            --warning: #fce100;
            --danger: #ff99a4;

            --code-background: #151515;
            --inline-code-background: rgba(255, 255, 255, 0.08);

            --sidebar-width: 320px;
            --header-height: 64px;

            --radius-small: 6px;
            --radius-medium: 10px;
            --radius-large: 14px;

            --shadow:
                0 16px 38px rgba(0, 0, 0, 0.40),
                0 2px 8px rgba(0, 0, 0, 0.30);

            --font:
                "Segoe UI Variable",
                "Segoe UI",
                system-ui,
                -apple-system,
                BlinkMacSystemFont,
                sans-serif;

            --mono-font:
                "Cascadia Code",
                "Cascadia Mono",
                "Consolas",
                monospace;
        }}

        * {{
            box-sizing: border-box;
        }}

        html {{
            scroll-behavior: smooth;
            scroll-padding-top: calc(var(--header-height) + 24px);
        }}

        body {{
            margin: 0;
            min-height: 100vh;
            background:
                radial-gradient(
                    circle at 15% 0%,
                    rgba(96, 205, 255, 0.08),
                    transparent 34rem
                ),
                radial-gradient(
                    circle at 100% 40%,
                    rgba(120, 90, 255, 0.055),
                    transparent 30rem
                ),
                var(--background);

            color: var(--text);
            font-family: var(--font);
            font-size: 15px;
            line-height: 1.65;
        }}

        a {{
            color: var(--accent);
            text-decoration: none;
        }}

        a:hover {{
            color: var(--accent-hover);
            text-decoration: underline;
        }}

        button,
        input {{
            font: inherit;
        }}

        .app-header {{
            position: fixed;
            top: 0;
            right: 0;
            left: 0;
            z-index: 1000;

            height: var(--header-height);

            display: flex;
            align-items: center;
            gap: 14px;

            padding: 0 20px;

            background: rgba(20, 20, 20, 0.78);
            border-bottom: 1px solid var(--border);

            backdrop-filter: blur(30px) saturate(150%);
            -webkit-backdrop-filter: blur(30px) saturate(150%);
        }}

        .menu-button,
        .header-button,
        .toc-close {{
            display: inline-flex;
            align-items: center;
            justify-content: center;

            border: 1px solid var(--border);
            background: rgba(255, 255, 255, 0.045);
            color: var(--text);

            border-radius: var(--radius-small);
            cursor: pointer;

            transition:
                background 120ms ease,
                border-color 120ms ease,
                transform 120ms ease;
        }}

        .menu-button {{
            width: 38px;
            height: 38px;
            font-size: 20px;
        }}

        .header-button {{
            min-height: 36px;
            padding: 7px 13px;
        }}

        .menu-button:hover,
        .header-button:hover,
        .toc-close:hover {{
            background: var(--surface-hover);
            border-color: var(--border-strong);
        }}

        .menu-button:active,
        .header-button:active,
        .toc-close:active {{
            background: var(--surface-active);
            transform: scale(0.97);
        }}

        .app-title-area {{
            min-width: 0;
            flex: 1;
        }}

        .app-title {{
            overflow: hidden;
            margin: 0;

            color: var(--text);
            font-size: 16px;
            font-weight: 600;
            line-height: 1.2;

            text-overflow: ellipsis;
            white-space: nowrap;
        }}

        .app-subtitle {{
            overflow: hidden;
            margin-top: 3px;

            color: var(--text-muted);
            font-size: 11px;
            line-height: 1.2;

            text-overflow: ellipsis;
            white-space: nowrap;
        }}

        .header-actions {{
            display: flex;
            gap: 8px;
        }}

        .toc {{
            position: fixed;
            top: var(--header-height);
            bottom: 0;
            left: 0;
            z-index: 900;

            width: var(--sidebar-width);

            background: rgba(24, 24, 24, 0.86);
            border-right: 1px solid var(--border);

            backdrop-filter: blur(35px) saturate(145%);
            -webkit-backdrop-filter: blur(35px) saturate(145%);

            transition: transform 180ms ease;
        }}

        .toc-header {{
            height: 58px;

            display: flex;
            align-items: center;
            justify-content: space-between;

            padding: 0 16px 0 20px;

            border-bottom: 1px solid var(--border);
        }}

        .toc-title {{
            font-size: 14px;
            font-weight: 600;
        }}

        .toc-close {{
            display: none;
            width: 34px;
            height: 34px;
            font-size: 22px;
        }}

        .toc-content {{
            height: calc(100% - 58px);
            overflow-y: auto;
            padding: 12px 10px 24px;
        }}

        .toc-document {{
            margin-bottom: 5px;
        }}

        .toc-document-link {{
            display: flex;
            align-items: center;
            gap: 10px;

            min-height: 38px;
            padding: 8px 10px;

            color: var(--text);
            font-size: 13px;
            font-weight: 600;

            border-radius: var(--radius-small);
        }}

        .toc-document-link:hover {{
            background: var(--surface-hover);
            color: var(--text);
            text-decoration: none;
        }}

        .toc-document-icon {{
            color: var(--accent);
        }}

        .toc-headings {{
            margin: 2px 0 8px 18px;
            padding-left: 12px;
            border-left: 1px solid var(--border);
        }}

        .toc-heading {{
            display: block;

            padding: 4px 8px;

            color: var(--text-muted);
            font-size: 12px;
            line-height: 1.4;

            border-radius: 4px;
        }}

        .toc-heading:hover {{
            background: var(--surface-hover);
            color: var(--text);
            text-decoration: none;
        }}

        .toc-level-1 {{
            font-weight: 600;
        }}

        .toc-level-2 {{
            padding-left: 14px;
        }}

        .toc-level-3 {{
            padding-left: 22px;
        }}

        .toc-level-4,
        .toc-level-5,
        .toc-level-6 {{
            padding-left: 30px;
        }}

        .app-main {{
            min-height: 100vh;

            margin-left: var(--sidebar-width);
            padding:
                calc(var(--header-height) + 32px)
                clamp(20px, 4vw, 64px)
                80px;
        }}

        .content-container {{
            width: min(100%, 1080px);
            margin: 0 auto;
        }}

        .hero {{
            margin-bottom: 32px;
            padding: 28px 30px;

            background:
                linear-gradient(
                    135deg,
                    rgba(96, 205, 255, 0.12),
                    rgba(255, 255, 255, 0.035)
                ),
                var(--surface);

            border: 1px solid var(--border);
            border-radius: var(--radius-large);

            box-shadow: var(--shadow);
            backdrop-filter: blur(20px);
            -webkit-backdrop-filter: blur(20px);
        }}

        .hero-label {{
            margin-bottom: 7px;

            color: var(--accent);
            font-size: 12px;
            font-weight: 600;

            letter-spacing: 0.08em;
            text-transform: uppercase;
        }}

        .hero h1 {{
            margin: 0;

            font-size: clamp(28px, 4vw, 44px);
            font-weight: 650;
            line-height: 1.12;
            letter-spacing: -0.03em;
        }}

        .hero-description {{
            margin: 14px 0 0;
            color: var(--text-secondary);
        }}

        .hero-meta {{
            display: flex;
            flex-wrap: wrap;
            gap: 8px;

            margin-top: 20px;
        }}

        .meta-item {{
            padding: 6px 10px;

            color: var(--text-secondary);
            font-size: 12px;

            background: rgba(0, 0, 0, 0.20);
            border: 1px solid var(--border);
            border-radius: 999px;
        }}

        .markdown-document {{
            margin-bottom: 32px;
            overflow: hidden;

            background: var(--surface);
            border: 1px solid var(--border);
            border-radius: var(--radius-large);

            box-shadow: var(--shadow);

            backdrop-filter: blur(24px);
            -webkit-backdrop-filter: blur(24px);
        }}

        .document-header {{
            display: flex;
            align-items: center;
            gap: 16px;

            padding: 20px 24px;

            background: rgba(255, 255, 255, 0.025);
            border-bottom: 1px solid var(--border);
        }}

        .document-number {{
            display: flex;
            align-items: center;
            justify-content: center;

            width: 42px;
            height: 42px;
            flex: 0 0 auto;

            color: var(--accent);
            font-size: 13px;
            font-weight: 700;

            background: var(--accent-muted);
            border: 1px solid rgba(96, 205, 255, 0.20);
            border-radius: 10px;
        }}

        .document-title-area {{
            min-width: 0;
        }}

        .document-title {{
            overflow: hidden;
            margin: 0;

            font-size: 18px;
            font-weight: 600;
            line-height: 1.3;

            text-overflow: ellipsis;
            white-space: nowrap;
        }}

        .document-path {{
            overflow: hidden;
            margin-top: 3px;

            color: var(--text-muted);
            font-size: 11px;

            text-overflow: ellipsis;
            white-space: nowrap;
        }}

        .markdown-content {{
            padding: clamp(24px, 5vw, 52px);
        }}

        .markdown-content > :first-child {{
            margin-top: 0;
        }}

        .markdown-content > :last-child {{
            margin-bottom: 0;
        }}

        .markdown-content h1,
        .markdown-content h2,
        .markdown-content h3,
        .markdown-content h4,
        .markdown-content h5,
        .markdown-content h6 {{
            position: relative;

            margin-top: 1.8em;
            margin-bottom: 0.65em;

            color: var(--text);
            font-weight: 600;
            line-height: 1.28;

            letter-spacing: -0.015em;
        }}

        .markdown-content h1 {{
            padding-bottom: 12px;
            font-size: 30px;
            border-bottom: 1px solid var(--border);
        }}

        .markdown-content h2 {{
            padding-bottom: 9px;
            font-size: 24px;
            border-bottom: 1px solid var(--border);
        }}

        .markdown-content h3 {{
            font-size: 20px;
        }}

        .markdown-content h4 {{
            font-size: 17px;
        }}

        .markdown-content h5 {{
            font-size: 15px;
        }}

        .markdown-content h6 {{
            color: var(--text-secondary);
            font-size: 14px;
        }}

        .markdown-content p {{
            margin: 0.85em 0;
            color: var(--text-secondary);
        }}

        .markdown-content strong {{
            color: var(--text);
            font-weight: 600;
        }}

        .markdown-content ul,
        .markdown-content ol {{
            margin: 0.8em 0;
            padding-left: 1.8em;
            color: var(--text-secondary);
        }}

        .markdown-content li {{
            margin: 0.35em 0;
        }}

        .markdown-content li::marker {{
            color: var(--accent);
        }}

        .markdown-content blockquote {{
            margin: 1.2em 0;
            padding: 14px 18px;

            color: var(--text-secondary);

            background: rgba(96, 205, 255, 0.07);
            border: 1px solid rgba(96, 205, 255, 0.13);
            border-left: 4px solid var(--accent);
            border-radius: 0 var(--radius-medium) var(--radius-medium) 0;
        }}

        .markdown-content blockquote p {{
            margin: 0.4em 0;
        }}

        .markdown-content code {{
            padding: 0.15em 0.4em;

            color: #f4f4f4;
            font-family: var(--mono-font);
            font-size: 0.9em;

            background: var(--inline-code-background);
            border: 1px solid var(--border);
            border-radius: 4px;
        }}

        .markdown-content pre {{
            position: relative;

            max-width: 100%;
            overflow: auto;

            margin: 1.2em 0;
            padding: 18px;

            background: var(--code-background);
            border: 1px solid var(--border);
            border-radius: var(--radius-medium);

            box-shadow:
                inset 0 1px 0 rgba(255, 255, 255, 0.025),
                0 8px 20px rgba(0, 0, 0, 0.18);
        }}

        .markdown-content pre code {{
            display: block;
            min-width: max-content;
            padding: 0;

            color: #e6e6e6;
            font-size: 13px;
            line-height: 1.55;

            background: transparent;
            border: 0;
            border-radius: 0;
        }}

        /* Syntaxfarben für Pygments / Markdown codehilite */
        .markdown-content .codehilite .hll {{ background: #3a3520; }}
        .markdown-content .codehilite .c,
        .markdown-content .codehilite .ch,
        .markdown-content .codehilite .cm,
        .markdown-content .codehilite .c1,
        .markdown-content .codehilite .cs {{ color: #6a9955; font-style: italic; }}
        .markdown-content .codehilite .k,
        .markdown-content .codehilite .kc,
        .markdown-content .codehilite .kd,
        .markdown-content .codehilite .kn,
        .markdown-content .codehilite .kp,
        .markdown-content .codehilite .kr,
        .markdown-content .codehilite .kt {{ color: #c586c0; }}
        .markdown-content .codehilite .n {{ color: #d4d4d4; }}
        .markdown-content .codehilite .na {{ color: #9cdcfe; }}
        .markdown-content .codehilite .nb,
        .markdown-content .codehilite .bp {{ color: #4ec9b0; }}
        .markdown-content .codehilite .nc,
        .markdown-content .codehilite .nd,
        .markdown-content .codehilite .ne,
        .markdown-content .codehilite .nf {{ color: #dcdcaa; }}
        .markdown-content .codehilite .nn {{ color: #4ec9b0; }}
        .markdown-content .codehilite .nv,
        .markdown-content .codehilite .vc,
        .markdown-content .codehilite .vg,
        .markdown-content .codehilite .vi {{ color: #9cdcfe; }}
        .markdown-content .codehilite .o,
        .markdown-content .codehilite .ow {{ color: #d4d4d4; }}
        .markdown-content .codehilite .m,
        .markdown-content .codehilite .mb,
        .markdown-content .codehilite .mf,
        .markdown-content .codehilite .mh,
        .markdown-content .codehilite .mi,
        .markdown-content .codehilite .mo,
        .markdown-content .codehilite .il {{ color: #b5cea8; }}
        .markdown-content .codehilite .s,
        .markdown-content .codehilite .sa,
        .markdown-content .codehilite .sb,
        .markdown-content .codehilite .sc,
        .markdown-content .codehilite .dl,
        .markdown-content .codehilite .sd,
        .markdown-content .codehilite .s1,
        .markdown-content .codehilite .s2,
        .markdown-content .codehilite .se,
        .markdown-content .codehilite .sh,
        .markdown-content .codehilite .si,
        .markdown-content .codehilite .sx,
        .markdown-content .codehilite .sr,
        .markdown-content .codehilite .ss {{ color: #ce9178; }}
        .markdown-content .codehilite .p {{ color: #d4d4d4; }}
        .markdown-content .codehilite .gd {{ color: #f48771; }}
        .markdown-content .codehilite .gi {{ color: #89d185; }}
        .markdown-content .codehilite .gh,
        .markdown-content .codehilite .gu {{ color: #569cd6; font-weight: 600; }}
        .markdown-content .codehilite .err {{ color: #f44747; }}

        .markdown-content hr {{
            margin: 2em 0;
            border: 0;
            border-top: 1px solid var(--border);
        }}

        .markdown-content img {{
            display: block;

            max-width: 100%;
            height: auto;
            margin: 1.4em auto;

            border: 1px solid var(--border);
            border-radius: var(--radius-medium);

            box-shadow: var(--shadow);
        }}

        .markdown-content table {{
            width: 100%;
            margin: 1.3em 0;

            overflow: hidden;
            border-collapse: separate;
            border-spacing: 0;

            border: 1px solid var(--border);
            border-radius: var(--radius-medium);
        }}

        .markdown-content th,
        .markdown-content td {{
            padding: 11px 13px;
            text-align: left;
            vertical-align: top;

            border-right: 1px solid var(--border);
            border-bottom: 1px solid var(--border);
        }}

        .markdown-content th {{
            color: var(--text);
            font-weight: 600;
            background: rgba(255, 255, 255, 0.055);
        }}

        .markdown-content td {{
            color: var(--text-secondary);
            background: rgba(0, 0, 0, 0.10);
        }}

        .markdown-content tr:last-child td {{
            border-bottom: 0;
        }}

        .markdown-content th:last-child,
        .markdown-content td:last-child {{
            border-right: 0;
        }}

        .markdown-content input[type="checkbox"] {{
            width: 16px;
            height: 16px;
            margin-right: 6px;
            accent-color: var(--accent);
            vertical-align: -2px;
        }}

        .back-to-top {{
            position: fixed;
            right: 24px;
            bottom: 24px;
            z-index: 800;

            display: flex;
            align-items: center;
            justify-content: center;

            width: 42px;
            height: 42px;

            color: var(--text);
            font-size: 20px;

            background: rgba(40, 40, 40, 0.86);
            border: 1px solid var(--border);
            border-radius: 50%;

            box-shadow: var(--shadow);

            opacity: 0;
            pointer-events: none;

            backdrop-filter: blur(20px);
            -webkit-backdrop-filter: blur(20px);

            transition:
                opacity 150ms ease,
                background 150ms ease,
                transform 150ms ease;
        }}

        .back-to-top.visible {{
            opacity: 1;
            pointer-events: auto;
        }}

        .back-to-top:hover {{
            color: var(--text);
            text-decoration: none;
            background: var(--surface-active);
            transform: translateY(-2px);
        }}

        .overlay {{
            position: fixed;
            inset: var(--header-height) 0 0 0;
            z-index: 850;

            display: none;

            background: rgba(0, 0, 0, 0.48);
            backdrop-filter: blur(3px);
            -webkit-backdrop-filter: blur(3px);
        }}

        ::selection {{
            color: #000000;
            background: var(--accent);
        }}

        ::-webkit-scrollbar {{
            width: 12px;
            height: 12px;
        }}

        ::-webkit-scrollbar-track {{
            background: transparent;
        }}

        ::-webkit-scrollbar-thumb {{
            background: rgba(255, 255, 255, 0.16);
            border: 3px solid transparent;
            border-radius: 999px;
            background-clip: padding-box;
        }}

        ::-webkit-scrollbar-thumb:hover {{
            background: rgba(255, 255, 255, 0.25);
            border: 3px solid transparent;
            background-clip: padding-box;
        }}

        @media print {{
            :root {{
                color-scheme: light;
            }}

            body {{
                color: #111111;
                background: #ffffff;
            }}

            .app-header,
            .toc,
            .overlay,
            .back-to-top {{
                display: none !important;
            }}

            .app-main {{
                margin: 0;
                padding: 0;
            }}

            .hero,
            .markdown-document {{
                color: #111111;
                background: #ffffff;
                border: 1px solid #dddddd;
                box-shadow: none;
            }}

            .markdown-content p,
            .markdown-content li,
            .markdown-content td {{
                color: #222222;
            }}

            .markdown-content h1,
            .markdown-content h2,
            .markdown-content h3,
            .markdown-content h4,
            .markdown-content h5,
            .markdown-content h6,
            .markdown-content strong {{
                color: #111111;
            }}

            .markdown-content pre {{
                white-space: pre-wrap;
                background: #f5f5f5;
                border-color: #dddddd;
            }}

            .markdown-content pre code {{
                color: #111111;
            }}
        }}

        @media (max-width: 900px) {{
            .toc {{
                width: min(88vw, 340px);
                transform: translateX(-105%);
                box-shadow: var(--shadow);
            }}

            .toc.open {{
                transform: translateX(0);
            }}

            .toc-close {{
                display: inline-flex;
            }}

            .overlay.visible {{
                display: block;
            }}

            .app-main {{
                margin-left: 0;
                padding-right: 18px;
                padding-left: 18px;
            }}

            .header-button-label {{
                display: none;
            }}

            .markdown-content {{
                padding: 24px 20px;
            }}
        }}

        @media (max-width: 540px) {{
            .app-header {{
                padding: 0 12px;
            }}

            .app-subtitle {{
                display: none;
            }}

            .hero {{
                padding: 23px 20px;
            }}

            .document-header {{
                padding: 16px;
            }}

            .document-number {{
                width: 38px;
                height: 38px;
            }}

            .markdown-content {{
                padding: 22px 16px;
            }}

            .markdown-content h1 {{
                font-size: 25px;
            }}

            .markdown-content h2 {{
                font-size: 21px;
            }}

            .back-to-top {{
                right: 16px;
                bottom: 16px;
            }}
        }}
    </style>
</head>

<body>
    <header class="app-header">
        <button
            class="menu-button"
            type="button"
            onclick="toggleNavigation()"
            aria-label="Navigation öffnen"
            title="Navigation"
        >
            ☰
        </button>

        <div class="app-title-area">
            <div class="app-title">
                {escaped_document_title}
            </div>

            <div class="app-subtitle">
                {escaped_source_directory}
            </div>
        </div>

        <div class="header-actions">
            <button
                class="header-button"
                type="button"
                onclick="window.print()"
                title="Dokument drucken"
            >
                <span>⎙</span>
                <span class="header-button-label">
                    Drucken
                </span>
            </button>
        </div>
    </header>

    {toc_html}

    <div
        class="overlay"
        id="overlay"
        onclick="closeNavigation()"
    ></div>

    <main class="app-main" id="top">
        <div class="content-container">
            <section class="hero">
                <div class="hero-label">
                    Markdown-Dokumentation
                </div>

                <h1>
                    {escaped_document_title}
                </h1>

                <p class="hero-description">
                    Zusammengeführte HTML-Dokumentation aus
                    {len(markdown_files)} Markdown-Dateien.
                </p>

                <div class="hero-meta">
                    <span class="meta-item">
                        {len(markdown_files)} Dokumente
                    </span>

                    <span class="meta-item">
                        Fluent Dark
                    </span>

                    <span class="meta-item">
                        Einzelne HTML-Datei
                    </span>
                </div>
            </section>

            {content_html}
        </div>
    </main>

    <a
        class="back-to-top"
        id="backToTop"
        href="#top"
        title="Nach oben"
        aria-label="Nach oben"
    >
        ↑
    </a>

    <script>
        const navigation = document.getElementById("toc");
        const overlay = document.getElementById("overlay");
        const backToTop = document.getElementById("backToTop");

        function toggleNavigation() {{
            navigation.classList.toggle("open");
            overlay.classList.toggle("visible");
        }}

        function closeNavigation() {{
            navigation.classList.remove("open");
            overlay.classList.remove("visible");
        }}

        document.querySelectorAll(".toc a").forEach((link) => {{
            link.addEventListener("click", () => {{
                if (window.innerWidth <= 900) {{
                    closeNavigation();
                }}
            }});
        }});

        window.addEventListener("scroll", () => {{
            if (window.scrollY > 500) {{
                backToTop.classList.add("visible");
            }} else {{
                backToTop.classList.remove("visible");
            }}
        }});

        document.addEventListener("keydown", (event) => {{
            if (event.key === "Escape") {{
                closeNavigation();
            }}
        }});
    </script>
</body>
</html>
"""


def convert_folder(
    source_directory: Path,
    output_file: Path,
    document_title: str,
    recursive: bool,
) -> tuple[int, Path]:
    """
    Führt den vollständigen Konvertierungsvorgang aus.
    """
    if not source_directory.exists():
        raise FileNotFoundError(
            "Der ausgewählte Quellordner existiert nicht."
        )

    if not source_directory.is_dir():
        raise NotADirectoryError(
            "Der ausgewählte Quellpfad ist kein Ordner."
        )

    markdown_files = collect_markdown_files(
        source_directory=source_directory,
        recursive=recursive,
        output_file=output_file,
    )

    if not markdown_files:
        raise RuntimeError(
            "Im ausgewählten Ordner wurden keine Markdown-Dateien gefunden."
        )

    output_file.parent.mkdir(
        parents=True,
        exist_ok=True,
    )

    complete_html = create_html_document(
        source_directory=source_directory,
        markdown_files=markdown_files,
        document_title=document_title,
    )

    output_file.write_text(
        complete_html,
        encoding="utf-8",
    )

    return len(markdown_files), output_file


def convert_folder_separately(
    source_directory: Path,
    output_directory: Path,
    recursive: bool,
) -> tuple[int, Path]:
    """
    Konvertiert jede Markdown-Datei in ein eigenes HTML-Dokument.

    Bei rekursiver Suche wird die Verzeichnisstruktur der Quelldateien
    im Ausgabeordner beibehalten. So werden gleichnamige Dateien aus
    unterschiedlichen Unterordnern nicht überschrieben.
    """
    if not source_directory.exists():
        raise FileNotFoundError(
            "Der ausgewählte Quellordner existiert nicht."
        )

    if not source_directory.is_dir():
        raise NotADirectoryError(
            "Der ausgewählte Quellpfad ist kein Ordner."
        )

    markdown_files = collect_markdown_files(
        source_directory=source_directory,
        recursive=recursive,
    )

    if not markdown_files:
        raise RuntimeError(
            "Im ausgewählten Ordner wurden keine Markdown-Dateien gefunden."
        )

    output_directory.mkdir(parents=True, exist_ok=True)

    for markdown_file in markdown_files:
        relative_path = markdown_file.relative_to(source_directory)
        output_file = (
            output_directory / relative_path
        ).with_suffix(".html")
        output_file.parent.mkdir(parents=True, exist_ok=True)

        document_title = markdown_file.stem.replace("_", " ")
        document_title = document_title.replace("-", " ")
        document_title = re.sub(r"\s+", " ", document_title).strip()

        complete_html = create_html_document(
            source_directory=source_directory,
            markdown_files=[markdown_file],
            document_title=document_title or markdown_file.stem,
        )
        output_file.write_text(complete_html, encoding="utf-8")

    return len(markdown_files), output_directory


# ============================================================
# Grafische Benutzeroberfläche
# ============================================================

class MarkdownConverterApp:
    """
    Minimale Fluent-Dark-Oberfläche für den Markdown-Konverter.
    """

    def __init__(self, root: Tk) -> None:
        self.root = root

        self.source_directory = StringVar()
        self.output_file = StringVar()
        self.output_mode = StringVar(value="combined")
        self.document_title = StringVar(
            value="Markdown-Dokumentation"
        )
        self.recursive = BooleanVar(value=False)
        self.open_after_conversion = BooleanVar(value=True)

        self.status_text = StringVar(
            value="Bereit"
        )

        self.create_window()
        self.create_styles()
        self.create_widgets()

    def create_window(self) -> None:
        self.root.title(
            f"{APP_NAME} – {APP_VERSION}"
        )

        self.root.geometry("720x580")
        self.root.minsize(650, 550)

        self.root.configure(
            background="#0c0c0c"
        )

        self.center_window()

    def center_window(self) -> None:
        self.root.update_idletasks()

        width = 720
        height = 580

        screen_width = self.root.winfo_screenwidth()
        screen_height = self.root.winfo_screenheight()

        x_position = max(
            0,
            (screen_width - width) // 2,
        )

        y_position = max(
            0,
            (screen_height - height) // 2,
        )

        self.root.geometry(
            f"{width}x{height}+{x_position}+{y_position}"
        )

    def create_styles(self) -> None:
        style = ttk.Style(self.root)

        try:
            style.theme_use("clam")
        except Exception:
            pass

        background = "#0c0c0c"
        surface = "#202020"
        surface_hover = "#2b2b2b"
        border = "#3a3a3a"
        text = "#ffffff"
        text_secondary = "#c8c8c8"
        accent = "#60cdff"
        accent_hover = "#86d8ff"

        style.configure(
            ".",
            background=background,
            foreground=text,
            font=("Segoe UI", 10),
            borderwidth=0,
        )

        style.configure(
            "App.TFrame",
            background=background,
        )

        style.configure(
            "Card.TFrame",
            background=surface,
            relief="flat",
        )

        style.configure(
            "Title.TLabel",
            background=background,
            foreground=text,
            font=("Segoe UI Semibold", 20),
        )

        style.configure(
            "Subtitle.TLabel",
            background=background,
            foreground="#9d9d9d",
            font=("Segoe UI", 9),
        )

        style.configure(
            "CardTitle.TLabel",
            background=surface,
            foreground=text,
            font=("Segoe UI Semibold", 11),
        )

        style.configure(
            "CardText.TLabel",
            background=surface,
            foreground=text_secondary,
            font=("Segoe UI", 9),
        )

        style.configure(
            "Status.TLabel",
            background=background,
            foreground="#9d9d9d",
            font=("Segoe UI", 9),
        )

        style.configure(
            "TEntry",
            fieldbackground="#181818",
            foreground=text,
            insertcolor=text,
            bordercolor=border,
            lightcolor=border,
            darkcolor=border,
            padding=(10, 8),
            relief="flat",
        )

        style.map(
            "TEntry",
            bordercolor=[
                ("focus", accent),
            ],
            lightcolor=[
                ("focus", accent),
            ],
            darkcolor=[
                ("focus", accent),
            ],
        )

        style.configure(
            "Fluent.TButton",
            background=surface_hover,
            foreground=text,
            padding=(14, 8),
            relief="flat",
            borderwidth=1,
            bordercolor=border,
            font=("Segoe UI", 9),
        )

        style.map(
            "Fluent.TButton",
            background=[
                ("active", "#363636"),
                ("pressed", "#404040"),
                ("disabled", "#1a1a1a"),
            ],
            foreground=[
                ("disabled", "#666666"),
            ],
        )

        style.configure(
            "Accent.TButton",
            background=accent,
            foreground="#000000",
            padding=(18, 10),
            relief="flat",
            borderwidth=0,
            font=("Segoe UI Semibold", 10),
        )

        style.map(
            "Accent.TButton",
            background=[
                ("active", accent_hover),
                ("pressed", "#4bb9e8"),
                ("disabled", "#345663"),
            ],
            foreground=[
                ("disabled", "#969696"),
            ],
        )

        style.configure(
            "Dark.TCheckbutton",
            background=surface,
            foreground=text_secondary,
            focuscolor=surface,
            font=("Segoe UI", 9),
        )

        style.configure(
            "Dark.TRadiobutton",
            background=surface,
            foreground=text_secondary,
            focuscolor=surface,
            font=("Segoe UI", 9),
        )

        style.map(
            "Dark.TRadiobutton",
            background=[("active", surface)],
            foreground=[("active", text)],
            indicatorcolor=[
                ("selected", accent),
                ("!selected", "#181818"),
            ],
        )

        style.map(
            "Dark.TCheckbutton",
            background=[
                ("active", surface),
            ],
            foreground=[
                ("active", text),
            ],
            indicatorcolor=[
                ("selected", accent),
                ("!selected", "#181818"),
            ],
        )

        style.configure(
            "Horizontal.TProgressbar",
            background=accent,
            troughcolor="#181818",
            borderwidth=0,
            lightcolor=accent,
            darkcolor=accent,
        )

    def create_widgets(self) -> None:
        main_frame = ttk.Frame(
            self.root,
            style="App.TFrame",
            padding=(28, 24),
        )

        main_frame.pack(
            fill="both",
            expand=True,
        )

        header_frame = ttk.Frame(
            main_frame,
            style="App.TFrame",
        )

        header_frame.pack(
            fill="x",
            pady=(0, 20),
        )

        ttk.Label(
            header_frame,
            text="Markdown zu HTML",
            style="Title.TLabel",
        ).pack(
            anchor="w",
        )

        ttk.Label(
            header_frame,
            text=(
                "Markdown-Dateien gemeinsam oder einzeln in "
                "Fluent-Dark-HTML umwandeln"
            ),
            style="Subtitle.TLabel",
        ).pack(
            anchor="w",
            pady=(4, 0),
        )

        card = ttk.Frame(
            main_frame,
            style="Card.TFrame",
            padding=(20, 18),
        )

        card.pack(
            fill="both",
            expand=True,
        )

        card.columnconfigure(
            0,
            weight=1,
        )

        self.create_input_row(
            parent=card,
            row=0,
            label_text="Markdown-Ordner",
            variable=self.source_directory,
            button_text="Auswählen",
            button_command=self.select_source_directory,
        )

        mode_frame = ttk.Frame(card, style="Card.TFrame")
        mode_frame.grid(
            row=1, column=0, columnspan=2, sticky="ew", pady=(0, 13)
        )

        ttk.Label(
            mode_frame,
            text="Ausgabemodus",
            style="CardTitle.TLabel",
        ).pack(anchor="w", pady=(0, 6))

        ttk.Radiobutton(
            mode_frame,
            text="Alle Markdown-Dateien in einer HTML-Datei",
            variable=self.output_mode,
            value="combined",
            command=self.output_mode_changed,
            style="Dark.TRadiobutton",
        ).pack(anchor="w")

        ttk.Radiobutton(
            mode_frame,
            text="Für jede Markdown-Datei eine eigene HTML-Datei",
            variable=self.output_mode,
            value="separate",
            command=self.output_mode_changed,
            style="Dark.TRadiobutton",
        ).pack(anchor="w", pady=(4, 0))

        self.create_input_row(
            parent=card,
            row=2,
            label_text="HTML-Ausgabe (Datei oder Ordner)",
            variable=self.output_file,
            button_text="Auswählen",
            button_command=self.select_output_file,
        )

        self.create_input_row(
            parent=card,
            row=3,
            label_text="Dokumenttitel",
            variable=self.document_title,
            button_text=None,
            button_command=None,
        )

        options_frame = ttk.Frame(
            card,
            style="Card.TFrame",
        )

        options_frame.grid(
            row=4,
            column=0,
            columnspan=2,
            sticky="ew",
            pady=(14, 6),
        )

        ttk.Checkbutton(
            options_frame,
            text="Unterordner einbeziehen",
            variable=self.recursive,
            command=self.update_file_count,
            style="Dark.TCheckbutton",
        ).pack(
            side="left",
        )

        ttk.Checkbutton(
            options_frame,
            text="HTML nach Konvertierung öffnen",
            variable=self.open_after_conversion,
            style="Dark.TCheckbutton",
        ).pack(
            side="left",
            padx=(24, 0),
        )

        separator = ttk.Separator(
            card,
            orient="horizontal",
        )

        separator.grid(
            row=5,
            column=0,
            columnspan=2,
            sticky="ew",
            pady=(14, 15),
        )

        information_label = ttk.Label(
            card,
            text=(
                "Alle CSS-Stile und Funktionen werden direkt "
                "in die HTML-Datei eingebettet."
            ),
            style="CardText.TLabel",
        )

        information_label.grid(
            row=6,
            column=0,
            columnspan=2,
            sticky="w",
        )

        self.progress_bar = ttk.Progressbar(
            card,
            mode="indeterminate",
            style="Horizontal.TProgressbar",
        )

        self.progress_bar.grid(
            row=7,
            column=0,
            columnspan=2,
            sticky="ew",
            pady=(18, 0),
        )

        footer_frame = ttk.Frame(
            main_frame,
            style="App.TFrame",
        )

        footer_frame.pack(
            fill="x",
            pady=(18, 0),
        )

        ttk.Label(
            footer_frame,
            textvariable=self.status_text,
            style="Status.TLabel",
        ).pack(
            side="left",
        )

        self.convert_button = ttk.Button(
            footer_frame,
            text="Konvertieren",
            style="Accent.TButton",
            command=self.start_conversion,
        )

        self.convert_button.pack(
            side="right",
        )

        ttk.Button(
            footer_frame,
            text="Ausgabeordner öffnen",
            style="Fluent.TButton",
            command=self.open_output_directory,
        ).pack(
            side="right",
            padx=(0, 10),
        )

    def create_input_row(
        self,
        parent: ttk.Frame,
        row: int,
        label_text: str,
        variable: StringVar,
        button_text: str | None,
        button_command,
    ) -> None:
        row_frame = ttk.Frame(
            parent,
            style="Card.TFrame",
        )

        row_frame.grid(
            row=row,
            column=0,
            columnspan=2,
            sticky="ew",
            pady=(0, 13),
        )

        row_frame.columnconfigure(
            0,
            weight=1,
        )

        ttk.Label(
            row_frame,
            text=label_text,
            style="CardTitle.TLabel",
        ).grid(
            row=0,
            column=0,
            columnspan=2,
            sticky="w",
            pady=(0, 6),
        )

        entry = ttk.Entry(
            row_frame,
            textvariable=variable,
        )

        entry.grid(
            row=1,
            column=0,
            sticky="ew",
        )

        if button_text and button_command:
            ttk.Button(
                row_frame,
                text=button_text,
                style="Fluent.TButton",
                command=button_command,
            ).grid(
                row=1,
                column=1,
                padx=(10, 0),
            )

    def select_source_directory(self) -> None:
        selected_directory = filedialog.askdirectory(
            title="Markdown-Ordner auswählen",
        )

        if not selected_directory:
            return

        source_path = Path(selected_directory)
        self.source_directory.set(str(source_path))

        if not self.output_file.get().strip():
            if self.output_mode.get() == "separate":
                default_output = source_path / "html"
            else:
                default_output = (
                    source_path
                    / f"{source_path.name}_Dokumentation.html"
                )

            self.output_file.set(
                str(default_output)
            )

        if (
            self.document_title.get().strip()
            == "Markdown-Dokumentation"
        ):
            self.document_title.set(
                f"{source_path.name} – Dokumentation"
            )

        self.update_file_count()

    def select_output_file(self) -> None:
        if self.output_mode.get() == "separate":
            selected_directory = filedialog.askdirectory(
                title="HTML-Ausgabeordner auswählen",
                initialdir=(
                    self.output_file.get().strip()
                    or self.source_directory.get().strip()
                    or None
                ),
            )
            if selected_directory:
                self.output_file.set(selected_directory)
            return

        initial_directory = None
        initial_file = "Dokumentation.html"

        current_output = self.output_file.get().strip()

        if current_output:
            current_path = Path(current_output)
            initial_directory = str(current_path.parent)
            initial_file = current_path.name
        elif self.source_directory.get().strip():
            initial_directory = self.source_directory.get().strip()

        selected_file = filedialog.asksaveasfilename(
            title="HTML-Datei speichern",
            initialdir=initial_directory,
            initialfile=initial_file,
            defaultextension=".html",
            filetypes=[
                ("HTML-Dateien", "*.html"),
                ("Alle Dateien", "*.*"),
            ],
        )

        if selected_file:
            self.output_file.set(selected_file)

    def output_mode_changed(self) -> None:
        source_text = self.source_directory.get().strip()

        if not source_text:
            self.output_file.set("")
            return

        source_path = Path(source_text)
        if self.output_mode.get() == "separate":
            self.output_file.set(str(source_path / "html"))
        else:
            self.output_file.set(
                str(source_path / f"{source_path.name}_Dokumentation.html")
            )

    def update_file_count(self) -> None:
        source_text = self.source_directory.get().strip()

        if not source_text:
            self.status_text.set("Bereit")
            return

        source_path = Path(source_text)

        if not source_path.is_dir():
            self.status_text.set(
                "Der ausgewählte Ordner ist ungültig."
            )
            return

        files = collect_markdown_files(
            source_directory=source_path,
            recursive=self.recursive.get(),
        )

        self.status_text.set(
            f"{len(files)} Markdown-Dateien gefunden"
        )

    def validate_inputs(
        self,
    ) -> tuple[Path, Path, str] | None:
        source_text = self.source_directory.get().strip()
        output_text = self.output_file.get().strip()
        title_text = self.document_title.get().strip()

        if not source_text:
            messagebox.showwarning(
                APP_NAME,
                "Bitte wähle zuerst einen Markdown-Ordner aus.",
            )
            return None

        source_path = Path(source_text)

        if not source_path.exists() or not source_path.is_dir():
            messagebox.showerror(
                APP_NAME,
                "Der ausgewählte Markdown-Ordner ist ungültig.",
            )
            return None

        if not output_text:
            messagebox.showwarning(
                APP_NAME,
                "Bitte gib eine HTML-Ausgabedatei an.",
            )
            return None

        output_path = Path(output_text)

        if (
            self.output_mode.get() == "combined"
            and output_path.suffix.lower() not in {".html", ".htm"}
        ):
            output_path = output_path.with_suffix(".html")
            self.output_file.set(str(output_path))

        if not title_text:
            title_text = "Markdown-Dokumentation"
            self.document_title.set(title_text)

        return source_path, output_path, title_text

    def start_conversion(self) -> None:
        if markdown is None or pygments is None:
            missing_package = "markdown" if markdown is None else "pygments"
            messagebox.showerror(
                APP_NAME,
                f"Das Python-Paket '{missing_package}' fehlt.\n\n"
                "Installiere es mit:\n"
                f"pip install {missing_package}",
            )
            return

        validated_inputs = self.validate_inputs()

        if validated_inputs is None:
            return

        source_path, output_path, title_text = validated_inputs

        self.convert_button.configure(
            state="disabled",
        )

        self.progress_bar.start(10)

        self.status_text.set(
            "Markdown-Dateien werden konvertiert …"
        )

        worker = threading.Thread(
            target=self.run_conversion,
            args=(
                source_path,
                output_path,
                title_text,
                self.recursive.get(),
                self.output_mode.get() == "separate",
            ),
            daemon=True,
        )

        worker.start()

    def run_conversion(
        self,
        source_path: Path,
        output_path: Path,
        title_text: str,
        recursive: bool,
        separate: bool,
    ) -> None:
        try:
            if separate:
                file_count, created_file = convert_folder_separately(
                    source_directory=source_path,
                    output_directory=output_path,
                    recursive=recursive,
                )
            else:
                file_count, created_file = convert_folder(
                    source_directory=source_path,
                    output_file=output_path,
                    document_title=title_text,
                    recursive=recursive,
                )

            self.root.after(
                0,
                lambda: self.conversion_finished(
                    file_count,
                    created_file,
                ),
            )

        except Exception as error:
            self.root.after(
                0,
                lambda error=error: self.conversion_failed(error),
            )

    def conversion_finished(
        self,
        file_count: int,
        created_file: Path,
    ) -> None:
        self.progress_bar.stop()

        self.convert_button.configure(
            state="normal",
        )

        self.status_text.set(
            f"{file_count} Dateien erfolgreich konvertiert"
        )

        messagebox.showinfo(
            APP_NAME,
            f"Die Konvertierung wurde erfolgreich abgeschlossen.\n\n"
            f"Markdown-Dateien: {file_count}\n"
            f"Ausgabe:\n{created_file}",
        )

        if self.open_after_conversion.get():
            if created_file.is_dir():
                open_path_in_file_manager(created_file)
            else:
                webbrowser.open(created_file.resolve().as_uri())

    def conversion_failed(
        self,
        error: Exception,
    ) -> None:
        self.progress_bar.stop()

        self.convert_button.configure(
            state="normal",
        )

        self.status_text.set(
            "Konvertierung fehlgeschlagen"
        )

        messagebox.showerror(
            APP_NAME,
            f"Die Konvertierung konnte nicht abgeschlossen werden.\n\n"
            f"{error}",
        )

    def open_output_directory(self) -> None:
        output_text = self.output_file.get().strip()

        if output_text:
            output_path = Path(output_text).expanduser()
            directory = (
                output_path
                if self.output_mode.get() == "separate"
                else output_path.parent
            )
        elif self.source_directory.get().strip():
            directory = Path(
                self.source_directory.get().strip()
            )
        else:
            messagebox.showwarning(
                APP_NAME,
                "Es wurde noch kein Ausgabeordner ausgewählt.",
            )
            return

        if not directory.exists():
            messagebox.showwarning(
                APP_NAME,
                "Der Ausgabeordner existiert noch nicht.",
            )
            return

        try:
            open_path_in_file_manager(directory)
        except Exception as error:
            messagebox.showerror(
                APP_NAME,
                str(error),
            )


# ============================================================
# Programmeinstieg
# ============================================================

def main() -> None:
    root = Tk()
    MarkdownConverterApp(root)
    root.mainloop()


if __name__ == "__main__":
    main()
