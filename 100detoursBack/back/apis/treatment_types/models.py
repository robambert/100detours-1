from mongoengine import IntField, StringField, SequenceField
from marshmallow import fields, validate

from ..common.models import RidDocument
from ..common.marshmallow_utils import RidSchema


class TreatmentTypeModel(RidDocument):
    rid = SequenceField(unique=True)  # Overriding to use own counter rather than the one commun to all RidDocument childs.
    code = IntField(required=True, unique=True)
    name = StringField(min_length=1, max_length=255)
    duration = IntField(required=True)


class TreatmentTypeSchema(RidSchema):
    __model__ = TreatmentTypeModel
    code = fields.Int(strict=True, required=True)
    name = fields.String(validate=validate.Length(1, 255))
    duration = fields.Int(strict=True, required=True, validate=validate.Range(min=1))
