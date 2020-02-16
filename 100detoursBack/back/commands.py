"""CLI commands to run the algorithm.
"""

from datetime import timedelta
import click
from flask.cli import with_appcontext
from flask import current_app

from .algo_request import request_algo


@click.command("run-algo")
@click.argument("date", type=click.DateTime(), required=False)
@with_appcontext
def run_algo(date):
    request_algo(date, current_app.config["ALGO_URI"])


def init_app(app):
    app.cli.add_command(run_algo)
