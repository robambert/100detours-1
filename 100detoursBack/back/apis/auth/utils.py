from datetime import datetime
from functools import wraps
import numbers

from flask_jwt_extended import verify_jwt_in_request, get_jwt_claims, get_jwt_identity

from ..users.models import UserModel
from .models import TokenModel

def auth_requied(fn):
    """(NOT IMPLEMENTED) Same as flask_jjwt_extended.jwt_required. (Will add proper documentation)."""
    pass

def manager_required(fn):
    """Protect a resource and only let managers access it."""
    @wraps(fn)
    def wrapper(*args, **kwargs):
        verify_jwt_in_request()
        claims = get_jwt_claims()
        if claims["usertype"] != 0:
            return {"message": "Forbidden resource"}, 403
        else:
            return fn(*args, **kwargs)
    return wrapper


def manager_or_owner_required(fn):
    """Protect a resource and only let managers or the 'owner' access it.
    Work by checking the function first argument (flask variable ressource name placeholder) against the identity extracted by the token.
    """
    @wraps(fn)
    def wrapper(_arg1, *_args, **_kwargs):
        verify_jwt_in_request()
        if get_jwt_claims()["usertype"] == 0 or (isinstance(_arg1, str) and get_jwt_identity() == _arg1):
            return fn(_arg1, *_args, **_kwargs)
        else:
            return {"message": "Forbidden resource"}, 403

    return wrapper


def usertype_required(allowed_usertypes):
    """Protect a resource and let `allowed_usertypes` access it"""
    if isinstance(allowed_usertypes, numbers.Integral):
        allowed_usertypes = {allowed_usertypes}

    def decorator(fn):
        @wraps(fn)
        def wrapper(*args, **kwargs):
            verify_jwt_in_request()
            claims = get_jwt_claims()
            if claims["usertype"] in allowed_usertypes:
                return {"message": "Forbidden resource"}, 403
            else:
                return fn(*args, **kwargs)
        return wrapper
    return decorator


def _claim_loader(user):
    """Receive flask_jwt_extended.create_access_token first argument and use it to generate the claims.

    Should recieve a UserModel document and return the username and usertype as claims.
    """
    return {
        "username": user.username,
        "usertype": user.usertype,
    }


def _identity_loader(user):
    """Receive flask_jwt_extended.create_access_token first argument and use it to generate the identity.

    Should recieve a UserModel document and return the user rid as identity.
    """
    return user.rid


def _user_loader(identity):
    """Retrieve UserModel instance from identity. Defines flask_jwt_extended.get_jwt_identity returns value."""
    return UserModel.with_rid(identity)


def _in_blacklist(decoded_token):
    """Callback checking wether a given token is valid."""
    token = TokenModel.objects(jti=decoded_token["jti"]).first()
    return token is None or token.revoked


def prune_tokens():
    """Empty the blacklisted tokens from expired ones."""
    TokenModel.objects(expires__lte=datetime.utcnow()).delete()
