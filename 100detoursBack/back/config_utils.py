import os
from collections import OrderedDict

from flask import Flask

from .errors import MONGO_URI_VALUE_ERROR


def parse_bool(str_val: str):
    tmp = str_val.lower().strip()
    if tmp in {"true", "1"}:
        return True
    elif tmp in {"false", "0"}:
        return False
    else:
        raise SyntaxError(f"Cannot parse {tmp} into a bool!")


def from_env(app: Flask):
    env_map = OrderedDict([
        ("SERVER_NAME", "SERVER_NAME"),
        ("FLASK_HOST", "FLASK_HOST"),
        ("FLASK_PORT", "FLASK_PORT"),
        ("FLASK_DEBUG", "DEBUG"),
        ("FLASK_ENV", "ENV"),

        ("MONGODB_HOST", "MONGODB_HOST"),
        ("MONGODB_PORT", "MONGODB_PORT"),
        ("MONGODB_DB", "MONGODB_DB"),
        ("MONGODB_CONNECT", "MONGODB_CONNECT"),

        ("JWT_BLACKLIST_ENABLED", "JWT_BLACKLIST_ENABLED"),

        ("ALGO_URI", "ALGO_URI")
    ])
    for env, conf_key in env_map.items():
        val = os.environ.get(env)
        if val:
            # if env == "FLASK_URI" or "ALGO_URI" or "FLASK_HOST":
            #     val = val.replace("localhost", "127.0.0.1")
            if conf_key in {"JWT_BLACKLIST_ENABLED", "MONGODB_CONNECT"}:
                val = parse_bool(val)

            app.config[conf_key] = val

    mongo_uri = os.environ.get("MONGODB_URI")
    if mongo_uri:
        prefix = "mongodb://"
        tmp = mongo_uri
        if tmp.startswith(prefix):
            tmp = tmp[len(prefix):]

        db_name = None
        if '/' in tmp:
            tmp, db_name = tmp.split("/")
        try:
            mongo_host, mongo_port = tmp.split(":")
            app.config["MONGODB_HOST"] = mongo_host
            app.config["MONGODB_PORT"] = int(mongo_port)
            if db_name:
                app.config["MONGODB_DB"] = db_name
        except ValueError:
            raise ValueError(f"Invalid value {mongo_uri}.\n" + MONGO_URI_VALUE_ERROR)
