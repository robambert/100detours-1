"""
100detours Flask server factory. 
"""

from flask import Flask
from .algo_request import request_algo

def create_app(run_algo=False):
    app = Flask(__name__, instance_relative_config=True, static_folder=None)

    # ---- App configuration ----

    # Default configuration
    app.config.from_pyfile("default.cfg")
    if run_algo:
        app.config.from_pyfile("run_algo.cfg")

    # Overriden conf from conf file
    app.config.from_envvar("FLASK_CONF", silent=True)

    # Overrides conf from env

    from .config_utils import from_env
    from_env(app)

    # ---- Extensions instanciation ----

    from .extensions import jwt, db, cors
    db.init_app(app)
    jwt.init_app(app)
    cors.init_app(app)

    # ---- Custom errors handler registration ----

    from . import errors
    errors.init_app(app)

    # ---- Database reset ----

    # Algo server should never reset the database
    if not run_algo:
        from .db_utils import reinit_db
        if app.config.get("MONGODB_REINIT_DB", False):
            reinit_db()

    # ---- Routes registration ----

    # If algo server only register one route /run_algo
    if run_algo:
        from datetime import datetime

        @app.route("/run_algo", methods=["POST", "GET"])
        def run_algo_imp():
            from flask import request
            print(request.json)
            date = request.json.get("date", "2020-01-06")
            print("Date: ", date)
            request_algo(datetime.fromisoformat(date), app.config["ALGO_URI"])
            return {}, 200

    else:
        from .apis import api
        api.init_app(app)

    # ---- CLI commands ----

    from . import commands
    commands.init_app(app)

    return app
