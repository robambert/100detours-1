from flask import request
from flask_restplus import Resource, Namespace
from flask_accepts import accepts
from marshmallow import Schema, fields
import requests as reqs

from ..auth.utils import manager_required

ns = Namespace("algo", "Launch algo")


@ns.route("/")
class Algo(Resource):

    @accepts(
        "Date",
        schema=Schema.from_dict(dict(date=fields.Date())),
        api=ns)
    @manager_required
    def post(self):
        """Request the algorithm to run with the current data."""
        try:
            reqs.post(
                "http://127.0.0.1:5010/run_algo",
                json=request.json,
                timeout=0.5)
        except Exception as e:
            print(e)
        return {}, 200
