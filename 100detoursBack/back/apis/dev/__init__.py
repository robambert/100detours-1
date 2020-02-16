from flask_restplus import Namespace, Resource
from flask_jwt_extended import get_raw_jwt, get_jwt_claims, jwt_optional


ns = Namespace("dev")


@ns.route("/hello")
class Hello(Resource):

    def get(self):
        return {"messagge": "Hello World!"}


@ns.route("/token")
class Token(Resource):

    @jwt_optional
    def get(self):
        return get_raw_jwt()


@ns.route("/claims")
class Claims(Resource):

    @jwt_optional
    def get(self):
        return get_jwt_claims()
