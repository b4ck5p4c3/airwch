#!/usr/bin/env python

import base64
import hashlib
import json
import random
import argparse
import os
import string

from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.backends import default_backend

DEFAULT_PREFIX = "".join(random.choice(string.ascii_uppercase + string.digits) for _ in range(6))

parser = argparse.ArgumentParser()
parser.add_argument("-n", "--nkeys", help="number of keys to generate", type=int, default=1)
parser.add_argument(
    "-p",
    "--prefix",
    help="prefix of the keyfiles",
    type=str,
    default=DEFAULT_PREFIX,
)
parser.add_argument("-o", "--output", help="output dir", type=str, default=os.getcwd())

args = parser.parse_args()


def sha256(data):
    digest = hashlib.new("sha256")
    digest.update(data)
    return digest.digest()


def gen_key():
    key = {}
    priv = random.getrandbits(224)
    adv = (
        ec.derive_private_key(priv, ec.SECP224R1(), default_backend())
        .public_key()
        .public_numbers()
        .x
    )

    priv_bytes = priv.to_bytes(28, "big")
    adv_bytes = adv.to_bytes(28, "big")

    key["private"] = base64.b64encode(priv_bytes).decode("ascii")
    key["advertisement"] = base64.b64encode(adv_bytes).decode("ascii")
    s256_b64 = base64.b64encode(sha256(adv_bytes)).decode("ascii")

    if "/" in s256_b64[:7]:
        gen_key()

    return key


def save_to_file(keys):
    file_name = "%s.keys" % (args.prefix)
    full_file_path = os.path.join(args.output, file_name)
    with open(full_file_path, "w") as file:
        file.write(keys)
    print(f"Output will be written to {full_file_path}")


if __name__ == "__main__":

    keys = []
    for i in range(args.nkeys):
        keys.append(gen_key())

    keys = json.dumps(keys, indent=4)
    print(keys)
    save_to_file(keys)
