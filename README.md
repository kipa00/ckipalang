ckipalang
========
The standard kipalang, ported from pykipalang.

### How to use
To build a ckipalang toolchain:

```bash
# To make a kipalang debugger
g++ -DDEBUG_MODE -std=c++0x main.cpp -o kipadb

# To make a kipalang compiler
g++ -DCOMPILE_MODE -std=c++0x main.cpp -o kipac

# To make a kipalang runtime
g++ -std=c++0x main.cpp -o kipa
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
