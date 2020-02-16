from flask import request
from flask_restplus import Namespace, Resource, abort
from flask_accepts import responds, accepts

from .models import PatientModel, PatientSchema
from ..auth.utils import manager_required, auth_required
from ..common.query_language import filter_args, Fields


ns = Namespace("patients", description="Patients related operations")


@ns.route("/")
class Patients(Resource):

    @filter_args(
        Fields.str("name"),
        Fields.str("address"),
        api=ns)
    @responds(schema=PatientSchema(many=True), api=ns)
    @manager_required
    def get(self):
        """Get all patients."""
        return PatientModel.objects

    @accepts(schema=PatientSchema(), api=ns)
    @responds(schema=PatientSchema(), api=ns)
    @manager_required
    def post(self):
        """Add a new patient."""
        patient = PatientModel(**request.parsed_obj)
        patient.save()
        return patient


@ns.route("/<int:rid>")
@ns.doc(params={"rid": "The patient rid (index start from 1)"})
class Patient(Resource):

    @responds(schema=PatientSchema(), api=ns)
    @auth_required
    def get(self, rid):
        """Get a single patient."""
        return PatientModel.with_rid(rid)

    @accepts(schema=PatientSchema(partial=True), api=ns)
    @responds(schema=PatientSchema(), api=ns)
    @manager_required
    def put(self, rid):
        """Update a patient."""
        patient = PatientModel.with_rid(rid)
        patient.modify(**request.parsed_obj)
        return patient

    @manager_required
    def delete(self, rid):
        """Delete a patient."""
        PatientModel.with_rid(rid).delete()
        return {}, 204
