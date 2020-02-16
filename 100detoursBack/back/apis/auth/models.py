from datetime import datetime
import numbers

from flask_jwt_extended import decode_token
from mongoengine import Document, StringField, BooleanField, DateTimeField


class TokenModel(Document):
    jti = StringField(required=True, unique=True)
    type = StringField(required=True, choices=("access", "refresh"))
    fresh = BooleanField(required=True)
    _expires = DateTimeField(required=True)
    revoked = BooleanField(db_name="revoked", default=False)

    @property
    def expires(self):
        return self._expires

    @expires.setter
    def expires(self, val):
        if isinstance(val, datetime):
            self._expires = val
        elif isinstance(val, numbers.Integral):
            self._expires = datetime.fromtimestamp(val)
        else:
            raise TypeError(f"TokenModel.expires must be a datetime.datetime instance or an Integer (timespamp) not a {type(val)}.")

    @classmethod
    def from_token(cls, encoded_token):
        decoded_token = decode_token(encoded_token)
        inst = cls()
        inst.jti = decoded_token["jti"]
        inst.type = decoded_token["type"]
        inst.expires = decoded_token["exp"]
        inst.fresh = decoded_token.get("fresh", False)
        inst.revoked = False
        return inst

    def is_expired(self):
        return self.expires < datetime.utcnow()
