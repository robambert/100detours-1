import re
from functools import wraps
from typing import Sequence
import datetime


from flask import request, Response
from flask_restplus import abort

DOC = r"""Parameters:
{desc}

Query filter language:

arg=val ...................... Equal
arg=ne:val ................... Different from
arg=gt:val ................... Greater than
arg=gte:val .................. Greater than or equal
arg=lt:val ................... Lower than
arg=lte:val .................. Lower than or equal
arg=btw:(val1,val2) .......... Between val1 (included) and val2 (excluded)
arg=nbtw:(val1,val2) ......... Lower than val1 (excluded) and bigger than val2 (included)
arg=in:(val1,[vals,..]) ...... One of val1, val2, ...
arg=nin:(val1,[vals,..]) ..... Not one of val1, val2, ...

If the field is a list, the filtering operation is applied to each element rather than the list itself;
if the filtered list is empty, the whole document is discarded. 

You can use quotation marks if treat any enclosed char as litterals.
You can escape quotation marks using '\'.

Example:
?date=lt:2020-01-25&name=Jean Dupont
?my_str=in:(Hello, "Oh, hi \"Mark\"")
"""


def filter_args(*__args, api=None, single=False):
    """Filters a Json-like document list or document.

    For each query filter argument pass a dict
    {
      "name": "my_arg",
      "caster": "int", # callable, used to deserialize query parameter
      "description": "Some information on the expected value"  # [Optional] Additional description for swagger doc
      "loader": "lamdba x: int(x) % 10" # [Optional] callable, used to load json field value before appling filter
    }
    Args
    ----
        __args: list[dict]
            comma serated list of dictionary with containing keys name" and "caster"
        api: flask-restplus.Api or flask-restplus.Namespace instance
            requiered for the filter query arguemnt to be documented in swagger ui
        single: False
            if True, filter a single document rather than a list of documents
    """
    for dct in __args:
        desc = dct.get("description")
        if desc and not desc.endswith("\n"):
            desc += "\n"
            dct["description"] = desc

    def wrapper(fct):
        @api.doc(params={r"{filter(s)}": DOC.format(desc="".join([dct.get('description', "") for dct in __args]))})
        @wraps(fct)
        def inner(*args, **kwargs):
            if getattr(request, "parsed_args", None) is None:
                setattr(request, "parsed_args", dict())
            for dct in __args:
                try:
                    __query_parser = FilterArgParser(dct["caster"], loader=dct.get("loader", None))
                    __query_parser.parse(request.args.get(dct["name"], None))
                    if __query_parser.parsed:
                        request.parsed_args.update({f"{dct['name']}": __query_parser})
                except QueryParsingException as e:
                    abort(getattr(e, "code", 400), e.with_sub(f"'{dct['name']}'"))

            return filter_json(fct(*args, **kwargs), getattr(request, "parsed_args", None))

        return inner

    return wrapper


class QueryParsingException(Exception):
    def __init__(self, *args, value=None, **kwargs):
        for k, v in kwargs.items():
            setattr(self, k, v)
        self.parsing_value = value
        super().__init__(*args)

    def __str__(self):
        if hasattr(self, "parsing_value"):
            return f"An error has occured while parsing '{self.parsing_value}'"
        if hasattr(self, "message"):
            return self.message
        else:
            return "An error has occured while parsing"

    def with_sub(self, pre=""):
        l = [pre] if pre else []
        l.extend([self, *self.args])
        return " | ".join([str(el) for el in l])


class QuerySyntaxError(Exception):
    def __init__(self, string=None, message=None):
        self.string = string
        self.message = message
        super().__init__()

    def __str__(self):
        msg = "invalid syntax"
        if hasattr(self, "string", None):
            msg = f"{self.string}: " + msg
        if hasattr(self, "message"):
            msg += ": " + self.message
        else:
            return "An error has occured while parsing"


OPERATORS = {
    "ne",
    "lt",
    "lte",
    "gt",
    "gte",
    "btw",
    "nbtw",
    "in",
    "nin",
}

OP_NUM_VALUES = {
    "eq": 1,
    "ne": 1,
    "lt": 1,
    "lte": 1,
    "gt": 1,
    "gte": 1,
    "btw": 2,
    "nbtw": 2,
    "in": -1,
    "nin": -1,
}

OP_TO_MONGOENGINE = {
    "eq": "",
    "ne": "__ne",
    "lt": "__lt",
    "lte": "__lte",
    "gt": "__gt",
    "gte": "__gte",
    "btw": ("gte", "lt"),
    "nbtw": ("lt", "gte"),
    "in": "__in",
    "nin": "__nin",
}

OP_TO_PRED = {
    "eq": lambda x, y: x == y,
    "ne": lambda x, y: x != y,
    "lt": lambda x, y: x < y,
    "lte": lambda x, y: x <= y,
    "gt": lambda x, y: x > y,
    "gte": lambda x, y: x >= y,
    "btw": lambda x, v: x >= v[0] and x <= v[1],
    "nbtw": lambda x, v: x < v[0] and x >= v[1],
    "in": lambda x, l: x in l,
    "nin": lambda x, l: x not in l,
}


class FilterArgParser(object):
    """Deserialize filtering query parameter."""

    # match: `{op}:{val}` where {op} is one of the operator and val can be anything
    _reg_complex = re.compile(f"^({'|'.join(OPERATORS)}):(.+)$")
    # Match any character in parenthesis
    _tuple_val = re.compile(r"^\(([^\)])$")

    def __init__(self, caster, loader=None):
        self.caster = caster
        self.loader = loader if loader != "caster" else caster
        self.parsed = None
        self.op = None
        self.val = None

    def cast(self, value):
        try:
            return self.caster(value)
        except Exception as e:
            raise QueryParsingException(e, value=value)

    def load(self, value):
        try:
            return self.loader(value)
        except Exception as e:
            raise QueryParsingException(e, value=value)

    def parse(self, data):
        """Parse a query parameter value according to the mini-language spec.

        Args
        ----
            data: str
                the value part of a url query argument key=val
        Returns
        -------
            None if data is None otherwise a dict {"op": op, "val": val} where op is one of OPERATORS and val is either a single value or
            a tuple of values of type defined by the return type of self.caster function.
            The returned dict is stored in self.parsed and its entries are accessible through self.op / self["op"] and self.val / self["val"]
        """
        if data is None:
            return

        data = unquote(data)
        m = self._reg_complex.match(data)
        if m is None:  # simple value
            self.val = self.cast(data)
            self.op = "eq"
        else:
            op, val = m.groups()
            num_val = OP_NUM_VALUES[op]
            self.op = op
            if num_val == 1:
                self.val = self.cast(val)
            else:
                if val.strip() == "()":
                    vals = tuple()
                else:
                    err = None
                    try:
                        vals = extract_tuple_elements(val)
                    except QuerySyntaxError as e:
                        err = e
                    finally:
                        if err or (num_val != -1 and len(vals) != num_val):
                            tuple_str = "None"
                            tuple_str = f"{num_val}-tuple" if num_val > 0 else tuple_str
                            tuple_str = f"n-tuple" if num_val < 0 else tuple_str
                            if err:
                                raise QueryParsingException(err, value=data)
                            else:
                                raise QueryParsingException(
                                    ValueError(f"Operator '{op}' expects a {tuple_str} as value but recieved '{val}'"),
                                    value=data)

                self.val = [self.cast(v) for v in vals]

        self.parsed = {"op": self.op, "val": self.val}
        return self.parsed

    def __call__(self, val):
        if isSeqNotStr(val):
            fval = []
            for v in val:
                fv = self(v)
                if fv:
                    fval.append(fv)
            return fval

        fval = val if self.loader is None else self.load(val)
        # print(f"* {fval} [{type(fval)}] {self.op} {self.val} [{type(self.val)}]")
        if self.op is None or OP_TO_PRED[self.op](fval, self.val):
            return val
        return None

    def __getitem__(self, key):
        if key in ("op", "val"):
            return getattr(self, key)


def filter_single_json(doc, filters):
    fdoc = dict()
    for key in doc:
        if key in filters:
            # print(f"**key : {key}")
            fval = filters[key](doc[key])
            if fval:
                fdoc[key] = fval
            else:
                fdoc = None
                break
        else:
            fdoc[key] = doc[key]
    return fdoc


def filter_json(data, filters, single=False):
    if isinstance(data, Response):
        if not data.is_json():
            raise ValueError("Cannot filter non-json response body!")
        data.data = filter_json(data.get_json(), filters)
        return data

    elif isinstance(data, tuple) and len(data) == 2:  # case (json-like, status_code)
        data, code = data
        return filter_json(data, filters), code

    ret = list()
    if single:
        ret = filter_single_json(data, filters)
    else:
        for i, doc in enumerate(data):
            # print(f"*** doc {i}")
            fdoc = filter_single_json(doc, filters)
            if fdoc:
                ret.append(fdoc)
    return ret


# ---- field dict helper ---


class Fields(object):
    """Common type helpers dict factories."""

    @classmethod
    def _to_dict(cls, name, caster, description=None, loader=None):
        dct = {"name": name, "caster": caster}
        if description:
            dct["description"] = description
        if loader:
            dct["loader"] = loader
        return dct

    @classmethod
    def int(cls, name, description=None):
        if description is None:
            description = f"{name}: int"
        return cls._to_dict(name, int, description)

    @classmethod
    def str(cls, name, description=None):
        if description is None:
            description = f"{name}: string"
        return cls._to_dict(name, str, description)

    @classmethod
    def date(cls, name, description=None):
        if description is None:
            description = f"{name}: date [format 'yyyy-mm-dd']"
        return cls._to_dict(name, datetime.date.fromisoformat, description, datetime.date.fromisoformat)

    @classmethod
    def datetime(cls, name, description=None):
        if description is None:
            description = f"{name}: datetime [format 'yyyy-mm-ddThh:mm:ss']"
        return cls._to_dict(name, datetime.date.fromisoformat, description, datetime.date.fromisoformat)

    @classmethod
    def date_or_datime(cls, name, description=None):
        """When filtering a datetime field, if the filter value is a date,
        then the time componement of the field is discarded for the comparison."""

        if description is None:
            description = f"{name}: date or datetime [format 'yyyy-mm-ddThh:mm:ss' or 'yyyy-mm-dd']"

        class DateDatetime(object):
            def __init__(self):
                self.use_date = False

            def caster(self, val):
                casted = None
                try:
                    casted = datetime.date.fromisoformat(val)
                    self.use_date = True
                except ValueError:
                    pass
                return casted or datetime.datetime.fromisoformat(val)

            def loader(self, val):
                dt = datetime.datetime.fromisoformat(val)
                return dt.date() if self.use_date else dt

        ddt = DateDatetime()
        return cls._to_dict(name, ddt.caster, description, ddt.loader)

# ---- filtering mongoengine.QuerySet ----


def to_mongoengine(argname, filter_dict):
    if not filter_dict:
        return dict()
    suffix = OP_TO_MONGOENGINE[filter_dict["op"]]
    if isinstance(suffix, list):
        ret = dict()
        for op, val in zip(suffix, filter_dict["val"]):
            ret.update(to_mongoengine(argname, {"op": f"{op}", "val": val}))
        return ret
    else:
        return {f"{argname}{suffix}": filter_dict['val']}


def filter_mongoengine(query_set, parsed_args):
    if not parsed_args:
        return query_set
    all_filters = dict()
    for argname, filter_dict in parsed_args.items():
        all_filters.update(to_mongoengine(argname, filter_dict))
    ret = query_set.filter(**all_filters)
    return ret


# ---- utils ----


def prune_nones(seq):
    """Return the list of non None elements of `seq`"""
    return [el for el in seq if el is not None]


def unquote(val, quote_char='"'):
    """Strip quotation enclosing marks if existing."""
    if val.startswith(quote_char) and val.endswith(quote_char):
        val = val[1:-1]
    return val


def extract_tuple_elements(val: str):
    """Extract string elements in valid tuple string."""
    val = val.strip()
    if not val.startswith("(") or not val.endswith(")"):
        raise QuerySyntaxError(val, f"tuple must start with '(' and end with ')'.")
    val = val[1:-1]
    quoted_els = find_quoted_elements(val)

    els = list()
    quot_iter = iter(quoted_els)
    sq, eq = next(quot_iter, (-1, -1))

    start = 0
    cur = start
    while cur < len(val):
        if cur == sq:
            cur = eq + 1
            sq, eq = next(quot_iter, (-1, -1))

        elif val[cur] == ',':
            els.append(val[start: cur])
            cur += 1
            start = cur
        else:
            cur += 1
    els.append(val[start:])
    return [unquote(el.strip()).replace(r'\"', '"') for el in els]


def find_quoted_elements(val: str):
    """Find pairs of non escaped quotation marks"""
    ret = list()
    cur = 0
    while(True):
        start = find_next_quote(val, cur)
        if start == -1:
            break
        else:
            end = find_next_quote(val, start + 1)
            if end == -1:
                raise QuerySyntaxError(val, f"opening '\"' at index {start} never closed.")
            ret.append((start, end))
            cur = end + 1
    return ret


def find_next_quote(val: str, start: int):
    """Return the index of the next non-escaped quotation mark, or -1."""
    cur = start
    while(True):
        cur = val.find('"', cur)
        if cur > 0 and val[cur - 1] == '\\':
            cur += 1
            continue
        else:
            return cur


def isSeqNotStr(val):
    return isinstance(val, Sequence) and not isinstance(val, (str, bytes))
