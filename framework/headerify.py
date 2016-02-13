#!/usr/bin/env python3

import sys, os
sys.path.append(os.path.dirname(__file__))

import re
import traceback

class Content:
  """ Content of a header file. """

  def __init__(self, lines):
    """
    Constructor of Content.

    @type  lines: [str]
    @param lines: content of the file.
    """
    self.lines = lines

  def epure(self):
    """ Remove all C/C++ comments, empty lines and trailing whitespace. """

    content = ""
    for line in self.lines:
      content += line

    # remove all /* */ comments
    start_pos = content.find("/*")
    while start_pos != -1:
      end_pos = content.find("*/", start_pos)
      content = content[:start_pos] + content[end_pos+2:]
      start_pos = content.find("/*")

    # remove all // comments
    start_pos = content.find("//")
    while start_pos != -1:
      end_pos = content.find("\n", start_pos)
      content = content[:start_pos] + content[end_pos:]
      start_pos = content.find("//")

    lines = []

    self.lines = content.split("\n")
    for line in self.lines.copy():
      line = line.rstrip()
      if line and not line.isspace():
        lines.append(line)

    self.lines = lines

class Prog:
  """ Main class of the program. """

  def __init__(self, folder, header):
    """
    Constructor of Prog.

    @type  folder: str
    @param folder: path to the folder containing the header files.

    @type  header: str
    @param header: path to the end file.
    """
    self.files_included = []
    self.folder_path = folder

    # create dist dir if necessary
    os.makedirs(os.path.dirname(os.path.abspath(header)), exist_ok=True)
    self.header = open(header, "w+")


    self.includeRegex = re.compile(r'#include[ \t]+"(?P<path>[a-zA-Z\/\.]+)"')
    self.commentRegex = re.compile(r'\/\*.*\*\/')

  def clean(self):
    """ Closes the file. """
    self.header.close()

  def endswith(self, pattern, extensions):
    """
    Checks if a string ends with at list one pattern of a list.

    @type  pattern: str
    @param pattern: the string whose end to check.

    @type  extensions: [str]
    @param extensions: list of patterns to check.

    @rtype: boolean
    @return: True if at least one pattern fits.
    """
    for extension in extensions:
      if pattern.endswith(extension): return True
    return False

  def find_first_unincluded_file(self):
    """
    Finds the first non-included header file in the header file folder.

    @rtype: str|None
    @return: the path to the file, or None if no file found.
    """
    extensions = [".hh", ".hpp", ".h"]

    for root, dirs, files in os.walk(self.folder_path):
      for file in files:
        if os.path.join(root, file) not in self.files_included and self.endswith(file, extensions):
          return os.path.join(root, file)

    return None

  def insert_file(self, file):
    """
    Inserts the given file in the final header file.
    When finding a relative include (i.e., #include "header.hh"),
    inserts the file instead (and recursively inserts its relative includes).

    If the file was already included by another header file, does nothing.

    @type  file: str
    @param file: path to the file to insert.
    """
    if file in self.files_included:
      return

    self.files_included.append(file)

    with open(file, "r") as f:
      content = Content(f.readlines())

    content.epure()

    for line in content.lines:
      if line and not line.isspace():
        try:
          inc = self.includeRegex.search(line)
          if inc is not None:
            inc = inc.group("path")
        except IndexError:
          inc = None
          pass

        if inc is None:
          if not line.endswith("\n"):
            line += "\n"
          try:
            self.header.write(line)
          except:
            traceback.print_exc()
        else:
          self.insert_file(os.path.join(self.folder_path, inc))

    self.header.write("\n")

  def headerify(self):
    """ Recursively inserts all headers and their relative includes. """
    file = self.find_first_unincluded_file()
    if file:
      self.insert_file(file)
      return self.headerify()

    return 0

if __name__ == "__main__":
  av = sys.argv
  ac = len(av)

  prog = Prog(av[1] if ac >= 2 else "./include", av[2] if ac >= 3 else "../dist/cppframework.hh")

  try:
    retCode = prog.headerify()
  except:
    traceback.print_exc()
    retCode = 1

  prog.clean()
  sys.exit(retCode)
