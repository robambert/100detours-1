from datetime import time
import requests as reqs
from flask_restplus import abort

def time_to_mins(val):
    return val.hour * 60 + val.minute


def mins_to_time(val):
    return time(hours=int(val / 60), minute=val % 60)


def find_duplicates(seq):
    """Return the set of duplicate elements in sequence `seq`, or an empty set."""
    duplicate = set()
    values_set = set()
    for el in seq:
        if el in values_set:
            duplicate.add(el)
        else:
            values_set.add(el)
    return duplicate


url = "https://api-adresse.data.gouv.fr/search/"
ref_lon = 2.305
ref_lat = 48.857


def get_coords(addr):
        r = reqs.get(
            url,
            dict(
                q=addr,
                lat=ref_lat,
                lon=ref_lon,
                autocomplete=1,
                type="housenumber"
            )
        )
        r.raise_for_status()
        if not r.json()["features"]:
            abort("400", "Address cannot be resolved!")

        val = r.json()["features"][0]
        addr = val["properties"]["label"]
        lon, lat = val["geometry"]["coordinates"]
        return lon, lat, addr
