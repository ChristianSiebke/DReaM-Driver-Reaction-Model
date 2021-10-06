################################################################################
# Copyright (c) 2020-2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

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
