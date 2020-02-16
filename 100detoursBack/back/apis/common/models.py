from collections import defaultdict

from flask_restplus import abort
from bson import json_util
from flask_mongoengine import BaseQuerySet, Document
from mongoengine import EmbeddedDocument, SequenceField, NotUniqueError, ListField, ValidationError, StringField, FloatField
from marshmallow import fields, post_load, pre_dump
from .marshmallow_utils import FlaskSchema
from .utils import get_coords

# ---- Extends Document class with RID (Ressource identifier) ----

MONGOID = 0
RID = 1
EMBED = 2


class RidQuerySet(BaseQuerySet):

    def to_bson(self, exclude_mongo_id=True, exclude=None, ref_as=EMBED):
        return [doc.to_bson(exclude_mongo_id, exclude, ref_as) for doc in self]

    def to_python(self, exclude_mongo_id=True, exclude=None, ref_as=EMBED):
        return [doc.to_python(exclude_mongo_id, exclude, ref_as) for doc in self]

    def to_json(self, exclude_mongo_id=True, exclude=None, ref_as=EMBED):
        return [doc.to_json(exclude_mongo_id, exclude, ref_as) for doc in self]


class RidDocument(Document):
    """Flask-Mongoengine abstract document with extra features:

    - RID : add a rid field (Ressource identifier), that is, an auto-incrementing
        field that can be used as a short id for rest apis, RID must not be inititalized nor set

    """
    meta = {
        "abstract": True,
        "queryset_class": RidQuerySet,
    }

    # auto-incrementing field that can be used as a short id for rest apis, RID must not be inititalized nor set
    rid = SequenceField(unique=True)

    def to_bson(self, exclude_mongo_id=True, exclude=None, ref_as=EMBED):
        exclude = set(exclude or set())

        # Extract subfield exclude (pattern: "field.subfield[.subsubfield]")
        exclude_sub = defaultdict(set)
        exclude_parent = set()
        for field in exclude:
            if "." in field:
                pos = field.find(".")
                exclude_sub[field[:pos]].add(field[pos + 1:])
            else:
                exclude_parent.add(field)

        # Retreive bson
        fields = set(self) - exclude_parent
        ret = super().to_mongo(fields=fields)

        # Delete '_id' field (which is always included by mongoengine)
        if exclude_mongo_id:
            del ret["_id"]

        # Resolve reference fields (Note: Embedded field are automaticaly resolved)
        if ref_as == EMBED:
            for field in ret:
                if isinstance(self[field], Document):
                    ret[field] = self[field].to_bson(exclude_mongo_id=exclude_mongo_id, exclude=exclude_sub[field], ref_as=ref_as)
        elif ref_as == RID:
            for field in ret:
                if isinstance(self[field], Document):
                    ret[field] = self[field].rid
        return ret

    def to_python(self, exclude_mongo_id=True, exclude=None, ref_as=EMBED):
        return self.to_bson(exclude_mongo_id, exclude, ref_as).to_dict()

    def to_json(self, exclude_mongo_id=True, exclude=None, ref_as=EMBED):
        return json_util.dumps(self.to_bson(exclude_mongo_id, exclude, ref_as))

    def save(self, *args, raise_http_error=True, **kwargs):
        try:
            super().save(*args, **kwargs)
        except NotUniqueError as e:
            if raise_http_error:
                abort(400, str(e))
            else:
                raise e

    @classmethod
    def with_rid(cls, val, raise_http_error=True):
        ret = cls.objects(rid=val).first()
        if ret is None and raise_http_error:
            abort(404, f"No {cls.get_collection_name()} with rid {val} can be found!")
        return ret

    @classmethod
    def all_bson(cls, exclude_mongo_id=True, exclude=None, ref_as=EMBED):
        print("All bson")
        return cls.objects.to_bson(exclude_mongo_id, exclude, ref_as)

    @classmethod
    def all_json(cls, exclude_mongo_id=True, exclude=None, ref_as=EMBED):
        return json_util.dumps(cls.all_bson(exclude_mongo_id, exclude, ref_as))

    @classmethod
    def all_python(cls, exclude_mongo_id=True, exclude=None, ref_as=EMBED):
        return [dict(doc) for doc in cls.all_bson(exclude_mongo_id, exclude, ref_as)]

    @classmethod
    def get_collection_name(cls):
        return cls._get_collection_name() or cls.__name__


# ---- Common models & schemas ----

class AddressModel(EmbeddedDocument):
    lon = FloatField()
    lat = FloatField()
    addr = StringField()


class AddressSchema(FlaskSchema):
    lon = fields.Float()
    lat = fields.Float()
    addr = fields.Str()


# ---- Helper methods ----

def remove_from_list(list_field: ListField, seq_to_remove):
    not_in_list = set()
    for el in seq_to_remove:
        try:
            list_field.remove(el)
        except ValueError:
            not_in_list.add(el)
    if not_in_list:
        raise ValidationError(errors={f"{list_field}": f"Elements {not_in_list} are not in list."})
