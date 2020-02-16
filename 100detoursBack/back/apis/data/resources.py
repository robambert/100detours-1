from flask import request
from flask_restplus import Resource, Namespace
from flask_accepts import accepts, responds

from .models import DataModel, DataSchema
from ..auth.utils import manager_required


ns = Namespace("data", description="Get and update global data for algorithm (walk speed, car speed, ...).")


@ns.route("/")
class Data(Resource):
    """Handle global data needed by the algorithm."""
    @responds(schema=DataSchema(), api=ns)
    @manager_required
    def get(self):
        """Return the algorithm parameters."""
        return DataModel.objects.get()

    @accepts(schema=DataSchema(partial=True), api=ns)
    @responds(schema=DataSchema(), api=ns)
    @manager_required
    def put(self):
        """Update the algorithm parameters."""
        data = DataModel.objects.get()
        data.modify(**request.parsed_obj)
        return data
