################################################################################
# Copyright (c) 2020-2021 in-tech GmbH
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
################################################################################

from .common import Report
from distutils import dir_util
from datetime import datetime
from enum import Enum
from math import isnan

class State(Enum):
     EXECUTED = "▶"
     PASSED = "🟢"
     FAILED = "❌"
     NOT_EXECUTED = "⏸️"
     UNAVAILABLE = "💀"

class HtmlReport(Report):
    @staticmethod
    def table_header(line):
        return r'<tr>' + ''.join(f'<th>{item}</th>' for item in line) + r'</tr>'

    @staticmethod
    def table_line(line):
        return r'<tr>' + ''.join(f'<td>{item}</td>' for item in line) + r'</tr>'

    @staticmethod
    def status(state: State):
        return f'<div>{state.value} {state.name.replace("_", " ")}</div>'

    @staticmethod
    def success_rates(rate):
        if rate is None:
            return 'N/A'
        
        (expected, actual, _) = rate
        
        color = "green" if actual >= expected else "red"
        primary = actual if actual >= expected else expected
        secondary = expected if actual >= expected else actual
        
        return f'<span>🏹 {expected*100:.0f}&#8201;&#37; 🎯 {actual*100:.0f}&#8201;&#37;</span>' \
               f'<div class=black      style="width:100%;"/>' \
               f'<div class={color}gap style="width:{primary*100}%;"/>' \
               f'<div class=green      style="width:{secondary*100}%;"/>'

    def add(self, line):
        self.generated += f'{line}\n'

    def __str__(self):
        self.add(r'<meta charset="UTF-8">')
        self.add(r'<html>')
        self.add(r'<head>')
        self.add(r'<title>pyOpenPASS EndToEnd Test Framework</title>')
        self.add(r'<link rel="stylesheet" type="text/css" href="report.css"/ >')
        self.add(r'</head>')
        self.add(r'<body>')
        self.add(r'<h1>pyOpenPASS EndToEnd Test Framework</h1>')
        self.add(r'<h2>Summary</h2>')
        self.add(f'<div class="generated">{datetime.now().strftime("%d.%m.%Y, %H:%M:%S")}</div>')
        self.add(r'<table class="results">')
        self.add(r'<thead>')
        self.add(self.table_header(("Config", "Scope", "Execution Status", "Test Result", "Duration", "Success Rate")))
        self.add(r'</thead>')
        self.add(r'</tbody>')

        for config_dir in self.all_config_dirs:
            for scope in self.scopes:
                try:
                    test_results = scope.test_results[config_dir.name]
                    test_rates = scope.test_rates[config_dir.name]
                    config_dir.handled = True
                    for (test_result, test_rate) in zip(test_results, test_rates):
                        self.add(self.table_line((
                            config_dir.name, scope.name, 
                            self.status(State.EXECUTED),
                            self.status(State.PASSED) if test_result.status else self.status(State.FAILED),
                            'unknown' if isnan(test_result.duration) else f'{test_result.duration:.2f}&#8201;s', 
                            self.success_rates(test_rate))))
                except KeyError:
                    pass

            if config_dir.handled is False:
                self.add(self.table_line((config_dir.name, "NONE", self.status(State.NOT_EXECUTED), "", "", "")))

        for missing_config in self.missing_configs:
            self.add(self.table_line((missing_config[0], missing_config[1], self.status(State.UNAVAILABLE), "", "", "")))

        self.add(r'</tbody>')
        self.add(r'</table>')
        self.add(r'</body></html>')

        return self.generated

    def generate(self):
        super().generate()
        dir_util.copy_tree('reports/resources/html', f'{self.output_path}')
        with open(f'{self.output_path}/report.html', 'w', encoding="utf-8") as report_file:
            report_file.write(self.generated)


