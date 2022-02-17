from pprint import pprint 
import subprocess as subproc

def load_file(filename):
    with open(filename, "r") as f:
        return f.readlines()

contents = load_file("tests/regex.txt")
contents = [x.split(' ') for x in contents]
contents = [[x[0], x[1], x[2].rsplit()[0]] for x in contents]
test_args = [[x[0], x[1]] for x in contents]
expected = [int(x[2]) for x in contents]

for idx, (arg0, arg1) in enumerate(test_args):
  capture = subproc.run(['./play', arg0, arg1], capture_output=True)
  got = capture.stdout

  result = got[0] - 48
  if expected[idx] == result:
    print("[PASSED] ", end='')
  else:
    print("[FAILED] ", end='')

  print(arg0, arg1)


