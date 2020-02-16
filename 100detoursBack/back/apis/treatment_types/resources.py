from flask import request
from flask_restplus import Resource, Namespace
from flask_accepts import accepts, responds

from .models import TreatmentTypeModel, TreatmentTypeSchema
from ..auth.utils import manager_required, auth_required


ns = Namespace("ttyes", description="Treatments types related operations.")


@ns.route("/")
class TreatmentTypes(Resource):

    @responds(schema=TreatmentTypeSchema(many=True), api=ns)
    @auth_required
    def get(self):
        """All treatment types."""
        return TreatmentTypeModel.objects

    @accepts(schema=TreatmentTypeSchema(), api=ns)
    @responds(schema=TreatmentTypeSchema(), api=ns)
    @manager_required
    def post(self):
        """Add a treatment type."""
        ttype = TreatmentTypeModel(**request.parsed_obj)
        ttype.save()
        return ttype


@ns.route("/<int:rid>")
@ns.doc(params={"rid": "The treatment type rid (index start from 1)"})
class TreatmentType(Resource):

    @responds(schema=TreatmentTypeSchema(), api=ns)
    @auth_required
    def get(self, rid):
        """Get a single treatment type."""
        return TreatmentTypeModel.with_rid(rid)

    @accepts(schema=TreatmentTypeSchema(partial=True), api=ns)
    @responds(schema=TreatmentTypeSchema(), api=ns)
    @manager_required
    def put(self, rid):
        """Update a treatment type."""
        ttype = TreatmentTypeModel.with_rid(rid)
        ttype.modify(**request.parsed_obj)
        return ttype

    @manager_required
    def delete(self, rid):
        """Delete a treatment type"""
        ttype = TreatmentTypeModel.with_rid(rid)
        ttype.delete()
        return {}, 204
