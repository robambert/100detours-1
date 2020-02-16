import copy
import requests
import datetime

from marshmallow import pprint

from .apis.data.models import DataModel, DataSchema
from .apis.treatment_types.models import TreatmentTypeSchema, TreatmentTypeModel
from .apis.nurses.models import NurseModel, NurseSchema
from .apis.patients.models import PatientModel, PatientSchema
from .apis.treatments.models import TreatmentModel, TreatmentSchema


dataSchema = DataSchema()
ttypeSchema = TreatmentTypeSchema(only=('rid', 'duration'))
nurseSchema = NurseSchema(only=('rid', 'treatment_types', 'agenda'))
patientSchema = PatientSchema(only=('rid', 'address'))
treatmentSchema = TreatmentSchema(only=('rid', 'patient', 'ttype', 'date'))


def get_mid():
    pass


def datetime_to_mins(val):
    if isinstance(val, str):
        val = datetime.datetime.fromisoformat(val)
    return val.hour * 60 + val.minute


def mins_to_datetime(val, date: datetime.date):
    return datetime.datetime(
        year=date.year,
        month=date.month,
        day=date.day,
        hour=val // 60,
        minute=val % 60,
        second=0
    )


def filter_and_cast_nurses(nurses, date):
    ret = list()
    for nurse in nurses:
        for entry in nurse["agenda"]:
            datetime_entry = datetime.datetime.fromisoformat(entry)
            if datetime_entry.date() == date.date():
                formated_nurse = copy.deepcopy(nurse)
                formated_nurse["agenda"] = datetime_to_mins(datetime_entry)
                ret.append(formated_nurse)
                break
    return ret


def filter_treatments(treatments, date):
    ret = list()
    for treatment in treatments:
        if datetime.datetime.fromisoformat(treatment["date"]).date() == date.date():
            cop = copy.deepcopy(treatment)
            del cop["date"]
            ret.append(cop)
    return ret


def remap_keys_intputs(data):
    ret = dict()
    ret["carSpeed"] = data["car_speed"]
    ret["walkSpeed"] = data["walk_speed"]
    ret["minCarDist"] = data["min_car_distance"]

    ret["treatmentTypes"] = list()
    for entry in data["ttypes"]:
        tmp = dict()
        tmp["_id"] = entry["rid"]
        tmp["duration"] = entry["duration"]
        ret["treatmentTypes"].append(tmp)

    ret["nurses"] = list()
    for entry in data["nurses"]:
        tmp = dict()
        tmp["_id"] = entry["rid"]
        tmp["treatmentTypeIds"] = entry["treatment_types"]
        tmp["startTime"] = entry["agenda"]
        ret["nurses"].append(tmp)

    ret["patients"] = list()
    rid_to_idx = {}
    cnt = 0
    for entry in data["patients"]:
        tmp = dict()
        tmp["_id"] = entry["rid"]
        tmp["location"] = entry["address"]
        tmp["treatments"] = list()
        ret["patients"].append(tmp)

        rid_to_idx[entry["rid"]] = cnt
        cnt += 1

    for entry in data["treatments"]:
        tmp = dict()
        tmp["_id"] = entry["rid"]
        tmp["treatmentTypeId"] = entry["ttype"]
        ret["patients"][rid_to_idx[entry["patient"]]]["treatments"].append(tmp)

    return ret


def prepare_inputs(date=None):
    if date is None:
        today = datetime.date.today()
        date = datetime.datetime(year=today.year, month=today.month, day=today.day)

    print(f"Preparing input for day : {date.date()}")
    ttypes = ttypeSchema.dump(TreatmentTypeModel.objects, many=True)

    nurses = nurseSchema.dump(NurseModel.objects, many=True)
    nurses = filter_and_cast_nurses(nurses, date=date)

    patients = patientSchema.dump(PatientModel.objects, many=True)

    treatments = treatmentSchema.dump(TreatmentModel.objects, many=True)
    treatments = filter_treatments(treatments, date=date)

    data = dataSchema.dump(DataModel.objects.get())
    ret = {"ttypes": ttypes, "nurses": nurses, "patients": patients, "treatments": treatments}
    ret.update(data)
    return ret


def handle_response(data, date):
    for appoint in data["appointments"]:
        t = TreatmentModel.with_rid(appoint["treatmentId"])
        t.modify(
            nurse=NurseModel.with_rid(appoint["nurseId"]),
            time=mins_to_datetime(appoint["schedule"], date)
        )


def request_algo(date, algo_url: str):
    if not algo_url.endswith("/"):
        algo_url += "/"
    data = prepare_inputs(date)
    formated = remap_keys_intputs(data)
    pprint(formated, indent=2)
    r = requests.post("http://" + algo_url, json=formated)
    r.raise_for_status()
    # pprint(r.json())
    handle_response(r.json(), date)
