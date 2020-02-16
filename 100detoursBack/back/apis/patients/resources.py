from flask import request
from flask_restplus import Namespace, Resource, abort
from flask_accepts import responds, accepts

from .models import PatientModel, PatientSchema
from ..auth.utils import manager_or_owner_required, manager_required, get_jwt_claims


ns = Namespace("patients", description="Patients related operations")


@ns.route("/")
class Patients(Resource):

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
    @manager_or_owner_required
    def get(self, rid):
        """Get a single patient."""
        return PatientModel.with_rid(rid)

    @accepts(schema=PatientSchema(partial=True), api=ns)
    @responds(schema=PatientSchema(), api=ns)
    @manager_or_owner_required
    def put(self, rid):
        """Update a patient."""
        patient = PatientModel.with_rid(rid)
        args = request.parsed_obj
        if get_jwt_claims()["usertype"] != 0 and "user" in args:
            raise abort(403, "Not allowed to change user_rid")
        patient.modify(**args)
        return patient

    @manager_required
    def delete(self, rid):
        """Delete a patient."""
        PatientModel.with_rid(rid).delete()
        return {}, 204
