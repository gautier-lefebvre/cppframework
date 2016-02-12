#!/usr/bin/env python3

import sys, os
sys.path.append(os.path.dirname(__file__))

import re
import traceback

class Prog:
  def __init__(self, folder, header):
    self.files_included = []
    self.folder_path = folder
    self.header = open(header, "w")
    self.regex = re.compile(r'#include[ \t]+"(?P<path>[a-zA-Z\/\.]+)"')

  def clean(self):
    self.header.close()

  def endswith(self, pattern, extensions):
    for extension in extensions:
      if pattern.endswith(extension): return True
    return False

  def find_first_unincluded_file(self):
    extensions = [".hh", ".hpp", ".h"]

    for root, dirs, files in os.walk(self.folder_path):
      for file in files:
        if os.path.join(root, file) not in self.files_included and self.endswith(file, extensions):
          return os.path.join(root, file)

    return None

  def insert_file(self, file):
    if file in self.files_included:
      return

    print("Including {0}".format(file))

    self.files_included.append(file)

    with open(file, "r") as f:
      for line in f.readlines():
        try:
          inc = self.regex.search(line)
          if inc is not None:
            inc = inc.group("path")
        except IndexError:
          inc = None
          pass

        if inc is None:
          self.header.write(line)
        else:
          self.insert_file(os.path.join(self.folder_path, inc))

      self.header.write("\n")

  def headerify(self):
    file = self.find_first_unincluded_file()
    if file:
      self.insert_file(file)
      return self.headerify()

    return 0

if __name__ == "__main__":
  av = sys.argv
  ac = len(av)

  prog = Prog(av[1] if ac >= 2 else "./include", av[2] if ac >= 3 else "./dist/cppframework.hh")

  try:
    os._exit(prog.headerify())
  except:
    traceback.print_exc()
    sys.exit(1)
  finally:
    prog.clean()
