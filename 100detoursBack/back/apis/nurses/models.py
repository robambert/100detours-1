from mongoengine import (StringField, DateTimeField, SequenceField, ReferenceField, EmbeddedDocumentField,
                         ListField, SortedListField, ValidationError)
from marshmallow import fields, validate, post_load, pre_dump

from ..common.utils import find_duplicates
from ..common.models import AddressModel, AddressSchema, RidDocument
from ..common.marshmallow_utils import RidSchema, RidField, FlaskSchema
from ..users.models import UserModel, UserSchema
from ..treatment_types.models import TreatmentTypeModel, TreatmentTypeSchema


class NurseModel(RidDocument):
    rid = SequenceField(unique=True)  # Overriding to use own counter rather than the one common to all RidDocument childs.
    user = ReferenceField(UserModel, required=True, unique=True)
    name = StringField(min_lenght=1, max_length=255, required=True)
    treatment_types = ListField(ReferenceField(TreatmentTypeModel, required=False), default=list)
    agenda = SortedListField(DateTimeField(), required=False, default=list())

    def clean(self):
        errs = dict()
        self.agenda.sort()
        for i in range(len(self.agenda) - 1):
            if self.agenda[i].date() == self.agenda[i + 1].date():
                errs["agenda"] = "Can only have one agenda per day!"

        duplicates = find_duplicates(self.treatment_types)
        if duplicates:
            errs["treatment_types"] = f"Treatment types list cannot contains duplicate elements."

        if errs:
            raise ValidationError(errors=errs)


class AgendaSchema(FlaskSchema):
    agenda = fields.List(
        fields.DateTime, required=True,
        description="List of this nurse work start times, can only contains one element for a given day."
    )

    @post_load
    def to_list(self, data, **kwargs):
        """Convert the dict with a single list field `agenda` to the list itself."""
        print(data["agenda"])
        return data["agenda"]

    @pre_dump
    def to_dict(self, data, **kwargs):
        """Convert the list to a dict with a single list field `agenda`."""
        return {"agenda": data}


class NurseSchema(RidSchema):
    __model__ = NurseModel
    user = RidField(UserSchema(), required=True, description="The user RID to associate with the new nurse")
    name = fields.Str(validate=validate.Length(1, 255), required=True)
    treatment_types = fields.List(RidField(TreatmentTypeSchema(), required=False), required=True, description="List of treatment_types RIDs")
    agenda = fields.List(
        fields.DateTime, required=True,
        description="List of this nurse work start times, can only contains one element for a given day."
    )


class HackNurseSchema(RidSchema):
    __model__ = NurseModel
    name = fields.Str(validate=validate.Length(1, 255), required=True)
    treatment_types = fields.List(RidField(TreatmentTypeSchema(), required=False), required=False,
                                  missing=list(), default=list(), description="List of treatment_types RIDs")
    agenda = fields.List(
        fields.DateTime, required=False, missing=list(), default=list(),
        description="List of this nurse work start times, can only contains one element for a given day."
    )

    @post_load
    def create_user(self, data, **kwargs):
        username = data["name"]

        if "agenda" not in data:
            data["agenda"] = list()

        if "treatment_types" not in data:
            data["treatment_types"] = list()

        user = UserModel(username=username, usertype=1)
        user.set_password("default")
        cnt = 2
        while(True):
            try:
                user.username = username.lower()
                user.save()
                user.reload()
                data["user"] = user
                break
            except Exception:
                username = data["name"] + str(cnt)
                cnt += 1
        print(data)
        return data
