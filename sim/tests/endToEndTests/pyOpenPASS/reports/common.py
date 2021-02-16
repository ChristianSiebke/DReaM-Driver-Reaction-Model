from pathlib import Path


class Report:
    def __init__(self, output_path, all_config_dirs):
        self.output_path = output_path
        self.all_config_dirs = all_config_dirs
        self.missing_configs = []
        self.scopes = []
        self.generated = ""

    def add_missing_config(self, config, scope):
        self.missing_configs.append((config, scope))

    def add_scope(self, scope):
        self.scopes.append(scope)

    def generate(self):
        self.generated = str(self)
        Path(self.output_path).mkdir(parents=True, exist_ok=True)

