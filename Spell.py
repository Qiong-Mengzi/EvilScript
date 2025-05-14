import argparse
import glob
import os

# gcc -fdiagnostics-color=always -O3 test/test_chaos_a.c -o test/test_chaos_a.exe -I include

# -------- ARGS PARSE --------
parser = argparse.ArgumentParser(description="EvilScript Library Builder")


# -------- BUILD PROJECT --------
CC = "gcc"
OPTIM = "-g"
INCLUDING: list[str] = ["include"]
LIBPATH: list[str] = []
LIBLINK: list[str] = []
BUILDDIR = "bin"

incstr = [f'-I {_}' for _ in INCLUDING]

for fn in glob.glob("**/*.c", root_dir="src/", recursive=True):
    os.system(
        f'{CC} {OPTIM} -c {os.path.join("src/", fn)} -o {os.path.join(BUILDDIR, os.path.splitext(fn)[0] + ".o")} {" ".join(incstr)} -Wall'
    )

os.system(f'ar rcs {os.path.join(BUILDDIR, "libevilrt.a")} {os.path.join(BUILDDIR, "*.o")}')
