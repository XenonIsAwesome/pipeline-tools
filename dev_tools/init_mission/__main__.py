import json
import os
import subprocess
from pathlib import Path

from dev_tools.init_mission.mission_form import MissionForm
from dev_tools.init_mission.renderers import render_file, render_snippet

SCRIPT_PARENT_DIR: Path = Path(__file__).resolve().parent

TEMPLATES_DIR_PATH: Path = SCRIPT_PARENT_DIR / "templates"
FILE_TEMPLATES_DIR_PATH: Path = TEMPLATES_DIR_PATH / "files"
SNIPPET_TEMPLATES_DIR_PATH: Path = TEMPLATES_DIR_PATH / "snippets"

TEMPLATES_JSON_FILEPATH: Path = SCRIPT_PARENT_DIR / "templates.json"
TEMPLATES_JSON = json.loads(TEMPLATES_JSON_FILEPATH.read_text())

PROJECT_DIR_PATH = Path.cwd()
MISSIONS_DIR_PATH = PROJECT_DIR_PATH / "missions"


def get_username():
    uname = ""
    try:
        uname = subprocess.check_output(["git", "config", "user.name"], text=True)
    except subprocess.CalledProcessError:
        uname = os.getenv("USER")

    return uname.strip()


def main():
    results = MissionForm().run()

    if results is None:
        return

    mission_dir_path = MISSIONS_DIR_PATH / MissionForm.format_name(results["mission_name"]).lower()
    mission_dir_path.mkdir(parents=True, exist_ok=True)

    for template_filepath in FILE_TEMPLATES_DIR_PATH.rglob("*.mako"):
        output_filename = os.path.splitext(template_filepath.name)[0]
        output_filepath = mission_dir_path / template_filepath.relative_to(FILE_TEMPLATES_DIR_PATH).parent / output_filename

        render_file(template_filepath, output_filepath, **results, username=get_username())

    for snippet_data in TEMPLATES_JSON:
        template_filepath = SNIPPET_TEMPLATES_DIR_PATH / snippet_data["template"]
        target_filepath = PROJECT_DIR_PATH / snippet_data["target"]

        render_snippet(template_filepath, target_filepath, **results, username=get_username())


if __name__ == "__main__":
    main()
