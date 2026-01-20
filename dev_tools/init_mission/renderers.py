import os
from pathlib import Path

from mako.template import Template


def render_file(template_file: Path, target_file: Path, **values):
    tmpl = Template(filename=str(template_file))
    rendered = tmpl.render(**values)

    target_file.parent.mkdir(parents=True, exist_ok=True)
    target_file.write_text(rendered)


def render_snippet(template_file: Path, target_file: Path, **values):
    tmpl = Template(filename=str(template_file))
    rendered_lines = tmpl.render(**values).split("\n")

    lines = []
    for line in target_file.read_text().split("\n"):
        if "END AUTO-GENERATED SECTION" in line:
            lines.extend(rendered_lines)

        lines.append(line)

    target_file.write_text("\n".join(lines))
