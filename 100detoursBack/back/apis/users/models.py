from mongoengine import SequenceField, IntField, StringField
from passlib.hash import pbkdf2_sha256 as sha256
from marshmallow import fields, validate

from ..common.models import RidDocument
from ..common.marshmallow_utils import RidSchema


USERTYPES = {
    0: "manager",
    1: "nurse",
}


def _get_usertypes_desc():
    desc = ""
    for k, v in USERTYPES.items():
        desc += f"\t{k}: {v}"
    return desc


class UserModel(RidDocument):
    """User mongoengine model. Defines methods to work with password hash.

    Notes
    -----
        Only set `password` with an hash.
    """
    rid = SequenceField(unique=True)  # Overriding to use own counter rather than the one commun to all RidDocument childs.
    username = StringField(min_lenght=1, max_length=64, required=True, unique=True)
    usertype = IntField(choices={0, 1}, required=True)

    password = StringField(min_lenght=1, required=True)

    @staticmethod
    def hash_password(password):
        """Hash and returns password."""
        return sha256.hash(password)

    def set_password(self, password):
        """Hash and set password."""
        self.password = self.hash_password(password)

    def verify_password(self, password):
        """Hash password and compares it to stored value."""
        return sha256.verify(password, self.password)


class UserSchema(RidSchema):
    __model__ = UserModel
    username = fields.Str(validate=validate.Length(1, 64), required=True, example="admin")
    usertype = fields.Int(strict=True, required=True, validate=validate.OneOf(set(USERTYPES)), description=f"Table:\n{_get_usertypes_desc()}")
    password = fields.Str(required=True, load_only=True)
