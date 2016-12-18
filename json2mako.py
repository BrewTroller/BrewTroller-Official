#!/usr/bin/env python2.7

from __future__ import print_function

import os
import sys
import json
import argparse
import traceback

from mako.template import Template

def parse_args():
    parser = argparse.ArgumentParser(description='BT JSON to CPP converted')
    parser.add_argument('-j','--json',     dest='json',     required=True, default=None, help='JSON input file')
    parser.add_argument('-t','--template', dest='template', required=True, default=None, help='Make Template Input File')
    parser.add_argument('-o','--output',   dest='output',   required=True, default=None, help='JSON input file')
    return parser.parse_args()

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

if __name__ == '__main__':
    args = parse_args()

    try:
        with open(args.json) as json_file:
            json_data = json.load(json_file)
    except Exception as exp:
        eprint("Unable to load JSON file '%s'" % args.json)
        eprint(exp)
        sys.exit(2)

    try:
        template = Template(filename=args.template)
    except Exception as exp:
        eprint("Unable to open MAKO template '%s'" % args.template )
        eprint(exp)
        sys.exit(3)

    try:
        with open(args.output, 'w') as output_file:
           output_file.write(template.render(json=json_data))
    except Exception as exp:
        eprint("Unable to create output file '%s'" % args.output)
        eprint(exp)
        sys.exit(4)

