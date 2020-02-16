from datetime import time


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
