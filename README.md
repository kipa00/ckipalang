ckipalang
========
The standard kipalang, ported from pykipalang.

### How to use
To build a ckipalang toolchain:

```bash
# To make a kipalang compiler and runtime
make

# To clean all files
make clean
```

To run a helloworld:

```bash
# Helloworld source code
tee hello.kl <<END
putch "Hello, world!
"
END

# Compile source code
./kipac hello.kl

# Run it
./kipa hello.klb
```
