import subprocess as subproc

def load_file(filename):
    with open(filename, "r") as f:
        return f.readlines()

contents = load_file("tests/regex.txt")
contents = [x.rsplit() for x in contents]

for test_exprs in contents:
    print(f"./play {test_exprs[0]}")
    capture = subproc.run(['./play', test_exprs[0]], capture_output=True)
    got = str(capture.stderr)
    if got != "b''":
      print("===FAILED===")
      print(got)
    else:
      print("===PASSED===")
