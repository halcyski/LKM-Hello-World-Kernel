## LKM Info Preface

### In order to develop kernels with linux there are a few key concepts that one must know first.

### User Space vs. Kernel Space
- Hardware-enforced privilege separation. 
- The CPU has different modes of operation (rings)
	- **User space** (Ring 3)
		- Restricted environment where all your applications run
		- Code here cannot directly access hardware or critical system memory. 
	- **Kernel** (Ring 0)
		- Privileged domain where the kernel itself runs.
		- Unrestricted access to all hardware and memory. 
	- A crash in user-space application is contained, while a crash in a kernel space brings down the entire system (kernel panic)

### System Call (syscall) interface
- Since user space cannot directly perform privileged operations, it must ask the OS/kernel to do them. This is done through system calls.
- When a program wants to open a file, it calls a C library function like ``open()``
- This library function wraps the necessary assembly code (syscall on x86-64) to trap into the kernel. This trap switches the CPU from user mode to kernel mode and transfer the execution to a specific entry point in the kernel's syscall table. The kernel validates the requestion, performs the requested action, and then returns from the trap switching back to CPU back to user mode.

### Loadable Kernel Modules (LMKs)
- Linux has a monolithic kernel, meaning that the core OS, device drivers, and filesystems all operate within the same privileged address space.
- To avoid recompiling this massive kernel every time you want to add or modify a driver, Linux uses Loadable Kernel Modules (LMKs).
- An LMK is a piece of object code (a ``.ko`` file) that can be dynamically linked into and unlinked from the running kernel at any time.


### An LKM has a simple and well-defined structure. It requires an entry and exit point.
- Entry point (``init_module``)
	- This function is executed when your module is loaded into the kernel with the ``insmod`` command. Its job is to allocate/register any resources that this module needs (like creating a procfs entry or setting up a Kprobe). If it returns 0, the load was successful, else it failed.
- Exit point (``cleanup_module``)
	- This function is executed when your module is unloaded with the ``rmmod`` command. It must undo everything that the entry point did (similar how stack cleanup works?) releasing all resources it aquired.
	- ``printk``
		- You cannot use ``printf`` in the kernel, as it relies on user-space libraries. 
		- The kernel equivalent of ``printf`` is ``printk``
		- This function prints messages to the kernel's internal ring buffer which can viewed from user space with the ``dmesg`` command. 
  

Required includes for Linux Modules:
```c
#include <linux/init.h> // needed for the __init and __exit macros 
#include <linux/module.h> // needed for all modules 
#include <linux/kernel.h> // needed for KERN_INFO and printk
```

A module license is required to access the full range of kernel symbols:
```c
MODULE_LICENSE("GPL");
```

A basic entry point can be created with the ``__init`` macro: 
```c
static int __init hello_init(void) {
	printk(KERN_INFO "LKM: Hello kernel!");
	return 0; // successful load
}
```

A basic exit point can be created with the ``__exit`` macro:
```c 
static void __exit hello_exit(void) {
	printk(KERN_INFO "LKM: Goodbye kernel!");
}
```

```c 
// Register entry and exit points with kernel
module_init(hello_init);
module_exit(hello_exit);
```


A standard Makefile for building external LKMs:
```Makefile
KERNEL_DIR = /home/user/WSL2-Linux-Kernel # specify where the kernel is, replace user with your username

obj-m += my_module.o

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
```

## In order to compile and run this kernel:

    1. First install Windows subsystem for Linux (WSL):
```Powershell
wsl --install
```

    2. Then, follow this guide to install the correct WSL2 kernel headers:
https://learn.microsoft.com/en-us/community/content/wsl-user-msft-kernel-v6


    3. Clone this repo into a new project folder. 

    4. Execute the command in WSL:
```Bash
make clean && make
``` 
    
    5. The following commands load the module, check its output, and unload it.
```Bash

# Load the module into the kernel
sudo insmod my_module.ko

# Check that the module is loaded
lsmod | grep my_module

# View the "Hello" message in the kernel log
dmesg | tail

# Unload the module
sudo rmmod my_module

# View the "Goodbye" message in the kernel log
dmesg | tail
```
