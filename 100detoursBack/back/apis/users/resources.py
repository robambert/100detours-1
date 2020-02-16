from flask import request
from flask_restplus import Resource, Namespace
from flask_accepts import responds, accepts

from .models import UserModel, UserSchema
from ..auth.utils import manager_or_owner_required, manager_required

ns = Namespace("users", description="Users related operations.")


@ns.route("/")
class Users(Resource):
    """Ressource to manage users"""

    @responds(schema=UserSchema(many=True), api=ns)
    @manager_required
    def get(self):
        from flask import request
        print(request.headers)
        """Get all users."""
        return UserModel.objects

    @accepts(schema=UserSchema(), api=ns)
    @responds(schema=UserSchema(), api=ns, status_code=201)
    @manager_required
    def post(self):
        """Add a user."""
        password = request.parsed_obj.pop("password")
        user = UserModel(**request.parsed_obj)
        user.set_password(password)
        user.save()
        return user


@ns.route("/<int:rid>")
@ns.doc(params={"rid": "The user rid (index start from 1)"})
class User(Resource):
    """Ressource to manage single user accessed by its rid."""

    @responds(schema=UserSchema(), api=ns)
    @manager_or_owner_required
    def get(self, rid):
        """Get a single user."""
        user = UserModel.with_rid(rid)
        print(UserSchema().dumps(user))
        return user

    @accepts(schema=UserSchema(partial=True), api=ns)
    @responds(schema=UserSchema(), api=ns, status_code=201)
    @manager_required
    def put(self, rid):
        """Update a user."""
        user = UserModel.with_rid(rid)
        user.modify(**request.parsed_obj)
        return user

    @manager_required
    def delete(self, rid):
        """Delete a user."""
        UserModel.with_rid(rid).delete()
        return {}, 204


@ns.route("/<int:rid>/password")
@ns.doc(params={"rid": "The user rid (index start from 1)"})
class UserPassword(Resource):

    @accepts("PasswordSchema", schema=UserSchema(only=["password"]), api=ns)
    @manager_or_owner_required
    def put(self, rid):
        """Update a user's password."""
        password = request.parsed_obj["password"]
        user = UserModel.with_rid(rid)
        user.set_password(password)
        user.save()
        return {}, 204
