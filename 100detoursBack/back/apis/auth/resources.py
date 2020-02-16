from flask_restplus import Namespace, Resource, abort
from flask_jwt_extended import (create_access_token, create_refresh_token, jwt_required,
                                jwt_refresh_token_required, get_raw_jwt, get_current_user)
from flask_accepts import accepts, responds
from marshmallow import Schema, fields

from .utils import auth_required
from .models import TokenModel
from ..users.models import UserModel, UserSchema
from ..nurses.models import NurseModel

ns = Namespace("auth", description="Login and authorization related operation")


@ns.route("/login")
class Login(Resource):
    schema = UserSchema(only=("username", "password"))

    @accepts(
        "Credentials",
        schema=Schema.from_dict(dict(username=fields.Str(default="admin"), password=fields.Str(default="100detours"))),
        api=ns)
    @responds(
        "Tokens",
        {"name": "message", "type": str},
        {"name": "access_token", "type": str},
        {"name": "refresh_token", "type": str},
        {"name": "usertype", "type": int},
        {"name": "nurse_rid", "type": int},
        {"name": "user_rid", "type": int},
        api=ns
    )
    @ns.doc(security=[])
    def post(self):
        """Login route."""
        data = self.schema.load_request()
        username, password = data["username"], data["password"]

        user = UserModel.objects(username=username).first()
        if user is None:
            abort(401, 'User {} doesn\'t exist'.format(username))

        if user.verify_password(password):
            access_token = create_access_token(identity=user)
            refresh_token = create_refresh_token(identity=user)
            TokenModel.from_token(access_token).save()
            TokenModel.from_token(refresh_token).save()
            dct = {
                'message': 'Logged in as {}'.format(username),
                'access_token': access_token,
                'refresh_token': refresh_token,
                'usertype': user.usertype,
                'user_rid': user.rid,
            }
            if user.usertype == 1:
                dct["nurse_rid"] = NurseModel.objects(user=user).get().rid
            return dct
        else:
            abort(401, 'Wrong credentials')


@ns.route("/revoke-access")
class RevokeAccess(Resource):

    @auth_required
    @responds(
        "Status message",
        {"name": "message", "type": str},
        api=ns
    )
    def post(self):
        """Revoke access token, refresh token status is unchanged."""
        jti = get_raw_jwt()['jti']
        token = TokenModel.objects(jti=jti).first()
        token.revoked = True
        token.save()
        return {'message': 'Access token has been revoked'}


@ns.route("/revoke-refresh")
class RevokeRefresh(Resource):
    @jwt_refresh_token_required
    @responds(
        "Status message",
        {"name": "message", "type": str},
        api=ns
    )
    def post(self):
        """[Refresh token required] Revoke refresh token, access token status is unchanged."""
        jti = get_raw_jwt()['jti']
        token = TokenModel.objects(jti=jti).first()
        token.revoked = True
        token.save()
        return {'message': 'Refresh token has been revoked'}


@ns.route("/refresh")
class TokenRefresh(Resource):
    @jwt_refresh_token_required
    @responds(
        "Status message",
        {"name": "message", "type": str},
        api=ns
    )
    def post(self):
        """[Refresh token required] Grant a new access token if refresh token is valid."""
        user = get_current_user()
        access_token = create_access_token(identity=user)
        TokenModel.from_token(access_token).save()
        return {'access_token': access_token}
