import json
from functools import wraps

from flask import request
from flask_accepts import responds as _responds

JOINRID_HEADER_DOC = \
"""Expect a string representing a list of string.

Each element of the list must have the format 'base[.sub[.subsub[...]]' where base is the name of an rid field.

Example:
let our document to be resolved be:
`FooModel = {
    rid: 2,
    bar: "hello",
    baz: 1.1
}`

and our normal response be:
`{
    "foo": 2,
    "someotherfield": "somevalue"
}`

if JoinRID = ["foo"]
then the returned document would be:
`{
    "foo": 2,
    "foo_join": {
        rid: 2,
        bar: "hello",
        baz: 1.1
    },
    "someotherfield": "somevalue"
}`

and if JoinRID = ["foo.bar"]
then the returned document would be:
`{
    "foo": 2,
    "foo_join": {
        bar: "hello"
    },
    "someotherfield": "somevalue"
}`
"""


@wraps(_responds)
def responds(model_name: str=None, schema=None, many: bool=False, api=None, status_code: int=200,
             validate: bool=False, description: str=None, use_swagger: bool=True):
    def wrapper(fn):
        wfn = _responds(model_name=model_name, schema=schema, many=many, api=api, status_code=status_code,
                        validate=validate, description=description, use_swagger=use_swagger)(fn)
        if api and use_swagger:
            wfn = api.doc(params={"JoinRID": JOINRID_HEADER_DOC})(wfn)
        @wraps(wfn)
        def inner(*args, **kwargs):
            if schema is not None:
                join_header = request.headers.get("JoinRID", None)
                if join_header:
                    val = json.loads(join_header)
                    schema.context["join"] = val
                    print(f"***** {val}")
            return wfn(*args, **kwargs)
        return inner
    return wrapper
