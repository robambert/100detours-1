from os import path
import re

from werkzeug.exceptions import HTTPException
from marshmallow import pprint

from .extensions import db
from .apis.users.models import UserSchema
from .apis.treatment_types.models import TreatmentTypeSchema
from .apis.nurses.models import NurseSchema
from .apis.patients.models import PatientSchema
from .apis.treatments.models import _TreatmentSchemaLoadAll
from .apis.data.models import DataSchema


BASE_DIR_DATA = path.join(path.dirname(path.abspath(__file__)), "data")

# WARNING: If json file contains reference to another collection, it must be declared after so the reference can be resolved
INIT_DATA = [
    (UserSchema(), "users.json"),
    (TreatmentTypeSchema(), "ttypes.json"),
    (NurseSchema(), "nurses.json"),
    (PatientSchema(), "patients.json"),
    (_TreatmentSchemaLoadAll(), "treatments.json"),
    (DataSchema(), "data.json")
]

_ref_reg = re.compile(r"\$REF\{([^:]+):(.+)\}")


def resolve_ref(ref):
    ref = _ref_reg.match(ref)
    if ref is None:
        raise ValueError(f"Invalid reference syntax {ref}")
    ref_key, ref_val = ref.groups()
    ref_col, ref_field = ref_key.split(".")
    try:
        ret = eval(f"{ref_col}.objects({ref_field}={ref_val}).first().id")
    except Exception as e:
        raise e
    return ret


def reinit_db():
    print("Reinitializing database...")
    with db.app.app_context():
        db_name = db.app.config["MONGODB_DB"]
        db.connection.drop_database(db_name)

    try:
        for schema, data_file in INIT_DATA:
            print(f"Parsing {data_file}")
            with open(path.join(BASE_DIR_DATA, data_file), "r") as file:
                docs = schema.loads(file.read(), many=True)
                for doc in docs:
                    try:
                        mod = schema.__model__(**doc)
                    except Exception as e:
                        print(f"Cannot instanciate {schema.__model__.__name__} from \n{schema.dumps(doc, indent=2)}\n")
                        raise e

                    if isinstance(schema, UserSchema):
                        mod.set_password(mod.password)  # hashing password
                    mod.save()
    except HTTPException as e:
        print(e)
        pprint(e.data, indent=2)
        exit()
