from mongoengine import (StringField, DateTimeField, SequenceField, ReferenceField, EmbeddedDocumentField,
                         ListField, EmbeddedDocument, ValidationError)
from marshmallow import fields, validate, post_load, pre_dump
from flask_restplus import abort

from ..common.models import RidDocument, AddressModel, AddressSchema
from ..common.marshmallow_utils import RidSchema, RidField, FlaskSchema
from ..common.utils import get_coords
from ..nurses.models import NurseModel
from ..treatment_types.models import TreatmentTypeModel


class PatientModel(RidDocument):
    rid = SequenceField(unique=True)  # Overriding to use own counter rather than the one common to all RidDocument childs.
    name = StringField(min_lenght=1, max_length=255, required=True)
    address = EmbeddedDocumentField(AddressModel, required=True)
    # treatments = ListField(ReferenceField(TreatmentModel), required=True)


class PatientSchema(RidSchema):
    __model__ = PatientModel
    name = fields.String(validate=validate.Length(1, 255), required=True)
    address = fields.Str(required=True)
    # treatments = fields.List(RidField(TreatmentModel), required=True)

    @post_load
    def add_coords(self, data, **kwargs):
        lon, lat, addr = get_coords(data["address"])
        data["address"] = dict(
            addr=addr,
            lon=lon,
            lat=lat
        )
        print(data)
        return data

    @pre_dump
    def rm_coords(self, data, **kwargs):
        data["address"] = data["address"]["addr"]
        return data


class _PatientSchemaAll(RidSchema):
    __model__ = PatientModel
    name = fields.String(validate=validate.Length(1, 255), required=True)
    address = fields.Nested(AddressSchema(), required=True)
