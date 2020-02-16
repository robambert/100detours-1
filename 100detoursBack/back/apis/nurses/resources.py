from flask import request
from flask_restplus import Namespace, Resource, abort
from flask_accepts import accepts

from .models import NurseModel, NurseSchema, AgendaSchema, HackNurseSchema
from ..auth.utils import manager_or_owner_required, manager_required, get_jwt_claims
from ..common.models import remove_from_list
from ..common.query_language import filter_args, Fields
from ..common.join_responds import responds


ns = Namespace("nurses", description="Nurses related operations")


@ns.route("/")
class Nurses(Resource):

    @manager_required
    @filter_args(Fields.int("rid"), Fields.str("name"), Fields.str("user"),
                 Fields.int("treatment_types"), Fields.date_or_datime("agenda"), api=ns)
    @responds(schema=NurseSchema(many=True), api=ns)
    def get(self):
        """Get all nurses."""
        return NurseModel.objects

    @accepts(schema=HackNurseSchema(), api=ns)
    @responds(schema=NurseSchema(), api=ns)
    @manager_required
    def post(self):
        """Add a new nurse. (Also create a new User in the process)"""
        nurse = NurseModel(**request.parsed_obj)
        nurse.save()
        return nurse


@ns.route("/<int:rid>")
@ns.doc(params={"rid": "The nurse rid (index start from 1)"})
class Nurse(Resource):

    @filter_args(Fields.date_or_datime("agenda"), api=ns, single=True)
    @responds(schema=NurseSchema(), api=ns)
    @manager_or_owner_required(NurseModel)
    def get(self, rid):
        """Get a single nurse."""
        return NurseModel.with_rid(rid)

    @accepts(schema=NurseSchema(partial=True), api=ns)
    @responds(schema=NurseSchema(), api=ns)
    @manager_or_owner_required(NurseModel)
    def put(self, rid):
        """Update a nurse."""
        nurse = NurseModel.with_rid(rid)
        args = request.parsed_obj
        if get_jwt_claims()["usertype"] != 0 and "user" in args:
            raise abort(403, "Not allowed to change user_rid")
        nurse.modify(**args)
        return nurse

    @manager_required
    def delete(self, rid):
        """Delete a nurse."""
        NurseModel.with_rid(rid).delete()
        return {}, 204


@ns.route("/<int:rid>/agenda")
@ns.doc(params={"rid": "The nurse rid (index start from 1)"})
class NurseAgenda(Resource):

    @responds(schema=AgendaSchema(), api=ns)
    @manager_or_owner_required(NurseModel)
    @filter_args(api=ns)
    def get(self, rid):
        """Get a nurse agenda."""
        agenda = NurseModel.with_rid(rid).agenda
        fdate = request.parsed_args["date"]
        print(f"query param: {fdate}")
        return agenda

    @accepts(schema=AgendaSchema(), api=ns)
    @responds(schema=AgendaSchema(), api=ns)
    @manager_or_owner_required(NurseModel)
    def post(self, rid):
        """Add one or multiple entries to the agenda."""
        nurse: NurseModel = NurseModel.with_rid(rid)
        nurse.agenda.extend(request.parsed_obj)
        nurse.save()
        return nurse.agenda

    @accepts(schema=AgendaSchema(), api=ns)
    @responds(schema=AgendaSchema(), api=ns)
    @manager_required
    def delete(self, rid):
        """Remove one or multiple entries in the agenda."""
        nurse: NurseModel = NurseModel.with_rid(rid)
        remove_from_list(nurse.agenda, request.parsed_obj)
        nurse.save()
        return nurse.agenda
