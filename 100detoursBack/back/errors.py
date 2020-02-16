import mongoengine
import flask

MONGO_URI_VALUE_ERROR = r"""MONGO_URI must match one of the following formats:
    - {host}:{port}
    - {host}:{port}/{db_name}
    - mongodb://{port}
    - mongodb://{port}/{db_name}"""


def init_app(app: flask.Flask):
    app.register_error_handler(mongoengine.ValidationError, _handle_validation_errors)


def _handle_validation_errors(err):
    """Mongoengine ValidationErrors as 400."""
    print("Using custom ValidationError handler !")
    return {"message": err.message, "errors": err.errors}, getattr(err, "code", 400)
