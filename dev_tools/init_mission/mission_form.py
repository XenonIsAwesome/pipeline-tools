from textual import on
from textual.app import App, ComposeResult
from textual.containers import Vertical, Center, Horizontal
from textual.widgets import Input, Button, Label, Sparkline, Checkbox, Switch


class MissionForm(App):
    DEFAULT_NS_PLACEHOLDER = "Mission Namespace"

    CSS = """
    Screen {
        align: center middle;
    }

    #form {
        width: 60;
        padding: 2;
        align: center middle;
    }
    
    Sparkline {
        margin-bottom: 2;
    }
    
    #toggle-row {
        width: 30;
    }

    Input {
        width: 100%;
        margin-bottom: 2;
        align: center middle;
    }

    Button {
        width: auto;
        align: center middle;
        margin-top: 1;
    }
    """

    def compose(self) -> ComposeResult:
        yield Vertical(
            Label("Initialize a new mission", id="title"),
            Sparkline(),
            Label("Mission Name"),
            Input(placeholder="Mission Name", id="mission_name"),
            Label(MissionForm.DEFAULT_NS_PLACEHOLDER),
            Input(placeholder=MissionForm.DEFAULT_NS_PLACEHOLDER, id="mission_ns"),
            Label("Enable this mission by default?"),
            Horizontal(
                Switch(id="default_check"),
                id="toggle-row",
            ),
            Center(
                Button("Initialize Mission", id="submit")
            ),
            id="form",
        )

    @on(Input.Changed, "#mission_name")
    def on_name_change(self, event: Input.Changed):
        self.query_one("#mission_ns", Input).placeholder = self.name_to_namespace(event.value) or MissionForm.DEFAULT_NS_PLACEHOLDER

    @staticmethod
    def format_name(name: str):
        mapping = {
            ' ': '_',
            '-': '_',
        }

        for k, v in mapping.items():
            name = name.replace(k, v)

        return name


    @staticmethod
    def name_to_namespace(name: str) -> str:
        return MissionForm.format_name(name.lower())


    @on(Button.Pressed, "#submit")
    def submit(self):
        name = self.query_one("#mission_name", Input).value
        ns = self.query_one("#mission_ns", Input).value
        default = self.query_one("#default_check", Switch).value

        self.exit({
            "mission_name": name,
            "mission_namespace": self.name_to_namespace(ns) or self.name_to_namespace(name),
            "default": default
        })

if __name__ == '__main__':
    print(MissionForm().run())