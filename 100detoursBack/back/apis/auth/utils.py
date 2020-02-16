from datetime import datetime
from functools import wraps
import numbers

from flask_jwt_extended import verify_jwt_in_request, get_jwt_claims, get_jwt_identity, jwt_required
from flask_restplus import abort

from ..users.models import UserModel
from ..nurses.models import NurseModel
from .models import TokenModel


def auth_required(fn):
    wfn = jwt_required(fn)
    wfn.__doc__ = "[User] " + (wfn.__doc__ or "")
    return wfn


def manager_required(fn):
    """Protect a resource and only let managers access it."""
    @wraps(fn)
    def wrapper(*args, **kwargs):
        verify_jwt_in_request()
        claims = get_jwt_claims()
        if claims["usertype"] != 0:
            abort(403, "Forbidden resource")
        else:
            return fn(*args, **kwargs)
    
    wrapper.__doc__ = "[Manager] " + (wrapper.__doc__ or "")
    return wrapper


def manager_or_owner_required(model=UserModel):
    """Protect a resource and only let managers or the 'owner' access it.
    Work by checking the function first argument (flask variable ressource name placeholder) against the identity extracted by the token.

    Args
    ----
        resource_type (default: UserModel): RidModel
            The class defining the accessed resource, can either be UserModel or NurseModel, used to reteive the owner identity.
    """

    if model == UserModel:
        def __get_owner(rid):
            return rid
    elif model == NurseModel:
        def __get_owner(rid):
            return NurseModel.with_rid(rid).user.rid

    def inner(fn):
        from flask import request
        @wraps(fn)
        def wrapper(*_args, **_kwargs):
            verify_jwt_in_request()
            if get_jwt_claims()["usertype"] == 0 or (isinstance(_kwargs["rid"], int) and get_jwt_identity() == __get_owner(_kwargs["rid"])):
                return fn(*_args, **_kwargs)
            else:
                abort(403, "Forbidden resource")

        wrapper.__doc__ = "[Manager/Owner] " + (wrapper.__doc__ or "")
        return wrapper
    return inner


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
