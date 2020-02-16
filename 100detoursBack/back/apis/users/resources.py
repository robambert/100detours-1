from flask import request
from flask_restplus import Resource, Namespace, abort
from flask_accepts import responds, accepts
from flask_jwt_extended import jwt_required
from marshmallow import Schema, fields

from .models import UserModel, UserSchema
from ..auth.utils import manager_or_owner_required, manager_required

ns = Namespace("users", description="Users related operations.")


@ns.route("/")
class Users(Resource):
    """Ressource to manage users"""

    @responds(schema=UserSchema(many=True), api=ns)
    @manager_required
    def get(self):
        """Get all users."""
        return UserModel.objects

    @accepts(schema=UserSchema(), api=ns)
    @responds(schema=UserSchema(), api=ns, status_code=201)
    @manager_required
    def post(self):
        """Add a user."""
        password = request.parsed_args.pop("password")
        user = UserModel(**request.parsed_args)
        user.set_password(password)
        user.save()
        return user


@ns.route("/<int:rid>")
@ns.param("rid", "The user rid (index start from 1)", "query")
class User(Resource):
    """Ressource to manage single user accessed by its rid."""

    @responds(schema=UserSchema(), api=ns)
    @manager_or_owner_required()
    def get(self, rid):
        """Get a single user."""
        print(rid)
        return UserModel.with_rid(rid)

    @accepts(schema=UserSchema(partial=True), api=ns)
    @responds(schema=UserSchema(), api=ns, status_code=201)
    @manager_required
    def put(self, rid):
        """Update a user."""
        user = UserModel.with_rid(rid)
        user.modify(**request.parsed_args)
        return user

    @manager_required
    def delete(self, rid):
        """Delete a user."""
        UserModel.with_rid(rid).delete()
        return {}, 204


@ns.route("/<int:rid>/password")
@ns.doc(params={"rid": "The user rid (index start from 1)"})
class UserPassword(Resource):

    @accepts(
        model_name="PasswordSchema",
        schema=Schema.from_dict(dict(
            current_password=fields.Str(),
            new_password=fields.Str()
        )),
        api=ns)
    @manager_or_owner_required()
    def post(self, rid):
        """Update a user's password."""
        cur_password = request.parsed_obj["current_password"]
        new_password = request.parsed_obj["new_password"]
        user = UserModel.with_rid(rid)
        if not user.verify_password(cur_password):
            abort(401, "Wrong password.")
        user.set_password(new_password)
        user.save()
        return {}, 204
