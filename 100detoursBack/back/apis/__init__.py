from flask_restplus import Api

from .addok.resources import ns as addok_ns
from .auth.resources import ns as auth_ns
from .data.resources import ns as data_ns
from .dev import ns as dev_ns
from .nurses.resources import ns as nurse_ns
from .patients.resources import ns as patient_ns
from .treatments.resources import ns as treatment_ns
from .treatment_types.resources import ns as ttype_ns
from .users.resources import ns as users_ns
from .algo.resources import ns as algo_ns

auths = {
    "bearer": {
        "type": "apiKey",
        "in": "Header",
        "name": "Authorization"
    }
}

api = Api(
    title="100detours back server and apis",
    description="Handles authorization, db and resources",
    version="1.0.0",
    security=["bearer"],
    authorizations=auths
)


api.add_namespace(auth_ns)
api.add_namespace(users_ns)
api.add_namespace(dev_ns)
api.add_namespace(treatment_ns)
api.add_namespace(ttype_ns)
api.add_namespace(nurse_ns)
api.add_namespace(patient_ns)
api.add_namespace(data_ns)
api.add_namespace(addok_ns)
api.add_namespace(algo_ns)
