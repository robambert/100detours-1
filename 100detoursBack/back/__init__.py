from flask import Flask


def create_app(test_config=None):
    app = Flask(__name__, instance_relative_config=True, static_folder=None)

    # ---- App configuration ----

    # Default configuration
    app.config.from_pyfile("default.cfg")

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

    from .db_utils import reinit_db
    if app.config.get("MONGODB_REINIT_DB", False):
        reinit_db()

    # ---- Blueprint registration ----

    from .apis import api
    api.init_app(app)

    # ---- CLI commands ----

    from . import commands
    commands.init_app(app)

    # from datetime import datetime
    # commands.request_algo(datetime(2020, 1, 25), "localhost:4242")

    return app
