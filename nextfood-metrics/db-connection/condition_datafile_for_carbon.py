#!/usr/bin/python

import sys
import re
import pytz
import dateutil.parser as dp

def main():
    if len(sys.argv) < 3:
        print "Usage: script.py  <input file> <output file>"
        sys.exit(1)


    with open(sys.argv[1],'r') as infile:
        with open(sys.argv[2],'w') as outfile: 
            for line in infile:
                line = line.rstrip()
                if not line: continue
                m = re.search('(.*?) \((.*?)\/(.*?)\) \[(.*?)\] (.+)', line)
                if m:
                    parsed_t = dp.parse(m.group(1)).astimezone(pytz.utc)
                    print >> outfile, "{}.{}.{} {} {}".format(m.group(2),m.group(3),m.group(4), m.group(5), parsed_t.strftime('%s'))
                else:
                    m = re.search('(.*?) \((.*?)\/(.*?)\) \[(.*?)\]', line)
                    if m:
                        parsed_t = dp.parse(m.group(1)).astimezone(pytz.utc)
                        print >> outfile, "{}.{}.{} 1 {}".format(m.group(2),m.group(3),m.group(4), parsed_t.strftime('%s'))


if __name__ == "__main__":
    main()


