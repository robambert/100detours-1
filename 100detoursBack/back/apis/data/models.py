from mongoengine import IntField, Document
from marshmallow import fields

from ..common.marshmallow_utils import FlaskSchema


class DataModel(Document):
    car_speed = IntField(required=True)
    walk_speed = IntField(required=True)
    min_car_distance = IntField(required=True)


class DataSchema(FlaskSchema):
    __model__ = DataModel
    car_speed = fields.Int(strict=True, required=True)
    walk_speed = fields.Int(strict=True, required=True)
    min_car_distance = fields.Int(strict=True, required=True)
