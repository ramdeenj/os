#!/usr/bin/env python

import asyncio
import sys
import asyncio.subprocess
import time
import os
import random
import re

TIMEOUT = 4

P=None

r1 = random.randrange(0,10000)
r2 = random.randrange(0,10000)
r3 = random.randrange(0,10000)

hello1 = f"""

extern void printf(const char* fmt, ... );
int main(){{
    printf("FOOBY_A {r1}\\n");
    char* p = (char*) 0x1234;
    *p = 42;
    printf("FOOBY_B {r2}\\n");
    while(1){{}}
}}
"""

hello2 = f"""

extern void printf(const char* fmt, ... );
int main(){{
    printf("FOOBY_C {r3}\\n");
    while(1){{}}
}}
"""


#os.environ["FOOL_NOW"] = "2023-Sep-08 01:02:04"
expectedDate = os.getenv("FOOL_NOW",None)
deletedFname=None
names=[]

async def getLine(timeLeft):
    line = ""

    while True:

        if timeLeft <= 0:
            if len(line) == 0:
                return None
            else:
                return line

        try:
            c = await asyncio.wait_for(P.stdout.read(1), timeLeft)
        except TimeoutError:
            if len(line) == 0:
                return None
            else:
                return line

        if len(c) == 0:
            return line

        c = c.decode(errors="replace")
        sys.stdout.write(c)
        sys.stdout.flush()

        line += c

        if c == "\n":
            return line

async def terminate():
    if P and P.returncode == None:
        P.stdin.write(b"`"); time.sleep(0.1); P.stdin.write(b"cquit\n")
        await asyncio.wait_for(P.wait(), 1)
        if P.returncode == None:
            P.terminate()
            await asyncio.wait_for(P.wait(), 1)
            if P.returncode == None:
                P.kill()
                await asyncio.wait_for(P.wait())

async def qprompt():
    lookingFor = "\n(qemu)"
    tmp = [""] * len(lookingFor)
    while True:
        c = await P.stdout.read(1)
        c=c.decode(errors="replace")
        tmp.pop(0)
        tmp.append(c)
        if "".join(tmp) == lookingFor:
            return



async def screencap(filename):
    global P
    tf = tempfile.NamedTemporaryFile(delete=False)
    tf.close()
    NAME = tf.name.replace("/","//")

    # ~ NAME = "debug"
    try:
        P.stdin.write(b"`"); time.sleep(0.1); P.stdin.write(b"c")
        await qprompt()
        P.stdin.write(f"screendump {NAME}\n".encode())
        await qprompt()
        P.stdin.write(b"`"); time.sleep(0.1); P.stdin.write(b"c")
        w,h,pix = bmputils.loadppm(tf.name)
        bmputils.savebmp(filename,w,h,pix,True)
    finally:
        os.unlink(tf.name)


async def doIt():
    global P
    P = await asyncio.create_subprocess_exec(
                sys.executable, "make.py",
                stdin=asyncio.subprocess.PIPE,
                stdout=asyncio.subprocess.PIPE,
                limit=65536
    )

    deadline = time.time() + TIMEOUT

    data = []

    while True:
        timeLeft = deadline - time.time()
        line = await getLine(timeLeft)
        if line == None:
            break
        line=line.strip()
        data.append(line)

    await terminate()

    return data


async def main():
    global P

    with open("user/hello.c","rb") as fp:
        helloc = fp.read()

    try:
        with open("user/hello.c","w") as fp:
            fp.write(hello1)
        data = await doIt()
        data = "\n".join(data)
        if f"FOOBY_A {r1}" in data and f"FOOBY_B {r2}" not in data:
            print("First test OK: Page fault occurred")
        else:
            print("First test failed: Page fault did not occur")
            return

        with open("user/hello.c","w") as fp:
            fp.write(hello2)
        data = await doIt()
        data = "\n".join(data)
        if f"FOOBY_C {r3}" in data:
            print("Second test OK: Page fault did not occur")
        else:
            print("Second test failed: Program did not execute")
            return

        print("All OK!")

    finally:
        with open("user/hello.c","wb") as fp:
            fp.write(helloc)
        await terminate()


asyncio.run(main())
