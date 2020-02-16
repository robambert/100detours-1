from ...extensions import jwt
from .utils import _claim_loader, _identity_loader, _user_loader, _in_blacklist


# ---- JWT settings ----

jwt.user_claims_loader(_claim_loader)
jwt.user_identity_loader(_identity_loader)
jwt.user_loader_callback_loader(_user_loader)
jwt.token_in_blacklist_loader(_in_blacklist)
